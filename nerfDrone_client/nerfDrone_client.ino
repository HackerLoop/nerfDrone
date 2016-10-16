

#include <ESP8266WiFi.h>

const char* ssid     = "nerfDrone";
const char* password = "nerfdrone";
const char* host = "192.168.4.1";

int motorsPin = D1;
int shotPin = D2;
int blastPin = D3;

bool motorsState = false;
bool shotState = false;
bool blastState = false;

String endpoint = String();

void setup() 
{
  initHardware();
  delay(100);
  connectWifi();
}

void loop() {
  if (edge(motorsPin, &motorsState)) {
    request(motorsState ? "/motors/on" : "/motors/off");
  }
  if (edge(shotPin, &shotState)) {
    if (shotState) {
      request("/shotState/on");
    }
  }
  if (edge(blastPin, &blastState)) {
    if (blastState) {
      request("/blast/on");
    }
  }
  delay(200);
}

bool edge(int pin, bool *currentState) {
  bool pinState = digitalRead(pin);
  if ( pinState == HIGH && !currentlyOn ) {
    *currentlyOn = true;
    return true;
  } else if ( pinState == LOW && currentlyOn ) {
    *currentlyOn = false;
    return true;
  }
  return false;
}

void initHardware() {
  Serial.begin(115200);
  pinMode(motorsPin, INPUT);
  pinMode(shotPin, INPUT);
  pinMode(blastPin, INPUT);
}

void connectWifi() {

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void request(String endpoint) {
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = endpoint;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}

