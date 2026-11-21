#include "../src/structA/StructureA.h"
#include <cassert>
#include <iostream>
#include <string>

int main() {
    // In the same bucket, the three keys form a known head/middle/tail chain.
    for (const char* victim : {"head", "middle", "tail"}) {
        StructureA t(false, 42, 4);
        // Single-letter keys with the same index modulo four.
        t.insert("a", -1);
        t.insert("e", 2);
        t.insert("i", 3);
        std::string removed = std::string(victim) == "head" ? "i" :
                              std::string(victim) == "middle" ? "e" : "a";
        assert(t.remove(removed) && !t.remove(removed));
        int value;
        for (auto key : {"a", "e", "i"})
            assert(t.get(key, value) == (key != removed));
        assert(t.count() == 2 && t.checkInvariants());
    }
    for (bool seeded : {false, true}) {
        StructureA t(seeded, 42, 2);
        int value;
        assert(!t.get("missing", value) && !t.remove("missing"));
        t.insert("first", 1);
        t.insert("first", -1);
        assert(t.count() == 1 && t.get("first", value) && value == -1);
        for (int i = 0; i < 128; ++i) t.insert(std::to_string(i), i);
        assert(t.bucketCount() >= 129 && t.count() == 129);
        for (int i = 0; i < 128; ++i)
            assert(t.get(std::to_string(i), value) && value == i);
        assert(t.get("first", value) && value == -1 && t.checkInvariants());
    }
    std::cout << "Structure A tests passed\n";
}
