import usb
from values import *

def get_device():
    busses = usb.busses()
    device = None 
    for bus in busses:
        devices = bus.devices  
        for dev in devices:
            if dev.idVendor == 0x1130 and dev.idProduct == 0x0202:
                device = dev
                return device
    return None

def ready_interface(handle, interface, interface_num):
    try:
        handle.detachKernelDriver(interface_num)
    except:
        pass
    handle.claimInterface(interface)

def open_device():
    device = get_device()
    print "Version is %s" % device.usbVersion
    print "Device is %s" % device.iManufacturer
    configuration = device.configurations[0]
    iface1 = configuration.interfaces[0][0]
    iface2 = configuration.interfaces[1][0]
    handle = device.open()
    try:
        handle.setConfiguration(configuration)    
    except:
        pass
    ready_interface(handle, iface1, 0)
    ready_interface(handle, iface2, 1)
    handle.reset()
    return handle


handle = open_device()
handle.controlMsg(0x21, 0x09, (), 8, 0x1) #Set_report with report type output, report id 0, interface 1, length 8 
#handle.bulkWrite(0x82, CBW)
handle.interruptWrite(0x81, CBW) # OUT to endpoint 0 with CBW and length
handle.controlMsg(0x21, 0x09, (), 16, 0x0) #Set_report with report type output, report id 0, interface 1, length 8 
#handle.bulkWrite(0x82, GREEN_LIGHT) 
handle.interruptWrite(0x81, GREEN_LIGHT) # OUT to endpoint 0 with data and length 8




