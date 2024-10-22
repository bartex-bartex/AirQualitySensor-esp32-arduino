#include "CommandProcessor.h"
#include <esp_log.h>

// {
//   "cmd":"setWiFi",
//   "params":{
//     "ssid":"Moja siec",
//     "password":"12345678"
//   }
// }

// {
//   "type":"setWiFiFeedback",
//   "result":"WiFi credentials saved successfully"
// }

ConfigManager CommandProcessor::config = ConfigManager::getInstance();

void CommandProcessor::processMessage(char *message) {
    ESP_LOGI("UDP", "Processing incoming UDP content");

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
        ESP_LOGE("UDP", "Failed to deserialize JSON message: %s", message);
        return;
    }

    const char* cmd = doc["cmd"];
    JsonObject params = doc["params"];

    ESP_LOGI("UDP", "Received command: %s", cmd);

    if (strcmp(cmd, "setWiFi") == 0) {
        bool success = saveWifiCredentials(params);

        StaticJsonDocument<256> response;
        response["type"] = "setWiFi";

        if (success) {
            ESP_LOGI("UDP", "WiFi credentials saved successfully.");
            response["result"] = "WiFi credentials saved successfully";
        } else {
            ESP_LOGE("UDP", "Failed to save WiFi credentials.");
            response["result"] = "Failed to save WiFi credentials";
        }

        serializeJson(response, message, sizeof(message));
    } else {
        ESP_LOGW("UDP", "Unknown command received: %s", cmd);
    }
}

bool CommandProcessor::saveWifiCredentials(JsonObject &params) {
    const char* ssid = params["ssid"];
    const char* password = params["password"];

    ESP_LOGI("WIFI", "Setting WiFi credentials: SSID: %s | pass: %s", ssid, password);

    config.setSSID(ssid);
    config.setPassword(password);

    return config.save();
}