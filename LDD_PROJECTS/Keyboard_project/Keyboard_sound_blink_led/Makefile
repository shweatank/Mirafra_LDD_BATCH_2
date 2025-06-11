obj-m += key_count_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc user.c -o user_app

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f user_app
