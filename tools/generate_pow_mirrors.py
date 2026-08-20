#!/usr/bin/env python3
from __future__ import annotations

import argparse
import posixpath
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BREED_ROOT = ROOT / "resource/set/breed"
GENERATED_DIR = "generated_pow"
ISO_DIR = "isolation_test"
MAPPING = ROOT / "docs/pow_mirror_mapping.tsv"
SKIP = ROOT / "docs/pow_mirror_skip.tsv"
WRITE_RELS = frozenset({"mp/nato/2022s/nato_rifleman.set"})

BEHAVIOUR_RE = re.compile(r"\{behaviour\s+\"?(\w+)\"?\}")
INCLUDE_RE = re.compile(r'\(include\s+"([^"]+)"\)')
SKIN_RE = re.compile(r"\{skin\b")
BREED_RE = re.compile(r"^\s*\{breed\b")
TAGS_RE = re.compile(r'(\{tags\s+")([^"]*)("\})')
COMBAT_RE = re.compile(
    r"\b(filled|filling|weapon|ammo|grenade|explosive|dynamite)\b|\bc4",
    re.I,
)
MINE_RE = re.compile(r"\bmines?\b", re.I)
DIAG_RE = re.compile(r"aio_marker_|secret_doc", re.I)


def detect_nl(text: str) -> str:
    return "\r\n" if "\r\n" in text else "\n"


def brace_delta(text: str) -> int:
    return text.count("{") - text.count("}")


def find_block(text: str, key: str) -> tuple[int, int] | None:
    token = "{" + key
    start = None
    idx = 0
    while True:
        found = text.find(token, idx)
        if found == -1:
            break
        end_token = found + len(token)
        if end_token < len(text) and (text[end_token].isalnum() or text[end_token] == "_"):
            idx = end_token
            continue
        if start is not None:
            raise ValueError(f"multiple {key} blocks")
        start = found
        idx = end_token
    if start is None:
        return None
    depth = 0
    for i, ch in enumerate(text[start:], start):
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                return start, i + 1
    raise ValueError(f"unclosed {key} block")


def iter_set_files(root: Path) -> list[Path]:
    return sorted(root.rglob("*.set"), key=lambda path: path.relative_to(root).as_posix())


def rel_posix(path: Path) -> str:
    return path.relative_to(BREED_ROOT).as_posix()


def excluded_subtree(rel: str) -> bool:
    parts = Path(rel).parts
    return GENERATED_DIR in parts


def classify(rel: str, text: str, *, allow_iso: bool = False) -> tuple[bool, str]:
    parts = Path(rel).parts
    if GENERATED_DIR in parts:
        return False, "generated_subtree"
    if ISO_DIR in parts and not allow_iso:
        return False, "isolation_test"
    if brace_delta(text) != 0:
        return False, "unbalanced_braces"
    if not BREED_RE.search(text):
        return False, "not_breed"
    if SKIN_RE.search(text) is None:
        return False, "no_skin"
    behaviours = BEHAVIOUR_RE.findall(text)
    if not behaviours:
        return False, "no_behaviour"
    if len(behaviours) != 1:
        return False, "multiple_behaviour"
    if behaviours[0] != "soldier":
        return False, "not_soldier"
    try:
        find_block(text, "inventory")
    except ValueError as exc:
        return False, str(exc).replace(" ", "_")
    return True, "ok"


def rewrite_includes(text: str, source_rel: str) -> str:
    src_dir = Path(source_rel).parent.as_posix()

    def repl(match: re.Match[str]) -> str:
        inc = match.group(1)
        if inc.startswith("/"):
            return match.group(0)
        resolved = posixpath.normpath(f"{src_dir}/{inc}")
        if resolved.startswith("..") or resolved.startswith("/"):
            raise ValueError(f"include escapes breed root: {inc}")
        return f'(include "/set/breed/{resolved}")'

    text = INCLUDE_RE.sub(repl, text)
    leftover = re.search(r'\(include\s+"[^/]', text)
    if leftover:
        raise ValueError("relative include remained")
    return text


def is_combat_item(line: str) -> bool:
    if "mine_detector" in line.lower():
        return False
    return COMBAT_RE.search(line) is not None or MINE_RE.search(line) is not None


def is_diagnostic_item(line: str) -> bool:
    return DIAG_RE.search(line) is not None


def strip_soldier_tag(text: str) -> str:
    def repl(match: re.Match[str]) -> str:
        kept = [token for token in match.group(2).split() if token != "soldier"]
        if kept:
            return match.group(1) + " ".join(kept) + match.group(3)
        return ""

    return TAGS_RE.sub(repl, text)


def disarm_inventory(text: str) -> str:
    block = find_block(text, "inventory")
    if block is None:
        return text
    start, end = block
    inner = text[start:end]
    nl = detect_nl(text)
    kept: list[str] = []
    for line in inner.split(nl):
        stripped = line.strip()
        if stripped.startswith("{item"):
            if is_diagnostic_item(stripped) or is_combat_item(stripped):
                continue
            kept.append(line)
        elif stripped.startswith("{in_hands"):
            kept.append(line)
    if not kept:
        indent = "\t"
        kept = [f"{indent}\t{{in_hands 0}}"]
    rebuilt = "{inventory" + nl + nl.join(kept) + nl + "\t}"
    return text[:start] + rebuilt + text[end:]


