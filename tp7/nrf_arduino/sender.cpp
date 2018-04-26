
#include "sender.hpp"

RF24 radio(9,10);
byte addresses[][6] = {"02048", "02049"};
char buff[5];
char buff2[20];

void setup_sender(struct context_sender *ctx, int timer_index, unsigned long period) {
  setup_Counter(&ctx->counter, timer_index, period);

  radio.begin();
  radio.setRetries(15,15);
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);
  radio.printDetails();
  radio.startListening();
}

void loop_sender(struct context_sender *ctx) {
  
  /* If another 500ms has passed */
  if (waitFor(ctx->counter.timer, ctx->counter.period)) {
    /* read the sensor */
    ctx->curSensorValue = analogRead(15);
    /* Write the sensor value */
    Serial.println(ctx->curSensorValue);
    itoa(ctx->curSensorValue, buff, 10);
    radio.stopListening();
    radio.write(buff, sizeof(buff), 1);
    radio.startListening();
  }
}

void loop_reader() {
  if (radio.available()) {
    radio.read(buff2, sizeof(buff2));
    Serial.println(buff2);
  }
}

