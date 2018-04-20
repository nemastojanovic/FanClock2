//includo la libreria servo che mi permette di controllare l'ESC
#include <Servo.h>

//il pin per controllare il motore dell'ESC 
int motorPin = 9;

//il valore in microsecondi da inviare all'ESC, inizio con 1000 in modo che sia fermo
//infatti esso comincia a girare da 1150 us fino ad arrivare 1900 us
int value = 1000;

//oggetto che rapresenta l'ESC
Servo esc;

void setup() {
    //setto il motorPin per controllare l'ESC
    esc.attach(motorPin);
    //inzio una comunicazione seriale
    Serial.begin(9600);
}

void loop() {
  Serial.println(value);
  //scrivo nell'ESC value
  esc.writeMicroseconds(value);

  //leggo il valore inserito così da poter modificare la velocità del motore
  if(Serial.available()){ 
    value = Serial.parseInt();
  }  
}
