#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "DHT.h"
#include <driver/adc.h>
#include "time.h"

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Digital pin connected to the DHT sensor
#define DHTPIN 4 
#define DHTTYPE DHT11 

//Analog pin to ground humidity sensor
#define GROUND_HUMIDITY_PIN 36

//Values for irrigation system
#define MIN_HUMIDITY 1638
#define MAX_HUMIDITY 2457

//Digital pin connected to Relay Module
#define RELAY_PIN 5

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

#define WIFI_SSID "ITE?"
#define WIFI_PASSWORD "albatro123"

// Your Firebase Project Web API Key
#define API_KEY "AIzaSyBn24d2k6-X8sWV9D9A6fwtj-Zr_mnhKKQ"

// Your Firebase Realtime database URL
#define DATABASE_URL "https://bloomie-c2584-default-rtdb.firebaseio.com/" 

const char* ntp_server = "pool.ntp.org";
const long gmt_offset_sec = -28800;//gmt -8 (local)
const int daylight_offset_sec = 3600;
struct tm timeinfo;
char day[20];
char dayofmonth[3];
char month[20];
char hour[3];
char minute[3];
char second[3];

// Device Location config
String device_location = "";

// Firebase Realtime Database Object
FirebaseData fbdo;

// Firebase Authentication Object
FirebaseAuth auth;

// Firebase configuration Object
FirebaseConfig config;

// Variables to hold sensor readings
float temperature = 0;
float humidity = 0;
int ground_humidity = 0;
String string_month = "";
String string_day = "";
String string_day_of_month = "";
String string_time = "";

// Firebase database path
String dht11_path = "dht11";
String databasePath = "";
String ground_humidity_path = "ground_humidity";

// Firebase Unique Identifier
String fuid = "";

// Stores the elapsed time from device start up
unsigned long elapsed_millis = 0;

// The frequency of sensor updates to firebase, set to 10seconds
unsigned long update_interval = 10000;

// Store device authentication status
bool isAuthenticated = false;

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
      delay(1000);//1 second
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
    else {
    
        Serial.printf("Failed, %s\n", config.signer.signupError.message.c_str());
        isAuthenticated = false;
    }

    // Assign the callback function for the long running token generation task, see addons/TokenHelper.h
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    // Initialise firebase service
    Firebase.begin(&config, &auth);
}

void groundhumidity_init(){
  ground_humidity_json.add("name", "Ground Humidity");
  ground_humidity_json.add("value", ground_humidity);

  String jsonStr;
  ground_humidity_json.toString(jsonStr, true);
  Serial.println(jsonStr);
}

void groundhumidity(){
  int analogValue = analogRead(GROUND_HUMIDITY_PIN);
  ground_humidity = analogValue;
  Serial.println(ground_humidity);
}

void getCurrentDate(){
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  } else {
    strftime(day, sizeof(day), "%A", &timeinfo);
    strftime(dayofmonth, sizeof(dayofmonth), "%d", &timeinfo);
    strftime(month, sizeof(month), "%B", &timeinfo);
    strftime(hour, sizeof(hour), "%H", &timeinfo);
    strftime(minute, sizeof(minute), "%M", &timeinfo);
    strftime(second, sizeof(second), "%S", &timeinfo);
    string_day = "/" + String(day) + " " + String(dayofmonth);
    string_month = "/" + String(month);
    string_time = "/" + String(hour) + ":" + String(minute) + ":" + String(second);
  }
}

void dhtt11_init(){
  dht.begin();
  // Initialise temprature json data
  temperature_json.add("name", "Temperature");
  temperature_json.add("value", temperature);
  // Initialise humidity json data
  humidity_json.add("name", "Humidity");
  humidity_json.add("value", humidity);
  // Print out initial temperature values
  String jsonStr;
  temperature_json.toString(jsonStr, true);
  Serial.println(jsonStr);
  String jsonStr2;
  humidity_json.toString(jsonStr2, true);
  Serial.println(jsonStr2);
}

void irrigationSystem(){
  groundhumidity();
  float current_humidity = ground_humidity;

  if(current_humidity < MIN_HUMIDITY){
    Serial.println("ta seco");
    digitalWrite(RELAY_PIN, HIGH);
    delay(2000);
    digitalWrite(RELAY_PIN, LOW);
  } else if(current_humidity <= MAX_HUMIDITY){
    Serial.println("ta bien");
  } else {
    Serial.println("???");
  }
}

void setup() {
  // Initialise serial communication for local diagnostics
  Serial.begin(115200);
  // Initialise Connection with location WiFi
  Wifi_Init();
  // Initialise firebase configuration and signup anonymously
  firebase_init();
  //initialize date configuration
  configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);
  // Initialise DHTT11 module
  dhtt11_init();
  //Ground humidity capture starts
  pinMode(GROUND_HUMIDITY_PIN,INPUT);
  groundhumidity_init();
  //Relay pin starts
  pinMode(RELAY_PIN, OUTPUT);
}

void updateSensorReadings(){
  Serial.println("------------------------------------");
  Serial.println("Reading Sensor data ...");
  groundhumidity();
  getCurrentDate();
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
  Serial.println(string_month);
  Serial.println(string_day);
  temperature_json.set("value", temperature);
  humidity_json.set("value", humidity);
  ground_humidity_json.set("value", ground_humidity);
}

void uploadSensorData() {
  if (millis() - elapsed_millis > update_interval && isAuthenticated && Firebase.ready())
    {
      elapsed_millis = millis();

      updateSensorReadings();
      String temperature_node = databasePath + dht11_path + "/temperature";
      String humidity_node = databasePath + dht11_path + "/humidity";
      String ground_humidity_node = databasePath + ground_humidity_path + "/ground_humidity";
      //String temperature_node = databasePath + dht11_path + string_month + string_day + string_time;  
      //String humidity_node = databasePath + dht11_path + string_month + string_day + string_time; 
      //String ground_humidity_node = databasePath + ground_humidity_path + string_month + string_day + string_time;
      if (Firebase.setJSON(fbdo, temperature_node.c_str(), temperature_json))
      {
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.print("VALUE: ");
          printResult(fbdo); //see addons/RTDBHelper.h
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
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.print("VALUE: ");
          printResult(fbdo); //see addons/RTDBHelper.h
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
        Serial.println("PATH: " + fbdo.dataPath());
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
