#define LED_BUILTIN 2

#include <Arduino.h>
#include <Bme280.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <NTPClient.h>

#include <InfluxDbClient.h>

#include "config.h"

#define TZ_INFO "CEST-1CET,M3.2.0/2:00:00,M11.1.0/2:00:00"

const char* ca_root_cert_ISRG_Root_X1 = "-----BEGIN CERTIFICATE-----\n\
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n\
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n\
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n\
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n\
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n\
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n\
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n\
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n\
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n\
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n\
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n\
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n\
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n\
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n\
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n\
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n\
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n\
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n\
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n\
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n\
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n\
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n\
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n\
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n\
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n\
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n\
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n\
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n\
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n\
-----END CERTIFICATE-----";

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, ca_root_cert_ISRG_Root_X1);

// Declare Data point
Point data_point("atmosphere");

Bme280TwoWire sensor;
WiFiMulti wiFiMulti;

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
  wiFiMulti.addAP(WIFI_SSID, WIFI_PSK);

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
  timeSync(TZ_INFO, "0.at.pool.ntp.org", "1.at.pool.ntp.org", "2.at.pool.ntp.org");
  data_point.addTag("location", "table-test");
  clear_lcd();

  // influx client
  WriteOptions wo;
  wo.maxRetryAttempts(10);
  wo.maxRetryInterval(150);
  wo.flushInterval(20);
  wo.bufferSize(30);

  HTTPOptions ho;
  ho.httpReadTimeout(10 * 1000); // specified in milli seconds
  ho.connectionReuse(true);

  client.setWriteOptions(wo);
  client.setHTTPOptions(ho);
}

void loop() {
  // TODO: check if still connected to wifi and reconnect
  while ((wiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }

  float temp = sensor.getTemperature();   // celsius
  float pressure = sensor.getPressure();  // pascal
  float humidity = sensor.getHumidity();  // percent

  data_point.clearFields();
  data_point.addField("temp", temp);
  data_point.addField("pressure", pressure);
  data_point.addField("humidity", humidity);
  if (!client.writePoint(data_point)) {
    Serial.print("InfluxDB write failed: ");
    clear_lcd();
    set_cusor_pos(0, 0);
    Serial.println(client.getLastErrorMessage());
    delay(5000);
  }

  clear_lcd();
  char line[20] = { 0 };
  snprintf(line, 20, "%02.2f C  %3.0f hPa", temp, pressure * 0.01);
  set_cusor_pos(0, 0);
  Serial.print(line);

  struct tm timeinfo;
  getLocalTime(&timeinfo);
  
  char spinner[] = " -+*";
  snprintf(line, 20, "%02.2f %%  %02d:%02d %c", humidity, timeinfo.tm_hour, timeinfo.tm_min, spinner[counter % (sizeof(spinner) - 1)]);
  set_cusor_pos(1, 0);
  Serial.print(line);
  delay(3000);

  counter++;
}
