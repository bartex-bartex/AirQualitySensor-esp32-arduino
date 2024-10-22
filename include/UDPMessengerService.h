#ifndef UDPMESSENGERSERVICE_H
#define UDPMESSENGERSERVICE_H

#include <WiFiUdp.h>
#include <WiFi.h>
#include "ConfigManager.h"
#include "ArduinoJson.h"

const int MAX_UDP_PACKET_SIZE = 512;

struct UdpPacket {
  IPAddress senderIp;
  uint16_t senderPort;
  char content[MAX_UDP_PACKET_SIZE];

  UdpPacket() {
    memset(content, 0, sizeof(content));  // Zero-initialize the content array
  }
};

class UDPMessengerService {
  private:
    ConfigManager config = ConfigManager::getInstance();
    WiFiUDP udp;
    uint16_t senderPort;
    IPAddress senderIp;
    
  public:
    UDPMessengerService(uint16_t port);

    bool readPacket(UdpPacket &packet);
    void sendPacket(UdpPacket &packet);
    
};


#endif /* UDPMESSENGERSERVICE_H */