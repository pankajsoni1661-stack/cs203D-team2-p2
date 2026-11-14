// Helper function to create a random string of length 1 to 25
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

int main() {
    const int FIXED_SEED = 42;
    const int NUM_OPS = 100000; // 10^5 random operations
    const int KEY_POOL = 500;   // 500 distinct random strings

    mt19937 rng(FIXED_SEED);

    // Step 1: Build a pool of 500 varied random keys (lengths 1 to 25)
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

    // Step 2: Run 100,000 operations picking from our 500 random strings
    for (int i = 0; i < NUM_OPS; i++) {
        int choice = opDist(rng);
        string key = pool[poolIdxDist(rng)];

        if (choice <= 35) {
            int val = valDist(rng);
            opFile << "I " << key << " " << val << "\n";
            oracle.insert(key, val);
        } 
        else if (choice <= 70) {
            opFile << "D " << key << "\n";
            oracle.remove(key);
        } 
        else if (choice <= 95) {
            opFile << "G " << key << "\n";
            ansFile << oracle.get(key) << "\n";
        } 
        else {
            opFile << "N\n";
            ansFile << oracle.count() << "\n";
        }
    }

    opFile.close();
    ansFile.close();

    cout << "Generated " << NUM_OPS << " operations with varied random keys (seed=" << FIXED_SEED << ").\n";
    return 0;
}
