#include <NTPClient.h>
#include <WiFiUdp.h>
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

// Insert your network credentials
#define WIFI_SSID "PPC-b2f0"
#define WIFI_PASSWORD "manikandhan5592"
// Insert Firebase project API Key 
#define API_KEY "AIzaSyAuRidtTJ-Bl3bevsIq6k8jJ-nYoJgk1T8"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "ledtest-87050-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//some importent variables
String sValue, sValue2;
bool signupOK = false;



//udp
// Replace with the NTP server's address and time zone
const char* ntpServer = "pool.ntp.org";
const long utcOffsetInSeconds = 19800; // Replace with your UTC offset in seconds
 // Replace with your DST offset in seconds

// Initialize the Wi-Fi client and the NTP client
WiFiClient wifiClient;
WiFiUDP udp;
NTPClient ntpClient(udp, ntpServer, utcOffsetInSeconds);


void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
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
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
//udp
 // Initialize the NTP client
  ntpClient.begin();

  
}

void loop() {

//udp
  ntpClient.update();

  // Get the current time from the NTP client
  unsigned long epochTime = ntpClient.getEpochTime();
  struct tm* currentTime = localtime((time_t*)&epochTime);

  // Print the current time in the serial monitor
  char timeString[20];
  sprintf(timeString, "%02d:%02d", currentTime->tm_hour, currentTime->tm_min);
//firebase
  if (Firebase.ready() && signupOK ) {
    if (Firebase.RTDB.getString(&fbdo, "/L1")) {
      if (fbdo.dataType() == "string") {
        sValue = fbdo.stringData();
        Serial.println(timeString);
        delay(500);
        if (sValue == timeString)
        {
     Serial.println("alarm now");
     delay(2000);

        }
      }
    }
    else {  
      Serial.println(fbdo.errorReason());
    }
  }
}
