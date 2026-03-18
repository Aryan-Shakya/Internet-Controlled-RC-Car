#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "kom";
const char* password = "00000000";

// Firebase Database URL - REST API
String firebaseHost = "https://esp32-rc-car-6b539-default-rtdb.firebaseio.com/gpio.json";

// Motor pins
const int motor1Pin1 = 14;  // IN1 - Motor A Forward
const int motor1Pin2 = 27;  // IN2 - Motor A Backward
const int motor2Pin1 = 26;  // IN3 - Motor B Forward
const int motor2Pin2 = 25;  // IN4 - Motor B Backward

unsigned long lastCheck = 0;
const unsigned long checkInterval = 1000; // Check every 1 second

// Track current state to avoid redundant serial prints
String currentState = "STOPPED";

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=================================");
  Serial.println("ESP32 Firebase RC Car");
  Serial.println("=================================\n");
  
  // Setup motor pins
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  
  stopMotors();
  Serial.println("✓ Motors initialized");
  
  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("\n✓ System Ready!");
    Serial.println("Checking Firebase every 1 second...\n");
  } else {
    Serial.println("\n✗ WiFi connection failed!");
    Serial.println("Restarting ESP32...");
    delay(3000);
    ESP.restart();
  }
}

void loop() {
  // Check Firebase at regular intervals
  if (millis() - lastCheck >= checkInterval) {
    lastCheck = millis();
    checkFirebase();
  }
  
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠ WiFi disconnected! Reconnecting...");
    WiFi.reconnect();
    delay(5000);
  }
}

void checkFirebase() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Make GET request to Firebase
    http.begin(firebaseHost);
    http.addHeader("Content-Type", "application/json");
    
    int httpCode = http.GET();
    
    if (httpCode == 200) {
      String payload = http.getString();
      
      // Parse JSON response
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, payload);
      
      if (!error) {
        // Get GPIO pin states
        int pin14 = doc["14"] | 0;
        int pin27 = doc["27"] | 0;
        int pin26 = doc["26"] | 0;
        int pin25 = doc["25"] | 0;
        
        // Only print when there's a change (to reduce serial spam)
        static int lastPin14 = -1, lastPin27 = -1, lastPin26 = -1, lastPin25 = -1;
        
        if (pin14 != lastPin14 || pin27 != lastPin27 || pin26 != lastPin26 || pin25 != lastPin25) {
          Serial.printf("GPIO States: 14=%d | 27=%d | 26=%d | 25=%d\n", pin14, pin27, pin26, pin25);
          lastPin14 = pin14;
          lastPin27 = pin27;
          lastPin26 = pin26;
          lastPin25 = pin25;
        }
        
        // Control motors based on GPIO states
        // CHECK DIAGONAL MOVEMENTS FIRST (2 pins active)
        if (pin14 == 1 && pin25 == 1) {
          forwardRight();
        }
        else if (pin14 == 1 && pin26 == 1) {
          forwardLeft();
        }
        else if (pin27 == 1 && pin25 == 1) {
          reverseRight();
        }
        else if (pin27 == 1 && pin26 == 1) {
          reverseLeft();
        }
        // THEN CHECK SINGLE MOVEMENTS
        else if (pin14 == 1) {
          moveForward();
        } 
        else if (pin27 == 1) {
          moveBackward();
        } 
        else if (pin26 == 1) {
          turnLeft();
        } 
        else if (pin25 == 1) {
          turnRight();
        } 
        else {
          stopMotors();
        }
        
      } else {
        Serial.print("✗ JSON parse error: ");
        Serial.println(error.c_str());
      }
      
    } else if (httpCode > 0) {
      Serial.printf("✗ HTTP Error code: %d\n", httpCode);
    } else {
      Serial.printf("✗ Connection failed: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();
    
  } else {
    Serial.println("✗ WiFi not connected!");
  }
}

// Motor control functions
void moveForward() {
  if (currentState != "FORWARD") {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
    Serial.println("🚗 ↑ MOVING FORWARD");
    currentState = "FORWARD";
  }
}

void moveBackward() {
  if (currentState != "BACKWARD") {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
    Serial.println("🚗 ↓ MOVING BACKWARD");
    currentState = "BACKWARD";
  }
}

void turnLeft() {
  if (currentState != "LEFT") {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
    Serial.println("🚗 ← TURNING LEFT");
    currentState = "LEFT";
  }
}

void turnRight() {
  if (currentState != "RIGHT") {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
    Serial.println("🚗 → TURNING RIGHT");
    currentState = "RIGHT";
  }
}

void forwardRight() {
  if (currentState != "FORWARD_RIGHT") {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
    Serial.println("🚗 ↗ FORWARD RIGHT");
    currentState = "FORWARD_RIGHT";
  }
}

void forwardLeft() {
  if (currentState != "FORWARD_LEFT") {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
    Serial.println("🚗 ↖ FORWARD LEFT");
    currentState = "FORWARD_LEFT";
  }
}

void reverseLeft() {
  if (currentState != "REVERSE_LEFT") {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
    Serial.println("🚗 ↙ REVERSE LEFT");
    currentState = "REVERSE_LEFT";
  }
}

void reverseRight() {
  if (currentState != "REVERSE_RIGHT") {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
    Serial.println("🚗 ↘ REVERSE RIGHT");
    currentState = "REVERSE_RIGHT";
  }
}

void stopMotors() {
  if (currentState != "STOPPED") {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
    Serial.println("🚗 ■ STOPPED");
    currentState = "STOPPED";
  }
}
