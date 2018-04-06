
/*********************************************************************
This is the main project for PERI - TME6

Authors :
Nicolas Phan <nicolas.van.phan@gmail.com>
Bryan Aggoun <bryan.aggoun@gmail.com>

2017/2018
*********************************************************************/
#include "screen.hpp"
#include "timer.hpp"

struct ctx_Screen ctx_screen;

/*********************  SETUP  **************************************/
void setup() {
  Serial.begin(9600);
  setup_Screen(&ctx_screen, 0, 1000000, 0);
}


/*********************  LOOP  ***************************************/
void loop() {
  loop_Screen(&ctx_screen);
}

