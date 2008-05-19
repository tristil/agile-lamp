import usb

class DeviceDescriptor(object) :
    def __init__(self, vendor_id, product_id, interface_id) :
        self.vendor_id = vendor_id
        self.product_id = product_id
        self.interface_id = interface_id

    def getDevice(self) :
        """
        Return the device corresponding to the device descriptor if it is
        available on a USB bus.  Otherwise, return None.  Note that the
        returned device has yet to be claimed or opened.
        """
        buses = usb.busses()
        for bus in buses :
            for device in bus.devices :
                if device.idVendor == self.vendor_id :
                    if device.idProduct == self.product_id :
                        return device
        return None


VENDOR_ID = 0x1130
PRODUCT_ID = 0x0202

device_descriptor = DeviceDescriptor(VENDOR_ID, PRODUCT_ID, 1)
device = device_descriptor.getDevice()
handle = device.open()
handle.claimInterface(1)

# Set_Report with report type output, report ID 0, interface 1, and length 8

#ret = hid_set_output_report(hid_interface_1, (), "8") 

# OUT transaction to endpoint 0 with CBW indicates write and length.
#ret = hid_interrupt_write(hid_interface_0, 0x81, CBW, 1000);

# Set_Report with report type output, report ID 0, interface 0, length 16.

#ret = hid_set_output_report(hid_interface_0, (), "16") 

# OUT transaction to endpoint 0 with data and length 8.

#ret = hid_interrupt_write(hid_interface_0, 0x81, "/0xff/0xfe/0xfd/0xfc/0xfb/0xfa/0xf9/0xf9", 1000);

# OUT transaction to endpoint 0 with data and length 8.

#ret = hid_interrupt_write(hid_interface_0, 0x81, "/0xf7/0xf6/0xf5/0xf4/0xf3/0xf2/0xf1/0xf0", 1000);

