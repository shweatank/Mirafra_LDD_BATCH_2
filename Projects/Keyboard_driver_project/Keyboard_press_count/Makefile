obj-m += keyboard_count.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o user_app user_app.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f user_app

