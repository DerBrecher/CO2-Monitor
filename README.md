# CO2-Monitor
CO2 Monitor based on an ESP32 and a MH Z19 Sensor. Also monitors temperature, humidity and air pressure with an BME280 Sensor. \
Publishes the data as a JSON String onto a MQTT broker. \
In my application used with [Node-Red](https://nodered.org/) to insert the data into an [InfluxDB](https://www.influxdata.com/).
Dashboarding done with [Grafana](https://grafana.com/).
