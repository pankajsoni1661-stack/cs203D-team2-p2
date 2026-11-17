#include "StructureA.h"

// Constructor
StructureA::StructureA() {
    capacity = 16;
    totalKeys = 0;
    buckets = new Node*[capacity]();
}

// Destructor
StructureA::~StructureA() {
    for (int i = 0; i < capacity; i++) {
        Node* current = buckets[i];

        while (current != nullptr) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }

    delete[] buckets;
}

// Polynomial hash function
unsigned long long StructureA::polynomialHash(const string& key) {
    unsigned long long hash = 0;

    for (int i = 0; i < (int)key.length(); i++) {
        hash = hash * 31 + (unsigned char)key[i];
    }

    return hash;
}

// Insert or update
void StructureA::insert(const string& key, int value) {
    int index = polynomialHash(key) % capacity;

    Node* current = buckets[index];

    while (current != nullptr) {
        if (current->key == key) {
            current->value = value;
            return;
        }

        current = current->next;
    }

    if (totalKeys + 1 > capacity) {
        resize();
        index = polynomialHash(key) % capacity;
    }

    Node* newNode = new Node;
    newNode->key = key;
    newNode->value = value;
    newNode->next = buckets[index];

    buckets[index] = newNode;
    totalKeys++;
}

// Search with probe counting
bool StructureA::get(const string& key, int& value, int& probes) {
    probes = 0;

    int index = polynomialHash(key) % capacity;
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

// Simple search
bool StructureA::get(const string& key, int& value) {
    int probes = 0;
    return get(key, value, probes);
}

// Delete a key
bool StructureA::remove(const string& key) {
    int index = polynomialHash(key) % capacity;

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

// Number of stored keys
int StructureA::count() {
    return totalKeys;
}

// Number of buckets
int StructureA::bucketCount() {
    return capacity;
}

// Double the table capacity
void StructureA::resize() {
    int oldCapacity = capacity;
    Node** oldBuckets = buckets;

    capacity = capacity * 2;
    buckets = new Node*[capacity]();

    for (int i = 0; i < oldCapacity; i++) {
        Node* current = oldBuckets[i];

        while (current != nullptr) {
            Node* nextNode = current->next;

            int index = polynomialHash(current->key) % capacity;

            current->next = buckets[index];
            buckets[index] = current;

            current = nextNode;
        }
    }

    delete[] oldBuckets;
}
