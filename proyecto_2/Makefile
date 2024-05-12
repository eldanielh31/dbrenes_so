CC = x86_64-linux-gnu-gcc-12
obj-m = arduino_driver.o
KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules CC=x86_64-linux-gnu-gcc-12

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean

load:
	sudo insmod arduino_driver.ko

unload:
	sudo rmmod arduino_driver

check:
	lsmod | grep arduino_driver

lib:
	gcc lib.c -o lib
	
run_lib:
	sudo ./lib
