# Vexx

qemu 逃逸题

## Solution

这题其实非常简单
主要思路来自 CVE-2019-6778的利用以及之前强网杯等一些比赛出过的题目
通过修改qemu timer的信息进行system("cat flag")

考察点是如何与qemu设备进行交互（mmio/ioport）以及如何通过qemu timer进行劫持控制流（timer真的是个好东西

大致逆向一下就能发现漏洞点在vexx_cmb_write
在`vexx->memorymode & 1 == true`的时候，会导致VexxState结构体上一个req_buf溢出

溢出便能覆盖跟在后面的`QEMUTimer dma_timer`

结合到整个设备，利用思路便是
1. 通过ioport对memorymode进行设置
2. 通过mmio针对vexx_cmb_ops进行泄漏及溢出
3. 通过mmio针对vexx_mmio_ops调用dma_rw触发dma_timer

因为环境问题，该题不能调用`system("/bin/sh")`，但是cat flag是可以的

## 反省

大家可能都注意到在比赛过程中，f61d的师傅在题目仅仅放出10分钟的情况下就一血了
这里是出题人的失误（我太菜了呜呜呜

由于出题人经验不足，在qemu启动脚本没有加上`-monitor /dev/null`
导致是可以直接进入qemu monitor的
在进入qemu monitor后，能操作的空间就太多了，包括pci设备热插拔、命令执行、监控虚拟机状态等

由于这个失误，导致直接就被cat flag了

这个配置在我们发现后也一直没有修，因为这题已经被拿一血了，此时修掉对其他队伍不公平，这也是体现别人知识水平比我们强的地方。
便任由这个问题存在了整场比赛

但是从大家的做题情况看出，大家都没有发现或者是不知道这个问题。

对题目配置失误给出不好的体验，在这里给大家道歉了。（我真菜555

