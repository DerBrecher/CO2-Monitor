#ifdef ESP32
hw_timer_t *timer = NULL;
#endif

void handleWatchdog() {
  #ifdef ESP32
  timerWrite(timer, 0);
  #endif
}

#ifdef ESP32
void IRAM_ATTR resetModule() {
  ets_printf("Watchdog timeout! Rebooting\n");
  esp_restart();
}

void setupWatchdog() {
  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt
  
  handleWatchdog(); //reset Watchdog
}
#endif

void maintainWifi() {
  Serial.print("WiFi disconnected. Trying to reconnect to Wifi with SSID: ");
  Serial.println(wifi_ssid);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_wpa2);
}

void setupWifi() {
  handleWatchdog();

  WiFi.mode(WIFI_STA);

  WiFi.begin(wifi_ssid, wifi_wpa2);

  for (int i = 0; i < 10; i++) {
    delay(500);
    Serial.print("trying to connect to Wifi with SSID: ");
    Serial.println(wifi_ssid);
    if (WiFi.status() == WL_CONNECTED) {
      break;
    }
  }

  byte mac[6];
  WiFi.macAddress(mac);
  #ifdef ESP32
  uniqID = String("ESP32-") + String(mac[0], HEX) + String(mac[1], HEX) + String(mac[2], HEX) + String(mac[3], HEX) + String(mac[4], HEX) + String(mac[5], HEX);
  #endif

  #ifdef ESP8266
  uniqID = String("ESP8266-") + String(mac[0], HEX) + String(mac[1], HEX) + String(mac[2], HEX) + String(mac[3], HEX) + String(mac[4], HEX) + String(mac[5], HEX);
  #endif
  
  uniqID.toCharArray(uniqIDChar, UNIQIDMAXLENGTH);

  uniqTopic[0] = 47; // ASCI for "/"
  strcat(uniqTopic, uniqIDChar);
  strcat(uniqTopic, "/");

  Serial.println("Unique ID of this ESP:");
  Serial.println(uniqIDChar);
}

void setupMQTT() {
  handleWatchdog();
  
  client.setServer(mqttServer, 1883);

  int retryCounter = 0;
  while (!client.connected() && retryCounter < 10) {
    Serial.print(retryCounter);
    Serial.println(". try to connect to mqtt");
    connectMqtt();

    retryCounter++;
    delay(5000);
  }

  if (client.connected()) {
    Serial.println("Connected to MQTT");
  }
  else {
    Serial.println("Not connected to MQTT");
  }
}

void connectMqtt() {
  if (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(uniqIDChar)) {
      Serial.println("Connected to MQTT");
      client.publish("/sensors/",  uniqIDChar);
      //subscriveToTopics();
    }
  }
}

void subscriveToTopics() {
  //client.subscribe("inTopic"); //callback needed for handling information
}
