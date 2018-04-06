
#include "timer.hpp"
#include "mailbox.hpp"
#include "sender.hpp"
#include "receiver.hpp"

/* ---------- Global Declarations ------------------ */
struct mailbox mBox0;

/* ---------- Thread Context Declarations ---------- */
struct context_sender  ctxSender;
struct context_receiver ctxReceiver;

/* ---------- SETUP -------------------------------- */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setup_sender(&ctxSender, &mBox0, 0, 500000);
  setup_receiver(&ctxReceiver, &mBox0, 1, 100000);
}

/* ---------- LOOP --------------------------------- */
void loop() {
  // put your main code here, to run repeatedly:
  loop_sender(&ctxSender);
  loop_receiver(&ctxReceiver);
}
