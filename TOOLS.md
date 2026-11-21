# References and Tool Assistance

This document records the references, software and AI assistance used for Project 2: Hash Tables and How to Break Them. It covers the project as a whole.

## Academic References

- Course lecture notes on hashing by Subhra Mazumdar Ma'am.
- Cormen, T. H., Leiserson, C. E., Rivest, R. L., and Stein, C. *Introduction to Algorithms*, 4th edition, MIT Press, 2022. Chapter 11: Hash Tables.
- Carter, J. L., and Wegman, M. N. "Universal Classes of Hash Functions." *Journal of Computer and System Sciences*, 18(2), 143-154, 1979.
- Crosby, S. A., and Wallach, D. S. "Denial of Service via Algorithmic Complexity Attacks." *USENIX Security Symposium*, 2003.

## Video References

The following video titles were recorded as learning references:

- Hash Tables: Theory, Implementation, and Universal Hashing.
- Hashing Tutorial 4 - Universal Hashing.
- Rolling Hash Function Tutorial (Rabin-Karp).

Exact video URLs were not recorded. Generic links to the YouTube homepage have therefore been omitted.

## Text Data

- Project Gutenberg texts of Jane Austen's *Emma*, *Persuasion*, and *Sense and Sensibility* were combined into `data/corpus.txt` for the word frequency application and corpus measurements.
- These texts are input data, not project source code.

## Software Tools

- **C++:** Implementation of the hash tables, command interface, word frequency counter, attack replay and test programs.
- **MSVC:** Local compilation and execution of the C++ programs during verification and measurement work.
- **Python and Matplotlib:** Processing measurement data and generating report figures.
- **Git and GitHub:** Version control, individual development branches, integration and storage of project files.
- **Canva and presentation-generation tools:** Assistance with presentation layout, diagrams, formatting and export.

## AI Assistance

AI tools were used to help write and revise some parts of the code, find bugs, review edge cases, explain concepts and prepare documentation. The uses below include generated code and scripts as well as review assistance.

### Google Gemini

- Clarified C++ syntax, header/source separation and file input/output redirection.
- Assisted with reviewing edge-case test designs, including tombstone probe-chain preservation and base-31 block-substitution collision strings.
- Assisted with formatting report documentation.

### Claude

- Assisted with planning the seeded-hashing analysis.
- Introduced tabulation hashing as a possible design for discussion. The experiment used the team's implemented position-weighted seeded hash.

### OpenAI ChatGPT / Codex

- Generated and revised the five-day project guide and explanations of hashing, application logic and analysis questions.
- Generated and revised application code for command processing, word frequency counting, fixed/seeded comparisons and attack replay.
- Suggested and provided corrections to the seeded hashing implementation in Structure B.
- Inspected shared implementations and assisted with integration, compilation, testing and identifying errors.
- Generated and adapted measurement and plotting scripts for search probes, corpus timings, attack replay and seeded-hashing analysis.
- Drafted and revised report sections, build/run instructions, tool disclosures and presentation content.
- Assisted with presentation diagrams, charts, layout and PDF/PowerPoint export.

## Analysis Questions Covered

References and tool-assisted explanations supported understanding and presentation of the five analysis questions:

- **Q1 - Separate chaining:** Expected probes for successful and unsuccessful searches, the effect of load factor, and comparison of theoretical estimates with measured probes.
- **Q2 - Linear probing:** The standard successful- and unsuccessful-search formulas, comparison with measured probes, and explanations of differences such as clustering and the assumptions behind the formulas.
- **Q3 - Tombstones:** Why deletion must preserve a probe chain, with a small example showing how marking a deleted slot as empty can make a later key unreachable.
- **Q4 - Collision construction:** Why replacing equal-length blocks with the same polynomial hash preserves the full string's hash, and how choosing between two such blocks at each of t positions produces 2^t distinct colliding keys.
- **Q5 - Seeded hashing:** How choosing a hash function from a universal family prevents a workload constructed for a known fixed hash from reliably causing the same collisions, and how the expected full-chain length is approximately 1 + alpha when the keys are independent of the randomly chosen hash function.

These topics were connected to the table implementations, test cases, measurements and report explanations. The word frequency counter and attack replay were also explained and checked as application features.

## Changes to Generated Material

Generated material was revised during integration, including through further AI-assisted edits:

- Application calls were adapted to the actual interfaces of Structure A and Structure B.
- Code and explanations were simplified, while preserving the required application behaviour.
- Seeded hashing changes were adapted to the existing Structure B implementation.
- Word counting was updated to use a large text file containing several Project Gutenberg books.
- Attack replay was adapted to use saved collision workloads, fresh tables, five repetitions per hash mode, recorded seeds and CSV timing output.
- Replay verification was extended to compare query answers, the number of live keys and the final presence and value of every mentioned key.
- Measurement drivers were adapted to the team's tables and the configurable load threshold in Structure B.
- Documentation and presentation material were revised to match the project structure, results and presentation requirements.

## Verification

- The test programs `oracle.cpp`, `differential_test.cpp`, `invariant_checker.cpp` and `collision_generator.cpp` were compiled and executed locally.
- Local checks included application fixtures, word-count comparisons, collision-workload replay, table invariants and large ordinary-key inputs.
- Probe and corpus measurement modes were checked using an MSVC build during their development.
- Reported timings depend on the workload, machine, compiler settings and recorded seed. Raw measurements and figures are stored in `results/`.

AI assistance was used for explanations, generated material, review and revision as described above. This disclosure does not claim that assisted code or documentation was written entirely without tool support.

## Submission fixes

The earlier assembled report credits Claude with drafting sections and interpreting archive results. Codex integrated the current constructors, shared the hash functions, added Structure A tests and million-key fixtures, verified the final build, recollected measurements, and updated the documentation and PDF exports. The preserved four-way replay CSVs came from the team.
