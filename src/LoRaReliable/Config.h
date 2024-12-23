#pragma once
#include <cstdint>
#include <cstddef>

namespace LoRaReliable {
    struct Config {
        long frequency;
        uint8_t spreadingFactor;
        long signalBandwidth;
        uint8_t codingRate;
        uint8_t txPower;
        uint8_t syncWord;
        int timeSlot;
        int slotDuration;
    };
}