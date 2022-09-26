/*
  BMP280_SPI_ESP8266_Toe_Beans

  Created by: Catherine Willis
  Created: 29/07/22
  Edited: 29/07/22

  Edited: 02/09/22 : Removed all commented out out of date code to declutter. Added Pin D4 to output either high or low signal on 
                     contact or release respectivly.

  Co-authored Tom Rooney
  Edited: 25/08/22

*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK  (14)
#define BMP_MISO (12)
#define BMP_MOSI (13)
#define BMP_CS   (15)
// MAX ROC is maximum rate of change values to average, is this even a correct way to do ROC?
#define MAXROC 10
#define CALIBRATION_OFFSET 101200
// Change the threshold for adjust sensitivity
#define CONTACT_THRESHOLD 200

Adafruit_BMP280 bmp1(BMP_CS); // hardware SPI

float contacts1 = 0;

float pressure = 0; // current pressure reading
float pressurelast[MAXROC]; // array to store previous readings
float ROCValue = 0;
float ROCDiff = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  //Checking in sensors detected
  Serial.println("Starting BMP280 device 1...");

  if (!bmp1.begin()) {
    Serial.println("Sensor BMP280 device 1 was not found.");
    while (1);
  }
  Serial.println("Initialize BMP280 1 completed.");
  delay(2000);

  //Init pin to low (No contact)
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);
}


void loop() {
  // Collect pressure sensor readings
  pressure = bmp1.readPressure();

  //ignore outliers, ask for a new value from the sensor. are these numbers caused by a loose connection?
  if (( pressure < 120000) && ( pressure > 96000)) {
    delay(50); // limit our update rate, do something better than this later

    // add the last value to the array, and shuffle them by 1 in the loop so we keep a the last MAXROC amount of values
    pressurelast[0] = pressure;
    for (int i = (MAXROC - 1); i--; i > 1) {
      pressurelast[i] = pressurelast[(i - 1)];
    }

    // addall the values in the array up, then divide by MAXROC which will get us the mean average.
    ROCValue = 0;
    for (int i = (MAXROC - 1); i--; i > 1) {
      ROCValue = pressurelast[i] + ROCValue;
    }
    ROCValue = ROCValue / (MAXROC - 1);


    //now whats the differece between our average and current value
    ROCDiff = ROCValue - pressure;

    if (ROCDiff > CONTACT_THRESHOLD) {
      Serial.print("Contact Negative");
      digitalWrite(2, HIGH); //On release, set pin D4 to high (test sending signal)
    }
    if (ROCDiff < -CONTACT_THRESHOLD) {
      Serial.print("Contact Positve");
      digitalWrite(2, LOW); //On contact, set pin D4 to low (test sending signal)
    }
    Serial.print(" :Diff: ");
    Serial.println(ROCDiff);
  }
  else {
    delay(1);
  }



}
