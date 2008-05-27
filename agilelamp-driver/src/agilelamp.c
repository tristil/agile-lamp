#include <usb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// I should acknowledge the USB missile launcher code that served as an initial
// template. Need to find the URL first...

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
  usb_resetep(launcher, 0);
  usb_resetep(launcher, 1);
  usb_clear_halt(launcher, 0);
  usb_clear_halt(launcher, 1);
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

/*
   Fun with bit-math:

   Everything on the first (from right) 8 bits of first (from right) byte 

b0:=1:Bell active;=0:Bell off
b1:=1:Green led on; =0:Green led off
b2:=1:Red led on;=0:red led off
b3:=0:7 colors led on;=1:7 colors led off

b0
1    1    1   1   1  111 
128  64   32  16  8  421 = 255

c    c    c   c   c  rgb  (c's are colors to cycle through, then red, green, bell)

0    0    0  16  0  000 =

*/

void only_bell_on(){
  char message_part_1[8] = {0,0,0,0, 0,0,0,0};
  char message_part_2[8] = {0,0,0,0, 0,0,0,1};
  send_lamp_command("green", message_part_1, message_part_2);
}

void red_with_no_bell(){
  char message_part_1[8] = {0,0,0,0, 0,0,0,0};
  char message_part_2[8] = {0,0,0,0, 0,0,0,4};
  send_lamp_command("red with no bell", message_part_1, message_part_2);
}

void set_green(){
  char message_part_1[8] = {0,0,0,0, 0,0,0,0};   //green
  char message_part_2[8] = {0,0,0,0 ,0,0,0,2};     // green 
  send_lamp_command("green", message_part_1, message_part_2);
}

void set_lights_off(){
  char message_part_1[8] = {0,0,0,0, 0,0,0,0};  // lights off
  char message_part_2[8] = {0,0,0,0, 0,0,0,0};  // lights off 
  send_lamp_command("lights off", message_part_1, message_part_2);
}

void set_colors_with_bell(){
  char message_part_1[8] = {0,0,0,0, 0,0,0,0};       //colors with bell
  char message_part_2[8] = {0,0,0,0, 0,0,0,24};       // colors with bells
  send_lamp_command("colors with bells", message_part_1, message_part_2);
}

int claim_interface(int num){
  int claimed = -1;
  //do stuff

  claimed = usb_claim_interface(launcher, num);
  printf("Interface %d claimed with %d\n", num, claimed);

  //usb_detach_kernel_driver_np(launcher, 1);
  //usb_detach_kernel_driver_np(launcher, 0);
  printf("%d <= 0: %s\n", claimed, claimed <= 0);

  if (claimed <= 0)
  {
    printf("Preparing to release interface %d\n", num);
    usb_release_interface(launcher, num);
    printf("Couldn't claim interface %d \n", num);
    usb_close(launcher);
    return 1;
  } else if (claimed > 0){
    printf("Claimed interface %d \n", num);
  }
  return 0;
}

int load_device(){
  int ret;

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

  if(launcher == NULL){ 
    printf("Didn't get launcher!\n");
    return 1;
  } else {
    printf("Got launcher...\n");
  }

  ret = claim_interface(0);
  printf("Interface 0 returned %d\n", ret);
  if(ret == 1){
    printf("Returning 1 in claim_interface\n");
    return 1;
  }

  ret = claim_interface(1);
  printf("Interface 1 returned %d\n", ret);
  if(ret == 1){
    printf("Returning 1 in claim_interface\n");
    return 1;
  }

  fprintf(stderr, "Claimed interfaces\n");
  return 0;
}

void lamp_shutdown(){
  int ret = usb_release_interface(launcher, 0);
  printf("Released interface 0: %d\n", ret);
  ret = usb_release_interface(launcher, 1);
  printf("Released interface 1: %d\n", ret);
  usb_close(launcher);
}

int main(int argc, char *argv[])
{
  if ( argc != 2 ) /* argc should be 2 for correct execution */
  {
    /* We print argv[0] assuming it is the program name */
    printf( "usage: %s red|green|bell|various\n", argv[0] );
  }
  else 
  {
    int ret = load_device();
    if(ret == 1)
      lamp_shutdown();
    return 1;
    char* argument = argv[1];
    if(strcmp(argument, "green") == 0){
      set_green();
    } else if (strcmp(argument, "red") == 0){
      red_with_no_bell();
    } else if (strcmp(argument, "off") == 0){
      set_lights_off();
    } else if (strcmp(argument, "various") == 0){
      set_colors_with_bell();
    } else if (strcmp(argument, "bell") == 0){
      only_bell_on();
    }
  }
  lamp_shutdown();
  return 0;
}
