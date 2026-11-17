#ifndef STRUCTURE_A_H
#define STRUCTURE_A_H

#include <string>
#include <cstddef>

using namespace std;
// Seeded hash class
class SeededHash {
private:
    unsigned long long seed;

public:
    SeededHash(unsigned long long s);

    unsigned long long operator()(const string& key) const;
};


// Structure A - Separate Chaining
class StructureA {
private:

    struct Node {
        string key;
        int value;
        Node* next;
    };

    const SeededHash* seededHash_;

    Node** buckets;
    size_t capacity;
    int totalKeys;
    double maxLoad;

    unsigned long long polynomialHash(const string& key) const;
    unsigned long long hashKey(const string& key) const;

    void doubleCapacity();

public:

    StructureA(const SeededHash* hash = nullptr,
               size_t initialCapacity = 16,
               double load = 1.0);

    ~StructureA();

    StructureA(const StructureA&) = delete;
    StructureA& operator=(const StructureA&) = delete;

    void insert(const string& key, int value);

    bool get(const string& key, int& value) const;

    bool get(const string& key, int& value,
             size_t& probes) const;

    bool remove(const string& key);

    int count() const;
    size_t bucketCount() const;

    double loadFactor() const;
    bool checkInvariants() const;
};

#endif
