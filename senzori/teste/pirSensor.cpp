#include <Arduino.h>

    const int pinPir=13;
    const int pinLed=2;

    int stareSenzor=LOW;

void setup() {
    
    Serial.begin(115200);

    pinMode(pinPir,INPUT);
    pinMode(pinLed,OUTPUT);

    Serial.print("Test Senzor Miscare");
    
    delay(10000);
    Serial.println("Senzor Activ");
}

void loop() {
    stareSenzor = digitalRead(pinPir);

    if(stareSenzor == HIGH){
        digitalWrite(pinLed,HIGH);
        Serial.println("Miscare Detectata");
        delay(500);
    }
    else{
        digitalWrite(pinLed,LOW);
        Serial.println("Nici o miscare detectata");
        delay(500);
    }
}
