#pragma once
#include <cstdint>
#include <cstddef>
#include <memory>
#include "Transport.h"
#include "Codec.h"
#include "TimeManager.h"

namespace LoRaReliable {

   template<typename T>
   struct ReceiveResult {
       T data;
       PacketIntegrity integrity;
       bool isValid;
   };

   class Device {
   private:
       std::unique_ptr<Transport> transport;
       std::unique_ptr<Codec> codec;
       std::unique_ptr<TimeManager> timeManager;
       Config config;
       enum class Mode { TX, RX } currentMode;

       void switchMode(Mode newMode) {
           if (currentMode != newMode) {
               if (newMode == Mode::RX) {
                   transport->startReceive();
               }
               currentMode = newMode;
           }
       }

   public:
       Device(std::unique_ptr<Transport> t, 
              std::unique_ptr<Codec> c,
              const Config& cfg)
           : transport(std::move(t))
           , codec(std::move(c))
           , config(cfg)
           , timeManager(std::make_unique<TimeManager>(cfg.timeSlot, cfg.slotDuration))
           , currentMode(Mode::RX) {}

       bool begin() {
           if (!transport->begin(config)) return false;
           if (!timeManager->begin()) return false;
           return true;
       }

       template<typename T>
       bool send(const T& data) {
           if (!timeManager->isMySlot()) return false;
           switchMode(Mode::TX);
           auto encoded = codec->encode(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
           return transport->send(encoded.data(), encoded.size());
       }

       template<typename T>
       ReceiveResult<T> receive() {
           if (timeManager->isMySlot()) {
               return ReceiveResult<T>{T{}, PacketIntegrity{}, false};
           }
           switchMode(Mode::RX);
           std::vector<uint8_t> buffer(256);
           size_t received = transport->receive(buffer.data(), buffer.size());
           if (received == 0) {
               return ReceiveResult<T>{T{}, PacketIntegrity{}, false};
           }
           auto integrity = codec->decode(buffer.data(), received);
           T data;
           memcpy(&data, buffer.data(), std::min(sizeof(T), received));
           return ReceiveResult<T>{data, integrity, true};
       }

       void loop() {
           if (timeManager->needsSync()) {
               timeManager->sync();
           }
       }

       ~Device() = default;
   };
}