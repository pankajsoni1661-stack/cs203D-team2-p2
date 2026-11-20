#include <iostream>
#include <string>
#include <random>
#include <cassert>

#include "../src/structA/StructureA.h"
#include "../src/structB/LinearProbing.hpp"

using namespace std;

int main() {
    const int NUM_OPS = 20000;
    const int KEY_POOL = 300;
    mt19937 rng(42);
    uniform_int_distribution<int> opDist(1, 100);
    uniform_int_distribution<int> keyDist(1, KEY_POOL);
    uniform_int_distribution<int> valDist(1, 35000);

    StructureA structA_fixed(false);
    StructureA structA_seeded(true, 987654321ULL);
    LinearProbingTable structB_fixed(16, 31, false);
    LinearProbingTable structB_seeded(16, 31, true, 987654321ULL);

    for (int i = 0; i < NUM_OPS; i++) {
        int choice = opDist(rng);
        string key = "key_" + to_string(keyDist(rng));
        int val = valDist(rng);

        if (choice <= 40) {
            structA_fixed.insert(key, val);
            structA_seeded.insert(key, val);
            structB_fixed.insert(key, val);
            structB_seeded.insert(key, val);
        } 
        else if (choice <= 80) {
            structA_fixed.remove(key);
            structA_seeded.remove(key);
            structB_fixed.remove(key);
            structB_seeded.remove(key);
        } 
        else {
            int out = 0;
            structA_fixed.get(key, out);
            structA_seeded.get(key, out);
            structB_fixed.get(key, out);
            structB_seeded.get(key, out);
        }

        if (i % 500 == 0) {
            assert(structA_fixed.checkInvariants());
            assert(structA_seeded.checkInvariants());
            assert(structB_fixed.checkInvariants());
            assert(structB_seeded.checkInvariants());
        }
    }

    assert(structA_fixed.checkInvariants());
    assert(structA_seeded.checkInvariants());
    assert(structB_fixed.checkInvariants());
    assert(structB_seeded.checkInvariants());

    cout << "[PASS] All invariants held for Structure A & B (fixed & seeded) across "
         << NUM_OPS << " operations!\n";
    return 0;
}
