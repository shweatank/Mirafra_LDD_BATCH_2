obj-m := oop.o
KDIR  ?= /lib/modules/$(shell uname -r)/build
all:
	$(MAKE) -C $(KDIR) M=$(PWD) EXTRA_CFLAGS="-g" modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

