#include "LoRaReliable/TimeManager.h"

namespace LoRaReliable {
    TimeManager::TimeManager(int slot, int duration) 
        : deviceSlot(slot), slotDuration(duration), timeClient(ntpUDP) {}

    bool TimeManager::begin() {
        timeClient.begin();
        return sync();
    }

    bool TimeManager::sync() {
        return timeClient.update();
    }

    bool TimeManager::needsSync() const {
        return millis() - lastSync > syncInterval;
    }

    bool TimeManager::isMySlot() const {
        int currentSlot = (timeClient.getEpochTime() % (slotDuration * 2)) / slotDuration;
        return currentSlot == deviceSlot;
    }
}