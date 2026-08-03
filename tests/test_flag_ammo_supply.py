"""Structure pins for the per-flag ammo supply points.

Every flag in this map family shipped with an EMPTY built-in placer socket -
``{Placer {State "ammo" {Unlinked}}}`` - that nothing ever filled, so holding a
flag bought the holder no resupply at all. The base game's own CTF maps fill that
same socket the other way round and never carry the Placer block: a childless
``{Entity "flagpoint_ammo"}`` holding the supply_zone extender, plus a
``{Link <child> {<flag> "ammo"}}`` line binding it into the slot. Reference shape:
base game ``multi/2v2_countryside/battle_zones.mi`` lines 353-357 and 401.

``tools/deploy_attack_support_probe.ps1`` reproduces that per flag on every managed
map, in the deployed copy AND the repo copy, and the ammo table itself comes from
the shadow ``flagpoint_ammo.def`` this repo ships - the vanilla def with exactly one
line changed so that its ``("flag_ammo_heavy")`` call resolves against Code:X's
modern tables instead of the base game's WW2 ones, whose regeneration is disabled
by gameclass.

The two forms are mutually exclusive by design. Vanilla has the Link and no Placer;
a pristine map here had the Placer and no Link; a patched map must look like
vanilla. A map carrying both would be pinned as a defect below.
"""

from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MULTI = ROOT / "resource/map/multi"
DEPLOY = ROOT / "tools/deploy_attack_support_probe.ps1"
DEF = ROOT / (
    "resource/entity/service/-multiplayer/flag_point/flagpoint_ammo/flagpoint_ammo.def"
)
DEF_RELATIVE = (
    "resource\\entity\\service\\-multiplayer\\flag_point"
    "\\flagpoint_ammo\\flagpoint_ammo.def"
)
WORKSHOP = Path("E:/Steam/steamapps/workshop/content/400750/3636883799")

FLAG = re.compile(r'\{Entity "flag_point_campaign_\d+" (0x[0-9a-fA-F]+)')
CHILD = re.compile(r'\{Entity "flagpoint_ammo" (0x[0-9a-fA-F]+)')
LINK = re.compile(r'\{Link (0x[0-9a-fA-F]+) \{(0x[0-9a-fA-F]+) "ammo"\}\}')
UNLINKED = '{State "ammo" {Unlinked}}'


def managed_maps(root: Path):
    """The exact map set the deploy script owns: the fourteen CWA CTF maps."""
    multi = root / "resource/map/multi"
    return sorted(
        d / "campaign_capture_the_flag.mi"
        for d in multi.iterdir()
        if d.is_dir() and d.name.startswith("dcg_[cwa71]_")
    )


class FlagAmmoDefTest(unittest.TestCase):
    """The shadow def: one line different from the base game's, and that is the point."""

    def test_shadow_def_exists_at_the_vanilla_virtual_path(self) -> None:
        # Same virtual path as the pak entry, which is what makes it a shadow rather
        # than a new entity - and what lets the .mdl and the supply_zone decal keep
        # resolving from the pak, so neither is shipped here (precedent:
        # barbwire_on_wall.def). Shipping a .mdl next to it would shadow the model too.
        self.assertTrue(DEF.exists(), str(DEF))
        self.assertFalse((DEF.parent / "flagpoint_ammo.mdl").exists())
        self.assertFalse((DEF.parent / "supply_zone.ebm").exists())

    def test_shadow_def_pulls_the_modern_resupply_tables(self) -> None:
        text = DEF.read_text(encoding="utf-8")
        # The whole reason this file exists. The base include's flag_ammo_heavy is
        # WW2 items with gameclass-disabled regeneration; the hotmod one is
        # resupply_hotmod.inc's - 24m radius, 5s regeneration, limit 750, modern items.
        self.assertIn('(include "/properties/resupply_hotmod.inc")', text)
        self.assertNotIn('(include "/properties/resupply.inc")', text)
        # ... and the call that consumes it has to survive the swap.
        self.assertIn('("flag_ammo_heavy")', text)
        # Everything else is verbatim vanilla, including the model reference.
        self.assertIn('(include "/properties/construction.inc")', text)
        self.assertIn('{extension "flagpoint_ammo.mdl"}', text)
        self.assertEqual(text.count("{game_entity"), 1)
        self.assertEqual(text.count("{"), text.count("}"))
        self.assertEqual(text.count("("), text.count(")"))


