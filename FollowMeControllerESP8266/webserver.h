#include <ESP8266WiFi.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "FollowMe";
WiFiServer server(80);


// Data sent by the follow-me tag
// The sizeof this struct should not exceed 32 bytes
struct MyData {
  int32_t lon;  // deg * 10000000
  int32_t lat;  // deg * 10000000
  int32_t alt;  // cm
  int32_t velN; // cm/s
  int32_t velE; // cm/s
};

String followMeMode = "";
String followMeState = "";
MyData data;

int ledCounter = 0;
int packetsRead = 0;

int packetCounts[10];
int packetCountIndex = 0;
int packetCountTotal = 0;

boolean ledOn = false;
boolean newRadioData = false; 

void toggleLed() {
  ledOn = ! ledOn;
  digitalWrite(LED_BUILTIN, ledOn);
}

void initHardware() {
  //Serial.begin(115200); - what serial port are we using to debug?
  pinMode(LED_BUILTIN, OUTPUT);
}

void setupWiFi() {
  WiFi.mode(WIFI_AP);

  // Add the last two bytes of the MAC (HEX) to SSID:
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  
  String AP_NameString = "Follow Me - " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void radio_setup() 
{
  data.lat = 0;
  data.lon = 0;
  
  initHardware();
  setupWiFi();
  server.begin();
}

String responseHeader = "HTTP/1.1 200 OK\r\n Content-Type: text/html\r\n\r\n <!DOCTYPE HTML>\r\n<html>\r\n";

void radio_loop() {
  int32_t oldLat = data.lat;
  int32_t oldLon = data.lon;
  int32_t oldAlt = data.alt;

  //Serial.println();
  //Serial.println("Radio Loop");

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String response = responseHeader;
  String req = client.readStringUntil('\r');
  //Serial.print("Client Request:");
  //Serial.println(req);
  client.flush();

  // if a request has been received compare it to what we are expecting
  if (req.indexOf("lat=") != -1) {
    int startPos = req.indexOf("lat=")+4;
    data.lat = (int32_t)(req.substring(startPos, req.indexOf("&lon")).toFloat()*10000000);
    response += "Lat updated: ";
    response += String(data.lat);
    response += "<br>";
  }
  if (req.indexOf("lon=") != -1) {
    int startPos = req.indexOf("lon=")+4;
    data.lon = (int32_t)(req.substring(startPos, req.indexOf("&alt")).toFloat()*10000000);
    response += "Lon updated: ";
    response += String(data.lon);
    response += "<br>";
  }
  if (req.indexOf("alt=") != -1) {
    int startPos = req.indexOf("alt=")+4;
    data.alt = (int32_t)(req.substring(startPos, req.indexOf("&mode")).toInt());
    response += "Alt updated: ";
    response += String(data.alt);
    response += "<br>";
  }
  if (req.indexOf("mode=") != -1) {
    int startPos = req.indexOf("mode=")+5;
    followMeMode = req.substring(startPos, req.indexOf(" HTTP"));
    response += "Mode updated to:";
    response += followMeMode;
    response += "<br>";
  }
  if (req.indexOf("followme=") != -1) {
    int startPos = req.indexOf("followme=")+9;
    followMeState = req.substring(startPos, req.indexOf(" HTTP"));
    response += "Follow Me:";
    response += followMeState;
    response += "<br>";
  }
  
  // Send the response to the client
  //Serial.println(response);
  client.print(response);
  //delay(1);

  toggleLed();
  
  if ( oldLat != data.lat || oldLon != data.lon || oldAlt != data.alt )
    newRadioData = true;

}

