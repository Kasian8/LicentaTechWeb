#include <Arduino.h>

    const int pinIR = 13;
    const int pinLed = 2;

    int stareSenzor;

void setup() {
    
    Serial.begin(115200);
    
    pinMode(pinIR,INPUT);
    pinMode(pinLed,OUTPUT);

    Serial.print("senzor activ");
    
}

void loop() {
 
    stareSenzor = digitalRead(pinIR);

    if(stareSenzor == LOW){
        digitalWrite(pinLed,HIGH);
        Serial.println("Detectie");
        delay(500);
    }
    else{
        digitalWrite(pinLed,LOW);
        Serial.println("Nici o miscare");
        delay(50);
    }
}