class FlagAmmoMapTest(unittest.TestCase):
    """One live supply point per flag, in every map the deploy owns."""

    def assert_map_is_linked(self, path: Path) -> None:
        text = path.read_text(encoding="utf-8")
        flags = FLAG.findall(text)
        children = CHILD.findall(text)
        links = LINK.findall(text)

        self.assertGreater(len(flags), 0, "no campaign flags at all")
        # Exactly one supply point per flag - no spares parked, none missing.
        self.assertEqual(len(children), len(flags), "child count != flag count")
        self.assertEqual(len(links), len(flags), "link count != flag count")

        sources = [s for s, _t in links]
        targets = [t for _s, t in links]
        # Link ids unique on both ends: a duplicated child id would rebind one
        # entity into two slots, a duplicated target would stack two supply points
        # on one flag.
        self.assertEqual(len(set(sources)), len(sources), "duplicate child ids")
        self.assertEqual(len(set(targets)), len(targets), "duplicate link targets")
        self.assertEqual(set(sources), set(children), "link sources != child ids")
        self.assertEqual(set(targets), set(flags), "every flag must be linked")

        # Vanilla never carries both forms. The empty socket goes when the link lands.
        self.assertNotIn(UNLINKED, text)

        for child_id in children:
            # The child entity block must be declared ahead of the Link line that
            # names it - the engine resolves a Link against entities it has already
            # read, and vanilla emits them in that order too.
            at_entity = text.index('{Entity "flagpoint_ammo" %s' % child_id)
            at_link = text.index("{Link %s {" % child_id)
            self.assertLess(at_entity, at_link, "link precedes its child: " + child_id)
            block = text[at_entity : text.index("\n\t}", at_entity)]
            self.assertIn('{Extender "supply_zone"', block)
            self.assertIn("{enabled}", block)
            self.assertIn("{current 0}", block)

    def test_every_repo_map_has_a_supply_point_on_every_flag(self) -> None:
        maps = managed_maps(ROOT)
        self.assertEqual(len(maps), 14, "the deploy owns exactly fourteen CWA maps")
        for path in maps:
            with self.subTest(repo_map=path.parent.name):
                self.assert_map_is_linked(path)

    def test_every_deployed_map_has_a_supply_point_on_every_flag(self) -> None:
        if not WORKSHOP.exists():
            self.skipTest("workshop copy not present")
        maps = managed_maps(WORKSHOP)
        self.assertEqual(len(maps), 14)
        for path in maps:
            with self.subTest(workshop_map=path.parent.name):
                self.assert_map_is_linked(path)

    def test_repo_and_workshop_agree_on_the_supply_points(self) -> None:
        """The generator is deterministic, so both copies get identical blocks."""
        if not WORKSHOP.exists():
            self.skipTest("workshop copy not present")
        for repo_map in managed_maps(ROOT):
            deployed = WORKSHOP / repo_map.relative_to(ROOT)
            with self.subTest(map=repo_map.parent.name):
                self.assertTrue(deployed.exists(), str(deployed))
                a = LINK.findall(repo_map.read_text(encoding="utf-8"))
                b = LINK.findall(deployed.read_text(encoding="utf-8"))
                self.assertEqual(a, b)

    def test_child_ids_stay_inside_the_swept_band(self) -> None:
        """0xfd00 upward was collision-swept; drifting out of it is a defect."""
        for path in managed_maps(ROOT):
            with self.subTest(repo_map=path.parent.name):
                for child_id in CHILD.findall(path.read_text(encoding="utf-8")):
                    self.assertGreaterEqual(int(child_id, 16), 0xFD00, child_id)
                    self.assertLessEqual(int(child_id, 16), 0xFD1F, child_id)

    def test_delimiters_stay_balanced_in_every_patched_map(self) -> None:
        for path in managed_maps(ROOT):
            text = path.read_text(encoding="utf-8")
            with self.subTest(repo_map=path.parent.name):
                self.assertEqual(text.count("{"), text.count("}"))
                self.assertEqual(text.count("("), text.count(")"))


