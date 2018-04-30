
obj-m = encdev.o
all: encdev.c
	make -C /lib/modules/$(shell uname -r)/build/	M=$(PWD) modules

install:
	insmod	encdev.ko
remove:
	rmmod encdev.ko
clean:
	make -C /lib/modules/$(shell uname -r)/build/	M=$(PWD) clean
