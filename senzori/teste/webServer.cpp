#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>

const char* ssid = "DIGI-fB6N";
const char* password = "7HcKj45w";

WebServer server(80);

const int output26 = 26;
const int output27 = 27;

// Serve index.html
void handleRoot() {
    File f = SPIFFS.open("/index.html", "r");
    if(!f) {
        server.send(500, "text/plain", "Failed to open index.html");
        return;
    }
    server.streamFile(f, "text/html");
    f.close();
}

void setup() {
    Serial.begin(115200);

    // GPIO setup
    pinMode(output26, OUTPUT);
    pinMode(output27, OUTPUT);
    digitalWrite(output26, LOW);
    digitalWrite(output27, LOW);

    // WiFi connect
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");
    Serial.println("IP Address: ");
    Serial.println(WiFi.localIP());

    // SPIFFS init
    if(!SPIFFS.begin(true)) { // true -> format if mount fails
        Serial.println("SPIFFS mount failed!");
        while(true) delay(1000);
    }
    Serial.println("SPIFFS mounted successfully");

    // HTTP routes
    server.on("/", handleRoot);

server.on("/26/on", []() { 
    digitalWrite(output26, HIGH); 
});
server.on("/26/off", []() { 
    digitalWrite(output26, LOW); 
});
server.on("/27/on", []() { 
    digitalWrite(output27, HIGH); 
});
server.on("/27/off", []() { 
    digitalWrite(output27, LOW); 
});
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}
