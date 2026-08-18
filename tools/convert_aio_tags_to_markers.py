from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BREED_ROOT = ROOT / "resource/set/breed"
STUFF_DIR = ROOT / "resource/set/stuff/special"

TOKEN_TO_ITEM = {
    "aio_morale_low": "aio_marker_morale_low",
    "aio_morale_regular": "aio_marker_morale_regular",
    "aio_morale_trained": "aio_marker_morale_trained",
    "aio_morale_elite": "aio_marker_morale_elite",
    "aio_cmd_junior": "aio_marker_cmd_junior",
    "aio_cmd_primary": "aio_marker_cmd_primary",
    "aio_cmd_senior": "aio_marker_cmd_senior",
    "aio_cmd_independent": "aio_marker_cmd_independent",
    "aio_discipline": "aio_marker_discipline",
    "aio_steadfast": "aio_marker_steadfast",
}
TAGS_RE = re.compile(r'(\{tags\s+")([^"]*)("\})')
AIO_RE = re.compile(r"^aio_")
STUFF_BODY = """{item
	{tag "itemin1hand doc_bag"}
	{entity "secret_doc_bag2"}
	{inventory
		{size 1 1}
		{weight 1}
		{fsm "stuff"}
	}
	{mass 1}
}
"""


def write_stuff_files() -> None:
    STUFF_DIR.mkdir(parents=True, exist_ok=True)
    for item in TOKEN_TO_ITEM.values():
        (STUFF_DIR / item).write_text(STUFF_BODY, encoding="utf-8")


def convert_text(text: str) -> str:
    nl = "\r\n" if "\r\n" in text else "\n"
    match = TAGS_RE.search(text)
    if not match:
        if "aio_marker_" in text or not AIO_RE.search(text):
            return text
        raise ValueError("missing tags")
    tokens = match.group(2).split()
    aio = [token for token in tokens if token in TOKEN_TO_ITEM]
    kept = [token for token in tokens if not AIO_RE.match(token)]
    unknown = [token for token in tokens if AIO_RE.match(token) and token not in TOKEN_TO_ITEM]
    if unknown:
        raise ValueError(f"unknown aio tokens: {unknown}")
    if not aio:
        return text
    if kept:
        text = text[: match.start()] + match.group(1) + " ".join(kept) + match.group(3) + text[match.end() :]
    else:
        line_start = text.rfind(nl, 0, match.start())
        if line_start == -1:
            line_start = 0
        else:
            line_start += len(nl)
        line_end = text.find(nl, match.end())
        if line_end == -1:
            line_end = len(text)
        else:
            line_end += len(nl)
        text = text[:line_start] + text[line_end:]
    items = [f'\t\t{{item "{TOKEN_TO_ITEM[token]}"}}' for token in aio]
    block = nl.join(items) + nl
    hands = re.search(r"[ \t]*\{in_hands ", text)
    if hands:
        return text[: hands.start()] + block + text[hands.start() :]
    close = text.rfind("\t}")
    if close == -1:
        raise ValueError("missing inventory close")
    return text[:close] + block + text[close:]


def main() -> None:
    write_stuff_files()
    changed = 0
    for path in sorted(BREED_ROOT.rglob("*.set")):
        original = path.read_bytes().decode("utf-8")
        try:
            updated = convert_text(original)
        except ValueError as exc:
            raise ValueError(f"{path}: {exc}") from exc
        if updated != original:
            path.write_bytes(updated.encode("utf-8"))
            changed += 1
    print(f"converted {changed} breeds")


if __name__ == "__main__":
    main()
