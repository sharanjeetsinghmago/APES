KERNEL_DIR 	?= /lib/modules/$(shell uname -r)/build
PWD 		= $(shell pwd)

obj-m := kthread_fifo.o


default:
	make -C $(KERNEL_DIR) M=$(PWD) modules


clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean

