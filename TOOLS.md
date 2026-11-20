
2. Video Tutorials & Conceptual Walkthroughs

• Hash Tables: Theory, Implementation, and Universal Hashing — Overview of load factors, collision resolution, and universal hashing guarantees.
• Hashing Tutorial 4 - Universal Hashing — Mathematical derivation showing why expected bucket collisions stay bounded at 1 + α under universal hash families.
• Rolling Hash Function Tutorial (Rabin-Karp) — Explanation of polynomial rolling hash mechanics (h = h ⋅ b + c) and why character position weights matter.
• Collision in Hash Table | Chaining vs. Open Addressing — Visual comparison of Separate Chaining and Linear Probing probe sequences.
• Property-Based & Differential Testing Concepts — Introduction to randomized operation generation and invariant verification across large test suites.

3. AI Tools Used (Disclosure)

• Google Gemini: Used during development to clarify C++ syntax (function pointers, header/source separation, and file I/O redirection), review edge-case test designs (tombstone probe-chain preservation and base-31 block-substitution collision strings), and structure the testing documentation. All source code (oracle.cpp, differential_test.cpp, invariant_checker.cpp, collision_generator.cpp) was compiled, executed, and verified locally by the team.

