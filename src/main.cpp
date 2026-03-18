#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <DHT.h>

const char* ssid = "DIGI-qz5A";
const char* password = "WsFtAN4t";

WebServer server(80);

const char* headerKeys[] = {"Cookie"};

bool is_authenticated() {
    if (server.hasHeader("Cookie")) {
        String cookie = server.header("Cookie");
        if (cookie.indexOf("ESPSESSIONID=1") != -1) {
            return true;
        }
    }
    return false;
}

const int DHT_PIN = 4;
const int DHT_TYPE = DHT11;
DHT dht(DHT_PIN, DHT_TYPE);

const int GAS_PIN = 34;

const int PIR_SENSOR = 35;
unsigned long LAST_MOTION_TIME = 0;

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

Servo s1, s2, s3, s4, s5;

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
    if (!is_authenticated()) {
        server.sendHeader("Location", "/login");
        server.send(303);
        return;
    }
    File f = SPIFFS.open("/dashboard.html", "r");
    if(f) { server.streamFile(f, "text/html"); f.close(); }
}

void setup() {
    Serial.begin(115200);

    dht.begin();

    analogReadResolution(12);

    pinMode(PIR_SENSOR, INPUT);

    pinMode(DATA_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    updateShiftRegister(0);

    pinMode(LIVING_PIN, OUTPUT);
    
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    s1.attach(12, 500, 2400);
    s2.attach(13, 500, 2400);
    s3.attach(14, 500, 2400);
    s4.attach(27, 500, 2400);
    s5.attach(33, 500, 2400);

    s1.writeMicroseconds(STOP_MS);
    s2.writeMicroseconds(STOP_MS);
    s3.writeMicroseconds(STOP_MS);
    s4.writeMicroseconds(STOP_MS);
    s5.writeMicroseconds(STOP_MS);

    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED) { delay(500); }
    SPIFFS.begin(true);
    Serial.println(WiFi.localIP());
    
    server.collectHeaders(headerKeys, 1);

    server.on("/", handleRoot);
    
    server.on("/login", HTTP_GET, []() {
        File f = SPIFFS.open("/login.html", "r");
        if(f) { server.streamFile(f, "text/html"); f.close(); }
    });

    server.on("/auth", HTTP_POST, []() {
        if (server.hasArg("username") && server.hasArg("password")) {
            String user = server.arg("username");
            String pass = server.arg("password");

            if (user == "admin" && pass == "1234") {
                server.sendHeader("Set-Cookie", "ESPSESSIONID=1; Path=/; HttpOnly");
                server.send(200, "text/plain", "OK");
                return;
            }
        }
        server.send(401, "text/plain", "Unauthorized");
    });

    server.on("/data/css/global.css", []() {
        File f = SPIFFS.open("/css/global.css", "r");
        if(f) { server.streamFile(f, "text/css"); f.close(); }
    });

    server.on("/data/css/login.css", []() {
        File f = SPIFFS.open("/css/login.css", "r");
        if(f) { server.streamFile(f, "text/css"); f.close(); }
    });

    server.on("/data/js/login.js", []() {
        File f = SPIFFS.open("/js/login.js", "r");
        if(f) { server.streamFile(f, "application/javascript"); f.close(); }
    });

    server.on("/css/global.css", []() {
        if (!is_authenticated()) { server.send(401, "text/plain", "Unauthorized"); return; }
        File f = SPIFFS.open("/css/global.css", "r");
        if(f) { server.streamFile(f, "text/css"); f.close(); }
    });

    server.on("/css/dashboard.css", []() {
        if (!is_authenticated()) { server.send(401, "text/plain", "Unauthorized"); return; }
        File f = SPIFFS.open("/css/dashboard.css", "r");
        if(f) { server.streamFile(f, "text/css"); f.close(); }
    });

    server.on("/js/dashboard.js", []() {
        if (!is_authenticated()) { server.send(401, "text/plain", "Unauthorized"); return; }
        File f = SPIFFS.open("/js/dashboard.js", "r");
        if(f) { server.streamFile(f, "application/javascript"); f.close(); }
    });

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
    
    server.on("/s3/stop", []() { s3.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s3/open", []() { s3.writeMicroseconds(SPEED_LEFT); delay(ROTATION_TIME); s3.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s3/close", []() { s3.writeMicroseconds(SPEED_RIGHT); delay(RETURN_TIME); s3.writeMicroseconds(STOP_MS); server.send(200); });

    server.on("/s4/stop", []() { s4.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s4/open", []() { s4.writeMicroseconds(SPEED_LEFT); delay(ROTATION_GARAGE_DOOR); s4.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s4/close", []() { s4.writeMicroseconds(SPEED_RIGHT); delay(RETURN_GARAGE_DOOR); s4.writeMicroseconds(STOP_MS); server.send(200); });

    server.on("/s5/stop", []() { s5.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s5/open", []() { s5.writeMicroseconds(SPEED_LEFT); delay(ROTATION_MAIN_DOOR); s5.writeMicroseconds(STOP_MS); server.send(200); });
    server.on("/s5/close", []() { s5.writeMicroseconds(SPEED_RIGHT); delay(RETURN_MAIN_DOOR); s5.writeMicroseconds(STOP_MS); server.send(200); });

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

    server.on("/sensor/gas", []() {
        int rawValue = analogRead(GAS_PIN);
        int gasPercent = map(rawValue, 0, 4095, 0, 100);

        String json = "{\"gas\": " + String(gasPercent) + "}";
        server.send(200, "application/json", json);
    });

    server.on("/sensor/motion", []() {
        unsigned long timeElapsed = millis() - LAST_MOTION_TIME;
        int seconds = timeElapsed / 1000;

        String jason = "{\"motion\": " + String(seconds) + "}";
        server.send(200, "application/json", jason);
    });

    server.begin();
}

void loop() {
    server.handleClient();

    if(digitalRead(PIR_SENSOR) == HIGH){
        LAST_MOTION_TIME = millis();
    }
}