"""From the repo root: python tests/run_suite.py [--bench] [--scale] [--msvc]."""
import argparse
import filecmp
from pathlib import Path
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for option in ("bench", "scale", "msvc"):
        parser.add_argument("--" + option, action="store_true")
    args = parser.parse_args()
    with tempfile.TemporaryDirectory() as tmp:
        build = Path(tmp)
        (build / "tests").mkdir()
        def compile_program(name, *sources):
            exe = build / (name + ".exe")
            paths = [str(ROOT / source) for source in sources]
            flags = (["cl", "/nologo", "/EHsc", "/std:c++17", "/O2", "/I" + str(ROOT), "/Fe:" + str(exe)]
                     if args.msvc else ["g++", "-std=c++17", "-O2", "-I", str(ROOT), "-o", str(exe)])
            subprocess.run(flags + paths, cwd=build, check=True)
            return exe

        def compare(command, input_path, expected):
            actual = build / "actual.txt"
            with input_path.open("rb") as inp, actual.open("wb") as out:
                subprocess.run(command, stdin=inp, stdout=out, check=True)
            if not filecmp.cmp(actual, expected, shallow=False):
                with actual.open() as a, expected.open() as b:
                    from itertools import zip_longest
                    if any(x != y for x, y in zip_longest(a, b)):
                        raise RuntimeError(f"Output mismatch for {command} on {input_path}")

        shared = "src/structA/StructureA.cpp"
        app = compile_program("run", "app/main.cpp", shared)
        for name in ("test_StructA", "test_StructB", "invariant_checker"):
            subprocess.run([compile_program(name, "tests/" + name + ".cpp", shared)], check=True)
        generator = compile_program("generate", "tests/differential_test.cpp")
        subprocess.run([generator], cwd=build, check=True)
        oracle = compile_program("oracle", "tests/oracle.cpp")
        ops = build / "tests/random_ops.txt"
        answers = build / "tests/oracle_out.txt"
        compare([oracle], ops, answers)
        if args.scale:
            subprocess.run([sys.executable, ROOT / "tests/scale_test_gen.py"], cwd=build, check=True)
        for st in "AB":
            for mode in ([], ["--seed", "42"]):
                command = [app, "--structure", st, *mode]
                for fixture in sorted((ROOT / "Application_tests").glob("*.in")):
                    words = ["--words", fixture] if fixture.stem.endswith("words") else []
                    compare(command + words, fixture, fixture.with_suffix(".expected"))
                compare(command, ops, answers)
                compare(command, ROOT / "tests/demo_1000000.in", ROOT / "tests/demo_1000000.expected")
                if args.scale:
                    compare(command, build / "scale_1m.in", build / "scale_1m.expected")
        print("All correctness checks passed.", flush=True)
        if args.bench:
            measure = compile_program("measure", "bench/measure.cpp", shared)
            q5 = compile_program("q5", "bench/q5_chain_length.cpp", shared)
            for name, command in [("probes", [measure, "probes"]), ("corpus", [measure, "corpus", ROOT / "data/corpus.txt"]),
                                  ("q5_chain_length", [q5])]:
                output = build / (name + ".csv")
                with output.open("wb") as out:
                    subprocess.run(command, stdout=out, check=True)
                import shutil
                shutil.copyfile(output, ROOT / "results" / output.name)
            for name in ("plot_measurements", "plot_q5", "plot_replay", "plot_seeded_replay"):
                subprocess.run([sys.executable, ROOT / "bench" / (name + ".py")], check=True)

if __name__ == "__main__":
    main()
