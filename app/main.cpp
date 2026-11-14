#include <iostream>
#include <string>
#include "../src/structA/StructureA.h"
#include "../src/structB/LinearProbing.hpp"

using namespace std;

int keyCount(const StructureA& table) {
    return table.count();
}

int keyCount(LinearProbingTable& table) {
    return table.size();
}

template <typename Table>
int runCommands(Table& table) {
    char command;
    string key;
    int value;

    while (cin >> command) {
        if (command == 'I') {
            if (!(cin >> key >> value)) return 1;
            table.insert(key, value);
        } else if (command == 'G') {
            if (!(cin >> key)) return 1;
            if (table.get(key, value)) {
                cout << value << '\n';
            } else {
                cout << "NOT_FOUND\n";
            }
        } else if (command == 'D') {
            if (!(cin >> key)) return 1;
            if (!table.remove(key)) {
                cout << "NOT_FOUND\n";
            }
        } else if (command == 'N') {
            cout << keyCount(table) << '\n';
        } else {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 3 || string(argv[1]) != "--structure") return 1;

    string choice = argv[2];

    if (choice == "A") {
        StructureA table;
        return runCommands(table);
    }

    if (choice == "B") {
        LinearProbingTable table;
        return runCommands(table);
    }

    return 1;
}

