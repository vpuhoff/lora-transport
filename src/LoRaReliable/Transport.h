#pragma once
#include <cstdint>
#include <cstddef>
#include "Config.h"

namespace LoRaReliable {
    class Transport {
    public:
        virtual bool begin(const Config& config) = 0;
        virtual bool send(const uint8_t* data, size_t size) = 0;
        virtual size_t receive(uint8_t* buffer, size_t maxSize) = 0;
        virtual int getRSSI() = 0;
        virtual float getSNR() = 0;
        virtual void startReceive() = 0;
        virtual ~Transport() = default;
    protected:
        Config _config;
    };
}