#ifndef TIMER_H
#define TIMER_H

#include <SPI.h>
#include <Wire.h>

#define MAX_WAIT_FOR_TIMER 3

struct Count_st {
  int timer;                                              // numéro de timer utilisé par WaitFor
  unsigned long period;                                   // periode d'incrémentation
  int value;                                              // valeur du compteur
};

unsigned int waitFor(int timer, unsigned long period);
void setup_Counter( struct Count_st * cnt, int timer, unsigned long period);

#endif
