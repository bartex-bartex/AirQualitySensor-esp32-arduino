
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include "WifiManager.h"
#include "UDPMessengerService.h"
#include <esp_log.h>
#include "CommandProcessor.h"

WifiManager::WifiManager() {

}

bool WifiManager::connectToWiFi(char* ssid, char* pass) {
    bool success = false;

    ESP_LOGI("WIFI", "Connecting to WiFi with SSID: %s", ssid);
    Serial.printf("Connecting to: %s\n", ssid);

    //WiFi.mode(WIFI_STA); // this stops AP

    // delete old config
    WiFi.disconnect(true);

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
        WiFi.disconnect(true); // stop trying to connect
    }

    return success;
}

bool WifiManager::connectToWiFi(ConfigManager& configManager, int maxUniqueCredentials) {
    bool success = false;

    char ssid[32] = {0};
    char pass[32] = {0};

    configManager.load();

    while (!configManager.isDeviceConfigured()) {
        getWiFiCredentials();
    } 

    do{

        configManager.getSSID(ssid);
        configManager.getPassword(pass);
        ESP_LOGI("CREDENTIALS", "SSID: %s", ssid);
        ESP_LOGI("CREDENTIALS", "Password: %s", pass);

        success = connectToWiFi(ssid, pass);

        if (!success){
            getWiFiCredentials();
        }
    } while(!success && maxUniqueCredentials--);

    return success;
}


void WifiManager::getWiFiCredentials() {

    ESP_LOGI("WIFI", "Resetting WiFi configuration");
    
    configManager.reset();

    ESP_LOGI("WIFI", "Initializing Access Point (AP)");

    // create own network
    IPAddress local_IP(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(ap_ssid, ap_pass);

   ESP_LOGI("WIFI", "Access Point created with SSID: %s", ap_ssid);

    // obtain and save data
    UDPMessengerService udpMessengerService = UDPMessengerService(1234);

    ESP_LOGI("WIFI", "Waiting for UDP packet containing WiFi credentials");

    UdpPacket packet;

    while (!configManager.isDeviceConfigured()) {

        if (udpMessengerService.readPacket(packet)){
            ESP_LOGI("WIFI", "Received UDP packet with content: %s", packet.content);

            // writes response message in packet.content
            CommandProcessor::processMessage(packet.content);

            udpMessengerService.sendPacket(packet);
        }
    }

    ESP_LOGI("WIFI", "WiFi credentials received and saved.");

    WiFi.softAPdisconnect(true);

    ESP_LOGI("WIFI", "Access Point disabled.");
}