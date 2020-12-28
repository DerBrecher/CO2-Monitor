void setupMisc() {
  pinMode(CALIBRATE_MODE_PIN, INPUT_PULLUP);
  pinMode(ENABLE_DISPLAY_PIN, INPUT_PULLUP);
}

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

  bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                  Adafruit_BME280::SAMPLING_X4,
                  Adafruit_BME280::SAMPLING_X4,
                  Adafruit_BME280::SAMPLING_X4,
                  Adafruit_BME280::FILTER_OFF,
                  Adafruit_BME280::STANDBY_MS_1000);
                    
  bme.setTemperatureCompensation(0); //needed because of self heating
}

void handleBME280Sensor() {
  doc["tempBME"] = bme.readTemperature();
  doc["pressureBME"] = bme.readPressure();
  doc["humidityBME"] = bme.readHumidity();
}

void setupC2OSensor() {
  handleWatchdog();

#ifdef ESP32
  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
#endif
#ifdef ESP8266
  ss.begin(9600);
#endif

  // ---------- ABS for CO2 ----------
  if (true) {
    Serial.println("Autobase line calibration deactivated");
    mhz.setAutoCalibration(false);
    delay(100);
  } else {
    Serial.println("Autobase line calibration activated");
    mhz.setAutoCalibration(true);
    delay(100);
  }

  if (digitalRead(CALIBRATE_MODE_PIN)) { // take old calibraion
    Serial.println("MH-Z19 will take old CO2 calibration");
    mhz19calibrated = true;
    displayCalibrated();
  } else { // start new calibration (takes 10 minutes)
    Serial.println("Starting new zero point calibration for MH-Z19");
    displayInCalibration();
    // calibration takes place after MHZ19CALIBRATIONDELAY ms
    // time is monitored in main loop
  }
}

void handleCO2Sensor() {
  if (mhz19calibrated) { //MH-Z19 is calibrated
    MHZ19_RESULT response = mhz.retrieveData();
    if (response == MHZ19_RESULT_OK)
    {
      doc["co2ppm"] = mhz.getCO2();
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
  else { // we want a new calibration
    if (millis() > MHZ19CALIBRATIONDELAY) { //wait until sensor has warmed up and settled.
      Serial.println("Taking new CO2 calibration right now!");
      mhz.calibrateZero();
      mhz19calibrated = true;
    }
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
