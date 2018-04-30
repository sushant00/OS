#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

char data[1024]={0};
int rp=0;
int wp=0;
int flag=0;
char key[16]={0};
char buff[16]={0};
char decrpyt[16]={0}; 
ssize_t my_read (struct file *myfile, char __user *buffer, size_t length, loff_t *offset)
{
	printk(KERN_ALERT "Inside the %s function",__FUNCTION__);
	if(rp>=wp)
	{	
		printk(KERN_ALERT "Nothing to read");
		return -1;
	}
	if(copy_to_user(buffer,(data+rp), length)==0)	
	{
		rp+=length;
		return length;
	}

}
ssize_t my_write (struct file *myfile, const char __user *buffer, size_t length, loff_t *offset)
{
	printk(KERN_ALERT "Inside the %s function",__FUNCTION__);
	if(flag)
	{	
		int temp=copy_from_user(buff,buffer,length);
		int i;
		for(i=0;i<16;i++)
		{
			if(buff[i]=='\0')
			{
				data[wp]='\0';
				break;
			}
			else
			{
				data[wp]=key[i] ^ buff[i];
			}	
			key[i]=data[wp];
			wp++;
		}
		return length;
	}
	else
	{
		int temp=copy_from_user(key,buffer,length);
		flag=1;
		return length;
	}	
}
int my_open (struct inode *myinode, struct file *myfile)
{
	printk(KERN_ALERT "Inside the %s function",__FUNCTION__);
	return 0;
}
int my_close (struct inode *myinode, struct file *myfile)
{
	printk(KERN_ALERT "Inside the %s function",__FUNCTION__);
	rp=0;
	wp=0;
	return 0;
}

struct file_operations my_file_operations = {
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_close,
};

int my_init(void)
{
	printk(KERN_ALERT "Inside the %s function",__FUNCTION__);
	register_chrdev(777,"Decryptor Device",&my_file_operations);
	return 0;
}
void my_exit(void)
{
	printk(KERN_ALERT "Inside the %s function",__FUNCTION__);
	unregister_chrdev(777,"Decryptor Device");
}

module_init(my_init);
module_exit(my_exit);