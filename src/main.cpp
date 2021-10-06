#include <Arduino.h>
#include <SSD1306.h>

const int pulseA = 12;
const int pulseB = 13;
const int pushSW = 2;
const int relay = 15;
const int light = 0;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
int mode = 0;

SSD1306 display(0x3c, 4, 5, GEOMETRY_128_32);

void IRAM_ATTR handleRotary() {
  int MSB = digitalRead(pulseA);
  int LSB = digitalRead(pulseB);
  int encoded = (MSB << 1) |LSB;
  int sum = (lastEncoded << 2) | encoded;
  if(sum == 0b1101 ||sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {encoderValue++;}
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {encoderValue--;}
  lastEncoded = encoded;
  if(encoderValue > 255) {
    encoderValue = 255;
  }
  else if(encoderValue < 0){
    encoderValue = 0;
  }
}

void IRAM_ATTR buttonClicked() {
  Serial.println("pushed");
  mode = 1;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pushSW, INPUT_PULLUP);
  pinMode(pulseA, INPUT_PULLUP);
  pinMode(pulseB, INPUT_PULLUP);
  attachInterrupt(pushSW, buttonClicked, FALLING);
  attachInterrupt(pulseA, handleRotary, CHANGE);
  attachInterrupt(pulseB, handleRotary, CHANGE);
  pinMode(relay, OUTPUT);
  pinMode(light, INPUT);
  display.init();
  display.flipScreenVertically();
  display.drawString(10,10, "Hello World");
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
  int lightValue = analogRead(light);
  attachInterrupt(pushSW, buttonClicked, FALLING);
  attachInterrupt(pulseA, handleRotary, CHANGE);
  attachInterrupt(pulseB, handleRotary, CHANGE);
  Serial.println(encoderValue);
  display.clear();
  display.drawString(10,0, String(lightValue));
  display.drawString(10,10, String(encoderValue));
  if(mode == 1) {
    display.drawString(10,20, "pushed");
    mode =0;
  }
  display.display();

  if(lightValue < encoderValue) {
    digitalWrite(relay, HIGH);
  }
  else {
    digitalWrite(relay, LOW);
  }
  delay(500);
}
