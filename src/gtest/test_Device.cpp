#include <gtest/gtest.h>
#include "LoRaReliable/Device.h"
#include "LoRaReliable/LoRaTransport.h"
#include "LoRaReliable/ReedSolomonCodec.h"
#include "LoRaReliable/Config.h"

using namespace LoRaReliable;

class MockTransport : public Transport {
public:
    bool begin(const Config& config) override { return true; }
    bool send(const uint8_t* data, size_t size) override { return true; }
    size_t receive(uint8_t* buffer, size_t maxSize) override { return 0; }
    int getRSSI() override { return -50; }
    float getSNR() override { return 5.0; }
    void startReceive() override {}
};

TEST(DeviceTest, Initialization) {
    Config config{
        433E6, 10, 31.25E3, 8, 20, 0x12, 0, 30000
    };
    auto transport = std::make_unique<MockTransport>();
    auto codec = std::make_unique<ReedSolomonCodec>();

    Device device(std::move(transport), std::move(codec), config);
    ASSERT_TRUE(device.begin());
}

TEST(DeviceTest, SendAndReceive) {
    Config config{
        433E6, 10, 31.25E3, 8, 20, 0x12, 0, 30000
    };
    auto transport = std::make_unique<MockTransport>();
    auto codec = std::make_unique<ReedSolomonCodec>();

    Device device(std::move(transport), std::move(codec), config);
    ASSERT_TRUE(device.begin());

    int dataToSend = 12345;
    EXPECT_TRUE(device.send(dataToSend));

    auto result = device.receive<int>();
    EXPECT_FALSE(result.isValid);
}
