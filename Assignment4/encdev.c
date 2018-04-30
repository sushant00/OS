#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

static int encdev_init(void);
static void encdev_exit(void);
static int encdev_open(struct inode *inodep, struct file *filep);
static ssize_t encdev_read(struct file *filep, char *buff, size_t len, loff_t *offset);
static ssize_t encdev_write(struct file *filep, const char *buff, size_t len, loff_t *offset);
static int encdev_close(struct inode *inodep, struct file *filep);





static int device_file_major = 0;
static const char device_name[] = "encdev";


static int firstWrite;
static char encMsg[512];
static int writePtr;
static int readPtr;
static char curKey[16];
static char buffer[16];

static struct file_operations encdev_fops = {
	.owner = THIS_MODULE,
	.open = encdev_open,
	.read = encdev_read,
	.write = encdev_write,
	.release = encdev_close,
};

static int encdev_init(void){
	int r;
	printk("initialise encdev\n");
	printk("encdev: register_chrdevstering\n");
	r = register_chrdev(0, device_name, &encdev_fops);	//allocate major
	if(r < 0){	//error
		printk("encdev: error registering, errno %d\n", r);
		return r;
	}else{
		device_file_major = r;
		printk("encdev: registered, major_num %d\n", r);
	}
	firstWrite = 0;		//first read not done
	return 0;
}

static void encdev_exit(void){
	printk("exiting my module\n");
	if(device_file_major != 0){
		unregister_chrdev(device_file_major, device_name);	//unregister encdev
		printk("encdev: unregistered\n");
	}
	return;
}


static int encdev_open(struct inode *inodep, struct file *filep){
	printk("encdev:	opened\n");
	return 0;
}


static ssize_t encdev_read(struct file *filep, char *buff, size_t len, loff_t *offset){
	int numBytes = 0;
	if(readPtr>=writePtr){
		printk("encdev: nothing to read\n");
		return 0;
	}
	if(numBytes = copy_to_user(buff, &encMsg[readPtr], 16)==0){
		printk("encdev: written msg %s\n", &encMsg[readPtr]);
	}else{
		printk("encdev: write unsuccessful, %d bytes not written\n", numBytes);
		return 0;
	}
	if(writePtr-readPtr < 16){
		numBytes = writePtr - readPtr;
	}else{
		numBytes = 16;
	}
	readPtr+=16;
	return numBytes;
}


static ssize_t encdev_write(struct file *filep, const char *buff, size_t len, loff_t *offset){
	int i = 0;
	int numBytes = 0;
	if(!firstWrite){
		if(numBytes = copy_from_user(curKey, buff, 16)==0){
			printk("encdev: read msg %s\n", curKey);
		}else{
			printk("encdev: read unsuccessful, %d bytes not written\n", numBytes);
			return 0;
		}
		firstWrite = 1;
		writePtr = 0;
		readPtr = 0;
		return 16;
	}else if(writePtr >= 256){
		printk("encdev: Msg buffer full\n");
		return -1;
	}else{//handle eof, errors
		if(numBytes = copy_from_user(buffer, buff, 16)==0){
			printk("encdev: read msg %s\n", buffer);
		}else{
			printk("encdev: read unsuccessful, %d bytes not written\n", numBytes);
			return 0;
		}	
		i = 0;
		for(i = 0; i<16; i++){
			if(buffer[i] == '\0'){
				encMsg[writePtr] = '\0';
				break;
			}else{
				encMsg[writePtr] = curKey[i] ^ buffer[i];
				printk("encdev: byte written %c xor %c = %c\n", curKey[i], buffer[i], encMsg[writePtr]);	
			}
			curKey[i] = encMsg[writePtr];
			writePtr++;
		}
		if(writePtr - (writePtr/16)*16 == 0){
			return 16;
		}else{
			return writePtr - (writePtr/16)*16;
		}
	}
}

static int encdev_close(struct inode *inodep, struct file *filep){
	printk("encdev:	closed\n");
	return 0;
}



module_init(encdev_init);
module_exit(encdev_exit);
