
#include "receiver.hpp"

void setup_receiver(struct context_receiver *ctx, struct mailbox *mBox,
                    int timer_index, unsigned long timer_period)
{
  ctx->mBox = mBox;
  setup_Counter(&ctx->counter, timer_index, timer_period);
  ctx->ledVal = 0;
  ctx->blinkPeriod = 0;
  ctx->blinkCountDown = 0;
  pinMode(13, OUTPUT);
}


void loop_receiver(struct context_receiver *ctx) {

  /* Blink */
  /* If a timer period has passed */
  if (waitFor(ctx->counter.timer, ctx->counter.period)) {
    ctx->blinkCountDown -= ctx->counter.period / 1000;  /* Update the countdown */
    /* If a blink period has passed */
    if (ctx->blinkCountDown < 0) {
      ctx->blinkCountDown = ctx->blinkPeriod;   /* Reset the countdown */
      ctx->ledVal = 1 - ctx->ledVal;            /* Toggle the LED value */
      digitalWrite(13, ctx->ledVal);
    }
  }
  
  /* If a new sensor value arrived, update the blinking period */
  if (ctx->mBox->state == FULL) {
    ctx->blinkPeriod = ctx->mBox->val * 5;      /* Set the new blink period */
    ctx->mBox->state = EMPTY;
  }
}
