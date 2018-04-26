#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <printf.h>
#include <SPI.h>

RF24 radio(9,10);

byte addresses[][6] = {"02048"};

void setup() {
  radio.begin();
  radio.setRetries(15,15);
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1,addresses[0]);
  radio.printDetails();
  radio.startListening();
  Serial.begin(9600);
}

void loop() {
  char buff[32];

  if( radio.available()){
     radio.read( buff, sizeof(buff) );             // Get the payload
     Serial.println(buff);
   }
}
