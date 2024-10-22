#include <Arduino.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <SPIFFS.h>
#include <ConfigManager.h>
#include <UDPMessengerService.h>
#include <WifiManager.h>

#define GPIO_LED 2

const char* ap_ssid = "ESP32-AP";
const char* ap_pass = "12345678";

void setup() {
    Serial.begin(115200);
    delay(100);

    // Format SPIFFS if mounting fails
    // if (!SPIFFS.begin(true)) {
    //     Serial.println("SPIFFS Mount Failed");
    //     return;
    // }

    // ConfigManager& configManager = ConfigManager::getInstance();
    WifiManager wifiManager = WifiManager();

    wifiManager.establishWiFiConnection();

    ESP_LOGI("LED", "Launching LED");
    pinMode(GPIO_LED, OUTPUT);
    digitalWrite(GPIO_LED, HIGH);
}

void loop() {

}