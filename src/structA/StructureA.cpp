#include "StructureA.h"
#include <limits>
#include <stdexcept>

using namespace std;

StructureA::StructureA()
    : capacity_(16), buckets_(new Node*[capacity_]{}), size_(0) {}

StructureA::~StructureA() {
    for (size_t i = 0; i < capacity_; ++i) {
        Node* node = buckets_[i];

        while (node != nullptr) {
            Node* next = node->next;
            delete node;
            node = next;
        }
    }

    delete[] buckets_;
}

unsigned long long StructureA::polynomialHash(const string& key) {
    unsigned long long hash = 0;

    for (unsigned char ch : key) {
        hash = hash * 31 + ch;
    }

    return hash;
}

void StructureA::insert(const string& key, int value) {
    const auto hash = polynomialHash(key);
    size_t index = hash % capacity_;

    // Update the existing key if it is already in this chain.
    for (Node* node = buckets_[index]; node != nullptr; node = node->next) {
        if (node->key == key) {
            node->value = value;
            return;
        }
    }

    if (size_ == numeric_limits<int>::max()) {
        throw length_error("StructureA key count limit reached");
    }

    // Double the bucket count before the load factor exceeds 1.0.
    if (static_cast<size_t>(size_) == capacity_) {
        doubleCapacity();
        index = hash % capacity_;
    }

    buckets_[index] = new Node{key, value, buckets_[index]};
    ++size_;
}

bool StructureA::get(const string& key, int& value) const {
    size_t probes;
    return get(key, value, probes);
}

bool StructureA::get(const string& key, int& value, size_t& probes) const {
    probes = 0;
    const size_t index = polynomialHash(key) % capacity_;

    for (const Node* node = buckets_[index]; node != nullptr; node = node->next) {
        ++probes;
        if (node->key == key) {
            value = node->value;
            return true;
        }
    }

    return false;
}

bool StructureA::remove(const string& key) {
    const size_t index = polynomialHash(key) % capacity_;
    Node* previous = nullptr;
    Node* node = buckets_[index];

    while (node != nullptr && node->key != key) {
        previous = node;
        node = node->next;
    }

    if (node == nullptr) {
        return false;
    }

    if (previous == nullptr) {
        buckets_[index] = node->next;
    } else {
        previous->next = node->next;
    }

    delete node;
    --size_;
    return true;
}

int StructureA::count() const {
    return size_;
}

size_t StructureA::bucketCount() const {
    return capacity_;
}

void StructureA::doubleCapacity() {
    const size_t newCapacity = capacity_ * 2;
    Node** newBuckets = new Node*[newCapacity]{};

    for (size_t i = 0; i < capacity_; ++i) {
        Node* node = buckets_[i];

        while (node != nullptr) {
            // Save the old link before moving the node into its new chain.
            Node* next = node->next;
            const size_t index = polynomialHash(node->key) % newCapacity;
            node->next = newBuckets[index];
            newBuckets[index] = node;
            node = next;
        }
    }

    delete[] buckets_;
    buckets_ = newBuckets;
    capacity_ = newCapacity;
}
