#include <iostream>
#include <string>
#include "LinearProbing.hpp"

using namespace std;

int main() {
    // Optimize standard I/O operations for speed (handling 10^6 inputs smoothly)
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    LinearProbingTable ht;
    char cmd;

    while (cin >> cmd) {
        if (cmd == 'I') {
            string key;
            int val;
            cin >> key >> val;
            ht.insert(key, val);
        }
        else if (cmd == 'G') {
            string key;
            cin >> key;
            int val = 0;
            if (ht.get(key, val)) {
                cout << val << "\n";
            } else {
                cout << "NOT_FOUND\n";
            }
        }
        else if (cmd == 'D') {
            string key;
            cin >> key;
            if (!ht.remove(key)) {
                cout << "NOT_FOUND\n";
            }
        }
        else if (cmd == 'N') {
            cout << ht.size() << "\n";
        }
    }
    return 0;
}
