#include <usb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

usb_dev_handle* launcher;
//wrapper for control_msg
int send_message(char* msg, int index)
{
  int i = 0;
  int j = usb_control_msg(launcher, 0x21, 0x9, 0x0, index, msg, 8, 1000);
  return j;
}

void send_cbw(){
  fprintf(stderr, "Sending CBW message\n");
  char cbw_message[8] = {0x55, 0x53, 0x42, 0x43, 0, 16, 2, 0};
  int ret = send_message(cbw_message, 1);
  fprintf(stderr, "%d bytes sent\n", ret);
}

void send_lamp_command(char* title, char* message1, char* message2){
  usb_reset(launcher);
  send_cbw();
  fprintf(stderr, "Submitting %s\n", title);
  int ret = send_message(message1, 0);
  fprintf(stderr, "%d bytes sent\n", ret);
  ret = send_message(message2, 0);
  fprintf(stderr, "%d bytes sent\n", ret);
}

void set_red_with_bell(){
  char message_part_1[8] = {0x0, 0x1, 0x4, 0x9,0x10, 0x19, 0x24, 0x31};  //red with bell
  char message_part_2[8] = {0x40, 0x51, 0x64, 0x79, 0x90, 0xa9, 0xc4, 0xe1}; // red with bell
  send_lamp_command("red with bell", message_part_1, message_part_2);
}

void set_green(){
  char message_part_1[8] = {0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a};   //green
  char message_part_2[8] = {0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a};   // green 
  send_lamp_command("green", message_part_1, message_part_2);
}

void set_lights_off(){
  char message_part_1[8] = {0x0, 0x1, 0x4, 0x9,0x10, 0x19, 0x24, 0x31};  //red with bell
  char message_part_2[8] = {0x40, 0x51, 0x64, 0x79, 0x90, 0xa9, 0xc4, 0xe1}; // red with bell
  send_lamp_command("lights off", message_part_1, message_part_2);
}

void set_colors_with_bell(){
  char message_part_1[8] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};       //colors with bell
  char message_part_2[8] = {0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};       // colors with bells
  send_lamp_command("colors with bells", message_part_1, message_part_2);
}

int load_device(){
  int claimed;

  fprintf(stderr, "Starting\n");

  struct usb_bus *busses;

  usb_init();

  fprintf(stderr, "usb_init...\n"); 

  usb_find_busses();
  usb_find_devices();
  fprintf(stderr, "Found busses, found devices...\n");

  busses = usb_get_busses();

  fprintf(stderr, "Got busses...\n");

  struct usb_bus *bus;
  int c, i, a;

  /* ... */

  for (bus = busses; bus; bus = bus->next) 
  {
    struct usb_device *dev;

    for (dev = bus->devices; dev; dev = dev->next) 
    {
      /* Check if this device is a printer */
      if (dev->descriptor.idVendor == 4400)
        if(dev->descriptor.idProduct == 514)
          launcher = usb_open(dev);
    }
  }

  fprintf(stderr, "Got launcher...\n");

  //do stuff
  if(launcher != NULL)
  {
    int claimed = usb_claim_interface(launcher, 1);
  }
  else
  {
    fprintf(stderr, "You didn't really get the launcher!\n");
    return 1;
  }

  usb_detach_kernel_driver_np(launcher, 1);
  usb_detach_kernel_driver_np(launcher, 0);

  if (claimed == 0)
  {
    usb_release_interface(launcher, 1);
    return 1;
  } else if (claimed > 0){
    fprintf(stderr, "Found launcher...\n");
  }

}

int main(int argc, char *argv[])
{
  if ( argc != 2 ) /* argc should be 2 for correct execution */
  {
    /* We print argv[0] assuming it is the program name */
    printf( "usage: %s filename\n", argv[0] );
  }
  else 
  {
    int ret = load_device();
    char* argument = argv[1];
    if(strcmp(argument, "green") == 0){
      set_green();
    } else if (strcmp(argument, "red") == 0){
      set_red_with_bell();
    } else if (strcmp(argument, "off") == 0){
      set_lights_off();
    } else if (strcmp(argument, "various") == 0){
      set_colors_with_bell();
    }
  }
  return 0;
}
