#include "Arduino.h"
#include <WiFi.h>
#include <AsyncUDP.h>
#include "osc.h"
#include <Adafruit_ADS1X15.h>

byte INDICATOR_LED = LED_BUILTIN;

//amp
Adafruit_ADS1115 ads;  

//WIFI
const char *wifi_ssid = "ipsum";
const char *wifi_password = ""; //leave blank if no password

//Multicast address to TX to
IPAddress multicast_ip_address = IPAddress(239,1,1,1);    //Muticast IP

//OSC Vars
osc oscObject;
AsyncUDP oscUDP;
unsigned int oscTXPort = 8000;    //port that this device will SEND packets to
char *oscTXAddress = "/geophone/raw";

int FPS=60; //updates per second

void setup() {
  pinMode(INDICATOR_LED, OUTPUT);

  Serial.begin(115200);
  Serial.println("Geophone waking up...");
  Serial.println("attempting to connect to wifi network with ssid " + String(wifi_ssid));

  ads.setGain(GAIN_ONE);
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

  WiFi.mode(WIFI_STA);
  if (strlen(wifi_password) == 0) {
    WiFi.begin(wifi_ssid);
  } else {
    WiFi.begin(wifi_ssid, wifi_password);
  }
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(100);
  }
  delay(100);
  Serial.printf("\r\n\tConnected to WIFI\t");
  Serial.print(WiFi.localIP());

  oscObject.addControll(oscTXAddress, 1, 'f');
  
  digitalWrite(INDICATOR_LED, HIGH);
}


unsigned long lastRunTime = 0;
int minElapsedTime = 1000/FPS;
void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastRunTime >= minElapsedTime) {
    oscObject.setValue(oscTXAddress,ads.readADC_Differential_0_1());
    oscObject.generateOSCPacket(oscTXAddress);
    oscUDP.writeTo((uint8_t*)oscObject.txPacketBuffer, oscObject.txPacketBufferLength, multicast_ip_address, oscTXPort);

    lastRunTime = currentTime;
  }
}
