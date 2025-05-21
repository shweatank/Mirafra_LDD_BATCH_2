obj-m += wait_queue_driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

user:
	gcc -o reader reader.c
	gcc -o writer writer.c