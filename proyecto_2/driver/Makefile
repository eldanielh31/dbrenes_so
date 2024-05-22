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

check:
	lsusb
	lsusb -t

kernel:
	sudo dmesg

lib:
	gcc arduino_lib.c -o arduino_lib
	
run_lib:
	sudo ./arduino_lib
