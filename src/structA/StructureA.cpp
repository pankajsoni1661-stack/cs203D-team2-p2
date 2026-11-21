#include "StructureA.h"
#include "../string_hash.hpp"

using namespace std;
StructureA::StructureA(bool useSeededMode,
                       unsigned long long hashSeed,
                       size_t initialCapacity,
                       double load) {
    useSeeded = useSeededMode;
    seed = hashSeed;

    if (initialCapacity == 0) {
        initialCapacity = 1;
    }

    capacity = initialCapacity;
    totalKeys = 0;

    if (load > 0 && load <= 1.0) {
        maxLoad = load;
    } else {
        maxLoad = 1.0;
    }

    buckets = new Node*[capacity]();
}

StructureA::~StructureA() {
    for (size_t i = 0; i < capacity; i++) {
        Node* current = buckets[i];
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
    delete[] buckets;
}


unsigned long long StructureA::hashKey(const string& key) const {
    if (useSeeded) {
        return string_hash::seededHash(key, seed);
    }
    return string_hash::polynomialHash(key);
}
void StructureA::insert(const string& key, int value) {
    size_t index = hashKey(key) % capacity;
    Node* current = buckets[index];

    while (current != nullptr) {
        if (current->key == key) {
            current->value = value;
            return;
        }
        current = current->next;
    }
  
    while ((totalKeys + 1.0) / capacity > maxLoad) {
        doubleCapacity();
    }

    index = hashKey(key) % capacity;
    Node* newNode = new Node;
    newNode->key = key;
    newNode->value = value;
    newNode->next = buckets[index];

    buckets[index] = newNode;
    totalKeys++;
}

bool StructureA::get(const string& key, int& value, size_t& probes) const {
    probes = 0;
    size_t index = hashKey(key) % capacity;
    Node* current = buckets[index];

    while (current != nullptr) {
        probes++;
        if (current->key == key) {
            value = current->value;
            return true;
        }
        current = current->next;
    }
    return false;
}

bool StructureA::get(const string& key, int& value) const {
    size_t probes = 0;
    return get(key, value, probes);
}

bool StructureA::remove(const string& key) {
    size_t index = hashKey(key) % capacity;
    Node* current = buckets[index];
    Node* previous = nullptr;

    while (current != nullptr) {
        if (current->key == key) {
            if (previous == nullptr) {
                buckets[index] = current->next;
            } else {
                previous->next = current->next;
            }
            delete current;
            totalKeys--;
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false;
}

int StructureA::count() const {
    return totalKeys;
}

size_t StructureA::bucketCount() const {
    return capacity;
}

double StructureA::loadFactor() const {
    return (double)totalKeys / capacity;
}

bool StructureA::checkInvariants() const {
    int total = 0;
    for (size_t i = 0; i < capacity; i++) {
        Node* current = buckets[i];
        while (current != nullptr) {
            total++;
            if (total > totalKeys) {
                return false;
            }

            size_t index = hashKey(current->key) % capacity;
            if (index != i) {
                return false;
            }

            int value = 0;
            if (!get(current->key, value)) {
                return false;
            }
            if (value != current->value) {
                return false;
            }

            current = current->next;
        }
    }
    return total == totalKeys;
}

void StructureA::doubleCapacity() {
    size_t newCapacity = capacity * 2;
    Node** newBuckets = new Node*[newCapacity]();

    for (size_t i = 0; i < capacity; i++) {
        Node* current = buckets[i];
        while (current != nullptr) {
            Node* next = current->next;
            size_t index = hashKey(current->key) % newCapacity;
            current->next = newBuckets[index];
            newBuckets[index] = current;
            current = next;
        }
    }

    delete[] buckets;
    buckets = newBuckets;
    capacity = newCapacity;
}
