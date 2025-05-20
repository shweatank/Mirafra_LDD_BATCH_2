# obj-m += key_int.o         //without user space reading or file

#all:   
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

#clean:
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean



obj-m += project.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o project_user project_user.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f project_user

#obj-m := key_a.o
#KDIR := /lib/modules/$(shell uname -r)/build
#PWD := $(shell pwd)

#all:
#	$(MAKE) -C $(KDIR) M=$(PWD) modules

#clean:
#	$(MAKE) -C $(KDIR) M=$(PWD) clean
