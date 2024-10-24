#ifndef WifiManager_H
#define WifiManager_H

#include "ConfigManager.h"
#include "UDPMessengerService.h"

class WifiManager {
private:
    ConfigManager& configManager = ConfigManager::getInstance();
    // TODO: Move below 2 to config file
    const char* ap_ssid = "ESP32-AP";
    const char* ap_pass = "12345678";
    void getWiFiCredentials(UDPMessengerService& udpMessengerService);
public:
    WifiManager();
    bool connectToWiFi(char* ssid, char* pass);
    bool connectToWiFi(ConfigManager& configManager, int maxUniqueCredentials);
};

#endif // WifiManager_H