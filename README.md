# VirtualHubGadget
The Virtual Hub Gadget is a gadgetFS driver that emulates a USB hub, allowing multiple USB devices to be connected to a single USB port on a host computer. This gadget can be used in a variety of scenarios where a USB hub is needed, such as in embedded systems, IoT devices, or other applications where multiple USB devices need to be connected to a single host computer. In terms of interoperability, this gadget should be compatible with any USB host that supports USB hubs, including the Steam Link and Steam Deck from Valve Inc. 

## USB Device Transfer Manager
The USB Device Transfer Manager of the VirtualHubGadget is a program that allows for coordinated and synchronized data transfer between a USB device and a host computer. It supports multiple transfer types, such as control, interrupt,isochronous, and bulk transfers, and uses multiple threads to handle each transfer type separately.
___
# Requirements
C compiler (GCC or Clang recommended)
LibUSB 1.0 library
libudev
___
# Installation
Clone the repository to your local machine.
```bash
git clone https://github.com/amasarac/VirtualHubGadget.git
```
# Install the LibUSB 1.0 library if not already installed. For example, on Ubuntu, you can use the following command:
```bash
sudo apt-get install libusb-1.0-0-dev
```
# Build the program by running the following command from the project directory:
```bash
make
```
# Run shell script that sets the gadget configuration and loads it as a USB gadget driver
```bash
./install.sh
```
# Run the program with the following command:
```bash
./usb-gadget
```
___
# Usage Overview
* Connect the USB device to the host computer. and run the transfer manager on the host to export the usb devices.
* Connect the gadget host to the endpoint computer.
* Run the Virtual Hub Gadget program as described in the installation instructions.
* Use the program to perform data transfers to and from the USB device.
___
#The command line interface for the Virtual Hub Gadget driver consists of the following commands:
* **list**: Lists the available USB devices on the host system.
* **attach** [device]: Attaches the specified USB device to the virtual hub gadget.
* **detach** [device]: Detaches the specified USB device from the virtual hub gadget.
* **connect** [device1] [device2]: Connects two USB devices to each other via the virtual hub gadget.
* **disconnect** [device]: Disconnects a USB device from the virtual hub gadget.
* **reset** [device]: Resets a USB device that is connected to the virtual hub gadget.
* **quit**: Quits the CLI and unloads the Virtual Hub Gadget driver from the kernel.
### The ***attach, detach, connect, disconnect, and reset*** commands require the ***device parameter***, which can be either the ***device ID*** or the ***device name*** as listed by the list command.
___
***The connect command is used to establish a connection between the host and the USB gadget device. It takes one argument, which is the path to the gadget device file (e.g. /dev/hidg0 for HID gadgets). The command first checks if the specified gadget device file exists, and if not, it prints an error message and exits.***

***If the device file exists, the command opens the file and sets the gadget device to the CONFIGURED state. If the device is already in the CONFIGURED state, the command prints a message and does nothing. If the device is in any other state, the command sets the device to the CONFIGURED state, and then waits for the host to send a SET_CONFIGURATION request.***

***Once the host sends the SET_CONFIGURATION request, the command sends a CONFIGURATION message back to the host, indicating that the configuration has been set. At this point, the gadget device is fully connected to the host, and any data transfer can begin.***
___
# Contributing

Contributions to the project are welcome. To contribute, please fork the repository, make your changes, and submit a pull request. Be sure to follow the project's code style and formatting guidelines.
