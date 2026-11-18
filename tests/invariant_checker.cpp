#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <cassert>

using namespace std;

// =========================================================================
// 1. INVARIANT CHECKER FOR STRUCTURE A (SEPARATE CHAINING)
// =========================================================================
struct ChainNode {
    string key;
    int value;
};

// Checks all internal rules of a Separate Chaining hash table
bool checkSeparateChainingInvariants(
    const vector<vector<ChainNode>>& buckets, // Array of bucket chains
    size_t reportedSize,                      // Table's stored N counter
    double maxLoadFactor,                     // Resizing threshold (e.g., 0.75 or 1.0)
    size_t (*hashFunc)(const string&, size_t) // Hash function returning 0..M-1
) {
    size_t M = buckets.size();
    if (M == 0) {
        cout << "[FAIL] Table capacity M cannot be 0.\n";
        return false;
    }

    // Rule 1: Load factor must not exceed threshold
    double currentAlpha = (double)reportedSize / (double)M;
    if (currentAlpha > maxLoadFactor) {
        cout << "[FAIL] Load factor " << currentAlpha << " exceeds max " << maxLoadFactor << "\n";
        return false;
    }

    size_t actualCount = 0;
    unordered_set<string> seenKeys;

    for (size_t i = 0; i < M; i++) {
        for (const auto& item : buckets[i]) {
            actualCount++;

            // Rule 2: No duplicate keys allowed anywhere in the table
            if (seenKeys.count(item.key)) {
                cout << "[FAIL] Duplicate key found in Separate Chaining: " << item.key << "\n";
                return false;
            }
            seenKeys.insert(item.key);

            // Rule 3: Key must sit in the exact bucket matching its hash value
            size_t expectedBucket = hashFunc(item.key, M);
            if (expectedBucket != i) {
                cout << "[FAIL] Key '" << item.key << "' is in bucket " << i
                     << " but hash says bucket " << expectedBucket << "\n";
                return false;
            }
        }
    }

    // Rule 4: Reported size N must equal actual number of stored entries
    if (actualCount != reportedSize) {
        cout << "[FAIL] Size mismatch! Reported N=" << reportedSize
             << ", actual entries=" << actualCount << "\n";
        return false;
    }

    return true;
}

// =========================================================================
// 2. INVARIANT CHECKER FOR STRUCTURE B (LINEAR PROBING WITH TOMBSTONES)
// =========================================================================
enum SlotState { EMPTY, OCCUPIED, TOMBSTONE };

struct ProbeSlot {
    string key;
    int value;
    SlotState state;
};

// Checks all internal rules of a Linear Probing hash table
bool checkLinearProbingInvariants(
    const vector<ProbeSlot>& table,           // Flat array of slots
    size_t reportedSize,                      // Table's stored N counter
    double maxLoadFactor,                     // Resizing threshold (e.g., 0.5 or 0.7)
    size_t (*hashFunc)(const string&, size_t) // Hash function returning 0..M-1
) {
    size_t M = table.size();
    if (M == 0) {
        cout << "[FAIL] Table capacity M cannot be 0.\n";
        return false;
    }

    // Rule 1: Load factor must not exceed threshold
    double currentAlpha = (double)reportedSize / (double)M;
    if (currentAlpha > maxLoadFactor) {
        cout << "[FAIL] Load factor " << currentAlpha << " exceeds max " << maxLoadFactor << "\n";
        return false;
    }

    size_t actualOccupied = 0;
    unordered_set<string> seenKeys;

    for (size_t i = 0; i < M; i++) {
        if (table[i].state == OCCUPIED) {
            actualOccupied++;
            const string& key = table[i].key;

            // Rule 2: No duplicate keys allowed
            if (seenKeys.count(key)) {
                cout << "[FAIL] Duplicate key found in Linear Probing: " << key << "\n";
                return false;
            }
            seenKeys.insert(key);

            // Rule 3: Probe sequence check (NO EMPTY slots between home index and slot i)
            size_t home = hashFunc(key, M);
            size_t curr = home;
            bool reached = false;

            for (size_t step = 0; step < M; step++) {
                if (table[curr].state == EMPTY) {
                    cout << "[FAIL] Broken probe chain! Key '" << key
                         << "' has an EMPTY slot at index " << curr
                         << " between home " << home << " and its position " << i << "\n";
                    return false;
                }
                if (curr == i) {
                    reached = true;
                    break;
                }
                curr = (curr + 1) % M; // Wrap around circularly
            }

            if (!reached) {
                cout << "[FAIL] Key '" << key << "' is unreachable from its home bucket.\n";
                return false;
            }
        }
    }

    // Rule 4: Reported size N must equal actual OCCUPIED slots (ignoring TOMBSTONES)
    if (actualOccupied != reportedSize) {
        cout << "[FAIL] Size mismatch! Reported N=" << reportedSize
             << ", actual OCCUPIED=" << actualOccupied << "\n";
        return false;
    }

    return true;
}

// =========================================================================
// 3. STANDALONE SELF-TEST (Proves the Invariant Checker catches bugs!)
// =========================================================================
size_t simpleHash(const string& key, size_t M) {
    size_t h = 0;
    for (char c : key) h = (h * 31 + c) % M;
    return h;
}

int main() {
    // Test 1: Valid Separate Chaining state
    vector<vector<ChainNode>> validBuckets(5);
    validBuckets[simpleHash("apple", 5)].push_back({"apple", 10});
    validBuckets[simpleHash("banana", 5)].push_back({"banana", 20});

    assert(checkSeparateChainingInvariants(validBuckets, 2, 0.75, simpleHash) == true);
    cout << "[PASS] Separate Chaining valid state passed all invariant checks.\n";

    // Test 2: Valid Linear Probing state with a Tombstone
    vector<ProbeSlot> probeTable(5, {"", 0, EMPTY});
    size_t h1 = simpleHash("cat", 5);
    probeTable[h1] = {"", 0, TOMBSTONE};              // Deleted key left a tombstone
    probeTable[(h1 + 1) % 5] = {"dog", 99, OCCUPIED}; // Colliding key sits right after it

    assert(checkLinearProbingInvariants(probeTable, 1, 0.75, simpleHash) == true);
    cout << "[PASS] Linear Probing with tombstone passed all invariant checks.\n";

    // Test 3: Deliberately break the probe chain (replace TOMBSTONE with EMPTY)
    probeTable[h1].state = EMPTY;
    assert(checkLinearProbingInvariants(probeTable, 1, 0.75, simpleHash) == false);
    cout << "[PASS] Invariant checker successfully caught the broken probe chain bug!\n";

    return 0;
}
