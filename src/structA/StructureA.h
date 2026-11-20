#ifndef STRUCTURE_A_H
#define STRUCTURE_A_H

#include <string>
#include <cstddef>

using namespace std;

// Structure A - Separate Chaining
class StructureA {
private:
    struct Node {
        string key;
        int value;
        Node* next;
    };

    Node** buckets;
    size_t capacity;
    int totalKeys;
    double maxLoad;

    bool useSeeded;
    unsigned long long seed;

    unsigned long long polynomialHash(const string& key) const;
    unsigned long long seededHash(const string& key) const;
    unsigned long long hashKey(const string& key) const;

    void doubleCapacity();

public:
    // Pass useSeeded = true (with an optional seed) for seeded mode, or false for fixed base-31
    StructureA(bool useSeeded = false,
               unsigned long long seed = 42,
               size_t initialCapacity = 16,
               double load = 1.0);

    ~StructureA();

    StructureA(const StructureA&) = delete;
    StructureA& operator=(const StructureA&) = delete;

    void insert(const string& key, int value);
    bool get(const string& key, int& value) const;
    bool get(const string& key, int& value, size_t& probes) const;
    bool remove(const string& key);

    int count() const;
    size_t bucketCount() const;
    double loadFactor() const;
    bool checkInvariants() const;
};

#endif
