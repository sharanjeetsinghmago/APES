#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/kthread.h>	
#include <linux/delay.h>
#include <linux/kfifo.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sharanjeet Singh Mago");
MODULE_DESCRIPTION("");
MODULE_ALIAS("kthread_fifo");

#define MY_KFIFO_NAME	mykfifo
#define MY_KFIFO_NAME_P	&mykfifo

/*Should be a power of 2 */
#define MY_KFIFO_SIZE	(1<<4)

static DEFINE_MUTEX(fifo_lock);

static int x = 0;
static int y = 0;

struct task_struct *task1;
struct task_struct *task2;

static DECLARE_KFIFO(MY_KFIFO_NAME,struct task_struct*,MY_KFIFO_SIZE);	

static unsigned long stimeInterval= 5;
module_param(stimeInterval, ulong, S_IRUGO | S_IWUSR);
	
int task1_callback(void *params)
{
	printk(KERN_INFO "From %s\n",__FUNCTION__);

	while(!kthread_should_stop())
	{		
		/* Lock the mutex*/
		if (mutex_lock_interruptible(&fifo_lock))
		{
			printk(KERN_ERR "Cannot get the lock\n");
			//return -1;
			return -ERESTARTSYS;
		}
		/* Push the data into kfifo*/
		if(0 == kfifo_put(MY_KFIFO_NAME_P, current))
			printk(KERN_INFO "KFIFO FULL\n");
		else
		{
			//printk(KERN_INFO "Process pushed id: %d\n",current->pid);
		}

		/* Unlock the mutex*/
		mutex_unlock(&fifo_lock);

		/* Signal the condition variable */

		x++;
		ssleep(stimeInterval);
	}

	printk(KERN_INFO "%s is terminated\n",__FUNCTION__);	

	return x;
}

int task2_callback(void *params)
{
	struct task_struct *fifoData;
	printk(KERN_INFO "From %s\n",__FUNCTION__);

	while(!kthread_should_stop())
	{
		/* Lock the mutex*/
		if (mutex_lock_interruptible(&fifo_lock))
		{
			printk(KERN_ERR "Cannot get the lock\n");
			//return -1;
			return -ERESTARTSYS;
		}

		/* Wait for the condition variable */

		/* Pop the data from kfifo*/
		if(0 == kfifo_get(MY_KFIFO_NAME_P, &fifoData))
		{
			//printk(KERN_INFO "KFIFO EMPTY\n");
		}
		else	
		{
			/* Process Id and Vruntime */
			printk(KERN_INFO "Previous Process ID: %d, Vruntime: %llu\n",list_prev_entry(fifoData, tasks)->pid, list_prev_entry(fifoData, tasks)->se.vruntime);
			printk(KERN_INFO "Current Process ID: %d, Vruntime: %llu\n",fifoData->pid, fifoData->se.vruntime);
			printk(KERN_INFO "Next Process ID: %d, Vruntime: %llu\n",list_next_entry(fifoData, tasks)->pid, list_next_entry(fifoData, tasks)->se.vruntime);
			y++;
		}

		/* Unlock the mutex*/	
		mutex_unlock(&fifo_lock);

		
	}

	printk(KERN_INFO "%s is terminated\n",__FUNCTION__);

	return y;	
}

int __init kthread_fifo_init(void)
{
	printk(KERN_INFO "Initializing kthread kfifo example Module. Function %s\n",__FUNCTION__);

	/* Init a kfifo */
	INIT_KFIFO(mykfifo);

	/* Create two threads */
	task1 = kthread_run(task1_callback,NULL,"Producer Task");
	if(IS_ERR(task1))	
	{
		printk(KERN_ERR "Producer Thread run failed.\n");
		return -1;
	}

	task2 = kthread_run(task2_callback,NULL,"Consumer Task");
	if(IS_ERR(task2))	
	{
		int ret;
		printk(KERN_ERR "Consumer thread run failed.\n");
		ret = kthread_stop(task1);
		if(-1 != ret)
		{
			printk(KERN_INFO "Producer Thread has stopped with %d\n",ret);
		}
		return -1;
	}

	/* Everything went as expected */
	return 0;
}


void __exit kthread_fifo_exit(void)
{
	/* Delete the kfifo */

	/* Stop the kthreads created */
	int ret = kthread_stop(task1);
	if(-1 != ret)
	{
		printk(KERN_INFO "Producer thread has stopped. Data Produced Count:%d\n",ret);
	}
	else printk(KERN_ERR "Error in Producer Thread");	

	ret = kthread_stop(task2);
	if(-1 != ret)
	{
		printk(KERN_INFO "Consumer thread has stopped. Data Consumed Count:%d\n",ret);
	}
	else printk(KERN_ERR "Error in Consumer Thread");

	printk(KERN_INFO "Exiting Kthread kfifo example Module. Function %s\n",__FUNCTION__);
}


module_init(kthread_fifo_init);
module_exit(kthread_fifo_exit);
