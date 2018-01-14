#include <Process.h>
#include <Wire.h>
#include "EasyTransferI2C.h"

//create object
EasyTransferI2C ET;

struct RECEIVE_DATA_STRUCTURE {
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int x;
  int y;
  int z;
};

//give a name to the group of data
RECEIVE_DATA_STRUCTURE mydata;

//define slave i2c address
#define I2C_SLAVE_ADDRESS 5

void setup() {
  Bridge.begin();   // Initialize the Bridge
  Wire.begin(I2C_SLAVE_ADDRESS);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
  ET.begin(details(mydata), &Wire);
  Wire.onReceive(receive);

  Serial.begin(9600);

  pinMode(13, OUTPUT);
  digitalWrite(3, LOW);
}

void loop() {

  //check and see if a data packet has come in.
  if (ET.receiveData()) {
    //this is how you access the variables. [name of the group].[variable name]
    //since we have data, we will blink it out.

    Process p;
    p.runShellCommand("curl -k -X POST https://foalarm.firebaseio.com/data/V4AD8dduE7KBovFXJmvk.json -d '{ \"id\" : \"V4AD8dduE7KBovFXJmvk\", \"x\" : " + String(mydata.x) + ", \"y\" : " + String(mydata.y) + ", \"z\" : " + String(mydata.z) + " }'");
    while (p.running());
  }

}

void receive(int numBytes) {}

