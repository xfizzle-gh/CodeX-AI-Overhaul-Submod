from pathlib import Path
import re

text = Path("tests/test_attack_support_slot_proof.py").read_text(encoding="utf-8-sig")
methods = re.findall(r"(?m)^[ \t]+def (test_[A-Za-z0-9_]+)\(", text)
for name in methods:
    if any(token in name for token in ("motor", "flank", "linked", "order", "drive", "emit")):
        print(name)
