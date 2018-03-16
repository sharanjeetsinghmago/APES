#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdint.h>

int fd;

char *bus = "/dev/i2c-2"; /* Pins P9_19 and P9_20 */
  int addr = 0x48;          /* The I2C address of TMP102 */
  char buf[2] = {0},pika=0x00;
  int temp;
  unsigned char MSB, LSB;

  float f,c;

typedef enum 
{
  TEMP_CELCIUS,
  TEMP_KELVIN,
  TEMP_FAHRENHEIT
}temp_format;

void signal_handler(int signum)
{

  assert(0 == close(fd));

  exit(signum);

}

void write_pointer_reg(uint8_t value)
{
  if(write(fd, &value, 1) != 1)
  {
    perror("Write Pointer Register Error\n");
  }
}

void read_temp_data_reg(void)
{

  write_pointer_reg(0b00000000);

  int x = read(fd,&buf,2);

  printf("number of bytes read = %d\n",x);

  if (x != 2) 
  {
    /* ERROR HANDLING: i2c transaction failed */
    perror("Failed to read from the i2c bus.\n");
    printf("ERROR : %s\n", strerror(errno));
  }
  else 
  {

    MSB = buf[0];
    LSB = buf[1];

    /* Convert 12bit int using two's compliment */
    /* Credit: http://bildr.org/2011/01/tmp102-arduino/ */
    temp = ((MSB << 8) | LSB) >> 4;

    c = temp*0.0625;
    f = (1.8 * c) + 32;

    printf("Temp Fahrenheit: %f Celsius: %f\n", f, c);
  }

}

int temp_init()
{
  if((fd = open(bus, O_RDWR)) < 0)
  {
    perror("Failed to open the i2c bus");
    /* ERROR HANDLING: you can check errno to see what went wrong */
    return -1;
  }

  if(ioctl(fd, I2C_SLAVE, addr) < 0)
  {
    perror("Failed to open the i2c bus");
    /* ERROR HANDLING: you can check errno to see what went wrong */
    return -1;
  }

  return 0;
}

void get_temp()
{


  
  printf("\n\n In the Get Temp Function \n\n");

  //for(long long i=0;i<1000000000;i++)

 

 /* Register the signal handler */
 //signal(SIGINT, signal_handler);

 printf(" fd = %d\n",fd);

 while(1)
   {
   	/*int x = write(fd,&pika,1);
   	printf("number of bytes written = %d\n",x);
   	 if (x != 1)
   	 {
   	 	/* ERROR HANDLING: i2c transaction failed 
   	 	printf("Failed to write to the i2c bus.\n");
   	 }
     
     x = write(fd,&pika,1);

     printf("number of bytes written again = %d\n",x);
     // Using I2C Read
*/
     int x = read(fd,&buf,2);

     printf("number of bytes read = %d\n",x);

     if (x != 2) {
       /* ERROR HANDLING: i2c transaction failed */
       perror("Failed to read from the i2c bus.\n");
       printf("ERROR : %s\n", strerror(errno));
	 }
	 // else {

       MSB = buf[0];
       LSB = buf[1];

       /* Convert 12bit int using two's compliment */
       /* Credit: http://bildr.org/2011/01/tmp102-arduino/ */
       temp = ((MSB << 8) | LSB) >> 4;

       c = temp*0.0625;
       f = (1.8 * c) + 32;

       printf("Temp Fahrenheit: %f Celsius: %f\n", f, c);
     //}

sleep(1);
   }


}
