void setupBME280Sensor() {
  Wire.begin();

  unsigned statusBme;

  statusBme = bme.begin(0x76, &Wire);

  delay(10);

  if (!statusBme) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
  }
}

void handleBME280Sensor() {
  doc["tempBME"] = bme.readTemperature();
  doc["pressureBME"] = bme.readPressure();
  doc["humidityBME"] = bme.readHumidity();
}

void setupC2OSensor() {
  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  //Serial.println("Calibrate Zero");
  //mhz.calibrateZero();
  //delay(10000);
  //Serial.println("Done calibrating");
}

void handleCO2Sensor() {
  MHZ19_RESULT response = mhz.retrieveData();
  if (response == MHZ19_RESULT_OK)
  {
    currentPPM = mhz.getCO2();  //currentPPM is globally used
    doc["co2ppm"] = currentPPM;
    doc["tempCO2"] = mhz.getTemperature();
    doc["accuracy"] = mhz.getAccuracy();
    doc["minCO2"] = mhz.getMinCO2();

  }
  else
  {
    Serial.print(F("Error, code: "));
    Serial.println(response);
  }
}

void sendSensorData() {
  serializeJson(doc, jsonString, JSONSIZE);
  Serial.println(jsonString);

  if (client.connected()) {
    char buf[UNIQIDMAXLENGTH + 30];
    strcpy(buf, uniqTopic);
    strcat(buf, sensorTopic);
    client.publish(buf,  jsonString);
    Serial.println("Send Json");
  }
}
