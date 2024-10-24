
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include "WifiManager.h"
#include <esp_log.h>
#include "CommandProcessor.h"

WifiManager::WifiManager() {

}

bool WifiManager::connectToWiFi(char* ssid, char* pass) {
    bool success = false;

    ESP_LOGI("WIFI", "Connecting to WiFi with SSID: %s", ssid);
    Serial.printf("Connecting to: %s\n", ssid);

    WiFi.disconnect();   // clears config, so it won't try to reconnect
    WiFi.mode(WIFI_STA); // BTW. this stops AP - THAT IS TESTED!

    WiFi.begin(ssid, pass);

    
    // try to connect to WiFi using saved credentials
    int time = 5;
    while (time--){
        if (WiFi.status() == WL_CONNECTED){
            success = true;
            ESP_LOGI("WIFI", "Successfully connected to SSID: %s", ssid);
            break;
        }

        delay(1000);
        ESP_LOGD("WIFI", "Attempt %d, still trying to connect...", 5 - time);
    }

    if (success){
        ESP_LOGI("WIFI", "Connected to WiFi SSID: %s", ssid);
    } else {
        ESP_LOGE("WIFI", "Failed to connect to WiFi SSID: %s", ssid);
        WiFi.disconnect(); // clears config, so it won't try to reconnect
    }

    return success;
}

bool WifiManager::connectToWiFi(ConfigManager& configManager, int maxUniqueCredentials) {
    bool success = false;

    char ssid[32] = {0};
    char pass[32] = {0};

    configManager.load();

    // Mode must be set to WIFI_STA before opening port (default is WIFI_MODE_NULL I guess)
    WiFi.disconnect();   // clears config, so it won't try to reconnect
    WiFi.mode(WIFI_STA); // BTW. this stops AP - THAT IS TESTED!

    UDPMessengerService udpMessengerService = UDPMessengerService(1234);

    while (!configManager.isDeviceConfigured()) {
        getWiFiCredentials(udpMessengerService);
    } 

    do{

        configManager.getSSID(ssid);
        configManager.getPassword(pass);
        ESP_LOGI("CREDENTIALS", "SSID: %s", ssid);
        ESP_LOGI("CREDENTIALS", "Password: %s", pass);

        success = connectToWiFi(ssid, pass);

        if (!success){
            getWiFiCredentials(udpMessengerService);
        }
    } while(!success && maxUniqueCredentials--);

    return success;
}


// When I send two fast UDPs until AP is turn off, it will be put in a queue
void WifiManager::getWiFiCredentials(UDPMessengerService& udpMessengerService) {

    ESP_LOGI("WIFI", "Resetting WiFi configuration");
    
    configManager.reset();

    ESP_LOGI("WIFI", "Initializing Access Point (AP)");

    // create own network
    IPAddress local_IP(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(ap_ssid, ap_pass);

    ESP_LOGI("WIFI", "Access Point created with SSID: %s", ap_ssid);

    ESP_LOGI("WIFI", "Waiting for UDP packet containing WiFi credentials");

    UdpPacket packet;

    while (!configManager.isDeviceConfigured()) {

        // TODO: Sometimes without sending UDP packet the code moves on...
        if (udpMessengerService.readPacket(packet)){
            ESP_LOGI("WIFI", "Received UDP packet with content: %s", packet.content);

            // writes response message in packet.content
            CommandProcessor::processMessage(packet.content);

            udpMessengerService.sendPacket(packet);
        }
    }

    ESP_LOGI("WIFI", "WiFi credentials received and saved.");

    // false -> AP switches state 2 times: STOP START (probably clear config or so)
    // true -> AP switches state 3 times: STOP START STOP -> error occures after first STOP | in fact it really stop the AP, that is OK 
    WiFi.softAPdisconnect(true); 

    ESP_LOGI("WIFI", "Access Point disabled.");

    //delay(20000);
}