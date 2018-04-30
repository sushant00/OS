
obj-m = encdev.o
all: encdev.c
	make -C /lib/modules/$(uname -r)/build/	M=$(PWD) modules
	insmod	encdev.ko
clean:
	make -C /lib/modules/$(uname -r)/build/	M=$(PWD) clean
