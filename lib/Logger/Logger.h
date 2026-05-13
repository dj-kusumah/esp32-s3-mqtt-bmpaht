#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

class Logger {
public:
    static void begin(unsigned long baudRate = 115200);
    static void info(const String& message);
};

#endif
