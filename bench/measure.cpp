#include "../src/structA/StructureA.h"
#include "../src/structB/LinearProbing.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using namespace std;
using Clock = chrono::steady_clock;
constexpr int runs = 5;
constexpr int probeCapacity = 4096;

void require(bool ok, const string& message) {
    if (!ok) throw runtime_error(message);
}

int live(const StructureA& t) { return t.count(); }
int live(LinearProbingTable& t) { return t.size(); }
size_t capacity(const StructureA& t) { return t.bucketCount(); }
size_t capacity(const LinearProbingTable& t) { return t.capacity; }

bool lookup(StructureA& t, const string& key, int& value, size_t& work) {
    return t.get(key, value, work);
}
bool lookup(LinearProbingTable& t, const string& key, int& value, size_t& work) {
    auto start = t.probes;
    bool found = t.get(key, value);
    work = static_cast<size_t>(t.probes - start);
    return found;
}

template<class F>
void eachTable(int initial, bool controlled, const SeededHash* hash, F action) {
    {
        StructureA a(hash, initial);
        action("A", a);
    }
    LinearProbingTable b(initial, 31, hash);
    if (controlled) b.max_load_factor = 0.9;
    action("B", b);
}

void header(ostream& out) {
    out << "experiment,structure,hash,seed,run,keys,key_len,capacity,live,alpha,"
           "search_type,queries,total_probes,avg_probes,seconds\n" << setprecision(12);
}

string randomKey(mt19937_64& rng, size_t length) {
    string key(length, ' ');
    for (auto& c : key) c = static_cast<char>(33 + rng() % 94);
    return key;
}

void probes(ostream& out) {
    for (int run = 1; run <= runs; ++run) {
        SeededHash hash(1000 + run);
        for (int step = 1; step <= 9; ++step) {
            mt19937_64 rng(31000 + 10 * run + step);
            const int n = probeCapacity * step / 10;
            vector<string> keys(n), absent(n);
            for (auto& k : keys) k = randomKey(rng, 12);
            for (auto& k : absent) k = randomKey(rng, 13);
            for (bool seeded : {false, true}) {
                eachTable(probeCapacity, true, seeded ? &hash : nullptr, [&](const char* st, auto& t) {
                    for (int i = 0; i < n; ++i) t.insert(keys[i], i);
                    require(live(t) == n && capacity(t) == probeCapacity, "probe setup mismatch");
                    for (bool hit : {true, false}) {
                        size_t total = 0;
                        for (int i = 0; i < n; ++i) {
                            size_t work = 0;
                            int value = -1;
                            bool found = lookup(t, hit ? keys[i] : absent[i], value, work);
                            require(found == hit && (!hit || value == i), "probe lookup mismatch");
                            total += work;
                        }
                        out << "probes," << st << ',' << (seeded ? "seeded" : "fixed") << ',';
                        if (seeded) out << 1000 + run;
                        out << ',' << run << ',' << n << ",12," << capacity(t) << ',' << live(t)
                            << ',' << double(n) / capacity(t) << ',' << (hit ? "successful" : "unsuccessful")
                            << ',' << n << ',' << total << ',' << double(total) / n << ",\n";
                    }
                });
            }
        }
    }
}

vector<string> readWords(const string& path) {
    ifstream in(path, ios::binary);
    require(in.is_open(), "cannot open corpus");
    vector<string> words;
    string word;
    char ch;
    while (in.get(ch)) {
        if (ch >= 'A' && ch <= 'Z') ch = char(ch - 'A' + 'a');
        if (ch >= 'a' && ch <= 'z') word += ch;
        else if (!word.empty()) { words.push_back(word); word.clear(); }
    }
    require(in.eof() && !in.bad(), "corpus read failed");
    if (!word.empty()) words.push_back(word);
    require(!words.empty(), "empty corpus");
    return words;
}

void timeCorpus(ostream& out, const vector<string>& words) {
    map<string, int> expected;
    for (const auto& word : words) ++expected[word];
    for (int run = 1; run <= runs; ++run) {
        SeededHash hash(5000 + run);
        const bool seededFirst = run % 2 == 0;
        for (bool seeded : {seededFirst, !seededFirst}) {
            eachTable(16, false, seeded ? &hash : nullptr, [&](const char* st, auto& table) {
                auto start = Clock::now();
                for (const auto& word : words) {
                    int count = 0;
                    table.get(word, count);
                    table.insert(word, count + 1);
                }
                double seconds = chrono::duration<double>(Clock::now() - start).count();
                require(size_t(live(table)) == expected.size(), "distinct word count mismatch");
                for (const auto& entry : expected) {
                    int count = 0;
                    require(table.get(entry.first, count) && count == entry.second, "word count mismatch");
                }
                out << "corpus," << st << ',' << (seeded ? "seeded" : "fixed") << ',';
                if (seeded) out << 5000 + run;
                out << ',' << run << ',' << words.size() << ",," << capacity(table) << ','
                    << live(table) << ',' << double(live(table)) / capacity(table)
                    << ",,,,," << seconds << '\n';
            });
        }
    }
}
} // namespace

int main(int argc, char** argv) {
    try {
        if (argc == 2 && string(argv[1]) == "probes") {
            header(cout);
            probes(cout);
        } else if (argc == 3 && string(argv[1]) == "corpus") {
            auto words = readWords(argv[2]);
            header(cout);
            timeCorpus(cout, words);
        } else {
            cerr << "Usage: measure probes | measure corpus CORPUS_FILE\n";
            return 2;
        }
        cout.flush();
        return cout ? 0 : 1;
    } catch (const exception& e) {
        cerr << e.what() << '\n';
        return 1;
    }
}
