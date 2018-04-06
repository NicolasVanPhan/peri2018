
#include "timer.hpp"

#define FULL  1
#define EMPTY 0

struct mailbox {
  int state;
  int val;
};

struct mailbox mb0;




struct context_T1 {
  struct mailbox    *mbox;
  struct Count_st   counter;
  int               cur_sensor_value;
} ctx_T1;

void setup_T1(struct context_T1 *context,
              struct mailbox *mbox,
              int timer_index,
              int period) {
  context->mbox = mbox;
  setup_Counter(&context->counter, timer_index, period);
}

void loop_T1(struct context_T1 *ctx, struct mailbox * mb) {
  if (waitFor(ctx->counter.timer, ctx->counter.period)) {
    ctx->cur_sensor_value = analogRead(15);
  }
  if (mb->state == EMPTY) {
    mb->val = ctx->cur_sensor_value;
    mb->state = FULL;  
  }
}



void loop_T2(struct mailbox * mb) {
  if (mb->state != FULL) return; // attend que la mailbox soit pleine
  // usage de mb->val
  mb->state = EMPTY;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  loop_T1(&mb0);
}
