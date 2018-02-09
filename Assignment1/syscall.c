#include <linux/kernel.h>	//for sprintf, printk
#include <linux/fs.h>		//for open, write
#include <linux/types.h>	// size_t
#include <linux/string.h>	
SYSCALL_DEFINE2(sh_task_info, int, input_pid, char *, filename){
	struct pid *pid_struct;
	struct task_struct *task;
	pid_struct = find_get_pid(input_pid);
	if(pid_struct == NULL){
		return -ESRCH;
	}else{
		task = pid_task(pid_struct, PIDTYPE_PID);
		if(task == NULL){
			return -ESRCH;
		}

		int fd = open(filename, O_WRONLY);		//open the file, get filedescriptor
		if(fd == -1){							//error opening file
			if(errno == EACCES){
				printk(KERN_ERR "Access denied to file '%s' ", filename);
				return -EACCES;
			}else if(errno == ENOENT){
				printk(KERN_ERR "'%s' nonexistent directory", filename);
				return -ENOENT;
			}else{
				printk(KERN_ERR "error occurred, error no. %d\n", errno);
				return -errno;
			}
		}else{									// print to the file
			char buff[400];
			// format the string to be printed to file and to kernel log
			int len = sprintf(buff, "task_struct PID %d:\n\
				state %ld,\n\
				flags %d,\n\
				ptrace %d,\n\
				prio %d, static_prio %d, normal_prio %d,\n\
				rt_priority %d,\n\
				policy %d,\n\
				nr_cpus_allowed %d,\n\
				exit_state %d,\n\
				exit_code %d, exit_signal %d, \n\
				personality %d, \n\
				link_count %d, total_link_count %d,\n",
				input_pid,
				task->state, task->flags, task->ptrace, task->prio, 
				task->static_prio, task->normal_prio, task->rt_priority,
				task->policy, task->nr_cpus_allowed, task->exit_state,
				task->exit_code,task->exit_signal, task->personality,
				task->link_count, task->total_link_count);

			size_t count = len;
			// system call "write"
			int writeFile = write(fd, buff, count);
			close(fd);
			if(writeFile == -1){
				if(errno == EINTR){			// call interrupted before write by a signal
					printk(KERN_ERR "sh_task_info: write: call interrupted before write to file by a signal ");
					return -EINTR;
				}else{
					printk(KERN_ERR "sh_task_info: some error occurred while writing to %s ", filename);
					return -errno;
				}
			}else{
				printk(KERN_INFO "%s", buff);
			}
		}
	}
	return 0;
}