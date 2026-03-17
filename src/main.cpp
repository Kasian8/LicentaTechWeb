#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <DHT.h>

// const char* ssid = "DIGI-fB6N";
// const char* password = "7HcKj45w";

const char* ssid = "DIGI-qz5A";
const char* password = "WsFtAN4t";


WebServer server(80);


    const int DHT_PIN = 4;
    const int DHT_TYPE = DHT11;
    DHT dht(DHT_PIN, DHT_TYPE);

    const int GAS_PIN = 34;


// int led-uri      
    const int LIVING_PIN = 2;
    const int DATA_PIN = 25;
    const int LATCH_PIN = 32;
    const int CLOCK_PIN = 26;

    const uint8_t HALLWAY = (1 << 1) | (1 << 2);
    const uint8_t BEDROOM = 1 << 3;
    const uint8_t BATHROOM = 1 << 4;
    const uint8_t GARAGE = 1 << 5;

    uint8_t currentState = 0;

    void updateShiftRegister(uint8_t data) {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data);
    digitalWrite(LATCH_PIN, HIGH);
}

//  Definire servo-uri
Servo s1, s2, s3, s4, s5;

// Vitezele generale servo
const int STOP_MS = 1500; 
const int SPEED_LEFT = 1700;  
const int SPEED_RIGHT = 1300; 

const int ROTATION_TIME = 700; 
const int RETURN_TIME = 750;

const int ROTATION_MAIN_DOOR = 700;
const int RETURN_MAIN_DOOR = 700;

const int ROTATION_GARAGE_DOOR=500;
const int RETURN_GARAGE_DOOR=550;

void handleRoot() {
    File f = SPIFFS.open("/dashboard.html", "r");
    if(f) { server.streamFile(f, "text/html"); f.close(); }
}

