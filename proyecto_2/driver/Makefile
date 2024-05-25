CC = x86_64-linux-gnu-gcc-12
obj-m = arduino_driver.o
KVERSION = $(shell uname -r)
KDIR = /lib/modules/$(KVERSION)/build

all:
	make -C $(KDIR) M=$(PWD) modules CC=$(CC)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f main

load:
	sudo insmod arduino_driver.ko

unload:
	sudo rmmod arduino_driver

unload_default:
	sudo rmmod cdc_acm

check:
	watch -n 2 lsusb -t

port:
	lsusb -v -d 2341:0043

kernel:
	sudo dmesg -w

comp:
	gcc -o send_message send_message.c arduino_lib.c
	
run:
	sudo ./send_message
