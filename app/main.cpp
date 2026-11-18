#include <fstream>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
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

    return input.bad() || !output ? 1 : 0;
}

template <typename Table>
int runWordCounter(Table& table, istream& input, ostream& output) {
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
        output << key << ' ' << count << '\n';
    }
    return output ? 0 : 1;
}

template <typename Table>
int runWordFile(Table& table, const char* filename, ostream& output) {
    ifstream input(filename, ios::binary);
    if (!input) {
        cerr << "Could not open " << filename << '\n';
        return 1;
    }
    return runWordCounter(table, input, output);
}

template <typename Table>
int compareWordCounts(Table& fixed, Table& seeded, const char* filename) {
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
    return cout ? 0 : 1;
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
                  int run, const string& structure, uint64_t seed,
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

int runAttack(const string& structure, uint64_t seed, const char* filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Could not open " << filename << '\n';
        return 1;
    }
    ostringstream buffer;
    buffer << file.rdbuf();
    if (file.bad() || !buffer) return 1;
    const string operations = buffer.str();
    if (operations.find_first_not_of(" \t\r\n") == string::npos) {
        cerr << "The attack input is empty.\n";
        return 1;
    }

    double fixedTotal = 0, seededTotal = 0;
    ostringstream rows;
    rows << fixed << setprecision(6);
    rows << "run,structure,hash_mode,seed,replay_ms,final_keys,verification\n";
    for (int run = 1; run <= 5; ++run) {
        SeededHash hash(seed);
        int status;
        if (structure == "A") {
            StructureA fixedTable;
            StructureA seededTable(&hash);
            status = compareReplay(fixedTable, seededTable, operations,
                                   run, structure, seed, rows, fixedTotal, seededTotal);
        } else {
            LinearProbingTable fixedTable;
            LinearProbingTable seededTable(16, 31, &hash);
            status = compareReplay(fixedTable, seededTable, operations,
                                   run, structure, seed, rows, fixedTotal, seededTotal);
        }
        if (status != 0) return status;
    }
    rows << "mean," << structure << ",fixed,NA," << fixedTotal / 5 << ",NA,PASS\n";
    rows << "mean," << structure << ",seeded," << seed << ','
         << seededTotal / 5 << ",NA,PASS\n";
    cout << rows.str();
    return cout ? 0 : 1;
}

int main(int argc, char* argv[]) {
    if (argc < 3 || string(argv[1]) != "--structure") return 1;
    string choice = argv[2];
    if (choice != "A" && choice != "B") return 1;

    int next = 3;
    bool useSeed = false;
    uint64_t seed = 0;
    if (next < argc && string(argv[next]) == "--seed") {
        if (next + 1 >= argc) return 1;
        string seedText = argv[next + 1];
        if (seedText.empty() || seedText.find_first_not_of("0123456789") != string::npos) {
            return 1;
        }
        istringstream seedInput(seedText);
        if (!(seedInput >> seed)) return 1;
        useSeed = true;
        next += 2;
    }

    const char* filename = nullptr;
    bool compareWords = false;
    bool attack = false;
    if (next < argc) {
        if (argc - next != 2) return 1;
        string option = argv[next];
        compareWords = option == "--compare-words";
        attack = option == "--attack";
        if (option != "--words" && !compareWords && !attack) return 1;
        if ((compareWords || attack) && !useSeed) {
            cerr << "Pass --seed before the comparison option.\n";
            return 1;
        }
        filename = argv[next + 1];
    }

    if (attack) return runAttack(choice, seed, filename);
    SeededHash seededHash(seed);
    const SeededHash* hash = useSeed ? &seededHash : nullptr;

    if (choice == "A") {
        StructureA table(hash);
        if (compareWords) {
            StructureA fixed;
            return compareWordCounts(fixed, table, filename);
        }
        return filename ? runWordFile(table, filename, cout) : runCommands(table);
    }

    if (choice == "B") {
        LinearProbingTable table(16, 31, hash);
        if (compareWords) {
            LinearProbingTable fixed;
            return compareWordCounts(fixed, table, filename);
        }
        return filename ? runWordFile(table, filename, cout) : runCommands(table);
    }

    return 1;
}

