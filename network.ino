void maintainWifi() {
  Serial.print("trying to reconnect to Wifi with SSID: ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_wpa2);
}

void setupWifi() {
  WiFi.mode(WIFI_STA);

  WiFi.begin(wifi_ssid, wifi_wpa2);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("trying to connect to Wifi with SSID: ");
    Serial.println(wifi_ssid);
  }
  
  byte mac[6];
  WiFi.macAddress(mac);
  uniqID = String("ESP32-") + String(mac[0], HEX) + String(mac[1], HEX) + String(mac[2], HEX) + String(mac[3], HEX) + String(mac[4], HEX) + String(mac[5], HEX);

  uniqID.toCharArray(uniqIDChar, UNIQIDMAXLENGTH);

  uniqTopic[0] = 47; // ASCI for "/"
  strcat(uniqTopic, uniqIDChar);
  strcat(uniqTopic, "/");

  Serial.println("Unique ID of this ESP:");
  Serial.println(uniqIDChar);
}

void setupMQTT() {
  Serial.println("Connected to WiFi");
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
