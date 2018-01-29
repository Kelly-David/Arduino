#include <Process.h>
#include <Wire.h>
#include "EasyTransferI2C.h"
#include <FileIO.h>

//create object
EasyTransferI2C ET;

// Timing
unsigned long int TIME;
int tSet = 0;
int aSet = 0;


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
  FileSystem.begin();
  Wire.begin(I2C_SLAVE_ADDRESS);
  ET.begin(details(mydata), &Wire);
  Wire.onReceive(receive);

  Serial.begin(9600);

  pinMode(13, OUTPUT);
  digitalWrite(3, LOW);
}

// Function getTimeStamp()
String getTimeStamp() {
  String result;
  Process time;
  time.begin("date");
  time.addParameter("+%D-%T");  
  time.run(); 

  while(time.available()>0) {
    char c = time.read();
    if(c != '\n')
      result += c;
  }

  return result;
}

void loop() {

  //check and see if a data packet has come in.
  if (ET.receiveData()) {

    String dataString;
    dataString += getTimeStamp();
    dataString += ",";
    dataString += String(mydata.x);
    dataString += ",";
    dataString += String(mydata.y);
    dataString += ",";
    dataString += String(mydata.z);

    // if alert flg is false continue
    if(aSet == 0) {

      // Check the x-axis
      if((mydata.x < -210) || (mydata.x > 220)) {
  
        // If the timer is not set
        if(tSet == 0) {
          // Start the timer
          TIME = millis();
          tSet = 1;
        } 
        else {
          // Check the time difference
          if(millis() > TIME + 8000) {
            // Run the command
            tSet = 0;
            // Run a shell command
            Process p;
            p.runShellCommand("curl -k -X POST https://foalarm.firebaseio.com/data/V4AD8dduE7KBovFXJmvk.json -d '{ \"id\" : \"V4AD8dduE7KBovFXJmvk\", \"x\" : " + String(mydata.x) + ", \"y\" : " + String(mydata.y) + ", \"z\" : " + String(mydata.z) + " }'");
            while (p.running());
            // Command was sent
            // Set alert flag to true
            aSet = 1;
          }
        }
      }
    }

    File dataFile = FileSystem.open("/mnt/sd/datalog.csv", FILE_APPEND);

    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
    }

    // Check timer to reset the alert flag
    // If alert flag is true
    if(aSet == 1) {
      if(millis() > TIME + 100000) {
        // Reset the alert flag to false
        aSet = 0;
      }
    }
  }

}

void receive(int numBytes) {}

