from __future__ import annotations

import os
import re
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BREED_ROOT = ROOT / "resource/set/breed"
ISO_SUBTREE = "isolation_test"
REPORT = ROOT / "docs/morale_command_classification.md"
TSV = ROOT / "docs/morale_command_classification.tsv"
PHASE0 = ROOT / "docs/morale_command_phase0_audit.md"
ALLOWLIST = ROOT / "docs/morale_legacy_visual_allowlist.txt"
DEFAULT_CODEX = Path(r"E:\Steam\steamapps\workshop\content\400750\3261086933\resource\set\breed")

MORALE = {
    "aio_morale_low",
    "aio_morale_regular",
    "aio_morale_trained",
    "aio_morale_elite",
}
CMD = {
    "aio_cmd_junior",
    "aio_cmd_primary",
    "aio_cmd_senior",
    "aio_cmd_independent",
}
SPECIAL = {"aio_discipline", "aio_steadfast"}
ALLOWED = MORALE | CMD | SPECIAL
RUNTIME_FORBIDDEN = {
    "aio_morale_shaken",
    "aio_morale_panic",
    "aio_morale_broken",
    "aio_cmd_linked",
    "aio_cmd_weak",
    "aio_cmd_lost",
    "aio_cmd_shock",
    "aio_morale_regrouping",
    "aio_morale_surrendering",
    "aio_morale_owned",
}
TAGS_RE = re.compile(r'(\{tags\s+")([^"]*)("\})')
AIO_RE = re.compile(r"^aio_")
ITEM_RE = re.compile(r'\{item "([^"]+)"')
MARKER_RE = re.compile(
    r'\r?\n[ \t]*\{item "(?:aio_morale_marker|secret_doc_bag2|aio_marker_[^"]+)"\}'
)
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
ITEM_TO_TOKEN = {item: token for token, item in TOKEN_TO_ITEM.items()}


def load_legacy_allowlist() -> set[str]:
    lines = ALLOWLIST.read_text(encoding="utf-8").splitlines()
    return {line.strip() for line in lines if line.strip() and not line.startswith("#")}


def aio_tokens(text: str) -> list[str]:
    items = ITEM_RE.findall(text)
    return [ITEM_TO_TOKEN[item] for item in items if item in ITEM_TO_TOKEN]


def strip_aio_tags(text: str) -> str:
    match = TAGS_RE.search(text)
    if not match:
        return re.sub(r'\t\{tags "aio_[^"]*"\}\r?\n', "", text, count=1)
    kept = [token for token in match.group(2).split() if not AIO_RE.match(token)]
    if kept:
        return text[: match.start()] + match.group(1) + " ".join(kept) + match.group(3) + text[match.end() :]
    line_start = text.rfind("\n", 0, match.start()) + 1
    line_end = text.find("\n", match.end())
    if line_end == -1:
        line_end = len(text)
    else:
        line_end += 1
    if match.group(2).split() and all(AIO_RE.match(token) for token in match.group(2).split()):
        return text[:line_start] + text[line_end:]
    return text[: match.start()] + match.group(1) + match.group(3) + text[match.end() :]


def strip_diag_marker(text: str) -> str:
    return MARKER_RE.sub("", text)


class MoraleBreedMetadataTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.sets = sorted(
            path for path in BREED_ROOT.rglob("*.set") if ISO_SUBTREE not in path.parts
        )
        cls.rel = {path.relative_to(BREED_ROOT).as_posix(): path for path in cls.sets}
        cls.legacy = load_legacy_allowlist()

    def test_hidden_marker_definitions_exist(self) -> None:
        stuff = ROOT / "resource/set/stuff/special"
        for item in TOKEN_TO_ITEM.values():
            path = stuff / item
            self.assertTrue(path.is_file(), path)
            text = path.read_text(encoding="utf-8")
            self.assertIn('{entity "secret_doc_bag2"}', text)
            self.assertIn('{fsm "stuff"}', text)
            self.assertIn("{size 1 1}", text)
        self.assertEqual(len(TOKEN_TO_ITEM), 10)

    def test_report_and_tsv_exist(self) -> None:
        self.assertTrue(REPORT.is_file())
        self.assertTrue(TSV.is_file())
        self.assertTrue(PHASE0.is_file())
        text = REPORT.read_text(encoding="utf-8")
        self.assertIn("aio_morale_low", text)
        self.assertIn("Static identity only", text)
        self.assertIn("3604287428", text)
        self.assertIn("3702483522", text)
        self.assertIn("GitHub CI does not prove Code:X freshness", text)
        self.assertIn("Accepted safe-defaults", text)

    def test_legacy_allowlist_is_exact_not_directory_prefix(self) -> None:
        self.assertEqual(len(self.legacy), 79)
        missing = sorted(path for path in self.legacy if path not in self.rel)
        self.assertEqual(missing, [])
        self.assertTrue(all("/" in path and path.endswith(".set") for path in self.legacy))

    def test_isolation_test_subtree_excluded_from_overlay_scan(self) -> None:
        iso = BREED_ROOT / ISO_SUBTREE
        self.assertTrue(iso.is_dir())
        iso_sets = sorted(iso.rglob("*.set"))
        self.assertGreaterEqual(len(iso_sets), 3)
        for path in iso_sets:
            self.assertNotIn(path.relative_to(BREED_ROOT).as_posix(), self.rel)
        self.assertTrue(all(ISO_SUBTREE not in Path(rel).parts for rel in self.rel))

    def test_every_overlay_has_exactly_one_morale_profile(self) -> None:
        self.assertEqual(len(self.sets), 2091)
        for path in self.sets:
            text = path.read_text(encoding="utf-8", errors="replace")
            tokens = aio_tokens(text)
            morale = [token for token in tokens if token in MORALE]
            self.assertEqual(len(morale), 1, path)
            unknown = [token for token in tokens if token not in ALLOWED]
            self.assertEqual(unknown, [], path)
            forbidden = [token for token in tokens if token in RUNTIME_FORBIDDEN]
            self.assertEqual(forbidden, [], path)
            cmd = [token for token in tokens if token in CMD and token != "aio_cmd_independent"]
            self.assertLessEqual(len(cmd), 1, path)
            tags = TAGS_RE.search(text)
            if tags:
                tag_aio = [token for token in tags.group(2).split() if AIO_RE.match(token)]
                self.assertEqual(tag_aio, [], path)

    def test_tsv_covers_every_overlay(self) -> None:
        lines = TSV.read_text(encoding="utf-8").splitlines()
        self.assertGreater(len(lines), 2091)
        paths = {line.split("\t", 1)[0] for line in lines[1:]}
        self.assertEqual(paths, set(self.rel))

    def test_no_dynamic_runtime_states_in_breeds(self) -> None:
        blob = "\n".join(path.read_text(encoding="utf-8", errors="replace") for path in self.sets)
        for token in RUNTIME_FORBIDDEN:
            self.assertNotIn(token, blob)

    def test_stale_upstream_against_local_codex_when_present(self) -> None:
        """Local-only Code:X freshness proof. GitHub CI is expected to skip this."""
        codex = Path(os.environ.get("CODEX_BREED_ROOT", DEFAULT_CODEX))
        if not codex.is_dir():
            self.skipTest(
                "GitHub CI / runners without local Code:X: skipped. "
                "This test is the stale-upstream proof and must be run locally "
                "against the live Workshop Code:X tree."
            )
        source = {path.relative_to(codex).as_posix() for path in codex.rglob("*.set")}
        self.assertEqual(source, set(self.rel))
        unexpected = []
        for rel, path in self.rel.items():
            aio = path.read_text(encoding="utf-8", errors="replace")
            stripped = strip_diag_marker(strip_aio_tags(aio))
            upstream = (codex / Path(*rel.split("/"))).read_text(encoding="utf-8", errors="replace")
            if stripped != upstream and rel not in self.legacy:
                unexpected.append(rel)
        self.assertEqual(unexpected, [])


if __name__ == "__main__":
    unittest.main()
