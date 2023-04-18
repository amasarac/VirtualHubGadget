# VirtualHubGadget
## USB Device Transfer Manager
The USB Device Transfer Manager of the VirtualHubGadget is a program that allows for coordinated and synchronized data transfer between a USB device and a host computer. It supports multiple transfer types, such as control, interrupt, and bulk transfers, and uses multiple threads to handle each transfer type separately.
___
# Requirements
C compiler (GCC or Clang recommended)
LibUSB 1.0 library
libudev
___
# Installation
Clone the repository to your local machine.
```bash
git clone https://github.com/username/repo.git
```
# Install the LibUSB 1.0 library if not already installed. For example, on Ubuntu, you can use the following command:
```bash
sudo apt-get install libusb-1.0-0-dev
```
# Build the program by running the following command from the project directory:
```bash
make
```
# Run the program with the following command:
```bash
./usb_transfer_manager
```
___
# Usage
Connect the USB device to the host computer.
Run the program as described in the installation instructions.
Use the program to perform data transfers to and from the USB device.
___
# Contributing

Contributions to the project are welcome. To contribute, please fork the repository, make your changes, and submit a pull request. Be sure to follow the project's code style and formatting guidelines.
