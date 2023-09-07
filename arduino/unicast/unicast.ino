#include <Adafruit_ADS1X15.h>
#include <ArduinoOSCWiFi.h>

Adafruit_ADS1115 ads;  
byte INDICATOR_LED = LED_BUILTIN;

const char* wifi_ssid = "ipsum";
const char* wifi_pwd = "";
// const IPAddress ip(192, 168, 1, 250); //if you want to use these, uncomment this and the line further down
// const IPAddress gateway(192, 168, 1, 1);
// const IPAddress subnet(255, 255, 255, 0);

// const char* host = "255.255.255.255"; //this is a bit naughty, but it vastly simplifies things. If you want unicast, just chuck an IP here instead.
const char* host = "192.168.2.14"; 
const int send_port = 8000;
const int publish_port = 8000;
float results;


void setup(void) {
  pinMode(INDICATOR_LED, OUTPUT);
  Serial.begin(115200);
  Serial.println("Geophone waking up...");
  Serial.println("attempting to connect to wifi network with ssid " + String(wifi_ssid));
  ads.setGain(GAIN_ONE);

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

#ifdef ESP_PLATFORM
    WiFi.disconnect(true, true);  
    delay(1000);
    WiFi.mode(WIFI_STA);
#endif

  if (strlen(wifi_pwd) == 0) {
    WiFi.begin(wifi_ssid);
  } else {
    WiFi.begin(wifi_ssid, wifi_pwd);
  }

   // WiFi.config(ip, gateway, subnet); //uncomment this if you want to use static network settings

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.print("WiFi connected, IP = ");
    Serial.println(WiFi.localIP());

    OscWiFi.publish(host, publish_port, "/geophone/raw", results)->setFrameRate(60.f); 

    digitalWrite(INDICATOR_LED, HIGH);
  }

void loop(void) {
  OscWiFi.update();  
  results = ads.readADC_Differential_0_1();
  // Serial.println(results);
}
