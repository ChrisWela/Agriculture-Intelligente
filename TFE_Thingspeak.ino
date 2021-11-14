
#include <Adafruit_ESP8266.h>
#include <SoftwareSerial.h>

#define ESP_RX 3
#define ESP_TX 4
#define ESP_RST 8
SoftwareSerial softser(ESP_RX, ESP_TX);

// Must declare output stream before Adafruit_ESP8266 constructor; can be
// a SoftwareSerial stream, or Serial/Serial1/etc. for UART.
Adafruit_ESP8266 wifi(&softser, &Serial, ESP_RST);
// Must call begin() on the stream(s) before using Adafruit_ESP8266 object.

#define ESP_SSID “ChrisTFE” // Your network name here
#define ESP_PASS “12345678” // Your network password here

String API = “PLANTE”; // CHANGE ME
#define HOST“chriswela22@gmail.com” // Find/Google your email provider’s SMTP outgoing server name for unencrypted email

#define PORT 80 // Find/Google your email provider’s SMTP outgoing port for unencrypted email
String field =“field1”;
int count = 0; // we’ll use this int to keep track of which command we need to send next
bool send_flag = false; // we’ll use this flag to know when to send the email commands
#define SOLPIN 5 // // pour le capteur de l’humidité du sol
int analogVal;

void setup() {
char buffer[50];

// This might work with other firmware versions (no guarantees)
// by providing a string to ID the tail end of the boot message:

// comment/replace this if you are using something other than v 0.9.2.4!
wifi.setBootMarker(F(“Version:0.9.2.4]\r\n\r\nready”));

softser.begin(9600); // Soft serial connection to ESP8266
Serial.begin(57600); while(!Serial); // UART serial debug

// Test if module is ready
Serial.print(F(“Hard reset…”));
if(!wifi.hardReset()) {
Serial.println(F(“no response from module.”));
for(;;);
}
Serial.println(F(“OK.”));

Serial.print(F(“Soft reset…”));
if(!wifi.softReset()) {
Serial.println(F(“no response from module.”));
for(;;);
}
Serial.println(F(“OK.”));

Serial.print(F(“Checking firmware version…”));
wifi.println(F(“AT+GMR”));
if(wifi.readLine(buffer, sizeof(buffer))) {
Serial.println(buffer);
wifi.find(); // Discard the ‘OK’ that follows
} else {
Serial.println(F(“error”));
}

Serial.print(F(“Connecting to WiFi…”));
if(wifi.connectToAP(F(ESP_SSID), F(ESP_PASS))) {

// IP addr check isn’t part of library yet, but
// we can manually request and place in a string.
Serial.print(F(“OK\nChecking IP addr…”));
wifi.println(F(“AT+CIFSR”));
if(wifi.readLine(buffer, sizeof(buffer))) {
Serial.println(buffer);
wifi.find(); // Discard the ‘OK’ that follows

Serial.print(F(“Connecting to host…”));

Serial.print(“Connected..”);
wifi.println(“AT+CIPMUX=0”); // configure for single connection,
//we should only be connected to one SMTP server
wifi.find();
wifi.closeTCP(); // close any open TCP connections
wifi.find();
Serial.println(“Type \”send it\” to send an email”);

} else { // IP addr check failed
Serial.println(F(“error”));
}
} else { // WiFi connection failed
Serial.println(F(“FAIL”));
}
}

void loop() {
send_flag = true;
if(send_flag){ // the send_flat is set, this means we are or need to start sending SMTP commands
if(do_next()){ // execute the next command
count++; // increment the count so that the next command will be executed next time.
}
}
}

boolean do_next()
{
switch(count){
case 0:
Serial.println(“Connecting…”);
return wifi.connectTCP(F(HOST), PORT);
break;
case 1:
analogVal = analogRead(SOLPIN); // lecture de l’humdité du sol
char charVal2[100];
char charVal1[100]= "GET https://api.thingspeak.com/update?api_key=WWN8GZNEXT7RMEWS&field1=0";
itoa(analogVal, charVal2, 10);
//dtostrf(analogVal, 4, 2, charVal2);
strcat(charVal1,charVal2);
return wifi.Send(charVal1); // Envoi des données au site Thinkspeak
delay(60000);
}
}
