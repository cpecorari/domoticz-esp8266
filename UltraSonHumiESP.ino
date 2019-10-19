#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "base64.h"

/*
********************************************
14CORE ULTRASONIC DISTANCE SENSOR CODE TEST
********************************************
*/
// defines pins numbers

const int trigPin = 4;  //D2 --- yes
const int echoPin = 0;  //D3 --- yes
const int analogInPin = A0;

// WiFi parameters to be configured
const char* ssid = "xxx";
const char* password = "xxx";

const char* host = "192.168.1.NN";
const int   port = 8080;
String authUsername = "xxx";
String authPassword = "xxx";
const int   watchdog = 120000; // Fréquence d'envoi des données à Domoticz - Frequency of sending data to Domoticz -- 120000 is enough !
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
    String url = "/json.htm?type=command&param=udevice&idx=423&nvalue=0&svalue=";
    url += humidity;
    sendDomoticz(url);
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
    String url = "/json.htm?type=command&param=udevice&idx=279&nvalue=0&svalue=";
    url += distance;
    sendDomoticz(url);
  }

 delay(watchdog);
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
