#ifndef LINEAR_PROBING_HPP
#define LINEAR_PROBING_HPP

#include <iostream>
#include <string>
#include <random>
#include "../structA/StructureA.h"

using namespace std;

const int EMPTY = 0;
const int OCCUPIED = 1;
const int TOMBSTONE = 2;

struct Slot {
    string key;
    int value;
    int state;

    Slot() {
        key = "";
        value = 0;
        state = EMPTY;
    }
};

class LinearProbingTable {
    // The application owns this object and keeps it alive with the table.
    const SeededHash* seededHash_ = nullptr;

public:
    Slot* table;
    int capacity;
    int num_keys;
    int num_tombstones;
    unsigned long long base;
    long long probes;

    LinearProbingTable(int init_cap = 16, unsigned long long hash_base = 31, bool use_seeded_hash = false) {
        capacity = init_cap;
        num_keys = 0;
        num_tombstones = 0;
        probes = 0;

        if (use_seeded_hash) {
            random_device rd;
            mt19937_64 gen(rd());
            uniform_int_distribution<unsigned long long> dist(257, 1000000007ULL);
            base = dist(gen) | 1ULL;
        } else {
            base = hash_base;
        }

        table = new Slot[capacity];
    }

    LinearProbingTable(int init_cap, unsigned long long hash_base,
                       const SeededHash* hash)
        : LinearProbingTable(init_cap, hash_base, false) {
        seededHash_ = hash;
    }

    ~LinearProbingTable() {
        delete[] table;
    }

    int hashFunction(const string& key) {
        if (seededHash_ != nullptr) {
            return (*seededHash_)(key) % capacity;
        }

        unsigned long long h = 0;
        for (char c : key) {
            h = h * base + (unsigned char)c;
        }
        return h % capacity;
    }

    void resize() {
        int old_cap = capacity;
        Slot* old_table = table;

        capacity = capacity * 2;
        table = new Slot[capacity];
        num_keys = 0;
        num_tombstones = 0;

        for (int i = 0; i < old_cap; i++) {
            if (old_table[i].state == OCCUPIED) {
                insert(old_table[i].key, old_table[i].value);
            }
        }
        delete[] old_table;
    }

    void insert(const string& key, int value) {
        if ((double)(num_keys + num_tombstones + 1) / capacity > 0.7) {
            resize();
        }

        int idx = hashFunction(key);
        int first_tombstone = -1;

        for (int i = 0; i < capacity; i++) {
            if (table[idx].state == EMPTY) {
                int target = (first_tombstone != -1) ? first_tombstone : idx;
                if (first_tombstone != -1) num_tombstones--;

                table[target].key = key;
                table[target].value = value;
                table[target].state = OCCUPIED;
                num_keys++;
                return;
            }
            else if (table[idx].state == TOMBSTONE) {
                if (first_tombstone == -1) first_tombstone = idx;
            }
            else if (table[idx].state == OCCUPIED && table[idx].key == key) {
                table[idx].value = value;
                return;
            }

            idx = (idx + 1) % capacity;
        }
    }

    bool get(const string& key, int& out_value) {
        int idx = hashFunction(key);

        for (int i = 0; i < capacity; i++) {
            probes++;
            if (table[idx].state == EMPTY) {
                return false;
            }
            if (table[idx].state == OCCUPIED && table[idx].key == key) {
                out_value = table[idx].value;
                return true;
            }
            idx = (idx + 1) % capacity;
        }
        return false;
    }

    bool remove(const string& key) {
        int idx = hashFunction(key);

        for (int i = 0; i < capacity; i++) {
            if (table[idx].state == EMPTY) {
                return false;
            }
            if (table[idx].state == OCCUPIED && table[idx].key == key) {
                table[idx].state = TOMBSTONE;
                num_keys--;
                num_tombstones++;
                return true;
            }
            idx = (idx + 1) % capacity;
        }
        return false;
    }

    int size() {
        return num_keys;
    }

    bool checkInvariants() {
        int counted = 0;
        for (int i = 0; i < capacity; i++) {
            if (table[i].state == OCCUPIED) {
                counted++;
                int val = 0;
                if (!get(table[i].key, val) || val != table[i].value) return false;
            }
        }
        return counted == num_keys;
    }
};

#endif
