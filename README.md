Build with C++11 or newer. From the repository root in PowerShell:

```powershell
g++ -std=c++11 -Wall -Wextra app/main.cpp src/structA/StructureA.cpp -o app.exe
Get-Content tests/application_commands.in | .\app.exe --structure A
Get-Content tests/application_commands.in | .\app.exe --structure B
.\app.exe --structure A --words tests/application_words.in
.\app.exe --structure B --words tests/application_words.in
```

The first two runs should match `tests/application_commands.expected`. The last two should match `tests/application_words.expected`. Structure B is implemented in its header. Its standalone driver has a separate `main()`.

In command mode, `I` inserts or updates, `G` prints a value or `NOT_FOUND`, `D` prints `NOT_FOUND` when the key is absent, and `N` prints the number of live keys.

In word mode, each run starts with a fresh table. A word consists of ASCII letters. The program converts capitals to lowercase and treats punctuation, digits, spaces, and other bytes as separators. It prints one `word count` line per distinct word in first-appearance order. Replace the sample filename with your corpus file and use the same file for both structures. Counts are stored in the selected hash table. The extra vector records only the order of distinct words.
