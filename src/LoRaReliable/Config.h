#pragma once
#include <cstdint>
#include <stdexcept>

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

        void validate() const {
            if (frequency < 137E6 || frequency > 960E6) {
                throw std::out_of_range("Frequency must be between 137 MHz and 960 MHz");
            }
            if (spreadingFactor < 6 || spreadingFactor > 12) {
                throw std::out_of_range("Spreading factor must be between 6 and 12");
            }
            if (signalBandwidth < 7800 || signalBandwidth > 500E3) {
                throw std::out_of_range("Signal bandwidth must be between 7.8 kHz and 500 kHz");
            }
            if (codingRate < 5 || codingRate > 8) {
                throw std::out_of_range("Coding rate must be between 5 and 8");
            }
            if (txPower < 2 || txPower > 20) {
                throw std::out_of_range("Tx power must be between 2 and 20 dBm");
            }
        }
    };
}
