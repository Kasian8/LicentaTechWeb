#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>

const char* ssid = "DIGI-qz5A";
const char* password = "WsFtAN4t";

WebServer server(80);

// Pini 74HC595
#define DATA_PIN  13
#define LATCH_PIN 12
#define CLOCK_PIN 14

// Măști biți pentru camere
#define HALLWAY  ((1 << 1) | (1 << 2))
#define BEDROOM  (1 << 3)
#define BATHROOM (1 << 4)
#define GARAGE   (1 << 5)

uint8_t currentState = 0; 

void updateShiftRegister(uint8_t data) {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data);
    digitalWrite(LATCH_PIN, HIGH);
}

void handleRoot() {
    File f = SPIFFS.open("/index.html", "r");
    if(!f) {
        server.send(500, "text/plain", "Fisierul index.html nu a fost gasit pe SPIFFS!");
        return;
    }
    server.streamFile(f, "text/html");
    f.close();
}

void setup() {
    Serial.begin(115200);

    pinMode(DATA_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    updateShiftRegister(0);

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nIP: " + WiFi.localIP().toString());

    if(!SPIFFS.begin(true)) {
        Serial.println("SPIFFS mount failed!");
    }

    server.on("/", handleRoot);

    // Rutele de control (Fără redirect, trimit doar un raspuns OK)
    server.on("/hallway/on", []() { currentState |= HALLWAY; updateShiftRegister(currentState); server.send(200); });
    server.on("/hallway/off", []() { currentState &= ~HALLWAY; updateShiftRegister(currentState); server.send(200); });
    
    server.on("/bedroom/on", []() { currentState |= BEDROOM; updateShiftRegister(currentState); server.send(200); });
    server.on("/bedroom/off", []() { currentState &= ~BEDROOM; updateShiftRegister(currentState); server.send(200); });
    
    server.on("/bathroom/on", []() { currentState |= BATHROOM; updateShiftRegister(currentState); server.send(200); });
    server.on("/bathroom/off", []() { currentState &= ~BATHROOM; updateShiftRegister(currentState); server.send(200); });
    
    server.on("/garage/on", []() { currentState |= GARAGE; updateShiftRegister(currentState); server.send(200); });
    server.on("/garage/off", []() { currentState &= ~GARAGE; updateShiftRegister(currentState); server.send(200); });

    server.begin();
}

void loop() {
    server.handleClient();
}