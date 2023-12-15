
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <SoftwareSerial.h>

// Set these to run example.
#define FIREBASE_HOST "akuaponikk-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "S1jPnL04rtK8bgzLXW92MhcDpLu2PFZ3wKLJ1GJM"
#define WIFI_SSID "Kostkostan"
#define WIFI_PASSWORD "abimanyu09"
SoftwareSerial serialMega(D1, D2);

void setup() {
  Serial.begin(9600);
  serialMega.begin(9600);
  delay(5000);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to WiFi. IP Address: ");
    Serial.println(WiFi.localIP());
    
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  } else {
    Serial.println("Failed to connect to WiFi. Please check your credentials and connection.");
  }
}

void loop() {
  if (serialMega.available()) {
    String data = serialMega.readStringUntil('\n');

    // Print received data from Arduino Mega
    Serial.println("Received data from Arduino Mega: " + data);

    // Parse data and send to Firebase
    char *ptr = strtok(const_cast<char *>(data.c_str()), ",");
    float pHair, kekeruhanair, suhuair, ppmair, tinggiair;

    if (ptr != NULL) {
      pHair = atof(ptr);
      Serial.print("pHair: ");
      Serial.println(pHair);
      ptr = strtok(NULL, ",");
    }

    if (ptr != NULL) {
      kekeruhanair = atof(ptr);
      Serial.print("kekeruhanair: ");
      Serial.println(kekeruhanair);
      ptr = strtok(NULL, ",");
    }

    if (ptr != NULL) {
      suhuair = atof(ptr);
      Serial.print("suhuair: ");
      Serial.println(suhuair);
      ptr = strtok(NULL, ",");
    }
    if (ptr != NULL) {
      ppmair = atof(ptr);
      Serial.print("ppmair: ");
      Serial.println(ppmair);
      ptr = strtok(NULL, ",");
    }

    if (ptr != NULL) {
      tinggiair = atof(ptr);
      Serial.print("tinggiair: ");
      Serial.println(tinggiair);
    }

    // Send data to Firebase
    Firebase.setFloat("pHair", pHair);
    Firebase.setFloat("kekeruhanair", kekeruhanair);
    Firebase.setFloat("suhuair", suhuair);
    Firebase.setFloat("ppmair", ppmair);
    Firebase.setFloat("tinggiair", tinggiair);

    if (Firebase.failed()) {
      Serial.println("Failed to send data to Firebase!!!");
    } else {
      Serial.println("Data successfully sent to Firebase");
    }
  }
}