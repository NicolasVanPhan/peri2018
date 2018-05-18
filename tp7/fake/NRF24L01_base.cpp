/**
 * PERI TP7
 * Nicolas Phan
 * Bryan Aggoun
 *
 * This program retrieves data sent by the Arduino through NRF
 * and stores it in a file/database.
 */
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>
#include <ctime>

typedef uint8_t byte;

using namespace std;

RF24 radio(15,8); // radio(CE,CS)			/* NRF communication */
byte addresses[][6] = {"02548", "02549"};	/* NRF communication channels */
ofstream myfile;							/* The database is a file */

void setup() {
	/* Initiate NRF communication and start listening to the Arduino */
  radio.begin();
  //radio.setChannel(100);
  radio.setPALevel(RF24_PA_LOW);
  //radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.printDetails();
  radio.startListening();
  /* Open the database */
  myfile.open("my_db.dat", ios::ate);
}

char mess[16];			/* Contains the first user argument (argv[1]) */
char buffer[32];		/* A buffer for retrieving the Arduino data */

void loop() {
  if (radio.available()) {	/* If we received new data from the Arduino */
	  cout << "toto" << std::endl;
	  radio.read(buffer, sizeof(buffer));	/* Retrieve it */
	  cout << buffer << std::endl;			/* Print it, just for debug */
	  myfile << buffer << ";" << time(NULL) << std::endl; /* Store it in DB */
  }
}

int main(int argc, char **argv)
{
    if (argc > 1) strcpy(mess, argv[1]);
    setup();
    while (1)
        loop();
    return 0;
}
