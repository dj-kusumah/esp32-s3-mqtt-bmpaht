#ifndef SENSORENV_H
#define SENSORENV_H

#include <Arduino.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

/**
 * Sensor AHT20 (suhu + kelembaban) + BMP280 (suhu + tekanan)
 *
 * Panggil Wire.begin(SDA, SCL) sebelum begin()
 * Alamat: AHT20=0x38, BMP280=0x76/0x77
 */
class SensorEnv {
public:
    SensorEnv();
    bool begin();
    bool read();
    float getTemperature();  // AHT20, °C
    float getHumidity();     // AHT20, %
    float getPressure();     // BMP280, Pa

private:
    Adafruit_AHTX0  _aht;
    Adafruit_BMP280 _bmp;
    float _temperature, _humidity, _pressure;
    bool  _ahtOk, _bmpOk;
};

#endif
