#pragma once
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

namespace LoRaReliable {
    class TimeManager {
    private:
        WiFiUDP ntpUDP;
        NTPClient timeClient;
        time_t lastSync;
        const int syncInterval = 3600000;
        const int slotDuration;
        const int deviceSlot;

    public:
        TimeManager(int slot, int duration = 30000);
        bool begin();
        bool sync();
        bool needsSync() const;
        bool isMySlot() const;
    };
}