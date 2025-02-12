#define LED_BUILTIN 2

#include <Arduino.h>
#include <Bme280.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <NTPClient.h>

Bme280TwoWire sensor;
WiFiMulti wiFiMulti;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.at.pool.ntp.org", 3600);
uint16_t counter = 0;

void clear_lcd() {
  Serial.write(0xFE);  //command flag
  Serial.write(0x01);  //clear command.
}

void set_cusor_pos(int row, int col) {
  Serial.write(0xFE);
  Serial.write(col + row * 64 + 128);
}

void setup() {
  // lcd / serial begin
  Serial.begin(9600);
  clear_lcd();

  // sensor begin
  Wire.begin(17, 16);  // SDA, SCL
  sensor.begin(Bme280TwoWireAddress::Primary);
  sensor.setSettings(Bme280Settings::indoor());

  // start wifi connection
  WiFi.mode(WIFI_STA);
  wiFiMulti.addAP("SSID", "PSK");

  // wait for WiFi connection
  Serial.print("WiFi...");
  while ((wiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");

  clear_lcd();
  set_cusor_pos(0, 0);
  Serial.print("IP addr:");
  set_cusor_pos(1, 0);
  Serial.print(WiFi.localIP());
  delay(2500);
  clear_lcd();

  // ntp
  timeClient.begin();
  timeClient.forceUpdate();
}

void loop() {
  // TODO: check if still connected to wifi and reconnect

  float temp = sensor.getTemperature();   // celsius
  float pressure = sensor.getPressure();  // pascal
  float humidity = sensor.getHumidity();  // percent
  
  timeClient.update();

  char line[20] = {0};
  snprintf(line, 20, "%02.2f C  %3.0f hPa", temp, pressure * 0.01);
  set_cusor_pos(0, 0);
  Serial.print(line);

  char spinner[] = " -+*";
  snprintf(line, 20, "%02.2f %%  %02d:%02d %c", humidity, timeClient.getHours(), timeClient.getMinutes(), spinner[counter % (sizeof(spinner) - 1)]);
  set_cusor_pos(1, 0);
  Serial.print(line);
  delay(500);

  counter++;
}
