CC = x86_64-linux-gnu-gcc-12
obj-m = driver.o
KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules CC=x86_64-linux-gnu-gcc-12

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
		