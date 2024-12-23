#pragma once
#include <LoRa.h>
#include "Transport.h"

namespace LoRaReliable {
    class LoRaTransport : public Transport {
    private:
        void setupPins(int8_t nss, int8_t reset, int8_t dio0);
        void configureModem();

    public:
        bool begin(const Config& config) override;
        bool send(const uint8_t* data, size_t size) override;
        size_t receive(uint8_t* buffer, size_t maxSize) override;
        int getRSSI() override;
        float getSNR() override;
        void startReceive() override;
    };
}