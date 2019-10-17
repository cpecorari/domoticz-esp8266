# domoticz-esp8266
ESP8266 codes for sending sensor values to Domoticz

Don't forget to configure your wifi, and Domoticz has to be configured in "Basic-Auth".

```
// WiFi parameters to be configured
const char* ssid = "xxx"; 
const char* password = "xxx";

const char* host = "192.168.1.NN";  // Your Domoticz Server IP
const int   port = 8080;            // Your Domoticz Server Port
String authUsername = "xxx";        // Your Domoticz 
String authPassword = "xxx";        // Your Domoticz Password
```
