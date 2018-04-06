
#include "sender.hpp"

void setup_sender(struct context_sender *ctx, struct mailbox *mBox,
              int timer_index, unsigned long period) {
  ctx->mBox = mBox;
  setup_Counter(&ctx->counter, timer_index, period);
}

void loop_sender(struct context_sender *ctx) {
  
  /* If another 500ms has passed, read the sensor */
  if (waitFor(ctx->counter.timer, ctx->counter.period)) {
    ctx->curSensorValue = analogRead(15);
  }
  
  /* If the mailbox is empty, write the last read value in it */
  if (ctx->mBox->state == EMPTY) {
    ctx->mBox->val = ctx->curSensorValue;
    ctx->mBox->state = FULL;
  }
  
}
