#include <RTClib.h> // https://github.com/adafruit/RTClib
#include <Wire.h>

const int secPin = 6;
const int minPin = 5;
const int hourPin = 3;
const int AMLEDPin = 11;
const int PMLEDPin = 10;

bool isSecDecay = false;
bool isMinDecay = false;
bool isHourDecay = false;
int decayPins[] = {secPin, minPin, hourPin}; // order must match isDecayPins in Decay method

bool secDecayHold = true;
bool minDecayHold = true;
bool hourDecayHold = true;

const int buttonPin1 = 8;     // the number of the pushbutton pin
const int buttonPin2 = 7;     // the number of the pushbutton pin

int buttonState1 = 0;         // variable for reading the pushbutton status
int buttonState2 = 0;         // variable for reading the pushbutton status

int hp1 = 0;
int hp2 = 0;

RTC_DS1307 rtc;

/**
  * Changes the meter over a range of values. All pins will change to the same
     value at relatively the same time
  *
  * @param int startVal   The starting value
  * @param int endVal     The ending value
  * @param int decayRate  The rate of change on each `delayTime`
                          (positive goes up, negative goes down)
  * @param int delayTime  The delay between changes
 */
void Decay(int startVal, int endVal, int decayRate, int delayTime) {
  int decay = startVal;
  bool isDecayPins[] = {isSecDecay, isMinDecay, isHourDecay};
  bool isResets[] = {secDecayHold, minDecayHold, hourDecayHold};

  // Loop through the decays
  for (int i = 0; i <= abs(startVal - endVal); i++) {
    // Loop through the pins
    for (int j = 0; j < sizeof decayPins/sizeof decayPins[0]; j++) {
      if (isDecayPins[j]) {
        if (isResets[j]) {
          analogWrite(decayPins[j], decay);
        }
      }
    }

    delay(delayTime);
    decay += decayRate;

    if (decay < 0) {
      decay = 0;
    }
  }

  secDecayHold = false;
  minDecayHold = false;
  hourDecayHold = false;
}

void InitializeButton() {
  // ## INITIALIZE BUTTON CODE

  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
}

void InitializePin() {
  // ## INITIALIZE PINS
  pinMode(AMLEDPin, OUTPUT);
  pinMode(PMLEDPin, OUTPUT);

  isSecDecay = true;
  isMinDecay = true;
  isHourDecay = true;

  digitalWrite(AMLEDPin, HIGH);
  digitalWrite(PMLEDPin, LOW);
  Decay(0, 255,  1, 10);

  secDecayHold = true;
  minDecayHold = true;
  hourDecayHold = true;

  digitalWrite(AMLEDPin, LOW);
  digitalWrite(PMLEDPin, HIGH);
  Decay(255, 0, -1, 10);

  secDecayHold = false;
  minDecayHold = false;
  hourDecayHold = false;
}

void setup() {

  Serial.begin(9600);
  Wire.begin();
  rtc.begin();

  InitializeButton();
  InitializePin();
}

/***
 *    ______                           _________     _________
 *    ___  / ____________________      __  ____/___________  /____
 *    __  /  _  __ \  __ \__  __ \     _  /    _  __ \  __  /_  _ \
 *    _  /___/ /_/ / /_/ /_  /_/ /     / /___  / /_/ / /_/ / /  __/
 *    /_____/\____/\____/_  .___/      \____/  \____/\__,_/  \___/
 *                       /_/
 */

void buttonLoop() {
//  buttonState1 = digitalRead(buttonPin1);
//  buttonState2 = digitalRead(buttonPin2);
//
//  if ((buttonState1 == HIGH) && (hp1 == 0)) {
//    Serial.println("Pressed1");
//    setTime(hour() + 1, minute(), second(), day(), month(), year());
//    Serial.println(hour());
//    hp1 = 1;
//    delay(200);
//  } else if (buttonState1 == LOW) {
//    hp1 = 0;
//  }
//
//  if ((buttonState2 == HIGH) && (hp2 == 0)) {
//    Serial.println("Pressed2");
//    setTime(hour(), minute() + 1, second(), day(), month(), year());
//    Serial.println(minute());
//    hp2 = 1;
//    delay(100);
//  } else if (buttonState2 == LOW) {
//    hp2 = 0;
//  }
}

void displayTime() {
  isSecDecay = false;
  isMinDecay = false;
  isHourDecay = false;

  // Retrieve the time from the real time clock.
  DateTime now = rtc.now();

  // ### For debuging purposes, write the time to the
  // Serial.print(now.hour());
  // Serial.print(":");
  // Serial.print(now.minute());
  // Serial.print(":");
  // Serial.println(now.second());

  if (now.second() == 0) {
    if (secDecayHold == false) {
      isSecDecay = true;
      secDecayHold = true;
    }
  } else {
    analogWrite(secPin, now.second() * (255 / 60));
  }

  if (now.minute() == 0) {
    if (minDecayHold == false) {
      isMinDecay = true;
      minDecayHold = true;
    }
  } else {
    analogWrite(minPin, now.minute() * (255 / 60));
  }

  if ((now.hour() + 1) % 12 == 0) {
    if (hourDecayHold == false) {
      isHourDecay = true;
      hourDecayHold = true;
    }
  } else {
    if (now.hour() <= 12) {
      analogWrite(hourPin, now.hour() * (255 / 12));
    } else {
      analogWrite(hourPin, (now.hour() - 12) * (255 / 12));
    }
  }

  if (now.hour() < 12) {
    digitalWrite(AMLEDPin, HIGH);
    digitalWrite(PMLEDPin, LOW);
  } else {
    digitalWrite(AMLEDPin, LOW);
    digitalWrite(PMLEDPin, HIGH);
  }

  Decay(255, 0, -2, 4);
}

void loop() {
  buttonLoop();
  displayTime();
  delay(100);
}
