#include "ConfigManager.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>

ConfigManager::ConfigManager() {
    if (!SPIFFS.begin()) {
        Serial.println("Failed to mount file system");
        return;
    }

    this->ssid = new char[64];
    this->pass = new char[64];

    memset(this->ssid, 0, 64);
    memset(this->pass, 0, 64);
}

ConfigManager::~ConfigManager() {
    delete[] this->ssid;
    delete[] this->pass;
}

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::load() {
    ESP_LOGI("CONFIG", "Loading config");

    File file = SPIFFS.open("/config.json", "r");
    if (!file) {
        ESP_LOGE("CONFIG", "Failed to open config file");
        return false;
    }

    size_t size = file.size();
    char* buffor = new char[size];
    file.readBytes(buffor, size);

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, buffor);
    if (error) {
        ESP_LOGE("CONFIG", "Failed to parse config file");
        delete[] buffor;
        return false;
    }

    const char* ssid = doc["ssid"];
    const char* pass = doc["pass"];

    this->setSSID(ssid);
    this->setPassword(pass);

    if (!isDeviceConfigured())
    {
        ESP_LOGE("CONFIG", "Config file is empty");
        delete[] buffor;
        return false;
    }

    ESP_LOGI("CONFIG", "Config loaded");
    delete[] buffor;
    return true;
}

bool ConfigManager::save() {
    StaticJsonDocument<256> doc;
    doc["ssid"] = this->ssid;
    doc["pass"] = this->pass;

    File file = SPIFFS.open("/config.json", "w");
    if (!file) {
        return false;
    }

    serializeJson(doc, file);
    file.close();
    return true;
}

bool ConfigManager::reset() {
    this->setSSID("");
    this->setPassword("");
    return this->save();
}

bool ConfigManager::isDeviceConfigured() {
    return this->ssid[0] != '\0' && this->pass[0] != '\0';
}

void ConfigManager::getSSID(char *buf) {
    strcpy(buf, this->ssid);
}

void ConfigManager::setSSID(const char *ssid) {
    strcpy(this->ssid, ssid);
}

void ConfigManager::getPassword(char *buf) {
    strcpy(buf, this->pass);
}

void ConfigManager::setPassword(const char *password) {
    strcpy(this->pass, password);
}