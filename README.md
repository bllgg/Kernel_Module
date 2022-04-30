# CSC415-Device-Driver

## Build the kernal module and user application

You can build the kernel module by executing the following command in the Module directory.
```
make all
```
The user app can be build using the 
```
make cryptographyTest
```
command in Test directory.

You can clear all the build files using
```
make clean
```
command in both Module and Test directories.


## View the kernel logs

kernel logs of the linux kernel can be seen by using
```
dmesg --following
```
command.

## Install the kernel module

You need to install the built kernel module in to the kernel using
```
sudo insmode cryptography.ko
```
within the Module directory.

## Run the application

After installing the kernel module into the kernel, the user application can be run.

Use the **sudo** command for run the application, as we are going to access device files with the user application
```
sudo ./cryptographyTest
```
- First you will be asked for provide a key. For that, you need to inser a single capital English letter.

- Then you will be asked for provide the mode. you need to insert the number of the option.

- Then insert the phrase you need to encrypt or decrypt

- Finally you will get the encrypted or decrypted phrase.

**Note:** as im using symmetryc key encription technique, same key can be used for bothe Encryptiona and decryption.

## Unload the kernel module

After running the user application, the kernel module should be unloaded.
```
sudo rmmode cryptography
```
can be used for unload the module.

## Screenshots

![results](https://github.com/bllgg/Kernel_Module/blob/master/ScreenShots/Kazam_screenshot_00000.png)