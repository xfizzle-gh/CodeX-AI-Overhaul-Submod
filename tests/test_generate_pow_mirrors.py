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
            self.assertNotIn("{in_hands", out, source)
            inv = gpm.find_block(out, "inventory")
            if inv is not None:
                inner = out[inv[0] : inv[1]]
                spans = gpm.ITEM_SPAN_RE.findall(inner)
                self.assertTrue(spans, source)
                for span in spans:
                    name = gpm.item_name(span)
                    self.assertEqual(gpm.item_class(name), "keep", f"{source} {name}")
                    self.assertIsNotNone(gpm.KEEP_RE.search(name), f"{source} {name}")
            self.assertEqual(mirror, f"generated_pow/{source}")

    def test_nato_mirror_transform_stays_out_of_live_tree(self) -> None:
        text = (gpm.BREED_ROOT / NATO).read_text(encoding="utf-8")
        out = gpm.transform(NATO, text)
        dest = gpm.BREED_ROOT / f"generated_pow/{NATO}"
        self.assertFalse(dest.is_file())
        live_gen = gpm.BREED_ROOT / "generated_pow"
        live_sets = list(live_gen.rglob("*.set")) if live_gen.exists() else []
        self.assertEqual(live_sets, [])
        self.assertIn("{behaviour civilian}", out)
        self.assertNotIn("{behaviour soldier}", out)
        self.assertNotIn('{tags "soldier"}', out)
        self.assertNotIn("mars_l", out)
        self.assertNotIn("m26 grenade", out)
        self.assertNotIn("m16a2 ammo", out)
        self.assertNotIn("aio_marker_morale_regular", out)
        self.assertIn('{item "backpack_eagleaiii"}', out)
        self.assertIn('{item "bandage_usa" 4.5 0.5}', out)
        self.assertIn('{item "shovel_csa"}', out)
        self.assertNotIn("{in_hands", out)
        self.assertEqual(gpm.item_class("backpack_eagleaiii"), "keep")
        self.assertEqual(gpm.item_class("mars_l"), "strip")
        self.assertEqual(gpm.item_class("ak74"), "strip")
        self.assertIn('{skin "nrf_1"}', out)
        self.assertIn('{body "nrf_vest_1"}', out)
        self.assertIn('(include "/set/breed/mp/nato/2022s/ability.inc")', out)
        self.assertNotIn('(include "ability.inc")', out)
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

    def test_item_classes_are_allowlist_not_denylist(self) -> None:
        rows = gpm.collect_item_classes()
        by_name = {name: klass for name, klass, _count in rows}
        self.assertEqual(by_name["backpack_eagleaiii"], "keep")
        self.assertEqual(by_name["mars_l"], "strip")
        self.assertEqual(by_name["ak74"], "strip")
        self.assertEqual(by_name["m4a1_v3b"], "strip")
        self.assertNotIn("keep", {by_name["mars_l"], by_name["ak74"], by_name["m4a1_v3b"]})
        kept = [name for name, klass, _count in rows if klass == "keep"]
        self.assertTrue(kept)
        self.assertTrue(all(gpm.KEEP_RE.search(name) for name in kept))
        stripped = [name for name, klass, _count in rows if klass == "strip"]
        self.assertTrue(stripped)
        self.assertTrue(all(gpm.KEEP_RE.search(name) is None for name in stripped))
        self.assertEqual(len(rows), len(by_name))
        self.assertNotIn("", by_name)

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
    def test_editor_pow_resources_removed_for_startup_isolation(self) -> None:
        human = (ROOT / "resource/set/interaction_entity/human_ce.inc").read_text(encoding="utf-8")
        self.assertFalse((ROOT / "resource/map/multi/ce/ce_pow_replace_editor.inc").is_file())
        self.assertFalse((ROOT / "resource/map/multi/ce/ce_pow_replace_editor_templates.inc").is_file())
        self.assertFalse((ROOT / "resource/set/breed/generated_pow").exists())
        self.assertNotIn("aio_pow_retire", human)
        self.assertNotIn("aio_pow_replace_src", human)
        self.assertNotIn("aio_pow_need_replace", human)
        self.assertNotIn("{delete}", human)


if __name__ == "__main__":
    unittest.main()
