/*
 * This file is a template for the first Lab in the SESI-PERI module.
 *
 * It draws inspiration from Dom & Gert code on ELinux.org.
 *
 * Authors:
 *   Julien Peeters <julien.peeters@lip6.fr>
 */

/*
 * Headers that are required for printf and mmap.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "libgpio.h"

/*
 * Base of peripherals and base of GPIO controller.
 */

#define BCM2835_PERIPH_BASE     0x20000000
#define BCM2835_GPIO_BASE       ( BCM2835_PERIPH_BASE + 0x200000 )

/*
 * Paging definitions.
 */

#define RPI_PAGE_SIZE           4096
#define RPI_BLOCK_SIZE          4096

/*
 * Helper macros for accessing GPIO registers.
 */

#define GPIO_CONF_FUNC_REG( ptr, gpio ) \
    *( (ptr) + ( (gpio) / 10 ) ) 

#define GPIO_CONF_FUNC_INPUT_MASK( gpio ) \
    ( ~( 0x7 << ( ( (gpio) % 10 ) * 3 ) ) ) 

#define GPIO_CONF_FUNC_OUTPUT_MASK( gpio ) \
    ( 0x1 << ( ( (gpio) % 10 ) * 3 ) ) 

#define GPIO_CONF_AS_INPUT( ptr, gpio ) \
    GPIO_CONF_FUNC_REG( ptr, gpio ) &= GPIO_CONF_FUNC_INPUT_MASK( gpio ) 

#define GPIO_CONF_AS_OUTPUT( ptr, gpio ) \
    do { \
        GPIO_CONF_AS_INPUT( ptr, gpio ); \
        GPIO_CONF_FUNC_REG( ptr, gpio ) |= \
            GPIO_CONF_FUNC_OUTPUT_MASK( gpio ); \
    } while ( 0 ) 

#define GPIO_CONF_REG( ptr, addr, gpio ) \
    *( (ptr) + ( ( (addr) / sizeof( uint32_t ) ) + ( (gpio) / 32 ) ) ) 

#define GPIO_SET_REG( ptr, gpio ) \
    GPIO_CONF_REG( ptr, 0x1c, gpio )

#define GPIO_CLR_REG( ptr, gpio ) \
    GPIO_CONF_REG( ptr, 0x28, gpio )

#define GPIO_SET( ptr, gpio ) \
    GPIO_SET_REG( ptr, gpio ) = 1 << ( (gpio) % 32 ) 

#define GPIO_CLR( ptr, gpio ) \
    GPIO_CLR_REG( ptr, gpio ) = 1 << ( (gpio) % 32 )

#define GPIO_VALUE( ptr, gpio ) \
    ( ( GPIO_CONF_REG( ptr, 0x34, gpio ) >> ( (gpio) % 32 ) ) & 0x1 ) 

/*
 * Setup the access to memory-mapped I/O.
 */

static int mmap_fd;

int
setup_gpio_mmap ( uint32_t volatile ** ptr )
{
    void * mmap_result;

    mmap_fd = open ( "/dev/mem", O_RDWR | O_SYNC );

    if ( mmap_fd < 0 ) {
        return -1;
    }

    mmap_result = mmap (
        NULL
      , RPI_BLOCK_SIZE
      , PROT_READ | PROT_WRITE
      , MAP_SHARED
      , mmap_fd
      , BCM2835_GPIO_BASE );

    if ( mmap_result == MAP_FAILED ) {
        close ( mmap_fd );
        return -1;
    }

    *ptr = ( uint32_t volatile * ) mmap_result;

    return 0;
}

void
teardown_gpio_mmap ( void * ptr )
{
    munmap ( ptr, RPI_BLOCK_SIZE );
}

void
delay ( unsigned int milisec )
{
    struct timespec ts, dummy;
    ts.tv_sec  = ( time_t ) milisec / 1000;
    ts.tv_nsec = ( long ) ( milisec % 1000 ) * 1000000;
    nanosleep ( &ts, &dummy );
}

#define min( a, b ) ( a < b ? a : b )
#define max( a, b ) ( a < b ? b : a )

/*
 * Main program.
 */

#define GPIO_LED0   4
#define GPIO_LED1   17 

#define GPIO_BTN0   18

int
main ( int argc, char **argv )
{
    int                 result;
    int                 period, half_period;
    uint32_t volatile * gpio_base = 0;
    int                 val_nouv = 1;
    int                 val_prec = 1;
    int                 BP_ON = 0;
    int                 BP_OFF = 0;

    /* Retreive the mapped GPIO memory. */
    /*result = setup_gpio_mmap ( &gpio_base );
    libgpio_base = gpio_base;*/
    gpio_init();
    gpio_base = libgpio_base;

    if ( result < 0 ) {
        printf ( "-- error: cannot setup mapped GPIO.\n" );
        exit ( 1 );
    }

    period = 200; /* default = 1Hz */
    if ( argc > 1 ) {
        period = atoi ( argv[1] );
    }
    half_period = period / 2;

    /* Setup GPIO of LED0 to output. */
    gpio_setup(LIBGPIO_LED0, LIBGPIO_OUTPUT);
    gpio_setup(LIBGPIO_LED1, LIBGPIO_OUTPUT);
    gpio_setup(LIBGPIO_BTN0, LIBGPIO_INPUT);

    /*GPIO_CONF_AS_OUTPUT ( gpio_base, GPIO_LED0 );
    GPIO_CONF_AS_OUTPUT ( gpio_base, GPIO_LED1 );
    GPIO_CONF_AS_INPUT ( gpio_base, GPIO_BTN0 );*/

    printf ( "-- info: start blinking.\n" );

    /* Blink led at frequency of 1Hz. */
    /*while (1) {
        GPIO_SET ( gpio_base, GPIO_LED0 );
        GPIO_CLR ( gpio_base, GPIO_LED1 );
        delay ( half_period );
        GPIO_CLR ( gpio_base, GPIO_LED0 );
        GPIO_SET ( gpio_base, GPIO_LED1 );
        delay ( half_period );
    }*/

    /* Diplay BTN0 value on loop */
    while (1) {
      delay(20);
      //val_nouv = GPIO_VALUE( gpio_base, GPIO_BTN0 );
      gpio_read(LIBGPIO_BTN0, &val_nouv);

      if((val_prec != val_nouv) && (val_nouv == 0))
        BP_ON = 1;
      if((val_prec != val_nouv) && (val_nouv == 1))
        BP_OFF = 1;

      if(BP_ON)
      {
        BP_ON = 0;
        printf(":))\n");
      }
      if(BP_OFF)
      {
        BP_OFF = 0;
        printf("nicolas\n");
      } 
      if (val_nouv == 1)
        gpio_write(LIBGPIO_LED0, 1);
      gpio_write(LIBGPIO_LED1, 0);
      delay ( half_period );
      val_prec = val_nouv;
      gpio_write(LIBGPIO_LED0, 0);
      gpio_write(LIBGPIO_LED1, 1);
      delay ( half_period );
    }

    return 0;
}
