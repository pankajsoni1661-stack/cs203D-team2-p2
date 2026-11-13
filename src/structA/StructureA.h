#ifndef STRUCTURE_A_H
#define STRUCTURE_A_H

#include <cstddef>
#include <string>

class StructureA {
public:
    StructureA();
    ~StructureA();

    StructureA(const StructureA&) = delete;
    StructureA& operator=(const StructureA&) = delete;

    void insert(const std::string& key, int value);
    bool get(const std::string& key, int& value) const;
    bool remove(const std::string& key);
    int count() const;

    bool get(const std::string& key, int& value, std::size_t& probes) const;
    std::size_t bucketCount() const;

    static unsigned long long polynomialHash(const std::string& key);

private:
    struct Node {
        std::string key;
        int value;
        Node* next;
    };

    void doubleCapacity();

    std::size_t capacity_;
    Node** buckets_;
    int size_;
};

#endif
