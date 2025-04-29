obj-m += key_interrupt.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o user_app user_key_value.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
