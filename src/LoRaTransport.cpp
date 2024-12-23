#include "LoRaReliable/LoRaTransport.h"

namespace LoRaReliable {

    bool LoRaTransport::begin(const Config& config) {
        _config = config;
        
        if (!LoRa.begin(config.frequency)) {
            return false;
        }

        LoRa.setSpreadingFactor(config.spreadingFactor);
        LoRa.setSignalBandwidth(config.signalBandwidth);
        LoRa.setCodingRate4(config.codingRate);
        LoRa.setTxPower(config.txPower);
        LoRa.setSyncWord(config.syncWord);
        
        return true;
    }

    bool LoRaTransport::send(const uint8_t* data, size_t size) {
        LoRa.beginPacket();
        LoRa.write(data, size);
        return LoRa.endPacket();
    }

    size_t LoRaTransport::receive(uint8_t* buffer, size_t maxSize) {
        int packetSize = LoRa.parsePacket();
        if (!packetSize) return 0;
        
        return LoRa.readBytes(buffer, std::min(maxSize, (size_t)packetSize));
    }

    void LoRaTransport::startReceive() {
        LoRa.receive();
    }

    int LoRaTransport::getRSSI() {
        return LoRa.packetRssi();
    }

    float LoRaTransport::getSNR() {
        return LoRa.packetSnr();
    }
}