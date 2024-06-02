#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/slab.h>

#define DEVICE_NAME "simple_driver"
#define CLASS_NAME  "simple_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sérgio Johann Filho");
MODULE_DESCRIPTION("A generic Linux char driver.");
MODULE_VERSION("0.1.0");

static int majorNumber;
static int number_opens = 0;
static struct class *charClass = NULL;
static struct device *charDevice = NULL;

const int MSG_SIZE_LIST = -1;
const int MAX_MSG = -1;

module_param(MSG_SIZE_LIST, int, 0644);
module_param(MAX_MSG, int, 0644);

#define NAME_SIZE	32 
#define MSG_SIZE_LIST	32
#define MAX_MSG	150

struct process_list{
	struct list_head link;
	char message[MSG_SIZE_LIST];
	//char * message; 
	short size_list;
};

struct message_s { //change message_s to process
	struct list_head link;
	char name[NAME_SIZE];
	//char * name;
	short size_name;
	pid_t pid;
	//struct process_list list_process;
	struct list_head list_process;
	char message[MSG_SIZE_LIST];
	short size;
};

struct list_head list;

int name_process(const char * buffer, char * name){ 
	int i = 4,j = 0;
	while(buffer[i] != '\0'){
		name[j] = buffer[i];
		i++;
		j++;
	}
	name[j] = '\0';
	return j;
}


bool register_process(const char * buffer, size_t len){ 
	
	if (len > NAME_SIZE){ 
		printk(KERN_INFO "Simple Driver: too many characters to deal with (%d)\n", len);
		return false;
	}

	struct message_s *new_node = kmalloc((sizeof(struct message_s)), GFP_KERNEL);

	if (!new_node) {
		printk(KERN_INFO "Memory allocation failed, this should never fail due to GFP_KERNEL flag\n");
		return false;
	}	

	new_node->size_name = name_process(buffer, new_node->name);
	printk("name_process : %s \n", new_node->name);
	printk("size : %d \n", new_node->size_name);
	new_node->pid = (int) task_pid_nr(current);
	INIT_LIST_HEAD(&new_node->list_process);
	list_add_tail(&(new_node->link), &list);
	
	struct message_s *entry;

	list_for_each_entry(entry, &list, link) {
		printk(KERN_INFO "Process #%d: %s\n", entry->pid, entry->name);
	}

	return true;
}

bool new_message(const char * buffer, size_t len){
	struct message_s *entry;

	//char * name_process = kmalloc((sizeof(char) * strlen(kstrndup(buffer, strchr(buffer, ' ') - buffer, GFP_KERNEL))), GFP_KERNEL);
	char * name_process = kmalloc((sizeof(char) * NAME_SIZE), GFP_KERNEL);

	name_process = kstrndup(buffer, strchr(buffer, ' ') - buffer, GFP_KERNEL);

	printk("name_process : %s \n", name_process);
	printk("size : %d \n", strlen(name_process));

	char * message = kmalloc((sizeof(char) * MSG_SIZE_LIST), GFP_KERNEL);

	message = kstrndup(strchr(buffer, ' ') + 1, len, GFP_KERNEL);

	printk("message : %s \n", message);
	printk("size : %d \n", strlen(message));

	//printk("buffer : %s \n", buffer);
	//printk("string : %s \n", name_process);
	//printk("mensagem : %s \n", message);

	bool name_found = false;

	list_for_each_entry(entry, &list, link) {
		if(!strcmp(entry->name, name_process)){
		//if (entry->pid == (int) task_pid_nr(current)){
			strcpy(entry->message, buffer);
			entry->size = strlen(buffer);
			if(list_empty(&entry->list_process)){
				struct process_list *new_node = kmalloc((sizeof(struct process_list)), GFP_KERNEL);
				printk("addr of new_node: %p\n", new_node);
				strcpy(new_node->message, message);
				new_node->size_list = strlen(message);
				list_add_tail(&(new_node->link), &entry->list_process);
				name_found = true;
			}
		}
	}

	struct process_list *entry_process;

	list_for_each_entry(entry, &list, link) {
		list_for_each_entry(entry_process, &entry->list_process, link) {
			printk("penes2 %d: %s\n", entry->pid, entry_process->message);
		}
	}

	kfree(name_process);
	kfree(message);

	if(name_found) return true;
	else return false;
}

int list_add_entry(char *data)
{
	struct message_s *new_node = kmalloc((sizeof(struct message_s)), GFP_KERNEL);
	
	if (!new_node) {
		printk(KERN_INFO "Memory allocation failed, this should never fail due to GFP_KERNEL flag\n");
		
		return 1;
	}
	strcpy(new_node->message, data);
	new_node->size = strlen(data);
	list_add_tail(&(new_node->link), &list);
	
	return 0;
}

