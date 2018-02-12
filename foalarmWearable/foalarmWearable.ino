#include <SPI.h>
#include <RH_RF22.h>
#include <Wire.h>
#include "BMA250.h"

// Singleton instance of the radio driver
RH_RF22 rf22(7, 3);
// Singleton instance of the accelometer dirver
BMA250 accel;

int timerStart = 0;
int alertSet = 0;
long int timer = 0;
long int wait = 0;
int foalingAlarm = 0;

struct dataStruct {
  int x;
  int y;
  int z;
  int alert;
} sensorReadings;

// RF communication, Dont put this on the stack:
byte buf[sizeof(sensorReadings)] = {0};

void setup()
{
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  Wire.begin();
  accel.begin(BMA250_range_2g, BMA250_update_time_64ms);

  sensorReadings.x = 0;
  sensorReadings.y = 0;
  sensorReadings.z = 0;
  sensorReadings.alert = 0;
  
}

void resetAlertFlags() {
  int timerStart = 0;
  int alertSet = 0;
  long int timer = 0;
  long int wait = 0;
  sensorReadings.alert = 0;
}

void loop()
{
  // Read the accelerometer
  accel.read();

  // Check the x-axis
  if ((accel.X < -210) || (accel.X > 220)) {

    // if alert flag is false continue
    if (alertSet == 0) {

      // If the timer has not started
      if (timerStart == 0) {
        // Start the timer
        timer = millis();
        // Set the timer flag to true
        timerStart = 1;
      }
      // The timer has started
      else if (timerStart == 1) {
        // Check the time difference
        if (millis() > timer + 10000) {
          // Unset the timer
          timerStart = 0;
          // Set the foalingAlarm flag in the accel data struct to true
          // ...
          sensorReadings.alert = 1;
          // Set alert flag to true
          wait = millis();
          alertSet = 1;
        }
      }
    }
  }
  else {
    resetAlertFlags();
  }

  Serial.println(accel.X);
  Serial.println(accel.Y);
  Serial.println(accel.Z);

  Serial.println("Sending to rf22_server");

  sensorReadings.x = accel.X;
  sensorReadings.y = accel.Y;
  sensorReadings.z = accel.Z;

  // Send a message to rf22_server
  memcpy(buf, &sensorReadings, sizeof(sensorReadings) );
  byte zize = sizeof(sensorReadings);

  rf22.send((uint8_t *)buf, zize);

  // Keep sending for 500 millis
  rf22.waitPacketSent();  

  if (alertSet == 1) {
    // Wait for two minutes before sending any more alerts.
    if (millis() > wait + 120000) {
      resetAlertFlags();
    }
  }

  sensorReadings.alert = 0;
  
  delay(8000);
}

