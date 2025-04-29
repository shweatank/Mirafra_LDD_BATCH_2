obj-m := IOCTL_strrev.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f Ioctl_usrrev

user:
	gcc -o Ioctl_usrrev Ioctl_usrrev.c
		

