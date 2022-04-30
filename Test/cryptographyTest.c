#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "../Module/ioctl_basic.h"

#define BUFFER_LENGTH 256
#define KEY_LENGTH 4

static char receive[BUFFER_LENGTH];

int main()
{
	int ret, fd;
	char stringToSend[BUFFER_LENGTH];
	char key[KEY_LENGTH];
	int mode;

	unsigned long converted_key;

	printf("Starting Cryptography module test...\n");

	fd = open("/dev/cryptography", O_RDWR);
	if (fd < 0)
	{
		perror("Failed to open the device...");
		return errno;
	}

	printf("Insert the Key (%d charactor long):\n", KEY_LENGTH);
	scanf("%[^\n]%*c", key);
	if (strlen(key) != KEY_LENGTH)
	{
		printf("Key should be %d chars!\n", KEY_LENGTH);
		return -1;
	}

	converted_key = 0;
	converted_key |= (key[0]);
	converted_key |= (key[1] << 8);
	converted_key |= (key[2] << 16);
	converted_key |= (key[3] << 24);

	ioctl(fd, IOCTL_INSERT_KEY, converted_key);

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

	printf("Type in a short string to send to the kernel module for crypto graphic operation:\n");
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

	printf("Press ENTER to read back from the device after the cryptographic operation...\n");
	getchar();

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
