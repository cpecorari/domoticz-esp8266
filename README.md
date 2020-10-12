# domoticz-esp8266
ESP8266 codes for sending sensor values to Domoticz

Don't forget to configure your wifi, and Domoticz has to be configured in "Basic-Auth" or avoid auth from local IPs.

```
// WiFi parameters to be configured
const char* ssid = "xxx"; 
const char* password = "xxx";

const char* host = "192.168.1.NN";  // Your Domoticz Server IP
const int   port = 8080;            // Your Domoticz Server Port
String authUsername = "xxx";        // Your Domoticz 
String authPassword = "xxx";        // Your Domoticz Password
```

## For Electric Version
Ids in Domoticz : Now finding id from its name in Domoticz, don't forget to change your name in code or in Domoticz to find it !

My colors to remember, BORDEAUX = VDD (5v in nodeMcu) // JAUNE = GND // bleu RX // vert TX
DONT FORGET to 'invert' TX and RX between PZEM and ESP8266
