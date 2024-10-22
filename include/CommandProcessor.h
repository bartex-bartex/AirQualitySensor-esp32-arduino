#ifndef CommandProcessor_h
#define CommandProcessor_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include "ConfigManager.h"

class CommandProcessor {
private:
    static ConfigManager config;
    static bool saveWifiCredentials(JsonObject &params);

public:
    static void processMessage(char *message);
};

#endif /* CommandProcessor_h */