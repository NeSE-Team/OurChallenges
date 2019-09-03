#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h> /* see definition struct file_operations in this file */
#include <linux/mutex.h>
#include <linux/random.h>
#include <linux/cred.h>
#include <linux/slab.h> /* kmalloc */
#include <linux/uaccess.h>
#include <linux/interrupt.h>


int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static long device_ioctl(struct file *, unsigned int, unsigned long);

//#define qword unsigned long
#define INVALID 0
#define BUCKETS 16
#define BUCKET_CAPACITY 16
#define MAX_ALLOCATED 64
#define DEVICE_NAME "osok"

/*
 * control flow hijack 
 */

struct control_flow_hijack_primitive{
    unsigned long rax;
    unsigned long rbx;
    unsigned long rcx;
    unsigned long rdx;
    unsigned long rsi;
    unsigned long rdi;
    unsigned long rsp;
    unsigned long rbp;
    unsigned long r8;
    unsigned long r9;
    unsigned long r10;
    unsigned long r11;
    unsigned long r12;
    unsigned long r13;
    unsigned long r14;
    unsigned long r15;
    unsigned long rip;
    unsigned long reset_all;
};

/*
 * The device file operations.
 */
static const struct file_operations fops = {
	.open           = device_open,
	.release        = device_release,
	.unlocked_ioctl = device_ioctl,
};

/*
 * The token bucket structure.
 */
struct token_bucket {
	__u64 token[BUCKET_CAPACITY];
};

/*
 * The taken token structure to be managed in a list
 */
struct taken_token {
	__u64 bucket_id;
	__u64 token;
};

struct taken_token_user {
	uid_t euid;
	struct taken_token token;
};

/*
 * The token storage structure.
 */
struct token_storage {
	struct taken_token_user taken_list[MAX_ALLOCATED];
	struct token_bucket free_bucket[BUCKETS];
};

static int major = -1;
static struct cdev mycdev;
static struct class *myclass = NULL;

static struct token_storage* storage = NULL;
static DEFINE_MUTEX(ts_mutex);

static void cleanup(int device_created)
{
	if (device_created) {
		device_destroy(myclass, major);
		cdev_del(&mycdev);
	}
	if (myclass)
		class_destroy(myclass);
	if (major != -1)
		unregister_chrdev_region(major, 1);
}

/*
 * This function is called when the module is loaded
 */

int init_module(void)
{
	int device_created = 0;
	printk(KERN_INFO "osok module init_module\n");
	/* cat /proc/devices */
	if (alloc_chrdev_region(&major, 0, 1, DEVICE_NAME "_proc") < 0)
		goto error;
	printk(KERN_INFO "osok chardev is created\n");
	/* ls /sys/class */
	if ((myclass = class_create(THIS_MODULE, DEVICE_NAME "_sys")) == NULL)
		goto error;
	printk(KERN_INFO "osok sysclass is created\n");
	/* ls /dev/ */
	if (device_create(myclass, NULL, major, NULL, DEVICE_NAME ) == NULL)
		goto error;
	printk(KERN_INFO "osok node is created\n");
	device_created = 1;
	cdev_init(&mycdev, &fops);
	if (cdev_add(&mycdev, major, 1) == -1)
		goto error;
	printk(KERN_INFO "osok module init finished\n");
	return 0;
error:
	cleanup(device_created);
	return -1;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{
	cleanup(1);
}

static int device_open(struct inode *inode, struct file *file)
{
    return 0;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	/* The call never fails */
	return 0;
}

struct control_flow_hijack_primitive cfh;
unsigned long rax,rbx,rcx,rdx,rsi,rdi,rsp,rbp,rip,r8,r9,r10,r11,r12,r13,r14,r15,reset_all;
int done=0;

int one_gadget_chain(unsigned long arg){
    if(done){
        return 0;
    }
        __asm__("push %[rdi]\n"
                "pop %%rdi\n"
               : : [rdi] "m"(rdi) : );
    rdi=0;
    done=1;
        __asm__(
                "xor %%rax, %%rax\n"
                //"xor %%rbx, %%rbx\n"
                "xor %%rcx, %%rcx\n"
                "xor %%rdx, %%rdx\n"
                "xor %%rsi, %%rsi\n"
                "push %[rip]\n"
                "ret\n"
                : : [rip] "m"(rip):);
    BUG_ON(1);
    return -EACCES;
}

#define IRQ_NO 11


static irqreturn_t irq_handler(int irq,void *dev_id) {
  one_gadget_chain(0);
  //printk(KERN_INFO "Shared IRQ: Interrupt Occurred");
  return IRQ_HANDLED;
}

int handle_args(unsigned long arg){
    int copied_bytes;
    copied_bytes=copy_from_user(&cfh, (__user void*)arg, sizeof(struct control_flow_hijack_primitive));
    if(copied_bytes<0){
        return -EACCES;
    }
    cfh.rax=0x92;
    cfh.rbx=0x10;
    cfh.rcx=0x92;
    cfh.rdx=0x10;
    cfh.rsi=0x92;
    cfh.r8=0x10;
    cfh.r9=0x10;
    cfh.r10=0x10;
    cfh.r11=0x10;
    cfh.r12=0x92;
    cfh.r13=0x92;
    cfh.r14=0x92;
    rdi = cfh.rdi;
    rsp = 0x9210;
    rip = cfh.rip;
    memset(&cfh,0,sizeof(struct control_flow_hijack_primitive));
    
    if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "osok_test", (void *)(irq_handler))) {
        printk(KERN_INFO "my_device: cannot register IRQ ");
        goto irq;
    }     
    return 0;
irq:
        free_irq(IRQ_NO,(void *)(irq_handler));
    return -1;
}

static long device_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	uid_t cur_euid = current_cred()->euid.val;
	int ret = 0;
	mutex_lock(&ts_mutex);
	//if(cmd <= 1336 || cmd >= 1340)return -EINVAL;
	switch(cmd) {
    case 1337:
        /* do sport*/
        ret = handle_args(arg);
        if(ret){
            //ret = one_gadget_chain(arg);
            BUG();
        }
    default:
        mutex_unlock(&ts_mutex);
        return -EINVAL;
	}
	mutex_unlock(&ts_mutex);
	return ret;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ww9210 <ww9210@gmail.com>");
MODULE_DESCRIPTION("one shot one kill");
