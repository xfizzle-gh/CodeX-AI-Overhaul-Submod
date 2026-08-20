from __future__ import annotations

import importlib.util
import re
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "generate_pow_mirrors", ROOT / "tools/generate_pow_mirrors.py"
)
assert SPEC and SPEC.loader
gpm = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(gpm)

NATO = "mp/nato/2022s/nato_rifleman.set"
ISO = "isolation_test/aio_iso_hostile_soldier.set"
CIV = "isolation_test/aio_iso_hostile_civ.set"


class GeneratePowMirrorsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.mapped, cls.skipped = gpm.collect()
        cls.by_source = {source: (mirror, scope) for source, mirror, scope in cls.mapped}

    def test_production_eligible_are_one_to_one(self) -> None:
        sources = [row[0] for row in self.mapped]
        mirrors = [row[1] for row in self.mapped]
        self.assertEqual(sources, sorted(sources))
        self.assertEqual(len(sources), len(set(sources)))
        self.assertEqual(len(mirrors), len(set(mirrors)))
        self.assertEqual(len(self.mapped), 2091)
        self.assertIn(NATO, self.by_source)
        self.assertEqual(self.by_source[NATO], (f"generated_pow/{NATO}", "production"))
        overlay = {
            path.relative_to(gpm.BREED_ROOT).as_posix()
            for path in gpm.BREED_ROOT.rglob("*.set")
            if "isolation_test" not in path.parts and "generated_pow" not in path.parts
        }
        mapped_prod = {row[0] for row in self.mapped}
        skipped = {row[0] for row in self.skipped}
        self.assertEqual(overlay, mapped_prod | skipped)
        self.assertNotIn(ISO, mapped_prod)
        self.assertNotIn(CIV, mapped_prod)
        self.assertNotIn(CIV, skipped)

    def test_every_mapped_source_transforms_to_civilian_unarmed(self) -> None:
        for source, mirror, _scope in self.mapped:
            text = (gpm.BREED_ROOT / source).read_text(encoding="utf-8", errors="replace")
            out = gpm.transform(source, text)
            self.assertIn("{behaviour civilian}", out, source)
            self.assertNotIn("{behaviour soldier}", out, source)
            self.assertIsNone(re.search(r'\{tags\s+"[^"]*\bsoldier\b', out), source)
            inv = gpm.find_block(out, "inventory")
            if inv is not None:
                inner = out[inv[0] : inv[1]]
                for line in inner.splitlines():
                    if line.strip().startswith("{item"):
                        self.assertFalse(gpm.is_combat_item(line), source)
                        self.assertFalse(gpm.is_diagnostic_item(line), source)
            self.assertEqual(mirror, f"generated_pow/{source}")

    def test_committed_nato_mirror_matches_generator(self) -> None:
        text = (gpm.BREED_ROOT / NATO).read_text(encoding="utf-8")
        out = gpm.transform(NATO, text)
        dest = gpm.BREED_ROOT / f"generated_pow/{NATO}"
        self.assertTrue(dest.is_file())
        actual = dest.read_text(encoding="utf-8")
        if not out.endswith("\n"):
            out += "\n"
        self.assertEqual(actual, out)
        self.assertIn("{behaviour civilian}", actual)
        self.assertNotIn("{behaviour soldier}", actual)
        self.assertNotIn('{tags "soldier"}', actual)
        self.assertNotIn("mars_l", actual)
        self.assertNotIn("m26 grenade", actual)
        self.assertNotIn("m16a2 ammo", actual)
        self.assertNotIn("aio_marker_morale_regular", actual)
        self.assertIn('{item "backpack_eagleaiii"}', actual)
        self.assertIn('{item "bandage_usa" 4.5 0.5}', actual)
        self.assertIn('{item "shovel_csa"}', actual)
        self.assertIn("{in_hands 0}", actual)
        self.assertIn('{skin "nrf_1"}', actual)
        self.assertIn('{body "nrf_vest_1"}', actual)
        self.assertIn('(include "/set/breed/mp/nato/2022s/ability.inc")', actual)
        self.assertNotIn('(include "ability.inc")', actual)
        self.assertFalse((gpm.BREED_ROOT / f"generated_pow/{ISO}").is_file())

    def test_ineligible_fixtures_fail_closed(self) -> None:
        civ = (gpm.BREED_ROOT / CIV).read_text(encoding="utf-8")
        ok, reason = gpm.classify(CIV, civ, allow_iso=True)
        self.assertFalse(ok)
        self.assertEqual(reason, "not_soldier")
        with self.assertRaises(ValueError):
            gpm.transform(CIV, civ)
        ok, reason = gpm.classify("x.set", "{breed\n\t{behaviour soldier}\n}\n")
        self.assertFalse(ok)
        self.assertEqual(reason, "no_skin")

    def test_mapping_files_are_fresh(self) -> None:
        self.assertEqual(gpm.check(), 0)

    def test_tmp_collect_is_deterministic(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            sample = root / "mp/nato/2022s/nato_rifleman.set"
            sample.parent.mkdir(parents=True)
            sample.write_text(
                (gpm.BREED_ROOT / NATO).read_text(encoding="utf-8"),
                encoding="utf-8",
            )
            old = gpm.BREED_ROOT
            try:
                gpm.BREED_ROOT = root
                first, _ = gpm.collect(root)
                second, _ = gpm.collect(root)
            finally:
                gpm.BREED_ROOT = old
            self.assertEqual(first, second)
            self.assertEqual(first[0][0], NATO)


class EditorPowReplaceTests(unittest.TestCase):
    def test_editor_only_surrender_commit_move(self) -> None:
        trig = ROOT / "resource/map/multi/ce/ce_triggers.inc"
        dcg = ROOT / "resource/map/multi/dcg_script.inc"
        editor = ROOT / "resource/map/multi/ce/ce_pow_replace_editor.inc"
        templates = ROOT / "resource/map/multi/ce/ce_pow_replace_editor_templates.inc"
        human = ROOT / "resource/set/interaction_entity/human_ce.inc"
        self.assertTrue(editor.is_file())
        self.assertTrue(templates.is_file())
        text = editor.read_text(encoding="utf-8")
        tpl = templates.read_text(encoding="utf-8")
        apply = human.read_text(encoding="utf-8").split('{on "aio_morale_surrender_apply"', 1)[1].split('{on "', 1)[0]
        self.assertNotIn("ce_pow_replace_editor", trig.read_text(encoding="utf-8"))
        self.assertNotIn("ce_pow_replace_editor", dcg.read_text(encoding="utf-8"))
        self.assertIn('{"placement"', text)
        self.assertNotIn("{clone}", text)
        self.assertNotIn("{clone_places}", text)
        self.assertNotIn("{stat_notify", text)
        self.assertNotIn('{player "0"}', text)
        self.assertNotIn("{control AI}", text)
        self.assertNotIn("{tag_add hidden}", text)
        self.assertNotIn("{inactive on}", text)
        self.assertIn("{effect aio_pow_retire}", text)
        self.assertIn("aio_pow_need_replace", text)
        self.assertIn("aio_pow_replace_src", text)
        self.assertIn("{effect aio_morale_surrender_apply}", text)
        self.assertIn("aio_pow_walk", text)
        retire = human.read_text(encoding="utf-8").split('{on "aio_pow_retire"', 1)[1].split("{on ", 1)[0]
        self.assertIn("{delete}", retire)
        self.assertNotIn("{stat_notify", retire)
        self.assertNotIn('{call "delete"}', retire)
        self.assertIn('{Human "generated_pow/mp/nato/2022s/nato_rifleman"', tpl)
        self.assertIn("{Player 2}", tpl)
        self.assertNotIn("{Player 0}", tpl)
        self.assertIn('"hidden"', tpl)
        self.assertIn('tagged "aio_pow_replace_src"', apply)
        self.assertIn('{tags add "aio_pow_need_replace"}', apply)
        self.assertNotIn('{player "0"}', apply)
        self.assertNotIn("{control AI}", apply)


if __name__ == "__main__":
    unittest.main()
