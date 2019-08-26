#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <linux/kvm.h>

#include "fragment.h"

#ifdef KVM_DEBUG 
	#define LOG(...) printf(__VA_ARGS__)
#else
	#define LOG(...)
#endif

extern struct fragment fragment_list[18];
extern const unsigned char guest[], guest_end[];

/* CR0 bits */
#define CR0_PE 1u
#define CR0_MP (1U << 1)
#define CR0_EM (1U << 2)
#define CR0_TS (1U << 3)
#define CR0_ET (1U << 4)
#define CR0_NE (1U << 5)
#define CR0_WP (1U << 16)
#define CR0_AM (1U << 18)
#define CR0_NW (1U << 29)
#define CR0_CD (1U << 30)
#define CR0_PG (1U << 31)

/* CR4 bits */
#define CR4_VME 1
#define CR4_PVI (1U << 1)
#define CR4_TSD (1U << 2)
#define CR4_DE (1U << 3)
#define CR4_PSE (1U << 4)
#define CR4_PAE (1U << 5)
#define CR4_MCE (1U << 6)
#define CR4_PGE (1U << 7)
#define CR4_PCE (1U << 8)
#define CR4_OSFXSR (1U << 8)
#define CR4_OSXMMEXCPT (1U << 10)
#define CR4_UMIP (1U << 11)
#define CR4_VMXE (1U << 13)
#define CR4_SMXE (1U << 14)
#define CR4_FSGSBASE (1U << 16)
#define CR4_PCIDE (1U << 17)
#define CR4_OSXSAVE (1U << 18)
#define CR4_SMEP (1U << 20)
#define CR4_SMAP (1U << 21)

#define EFER_SCE 1
#define EFER_LME (1U << 8)
#define EFER_LMA (1U << 10)
#define EFER_NXE (1U << 11)

/* 32-bit page directory entry bits */
#define PDE32_PRESENT 1
#define PDE32_RW (1U << 1)
#define PDE32_USER (1U << 2)
#define PDE32_PS (1U << 7)

/* 64-bit page * entry bits */
#define PDE64_PRESENT 1
#define PDE64_RW (1U << 1)
#define PDE64_USER (1U << 2)
#define PDE64_ACCESSED (1U << 5)
#define PDE64_DIRTY (1U << 6)
#define PDE64_PS (1U << 7)
#define PDE64_G (1U << 8)


#define MEM_SIZE 0x200000

struct VM{
    int sys_fd;
    int vm_fd;
    int vcpu_fd;
    char *mem;
    struct kvm_run *kvm_run;
};

