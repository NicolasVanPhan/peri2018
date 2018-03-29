
#ifndef LIBGPIO_H
#define LIBGPIO_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define LIBGPIO_BCM2835_PERIPH_BASE   0x20000000
#define LIBGPIO_BCM2835_GPIO_BASE     (LIBGPIO_BCM2835_PERIPH_BASE + 0x200000)
#define LIBGPIO_RPI_BLOCK_SIZE        4096

#define LIBGPIO_LED0   4
#define LIBGPIO_LED1   17
#define LIBGPIO_BTN0   18

#define LIBGPIO_INPUT   0
#define LIBGPIO_OUTPUT  1

#define LIBGPIO_HIGH    1
#define LIBGPIO_LOW     0

int gpio_init   (void);
int gpio_setup  (int gpio, int direction);
int gpio_read   (int gpio, int *val);
int gpio_write  (int gpio, int val);
 
uint32_t volatile*  libgpio_base;
int                 libgpio_mmap_fd;

#endif
