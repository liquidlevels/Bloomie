/*
  Fabrice Beya
  The complete project details can be found here: 

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "DHT.h"
#include <driver/adc.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Digital pin connected to the DHT sensor
#define DHTPIN 4 
#define DHTTYPE DHT11 

//Analog pin to ground humidity sensor
#define GROUND_HUMIDITY_PIN 36

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Device ID
#define DEVICE_UID "4X"

//MICASA
//#define WIFI_SSID "Totalplay-C3AA-5G"
//#define WIFI_PASSWORD "C3AA5AEFbJYcHgvZ"

//#define WIFI_SSID "TOTALPLAY_A8D35F_2.4"
//#define WIFI_PASSWORD "FZT4101C4T"

#define WIFI_SSID "dogfucker"
#define WIFI_PASSWORD "holaquetal"

//#define WIFI_SSID "WiFi.ITE 2.4"
//#define WIFI_PASSWORD ""

// Your Firebase Project Web API Key
#define API_KEY "AIzaSyBn24d2k6-X8sWV9D9A6fwtj-Zr_mnhKKQ"

// Your Firebase Realtime database URL
#define DATABASE_URL "https://bloomie-c2584-default-rtdb.firebaseio.com/" 

// Constants
const int VAL_HIGH = 1024; // DRY
const int VAL_LOW = 800; // WET

// Device Location config
String device_location = "";

// Firebase Realtime Database Object
FirebaseData fbdo;

// Firebase Authentication Object
FirebaseAuth auth;

// Firebase configuration Object
FirebaseConfig config;

// Firebase database path
String dht11_path = "DHT11";
String databasePath = "";
String ground_humidity_path = "GROUND_HUMIDITY";

// Firebase Unique Identifier
String fuid = "";

// Stores the elapsed time from device start up
unsigned long elapsedMillis = 0;

// The frequency of sensor updates to firebase, set to 10seconds
unsigned long update_interval = 10000;

// Dummy counter to test initial firebase updates
int count = 0;

// Store device authentication status
bool isAuthenticated = false;

// Variables to hold sensor readings
//float temperature = 24.7;
float temperature = 2;
//float humidity = 60;
float humidity = 30;
int ground_humidity = 20;

// JSON object to hold updated sensor values to be sent to firebase
FirebaseJson temperature_json;
FirebaseJson humidity_json;
FirebaseJson ground_humidity_json;

void Wifi_Init() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.print(".");
      delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void firebase_init() {
    // configure firebase API Key
    config.api_key = API_KEY;

    // configure firebase realtime database url
    config.database_url = DATABASE_URL;

    // Enable WiFi reconnection 
    Firebase.reconnectWiFi(true);

    Serial.println("------------------------------------");
    Serial.println("Sign up new user...");

    // Sign in to firebase Anonymously
    if (Firebase.signUp(&config, &auth, "", ""))
    {
        Serial.println("Success");
        isAuthenticated = true;

        // Set the database path where updates will be loaded for this device
        databasePath = "/";
        fuid = auth.token.uid.c_str();
    }
    else
    {
        Serial.printf("Failed, %s\n", config.signer.signupError.message.c_str());
        isAuthenticated = false;
    }

    // Assign the callback function for the long running token generation task, see addons/TokenHelper.h
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    // Initialise firebase service
    Firebase.begin(&config, &auth);
}

void groundhumidity_init(){
  
  //groundhumidity_begin();
  ground_humidity_json.add("name", "Ground Humidity");
  ground_humidity_json.add("value", ground_humidity);

  String jsonStr;
  ground_humidity_json.toString(jsonStr, true);
  Serial.println(jsonStr);
}
/*
void groundhumidity(){
  int analogValue = analogRead(GROUND_HUMIDITY_PIN);
  ground_humidity = analogValue;
  Serial.println(ground_humidity);
}
*/
void dhtt11_init(){
  dht.begin();

  // Initialise temprature json data
  //temperature_json.add("deviceuid", DEVICE_UID);
  temperature_json.add("name", "Temperature");
  //temperature_json.add("type", "Temperature");
  //temperature_json.add("location", device_location);
  temperature_json.add("value", temperature);

  // Print out initial temperature values
  String jsonStr;
  temperature_json.toString(jsonStr, true);
  Serial.println(jsonStr);

  // Initialise humidity json data
  //humidity_json.add("deviceuid", DEVICE_UID);
  humidity_json.add("name", "Humidity");
  //humidity_json.add("type", "Humidity");
  //humidity_json.add("location", device_location);
  humidity_json.add("value", humidity);

  String jsonStr2;
  humidity_json.toString(jsonStr2, true);
  Serial.println(jsonStr2);
}

