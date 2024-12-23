# LoRaReliable Library

ESP32 library for reliable LoRa communication with time synchronization and error correction.

## Features
- Time-slotted transmission using NTP
- Reed-Solomon error correction
- Packet integrity verification
- Automatic synchronization
- Configurable parameters for range/speed optimization

## Installation
1. Clone repository to Arduino libraries folder
2. Install dependencies: LoRa, NTPClient, CRC32

## Usage
```cpp
#include <LoRaReliable.h>

// Configure parameters
LoRaReliable::Config config {
   .frequency = 433E6,
   .spreadingFactor = 10, 
   .signalBandwidth = 31.25E3,
   .codingRate = 8,
   .txPower = 20,
   .syncWord = 0x12,
   .timeSlot = 0,        // 0 or 1
   .slotDuration = 30000 // ms
};

void setup() {
   auto transport = std::make_unique<LoRaReliable::LoRaTransport>();
   auto codec = std::make_unique<LoRaReliable::ReedSolomonCodec>();
   LoRaReliable::Device device(std::move(transport), std::move(codec), config);
   device.begin();
}