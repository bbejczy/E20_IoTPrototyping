#include "ThingSpeak.h"
#include "network_and_channel.h"
#include <ESP8266WiFi.h>

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
//int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;


// Channel details
unsigned long ChannelNumber = SECRET_CH_ID;
const char * myReadAPIKey = SECRET_READ_APIKEY;
unsigned int FieldNumber = 1; 

void setup() {
  Serial.begin(115200);  // Initialize serial

  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  int statusCode = 0;
  
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected");
  }


  // Read in field 1 of the private channel    
  float temperatureC = ThingSpeak.readFloatField(ChannelNumber, FieldNumber, myReadAPIKey); 
  float  temperatureF = ThingSpeak.readFloatField(ChannelNumber, 2, myReadAPIKey);

   // Check the status of the read operation to see if it was successful
  statusCode = ThingSpeak.getLastReadStatus();
  if(statusCode == 200){
    Serial.println("Temperature: " + String(temperatureC) + " deg C " + String(temperatureF) + "deg F");
  }
  else{
    Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
  }
  
  delay(15000); // No need to read the datas too often.
  
}
