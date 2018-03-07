#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/pid.h>


#define getChildrenCount(child_taskStruct)		\
({												\
	static unsigned int child_count = 0;		\
	struct list_head *list_itr	;				\
	list_for_each(list_itr,child_taskStruct)	\
	{											\
		child_count++;							\
	}											\
	child_count;								\
})


int __init processtree_init(void)
{
	struct task_struct *task;

	printk(KERN_INFO "Initializing The Processtree Kernel Module.\n");

	task = current;

    do
    {
        printk(KERN_INFO "Process : %s\t PID: %d\t State: %d\t Number of Children: %u\t Nice: %d", task->comm, task->pid, task->state, getChildrenCount(&task->children),  task_nice(task));
        task = task->parent;

    }while(0 != task->pid);	

	return 0;
}


void __exit processtree_exit(void)
{
	printk(KERN_INFO "Exiting Process tree example Module. Function %s\n",__FUNCTION__);
}


module_init(processtree_init);
module_exit(processtree_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sharanjeet Singh Mago");
MODULE_DESCRIPTION("");
MODULE_ALIAS("processtree");