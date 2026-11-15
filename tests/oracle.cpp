#include <iostream>
#include <vector>
#include <string>

using namespace std;

// A simple struct to hold one key-value pair
struct Entry {
    string key;
    int value;
};

class SortedArrayOracle {
private:
    vector<Entry> arr; // Keeps all entries sorted by key

    // Standard Binary Search: returns the index where 'key' is (or should be)
    int binarySearch(const string& key) {
        int low = 0;
        int high = (int)arr.size() - 1;

        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (arr[mid].key == key) {
                return mid; // Key found at index mid
            } else if (arr[mid].key < key) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        return low; // Key not found; 'low' is the exact spot to insert it
    }

public:
    // I <key> <value>: Insert key-value, or update value if key already exists
    void insert(const string& key, int value) {
        int pos = binarySearch(key);

        if (pos < (int)arr.size() && arr[pos].key == key) {
            arr[pos].value = value; // Update existing key
        } else {
            arr.insert(arr.begin() + pos, {key, value}); // Insert in sorted order
        }
    }

    // G <key>: Return the value as a string, or "NOT_FOUND"
    string get(const string& key) {
        int pos = binarySearch(key);

        if (pos < (int)arr.size() && arr[pos].key == key) {
            return to_string(arr[pos].value);
        }
        return "NOT_FOUND";
    }

    // D <key>: Delete key if present; return true if deleted, false if absent
    bool remove(const string& key) {
        int pos = binarySearch(key);

        if (pos < (int)arr.size() && arr[pos].key == key) {
            arr.erase(arr.begin() + pos);
            return true;
        }
        return false;
    }

    // N: Return total number of active keys
    int count() {
        return (int)arr.size();
    }
};

int main() {
    SortedArrayOracle oracle;
    string command;

    // Read commands from input until end of file
    while (cin >> command) {
        if (command == "I") {
            string key;
            int val;
            cin >> key >> val;
            oracle.insert(key, val);
        } 
        else if (command == "G") {
            string key;
            cin >> key;
            cout << oracle.get(key) << "\n";
        } 
        else if (command == "D") {
            string key;
            cin >> key;
            if (!oracle.remove(key)) {
                cout << "NOT_FOUND\n";
            }
        } 
        else if (command == "N") {
            cout << oracle.count() << "\n";
        }
    }

    return 0;
}
