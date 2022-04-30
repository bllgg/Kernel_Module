#include <linux/ioctl.h>

#define ENCRYPT_MAGIC       10
#define DECRYPT_MAGIC       15
#define INSERT_KEY_MAGIC    20
#define EXECUTE_MAGIC       25

#define IOCTL_ENCRYPT       _IO(ENCRYPT_MAGIC, 0)
#define IOCTL_DECRYPT       _IO(DECRYPT_MAGIC, 0)
#define IOCTL_INSERT_KEY    _IO(INSERT_KEY_MAGIC, 0)
#define IOCTL_EXECUTE       _IO(EXECUTE_MAGIC, 0)
