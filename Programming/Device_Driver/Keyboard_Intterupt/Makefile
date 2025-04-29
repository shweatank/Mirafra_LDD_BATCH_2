obj-m += main.o

all:
	# (this line must begin with a TAB, not spaces)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	# (this line must begin with a TAB, not spaces)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

