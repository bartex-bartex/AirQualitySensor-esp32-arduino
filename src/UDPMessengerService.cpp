#include "UDPMessengerService.h"

UDPMessengerService::UDPMessengerService(uint16_t port) {
    // non-blocking call: unless UDP is waiting lets move futher
    this->udp.begin(port);
}

void UDPMessengerService::listen() {
    char message[MAX_PACKET_SIZE];
    int packetSize = this->udp.parsePacket();

    if (packetSize) {
        IPAddress senderIp = this->udp.remoteIP();
        uint16_t senderPort = this->udp.remotePort();

        int len = this->udp.read(message, MAX_PACKET_SIZE);
        if (len > 0) {
            message[len] = 0;
        }

        this->processMessage(senderIp, senderPort, message);
    }

}

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

void UDPMessengerService::processMessage(IPAddress& senderIp, uint16_t senderPort, char *message) {
    ESP_LOGI("UDP", "Processing incoming UDP message from IP: %s, Port: %u", senderIp.toString().c_str(), senderPort);

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
        bool success = this->setWiFi(params);

        StaticJsonDocument<256> response;
        response["type"] = "setWiFi";

        if (success) {
            ESP_LOGI("UDP", "WiFi credentials saved successfully.");
            response["result"] = "WiFi credentials saved successfully";
        } else {
            ESP_LOGE("UDP", "Failed to save WiFi credentials.");
            response["result"] = "Failed to save WiFi credentials";
        }

        char responseBuffer[256];
        serializeJson(response, responseBuffer, sizeof(responseBuffer));
        this->sendPacket(senderIp, senderPort, responseBuffer);
    } else {
        ESP_LOGW("UDP", "Unknown command received: %s", cmd);
    }
}

bool UDPMessengerService::setWiFi(JsonObject &params) {
    const char* ssid = params["ssid"];
    const char* password = params["password"];

    ESP_LOGI("WIFI", "Setting WiFi credentials: SSID: %s | pass: %s", ssid, password);

    this->config.setSSID(ssid);
    this->config.setPassword(password);

    return this->config.save();
}

void UDPMessengerService::sendPacket(IPAddress& ip, uint16_t port, const char* content) {
    ESP_LOGI("UDP", "Sending UDP packet to IP: %s, Port: %u", ip.toString().c_str(), port);
    ESP_LOGD("UDP", "Packet content: %s", content);

    this->udp.beginPacket(ip, port);
    this->udp.print(content);
    this->udp.endPacket();

    ESP_LOGI("UDP", "Packet sent successfully.");
}
