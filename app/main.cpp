#include <fstream>
#include <chrono>
#include <climits>
#include <iomanip>
#include <iostream>
#include <sstream>
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
int runCommands(Table& table, istream& input = cin, ostream& output = cout) {
    string command;
    string key;
    int value;

    while (input >> command) {
        if (command == "I") {
            if (!(input >> key >> value)) return 1;
            table.insert(key, value);
        } else if (command == "G") {
            if (!(input >> key)) return 1;
            if (table.get(key, value)) {
                output << value << '\n';
            } else {
                output << "NOT_FOUND\n";
            }
        } else if (command == "D") {
            if (!(input >> key)) return 1;
            if (!table.remove(key)) {
                output << "NOT_FOUND\n";
            }
        } else if (command == "N") {
            output << keyCount(table) << '\n';
        } else {
            return 1;
        }
    }

    if (input.bad() || !output) {
        return 1;
    }
    return 0;
}

// Add one word to the selected table.
template <typename Table>
bool addWord(Table& table, vector<string>& words, string word) {
    if (word.empty()) {
        return true;
    }

    int count = 0;
    if (!table.get(word, count)) {
        words.push_back(word);
    }
    if (count == INT_MAX) {
        cerr << "Word count is too large.\n";
        return false;
    }
    table.insert(word, count + 1);
    return true;
}

template <typename Table>
int runWordCounter(Table& table, istream& input, ostream& output) {
    vector<string> words;
    string word;
    char ch;

    // Read one character at a time.
    while (input.get(ch)) {
        if (ch >= 'A' && ch <= 'Z') {
            ch = ch - 'A' + 'a';
        }

        if (ch >= 'a' && ch <= 'z') {
            word += ch;
        } else {
            if (!addWord(table, words, word)) {
                return 1;
            }
            word = "";
        }
    }

    if (!input.eof() || input.bad()) {
        cerr << "Could not read the text file.\n";
        return 1;
    }

    // The file may finish immediately after its last word.
    if (!addWord(table, words, word)) {
        return 1;
    }

    // The vector stores word order. The hash table stores the counts.
    for (int i = 0; i < (int)words.size(); i++) {
        int count = 0;
        if (!table.get(words[i], count)) {
            return 1;
        }
        output << words[i] << ' ' << count << '\n';
    }

    if (!output) {
        return 1;
    }
    return 0;
}

template <typename Table>
int runWordFile(Table& table, string filename, ostream& output) {
    ifstream input(filename, ios::binary);
    if (!input) {
        cerr << "Could not open " << filename << '\n';
        return 1;
    }
    return runWordCounter(table, input, output);
}

template <typename Table>
int compareWordCounts(Table& fixed, Table& seeded, string filename) {
    ostringstream fixedCounts;
    if (runWordFile(fixed, filename, fixedCounts) != 0) return 1;

    ostringstream seededCounts;
    if (runWordFile(seeded, filename, seededCounts) != 0) return 1;

    if (fixedCounts.str() != seededCounts.str()) {
        cerr << "Fixed and seeded word counts differ.\n";
        return 1;
    }

    cout << "Fixed and seeded word counts match for "
         << keyCount(fixed) << " distinct words.\n";
    if (!cout) {
        return 1;
    }
    return 0;
}

struct ReplayResult {
    string answers;
    double milliseconds;
};

template <typename Table>
int measureReplay(Table& table, const string& operations, ReplayResult& result) {
    istringstream input(operations);
    ostringstream output;
    const auto start = chrono::steady_clock::now();
    int status = runCommands(table, input, output);
    const auto stop = chrono::steady_clock::now();
    result.milliseconds = chrono::duration<double, milli>(stop - start).count();
    result.answers = output.str();
    return status;
}

template <typename Table>
bool sameFinalState(Table& fixed, Table& seeded, const string& operations) {
    if (keyCount(fixed) != keyCount(seeded)) return false;

    // Check every key mentioned in the input, including deleted and updated keys.
    istringstream input(operations);
    string command, key;
    int insertedValue;
    while (input >> command) {
        if (command == "N") continue;
        input >> key;
        if (command == "I") input >> insertedValue;
        int fixedValue = 0, seededValue = 0;
        bool fixedFound = fixed.get(key, fixedValue);
        bool seededFound = seeded.get(key, seededValue);
        if (fixedFound != seededFound || (fixedFound && fixedValue != seededValue)) {
            return false;
        }
    }
    return true;
}

