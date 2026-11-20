#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

void createCollisionFile(const string& filename, int numBlocks) {
    int numKeys = 1 << numBlocks; // 2^numBlocks distinct keys
    ofstream out(filename);

    vector<string> keys;
    keys.reserve(numKeys);

    // Build 2^numBlocks strings using 2-char blocks "aB" and "`a" (both hash to 3073)
    for (int mask = 0; mask < numKeys; mask++) {
        string s = "";
        for (int b = 0; b < numBlocks; b++) {
            s += (mask & (1 << b)) ? "aB" : "`a";
        }
        keys.push_back(s);
        out << "I " << s << " " << (mask + 1) << "\n";
    }

    // Also query every key so the benchmark measures worst-case O(N) chain/probe lookups
    for (int i = 0; i < numKeys; i++) {
        out << "G " << keys[i] << "\n";
    }

    out.close();
    cout << "Created " << filename << " with " << numKeys
         << " distinct colliding keys (" << numKeys * 2 << " total ops).\n";
}

int main() {
    createCollisionFile("tests/collision_1024.txt", 10); // 1,024 distinct keys
    createCollisionFile("tests/collision_4096.txt", 12); // 4,096 distinct keys
    createCollisionFile("tests/collision_8192.txt", 13); // 8,192 distinct keys
    return 0;
}
