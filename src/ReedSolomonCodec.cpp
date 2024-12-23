#include "LoRaReliable/ReedSolomonCodec.h"
#include <CRC32.h>

namespace LoRaReliable {

    ReedSolomonCodec::ReedSolomonCodec() {
        initTables();
        generateGenerator();
    }

    void ReedSolomonCodec::initTables() {
        int x = 1;
        for (int i = 0; i < 255; i++) {
            gf_exp[i] = x;
            gf_log[x] = i;
            x = (x << 1) ^ ((x & 0x80) ? 0x11d : 0);
        }
        for (int i = 255; i < 512; i++) {
            gf_exp[i] = gf_exp[i - 255];
        }
    }

    void ReedSolomonCodec::generateGenerator() {
        generator[0] = 1;
        for (int i = 0; i < RS_PARITY_SIZE; i++) {
            generator[i + 1] = 1;
            for (int j = i; j > 0; j--) {
                generator[j] = gfMultiply(generator[j], i + 1) ^ generator[j - 1];
            }
            generator[0] = gfMultiply(generator[0], i + 1);
        }
    }

    uint8_t ReedSolomonCodec::gfMultiply(uint8_t x, uint8_t y) {
        if (x == 0 || y == 0) return 0;
        return gf_exp[(gf_log[x] + gf_log[y]) % 255];
    }

    void ReedSolomonCodec::findErrorLocator(uint8_t* syndromes, uint8_t* lambda) {
        uint8_t b[RS_PARITY_SIZE + 1] = {1};
        memset(lambda, 0, RS_PARITY_SIZE + 1);
        lambda[0] = 1;

        int L = 0;
        for (int r = 0; r < RS_PARITY_SIZE; r++) {
            uint8_t delta = syndromes[r];
            for (int j = 1; j <= L; j++) {
                delta ^= gfMultiply(lambda[j], syndromes[r - j]);
            }

            memmove(b + 1, b, RS_PARITY_SIZE);
            b[0] = 0;

            if (delta != 0) {
                if (2 * L <= r) {
                    uint8_t temp[RS_PARITY_SIZE + 1];
                    memcpy(temp, lambda, RS_PARITY_SIZE + 1);
                    for (int j = 0; j <= L; j++) {
                        b[j] = gfMultiply(temp[j], delta);
                    }
                    L = r + 1 - L;
                }
                for (int j = 0; j <= L; j++) {
                    lambda[j] ^= gfMultiply(delta, b[j]);
                }
            }
        }
    }

    void ReedSolomonCodec::findErrorEvaluator(uint8_t* syndromes, uint8_t* lambda, uint8_t* omega) {
        for (int i = 0; i < RS_PARITY_SIZE; i++) {
            uint8_t sum = 0;
            for (int j = 0; j <= i; j++) {
                sum ^= gfMultiply(lambda[j], syndromes[i - j]);
            }
            omega[i] = sum;
        }
    }

    std::vector<uint8_t> ReedSolomonCodec::encode(const uint8_t* input, size_t inputSize) {
        std::vector<uint8_t> output(inputSize + RS_PARITY_SIZE);
        memcpy(output.data(), input, inputSize);
        memset(output.data() + inputSize, 0, RS_PARITY_SIZE);

        for (size_t i = 0; i < inputSize; i++) {
            uint8_t feedback = output[i];
            if (feedback != 0) {
                for (int j = 0; j < RS_PARITY_SIZE; j++) {
                    output[inputSize + j] ^= gfMultiply(generator[j], feedback);
                }
            }
        }
        return output;
    }

    PacketIntegrity ReedSolomonCodec::decode(uint8_t* data, size_t size) {
        PacketIntegrity integrity = {0, 0, 0};

        // Check for valid size
        if (size < RS_PARITY_SIZE || size > RS_BLOCK_SIZE) {
            integrity.errorRate = 100.0f;
            integrity.signalQuality = 0.0f;
            integrity.recoveryRate = 0.0f;
            return integrity;
        }

        uint8_t syndromes[RS_PARITY_SIZE];
        bool hasErrors = false;

        // Calculate syndromes
        for (int i = 0; i < RS_PARITY_SIZE; i++) {
            syndromes[i] = data[0];
            for (size_t j = 1; j < size; j++) {
                syndromes[i] = data[j] ^ gfMultiply(syndromes[i], gf_exp[i]);
            }
            if (syndromes[i] != 0) hasErrors = true;
        }

        if (!hasErrors) {
            integrity.errorRate = 0.0f;
            integrity.signalQuality = 1.0f;
            integrity.recoveryRate = 100.0f;
            return integrity;
        }

        // Attempt error correction
        uint8_t lambda[RS_PARITY_SIZE + 1] = {1};
        uint8_t omega[RS_PARITY_SIZE] = {0};

        findErrorLocator(syndromes, lambda);
        findErrorEvaluator(syndromes, lambda, omega);

        int errors = 0;
        for (size_t i = 0; i < size; i++) {
            uint8_t sum = 0;
            for (int j = 1; j <= RS_PARITY_SIZE; j++) {
                sum ^= gfMultiply(lambda[j], gf_exp[(j * i) % 255]);
            }
            if (sum == 0) {
                uint8_t denominator = 0;
                for (int j = 1; j <= RS_PARITY_SIZE; j += 2) {
                    denominator ^= gfMultiply(lambda[j], gf_exp[(j * i) % 255]);
                }
                uint8_t numerator = 0;
                for (int j = 0; j < RS_PARITY_SIZE; j++) {
                    numerator ^= gfMultiply(omega[j], gf_exp[(j * i) % 255]);
                }
                if (denominator == 0) continue; // Avoid division by zero
                data[i] ^= gfMultiply(numerator, denominator);
                errors++;
            }
        }

        integrity.errorRate = (float)errors / size * 100;
        integrity.signalQuality = 1.0f - (float)errors / (RS_PARITY_SIZE / 2);
        integrity.recoveryRate = errors > 0 ? 100.0f : 0.0f;

        return integrity;
    }


    uint32_t ReedSolomonCodec::calculateChecksum(const uint8_t* data, size_t size) {
        CRC32 crc;
        crc.update(data, size);
        return crc.finalize();
    }
}