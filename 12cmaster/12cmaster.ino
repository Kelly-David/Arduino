#include <Wire.h>
#include <RH_RF22.h>
#include "EasyTransferI2C.h"

//create object
EasyTransferI2C ET;

RH_RF22 rf22(7, 3);

struct SEND_DATA_STRUCTURE {
  int x;
  int y;
  int z;
  int alert;
};

SEND_DATA_STRUCTURE mydata;

//define slave i2c address
#define I2C_SLAVE_ADDRESS 5

void setup() {
  Wire.begin();
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
  ET.begin(details(mydata), &Wire);

  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("init failed");
  Serial.end();

  randomSeed(analogRead(0));
}

void loop() {

  if (rf22.available()) {

    uint8_t buf[RH_RF22_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);
    if (rf22.recv(buf, &buflen)) {
      memcpy(&mydata, buf, sizeof(mydata));

      //send the data
      ET.sendData(I2C_SLAVE_ADDRESS);

    }
  }
  delay(0);
}
