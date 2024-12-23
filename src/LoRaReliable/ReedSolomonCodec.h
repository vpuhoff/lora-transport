#pragma once
#include "Codec.h"
#include <vector>
#include <cstdint>
#include <cstddef>

namespace LoRaReliable {

    /**
     * ReedSolomonCodec class provides error correction encoding and decoding
     * using Reed-Solomon algorithm.
     */
    class ReedSolomonCodec : public Codec {
    private:
        static constexpr size_t RS_BLOCK_SIZE = 255; ///< Total block size
        static constexpr size_t RS_DATA_SIZE = 180; ///< Data portion size
        static constexpr size_t RS_PARITY_SIZE = 75; ///< Parity portion size

        uint8_t gf_exp[512]; ///< Galois Field exponent table
        uint8_t gf_log[256]; ///< Galois Field logarithm table
        uint8_t generator[RS_PARITY_SIZE + 1]; ///< Reed-Solomon generator polynomial

        /** Initialize Galois Field tables */
        void initTables();

        /** Generate Reed-Solomon generator polynomial */
        void generateGenerator();

        /** Galois Field multiplication */
        uint8_t gfMultiply(uint8_t x, uint8_t y);

        /** Find error locator polynomial */
        void findErrorLocator(uint8_t* syndromes, uint8_t* lambda);

        /** Find error evaluator polynomial */
        void findErrorEvaluator(uint8_t* syndromes, uint8_t* lambda, uint8_t* omega);

    public:
        /** Constructor */
        ReedSolomonCodec();

        /**
         * Encode data using Reed-Solomon algorithm.
         *
         * @param input Pointer to input data
         * @param inputSize Size of input data
         * @return Encoded data including parity bytes
         */
        std::vector<uint8_t> encode(const uint8_t* input, size_t inputSize) override;

        /**
         * Decode data and correct errors using Reed-Solomon algorithm.
         *
         * @param data Pointer to received data
         * @param size Size of received data
         * @return Packet integrity information
         */
        PacketIntegrity decode(uint8_t* data, size_t size) override;

    protected:
        /**
         * Calculate checksum for the data.
         *
         * @param data Pointer to data
         * @param size Size of data
         * @return Computed checksum value
         */
        uint32_t calculateChecksum(const uint8_t* data, size_t size) override;
    };
}
