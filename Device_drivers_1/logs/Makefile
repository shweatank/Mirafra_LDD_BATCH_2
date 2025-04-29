obj-m := kernel_log.o
obj-m := logk.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f user_rev

user:
	gcc -o user_rev user_rev.c
		

