# Air sensor at home

This is a simple IoT project that utilizes an ESP32 and a BME280 sensor to push data to an InfluxDB.
The BME280 can read the air temperature, humidity and pressure.

*Work in progress*

## Usage

Download the libraries inside the Arduino IDE and create a `config.h` with
the correct information in the defines. Then just upload and compile.

## Dependencies

- [NTPClient](https://github.com/arduino-libraries/NTPClient)
- [Bme280](https://github.com/malokhvii-eduard/arduino-bme280)
- [ESP8266 Influxdb](https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino)
- [EspSoftwareSerial](https://github.com/plerup/espsoftwareserial)

These can be installed with the Arduino Library Manager.

## Problems

I don't know why, but after sometime the server says there is a problem with the connection to
the server and the writes to the InfluxDB start failing.
This is the message in the nginx message log:

```
SSL_read() failed (SSL: error:0A000126:SSL routines::unexpected eof while reading) while keepalive
```

The first error message on the ESP is a timeout and subsequent writes also fail without producing
an error message on the server side.
I tried multiple things but the best fix I came up with was to reboot the ESP as soon as this
occurs. :(

## TODO + Ideas

- Builtin LED should blink while writing to InfluxDB
- Also log Wifi stats
- Instead of rebooting, maybe reconstruct the InfluxDB client object
- Log IP, Version, MAC-Address and other information on boot only

