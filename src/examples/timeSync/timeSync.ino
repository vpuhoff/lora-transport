#include <LoRaReliable.h>
#include <WiFi.h>

const char* ssid = "your_ssid";
const char* password = "your_password";

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    LoRaReliable::Config config {
        .frequency = 433E6,
        .spreadingFactor = 10,
        .signalBandwidth = 31.25E3,
        .codingRate = 8,
        .txPower = 20,
        .syncWord = 0x12,
        .timeSlot = 0,
        .slotDuration = 30000
    };

    auto transport = std::make_unique<LoRaReliable::LoRaTransport>();
    auto codec = std::make_unique<LoRaReliable::ReedSolomonCodec>();
    
    LoRaReliable::Device device(std::move(transport), 
                               std::move(codec), 
                               config);
    
    if (!device.begin()) {
        Serial.println("Failed to initialize");
        return;
    }

    while(true) {
        device.loop();
        if (device.send("Test message")) {
            Serial.println("Sent in my slot");
        }
        
        auto result = device.receive<String>();
        if (result.isValid) {
            Serial.println("Received: " + result.data);
        }
        delay(100);
    }
}

void loop() {}