class FlagAmmoDeployTest(unittest.TestCase):
    """The deploy script owns the patching, ships the def, and self-heals."""

    @classmethod
    def setUpClass(cls) -> None:
        cls.deploy = DEPLOY.read_text(encoding="utf-8")

    def test_deploy_ships_the_shadow_def(self) -> None:
        copy_list = self.deploy[self.deploy.index("$files = @("):]
        copy_list = copy_list[: copy_list.index("\n)")]
        self.assertIn(DEF_RELATIVE, copy_list)
        # Appended, never inserted: the $files[n] lookups are positional, so an
        # insert silently repoints every later index at the wrong file.
        self.assertIn("$flagAmmoDefSource = Join-Path $RepoRoot $files[16]", self.deploy)
        self.assertIn("$flagPropsTplSource = Join-Path $RepoRoot $files[15]", self.deploy)
        self.assertIn("$conquestSource = Join-Path $RepoRoot $files[8]", self.deploy)

    def test_deploy_guards_the_include_swap_on_both_copies(self) -> None:
        for marker in (
            "Source flagpoint_ammo.def does not pull the modern resupply tables",
            "Source flagpoint_ammo.def still pulls the base WW2 resupply tables",
            "Source flagpoint_ammo.def is missing the flag_ammo_heavy supply-zone call",
            "Workshop flagpoint_ammo.def does not pull the modern resupply tables",
            "Workshop flagpoint_ammo.def still pulls the base WW2 resupply tables",
            "Workshop flagpoint_ammo.def is missing the flag_ammo_heavy supply-zone call",
            "Workshop carries a shadow flagpoint_ammo.mdl",
        ):
            self.assertIn(marker, self.deploy, marker)

    def test_deploy_patcher_is_self_healing_and_verified(self) -> None:
        for marker in (
            "function Set-FlagAmmoSupply",
            # Strips its own previous output before rebuilding, so a rerun is a
            # no-op and an interrupted run repairs itself.
            "Could not strip a previously written flagpoint_ammo block from",
            # Removes the empty socket rather than leaving both forms on the flag.
            "Map still carries an unlinked ammo placer socket after the strip",
            "Map carries both a linked supply point and an unlinked socket",
            # Per-file id uniqueness and the collision sweep.
            "$FlagAmmoIdBase = 0xfd00",
            "already in use in",
            "Duplicate flag-ammo child ids in",
            "Two ammo supply points linked into the same flag in",
            "has no ammo supply point in",
            # Both copies, and a reported total.
            '$flagsPatched = Set-FlagAmmoSupply $mapFile "workshop"',
            '$null = Set-FlagAmmoSupply $repoMap "repo"',
            "Flag ammo supply points linked:",
        ):
            self.assertIn(marker, self.deploy, marker)

    def test_patcher_runs_after_the_include_it_anchors_on_is_verified(self) -> None:
        anchor = "$FlagAmmoAnchor = '(include \"../attack_support_templates.inc\")'"
        self.assertIn(anchor, self.deploy)
        # The anchor is guaranteed present exactly once by the include audit, so the
        # patch call has to sit behind that audit, not ahead of it.
        at_audit = self.deploy.index("Expected exactly one $include in")
        at_call = self.deploy.index('$flagsPatched = Set-FlagAmmoSupply $mapFile')
        self.assertLess(at_audit, at_call)

    def test_deploy_owns_the_cwa_map_set_and_nothing_else(self) -> None:
        """Scope pin: the fourteen CWA CTF maps, not the other CTF maps present."""
        self.assertIn("^dcg_\\[cwa71\\]_", self.deploy)
        self.assertIn("Expected 14 CWA campaign_capture_the_flag.mi files", self.deploy)
        # bakhmut_1 / forest_ / map_ukrcity ship CTF maps too but are NOT managed by
        # this deploy - no includes, no waypoints, and so no supply points either.
        # They are pinned as out of scope so a future widening is a deliberate act.
        for unmanaged in ("bakhmut_1", "forest_", "map_ukrcity"):
            path = MULTI / unmanaged / "campaign_capture_the_flag.mi"
            if not path.exists():
                continue
            with self.subTest(unmanaged=unmanaged):
                self.assertNotIn(unmanaged, self.deploy)
                self.assertNotIn(
                    "flagpoint_ammo", path.read_text(encoding="utf-8", errors="ignore")
                )


if __name__ == "__main__":
    unittest.main()
