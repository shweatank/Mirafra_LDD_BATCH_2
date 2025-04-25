obj-m += string_rev.o


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o user_app kernel_Add.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f user_app
