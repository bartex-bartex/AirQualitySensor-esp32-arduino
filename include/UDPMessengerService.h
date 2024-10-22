#ifndef UDPMESSENGERSERVICE_H
#define UDPMESSENGERSERVICE_H

#include <WiFiUdp.h>
#include <WiFi.h>
#include "ConfigManager.h"
#include "ArduinoJson.h"

class UDPMessengerService {
  private:
    static const int MAX_PACKET_SIZE = 512;
    ConfigManager config = ConfigManager::getInstance();
    WiFiUDP udp;

    void processMessage(IPAddress& senderIp, uint16_t senderPort, char *message);
    bool setWiFi(JsonObject &params);
    void sendPacket(IPAddress& ip, uint16_t port, const char* content);
    
  public:
    UDPMessengerService(uint16_t port);
    void listen();
};

#endif /* UDPMESSENGERSERVICE_H */