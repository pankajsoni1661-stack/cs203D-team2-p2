#ifndef LINEAR_PROBING_HPP
#define LINEAR_PROBING_HPP

#include <iostream>
#include <string>
#include "../string_hash.hpp"

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
private:
    bool useSeeded;
    unsigned long long seed;
public:
    Slot* table;
    int capacity;
    int num_keys;
    int num_tombstones;
    unsigned long long base;
    long long probes;
    double max_load_factor;

    LinearProbingTable(int init_cap = 16, unsigned long long hash_base = 31,
                       bool use_seeded_hash = false, unsigned long long hash_seed = 42) {
        capacity = init_cap;
        num_keys = 0;
        num_tombstones = 0;
        probes = 0;
        max_load_factor = 0.7;

        base = hash_base;
        useSeeded = use_seeded_hash;
        seed = hash_seed;

        table = new Slot[capacity];
    }

    ~LinearProbingTable() {
        delete[] table;
    }

    int hashFunction(const string& key) {
        const auto hash = useSeeded ? string_hash::seededHash(key, seed)
                                    : string_hash::polynomialHash(key, base);
        return static_cast<int>(hash % capacity);
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
        if ((double)(num_keys + num_tombstones + 1) / capacity > max_load_factor) {
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
