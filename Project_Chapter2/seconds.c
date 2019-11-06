/**
 * jiffies.c
 *
 * Kernel module that communicates with /proc file system.
 * 
 * The makefile must be modified to compile this program.
 * Change the line "simple.o" to "seconds.o"
 *
 * Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128

#define PROC_NAME "seconds"
#define MESSAGE "Hello World\n"

unsigned long old_jiffies;

/**
 * Function prototypes
 */
static ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
};


/* This function is called when the module is loaded. */
static int proc_init(void)
{

        // creates the /proc/seconds entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
	old_jiffies = jiffies;
        proc_create(PROC_NAME, 0, NULL, &proc_ops);
        printk(KERN_INFO "Loading Module\n");
	printk(KERN_INFO "frequency of the time interrupt: %d\n", HZ);
	printk(KERN_INFO "jiffies when the module is loaded: %lu\n", old_jiffies);
        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

	return 0;
}

/* This function is called when the module is removed. */
static void proc_exit(void) {

        // removes the /proc/hello entry
        remove_proc_entry(PROC_NAME, NULL);

        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/jiffies is read.
 * 
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 *
 * params:
 *
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */


static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;
	int time_elasped = (jiffies - old_jiffies) / HZ;


        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;


	printk(KERN_INFO "Jiffies when the function is called: %lu\n", jiffies);
	printk(KERN_INFO "Time elasped : %d \n ", time_elasped);

        rv = sprintf(buffer, "Time elasped : %d \n", time_elasped);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("SGG");
