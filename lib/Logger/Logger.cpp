#include "Logger.h"

void Logger::begin(unsigned long baudRate) {
    Serial.begin(baudRate);
    while (!Serial) { delay(10); }
    Serial.println("=== Logger Started ===");
}

void Logger::info(const String& message) {
    Serial.print("[INFO] ");
    Serial.println(message);
}
