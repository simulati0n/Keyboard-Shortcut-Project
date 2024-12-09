#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "shortcut_dev"
#define CLASS_NAME "shortcut"
#define IOCTL_MAGIC 'k'
#define IOCTL_CMD _IO(IOCTL_MAGIC, 1)

MODULE_LICENSE("GPL");

static char *sequence[] = {"ctrl", "alt", "y"}; // Sequence to detect
static int seq_index = 0; // Index in the sequence
static unsigned long last_key_time = 0; // Timestamp of last key press
static const unsigned long timeout_ms = 4000; // Timeout for sequence (4 seconds)
static int signal_sent = 0; // Flag to indicate if the signal has been sent

static int major_number;
static struct class *shortcut_class = NULL;
static struct device *shortcut_device = NULL;
static struct cdev shortcut_cdev;

static int device_open(struct inode *inode, struct file *file) {
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    return 0;
}

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    if (cmd == IOCTL_CMD) {
        printk(KERN_INFO "IOCTL command received: Resetting signal\n");
        signal_sent = 0; // Reset the signal flag
    }
    return 0;
}

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = device_ioctl,
};

// Function to execute the action (Open YouTube)
static void execute_action(void) {
    if (!signal_sent) {
        printk(KERN_INFO "Shortcut Action: Signaling user-space...\n");
        signal_sent = 1; // Set the signal flag
    }
}

// Function to reset the sequence
static void reset_sequence(void) {
    seq_index = 0;
    last_key_time = 0;
}

// Keyboard event notification
static int keyboard_event_notify(struct notifier_block *nb, unsigned long action, void *data) {
    struct keyboard_notifier_param *param = data;
    unsigned long current_time = jiffies;

    if (action == KBD_KEYCODE && param->down) {
        printk(KERN_INFO "Key pressed: keycode=%d\n", param->value);

        char *key_name;
        switch (param->value) { // cases can be customized, but are defined for ctrl alt y currently
            case KEY_LEFTCTRL: key_name = "ctrl"; break;
            case KEY_LEFTALT:  key_name = "alt"; break;
            case KEY_Y:        key_name = "y"; break;
            default:
                printk(KERN_INFO "Unknown key or not part of sequence. Resetting.\n");
                reset_sequence();
                return NOTIFY_OK;
        }

        // Check timeout
        if (time_after(jiffies, last_key_time + msecs_to_jiffies(timeout_ms))) {
            printk(KERN_INFO "Timeout exceeded. Resetting sequence.\n");
            reset_sequence();
        }
        last_key_time = current_time;

        // Match current key with sequence
        if (strcmp(key_name, sequence[seq_index]) == 0) {
            printk(KERN_INFO "Key matched: %s\n", key_name);
            seq_index++;
            if (seq_index == ARRAY_SIZE(sequence)) {
                printk(KERN_INFO "Shortcut detected: Ctrl + Alt + Y\n");
                execute_action();
                reset_sequence();
            }
        } else {
            printk(KERN_INFO "Key mismatch. Resetting sequence.\n");
            reset_sequence();
        }
    }

    return NOTIFY_OK;
}

// Notifier block structure
static struct notifier_block keyboard_nb = {
    .notifier_call = keyboard_event_notify
};

// Module initialization
static int __init shortcut_driver_init(void) {
    printk(KERN_INFO "Loading keyboard shortcut driver...\n");

    // Register character device
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }

    // Register device class
    shortcut_class = class_create(CLASS_NAME);
    if (IS_ERR(shortcut_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(shortcut_class);
    }

    // Register device driver
    shortcut_device = device_create(shortcut_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(shortcut_device)) {
        class_destroy(shortcut_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(shortcut_device);
    }

    // Initialize cdev
    cdev_init(&shortcut_cdev, &fops);
    shortcut_cdev.owner = THIS_MODULE;
    if (cdev_add(&shortcut_cdev, MKDEV(major_number, 0), 1) < 0) {
        device_destroy(shortcut_class, MKDEV(major_number, 0));
        class_destroy(shortcut_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to add cdev\n");
        return -1;
    }

    register_keyboard_notifier(&keyboard_nb);
    return 0;
}

// Module exit
static void __exit shortcut_driver_exit(void) {
    printk(KERN_INFO "Unloading keyboard shortcut driver...\n");
    unregister_keyboard_notifier(&keyboard_nb);
    cdev_del(&shortcut_cdev);
    device_destroy(shortcut_class, MKDEV(major_number, 0));
    class_destroy(shortcut_class);
    unregister_chrdev(major_number, DEVICE_NAME);
}

module_init(shortcut_driver_init);
module_exit(shortcut_driver_exit);
