/** 
Côme Pecorari
No Licence at all, feel free to copy, as I did inspire myself from others to create this one :)
Cheerz
**/

#define USE_MDNS true

#include <PZEM004Tv30.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "base64.h"

#include "RemoteDebug.h"  //https://github.com/JoaoLopesF/RemoteDebug -- Wifi debug helps a lot !!

#ifdef USE_MDNS
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#endif

#define HOST_NAME "pecodebug"

// WiFi parameters to be configured
const char* ssid = "xxx";
const char* password = "xxx";

const char* host = "192.168.1.NN";
const int   port = 8080;
String authUsername = "xxx";
String authPassword = "xxx";
const int   watchdog = 120000; // Frequency of sending data to Domoticz -- 120000 is enough !
HTTPClient http;

PZEM004Tv30 pzem(3, 1);           // Attach PZEM to hwserial

bool pzemrdy = false;
float i_=0; 
float v_=0;
float p_=0;
float e_=0;

RemoteDebug Debug;

void setup ()
{  
  Serial.begin(115200);
  //pzem.setPowerAlarm(ip, 12000);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
  }

  String hostNameWifi = HOST_NAME;
  hostNameWifi.concat(".local");
  WiFi.hostname(hostNameWifi);
  #ifdef USE_MDNS  // Use the MDNS ?
  MDNS.addService("telnet", "tcp", 23);
  #endif
  
  Debug.begin(HOST_NAME);
  Debug.showColors(true); // Colors

// TUTORIAL used partly for PZEM http://pdacontrolen.com/electricity-consumption-meter-peacefair-pzem-004-esp8266-arduino-nano/

}

void loop ()
{
  debugA("* Beginning loop !");

  float v = pzem.voltage();
  if (v > 0.0)   {v_=v;}
  debugA("v_ value : %f", v);

  float i = 1.2102;//pzem.current();
  if(i >= 0.0){ i_=i; }
  debugA("i_ value : %f", i);

  float p = pzem.power();
  if(p >= 0.0){ p_=p;}
  debugA("p_ value : %f", p);
  
  float e = pzem.energy();
  if(e >= 0.0){e_=e;}
  debugA("e_ value : %f", e);

   // My ids in DOMOTICZ : id WL1 = 428 for conso & power // 454 for Voltage ---> Still don't know how to choose IDs in Domoticz... So change it here when you created your dummy device in Domoticz !
   // (My colors to remember) DONT FORGET to 'invert' TX and RX between PZEM and ESP8266 == BORDEAUX = VDD // JAUNE = GND // bleu RX // vert TX
  if(WiFi.status() == WL_CONNECTED) {
    String url = "/json.htm?type=command&param=udevice&idx=454&nvalue=0&svalue=";
    url.concat(v_);
    debugA("v_ value : %f", v_);
    debugA("URL Sent : %s", url.c_str());
    sendDomoticz(url);
    delay(500);
    String url2 = "/json.htm?type=command&param=udevice&idx=428&nvalue=0&svalue="; 
    url2.concat(p_);
    url2.concat(";");
    url2.concat(e_);
    sendDomoticz(url2);
  }

  Debug.handle();

  delay(watchdog);

}

void sendDomoticz(String url){
  
  http.begin(host,port,url);
  String auth = base64::encode(authUsername + ":" + authPassword);
  http.addHeader("Authorization", "Basic " + auth);
  int httpCode = http.GET();
    if (httpCode) {
      if (httpCode == 200) {
        String payload = http.getString();
      }
    }
  http.end();
}
