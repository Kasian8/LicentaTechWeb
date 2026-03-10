#include <Arduino.h>

    int senzPin = 34; 


void setup() {

    Serial.begin(115200);

    pinMode(senzPin,INPUT);

    analogReadResolution(12);

    Serial.print("Inceput");
}

void loop() {
 
    int rawValue = analogRead(senzPin);

    float vEsp = (rawValue * 3.3) / 4095.0;

    float vReal = vEsp * 1.5;

    Serial.println("Raw Value:");
    Serial.println(rawValue);
    Serial.println("");

    Serial.println("V at Esp Pin:");
    Serial.println(vEsp);
    Serial.println("");

    Serial.println("Real V:");
    Serial.println(vReal);
    Serial.println("");
    Serial.println("");
    Serial.println("");


    delay(500);
}
