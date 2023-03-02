/******************************************************************************
  Flex_Sensor_Example.ino
  Example sketch for SparkFun's flex sensors
  (https://www.sparkfun.com/products/10264)
  Jim Lindblom @ SparkFun Electronics
  April 28, 201

  Create a voltage divider circuit combining a flex sensor with a 47k resistor.
  - The resistor should connect from A0 to GND.
  - The flex sensor should connect from A0 to 3.3V
  As the resistance of the flex sensor increases (meaning it's being bent), the
  voltage at A0 should decrease.

  Development environment specifics:
  Arduino 1.6.7
******************************************************************************/
const int FLEX_PIN = A0; // Pin connected to voltage divider output
#include <SoftwareSerial.h>
SoftwareSerial hc06(9, 10); // tx, rx
// Measure the voltage at 5V and the actual resistance of your
// 47k resistor, and enter them below:
const float VCC = 5.0f;      // Measured voltage of Ardunio 5V line
const float R_DIV = 47500.0; // Measured resistance of 3.3k resistor

// Upload the code, then try to adjust these values to more
// accurately calculate bend degree.
const float STRAIGHT_RESISTANCE = 65505.82; // resistance when straight
const float BEND_RESISTANCE = 103408.38;    // resistance at 90 deg

// resistance for led to light up
const float yellow_threshold = 12.0;
const float green_threshold = yellow_threshold * 2.2; // 26.4

// for unblocking delay
unsigned long previous = 0;
const long interval = 2500;

const int red = 7;
const int yellow = 4;
const int green = 3;

float get_angle() {
  // Read the ADC, and calculate voltage and resistance from it
  int flexADC = analogRead(FLEX_PIN);
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV * (VCC / flexV - 1.0);
  //Serial.println("Resistance: " + String(flexR) + " ohms");

  // Use the calculated resistance to estimate the sensor's
  // bend angle:
  float angle = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE, 0, 90.0);
  //Serial.println("Bend: " + String(angle) + " degrees");
  Serial.println();
  return angle;
}
void setup() {
  Serial.begin(9600);
  pinMode(FLEX_PIN, INPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  hc06.begin(9600);
}

char get_status_code_and_flip_leds(void){
  char status = '0';
  float angle = get_angle();
  // Serial.println(angle);
  if (angle < yellow_threshold) { // red light
    digitalWrite(red, HIGH);
    digitalWrite(yellow, LOW);
    digitalWrite(green, LOW);
    status = '1';
  } else if (angle >= yellow_threshold &&
             angle <= green_threshold) { // yellow light
    digitalWrite(yellow, HIGH);
    digitalWrite(red, LOW);
    digitalWrite(green, LOW);
    status = '2';
  } else if (angle >= green_threshold) { // green light
    digitalWrite(green, HIGH);
    digitalWrite(red, LOW);
    digitalWrite(yellow, LOW);
    status = '3';
  }
  return status;
}

void loop() {
  char status = get_status_code_and_flip_leds();
  char last;
  int rx = hc06.read();
  //Serial.println(rx);
  delay(1000);
  if(rx == 100 && last != status){
    hc06.write(status);
    last = status;
  }
}
