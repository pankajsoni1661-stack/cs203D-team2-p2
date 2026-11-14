# cs203D-team2-p2
Use a C++11-compatible GCC compiler. The commands below are for PowerShell on Windows. Run them from the repository root.

```powershell
g++ -std=c++11 -Wall -Wextra app/main.cpp src/structA/StructureA.cpp -o app.exe
Get-Content tests/application_commands.in | .\app.exe --structure A
Get-Content tests/application_commands.in | .\app.exe --structure B
```

Structure B is implemented in `src/structB/LinearProbing.hpp`. Its separate driver, `Structure_B.cpp`, has its own `main()` and is built separately.

Both runs should print the lines in `tests/application_commands.expected`. The application reads commands from standard input. `I key value` inserts or updates, `G key` prints the value or `NOT_FOUND`, `D key` prints `NOT_FOUND` only if the key is absent, and `N` prints the current key count.

Also run the two commands with `tests/application_edge_cases.in` and `tests/application_resize.in` in place of `tests/application_commands.in`. Compare each run with the corresponding `.expected` file. The resize test inserts 20 keys and checks that all 20 values remain retrievable with the supplied default capacities.