int list_delete_head(void)
{
	struct message_s *entry; //= list_first_entry(&list, struct message_s, link);
	struct process_list *entry_process;

	if (list_empty(&list)) {
		printk(KERN_INFO "Empty list.\n");
		
		return 1;
	}

	list_for_each_entry(entry, &list, link) {
		if (entry->pid == (int) task_pid_nr(current)){
			entry_process = list_first_entry(&entry->list_process, struct process_list, link);
			break;
		}
	}

	if (list_empty(&list)) {
		printk(KERN_INFO "Empty list.\n");
		
		return 1;
	}

	entry = list_first_entry(&list, struct message_s, link);
	
	list_del(&entry->link);
	kfree(entry);
		
	return 0;
}

void list_show(void)
{
	struct message_s *entry = NULL;
	int i = 0;
	
	list_for_each_entry(entry, &list, link) {
		printk(KERN_INFO "Message #%d: %s\n", i++, entry->message);
	}
}

int list_delete_entry(char *data)
{
	struct message_s *entry = NULL;
	
	list_for_each_entry(entry, &list, link) {
		if (strcmp(entry->message, data) == 0) {
			list_del(&(entry->link));
			kfree(entry);
			
			return 0;
		}
	}
	
	printk(KERN_INFO "Could not find data.");
	
	return 1;
}


static int	dev_open(struct inode *, struct file *);
static int	dev_release(struct inode *, struct file *);
static ssize_t	dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t	dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

static int simple_init(void)
{
	printk(KERN_INFO "Simple Driver: Initializing the LKM\n");

	// Try to dynamically allocate a major number for the device -- more difficult but worth it
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber < 0) {
		printk(KERN_ALERT "Simple Driver failed to register a major number\n");
		return majorNumber;
	}
	
	printk(KERN_INFO "Simple Driver: registered correctly with major number %d\n", majorNumber);

	// Register the device class
	charClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(charClass)) {		// Check for error and clean up if there is
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Simple Driver: failed to register device class\n");
		return PTR_ERR(charClass);	// Correct way to return an error on a pointer
	}
	
	printk(KERN_INFO "Simple Driver: device class registered correctly\n");

	// Register the device driver
	charDevice = device_create(charClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(charDevice)) {		// Clean up if there is an error
		class_destroy(charClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Simple Driver: failed to create the device\n");
		return PTR_ERR(charDevice);
	}
	
	printk(KERN_INFO "Simple Driver: device class created.\n");
	
	INIT_LIST_HEAD(&list);
		
	return 0;
}

static void simple_exit(void)
{
	device_destroy(charClass, MKDEV(majorNumber, 0));
	class_unregister(charClass);
	class_destroy(charClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	printk(KERN_INFO "Simple Driver: goodbye.\n");
}

static int dev_open(struct inode *inodep, struct file *filep)
{
	number_opens++;
	printk(KERN_INFO "Simple Driver: device has been opened %d time(s)\n", number_opens);
	printk("Process id: %d, name: %s\n", (int) task_pid_nr(current), current->comm);

	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int error = 0;
	struct message_s *entry; //= list_first_entry(&list, struct message_s, link);
	struct process_list *entry_process = NULL;

	if (list_empty(&list)) {
		printk(KERN_INFO "mq list has no data.\n");
		return 0;
	}
	
	list_for_each_entry(entry, &list, link) {
		if (entry->pid == (int) task_pid_nr(current)){
			entry_process = list_first_entry(&entry->list_process, struct process_list, link);
			break;
		}
	}

	if (!entry_process) {
		printk(KERN_INFO "The process pid isn't registered in mq\n");
		return 0;
	}

	if (list_empty(&entry->list_process)) {
		printk(KERN_INFO "The process list has no data to be read.\n");
		return 0;
	}	
	
	// copy_to_user has the format ( * to, *from, size) and returns 0 on success
	error = copy_to_user(buffer, entry_process->message, entry_process->size_list);

	if (!error) {				// if true then have success
		printk(KERN_INFO "Simple Driver: sent %d characters to the user\n", entry_process->size_list);
		//list_delete_head(); adaptar
		list_del(&entry_process->link);
		kfree(entry_process);

		return 0;
	} else {
		printk(KERN_INFO "Simple Driver: failed to send %d characters to the user\n", error);
		
		return -EFAULT;			// Failed -- return a bad address message (i.e. -14)
	}
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	if(!strcmp(kstrndup(buffer, 4, GFP_KERNEL), "reg ")){
		bool register_val = register_process(buffer, len);
		if (register_val) return true;
		else return false;
	}
	else if (len < MSG_SIZE_LIST) {
		new_message(buffer, len);
		
		/*list_add_entry(buffer);
		list_show();

		printk(KERN_INFO "Simple Driver: received %zu characters from the user\n", len);
		
		printk("The process id is %d\n", (int) task_pid_nr(current));*/

		return len;
	} else {
		printk(KERN_INFO "Simple Driver: too many characters to deal with (%d)\n", len);
		
		return 0;
	}

	/*printk(KERN_INFO "Simple Driver: received %zu characters from the user\n", len);

	printk("buffer : %s\n", buffer[3]);

	return 0;*/
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "Simple Driver: device successfully closed\n");

	return 0;
}

module_init(simple_init);
module_exit(simple_exit);
