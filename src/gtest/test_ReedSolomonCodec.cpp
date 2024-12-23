#include <gtest/gtest.h>
#include "LoRaReliable/ReedSolomonCodec.h"

using namespace LoRaReliable;

TEST(ReedSolomonCodecTest, EncodeDecodeCycle) {
    ReedSolomonCodec codec;
    const std::vector<uint8_t> input = {1, 2, 3, 4, 5};

    auto encoded = codec.encode(input.data(), input.size());
    ASSERT_EQ(encoded.size(), input.size() + ReedSolomonCodec::RS_PARITY_SIZE);

    auto integrity = codec.decode(encoded.data(), encoded.size());
    ASSERT_EQ(integrity.errorRate, 0.0f);
    ASSERT_EQ(integrity.recoveryRate, 100.0f);

    std::vector<uint8_t> decoded(input.size());
    memcpy(decoded.data(), encoded.data(), input.size());
    EXPECT_EQ(input, decoded);
}

TEST(ReedSolomonCodecTest, DecodeWithErrors) {
    ReedSolomonCodec codec;
    std::vector<uint8_t> input = {1, 2, 3, 4, 5};

    auto encoded = codec.encode(input.data(), input.size());
    encoded[0] ^= 0xFF; // Introduce an error
    encoded[2] ^= 0xFF; // Introduce another error

    auto integrity = codec.decode(encoded.data(), encoded.size());
    EXPECT_GT(integrity.errorRate, 0.0f);
    EXPECT_LE(integrity.recoveryRate, 100.0f);
}