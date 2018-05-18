
#include "timer.hpp"
#include "sender.hpp"

/* ---------- Thread Context Declarations ---------- */
struct context_sender  ctxSender;

/* ---------- SETUP -------------------------------- */
void setup() {
  setup_sender(&ctxSender, 0, 1000000);
}

/* ---------- LOOP --------------------------------- */
void loop() {
  loop_sender(&ctxSender);
  loop_reader();
}
