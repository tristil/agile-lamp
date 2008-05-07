require 'rubygems'
require 'usb'

WRITE_ENDPOINT1 = 0x81
WRITE_ENDPOINT2 = 0x82

MESSAGE0="5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5aEOT"
MESSAGE1="0149101924314051647990a9c4e1EOT"
MESSAGE2="0123456789abcde"
MESSAGE3="fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0EOT"
def get_interface(handle, interface)
  begin
    handle.usb_claim_interface(interface)
  rescue
    handle.usb_detach_kernel_driver_np(interface, interface)
    retry
  end
end

def interrupt_write(handle, endpoint)
  begin
    #error_msg = handle.usb_interrupt_write(endpoint, MESSAGE0, 1000) 
    #error_msg = handle.usb_interrupt_write(endpoint, MESSAGE1, 1000) 
    #error_msg = handle.usb_interrupt_write(endpoint, MESSAGE2, 1000) 
    #error_msg = handle.usb_interrupt_write(endpoint, MESSAGE3, 1000) 
    #error_msg = handle.usb_bulk_write(endpoint, MESSAGE3, 1000) 
    handle.usb_control_msg(requesttype=0x81, request = USB::USB_REQ_SET_FEATURE, value = 0x8, index = 1, bytes = "", timeout = 1000)
    puts "wrote interrupt for #{endpoint}"
    puts error_msg
    raise if error_msg < 0 
  rescue RuntimeError
    puts "Transmission error in sending to #{endpoint}"
  rescue Errno::EBUSY
    puts "couldn't write interrupt for #{endpoint} because device was busy"
  rescue Errno::ETIMEDOUT
    puts "couldn't write interrupt for #{endpoint} because device timed out"
  end
end

device = USB.devices.find_all {|device| (device.idVendor == 4400) && (device.idProduct == 514)}.first

handle = device.usb_open
get_interface(handle, 0x0)
get_interface(handle, 0x1)

interrupt_write(handle, WRITE_ENDPOINT1)
interrupt_write(handle, WRITE_ENDPOINT2)

handle.usb_close
