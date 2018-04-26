
#ifndef H_SENDER
#define H_SENDER

#include "timer.hpp"

#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

struct context_sender {
  //struct mailbox    *mBox;
  struct Count_st   counter;
  int               curSensorValue;
};

void setup_sender(struct context_sender *ctx, int timer_index, unsigned long period);

void loop_sender(struct context_sender *ctx);
void loop_reader();

#endif
