
#include "timer.hpp"
#include "sender.hpp"

/* ---------- Thread Context Declarations ---------- */
struct context_sender  ctxSender;

/* ---------- SETUP -------------------------------- */
void setup() {
  Serial.begin(9600);
  setup_sender(&ctxSender, 0, 500000);
}

/* ---------- LOOP --------------------------------- */
void loop() {
  loop_sender(&ctxSender);
  loop_reader();
}
