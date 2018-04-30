
obj-m := encdev.o
obj-m += decryptor.o
all: encdev.c decryptor.c
	make -C /lib/modules/$(shell uname -r)/build/	M=$(PWD) modules
install:
	insmod	encdev.ko
	insmod decryptor.ko
remove:
	rmmod encdev.ko
	rmmod decryptor.ko
clean:
	make -C /lib/modules/$(shell uname -r)/build/	M=$(PWD) clean
