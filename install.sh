#!/bin/bash

# Set configuration
modprobe libcomposite
cd /sys/kernel/config/usb_gadget/
mkdir -p pi0_usb_gadget
cd pi0_usb_gadget

echo 0x03EB > idVendor
echo 0x0902 > idProduct
echo 0x0100 > bcdDevice

mkdir -p strings/0x409
echo "Virtual Hub Gadget" > strings/0x409/product
echo "GadgetFS" > strings/0x409/manufacturer

# Configure functions
mkdir -p functions/hid.usb0
echo 1 > functions/hid.usb0/protocol
echo 1 > functions/hid.usb0/subclass
echo 8 > functions/hid.usb0/report_length
echo -ne "\x05\x01\x09\x06\xa1\x01\x05\x07\x19\xe0\x29\xe7\x15\x00\x25\x01\x75\x01\x95\x08\x81\x02\x95\x01\x75\x08\x81\x03\x05\x08\x09\x01\xa1\x00\x05\x0d\x09\x01\xa1\x01\x85\x03\x75\x08\x95\x02\x91\x02\xc0\xc0" > functions/hid.usb0/report_desc

mkdir -p functions/hub.usb0
echo 4 > functions/hub.usb0/ports

ln -s functions/hid.usb0 configs/c.1/
ln -s functions/hub.usb0 configs/c.1/

# Enable gadget
ls /sys/class/udc > UDC
