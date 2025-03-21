// jiffies.c
// Kernel module that reports the value of jiffies.

#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

#define BUFFER_SIZE 32
#define PROC_NAME   "jiffies"

static ssize_t proc_read(struct file *file, char __user *buf, size_t count, loff_t *pos);

static struct proc_ops proc_op = {
    .proc_flags = 0,
    .proc_read  = proc_read,
};

static int proc_init(void)
{
    proc_create(PROC_NAME, 0, NULL, &proc_op);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

    return 0;
}

static void proc_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int         len;
    static bool complete = false;
    char        buffer[BUFFER_SIZE];

    if (complete)
    {
        complete = false;
        return 0;
    }

    len = sprintf(buffer, "%llu\n", get_jiffies_64());
    if (copy_to_user(usr_buf, buffer, len))
    {
        printk(KERN_WARNING "/proc/%s copy_to_user failed\n", PROC_NAME);
        return 0;
    }

    complete = true;
    return len;
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Jiffies Module");
MODULE_AUTHOR("Project");