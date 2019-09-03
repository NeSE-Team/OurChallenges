obj-m = osok.o

#KBUILD_PATH_KASAN=/home/ww9210/kernels/4.16-rc7
#KBUILD_PATH_NOKASAN=/home/ww9210/kernels/4.16-rc7-nokasan
KBUILD_PATH_NOKASAN=//home/ww9210/kernel/5.2
#GCC=/home/ww9210/gcc
GCC=/usr/bin/gcc

#kasan:
	#make CC="$(GCC)/install/bin/gcc" -C ${KBUILD_PATH_KASAN} M=$(PWD) modules

nokasan: osok.c
	#make CC="$(GCC)/install/bin/gcc" -C ${KBUILD_PATH_NOKASAN} M=$(PWD) modules
	make CC="$(GCC)" -C ${KBUILD_PATH_NOKASAN} M=$(PWD) modules

clean:
	#make -C ${KBUILD_PATH_KASAN} M=$(PWD) clean
	#make -C ${KBUILD_PATH_NOKASAN} M=$(PWD) clean
	rm osok.ko
	rm Module.symvers
	rm modules.order
	rm *mod.*
	rm *.o
