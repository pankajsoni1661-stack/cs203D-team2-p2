"""Generate the report's deterministic million-key workload and answer stream."""
import random
from pathlib import Path

def generate():
    rng = random.Random(20261121)
    values = {}
    while len(values) < 1_000_000:
        key = "".join(chr(rng.randrange(33, 127)) for _ in range(rng.randint(8, 16)))
        values.setdefault(key, len(values))
    keys = list(values)
    with Path("scale_1m.in").open("w", encoding="ascii") as ops, Path("scale_1m.expected").open("w", encoding="ascii") as answers:
        def count():
            print("N", file=ops)
            print(len(values), file=answers)

        for key, value in values.items():
            print("I", key, value, file=ops)
        count()
        rng.shuffle(keys)
        for key in keys:
            print("G", key, file=ops)
            print(values[key], file=answers)
        for key in keys[::2]:
            print("D", key, file=ops)
            del values[key]
        count()
        for key in keys:
            print("G", key, file=ops)
            print(values.get(key, "NOT_FOUND"), file=answers)
        for key in keys[:2000:2]:
            print("D", key, file=ops)
            print("NOT_FOUND", file=answers)
        count()

if __name__ == "__main__":
    generate()
