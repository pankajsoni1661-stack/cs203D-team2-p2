#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
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

template <typename Table>
int runWordCounter(Table& table, istream& input) {
    vector<string> distinctWords;
    string word;
    char character;

    // Flush the current word at each separator and once more at EOF.
    while (true) {
        bool hasCharacter = static_cast<bool>(input.get(character));
        if (hasCharacter && character >= 'A' && character <= 'Z') {
            character = static_cast<char>(character - 'A' + 'a');
        }
        if (hasCharacter && character >= 'a' && character <= 'z') {
            word += character;
            continue;
        }
        if (!word.empty()) {
            int count = 0;
            if (!table.get(word, count)) {
                distinctWords.push_back(word);
            }
            if (count == numeric_limits<int>::max()) {
                cerr << "Word count is too large for the table.\n";
                return 1;
            }
            table.insert(word, count + 1);
            word.clear();
        }
        if (!hasCharacter) break;
    }
    if (!input.eof() || input.bad()) {
        cerr << "Could not read the text file.\n";
        return 1;
    }

    // This vector controls output order. The table holds every count.
    for (const string& key : distinctWords) {
        int count;
        if (!table.get(key, count)) return 1;
        cout << key << ' ' << count << '\n';
    }
    return cout ? 0 : 1;
}

template <typename Table>
int runApplication(Table& table, const char* filename) {
    if (filename == nullptr) return runCommands(table);

    ifstream input(filename, ios::binary);
    if (!input) {
        cerr << "Could not open " << filename << '\n';
        return 1;
    }
    return runWordCounter(table, input);
}

int main(int argc, char* argv[]) {
    if ((argc != 3 && argc != 5) || string(argv[1]) != "--structure") return 1;
    if (argc == 5 && string(argv[3]) != "--words") return 1;

    const char* filename = argc == 5 ? argv[4] : nullptr;

    string choice = argv[2];

    if (choice == "A") {
        StructureA table;
        return runApplication(table, filename);
    }

    if (choice == "B") {
        LinearProbingTable table;
        return runApplication(table, filename);
    }

    return 1;
}

