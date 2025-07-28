
obj-m +=ds1307.o

EXTRA_CFLAGS += -g
#ccflags-y += -g

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) CONFIG_DEBUG_INFO=y modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
