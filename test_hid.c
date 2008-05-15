#include <hid.h>

HIDInterface *hid;
hid_return ret;

HIDInterfaceMatcher matcher =
	 { 0x0ce5, 0x0003, NULL, NULL, 0 };
ret = hid_force_open(hid, 0, &matcher, 3);

int const PATH_LEN = 2;
int const PATH_IN[2] = { 0xffa00001, 0xffa00003 };

int const WRITE_PACKET_LEN = 8;
char write_packet[8] =
	 { 0x04,0x7f,0x7f,0x00,0x02,0x00,0x00,0x00 };

int const READ_PACKET_LEN = 5;
char read_packet[5];

ret = hid_set_output_report(hid,
 PATH_IN,
 PATH_LEN,
 write_packet,
 WRITE_PACKET_LEN);

ret = hid_interrupt_read(hid,
 USB_ENDPOINT_IN+1,
 read_packet,
 READ_PACKET_LEN,
 0);

