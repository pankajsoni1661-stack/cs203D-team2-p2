#include <iostream>
#include <cassert>
#include <string>
#include "src/structB/LinearProbing.hpp" 

using namespace std;

void testInsertAndUpdate() {
    LinearProbingTable ht(16, 31);
    int val = 0;

    ht.insert("apple", 10);
    ht.insert("banana", 20);
    assert(ht.size() == 2);
    assert(ht.get("apple", val) == true && val == 10);
    assert(ht.get("banana", val) == true && val == 20);

    ht.insert("apple", 99);
    assert(ht.size() == 2); 
    assert(ht.get("apple", val) == true && val == 99);

    cout << "[PASS] Test 1: Insert and Update\n";
}

void testTombstoneSearchChain() {
    LinearProbingTable ht(16, 31);
    int val = 0;

    ht.insert("a", 100); 
    ht.insert("q", 200); 
    ht.insert("A", 300); 
    assert(ht.remove("q") == true);
    assert(ht.size() == 2);
    assert(ht.num_tombstones == 1);
    assert(ht.get("q", val) == false);
    assert(ht.get("A", val) == true && val == 300);

    ht.insert("A", 350);
    assert(ht.size() == 2);
    assert(ht.get("A", val) == true && val == 350);

    cout << "[PASS] Test 2: Tombstone Creation and Search Past Tombstone\n";
}

void testTombstoneRecycling() {
    LinearProbingTable ht(16, 31);
    int val = 0;

    ht.insert("a", 1);
    ht.insert("q", 2);
    ht.insert("A", 3);
    ht.remove("q");
    assert(ht.num_tombstones == 1);

    ht.insert("Q", 4);
    assert(ht.size() == 3);
    assert(ht.num_tombstones == 0); 
    assert(ht.get("Q", val) == true && val == 4);

    cout << "[PASS] Test 3: Tombstone Slot Recycling\n";
}

void testResizeClearsTombstones() {
    LinearProbingTable ht(8, 31); 
    int val = 0;

    ht.insert("k1", 1);
    ht.insert("k2", 2);
    ht.insert("k3", 3);
    ht.insert("k4", 4);

    ht.remove("k1");
    ht.remove("k2");
    ht.remove("k3");
    assert(ht.size() == 1);
    assert(ht.num_tombstones == 3);
    assert(ht.capacity == 8);

    ht.insert("k5", 5);
    ht.insert("k6", 6);

    assert(ht.capacity == 16);      
    assert(ht.num_tombstones == 0);
    assert(ht.size() == 3);        
    assert(ht.get("k4", val) == true && val == 4);
    assert(ht.get("k5", val) == true && val == 5);
    assert(ht.get("k6", val) == true && val == 6);
    assert(ht.checkInvariants() == true);

    cout << "[PASS] Test 4: Resize Doubles Capacity and Clears Tombstones\n";
}

void testSeededHashing() {
    LinearProbingTable ht(16, 31, true);
    int val = 0;

    for (int i = 0; i < 100; i++) {
        ht.insert("word_" + to_string(i), i * 10);
    }
    assert(ht.size() == 100);
    assert(ht.get("word_50", val) == true && val == 500);
    assert(ht.checkInvariants() == true);

    cout << "[PASS] Test 5: Seeded Hashing Mode and Invariants\n";
}

int main() {
    testInsertAndUpdate();
    testTombstoneSearchChain();
    testTombstoneRecycling();
    testResizeClearsTombstones();
    testSeededHashing();

    cout << "\nALL STRUCTURE B UNIT TESTS PASSED SUCCESSFULLY!\n";
    return 0;
}
