from __future__ import annotations

import os
import re
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BREED_ROOT = ROOT / "resource/set/breed"
REPORT = ROOT / "docs/morale_command_classification.md"
TSV = ROOT / "docs/morale_command_classification.tsv"
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

LEGACY_VISUAL = {
    "mp/nato/2022s/lrs_javelin.set",
    "mp/nato/2022s/lrs_marksman.set",
    "mp/nato/2022s/lrs_mg.set",
    "mp/nato/2022s/lrs_rifleman.set",
    "mp/nato/2022s/lrs_squadlead.set",
    "mp/nato/2022s/rsta_antitank.set",
    "mp/nato/2022s/rsta_autorifle.set",
    "mp/nato/2022s/rsta_rifleman.set",
    "mp/nato/2022s/rsta_scout.set",
    "mp/nato/2022s/rsta_squadlead.set",
    "mp/nato/2022s/rsta_teamlead.set",
}


def aio_tokens(text: str) -> list[str]:
    match = TAGS_RE.search(text)
    if not match:
        return []
    return [token for token in match.group(2).split() if AIO_RE.match(token)]


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


class MoraleBreedMetadataTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.sets = sorted(BREED_ROOT.rglob("*.set"))
        cls.rel = {path.relative_to(BREED_ROOT).as_posix(): path for path in cls.sets}

    def test_report_and_tsv_exist(self) -> None:
        self.assertTrue(REPORT.is_file())
        self.assertTrue(TSV.is_file())
        text = REPORT.read_text(encoding="utf-8")
        self.assertIn("aio_morale_low", text)
        self.assertIn("Static identity only", text)
        self.assertIn("3604287428", text)
        self.assertIn("3702483522", text)

    def test_every_overlay_has_exactly_one_morale_profile(self) -> None:
        self.assertEqual(len(self.sets), 2091)
        for path in self.sets:
            tokens = aio_tokens(path.read_text(encoding="utf-8", errors="replace"))
            morale = [token for token in tokens if token in MORALE]
            self.assertEqual(len(morale), 1, path)
            unknown = [token for token in tokens if token not in ALLOWED]
            self.assertEqual(unknown, [], path)
            forbidden = [token for token in tokens if token in RUNTIME_FORBIDDEN]
            self.assertEqual(forbidden, [], path)
            cmd = [token for token in tokens if token in CMD and token != "aio_cmd_independent"]
            self.assertLessEqual(len(cmd), 1, path)

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
        codex = Path(os.environ.get("CODEX_BREED_ROOT", DEFAULT_CODEX))
        if not codex.is_dir():
            self.skipTest("local Code:X breed tree not present")
        unexpected = []
        missing = []
        for rel, path in self.rel.items():
            src = codex / Path(*rel.split("/"))
            if not src.is_file():
                missing.append(rel)
                continue
            aio = path.read_text(encoding="utf-8", errors="replace")
            stripped = strip_aio_tags(aio)
            upstream = src.read_text(encoding="utf-8", errors="replace")
            if stripped != upstream and rel not in LEGACY_VISUAL and not rel.startswith("mp/nato/新建文件夹/") and not rel.startswith("mp/usam/"):
                unexpected.append(rel)
        self.assertEqual(missing, [])
        self.assertEqual(unexpected, [])


if __name__ == "__main__":
    unittest.main()
