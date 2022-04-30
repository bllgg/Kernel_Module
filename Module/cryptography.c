#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "ioctl_basic.h"

#define DEVICE_NAME "cryptography"
#define CLASS_NAME "crypto"

#define BUFFER_LENGTH 16

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Doe"); // ToDO: Change this as your name
MODULE_DESCRIPTION("A simple Linux char driver for encrypt and decrypt");
MODULE_VERSION("1.0");

static int majorNumber;
static char received_message[BUFFER_LENGTH] = {0};
static char send_message[BUFFER_LENGTH] = {0};
unsigned char key;
static short size_of_message;
static int numberOpens = 0;
static struct class *cryptoClass = NULL;
static struct device *cryptoDevice = NULL;
static bool isEncrypt = true;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static long ioctl_funcs(struct file *filp, unsigned int cmd, unsigned long arg);
static int encrypt_or_decrypt(bool encrypt);

static struct file_operations fops =
    {
        .open = dev_open,
        .read = dev_read,
        .write = dev_write,
        .release = dev_release,
        .unlocked_ioctl = ioctl_funcs,
};

static int __init crypto_init(void)
{
    printk(KERN_INFO "CRYPTOGRAPHY: Initializing the CRYPTOGRAPHY LKM\n");

    // Try to dynamically allocate a major number for the device -- more difficult but worth it
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0)
    {
        printk(KERN_ALERT "CRYPTOGRAPHY failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "CRYPTOGRAPHY: registered correctly with major number %d\n", majorNumber);

    // Register the device class
    cryptoClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(cryptoClass))
    { // Check for error and clean up if there is
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(cryptoClass); // Correct way to return an error on a pointer
    }
    printk(KERN_INFO "CRYPTOGRAPHY: device class registered correctly\n");

    // Register the device driver
    cryptoDevice = device_create(cryptoClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(cryptoDevice))
    {                               // Clean up if there is an error
        class_destroy(cryptoClass); // Repeated code but the alternative is goto statements
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(cryptoDevice);
    }
    printk(KERN_INFO "CRYPTOGRAPHY: device class created correctly\n"); // Made it! device was initialized
    return 0;
}

static void __exit crypto_exit(void)
{
    device_destroy(cryptoClass, MKDEV(majorNumber, 0)); // remove the device
    class_unregister(cryptoClass);                      // unregister the device class
    class_destroy(cryptoClass);                         // remove the device class
    unregister_chrdev(majorNumber, DEVICE_NAME);        // unregister the major number
    printk(KERN_INFO "CRYPTOGRAPHY: Exiting from the LKM!\n");
}

static long ioctl_funcs(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    switch (cmd)
    {

    case IOCTL_EXECUTE:
        if (isEncrypt)
        {
            ret = encrypt_or_decrypt(true);
        }
        else
        {
            ret = encrypt_or_decrypt(false);
        }

        if (ret == 0)
        {
            printk(KERN_INFO "CRYPTOGRAPHY: Executed successfully\n");
        }
        else
        {
            printk(KERN_INFO "CRYPTOGRAPHY: Executed unsuccessfully\n");
        }
        
        break;

    case IOCTL_ENCRYPT:
        printk(KERN_INFO "CRYPTOGRAPHY: Encrypt selected");
        isEncrypt = true;
        break;

    case IOCTL_DECRYPT:
        printk(KERN_INFO "CRYPTOGRAPHY: Decrypt selected");
        isEncrypt = false;
        break;

    case IOCTL_INSERT_KEY:
        key = arg & 0xFF;
        printk(KERN_INFO "CRYPTOGRAPHY: Added key => %c", key);

        break;
    default:
        printk(KERN_INFO "CRYPTOGRAPHY: Command not found");
        ret = -EINVAL;
        break;
    }
    return ret;
}

static int encrypt_or_decrypt(bool encrypt)
{

    if (encrypt)
    {
        size_t i;

        for (i = 0; i < strlen(received_message); i++)
        {
            send_message[i] = (received_message[i] -32 + key -32) % 95 + 32;
            printk(KERN_INFO "CRYPTOGRAPHY: Encrypted message => %c", send_message[i]);
        }
        
        return 0;
    }
    else
    {
        size_t i;

        for (i = 0; i < strlen(received_message); i++)
        {
            char temp = ((received_message[i] - 32) - (key - 32));
            if (temp < 0)
            {
                temp = 95 + temp;
            }
            send_message[i] = temp + 32;
        }
        
        return 0;
    }

    return -1;
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    numberOpens++;
    printk(KERN_INFO "CRYPTOGRAPHY: Device has been opened %d time(s)\n", numberOpens);
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int error_count = 0;
    // copy_to_user has the format ( * to, *from, size) and returns 0 on success
    error_count = copy_to_user(buffer, send_message, size_of_message);

    if (error_count == 0)
    { // if true then have success
        printk(KERN_INFO "CRYPTOGRAPHY: Sent %d characters to the user\n", size_of_message);
        return (size_of_message = 0); // clear the position to the start and return 0
    }
    else
    {
        printk(KERN_INFO "CRYPTOGRAPHY: Failed to send %d characters to the user\n", error_count);
        return -EFAULT; // Failed -- return a bad address received_message (i.e. -14)
    }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    size_t ret = copy_from_user(received_message, buffer, len);
    // sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
    size_of_message = strlen(received_message); // store the length of the stored message
    printk(KERN_INFO "CRYPTOGRAPHY: Received %zu characters from the user\n", len);
    return ret;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    size_t i;
    for (i = 0; i < BUFFER_LENGTH; i++)
    {
        received_message[i] = 0;
        send_message[i] = 0;
    }
    printk(KERN_INFO "CRYPTOGRAPHY: Device successfully closed\n");
    return 0;
}

module_init(crypto_init);
module_exit(crypto_exit);
