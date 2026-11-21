#pragma once
#include <random>
#include <string>

namespace string_hash {
inline unsigned long long polynomialHash(const std::string& key, unsigned long long base = 31) {
    unsigned long long hash = 0;
    for (unsigned char ch : key) {
        hash = hash * base + ch;
    }
    return hash;
}

inline unsigned long long seededHash(const std::string& key, unsigned long long seed) {
    const unsigned long long MOD = 1000000007ULL;

    std::mt19937_64 randomGenerator(seed);
    std::uniform_int_distribution<unsigned long long> randomNumber(0, MOD - 1);

    unsigned long long hash = 0;
    for (unsigned char ch : key) {
        unsigned long long a = randomNumber(randomGenerator);
        hash = (hash + a * (ch + 1)) % MOD;
    }
    return hash;
}

} // namespace string_hash
