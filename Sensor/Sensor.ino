/*
   Lora to Lora (P2P) using RN2484 and ESP8266
   This sketch will transmit a lora message with payload "20" every 2 seconds.
   An LED will blink every time a package is sent. Payload and package ID is printed on serial monitor.

   Code based on JP Meijers code from 30 September 2016
   MN Petersen, Aug 2020

   CONNECTIONS:
   RN2483 - ESP8266:
   TX     - D5
   RX     - D6
   3V3    - 3V3
   GND    - GND

   D7 on ESP8266 is connected to anode (long leg) on LED. Insert resistor between cathode and GND on ESP8266.

*/

#include <SoftwareSerial.h>
SoftwareSerial loraSerial(14, 12, false); // (rxPin (D5), txPin (D6), inverse_logic, buffer size);

String str;
String frequency = "869100000";
String sf = "sf12";
String message = "48656c6C6F";
String receivedMessage;

int currentMillis;
int startingMillis;
int deltaTime;

#define red 0     // D3
#define yellow 5   // D1
#define green 4    // D2


void setup() {
  //output LED pin
  pinMode(13, OUTPUT);  // D7 on ESP8266

  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);

  // Open serial communications and wait for port to open:
  Serial.begin(57600);  // Serial communication to PC

  loraSerial.begin(9600);  // Serial communication to RN2483
  loraSerial.setTimeout(1000);
  lora_autobaud();


  InitialisingLora(frequency, sf);

  //LED
  LED_sensor();

  Serial.println("starting loop");
}

//=============================================================
// LOOP
//=============================================================
void loop() {
  // send message for defined time
  startingMillis = millis();
  int sendingTime = 10000;
  currentMillis = millis();
  deltaTime = currentMillis - startingMillis;

  while (deltaTime <= sendingTime){
    sendMessage(message);
    currentMillis = millis();
    deltaTime = currentMillis - startingMillis;
    yield(); // so watchdog doesnt reset controller
    delay(100);
    }

  
  receivedMessage = receiveMessage();
  if (receivedMessage != ""){
    Serial.print("Received Message: ");
    Serial.println(receivedMessage);
    delay(10000);
  }
  else{
    Serial.println("No message received");
    Serial.println("Sleep for short time");
    delay(5000);
  }
}


//=============================================================
// Function declarations
//=============================================================

void lora_autobaud()
{
  String response = "";
  while (response == "")
  {
    delay(1000);
    loraSerial.write((byte)0x00);
    loraSerial.write(0x55);
    loraSerial.println();
    loraSerial.println("sys get ver");
    response = loraSerial.readStringUntil('\n');
  }
}


int wait_for_ok() {
  /*
    This function blocks until the word "ok\n" is received on the UART,
    or until a timeout of 3*5 seconds.
  */
  str = loraSerial.readStringUntil('\n');
  if ( str.indexOf("ok") == 0 ) {
    return 1;
  }
  else return 0;
}


void led_on(int LED)
{
  digitalWrite(LED, 1);
}

void led_off(int LED)
{
  digitalWrite(LED, 0);
}



void InitialisingLora(String frequency, String sf) {
  Serial.println("Initing LoRa");

  loraSerial.println("radio rxstop"); // causes the radio to exit Continuous Receive mode
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.listen();
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  loraSerial.println("sys get ver");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.println("mac pause");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  //  loraSerial.println("radio set bt 0.5");  // Uncomment if we want to use FSK
  //  wait_for_ok();

  loraSerial.println("radio set mod lora"); // Comment if we want to use FSK
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.println("radio set freq "  + frequency);
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.println("radio set pwr 14");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.println("radio set sf " + sf);
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.println("radio set afcbw 41.7");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.println("radio set rxbw 125");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  //  loraSerial.println("radio set bitrate 50000");
  //  wait_for_ok();

  //  loraSerial.println("radio set fdev 25000");
  //  wait_for_ok();

  loraSerial.println("radio set prlen 8");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.println("radio set crc on");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.println("radio set iqi off");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.println("radio set cr 4/5");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.println("radio set wdt 60000"); //disable for continuous reception
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.println("radio set sync 12");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  loraSerial.println("radio set bw 125");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
}

void sendMessage(String message) {
  led_on(red);
  Serial.print("Sending Message: ");
  Serial.println(message);
  delay(500);
  Serial.println("TX: radio tx");
  loraSerial.print("radio tx ");
  loraSerial.println(message);
  str = loraSerial.readStringUntil('\n');
  Serial.print("RX: ");
  Serial.println(str);
  str = loraSerial.readStringUntil('\n');
  Serial.print("RX: ");
  Serial.println(str);
  delay(1000);
  led_off(red);

}

String receiveMessage() {
  led_on(green);
  // intialise emtpy message
  String message = "";
  Serial.println("Receving Message");
  delay(1000);
  Serial.println("TX: radio rx 0");
  loraSerial.println("radio rx 0"); //wait for 60 seconds to receive
  str = loraSerial.readStringUntil('\n');
  Serial.print("RX: ");
  Serial.println(str);
  delay(200);

  if ( str.indexOf("ok") == 0 )
  {
    str = String("");
    while (str == "")
    {
      str = loraSerial.readStringUntil('\n');
    }
    if ( str.indexOf("radio_rx") == 0 )  //checking if data was reeived (equals radio_rx = <data>). indexOf returns position of "radio_rx"
    {
      //      toggle_led();
      Serial.println(str); //printing received data
      message = str;
    }
    else
    {
      Serial.println("Received nothing");
    }
  }
  else if (str.indexOf("busy") == 0 ) {
    Serial.println("radio not going into receive mode");
    Serial.println("  Stop the radio and mac pause");
    delay(1000);
    // dont know if mac pause is needed, but it is required to be in init
    loraSerial.println("mac pause");
    str = loraSerial.readStringUntil('\n');
    Serial.println(str);

    loraSerial.println("radio rxstop"); // causes the radio to exit Continuous Receive mode
    str = loraSerial.readStringUntil('\n');
    Serial.println(str);

  }
  else
  {
    Serial.println("radio not going into receive mode");
    delay(1000);
  }
  led_off(green);
  return message;
}

void LED_sensor(){
  // Central Box LED show
  led_off(green);
  led_off(yellow);
  led_off(red);
  delay(500);
  led_on(red);
  led_on(yellow);
  led_on(green);
  delay(250);
  led_off(green);
  delay(250);
  led_off(yellow);
  delay(250);
  led_off(red);
  
}
