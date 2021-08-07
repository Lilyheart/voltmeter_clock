// Import necessary libs

#include <RTClib.h>
#include <Wire.h>

// Factors used to derive the correct time
// sec and mins are 60 increments of 0.083v
// analog values are 255 divided by 60 = 4.25
// hours are 12 increments of 21.25
// analog hour values are 255 divided by 12 = 21.25

RTC_DS1307 rtc;
float sec_factor = (4.25);
float min_factor = (4.25);
float hrs_factor = (21.25); // This is 255 (i.e. 5v) divided by 12hrs gives 12hr time

float h = 0;
// define pinouts for relay and two push buttons
// button 1 is hours
// button 2 is mins

const int buttonPin1 = 2;
const int buttonPin2 = 4;

// Following are null/LOW values so that the user can adjust the hrs and mins

byte buttonState1;
byte buttonState2;
byte lastState1 = LOW;
byte lastState2 = LOW;
byte minsCount = 0;
byte hrsCount = 0;

void setup() {

  Serial.begin(9600);
  Wire.begin();
  rtc.begin();


  //define inputs and outputs
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);

}

void loop() {

  // float time values to be manipulated in a calculation later
  DateTime now = rtc.now();
  float seconds = (now.second());
  float minutes = (now.minute());
  float hours = (now.hour());
  float hours2 = (now.hour()-12);

  // Code for push button 1 (hours)

  buttonState1 = digitalRead(buttonPin1);

  if(buttonState1 && buttonState1 != lastState1)  // button latch, no debounce needed.
  {
    if(hrsCount < 12)
      hrsCount += 1;
    else
      hrsCount = 0;
    Serial.println(hrsCount);
  }

  lastState1 = buttonState1;

// Code for push button 2 (mins)

  buttonState2 = digitalRead(buttonPin2);

  if(buttonState2 && buttonState2 != lastState2)  // button latch, no debounce needed.
  {
    if(minsCount < 60)
      minsCount += 1;
    else
      minsCount = 0;
    Serial.println(minsCount);
  }

  lastState2 = buttonState2;

  // Finally write analog values to PWM pins 6, 10, and 11


    if (hours > 12)
    {
      analogWrite(6, sec_factor * seconds); // Write Vout seconds
      analogWrite(5, min_factor * (minutes + minsCount)); // Write Vout minutes
      analogWrite(3, hrs_factor * (hours2 + hrsCount));
    }
    else
    {
      analogWrite(6, sec_factor * seconds); // Write Vout seconds
      analogWrite(5, min_factor * (minutes + minsCount)); // Write Vout minutes
      analogWrite(3, hrs_factor * (hours + hrsCount));
    }
  }
