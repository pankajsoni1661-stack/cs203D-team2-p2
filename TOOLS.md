# Tools and assistance

## Adwit Tiwari · Measurements and Q5

- The five-day team guide and a Claude conversation helped plan the experiments and introduced tabulation hashing as a candidate design.
- Codex inspected the team's C++ tables, adapted the probe and corpus benchmark to their interfaces, wrote and ran the Q5 experiment, and drafted the measurement report and Q5 slide using the recorded results.
- Codex collected the local seeded replay timings. For the final four-way comparison, it checked the six supplied CSVs, recomputed their means and plotted the fixed and seeded runs from those files. The supplied CSVs do not identify their machine or compiler.
- The local measurements used Microsoft Visual C++ with x64 optimization on Windows. Python and Matplotlib processed CSV data and rendered the figures. Git and GitHub recorded the measurement code, data and report revisions. [Measurement snapshot](https://github.com/pankajsoni1661-stack/cs203D-team2-p2/tree/69e5f7e)

## Repository documentation

Codex edited the main-branch README and compiled the application with MSVC to check its usage examples. This disclosure records the assistance documented for Adwit's work and the repository documentation. Other contributors can add their own tool disclosures here.
