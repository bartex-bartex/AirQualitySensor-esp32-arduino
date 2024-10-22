#ifndef WifiManager_H
#define WifiManager_H

#include "ConfigManager.h"

class WifiManager {
private:
    ConfigManager& configManager = ConfigManager::getInstance();
    // TODO: Move below 2 to config file
    const char* ap_ssid = "ESP32-AP";
    const char* ap_pass = "12345678";
    void getWiFiCredentials();
    bool connectToWiFi(char* ssid, char* pass);
public:
    WifiManager();
    bool establishWiFiConnection();
};

#endif // WifiManager_H