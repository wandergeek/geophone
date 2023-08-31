#include <Adafruit_ADS1X15.h>
#include <ArduinoOSCWiFi.h>

byte INDICATOR_LED = LED_BUILTIN;

Adafruit_ADS1115 ads;  

// WiFi stuff
const char* ssid = "ipsum";
// const char* pwd = "testinggrounds";
// const IPAddress ip(192, 168, 1, 250); //if you want to use these, uncomment this and the line further down
// const IPAddress gateway(192, 168, 1, 1);
// const IPAddress subnet(255, 255, 255, 0);

// for ArduinoOSC
// const char* host = "255.255.255.255"; //this is a bit naughty, but it vastly simplifies things. If you want unicast, just chuck an IP here instead.
const char* host = "192.168.2.14"; 
const int send_port = 8000;
const int publish_port = 8000;
float results;


void setup(void) {
  pinMode(INDICATOR_LED, OUTPUT);
  Serial.begin(115200);
  Serial.println("Geophone waking up...");
  Serial.println("attempting to connect to wifi network with ssid " + String(ssid));

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
 // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
  
  // WiFi stuff (no timeout setting for WiFi)
#ifdef ESP_PLATFORM
    WiFi.disconnect(true, true);  
    delay(1000);
    WiFi.mode(WIFI_STA);
#endif
   WiFi.begin(ssid);

  //  WiFi.begin(ssid, pwd); //if you're using a password, you'll need to instantiate this way and comment out the line above. I could write a switch but Im lazy as shit
   // WiFi.config(ip, gateway, subnet); //uncomment this if you want to use static network settings
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.print("WiFi connected, IP = ");
    Serial.println(WiFi.localIP());

    OscWiFi.publish(host, publish_port, "/publish/analogInput", results)
        ->setFrameRate(60.f); // this is probably too aggressive, but it's a good starting point for testing

  digitalWrite(INDICATOR_LED, HIGH);
  }

void loop(void) {
  OscWiFi.update();  
  float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */
  results = ads.readADC_Differential_0_1();
  // results = map(ads.readADC_Differential_0_1(), -2500,2500,0, 1);
  // Serial.println(results);
}
