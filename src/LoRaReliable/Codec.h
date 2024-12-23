#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>

namespace LoRaReliable {
    struct PacketIntegrity {
        float errorRate;
        float signalQuality;
        float recoveryRate;
    };

    class Codec {
    public:
        virtual std::vector<uint8_t> encode(const uint8_t* input, size_t inputSize) = 0;
        virtual PacketIntegrity decode(uint8_t* data, size_t size) = 0;
        virtual ~Codec() = default;
    protected:
        virtual uint32_t calculateChecksum(const uint8_t* data, size_t size) = 0;
    };
}