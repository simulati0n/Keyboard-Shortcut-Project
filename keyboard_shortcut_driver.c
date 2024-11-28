#include <linux/module.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/input.h>

static int notify_key_event(struct notifier_block *nblock,
                            unsigned long code, void *_param) {
    struct keyboard_notifier_param *param = _param;

    if (code == KBD_KEYSYM && param->down) {
        printk(KERN_INFO "Key %d pressed\n", param->value);
    }

    return NOTIFY_OK;
}

static struct notifier_block key_notifier = {
    .notifier_call = notify_key_event,
};

static int __init key_shortcut_driver_init(void) {
    register_keyboard_notifier(&key_notifier);
    printk(KERN_INFO "Keyboard Shortcut Driver Loaded\n");
    return 0;
}

static void __exit key_shortcut_driver_exit(void) {
    unregister_keyboard_notifier(&key_notifier);
    printk(KERN_INFO "Keyboard Shortcut Driver Unloaded\n");
}

module_init(key_shortcut_driver_init);
module_exit(key_shortcut_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Keyboard Shortcut Driver");
