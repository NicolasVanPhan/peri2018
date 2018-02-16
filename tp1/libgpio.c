
#include "libgpio.h"

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

