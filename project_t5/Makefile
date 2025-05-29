obj-m += my_driver.o
 
KDIR = /lib/modules/$(shell uname -r)/build
 
all:
	make -C $(KDIR)  M=$(shell pwd) modules
	gcc tcp_server.c -o server
 
clean:
	make -C $(KDIR)  M=$(shell pwd) clean
	rm -f server

