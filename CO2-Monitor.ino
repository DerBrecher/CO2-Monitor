
  #include <MHZ19.h>
#include <NeoPixelBus.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <secrets.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#ifdef ESP32
#include <esp_system.h>
#include <WiFi.h> 
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#endif

// ---------- PINS ----------
#ifdef ESP32
#define CALIBRATE_MODE_PIN 19
#define ENABLE_DISPLAY_PIN 23
#define RX_PIN 16
#define TX_PIN 17
#define LED_PIN 18
// SDA = 21; SCL = 22
#endif

#ifdef ESP8266
#define CALIBRATE_MODE_PIN 13 //D7
#define ENABLE_DISPLAY_PIN 16 //D0
#define RX_PIN 14 // D5
#define TX_PIN 12 // D6
//LED PIN has to be RX(3) because of DMA
//SDA = D2(4) SCL = D1(5) because of Hardware I2C
#endif

// ---------- LEDS ----------
#define LED_COUNT 4

#ifdef ESP32
NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1800KbpsMethod> strip(LED_COUNT, LED_PIN);
#endif
#ifdef ESP8266
NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> strip(LED_COUNT);
#endif

int brightness = 20;

// ---------- NETWORK ----------
const char* wifi_ssid = SSIDHDS;
const char* wifi_wpa2 = WPA2HDS;
#define UNIQIDMAXLENGTH 30
const char* mqttServer = "hope"; //mqtt server
//const char* mqttServer = "jetson-4-3"; //mqtt server
//const char* mqttServer = "192.168.178.43"; //mqtt server
const uint32_t mqttConnectionCheckInterval = 3000;
const uint32_t wifiConnectionCheckInterval = 3000;

String uniqID;
char uniqIDChar[UNIQIDMAXLENGTH];
char uniqTopic[UNIQIDMAXLENGTH + 2];
char sensorTopic[] = "sensorReadings";

WiFiClient espClient;
PubSubClient client(espClient);


// ---------- CO2 Sensor ----------
#define MHZ19CALIBRATIONDELAY 600000 //600000 for 10 minutes

#ifdef ESP32
MHZ19 mhz(&Serial1);
#endif

#ifdef ESP8266
SoftwareSerial ss(RX_PIN, TX_PIN);
MHZ19 mhz(&ss);
#endif

const uint32_t updateIntervalCO2 = 10000;

boolean mhz19calibrated = false;

// ---------- BME280 Sensor ----------
Adafruit_BME280 bme; // I2C

// ---------- JSON ----------
#define JSONSIZE 200

StaticJsonDocument<JSONSIZE> doc;
char jsonString[JSONSIZE];

// ---------- TIMING Variables ----------
uint32_t lastUpdate = 0;
uint32_t lastMqttConnectionCheck = 0;
uint32_t lastWifiConnectionCheck = 0;

// ---------- Watchdog ----------
const int wdtTimeout = 20000;  //time in ms to trigger the watchdog

void setup()
{
  delay(100);
  Serial.begin(115200);
  delay(1);
  Serial.println("Stared Setup");

#ifdef ESP32
  setupWatchdog();
#endif

  setupMisc();

  setupLeds();

  setupBME280Sensor();

  setupC2OSensor();

  setupWifi();

  setupMQTT();

  Serial.println("Finished Setup");
}

uint32_t lastMillis = 0;

void loop()
{
  handleWatchdog();

  if ((WiFi.status() != WL_CONNECTED) && (millis() - lastWifiConnectionCheck > wifiConnectionCheckInterval)) {
    lastWifiConnectionCheck = millis();
    maintainWifi();
  }

  if (millis() - lastUpdate > updateIntervalCO2) {
    lastUpdate = millis();
    handleBME280Sensor();
    handleCO2Sensor();
    sendSensorData();
    updateDisplay();
  }

  if (!client.connected() && (millis() - lastMqttConnectionCheck > mqttConnectionCheckInterval)) {
    lastMqttConnectionCheck = millis();
    connectMqtt(); //mqtt client.connect() is blocking for 5 seconds (normally 15 seconds, libary edited)
  }

  if (client.connected()) {
    client.loop();
  }
}
