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
    doc["temperature"] = mhz.getTemperature();
    doc["accuracy"] = mhz.getAccuracy();
    doc["minCO2"] = mhz.getMinCO2();

  }
  else
  {
    Serial.print(F("Error, code: "));
    Serial.println(response);
  }

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
