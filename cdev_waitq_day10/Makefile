obj-m += simple_chardev.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc userread.c -o user_rd
	gcc userwrite.c -o user_wr

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f user_rd
	rm -f user_wr
