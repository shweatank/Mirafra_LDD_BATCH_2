# ~/ganga/pci/Makefile

obj-m += pci_e1000_driver.o

KDIR := /home/mirafra/linux       # Path to your custom kernel source
PWD  := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

install:
	make -C $(KDIR) M=$(PWD) modules_install INSTALL_MOD_PATH=$(HOME)/qemu-x86/initramfs

clean:
	make -C $(KDIR) M=$(PWD) clean

