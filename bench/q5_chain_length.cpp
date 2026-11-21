// Q5: measure the full chain containing a uniformly selected stored key.
#include "../src/structA/StructureA.h"
#include "../src/string_hash.hpp"
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    constexpr std::size_t buckets = 4096;
    constexpr int repetitions = 16;
    constexpr unsigned inputSeed = 203;
    std::mt19937_64 input(inputSeed);
    std::vector<std::string> keys(buckets * 9 / 10, std::string(12, ' '));
    for (auto& key : keys) {
        for (char& c : key) c = static_cast<char>(33 + input() % 94);
    }
    auto sorted = keys;
    std::sort(sorted.begin(), sorted.end());
    if (std::adjacent_find(sorted.begin(), sorted.end()) != sorted.end()) {
        throw std::runtime_error("input must contain distinct keys");
    }

    std::cout << "input_seed,hash_seed,buckets,keys,alpha,mean_chain,mean_hit_probes,"
                 "universal_bound,longest_chain\n" << std::setprecision(12);
    for (int run = 1; run <= repetitions; ++run) {
        const unsigned seed = 1000 + run;
        StructureA table(true, seed, buckets);
        std::vector<std::size_t> sizes(buckets);
        std::size_t inserted = 0;
        for (int step = 1; step <= 9; ++step) {
            const std::size_t n = buckets * step / 10;
            while (inserted < n) {
                const auto& key = keys[inserted];
                table.insert(key, static_cast<int>(inserted));
                ++sizes[string_hash::seededHash(key, seed) % buckets];
                ++inserted;
            }
            if (table.bucketCount() != buckets || table.count() != static_cast<int>(n)) {
                throw std::runtime_error("table capacity or live count changed unexpectedly");
            }
            std::uint64_t totalChain = 0;
            for (std::uint64_t k : sizes) totalChain += k * k;
            std::uint64_t totalProbes = 0;
            for (std::size_t i = 0; i < n; ++i) {
                std::size_t probes = 0;
                int value = -1;
                if (!table.get(keys[i], value, probes) || value != static_cast<int>(i)) {
                    throw std::runtime_error("stored key or value mismatch");
                }
                totalProbes += probes;
            }
            // In a chain of k keys, the successful searches take 1 + ... + k probes.
            if (totalChain != 2 * totalProbes - n) {
                throw std::runtime_error("occupancy and actual lookup probes disagree");
            }
            double alpha = static_cast<double>(n) / buckets;
            std::cout << inputSeed << ',' << seed << ',' << buckets << ',' << n << ',' << alpha
                      << ',' << static_cast<double>(totalChain) / n
                      << ',' << static_cast<double>(totalProbes) / n
                      << ',' << 1.0 + static_cast<double>(n - 1) / buckets
                      << ',' << *std::max_element(sizes.begin(), sizes.end()) << '\n';
        }
    }
    return std::cout ? 0 : 1;
}
