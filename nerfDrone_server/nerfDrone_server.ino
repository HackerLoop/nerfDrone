#include <ESP8266WiFi.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "nerfdrone";

/////////////////////
// Pin Definitions //
/////////////////////

const int motor = D3;
const int trigger = D4;

WiFiServer server(80);

void setup() 
{
  initHardware();
  setupWiFi();
  server.begin();
}

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  if (req.indexOf("/motors/off") != -1)
    val = 0; 
  else if (req.indexOf("/motors/on") != -1)
    val = 1; 
  else if (req.indexOf("/shot") != -1)
    val = -2; 
  else if (req.indexOf("/blast") != -1)
    val = -3; 
    
  if (val == 0)
    digitalWrite(motor, HIGH);
  else if (val == 1)
    digitalWrite(motor, LOW);
  else if (val == -2)
    triggerShot();
  else if (val == -3)
    triggerBlast();
    

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  // If we're setting the LED, print out a message saying we did
  if (val == 0)
  {
    s += "motors off";
  }
  else if (val == 1)
  { 
    s += "motors on";
  }
  else if (val == -2)
  { 
    s += "Triggered a shot";
  }
  else if (val == -3)
  { 
    s += "Triggered a blast";
  }
  else
  {
    s += "Invalid Request.<br> Try /led/1, /led/0, or /read.";
  }
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}



void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "NerfDrone " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware()
{
  Serial.begin(115200);
  pinMode(motor, OUTPUT);    
  pinMode(trigger, OUTPUT);
  digitalWrite(trigger, HIGH);     
}

void triggerShot()
{
  digitalWrite(trigger, LOW);
  delay(200);
  digitalWrite(trigger, HIGH);
}

void triggerBlast()
{
  digitalWrite(trigger, LOW);
  delay(500);
  digitalWrite(trigger, HIGH);
}

