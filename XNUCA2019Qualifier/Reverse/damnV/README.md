# damnV

## solution

这题的考点其实是kvm逆向，其他一些算法其实都是点缀，所以也没有弄很难，目的是传达一个思想
**通过VMM trap可以修改vcpu的寄存器，从而更改vm的执行流**

当中主要通过一个结构体储存check fragment的信息，包括输入长度，check函数，checkdata这3个数据

在vm启动后，会通过`vmcall`触发vm_exit，最后根据当前check fragment的数据更改rip的值跳转到不同的check函数
最后再通过`hlt`再次触发vm_exit，读取check函数的返回结果

4种check函数分别为crc32、fibnacci、改table的base64、xorstring

mian函数上一共233轮check，每一次check结束对check fragment进行打乱，再次进行check

其实本题并不难，就是编译开了`-O2`看起来可能没那么方便


## 题外话

因为本题设计问题，通过vmm trap更改控制流的操作没有做到很突出
kvm的调试也是很大的一个问题，guest中的代码似乎没有什么好办法能调的
并且我在实现时也遇到了各种奇怪的问题，应该是因为自己对这方面了解还不够深入吧
真对太太太菜了