void vm_init(struct VM *vm, size_t mem_size)
{
	int api_ver;
	struct kvm_userspace_memory_region memreg;

	vm->sys_fd = open("/dev/kvm", O_RDWR);
	if (vm->sys_fd < 0) {
		perror("open /dev/kvm");
		exit(1);
	}

	api_ver = ioctl(vm->sys_fd, KVM_GET_API_VERSION, 0);
	if (api_ver < 0) {
		perror("KVM_GET_API_VERSION");
		exit(1);
	}

	if (api_ver != KVM_API_VERSION) {
		fprintf(stderr, "Got KVM api version %d, expected %d\n",
			api_ver, KVM_API_VERSION);
		exit(1);
	}

	vm->vm_fd = ioctl(vm->sys_fd, KVM_CREATE_VM, 0);
	if (vm->vm_fd < 0) {
		perror("KVM_CREATE_VM");
		exit(1);
	}

	if (ioctl(vm->vm_fd, KVM_SET_TSS_ADDR, 0xfffbd000) < 0) {
                perror("KVM_SET_TSS_ADDR");
		exit(1);
	}

	vm->mem = mmap(0, mem_size, PROT_READ | PROT_WRITE,
		   MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (vm->mem == MAP_FAILED) {
		perror("mmap mem");
		exit(1);
	}

	madvise(vm->mem, mem_size, MADV_MERGEABLE);

	memreg.slot = 0;
	memreg.flags = 0;
	memreg.guest_phys_addr = 0;
	memreg.memory_size = mem_size;
	memreg.userspace_addr = (unsigned long)vm->mem;
        if (ioctl(vm->vm_fd, KVM_SET_USER_MEMORY_REGION, &memreg) < 0) {
		perror("KVM_SET_USER_MEMORY_REGION");
                exit(1);
	}
}

void vcpu_init(struct VM *vm)
{
	int vcpu_mmap_size;

	vm->vcpu_fd = ioctl(vm->vm_fd, KVM_CREATE_VCPU, 0);
	if (vm->vcpu_fd < 0) {
		perror("KVM_CREATE_VCPU");
                exit(1);
	}

	vcpu_mmap_size = ioctl(vm->sys_fd, KVM_GET_VCPU_MMAP_SIZE, 0);
	if (vcpu_mmap_size <= 0) {
		perror("KVM_GET_VCPU_MMAP_SIZE");
                exit(1);
	}

	vm->kvm_run = mmap(NULL, vcpu_mmap_size, PROT_READ | PROT_WRITE,
			     MAP_SHARED, vm->vcpu_fd, 0);
	if (vm->kvm_run == MAP_FAILED) {
		perror("mmap kvm_run");
		exit(1);
	}

	struct kvm_guest_debug debug = {
		.control = KVM_GUESTDBG_ENABLE | KVM_GUESTDBG_SINGLESTEP,
	};
	if(ioctl(vm->vcpu_fd, KVM_SET_GUEST_DEBUG, &debug) < 0){
		perror("KVM_SET_GUEST_DEBUG");
		exit(1);
	}

}

int run_vm(struct VM *vm, char *input)
{
	struct kvm_regs regs;
	struct fragment frag;
	int offset = 0;
	uint32_t jmp_addr;
	int ret = 0;

	for (int i=0; i<18;) {

		frag = fragment_list[i];

		if (ioctl(vm->vcpu_fd, KVM_RUN, 0) < 0) {
			perror("KVM_RUN");
			exit(1);
		}

		if ((ret = ioctl(vm->vcpu_fd, KVM_GET_REGS, &regs)) < 0){
			perror("KVM_GET_REGS");
			exit(1);
		}
		LOG("rip = %d\n", regs.rip);

		char data[10];
		memset(data, 0, sizeof(data));
		memcpy(data, &input[offset], frag.inputlen);

		switch (vm->kvm_run->exit_reason) {
		case KVM_EXIT_HLT:
			LOG("HLT\n");
			ret = ioctl(vm->vcpu_fd, KVM_GET_REGS, &regs);
			if(ret < 0) exit(1);
			ret = regs.rax;
			if(ret != 1) {
				LOG("wrong\n");
				return 0; // check failed, return 
			}
			LOG("correct\n");
			i++;
			offset += frag.inputlen;
			// if(i == 18) return;
			memset(&regs, 0, sizeof(regs));
			regs.rsp = 2 << 20;
			regs.rflags = 2;
			regs.rip = 0;
			ret = ioctl(vm->vcpu_fd, KVM_SET_REGS, &regs);
			if(ret < 0) exit(1);
			memcpy(vm->mem, guest, guest_end-guest);
			break;

		case KVM_EXIT_IO:

			if (vm->kvm_run->io.direction == KVM_EXIT_IO_OUT
			    && vm->kvm_run->io.port == 0x78) {
				char *p = (char *)vm->kvm_run;
				fwrite(p + vm->kvm_run->io.data_offset,
				       vm->kvm_run->io.size, 1, stdout);
				fflush(stdout);
			} else if(vm->kvm_run->io.direction == KVM_EXIT_IO_IN
				&& vm->kvm_run->io.port == 0x78) {
				char *p = (char *)vm->kvm_run;
				fread(p + vm->kvm_run->io.data_offset, vm->kvm_run->io.size, 1, stdin);
			}
			break;
		case KVM_EXIT_DEBUG:
			LOG("KVM_EXIT_DEBUG\n");
			ret = ioctl(vm->vcpu_fd, KVM_GET_REGS, &regs);
			if(ret < 0) exit(1);
			if(vm->mem[regs.rip]==0xf && vm->mem[regs.rip+1]==0x1 && vm->mem[regs.rip+2]==(char)0xc1) {
				LOG("VMCALL\n");
				switch (frag.func){
				case CRC32:
					LOG("CRC32\n");
					jmp_addr = 0x3;
					break;
				case FIBNACCI:
					LOG("FIBNACCI\n");
					jmp_addr =  0x17;
					break;
				case BASE64:
					LOG("BASE64\n");
					jmp_addr = 0x2b; 
					break;
				case XORSTRING:
					LOG("XORSTRING\n");
					jmp_addr = 0x3f; 
					break;
				default:
					jmp_addr = 0;
					break;
				}			

				memset(vm->mem + 0x1000, 0, 10);
				memset(vm->mem + 0x1100, 0, 64);
				memcpy(vm->mem + 0x1000, data, 10);
				memcpy(vm->mem + 0x1100, frag.checkdata, 64);

				regs.rip = jmp_addr;

				ret = ioctl(vm->vcpu_fd, KVM_SET_REGS, &regs);
				if(ret < 0) exit(1);
			}

			break;

		case KVM_EXIT_MMIO:
			printf("MMIO\n");
			break;

		case KVM_EXIT_INTERNAL_ERROR:
			ret = ioctl(vm->vcpu_fd, KVM_GET_REGS, &regs);
			if(ret < 0){
				perror("KVM_EXIT_INTERNAL_ERROR");
				exit(1);
			}
			printf("suberror: %d\n", vm->kvm_run->internal.suberror);
			exit(1);

		default:
			fprintf(stderr,	"Got exit_reason %d\n", vm->kvm_run->exit_reason);
			exit(1);
		}
	}

	return 1;
}

static void setup_paged_32bit_mode(struct VM *vm, struct kvm_sregs *sregs)
{
	uint32_t pd_addr = 0x2000;
	uint32_t *pd = (void *)(vm->mem + pd_addr);

	/* A single 4MB page to cover the memory region */
	pd[0] = PDE32_PRESENT | PDE32_RW | PDE32_USER | PDE32_PS;
	/* Other PDEs are left zeroed, meaning not present. */

	sregs->cr3 = pd_addr;
	sregs->cr4 = CR4_PSE;
	sregs->cr0
		= CR0_PE | CR0_MP | CR0_ET | CR0_NE | CR0_WP | CR0_AM | CR0_PG;
	sregs->efer = 0;
}

static void setup_protected_mode(struct kvm_sregs *sregs)
{
	struct kvm_segment seg = {
		.base = 0,
		.limit = 0xffffffff,
		.selector = 1 << 3,
		.present = 1,
		.type = 11, /* Code: execute, read, accessed */
		.dpl = 0,
		.db = 1,
		.s = 1, /* Code/data */
		.l = 0,
		.g = 1, /* 4KB granularity */
	};

	sregs->cr0 |= CR0_PE; /* enter protected mode */

	sregs->cs = seg;

	seg.type = 3; /* Data: read/write, accessed */
	seg.selector = 2 << 3;
	sregs->ds = sregs->es = sregs->fs = sregs->gs = sregs->ss = seg;
}


int run_protected_mode(struct VM *vm, char *input)
{
	struct kvm_sregs sregs;
	struct kvm_regs regs;


	if (ioctl(vm->vcpu_fd, KVM_GET_SREGS, &sregs) < 0) {
		perror("KVM_GET_SREGS");
		exit(1);
	}

	setup_protected_mode(&sregs);
	setup_paged_32bit_mode(vm, &sregs);

	if (ioctl(vm->vcpu_fd, KVM_SET_SREGS, &sregs) < 0) {
		perror("KVM_SET_SREGS");
		exit(1);
	}

	memset(&regs, 0, sizeof(regs));
	/* Clear all FLAGS bits, except bit 1 which is always set. */
	regs.rsp = 2 << 20;
	regs.rflags = 2;
	regs.rip = 0;

	if (ioctl(vm->vcpu_fd, KVM_SET_REGS, &regs) < 0) {
		perror("KVM_SET_REGS");
		exit(1);
	}

	memcpy(vm->mem, guest, guest_end-guest);
	return run_vm(vm, input);
}

void changeTable(struct fragment *frag_l){
	for(int i=0;i<18;i++){
		// int pos1 = ((unsigned)rand() * 100) % 18;
		int pos1 = i;
		int pos2 = ((unsigned)rand() * 100) % 18;
		struct fragment tmp = frag_l[pos1];
		frag_l[pos1] = frag_l[pos2];
		frag_l[pos2] = tmp;
	}
}

void xorflag(char *flag, char *input){
	for(int i=0; i<73; i++){
		flag[i] ^= input[i];
	}
}

int main(){
    
	setbuf(stdin, 0);
	setbuf(stdout, 0);

    struct VM vm;
    
    printf("Try to figure out what the author thinking...\n");

    vm_init(&vm, MEM_SIZE);
    vcpu_init(&vm);

	srand(42);

	char encrypt_flag[] = {85, 17, 7, 70, 17, 121, 92, 71, 98, 7, 37, 38, 36, 50, 12, 68, 21, 21, 80, 104, 116, 29, 126, 119, 124, 26, 84, 8, 122, 82, 58, 119, 46, 123, 84, 118, 95, 49, 117, 56, 103, 64, 71, 90, 116, 90, 88, 112, 29, 85, 115, 96, 87, 52, 1, 33, 82, 77, 49, 56, 114, 9, 61, 73, 88, 116, 80, 118, 81, 13, 27, 69, 68};

	char input[80] = {0};

	for (int i=0; i<233; i++){
		printf("Try to input the correct ans(%d/233): \n", i+1);
		fgets(input, 80, stdin);
		// printf(input);
		if (!run_protected_mode(&vm, input)){
			printf("Sorry...\n");
			return 0;
		}
		xorflag(encrypt_flag, input);
		changeTable(fragment_list);
	}

	printf("AHHHH, you know me!\n%s", encrypt_flag);

    return 0;
}