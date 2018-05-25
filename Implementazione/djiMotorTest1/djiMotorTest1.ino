#include <Servo.h>

int motorPin = 9;
int value = 1050;
Servo esc;
void setup() {
    esc.attach(motorPin);
    Serial.begin(9600);
}

void loop() {
  Serial.println(value);
  esc.writeMicroseconds(value);
  
  if(Serial.available()){ 
    value = Serial.parseInt();
  }  
}
