#include "Arduino.h"
#include <WiFi.h>
#include <AsyncUDP.h>
#include "osc.h"
#include <Adafruit_ADS1X15.h>

//amp
Adafruit_ADS1115 ads;  

//WIFI
const char *wifi_ssid = "TheNicks";
const char *wifi_password = "xxx";

//Multicast address to TX to
IPAddress multicast_ip_address = IPAddress(239,1,1,1);    //Muticast IP
float exampleData[2] = {-4096.0,0};

//OSC Vars
osc oscObject;
AsyncUDP oscUDP;
unsigned int oscRXPort = 5555;    //port that this device will RECIVE packets on
unsigned int oscTXPort = 8000;    //port that this device will SEND packets to
const char *oscTXAddress = "/geophone/raw";

byte INDICATOR_LED = LED_BUILTIN;


float fps=100;
int delayTime=1000/fps;

void setup() {
  pinMode(INDICATOR_LED, OUTPUT);
  Serial.begin(115200);
  Serial.println("Geophone waking up...");
  Serial.println("attempting to connect to wifi network with ssid " + String(ssid));
  ads.setGain(GAIN_ONE);

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

  //Eable & connect to WIFI
  WiFi.mode(WIFI_STA);
 WiFi.begin(wifi_ssid, wifi_password);
//  WiFi.begin(wifi_ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.printf(".");
    delay(100);
  }
  delay(100);
  Serial.printf("\r\n\tConnected to WIFI\t");
  Serial.print(WiFi.localIP());

//Is this going to cause issues if something else publishes on the multicast ip? NPE or something?
  //Set up UDP server for reciving UDP on port oscRXPort
  // oscUDP.listenMulticast(multicast_ip_address, oscRXPort);
  //Register method that handles INCOMING OSC data
  // oscUDP.onPacket(handleData);

  //Set up your OSC objects by adding in the controll names eg here we add /slider1 /slider2 /rotary1 /fader1
  oscObject.addControll((char*)"/geophone/raw", 1, 'f');
  digitalWrite(INDICATOR_LED, HIGH);

}

void loop() 
{
  exampleData[0]+=50;
  oscObject.setValue((char*)"/geophone/raw", exampleData[0]);

  oscObject.generateOSCPacket((char*)"/geophone/raw");
  oscUDP.writeTo((uint8_t*)oscObject.txPacketBuffer, oscObject.txPacketBufferLength, multicast_ip_address, oscTXPort);

  if(exampleData[0]>4096)
  {
    exampleData[0]=-4096;
  }


  delay(delayTime);
}

//debug to check your packet
void dumpString(char* tempString, unsigned short int stringLength)
{
  unsigned short int stringCnt=0;
  for(stringCnt=0; stringCnt<stringLength; stringCnt++)
  {
    Serial.printf("[%d]->(%c)", tempString[stringCnt], tempString[stringCnt]);
  }
}

void handleData(AsyncUDPPacket packet)
{
  memcpy(oscObject.packetBuffer, packet.data(), packet.length());
  oscObject.currentPacketSize = packet.length();
  oscObject.toggleState();
  oscObject.parseOSCPacket();
  Serial.printf("\r\n\tGot\t%d\tBytes", packet.length());
}
