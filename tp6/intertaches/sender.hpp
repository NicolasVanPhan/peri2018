
#ifndef H_SENDER
#define H_SENDER

#include "timer.hpp"
#include "mailbox.hpp"

struct context_sender {
  struct mailbox    *mBox;
  struct Count_st   counter;
  int               curSensorValue;
};

void setup_sender(struct context_sender *ctx, struct mailbox *mBox,
                  int timer_index, unsigned long period);

void loop_sender(struct context_sender *ctx);

#endif