void setup() {
    Serial.begin(115200);

    dht.begin();

    analogReadResolution(12);


    //init shifter
    pinMode(DATA_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    updateShiftRegister(0);

    //living led

    pinMode(LIVING_PIN, OUTPUT);
    

    // Alocare timer pwm
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    // atasare servo
    s1.attach(12, 500, 2400);
    s2.attach(13, 500, 2400);
    s3.attach(14, 500, 2400);
    s4.attach(27, 500, 2400);
    s5.attach(33, 500, 2400);

    // calibrare Stop initial
    s1.writeMicroseconds(STOP_MS);
    s2.writeMicroseconds(STOP_MS);
    s3.writeMicroseconds(STOP_MS);
    s4.writeMicroseconds(STOP_MS);
    s5.writeMicroseconds(STOP_MS);

    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED) { delay(500); }
    SPIFFS.begin(true);
    Serial.println(WiFi.localIP());
    
    server.on("/", handleRoot);
    
    // rute css si js
   server.on("/css/global.css", []() {
        File f = SPIFFS.open("/css/global.css", "r");
        if(f) { server.streamFile(f, "text/css"); f.close(); }
    });

    server.on("/css/dashboard.css", []() {
        File f = SPIFFS.open("/css/dashboard.css", "r");
        if(f) { server.streamFile(f, "text/css"); f.close(); }
    });

    server.on("/js/dashboard.js", []() {
        File f = SPIFFS.open("/js/dashboard.js", "r");
        if(f) { server.streamFile(f, "application/javascript"); f.close(); }
    });


    //Rute LEDURI
    server.on("/hallway/on", []() { currentState |= HALLWAY; updateShiftRegister(currentState); server.send(200); });
    server.on("/hallway/off", []() { currentState &= ~HALLWAY; updateShiftRegister(currentState); server.send(200); });
    
    server.on("/bedroom/on", []() { currentState |= BEDROOM; updateShiftRegister(currentState); server.send(200); });
    server.on("/bedroom/off", []() { currentState &= ~BEDROOM; updateShiftRegister(currentState); server.send(200); });
    
    server.on("/bathroom/on", []() { currentState |= BATHROOM; updateShiftRegister(currentState); server.send(200); });
    server.on("/bathroom/off", []() { currentState &= ~BATHROOM; updateShiftRegister(currentState); server.send(200); });
    
    server.on("/garage/on", []() { currentState |= GARAGE; updateShiftRegister(currentState); server.send(200); });
    server.on("/garage/off", []() { currentState &= ~GARAGE; updateShiftRegister(currentState); server.send(200); });

    server.on("/living/on", []() { analogWrite(LIVING_PIN, 255); server.send(200); });
    server.on("/living/off", []() { analogWrite(LIVING_PIN, 0); server.send(200); });
    server.on("/living/10", []() { analogWrite(LIVING_PIN, 26); server.send(200); });
    server.on("/living/20", []() { analogWrite(LIVING_PIN, 51); server.send(200); });
    server.on("/living/30", []() { analogWrite(LIVING_PIN, 77); server.send(200); });
    server.on("/living/40", []() { analogWrite(LIVING_PIN, 102); server.send(200); });
    server.on("/living/50", []() { analogWrite(LIVING_PIN, 128); server.send(200); });
    server.on("/living/60", []() { analogWrite(LIVING_PIN, 153); server.send(200); });
    server.on("/living/70", []() { analogWrite(LIVING_PIN, 179); server.send(200); });
    server.on("/living/80", []() { analogWrite(LIVING_PIN, 204); server.send(200); });
    server.on("/living/90", []() { analogWrite(LIVING_PIN, 230); server.send(200); });
    


    //Parte de servo
    // RUTE SERVO 
        
    server.on("/bedroom/open", []() { 
        s1.writeMicroseconds(SPEED_LEFT); 
        s2.writeMicroseconds(SPEED_RIGHT); 
        delay(ROTATION_TIME); 
        s1.writeMicroseconds(STOP_MS); 
        s2.writeMicroseconds(STOP_MS); 
        server.send(200); 
    });

    server.on("/bedroom/close", []() { 
        s1.writeMicroseconds(SPEED_RIGHT); 
        s2.writeMicroseconds(SPEED_LEFT); 
        delay(RETURN_TIME); 
        s1.writeMicroseconds(STOP_MS); 
        s2.writeMicroseconds(STOP_MS); 
        server.send(200); 
    });
    

            
    // RUTE SERVO 3 (Pin 14) BATH
    server.on("/s3/stop", []() { s3.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s3/open", []() { s3.writeMicroseconds(SPEED_LEFT); delay(ROTATION_TIME); s3.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s3/close", []() { s3.writeMicroseconds(SPEED_RIGHT); delay(RETURN_TIME); s3.writeMicroseconds(STOP_MS); server.send(200); });

    // RUTE SERVO 4 (Pin 27) GARAGE
    server.on("/s4/stop", []() { s4.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s4/open", []() { s4.writeMicroseconds(SPEED_LEFT); delay(ROTATION_GARAGE_DOOR); s4.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s4/close", []() { s4.writeMicroseconds(SPEED_RIGHT); delay(RETURN_GARAGE_DOOR); s4.writeMicroseconds(STOP_MS); server.send(200); });

    // RUTE SERVO 5 (Pin 33) MAIN
    server.on("/s5/stop", []() { s5.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s5/open", []() { s5.writeMicroseconds(SPEED_LEFT); delay(ROTATION_MAIN_DOOR); s5.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s5/close", []() { s5.writeMicroseconds(SPEED_RIGHT); delay(RETURN_MAIN_DOOR); s5.writeMicroseconds(STOP_MS); server.send(200); });
            //sfarsit parte de servo

    // DHT

    // Rută pentru DHT11
    server.on("/sensor/dht", []() {
        float h = dht.readHumidity();
        float t = dht.readTemperature();

        if(isnan(h) || isnan(t)){   
            server.send(500, "application/json", "{\"error\":\"Eroare DHT\"}");
            return;
        }

        String json = "{\"temperature\": " + String(t) + ", \"humidity\": " + String(h) + "}";
        server.send(200, "application/json", json);
    });

    // Rută separată pentru Gaz
    server.on("/sensor/gas", []() {
        int rawValue = analogRead(GAS_PIN);
        int gasPercent = map(rawValue, 0, 4095, 0, 100);

        String json = "{\"gas\": " + String(gasPercent) + "}";
        server.send(200, "application/json", json);
    });


    server.begin();
}

void loop() {
    server.handleClient();
}