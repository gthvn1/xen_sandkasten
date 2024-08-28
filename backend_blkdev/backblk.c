#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for KERN_NOTICE */

#include <xen/xen.h>
#include <xen/xenbus.h>

// The function is called on activation of the device.
// TODO: Activation is done using writes on xenstore.
static int backblk_probe(struct xenbus_device *dev,
                         const struct xenbus_device_id *id) {
  pr_info("Probe called.\n");
  return 0;
}

// This defines the name of the devices the driver reacts to
static const struct xenbus_device_id backblk_ids[] = {{"backblkdev"}, {""}};

// We set up the callback functions
static struct xenbus_driver backblk_driver = {
    .ids = backblk_ids,
    .probe = backblk_probe,
};

// On loading this kernel module, we register as a frontend driver
static int __init backblk_init(void) {
  pr_info("Hello World!\n");

  return xenbus_register_backend(&backblk_driver);
}
module_init(backblk_init);

// ...and on unload we unregister
static void __exit backblk_exit(void) {
  xenbus_unregister_driver(&backblk_driver);
  pr_info("Goodbye world.\n");
}
module_exit(backblk_exit);

MODULE_LICENSE("GPL");
MODULE_ALIAS("backblkdev");
