ifneq ($(KERNELRELEASE),)
	obj-m := sche_ctrl.o
    CFLAGS_sche_ctrl.o := -DDEBUG
else
	obj-m := sche_ctrl.o
	KERNELDIR ?= /usr/src/linux
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

endif


