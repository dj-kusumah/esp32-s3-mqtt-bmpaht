#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "secrets.h"
#include <SensorEnv.h>
#include <Logger.h>

// ── I2C ──────────────────────────────────────────────
#define I2C_SDA 8
#define I2C_SCL 9

// ── MQTT ─────────────────────────────────────────────
#define MQTT_PORT        1883
#define MQTT_CLIENT_ID   "esp32-latihan8"
#define MQTT_TOPIC_DATA  "esp32/sensor/data"
#define MQTT_TOPIC_TEMP  "esp32/sensor/temperature"
#define MQTT_TOPIC_HUM   "esp32/sensor/humidity"
#define MQTT_TOPIC_PRES  "esp32/sensor/pressure"

// ── Interval baca & publish sensor (ms) ──────────────
#define READ_INTERVAL_MS 5000

// ── Objek ─────────────────────────────────────────────
WiFiClient   wifiClient;
PubSubClient mqtt(wifiClient);
SensorEnv    sensor;

unsigned long lastReadTime    = 0;
unsigned long lastReconnectMs = 0;

// ══════════════════════════════════════════════════════
void connectWiFi() {
    Logger::info("Menghubungkan ke WiFi: " + String(SECRET_WIFI_SSID));
    WiFi.mode(WIFI_STA);
    WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASS);

    uint8_t attempt = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (++attempt >= 40) {          // timeout 20 detik
            Serial.println();
            Logger::info("GAGAL konek WiFi! Reboot...");
            ESP.restart();
        }
    }
    Serial.println();
    Logger::info("WiFi terhubung, IP: " + WiFi.localIP().toString());
}

void connectMQTT() {
    while (!mqtt.connected()) {
        Logger::info("Menghubungkan ke broker MQTT " +
                     String(SECRET_MQTT_SERVER) + ":" + String(MQTT_PORT) + "...");

        if (mqtt.connect(MQTT_CLIENT_ID)) {
            Logger::info("MQTT terhubung! Client ID: " + String(MQTT_CLIENT_ID));
        } else {
            Logger::info("MQTT gagal, rc=" + String(mqtt.state()) +
                         " — coba lagi 5 detik");
            delay(5000);
        }
    }
}

// ══════════════════════════════════════════════════════
void setup() {
    Logger::begin(115200);

    // Init I2C & sensor
    Wire.begin(I2C_SDA, I2C_SCL);
    if (!sensor.begin()) {
        Logger::info("ERROR: Periksa koneksi sensor BMP280+AHT20");
    }

    connectWiFi();

    mqtt.setServer(SECRET_MQTT_SERVER, MQTT_PORT);
    connectMQTT();

    Logger::info("Setup selesai. Publish ke topik: " + String(MQTT_TOPIC_DATA));
}

void loop() {
    // Jaga koneksi MQTT tetap hidup
    if (!mqtt.connected()) {
        unsigned long now = millis();
        if (now - lastReconnectMs >= 5000) {
            lastReconnectMs = now;
            Logger::info("MQTT terputus, mencoba reconnect...");
            if (mqtt.connect(MQTT_CLIENT_ID)) {
                Logger::info("MQTT reconnected");
            }
        }
    }
    mqtt.loop();

    // Jaga koneksi WiFi
    if (WiFi.status() != WL_CONNECTED) {
        Logger::info("WiFi terputus, reconnect...");
        WiFi.reconnect();
        delay(2000);
    }

    // Baca & publish sensor
    if (millis() - lastReadTime >= READ_INTERVAL_MS) {
        lastReadTime = millis();

        if (!sensor.read()) {
            Logger::info("Gagal baca sensor");
            return;
        }

        float temp = sensor.getTemperature();
        float hum  = sensor.getHumidity();
        float pres = sensor.getPressure() / 100.0f;  // Pa → hPa

        // Topik gabungan (JSON)
        char json[64];
        snprintf(json, sizeof(json),
                 "{\"t\":%.1f,\"h\":%.1f,\"p\":%.1f}", temp, hum, pres);
        mqtt.publish(MQTT_TOPIC_DATA, json, true);  // retain=true

        // Topik individual (nilai plain, mudah dibaca dashboard/Node-RED)
        char buf[16];
        snprintf(buf, sizeof(buf), "%.1f", temp);
        mqtt.publish(MQTT_TOPIC_TEMP, buf, true);

        snprintf(buf, sizeof(buf), "%.1f", hum);
        mqtt.publish(MQTT_TOPIC_HUM, buf, true);

        snprintf(buf, sizeof(buf), "%.1f", pres);
        mqtt.publish(MQTT_TOPIC_PRES, buf, true);

        Logger::info("Publish → " + String(json));
    }
}