#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

using namespace std;

// --- 1. OUR TRUSTED ORACLE (Sorted Array + Binary Search) ---
struct Entry {
    string key;
    int value;
};

class SortedArrayOracle {
private:
    vector<Entry> arr;

    int binarySearch(const string& key) {
        int low = 0;
        int high = (int)arr.size() - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (arr[mid].key == key) return mid;
            else if (arr[mid].key < key) low = mid + 1;
            else high = mid - 1;
        }
        return low;
    }

public:
    void insert(const string& key, int value) {
        int pos = binarySearch(key);
        if (pos < (int)arr.size() && arr[pos].key == key) {
            arr[pos].value = value;
        } else {
            arr.insert(arr.begin() + pos, {key, value});
        }
    }

    string get(const string& key) {
        int pos = binarySearch(key);
        if (pos < (int)arr.size() && arr[pos].key == key) {
            return to_string(arr[pos].value);
        }
        return "NOT_FOUND";
    }

    bool remove(const string& key) {
        int pos = binarySearch(key);
        if (pos < (int)arr.size() && arr[pos].key == key) {
            arr.erase(arr.begin() + pos);
            return true;
        }
        return false;
    }

    int count() {
        return (int)arr.size();
    }
};

// --- 2. REALISTIC RANDOM KEY GENERATOR (Lengths 1 to 25, no spaces) ---
string makeRandomKey(mt19937& rng) {
    const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    uniform_int_distribution<int> lenDist(1, 25);
    uniform_int_distribution<int> charDist(0, (int)chars.size() - 1);

    int len = lenDist(rng);
    string s = "";
    for (int i = 0; i < len; i++) {
        s += chars[charDist(rng)];
    }
    return s;
}

// --- 3. DIFFERENTIAL TEST GENERATOR ---
int main() {
    const int FIXED_SEED = 42;
    const int NUM_OPS = 100000; // 10^5 random operations
    const int KEY_POOL = 500;   // 500 varied random keys reused across 10^5 ops

    mt19937 rng(FIXED_SEED);

    // Pre-generate 500 varied-length random keys
    vector<string> pool(KEY_POOL);
    for (int i = 0; i < KEY_POOL; i++) {
        pool[i] = makeRandomKey(rng);
    }

    uniform_int_distribution<int> opDist(1, 100);
    uniform_int_distribution<int> poolIdxDist(0, KEY_POOL - 1);
    uniform_int_distribution<int> valDist(1, 35000);

    ofstream opFile("tests/random_ops.txt");
    ofstream ansFile("tests/oracle_out.txt");

    SortedArrayOracle oracle;

    for (int i = 0; i < NUM_OPS; i++) {
        int choice = opDist(rng);
        string key = pool[poolIdxDist(rng)];

        if (choice <= 35) {
            // 35% Insert / Update (I)
            int val = valDist(rng);
            opFile << "I " << key << " " << val << "\n";
            oracle.insert(key, val);
        } 
        else if (choice <= 70) {
            // 35% Delete (D) - Prints NOT_FOUND if absent, nothing if deleted
            opFile << "D " << key << "\n";
            if (!oracle.remove(key)) {
                ansFile << "NOT_FOUND\n";
            }
        } 
        else if (choice <= 95) {
            // 25% Get / Lookup (G) - Prints integer value or NOT_FOUND
            opFile << "G " << key << "\n";
            ansFile << oracle.get(key) << "\n";
        } 
        else {
            // 5% Count (N)
            opFile << "N\n";
            ansFile << oracle.count() << "\n";
        }
    }

    opFile.close();
    ansFile.close();

    cout << "Generated " << NUM_OPS << " operations with varied random keys (seed=" << FIXED_SEED << ").\n";
    return 0;
}
