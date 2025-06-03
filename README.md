## VirtualHubGadget

The Virtual Hub Gadget is a gadgetFS driver that emulates a USB hub. It lets multiple USB devices connect to a single port on the host computer. This is helpful for embedded systems, IoT devices or other scenarios where several devices need to share one USB port. The gadget is compatible with any host that supports USB hubs, including the Steam Link and Steam Deck from Valve Inc.

## USB Device Transfer Manager

The transfer manager coordinates data flow between a USB device and the host. It handles control, interrupt, isochronous and bulk transfers in separate threads to keep everything synchronized.
___
## Requirements

- C compiler (GCC or Clang recommended)
- LibUSB 1.0 library
- libudev
___
## Installation

Clone the repository:

```bash
git clone https://github.com/amasarac/VirtualHubGadget.git
```

Install the LibUSB 1.0 library (Ubuntu example):

```bash
sudo apt-get install libusb-1.0-0-dev
```

Build the program:

```bash
make
```

Run the setup script to configure and load the gadget driver:

```bash
./install.sh
```

Run the program:

```bash
./usb-gadget
```
___
## Usage Overview

- Connect the USB device to the host computer and run the transfer manager to export the devices.
- Connect the gadget host to the endpoint computer.
- Run the Virtual Hub Gadget program as described in the installation instructions.
- Use the program to perform data transfers to and from the USB device.
___
## Command Line Interface

- **list**: Lists the available USB devices on the host system.
- **attach** *device*: Attaches the specified USB device to the virtual hub gadget.
- **detach** *device*: Detaches the specified USB device from the virtual hub gadget.
- **connect** *device1 device2*: Connects two USB devices via the virtual hub gadget.
- **disconnect** *device*: Disconnects a USB device from the virtual hub gadget.
- **reset** *device*: Resets a device connected to the virtual hub gadget.
- **quit**: Quits the CLI and unloads the driver from the kernel.

The **attach**, **detach**, **connect**, **disconnect** and **reset** commands require a device parameter, either the device ID or the device name as listed by the **list** command.
___
The **connect** command establishes a connection between the host and the USB gadget device. Provide the path to the gadget device file (for example `/dev/hidg0` for HID gadgets). The command checks whether the file exists and fails with an error message if it does not.

If the file exists, the command opens it and sets the gadget device to the `CONFIGURED` state. If the device is already configured, it prints a message and does nothing. Otherwise it waits for the host to send a `SET_CONFIGURATION` request.

After the host sends this request, the command confirms that the configuration has been set. The gadget is then fully connected to the host and data transfer can begin.
___
## Contributing

Contributions are welcome! Fork the repository, make your changes and submit a pull request. Please follow the project's code style and formatting guidelines.
