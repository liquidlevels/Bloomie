/*
  Rui Santos
  Complete project details at our blog.
    - ESP32: https://RandomNerdTutorials.com/esp32-firebase-realtime-database/
    - ESP8266: https://RandomNerdTutorials.com/esp8266-nodemcu-firebase-realtime-database/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  Based in the RTDB Basic Example by Firebase-ESP-Client library by mobizt
  https://github.com/mobizt/Firebase-ESP-Client/blob/main/examples/RTDB/Basic/Basic.ino
*/
#include <DHT.h>

// Define el pin al que está conectado el sensor DHT11
#define DHTPIN 4

// Selecciona el tipo de sensor DHT (en este caso, DHT11)
#define DHTTYPE DHT11

// Crea una instancia del sensor DHT
DHT dht(DHTPIN, DHTTYPE);

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

//ITE SSID
//#define WIFI_SSID "WiFi.ITE 2.4"
//#define WIFI_PASSWORD ""

//MICASA
#define WIFI_SSID "Totalplay-C3AA-5G"
#define WIFI_PASSWORD "C3AA5AEFbJYcHgvZ"

//Bloomie DB
// Insert Firebase project API Key
//#define API_KEY "AIzaSyBn24d2k6-X8sWV9D9A6fwtj-Zr_mnhKKQ"
// Insert RTDB URLefine the RTDB URL */
//#define DATABASE_URL "https://bloomie-c2584-default-rtdb.firebaseio.com/" 

//Test DB
#define API_KEY "AIzaSyCkm3kK2mGVyMMFBmGp3tQvvfKO2-CvqG8"
#define DATABASE_URL "https://test-b3d6b-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup(){
  // Inicializa el sensor DHT
  dht.begin();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    
    // Lee la humedad relativa
    float humedad = dht.readHumidity();

    // Lee la temperatura en grados Celsius
    float temperatura = dht.readTemperature();

    // Verifica si la lectura fue exitosa
    if (isnan(humedad) || isnan(temperatura)) {
      Serial.println("Error al leer del sensor DHT11");
      return;
    }
    
    // Write an Int number on the database path test/int
    /*if (Firebase.RTDB.setInt(&fbdo, "sensorHumedad/int", count)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    */
    
    if(Firebase.RTDB.setFloat(&fbdo, "sensorHumedad/temperatura"), temperatura){
      Serial.println("PASSED");
      Serial.print("Temperatura: ");
      Serial.print(temperatura);
      Serial.println(" °C");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "sensorHumedad/humedad", humedad){
      Serial.println("PASSED");
      Serial.print("Humedad: ");
      Serial.print(humedad);
      Serial.print(" %\t");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}

