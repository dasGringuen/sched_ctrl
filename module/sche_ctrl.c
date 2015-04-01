/**
 *   @file sch_ctrl.c
 *   @brief -
 *
 *   @author Adrian Remonda
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/sched.h>

spinlock_t wait_lock;

#include "linux/thread_info.h"

static bool wait_sync = false;

struct task_struct *master_task;

static void print_all_tasks(void)
{
	struct task_struct *task = current;
	struct task_struct *t = task;

	printk("Print all tasks\n");

	printk("father %d\n", current->tgid);

	do {
		printk("Task: 0x%p, id=%d", t, task_pid_nr(t));

		if (t == master_task)
			printk("-> master task");

		printk("\n");
		t = next_thread(t);
	} while (t != task);

	printk("\n");
}

static ssize_t reset_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	pr_debug("RESET\n");
	wait_sync = false;

	return 0;
}

static ssize_t reset_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	return count;
}

static ssize_t debug_sched_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	return 0;
}

static ssize_t debug_sched_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	printk("debug_sched\n");
	print_all_tasks();

	return 0;
}


static ssize_t switch_to_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	return 0;
}

struct task_struct *search_task(int pid)
{
	struct task_struct *task = current;
	struct task_struct *t = task;

	printk("Searching pid: %d\n", pid);

	do {
		printk("Task: 0x%p, id=%d", t, task_pid_nr(t));

		if (pid == task_pid_nr(t)) {
			printk("-> found! \n");
			return t;
		}
		printk("\n");
		t = next_thread(t);
	} while (t != task);

	printk("Not found...\n");
	printk("\n");
	return NULL;
}

/*
 *	Switch to function
 */
static ssize_t switch_to_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	int pid;
	int ret;
	struct task_struct *t;

	ret = kstrtoint(buf, 10, &pid);

	set_current_state(TASK_INTERRUPTIBLE);

	pr_debug("switch_to. %s (%ld B)\n", buf, count);
	pr_debug("pid: %d (%d)\n", pid, ret);

	t =	search_task(pid);

	if (t) {
		printk("switching to: %p, pid %d\n", t, task_pid_nr(t));
		wake_up_process(t);
		ret = count;
	} else {
		printk("not found: %d\n", pid);
		print_all_tasks();
		ret = -EINVAL;
	}

	schedule();
	return ret;
}

void sleep_all_but_this(void)
{
	struct task_struct *task = current;
	struct task_struct *t = task;

	pr_debug("sleep_all_but_this\n");
	printk("Current: %p\n", current);

	do {

		if (t)
			printk("%p, pid %d\n", t, task_pid_nr(t));
		else
			printk("null task\n");

		t = next_thread(t);
		//		wake_up_process(t);
	} while (t != task);

	schedule();

	return;
}

/*
 *	Wait for control
 */
static ssize_t wait_for_control_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	pr_err("wait_for_control SHOW\n");
	return 0;
}

static ssize_t wait_for_control_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	unsigned long flags;

	pr_debug("wait_for_control. %s (%ld B) pid:%d\n", buf, count, task_pid_nr(current));

	set_current_state(TASK_INTERRUPTIBLE);
	spin_lock_irqsave(&wait_lock, flags);

	if(buf[0] == '0') {
		spin_unlock_irqrestore(&wait_lock, flags);
		schedule();
		return 0;
	}
	set_current_state(TASK_RUNNING);

	master_task = current;

	/* sleep all but this */
	sleep_all_but_this();

	wait_sync = true;

	/* Rest of the code ... */
	spin_unlock_irqrestore(&wait_lock, flags);
	return 0;
}

static ssize_t wake_all_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{

	struct task_struct *task = current;
	struct task_struct *t = task;
	int pid;

	pr_debug("wake all\n");
	printk("Current: %p\n", current);

	do {
		pid = task_pid_nr(t);

		printk("Task: 0x%p, id=%d", t, pid);

		if (pid == t->tgid) {
			printk(" -> was main thread. Omit \n");
		} else {
			printk(" -> waking up");
			wake_up_process(t);
		}

		printk("\n");
		t = next_thread(t);
	} while (t != task);

	return 0;
}

static struct kobject *root_kobj;

static struct kobj_attribute switch_to_attribute =
__ATTR(switch_to, 0777, switch_to_show, switch_to_store);
static struct kobj_attribute wait_for_control_attribute =
__ATTR(wait_for_control, 0777,  wait_for_control_show, wait_for_control_store);

static struct kobj_attribute debug_sched_attribute =
__ATTR(debug_sched, 0777,  debug_sched_show, debug_sched_store);
static struct kobj_attribute reset_attribute =
__ATTR(reset, 0777,  reset_show, reset_store);

static struct kobj_attribute wake_all_attribute =
__ATTR(wake_all, 0444,  wake_all_show, NULL);

static struct attribute *attrs[] = {
	&switch_to_attribute.attr,
	&wait_for_control_attribute.attr,
	&debug_sched_attribute.attr,
	&reset_attribute.attr,
	&wake_all_attribute.attr,
	NULL,   /* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __init sch_ctrl_init(void)
{
	int ret;

	pr_debug("INIT\n");

	/*
	 * located under /sys/kernel/
	 */
	root_kobj = kobject_create_and_add("sch", kernel_kobj);
	if (!root_kobj) {
		pr_err("Could not create root sys");
		return -ENOMEM;
	}

	/* Create the files associated with this kobject */
	ret = sysfs_create_group(root_kobj, &attr_group);
	if (ret) {
		pr_err("Could not create group");
		ret = -ENOMEM;
		goto err;
	}
	return ret;
err:
	kobject_put(root_kobj);
	return ret;
}

static void __exit sch_ctrl_exit(void)
{
	kobject_put(root_kobj);
	pr_debug("EXIT scheduler\n");
}

module_init(sch_ctrl_init);
module_exit(sch_ctrl_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Remonda");
MODULE_DESCRIPTION("Task9 of linux challenge sysfs");


