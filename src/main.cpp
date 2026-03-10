#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "DIGI-qz5A";
const char* password = "WsFtAN4t";

WebServer server(80);

// Definire obiecte pentru cele 5 servomotoare
Servo s1, s2, s3, s4, s5;

// Setări mișcare (Valabile pentru toate)
const int STOP_MS = 1500; 
const int SPEED_LEFT = 1550;  
const int SPEED_RIGHT = 1450; 
const int TIMP_ROTATIE = 1000; 

void handleRoot() {
    File f = SPIFFS.open("/index.html", "r");
    if(f) { server.streamFile(f, "text/html"); f.close(); }
}

void setup() {
    Serial.begin(115200);

    // Alocare timere PWM
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    // Atașare servomotoare pe pini
    s1.attach(12, 500, 2400);
    s2.attach(13, 500, 2400);
    s3.attach(14, 500, 2400);
    s4.attach(27, 500, 2400);
    s5.attach(33, 500, 2400);

    // Oprire inițială
    s1.writeMicroseconds(STOP_MS);
    s2.writeMicroseconds(STOP_MS);
    s3.writeMicroseconds(STOP_MS);
    s4.writeMicroseconds(STOP_MS);
    s5.writeMicroseconds(STOP_MS);

    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED) { delay(500); }
    SPIFFS.begin(true);

    server.on("/", handleRoot);
    
    // RUTE SERVO 1 (Pin 12)
    server.on("/s1/left", []() { s1.writeMicroseconds(SPEED_LEFT); server.send(200); });
    server.on("/s1/right", []() { s1.writeMicroseconds(SPEED_RIGHT); server.send(200); });
    server.on("/s1/stop", []() { s1.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s1/open", []() { s1.writeMicroseconds(SPEED_LEFT); delay(TIMP_ROTATIE); s1.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s1/close", []() { s1.writeMicroseconds(SPEED_RIGHT); delay(TIMP_ROTATIE); s1.writeMicroseconds(STOP_MS); server.send(200); });

    // RUTE SERVO 2 (Pin 13)
    server.on("/s2/left", []() { s2.writeMicroseconds(SPEED_LEFT); server.send(200); });
    server.on("/s2/right", []() { s2.writeMicroseconds(SPEED_RIGHT); server.send(200); });
    server.on("/s2/stop", []() { s2.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s2/open", []() { s2.writeMicroseconds(SPEED_LEFT); delay(TIMP_ROTATIE); s2.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s2/close", []() { s2.writeMicroseconds(SPEED_RIGHT); delay(TIMP_ROTATIE); s2.writeMicroseconds(STOP_MS); server.send(200); });

    // RUTE SERVO 3 (Pin 14)
    server.on("/s3/left", []() { s3.writeMicroseconds(SPEED_LEFT); server.send(200); });
    server.on("/s3/right", []() { s3.writeMicroseconds(SPEED_RIGHT); server.send(200); });
    server.on("/s3/stop", []() { s3.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s3/open", []() { s3.writeMicroseconds(SPEED_LEFT); delay(TIMP_ROTATIE); s3.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s3/close", []() { s3.writeMicroseconds(SPEED_RIGHT); delay(TIMP_ROTATIE); s3.writeMicroseconds(STOP_MS); server.send(200); });

    // RUTE SERVO 4 (Pin 27)
    server.on("/s4/left", []() { s4.writeMicroseconds(SPEED_LEFT); server.send(200); });
    server.on("/s4/right", []() { s4.writeMicroseconds(SPEED_RIGHT); server.send(200); });
    server.on("/s4/stop", []() { s4.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s4/open", []() { s4.writeMicroseconds(SPEED_LEFT); delay(TIMP_ROTATIE); s4.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s4/close", []() { s4.writeMicroseconds(SPEED_RIGHT); delay(TIMP_ROTATIE); s4.writeMicroseconds(STOP_MS); server.send(200); });

    // RUTE SERVO 5 (Pin 33)
    server.on("/s5/left", []() { s5.writeMicroseconds(SPEED_LEFT); server.send(200); });
    server.on("/s5/right", []() { s5.writeMicroseconds(SPEED_RIGHT); server.send(200); });
    server.on("/s5/stop", []() { s5.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s5/open", []() { s5.writeMicroseconds(SPEED_LEFT); delay(TIMP_ROTATIE); s5.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s5/close", []() { s5.writeMicroseconds(SPEED_RIGHT); delay(TIMP_ROTATIE); s5.writeMicroseconds(STOP_MS); server.send(200); });

    server.begin();
}

void loop() {
    server.handleClient();
}