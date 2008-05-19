#!/usr/bin/python
#
# This is just an example. Please see the test_libhid.c file for more
# comments.
#

CBW = "/0x55/0x53/0x42/0x43/0/16/2/0/"

import sys

# allow it to run right out of the build dir
import os
libsdir = os.getcwd() + '/.libs'
if os.path.isdir(libsdir) and os.path.isfile(libsdir + '/_hid.so'):
  sys.path.insert(0, libsdir)

from hid import *

def open_interface(interface, matcher):
  hid = hid_new_HIDInterface()
  ret = hid_force_open(hid, interface, matcher, 3)
  if ret != HID_RET_SUCCESS:
    sys.stderr.write("hid_force_open %s failed with return code %d.\n" % (interface, ret))
  return hid

def close_interface(hid):
  ret = hid_close(hid)
  if ret != HID_RET_SUCCESS:
    sys.stderr.write("hid_close failed with return code %d.\n" % ret)

def main():
  hid_set_debug(HID_DEBUG_WARNINGS)
  hid_set_debug_stream(sys.stderr)
  hid_set_usb_debug(0)

  ret = hid_init()

  matcher = HIDInterfaceMatcher()
  matcher.vendor_id = 0x1130
  matcher.product_id = 0x0202

  hid_interface_1 = open_interface(1, matcher)
  hid_interface_0 = open_interface(0, matcher)
  sys.exit() 
  # Set_Report with report type output, report ID 0, interface 1, and length 8

  ret = hid_set_output_report(hid_interface_1, (), "8") 

  # OUT transaction to endpoint 0 with CBW indicates write and length.
  ret = hid_interrupt_write(hid_interface_0, 0x81, CBW, 1000);

  # Set_Report with report type output, report ID 0, interface 0, length 16.

  ret = hid_set_output_report(hid_interface_0, (), "16") 

  # OUT transaction to endpoint 0 with data and length 8.

  ret = hid_interrupt_write(hid_interface_0, 0x81, "/0xff/0xfe/0xfd/0xfc/0xfb/0xfa/0xf9/0xf9", 1000);

  # OUT transaction to endpoint 0 with data and length 8.

  ret = hid_interrupt_write(hid_interface_0, 0x81, "/0xf7/0xf6/0xf5/0xf4/0xf3/0xf2/0xf1/0xf0", 1000);

  close_interface(hid_interface_1)
  
  hid_cleanup()

if __name__ == '__main__':
  main()

# COPYRIGHT --
#
# This file is part of libhid, a user-space HID access library.
# libhid is (c) 2003-2005
#   Martin F. Krafft <libhid@pobox.madduck.net>
#   Charles Lepple <clepple@ghz.cc>
#   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
# and distributed under the terms of the GNU General Public License.
# See the file ./COPYING in the source distribution for more information.
#
# THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
# OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