template <typename Table>
int compareReplay(Table& fixed, Table& seeded, const string& operations,
                  int run, const string& structure, unsigned long long seed,
                  ostream& rows, double& fixedTotal, double& seededTotal) {
    ReplayResult fixedResult, seededResult;
    if (measureReplay(fixed, operations, fixedResult) != 0 ||
        measureReplay(seeded, operations, seededResult) != 0) {
        cerr << "Invalid command input for attack replay.\n";
        return 1;
    }
    if (fixedResult.answers != seededResult.answers ||
        !sameFinalState(fixed, seeded, operations)) {
        cerr << "Fixed and seeded replay results differ.\n";
        return 1;
    }

    fixedTotal += fixedResult.milliseconds;
    seededTotal += seededResult.milliseconds;
    rows << run << ',' << structure << ",fixed,NA,"
         << fixedResult.milliseconds << ',' << keyCount(fixed) << ",PASS\n";
    rows << run << ',' << structure << ",seeded," << seed << ','
         << seededResult.milliseconds << ',' << keyCount(seeded) << ",PASS\n";
    return 0;
}

int runAttack(const string& structure, unsigned long long seed, string filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Could not open " << filename << '\n';
        return 1;
    }
    // Read the file before starting the timer.
    string operations;
    string line;
    while (getline(file, line)) {
        operations += line;
        operations += '\n';
    }
    if (file.bad()) {
        return 1;
    }
    if (operations.find_first_not_of(" \t\r\n") == string::npos) {
        cerr << "The attack input is empty.\n";
        return 1;
    }

    double fixedTotal = 0, seededTotal = 0;
    ostringstream rows;
    rows << fixed << setprecision(6);
    rows << "run,structure,hash_mode,seed,replay_ms,final_keys,verification\n";
    for (int run = 1; run <= 5; ++run) {
        int status;
        if (structure == "A") {
            StructureA fixedTable;
            StructureA seededTable(true, seed);
            status = compareReplay(fixedTable, seededTable, operations,
                                   run, structure, seed, rows, fixedTotal, seededTotal);
        } else {
            LinearProbingTable fixedTable;
            LinearProbingTable seededTable(16, 31, true, seed);
            status = compareReplay(fixedTable, seededTable, operations,
                                   run, structure, seed, rows, fixedTotal, seededTotal);
        }
        if (status != 0) return status;
    }
    rows << "mean," << structure << ",fixed,NA," << fixedTotal / 5 << ",NA,PASS\n";
    rows << "mean," << structure << ",seeded," << seed << ','
         << seededTotal / 5 << ",NA,PASS\n";
    cout << rows.str();
    if (!cout) {
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 3 || string(argv[1]) != "--structure") return 1;
    string choice = argv[2];
    if (choice != "A" && choice != "B") return 1;

    int next = 3;
    bool useSeed = false;
    unsigned long long seed = 0;
    if (next < argc && string(argv[next]) == "--seed") {
        if (next + 1 >= argc) return 1;
        string seedText = argv[next + 1];
        if (seedText.empty()) {
            return 1;
        }
        for (int i = 0; i < (int)seedText.size(); i++) {
            if (seedText[i] < '0' || seedText[i] > '9') {
                return 1;
            }
        }
        istringstream seedInput(seedText);
        if (!(seedInput >> seed)) return 1;
        useSeed = true;
        next += 2;
    }

    string filename;
    string mode = "commands";
    if (next < argc) {
        if (argc - next != 2) return 1;
        mode = argv[next];
        if (mode != "--words" && mode != "--compare-words" && mode != "--attack") {
            return 1;
        }
        if ((mode == "--compare-words" || mode == "--attack") && !useSeed) {
            cerr << "Pass --seed before the comparison option.\n";
            return 1;
        }
        filename = argv[next + 1];
    }

    if (mode == "--attack") {
        return runAttack(choice, seed, filename);
    }

    if (choice == "A") {
        StructureA table(useSeed, seed);
        if (mode == "--compare-words") {
            StructureA fixed;
            return compareWordCounts(fixed, table, filename);
        }
        if (mode == "--words") {
            return runWordFile(table, filename, cout);
        }
        return runCommands(table);
    }

    if (choice == "B") {
        LinearProbingTable table(16, 31, useSeed, seed);
        if (mode == "--compare-words") {
            LinearProbingTable fixed;
            return compareWordCounts(fixed, table, filename);
        }
        if (mode == "--words") {
            return runWordFile(table, filename, cout);
        }
        return runCommands(table);
    }

    return 1;
}

