#include "RTClib.h" // https://github.com/adafruit/RTClib

const int secPin = 6;
const int minPin = 5;
const int hourPin = 3;
const int AMLEDPin = 10;
const int PMLEDPin = 11;

bool isSecDecay = false;
bool isMinDecay = false;
bool isHourDecay = false;
bool isAMLEDDecay = false;
bool isPMLEDDecay = false;
int decayPins[] = {secPin, minPin, hourPin, AMLEDPin, PMLEDPin}; // order must match isDecayPins in Decay method

int secReset = 0;
int minReset = 0;
int hourReset = 0;

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
  bool isDecayPins[] = {isSecDecay, isMinDecay, isHourDecay, isAMLEDDecay, isPMLEDDecay};
  bool isResets[] = {secReset, minReset, hourReset, 0, 0};

  // Loop through the decays
  for (int i = 0; i <= abs(startVal - endVal); i++) {
    // Loop through the pins
    for (int j = 0; j < sizeof decayPins/sizeof decayPins[0]; j++) {
      if (isDecayPins[j]) {
        if (isResets[j] == 1) {
          analogWrite(decayPins[j], decay);
        }
      }
    }

    delay(delayTime);
    decay += decayRate;
  }
}

void InitializeButton() {
  // ## INITIALIZE BUTTON CODE

  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
}

void InitializePin() {
  // ## INITIALIZE PINS
  isSecDecay = true;
  isMinDecay = true;
  isHourDecay = true;
  isAMLEDDecay = true;
  isPMLEDDecay = true;

  Decay(0, 255,  1, 10);
  Decay(255, 0, -1, 10);
}

void setup() {

  Serial.begin(9600);

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
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);

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
  isAMLEDDecay = false;
  isPMLEDDecay = false;

  // Retrieve the time from the real time clock.
  DateTime now = rtc.now();

  // ### For debuging purposes, write the time to the
  // Serial.print(h);
  // Serial.print(":");
  // Serial.print(now.minute());
  // Serial.print(":");
  // Serial.println(now.second());

  if (now.second() == 0) {
    if (secReset == 0) {
      isSecDecay = true;
      secReset = 1;
    }
  } else {
    secReset = 0;
    analogWrite(secPin, now.second() * (255 / 60));
  }

  if (now.minute() == 0) {
    if (minReset == 0) {
      isMinDecay = true;
      minReset = 1;
    }
  } else {
    minReset = 0;
    analogWrite(minPin, now.minute() * (255 / 60));
  }

  if (now.hour() % 12 == 0) {
    if (hourReset == 0) {
      isHourDecay = true;
      hourReset = 1;
    }
  } else {
    hourReset = 0;
    analogWrite(hourPin, (now.hour() % 12) * (255 / 12));
  }

  if (now.hour() < 12) {
    analogWrite(AMLEDPin, 250);
    analogWrite(PMLEDPin, 0);
  } else {
    analogWrite(AMLEDPin, 0);
    analogWrite(PMLEDPin, 250);
  }

  Decay(255, 0, 2, 3);
}

void loop() {
  buttonLoop();
  displayTime();
  delay(100);
}
