#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "base64.h"

/*
********************************************
Get distance and humidity from GPIO pins (Analog and Digital)
Then get favorite devices from Domoticz locally
And finally send values to corresponding idx in Domoticz from given names
********************************************
*/

// defines pins numbers
const int trigPin = 4;  //D2
const int echoPin = 0;  //D3
const int analogInPin = A0;

// WiFi parameters to be configured
const char* ssid = "xxx";
const char* password = "xxx";

const char* host = "192.168.1.x";
const int   port = 8080;
String authUsername = "xxx";
String authPassword = "xxx";
const int   watchdog = 60000; // Frequency for sending data to Domoticz -- 60000 is enough (1 minute), could be less
HTTPClient http;

// defines variables
long duration;
int distance;
int humidity;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(115200); // Starts the serial communication 9600 or 74880 or 115200 ...
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting wifi.");
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {

  humidity = map(analogRead(analogInPin), 440, 860, 100, 0);
  Serial.println("Value analogPin : ");
  Serial.println(humidity);
 
 // id WL1 = 423 for humidity
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected !");
  } else {  
    String nameIdx = "HumTomato2";
    int idxHum = getIdx(nameIdx);
    String url = "/json.htm?type=command&param=udevice&idx=" + String(idxHum) + "&nvalue=0&svalue=" + String(humidity);
    if(idxHum > 0) 
    {
      sendDomoticz(url);
    }
  }
  
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
 
 // id WL1 = 279 for distance
  
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected !");
  } else { 
    String nameIdx = "WaterTomato2";
    int idxDist = getIdx(nameIdx);
    Serial.println("Got Idx ? ");
    Serial.println(idxDist);
    String url = "/json.htm?type=command&param=udevice&idx=" + String(idxDist) + "&nvalue=0&svalue=" + String(distance);
    if(idxDist > 0) 
    {
      sendDomoticz(url);
    }
  }

 delay(watchdog);
}

int getIdx(String idxName){
  DynamicJsonDocument jsonDoc(20480);
  // 20480 Should be enough for about 20 favorite devices in Domoticz (around 1024 per device), go to https://arduinojson.org/v6/assistant/
  Serial.print("Getting name :");
  Serial.println(idxName);
  Serial.print("Requesting URL: ");
  String url = "/json.htm?type=devices&used=true&filter=all&favorite=1"; // To get favorites devices in Domoticz, refer to doc to filter
  http.begin(host,port,url);
  String auth = base64::encode(authUsername + ":" + authPassword);
  http.addHeader("Authorization", "Basic " + auth);
  int httpCode = http.GET();
    if (httpCode) {
      if (httpCode == 200) {
        String payload = http.getString();
        Serial.print("Domoticz get Json response "); 
        DeserializationError err = deserializeJson(jsonDoc, payload);
        if (err) {
          Serial.print(F("deserializeJson() failed with code "));
          Serial.println(err.c_str());
        }
        JsonArray arr = jsonDoc["result"].as<JsonArray>();
        Serial.print("Domoticz array result Json ? ");
        Serial.println(arr);
        for (JsonObject repo : arr) {
          const String name = repo["Name"];
          if(name == idxName)
          {
            Serial.print("FOUND Name idx !");
            Serial.println(repo);
            return repo["idx"];
          }
        }
      }
      else{
        Serial.println("Domoticz request error..."); 
        return 0;
      }
    }
  Serial.println("closing connection");
  http.end();
}

void sendDomoticz(String url){
  Serial.print("connecting to ");
  Serial.println(host);
  Serial.print("Requesting URL: ");
  Serial.println(url);
  http.begin(host,port,url);
  String auth = base64::encode(authUsername + ":" + authPassword);
  http.addHeader("Authorization", "Basic " + auth);
  int httpCode = http.GET();
    if (httpCode) {
      if (httpCode == 200) {
        String payload = http.getString();
        Serial.println("Domoticz response "); 
        Serial.println(payload);
      }
      else
        Serial.println("Domoticz request error..."); 
    }
  Serial.println("closing connection");
  http.end();
}
