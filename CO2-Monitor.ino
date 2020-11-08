#include <MHZ19.h>
#include <NeoPixelBus.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_system.h"
#include <secrets.h>

// ---------- PINS ----------
#define RX_PIN 16
#define TX_PIN 17
#define LED_PIN 18

// ---------- LEDS ----------
#define LED_COUNT 4

NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1800KbpsMethod> strip(LED_COUNT, LED_PIN);
int brightness = 20;

// ---------- NETWORK ----------
const char* wifi_ssid = SSIDSB;
const char* wifi_wpa2 = WPA2SB;
#define UNIQIDMAXLENGTH 30
//const char* mqttServer = "jetson-4-3"; //mqtt server
const char* mqttServer = "192.168.178.43"; //mqtt server
const uint32_t mqttConnectionCheckInterval = 3000;
const uint32_t wifiConnectionCheckInterval = 3000;

String uniqID;
char uniqIDChar[UNIQIDMAXLENGTH];
char uniqTopic[UNIQIDMAXLENGTH + 2];
char sensorTopic[] = "sensorReadings";

WiFiClient espClient;
PubSubClient client(espClient);


// ---------- CO2 Sensor ----------
MHZ19 mhz(&Serial1);
const uint32_t updateIntervalCO2 = 10000;

int currentPPM = 400;

// ---------- JSON ----------
#define JSONSIZE 200

StaticJsonDocument<JSONSIZE> doc;
char jsonString[JSONSIZE];

// ---------- TIMING Variables ----------
uint32_t lastUpdate = 0;
uint32_t lastMqttConnectionCheck = 0;
uint32_t lastWifiConnectionCheck = 0;

// ---------- Watchdog ----------
const int wdtTimeout = 10000;  //time in ms to trigger the watchdog

void setup()
{
  Serial.begin(115200);
  Serial.println("Stared Setup");

  setupWatchdog();

  setupC2OSensor();

  setupLeds();

  handleWatchdog();
  setupWifi();

  handleWatchdog();
  setupMQTT();

  Serial.println("Finished Setup");
}

void loop()
{
  handleWatchdog();

  if ((WiFi.status() != WL_CONNECTED) && (millis() - lastWifiConnectionCheck > wifiConnectionCheckInterval)) {
    maintainWifi();
  }

  if (millis() - lastUpdate > updateIntervalCO2) {
    lastUpdate = millis();
    handleCO2Sensor();
    updateDisplay(currentPPM);
  }

  if (!client.connected() && (millis() - lastMqttConnectionCheck > mqttConnectionCheckInterval)) {
    lastMqttConnectionCheck = millis();
    connectMqtt(); //mqtt client.connect() is blocking for 5 seconds (normally 15 seconds, libary edited)
  }

  if (client.connected()) {
    client.loop();
  }
}
