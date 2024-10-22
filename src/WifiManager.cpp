
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include "WifiManager.h"
#include "UDPMessengerService.h"
#include <esp_log.h>

WifiManager::WifiManager() {

}

// Loop:
// load config 
// if config doesn't exist, getWiFiCredentials

// Loop:
// try to connect to WiFi
// if failed, getWiFiCredentials
bool WifiManager::establishWiFiConnection() {
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
    } while(!success);

    return success;
}

bool WifiManager::connectToWiFi(char* ssid, char* pass) {
    bool success = false;

    ESP_LOGI("WIFI", "Connecting to WiFi with SSID: %s", ssid);
    Serial.printf("Connecting to: %s\n", ssid);

    WiFi.mode(WIFI_STA); // this stops AP
    // [ 26919][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 11 - AP_STOP
    // [ 26926][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 2 - STA_START
    if (WiFi.status() == WL_CONNECTED){
        ESP_LOGI("WIFI", "Disconnecting from previous WiFi");
        WiFi.disconnect(true);
    }
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
        WiFi.disconnect(); // stop trying to connect
    }

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

    while (!configManager.isDeviceConfigured()) {
        udpMessengerService.listen();
    }

    ESP_LOGI("WIFI", "WiFi credentials received and saved.");

    // disconnect as AP, true = disable AP TODO: should changed to true
    // [ 26871][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 11 - AP_STOP
    // [ 26885][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 10 - AP_START
    WiFi.softAPdisconnect(true);

    ESP_LOGI("WIFI", "Access Point disabled.");
}