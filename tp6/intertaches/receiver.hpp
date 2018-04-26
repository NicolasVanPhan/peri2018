
#ifndef H_RECEIVER
#define H_RECEIVER

#include "timer.hpp"
#include "mailbox.hpp"

struct context_receiver {
  struct mailbox    *mBox;
  struct Count_st   counter;
  long              blinkPeriod;    /* How many timer periods in a blink period */
  long              blinkCountDown; /* Counts down until a blink period has passed */
  int               ledVal;         /* The current LED state (HIGH or LOW) */
};

void setup_receiver(struct context_receiver *ctx, struct mailbox *mBox,
                    int timer_index, unsigned long timer_period);\
void loop_receiver(struct context_receiver *ctx);

#endif
