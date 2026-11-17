#ifndef STRUCTURE_A_H
#define STRUCTURE_A_H

#include <string>
#include <cstddef>
using namespace std;

class SeededHash;
class StructureA {
private:
    struct Node {
        string key;
        int value;
        Node* next;
    };
    const SeededHash* seededHash_;
    size_t capacity_;
    Node** buckets_;
    int size_;
    double maxLoad_;

    static unsigned long long polynomialHash(const string& key);
    unsigned long long hashKey(const string& key) const;

    void doubleCapacity();

public:
    explicit StructureA(const SeededHash* hash = nullptr,
                        size_t initialCapacity = 16,
                        double maxLoad = 1.0);

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
