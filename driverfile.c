#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Multi-Key Sequence Shortcut Driver");

static char *sequence[] = {"ctrl", "alt", "y"};  // Example sequence
static int seq_index = 0;
static unsigned long last_key_time = 0;
static const unsigned long timeout_ms = 1000; // Timeout for sequence (1 second)

static void execute_action(void) {
    printk(KERN_INFO "Shortcut Action: Open YouTube\n");

    // xdg-open opens YouTube in default browser
    char *argv[] = {"/usr/bin/xdg-open", "https://www.youtube.com", NULL};
    char *envp[] = {NULL};

    // call_usermodehelper to launch the browser
    int ret = call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);

    if (ret < 0) {
        printk(KERN_ERR "Failed to execute action: %d\n", ret);
    } else {
        printk(KERN_INFO "Browser launched successfully.\n");
    }
}

static void reset_sequence(void) {
    seq_index = 0;
    printk(KERN_INFO "Sequence reset.\n");
}

static int keyboard_event_notify(struct notifier_block *nb, unsigned long action, void *data) {
    struct keyboard_notifier_param *param = data;
    unsigned long current_time = jiffies;

    if (action == KBD_KEYCODE && param->down) {
        char *key_name;

        switch (param->value) {
            case KEY_LEFTCTRL: key_name = "ctrl"; break;
            case KEY_LEFTALT:  key_name = "alt"; break;
            case KEY_Y:        key_name = "y"; break;
            default: reset_sequence(); return NOTIFY_OK;
        }

        if (time_after(jiffies, last_key_time + msecs_to_jiffies(timeout_ms))) {
            reset_sequence();
        }
        last_key_time = current_time;

        if (strcmp(key_name, sequence[seq_index]) == 0) {
            seq_index++;
            if (seq_index == ARRAY_SIZE(sequence)) {
                printk(KERN_INFO "Shortcut detected: Ctrl + Alt + Y\n");
                execute_action();
                reset_sequence();
            }
        } else {
            reset_sequence();
        }
    }

    return NOTIFY_OK;
}

static struct notifier_block keyboard_nb = {
    .notifier_call = keyboard_event_notify
};

static int __init shortcut_driver_init(void) {
    printk(KERN_INFO "Loading shortcut driver...\n");
    register_keyboard_notifier(&keyboard_nb);
    return 0;
}

static void __exit shortcut_driver_exit(void) {
    printk(KERN_INFO "Unloading shortcut driver...\n");
    unregister_keyboard_notifier(&keyboard_nb);
}

module_init(shortcut_driver_init);
module_exit(shortcut_driver_exit);
