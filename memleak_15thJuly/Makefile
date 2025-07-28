KDIR := /home/mirafra/linux    # Path to your custom kernel source
PWD  := $(shell pwd)

obj-m += kmemleak_test.o

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

