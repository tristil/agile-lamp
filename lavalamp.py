import usb

# Code is to turn on/turn off lights and a bell on a USB lava lamp
#
# Manufacturer says:
#
# Host first issues the CBW(Command Block Wrapper) which indicates the command
# and length to endpoint 0 by Set_Report request with wIndex equal to 0 which
# indicates interface 0. Firmware will parse the CBW in order to determine the
# next response. Then host issue IN transaction to endpoint 1 or OUT transaction
# to endpoint 0 with wIndex equal to 1 which indicates interface 1. 
#
# Also:
# 
# Write buffer (02) Host will issue OUT packets with specified length following
# CBW packet. Firmware will save data into buffer. Currently only length 16 write
# is available. That means CBW could only specify the length 16.

# These are values the manufacturer gave for testing. I don't really know how
# to use them, so I stuck them in tuples.  

# or 5a 5a 5a 5a 5a 5a 5a 5a 5a 5a 5a 5a 5a 5a 5a 5a
GREEN_LIGHT = (0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a)

# or 0 1 4 9 10 19 24 31 40 51 64 79 90 a9 c4 e1
RED_LIGHT_WITH_BELL = (0x0, 0x1, 0x4, 0x9, 0x10, 0x19, 0x24, 0x31, 0x40, 0x51, 0x64, 0x79, 0x90, 0xa9, 0xc4, 0xe1)

# or 0 1 2 3 4 5 6 7 8 9 a b c d e f 
DONT_REMEMBER = (0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf)

# or ff fe fd fc fb fa f9 f8 f7 f6 f5 f4 f3 f2 f1 f0
ALL_OFF = (0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0)

SIXTEEN_BYTE_THEORY = (0,0,0,0, 0,0,0,0, 
                       0,0,0,0, 0,0,0,0,
                       0,0,0,0, 0,0,0,0,
                       0,0,0,0, 0,0,0,0,
                       0,0,0,0, 0,0,0,0,
                       0,0,0,0, 0,0,0,0,
                       0,0,0,0, 0,0,0,0,
                       0,0,0,0, 0,0,0,5)
SIXTEEN_BYTE_THEORY_OTHER_WAY = (5,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0)

SET_FEATURE_REQUEST_TYPE = usb.TYPE_STANDARD | usb.RECIP_ENDPOINT

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
    # This stuff seems to be boilerplate for getting a device 
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

    # After this I want to do something with the devhandle.

    # I'm going on the assumption that control message is the same as CBW, but
    # I don't have to construct the entire bitmap. Spec refers to set_Report, which
    # is not a listed "requesttype" in libusb, but elsewhere it's listed as 0x21

    # Python control message is requesttype, request, buffer, value, index
    report_set = handle.controlMsg(0x21, 0x09, (), 0, 0x0) 
    print report_set

    # I don't know if that did anything. I also don't know if the next step is
    # another "CBW"/control message or an interruptWrite or a bulkWrite. IN
    # transaction to endpoint 1 makes me think interruptWrite. 

    # Just reading to see if anything comes out
    read = handle.bulkRead(0x82, 16, 1000)
    print "First read is %s" % str(read)

    # This is where something needs to happen.
    # 0x82 is endpoint2, the IN/writable endpoint
    print "Writing to 0x82" 

    # interrupt-style messages are endpoint, buffer, timeout
    handle.bulkWrite(0x82, SIXTEEN_BYTE_THEORY, 1000)
    handle.interruptWrite(0x82, SIXTEEN_BYTE_THEORY, 1000)
    handle.controlMsg(SET_FEATURE_REQUEST_TYPE, usb.REQ_SET_FEATURE, SIXTEEN_BYTE_THEORY, 32, 0x82)

    # I got this from http://www.beyondlogic.org/usbnutshell/usb6.htm under
    # Standard Endpoint Requests, Standard Endpoint SET_FEATURE request. This
    # gives a broken pipe message.

    #wrote = handle.controlMsg(0x010b, 0x03, (0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,9), 1, 0x82)
    #wrote = handle.controlMsg(0x21, 0x09, (0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,9), 0x0, 1)
    #print wrote

    read = handle.bulkRead(0x82, 16, 1000)
    print "Second read (after 'write' is %s)" % str(read)

open_device()