def transform(rel: str, text: str) -> str:
    ok, reason = classify(rel, text, allow_iso=True)
    if not ok:
        raise ValueError(reason)
    text = rewrite_includes(text, rel)
    text = BEHAVIOUR_RE.sub("{behaviour civilian}", text, count=1)
    text = strip_soldier_tag(text)
    text = disarm_inventory(text)
    if "{behaviour civilian}" not in text:
        raise ValueError("behaviour_not_rewritten")
    if "{behaviour soldier}" in text:
        raise ValueError("soldier_behaviour_remained")
    if re.search(r'\{tags\s+"[^"]*\bsoldier\b', text):
        raise ValueError("soldier_tag_remained")
    inv = find_block(text, "inventory")
    if inv is not None:
        inner = text[inv[0] : inv[1]]
        for line in inner.splitlines():
            if line.strip().startswith("{item") and (
                is_combat_item(line) or is_diagnostic_item(line)
            ):
                raise ValueError("combat_item_remained")
    if brace_delta(text) != 0:
        raise ValueError("unbalanced_after_transform")
    return text


def mirror_rel(source_rel: str) -> str:
    return f"{GENERATED_DIR}/{source_rel}"


def collect(root: Path | None = None) -> tuple[list[tuple[str, str, str]], list[tuple[str, str]]]:
    breed_root = root or BREED_ROOT
    mapped: list[tuple[str, str, str]] = []
    skipped: list[tuple[str, str]] = []
    for path in iter_set_files(breed_root):
        rel = path.relative_to(breed_root).as_posix()
        if excluded_subtree(rel):
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        ok, reason = classify(rel, text)
        if ok:
            mapped.append((rel, mirror_rel(rel), "production"))
        elif reason in {"isolation_test", "generated_subtree"}:
            continue
        else:
            skipped.append((rel, reason))
    return mapped, skipped


def render_mapping(rows: list[tuple[str, str, str]]) -> str:
    lines = ["source\tmirror\tscope"]
    lines.extend(f"{source}\t{mirror}\t{scope}" for source, mirror, scope in rows)
    return "\n".join(lines) + "\n"


def render_skip(rows: list[tuple[str, str]]) -> str:
    lines = ["source\treason"]
    lines.extend(f"{source}\t{reason}" for source, reason in rows)
    return "\n".join(lines) + "\n"


def write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8", newline="\n")


def write_mirrors(rows: list[tuple[str, str, str]], *, only_committed: bool) -> list[Path]:
    written: list[Path] = []
    for source, mirror, scope in rows:
        if only_committed and source not in WRITE_RELS:
            continue
        src = BREED_ROOT / source
        text = transform(source, src.read_text(encoding="utf-8", errors="replace"))
        dest = BREED_ROOT / mirror
        dest.parent.mkdir(parents=True, exist_ok=True)
        nl = detect_nl(text)
        if not text.endswith(nl):
            text += nl
        dest.write_bytes(text.encode("utf-8"))
        written.append(dest)
    return written


def check() -> int:
    mapped, skipped = collect()
    mapping_text = render_mapping(mapped)
    skip_text = render_skip(skipped)
    errors: list[str] = []
    if not MAPPING.is_file() or MAPPING.read_text(encoding="utf-8") != mapping_text:
        errors.append("stale mapping tsv")
    if not SKIP.is_file() or SKIP.read_text(encoding="utf-8") != skip_text:
        errors.append("stale skip tsv")
    for source, mirror, scope in mapped:
        dest = BREED_ROOT / mirror
        if source in WRITE_RELS and not dest.is_file():
            errors.append(f"missing committed mirror {mirror}")
            continue
        if not dest.is_file():
            continue
        expected = transform(source, (BREED_ROOT / source).read_text(encoding="utf-8", errors="replace"))
        actual = dest.read_text(encoding="utf-8", errors="replace")
        exp_nl = detect_nl(expected)
        if not expected.endswith(exp_nl):
            expected += exp_nl
        if actual != expected:
            errors.append(f"stale mirror {mirror}")
    extra = [
        path
        for path in (BREED_ROOT / GENERATED_DIR).rglob("*.set")
        if path.relative_to(BREED_ROOT).as_posix() not in {mirror for _, mirror, _ in mapped}
    ]
    errors.extend(f"unexpected mirror {path.relative_to(BREED_ROOT).as_posix()}" for path in extra)
    if errors:
        print("\n".join(errors), file=sys.stderr)
        return 1
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--write-all", action="store_true")
    args = parser.parse_args(argv)
    if args.check:
        return check()
    mapped, skipped = collect()
    write_text(MAPPING, render_mapping(mapped))
    write_text(SKIP, render_skip(skipped))
    written = write_mirrors(mapped, only_committed=not args.write_all)
    print(f"mapped {len(mapped)} skipped {len(skipped)} wrote {len(written)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
