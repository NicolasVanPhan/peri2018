#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "libgpio.h"

#define MAXServerResquest 1024

int gpio_init   (void)
{
  void  *mmap_result;

  /* Open mapping file */
  libgpio_mmap_fd = open ("/dev/mem", O_RDWR | O_SYNC);
  if (libgpio_mmap_fd < 0)
    return -1;
  
  /* Mapping */
  mmap_result = mmap (
      NULL,
      LIBGPIO_RPI_BLOCK_SIZE,
      PROT_READ | PROT_WRITE,
      MAP_SHARED,
      libgpio_mmap_fd,
      LIBGPIO_BCM2835_GPIO_BASE );
  if (mmap_result == MAP_FAILED)
  {
    close(libgpio_mmap_fd);
    return -1;
  }

  /* Setup the base adress */
  libgpio_base = (uint32_t volatile*)mmap_result;
  return 0;
}

int gpio_setup  (int gpio, int direction)
{
  uint32_t volatile* ptr = libgpio_base;

  if (direction == LIBGPIO_OUTPUT)   /* output */
  {
    //conf_func_reg &= gpio_conf_func_input_mask
    *( (ptr) + ( (gpio) / 10 ) )    &=    ( ~( 0x7 << ( ( (gpio) % 10 ) * 3 ) ) );
    //conf func reg |= conf_funk_output_mask
    *( (ptr) + ( (gpio) / 10 ) )    |=    ( 0x1 << ( ( (gpio) % 10 ) * 3 ) );
  }

  else if (direction == LIBGPIO_INPUT)  /* input */
  {
    //conf_func_reg &= gpio_conf_func_input_mask
    *( (ptr) + ( (gpio) / 10 ) )    &=    ( ~( 0x7 << ( ( (gpio) % 10 ) * 3 ) ) );
  }
  return 0;
}

int gpio_read   (int gpio, int *val)
{
  uint32_t volatile* ptr = libgpio_base;
  uint32_t gpio_conf_reg = *( (ptr) + ( ( (0x34) / sizeof( uint32_t ) ) + ( (gpio) / 32 ) ) );
  *val = ( ( gpio_conf_reg >> ( (gpio) % 32 ) ) & 0x1 );
  return 0;
}

int gpio_write  (int gpio, int val)
{
  uint32_t volatile* ptr = libgpio_base;
  uint32_t volatile* gpio_set_reg_ptr = ( (ptr) + ( ( (0x1c) / sizeof( uint32_t ) ) + ( (gpio) / 32 ) ) );
  uint32_t volatile* gpio_clr_reg_ptr = ( (ptr) + ( ( (0x28) / sizeof( uint32_t ) ) + ( (gpio) / 32 ) ) );

  // gpio set
  if (val == 0)
    *gpio_clr_reg_ptr = 1 << ( (gpio) % 32 );
  else
    *gpio_set_reg_ptr = 1 << ( (gpio) % 32 );
  return 0;
}




int main()
{
    int     f2s, s2f;                                       // fifo file descriptors
    char    *f2sName = "/tmp/f2s_aggounphan";                       // filo names
    char    *s2fName = "/tmp/s2f_aggounphan";                       //
    char    serverRequest[MAXServerResquest];               // serverResponseer for the request
    fd_set  rfds;                                           // flag for select
    int	    nfds;
    struct  timeval tv;                                     // timeout
    tv.tv_sec = 1;                                          // 1 second
    tv.tv_usec = 0;                                         //
    int     btnValue;
    char    serverResponse[3] = "0\n";

    mkfifo(s2fName, 0666);                                  // fifo creation
    mkfifo(f2sName, 0666);

    /* open both fifos */
    s2f = open(s2fName, O_RDWR);                            // fifo openning
    f2s = open(f2sName, O_RDWR);

    /* Init gpio things */
    gpio_init();
    gpio_setup(LIBGPIO_LED0, LIBGPIO_OUTPUT);
    gpio_setup(LIBGPIO_LED1, LIBGPIO_OUTPUT);
    gpio_setup(LIBGPIO_BTN0, LIBGPIO_INPUT);

    do {
        FD_ZERO(&rfds);                                     // erase all flags
        FD_SET(s2f, &rfds);                                 // wait for s2f
        FD_SET(STDIN_FILENO, &rfds);                        // wait for stdin

	nfds = s2f > STDIN_FILENO ? s2f : STDIN_FILENO;
	nfds++;
        if (select(nfds, &rfds, NULL, NULL, &tv) != 0) {   // wait until timeout
	    // If there's something coming from the pipe
            if (FD_ISSET(s2f, &rfds)) {                     // something to read
                int nbchar;
                if ((nbchar = read(s2f, serverRequest, MAXServerResquest)) == 0) break;
                serverRequest[nbchar]=0;
		printf("Receive : %s\n", serverRequest);

		// Set the LED state
		if (((int)serverRequest[2] - '0') & 0x1)
			gpio_write(LIBGPIO_LED0, 1);
		else
			gpio_write(LIBGPIO_LED0, 0);

		if (((int)serverRequest[2] - '0') & 0x2)
			gpio_write(LIBGPIO_LED1, 1);
		else
			gpio_write(LIBGPIO_LED1, 0);

		// Return the button state to the client
		//gpio_read(LIBGPIO_BTN0, &btnValue);
		//serverResponse[0] = btnValue + '0';
                write(f2s, serverResponse, 50);
            }
        }
    }
    while (1);

    close(f2s);
    close(s2f);

    return 0;
}
