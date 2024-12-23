#pragma once
#include "Codec.h"

namespace LoRaReliable {
    class ReedSolomonCodec : public Codec {
    private:
        static constexpr size_t RS_BLOCK_SIZE = 255;
        static constexpr size_t RS_DATA_SIZE = 180;
        static constexpr size_t RS_PARITY_SIZE = 75;

        uint8_t gf_exp[512];
        uint8_t gf_log[256];
        uint8_t generator[RS_PARITY_SIZE + 1];

        void initTables();
        void generateGenerator();
        uint8_t gfMultiply(uint8_t x, uint8_t y);
        void findErrorLocator(uint8_t* syndromes, uint8_t* lambda);
        void findErrorEvaluator(uint8_t* syndromes, uint8_t* lambda, uint8_t* omega);

    public:
        ReedSolomonCodec();
        std::vector<uint8_t> encode(const uint8_t* input, size_t inputSize) override;
        PacketIntegrity decode(uint8_t* data, size_t size) override;
    protected:
        uint32_t calculateChecksum(const uint8_t* data, size_t size) override;
    };
}