#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "../Module/ioctl_basic.h"

#define BUFFER_LENGTH 16

static char receive[BUFFER_LENGTH];

int main()
{
	// variables
	int ret, fd;
	char stringToSend[BUFFER_LENGTH];
	char key;
	int mode;

	unsigned long converted_key;

	printf("Starting Cryptography module test...\n");

	// open device file
	fd = open("/dev/cryptography", O_RDWR);
	if (fd < 0)
	{
		perror("Failed to open the device...");
		return errno;
	}

	// get key
	printf("Insert the Key (Key should be a capital English letter):\n");
	scanf(" %c", &key);
	if (key < 41 || key > 90)
	{
		printf("Invalid key...\n");
		return -1;
	}

	// set the key
	ioctl(fd, IOCTL_INSERT_KEY, (unsigned long) key);

	// get mode
	printf("Chose mode (input the option number):\n");
	printf("1. Encryption\n");
	printf("2. Decryption\n");
	scanf(" %d", &mode);

	switch (mode)
	{
	case 1:
		ioctl(fd, IOCTL_ENCRYPT);
		break;

	case 2:
		ioctl(fd, IOCTL_DECRYPT);
		break;

	default:
		exit(-1);
	}

	// get string to send
	printf("Type in a short string to send to the kernel module for cryptographic operation:\n");
	scanf(" %[^\n]%*c", stringToSend);

	if (strlen(stringToSend) > BUFFER_LENGTH)
	{
		printf("String too long!\n");
		return -1;
	}

	printf("Writing message to the device [%s].\n", stringToSend);

	ret = write(fd, stringToSend, strlen(stringToSend));
	if (ret < 0)
	{
		perror("Failed to write the message to the device.");
		return errno;
	}

	printf("Executing cryptographic operation...\n");
	ioctl(fd, IOCTL_EXECUTE);

	printf("Press ENTER to read back from the device after the cryptographic operation...\n");
	getchar();

	// read back from the device
	printf("Reading from the device...\n");
	ret = read(fd, receive, BUFFER_LENGTH);
	if (ret < 0)
	{
		perror("Failed to read the message from the device.");
		return errno;
	}
	printf("The received message is: [%s]\n", receive);
	
	printf("End of the program\n");
	return 0;
}
