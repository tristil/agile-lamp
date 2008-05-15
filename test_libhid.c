#include <hid.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> /* for getopt() */

bool match_serial_number(struct usb_dev_handle* usbdev, void* custom, unsigned int len)
{
  bool ret;
  char* buffer = (char*)malloc(len);
  usb_get_string_simple(usbdev, usb_device(usbdev)->descriptor.iSerialNumber,
      buffer, len);
  ret = strncmp(buffer, (char*)custom, len) == 0;
  free(buffer);
  return ret;
}

void set_debug(){
  /* see include/debug.h for possible values */
  hid_set_debug(HID_DEBUG_WARNINGS);
  hid_set_debug_stream(stderr);
  /* passed directly to libusb */
  hid_set_usb_debug(0);
}

int do_hid_init(){
  hid_return ret;
  fprintf(stderr, "Before hid_init\n");
  ret = hid_init();
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_init failed with return code %d\n", ret);
    return 1;
  }
  fprintf(stderr, "After hid_init\n");
  return 0;
}

int force_open_interface(HIDInterface * hid, HIDInterfaceMatcher matcher, int iface_num){
  hid_return ret;
  ret = hid_force_open(hid, iface_num, &matcher, 5);
  fprintf(stderr, "After hid_force_open %d\n", iface_num);
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_force_open failed with return code %d\n", ret);
    return 1;
  }
  return 0; 
}



int main(int argc, char *argv[])
{
  HIDInterface* hid_interface_0;
  HIDInterface* hid_interface_1;
  hid_return ret;

  unsigned short vendor_id  = 0x1130;
  unsigned short product_id = 0x0202;
  char *vendor, *product;

  int flag;

  HIDInterfaceMatcher matcher = { vendor_id, product_id, NULL, NULL, 0 };

  set_debug();  
  if(do_hid_init() == 1)
      return 1;

  hid_interface_0 = hid_new_HIDInterface();
  if (hid_interface_0 == 0) {
    fprintf(stderr, "hid_new_HIDInterface() failed, out of memory?\n");
    return 1;
  }
  fprintf(stderr, "After created new HIDInterface hid_interface_0 \n");

  hid_interface_1 = hid_new_HIDInterface();
  if (hid_interface_1 == 0) {
    fprintf(stderr, "hid_new_HIDInterface() failed, out of memory?\n");
    return 1;
  }
  fprintf(stderr, "After created new HIDInterface hid_interface_1\n");

   if(force_open_interface(hid_interface_0, matcher, 0) == 1)
        return 1;
   if(force_open_interface(hid_interface_1, matcher, 1) == 1)
        return 1;
 
   char write_cbw_packet[8] = {0x55, 0x53, 0x42, 0x43, 0, 16, 2,0};
   unsigned char const PATHLEN = 1;
   int const PATH_IN[1]  =  { 4194304 };
   int const PATH_OUT[3] = { 0xffa00001, 0xffa00002, 0xffa10004 };
 
   fprintf(stderr, "Before hid_set_output_report\n");
   ret = hid_set_output_report(hid_interface_1, PATH_IN, PATHLEN, "", 8);
   ret = hid_interrupt_write(hid_interface_0, 0x81, write_cbw_packet, 8, 1000); 
   ret = hid_set_output_report(hid_interface_0, PATH_IN, PATHLEN, "", 16);

   printf("\n%d\n\n", ret); // Prints out '0'



   char green_light[16] = {0x5a,0x5a,0x5a,0x5a,
                           0x5a,0x5a,0x5a,0x5a,
                           0x5a,0x5a,0x5a,0x5a,
                           0x5a,0x5a,0x5a,0x5a};


   fprintf(stderr, "Before hid_interrupt_write again:\n");
   ret = hid_interrupt_write(hid_interface_0, 0x81, green_light, 16, 1000); 
   printf("\n%d\n", ret); // Prints out '21' 

  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_cleanup failed with return code %d\n", ret);
    return 1;
  }

  ret = hid_close(hid_interface_0);
  ret = hid_close(hid_interface_1);

  hid_delete_HIDInterface(&hid_interface_0);
  hid_delete_HIDInterface(&hid_interface_1);

  ret = hid_cleanup();
  
  return 0;
}

/* COPYRIGHT --
 *
 * This file is part of libhid, a user-space HID access library.
 * libhid is (c) 2003-2005
 *   Martin F. Krafft <libhid@pobox.madduck.net>
 *   Charles Lepple <clepple@ghz.cc>
 *   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
 * and distributed under the terms of the GNU General Public License.
 * See the file ./COPYING in the source distribution for more information.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
 * OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
