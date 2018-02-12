#include <Process.h>
#include <Wire.h>
#include "EasyTransferI2C.h"
#include <FileIO.h>

//create object
EasyTransferI2C ET;

struct RECEIVE_DATA_STRUCTURE {
  int x;
  int y;
  int z;
  int alert;
};

//give a name to the group of data
RECEIVE_DATA_STRUCTURE mydata;

//define slave i2c address
#define I2C_SLAVE_ADDRESS 5

void setup() {
  Bridge.begin();   // Initialize the Bridge
//  FileSystem.begin();
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

  while (time.available() > 0) {
    char c = time.read();
    if (c != '\n')
      result += c;
  }
  return result;
}

void loop() {


  //check and see if a data packet has come in.
  if (ET.receiveData()) {

    Serial.println("I2c received");

    String createdAt;
    createdAt += millis();

    Process request;
    request.runShellCommandAsynchronously("curl -k -X POST https://foalarm.firebaseio.com/activity/V4AD8dduE7KBovFXJmvk.json -d '{ \"id\" : \"V4AD8dduE7KBovFXJmvk\",  \"createdAt\" : " + createdAt + ", \"x\" : " + String(mydata.x) + ", \"y\" : " + String(mydata.y) + ", \"z\" : " + String(mydata.z) + " }'");
    while (request.running());

    String dataString;
    dataString += getTimeStamp();
    dataString += ",";
    dataString += String(mydata.x);
    dataString += ",";
    dataString += String(mydata.y);
    dataString += ",";
    dataString += String(mydata.z);

    // if alert flg is true
    if (mydata.alert == 1) {
      Process p;
      p.runShellCommand("curl -k -X POST https://foalarm.firebaseio.com/data/V4AD8dduE7KBovFXJmvk.json -d '{ \"id\" : \"V4AD8dduE7KBovFXJmvk\", \"x\" : " + String(mydata.x) + ", \"y\" : " + String(mydata.y) + ", \"z\" : " + String(mydata.z) + " }'");
      while (p.running());
    }

//    File dataFile = FileSystem.open("/mnt/sd/datalog.csv", FILE_APPEND);
//
//    if (dataFile) {
//      dataFile.println(dataString);
//      Serial.println(dataString);
//      dataFile.close();
//    }

    mydata.alert = 0;

  }

}

void receive(int numBytes) {}

