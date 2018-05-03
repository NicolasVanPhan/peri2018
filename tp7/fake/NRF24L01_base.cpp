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

RF24 radio(15,8); // radio(CE,CS)

byte addresses[][6] = {"02048", "02049"};

ofstream myfile;

void setup() {
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.printDetails();
  radio.startListening();
  myfile.open("my_db.dat", ios::ate);
}

char mess[16];
char buffer[32];
char buffer2[32];
int  myhint;
unsigned long timer;

void loop() {
  //cout << "." << flush;
  //sprintf(buffer,"%s %d", mess, timer);
  if (radio.available()) {
	  radio.read(buffer, sizeof(buffer));
	  cout << buffer << std::endl;
	  myfile << buffer << ";" << time(NULL) << std::endl;
	  // don't forget to flush, just in case
	  // and one day you'll have to close the file...
	  myhint = atoi(buffer);
	  sprintf(buffer2, "%d", myhint + 1000);

	  //buffer2 << "toto : " << buffer;
	  radio.stopListening();
	  radio.write(buffer2, 32);
	  radio.startListening();
  }
  //radio.write( buffer, sizeof(buffer) );
  //timer++;  
  //sleep(1);
}

int main(int argc, char **argv)
{
    if (argc > 1) strcpy(mess, argv[1]);
    setup();
    while (1)
        loop();
    return 0;
}
