#include "UDPMessengerService.h"

UDPMessengerService::UDPMessengerService(uint16_t port) {
    // non-blocking call: unless UDP is waiting lets move futher
    this->udp.begin(port);
}

bool UDPMessengerService::readPacket(UdpPacket &packet) {

    int packetSize = this->udp.parsePacket();

    if (packetSize) {
        packet.senderIp = this->udp.remoteIP();
        packet.senderPort = this->udp.remotePort();

        int len = this->udp.read(packet.content, MAX_UDP_PACKET_SIZE);
        if (len > 0) {
            packet.content[len] = 0;
        }

        //this->processMessage(senderIp, senderPort, message);

        return true;
    }

    return false;
}


void UDPMessengerService::sendPacket(UdpPacket &packet) {
    ESP_LOGI("UDP", "Sending UDP packet to IP: %s, Port: %u", packet.senderIp.toString().c_str(), packet.senderPort);
    ESP_LOGD("UDP", "Packet content: %s", packet.content);

    this->udp.beginPacket(packet.senderIp, packet.senderPort);
    this->udp.print(packet.content);
    this->udp.endPacket();

    ESP_LOGI("UDP", "Packet sent successfully.");
}
