# Latihan8

Project PlatformIO untuk ESP32-S3 DevKitC-1 yang membaca data dari sensor BMP280 + AHT20, lalu mengirimkannya ke broker MQTT. Broker Mosquitto di PC juga bisa diaktifkan dengan listener WebSocket supaya data yang sama bisa dipakai client browser.

## Fitur

1. Membaca suhu dari AHT20.
2. Membaca kelembapan dari AHT20.
3. Membaca tekanan udara dari BMP280.
4. Connect ke WiFi mode station.
5. Connect ke broker MQTT di PC.
6. Publish JSON gabungan dan topic individual.
7. Mendukung arsitektur broker yang juga membuka listener WebSocket.

## Hardware yang dipakai

1. ESP32-S3 DevKitC-1
2. Modul BMP280 + AHT20
3. Kabel jumper
4. PC atau laptop Windows untuk broker Mosquitto

## Pin dan wiring

Pin I2C yang dipakai:

1. `GPIO 8` untuk `SDA`
2. `GPIO 9` untuk `SCL`

Koneksi sensor:

1. `VCC` -> `3.3V`
2. `GND` -> `GND`
3. `SDA` -> `GPIO 8`
4. `SCL` -> `GPIO 9`

## Dependency utama

Dependency firmware di `platformio.ini`:

1. `knolleary/PubSubClient`
2. `Adafruit AHTX0`
3. `Adafruit BMP280 Library`
4. `Adafruit Unified Sensor`

## Struktur inti project

```text
Latihan8/
|- platformio.ini
|- include/
|  |- secrets.example.h
|  |- secrets.h
|- lib/
|  |- Logger/
|  |- SensorEnv/
|- src/
|  |- main.cpp
```

## File kredensial

Salin `include/secrets.example.h` menjadi `include/secrets.h`, lalu isi dengan data yang sesuai:

```cpp
#pragma once

#define SECRET_WIFI_SSID "Nama_WiFi_Kamu"
#define SECRET_WIFI_PASS "PasswordWiFi"
#define SECRET_MQTT_SERVER "192.168.1.10"
```

Catatan:

1. `SECRET_MQTT_SERVER` harus berisi IP PC yang menjalankan Mosquitto.
2. `include/secrets.h` sudah masuk `.gitignore`.

## Topic MQTT yang dipakai

Project ini publish ke topic berikut:

1. `esp32/sensor/data`
2. `esp32/sensor/temperature`
3. `esp32/sensor/humidity`
4. `esp32/sensor/pressure`

Payload gabungan `esp32/sensor/data` berbentuk JSON seperti:

```json
{"t":29.1,"h":72.4,"p":1008.6}
```

## Contoh konfigurasi Mosquitto dengan WebSocket

Contoh `mosquitto.conf` untuk testing lokal:

```conf
listener 1883
protocol mqtt

listener 9001
protocol websockets

allow_anonymous true
```

Jalankan broker:

```powershell
mosquitto -c .\mosquitto.conf -v
```

Subscribe dari PC:

```powershell
mosquitto_sub -h localhost -t esp32/sensor/# -v
```

## Cara build dan upload

Jalankan dari folder project ini:

```powershell
pio run
pio run -t upload
pio device monitor
```

## Perilaku program saat ini

Saat board dinyalakan:

1. I2C diinisialisasi di `GPIO 8` dan `GPIO 9`
2. Sensor BMP280 + AHT20 diinisialisasi
3. ESP32 connect ke WiFi
4. ESP32 connect ke broker MQTT di port `1883`
5. Data sensor dibaca setiap 5 detik
6. Data dipublish ke satu topic JSON dan tiga topic individual

## Troubleshooting singkat

1. Kalau ESP32 gagal connect ke WiFi, cek SSID dan password di `secrets.h`.
2. Kalau MQTT gagal connect, cek IP broker di `SECRET_MQTT_SERVER` dan pastikan Mosquitto sedang jalan.
3. Kalau subscriber tidak menerima data, cek topik yang dipakai dan pastikan broker listen di port `1883`.
4. Kalau client web tidak bisa connect lewat WebSocket, cek listener `9001` dan firewall Windows.
5. Kalau sensor tidak terbaca, cek wiring I2C dan log serial.