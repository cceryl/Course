// pid.c
// Kernel module that reports information of the input pid.

#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#define BUFFER_SIZE 128
#define PROC_NAME   "pid"

static ssize_t proc_read(struct file *file, char __user *buf, size_t count, loff_t *pos);
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

static struct proc_ops proc_op = {
    .proc_flags = 0,
    .proc_read  = proc_read,
    .proc_write = proc_write,
};

static pid_t pid;

static int proc_init(void)
{
    proc_create(PROC_NAME, 0666, NULL, &proc_op);
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
    int                 len;
    static bool         complete = false;
    char                buffer[BUFFER_SIZE];
    struct task_struct *task;
    struct pid         *vpid;

    if (complete)
    {
        complete = false;
        return 0;
    }

    vpid = find_get_pid(pid);
    if (vpid == NULL)
        len = sprintf(buffer, "No process with pid = [%d]\n", pid);
    else
    {
        task = pid_task(find_vpid(pid), PIDTYPE_PID);
        if (task == NULL)
            len = sprintf(buffer, "No process with pid = [%d]\n", pid);
        else
            len = sprintf(buffer, "command = [%s] pid = [%d] state = [%d]\n", task->comm, task->pid, task_state_index(task));
    }

    if (copy_to_user(usr_buf, buffer, len))
    {
        printk(KERN_WARNING "/proc/%s copy_to_user failed\n", PROC_NAME);
        return -1;
    }

    complete = true;
    return len;
}

static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos)
{
    char buffer[BUFFER_SIZE];

    if (copy_from_user(buffer, usr_buf, count))
    {
        printk(KERN_WARNING "/proc/%s copy_from_user failed\n", PROC_NAME);
        return -1;
    }

    buffer[count] = '\0';
    if (kstrtoint(buffer, 10, &pid))
    {
        printk(KERN_WARNING "/proc/%s kstrtoint failed\n", PROC_NAME);
        return -1;
    }

    return count;
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PID Module");
MODULE_AUTHOR("Project");