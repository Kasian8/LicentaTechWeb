#include <Arduino.h>
#include <ESP32Servo.h>

// Definire pini
const int SERVO_PIN = 19;
const int BUTTON_LEFT = 26;
const int BUTTON_RIGHT = 25;

// Valori pentru controlul vitezei
const int STOP_MS = 1500; 
const int SPEED_LEFT = 1550;  // Ajustează între 1510 și 1600 pentru viteză mică la stânga
const int SPEED_RIGHT = 1450; // Ajustează între 1490 și 1400 pentru viteză mică la dreapta

Servo myServo;

void setup() {
    Serial.begin(115200);
    
    pinMode(BUTTON_LEFT, INPUT_PULLUP);
    pinMode(BUTTON_RIGHT, INPUT_PULLUP);

    myServo.attach(SERVO_PIN, 500, 2400);
    
    // Oprire inițială
    myServo.writeMicroseconds(STOP_MS);
    Serial.println("Sistem pornit - Control manual activat");
}

void loop() {
    bool leftPressed = (digitalRead(BUTTON_LEFT) == LOW);
    bool rightPressed = (digitalRead(BUTTON_RIGHT) == LOW);

    if (leftPressed) {
        // Merge la stânga cât timp butonul e apăsat
        myServo.writeMicroseconds(SPEED_LEFT);
        Serial.println("Mișcare la STÂNGA...");
    } 
    else if (rightPressed) {
        // Merge la dreapta cât timp butonul e apăsat
        myServo.writeMicroseconds(SPEED_RIGHT);
        Serial.println("Mișcare la DREAPTA...");
    } 
    else {
        // Dacă nu e apăsat niciun buton, motorul stă pe loc
        myServo.writeMicroseconds(STOP_MS);
    }

    // Mică pauză pentru stabilitatea procesorului (nu afectează mișcarea sesizabil)
    delay(10); 
}