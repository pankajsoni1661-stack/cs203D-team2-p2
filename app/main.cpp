#include <iostream>
#include <string>
#include "../src/structA/ChainingTable.hpp"
#include "../src/structB/LinearProbingTable.hpp"

using namespace std;

template <typename Table>
int runCommands(Table& table) {
    string command, key;
    long long value;

    while (cin >> command) {
        if (command == "I") {
            if (!(cin >> key >> value)) return 1;
            table.put(key, value);
        }
        else if (command == "G") {
            if (!(cin >> key)) return 1;

            if (table.get(key, value)) {
                cout << value << '\n';
            } else {
                cout << "NOT_FOUND\n";
            }
        }
        else if (command == "D") {
            if (!(cin >> key)) return 1;

            if (!table.erase(key)) {
                cout << "NOT_FOUND\n";
            }
        }
        else if (command == "N") {
            cout << table.size() << '\n';
        }
        else {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 3 || string(argv[1]) != "--structure") {
        return 1;
    }

    string choice = argv[2];

    if (choice == "A") {
        ChainingTable table;
        return runCommands(table);
    }
    if (choice == "B") {
        LinearProbingTable table;
        return runCommands(table);
    }
    return 1;
}