void setup() {
  // Initialise serial communication for local diagnostics
  Serial.begin(115200);
  // Initialise Connection with location WiFi
  Wifi_Init();
  // Initialise firebase configuration and signup anonymously
  firebase_init();
  // Initialise DHTT11 module
  dhtt11_init();
  //Ground humidity capture starts
  pinMode(GROUND_HUMIDITY_PIN,INPUT);
  groundhumidity_init();
}

void updateSensorReadings(){
  Serial.println("------------------------------------");
  Serial.println("Reading Sensor data ...");
  //groundhumidity();
  int analogValue = analogRead(GROUND_HUMIDITY_PIN);
  ground_humidity = analogValue;
  Serial.println(ground_humidity);
  
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.printf("Temperature reading: %.2f \n", temperature);
  Serial.printf("Humidity reading: %.2f \n", humidity);
  Serial.printf("Ground Humidity reading: %d \n", ground_humidity);

  temperature_json.set("value", temperature);
  humidity_json.set("value", humidity);
  ground_humidity_json.set("value", ground_humidity);
}

void uploadSensorData() {
  if (millis() - elapsedMillis > update_interval && isAuthenticated && Firebase.ready())
    {
      elapsedMillis = millis();

      updateSensorReadings();
      String temperature_node = databasePath + dht11_path + "/temperature";  
      String humidity_node = databasePath + dht11_path + "/humidity"; 
      String ground_humidity_node = databasePath + ground_humidity_path + "/ground_humidity";
      //String temperature_node = "/temperature";  
      //String humidity_node = "/humidity"; 

      if (Firebase.setJSON(fbdo, temperature_node.c_str(), temperature_json))
      {
          //Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          //Serial.println("TYPE: " + fbdo.dataType());
          //Serial.println("ETag: " + fbdo.ETag());
          Serial.print("VALUE: ");
          printResult(fbdo); //see addons/RTDBHelper.h
          //Serial.println("------------------------------------");
          Serial.println();
      }
      else
      {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
          Serial.println("------------------------------------");
          Serial.println();
      }

      if (Firebase.setJSON(fbdo, humidity_node.c_str(), humidity_json))
      {
          //Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          //Serial.println("TYPE: " + fbdo.dataType());
          //Serial.println("ETag: " + fbdo.ETag());
          Serial.print("VALUE: ");
          printResult(fbdo); //see addons/RTDBHelper.h
          //Serial.println("------------------------------------");
          Serial.println();
      }
      else
      {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
          Serial.println("------------------------------------");
          Serial.println();
      }
      
      if(Firebase.setJSON(fbdo,ground_humidity_node.c_str(), ground_humidity_json)){
        //Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        //Serial.println("TYPE: " + fbdo.dataType());
        //Serial.println("ETag: " + fbdo.ETag());
        Serial.print("VALUE: ");
        printResult(fbdo); //see addons/RTDBHelper.h
        Serial.println("------------------------------------");
        Serial.println();
      }
      else
      {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
          Serial.println("------------------------------------");
          Serial.println();
      } 
      
    }
}

void loop() {
  uploadSensorData();
}
