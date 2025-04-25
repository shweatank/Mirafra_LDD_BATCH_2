obj-m += char_string1.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

 	#gcc -o user user.c	

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	
	#rm -f user
