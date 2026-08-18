from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BEH = ROOT / "resource/map/multi/ce/ce_broken_behavior_triggers.inc"
MACHINE = ROOT / "resource/map/multi/ce/ce_morale_machine_triggers.inc"
HUMAN = ROOT / "resource/set/interaction_entity/human_ce.inc"
DCG = ROOT / "resource/map/multi/dcg_script.inc"
CONQ = ROOT / "resource/script/multiplayer/modes/conquest.lua"
WAVES = ROOT / "resource/map/multi/attack_support_waves.inc"


class CeBrokenBehaviorTests(unittest.TestCase):
    def test_stack_and_lua_yield(self) -> None:
        self.assertIn("ce_broken_behavior_triggers.inc", DCG.read_text(encoding="utf-8"))
        self.assertIn("aio_morale_owned", CONQ.read_text(encoding="utf-8"))
        self.assertIn("aio_morale_owned", WAVES.read_text(encoding="utf-8"))

    def test_player_excluded_from_broken_and_surrender(self) -> None:
        text = BEH.read_text(encoding="utf-8") + MACHINE.read_text(encoding="utf-8")
        self.assertIn("{state user_control}", text)
        self.assertIn("{tag player}", MACHINE.read_text(encoding="utf-8").split("escalate_broken", 1)[1])
        human = HUMAN.read_text(encoding="utf-8")
        surr = human.split('{on "aio_morale_surrender"', 1)[1]
        self.assertIn("not user_control", surr)

    def test_broken_recovers_to_panic(self) -> None:
        human = HUMAN.read_text(encoding="utf-8")
        done = human.split('{on "recovered_from_broken"', 1)[1].split('{on "', 1)[0]
        self.assertIn('{tags add "aio_morale_panic"}', done)
        self.assertIn("aio_cmd_linked", done)
        self.assertNotIn('{tags remove "aio_morale_owned"}', done)
        self.assertIn('{call "recovering_from_panic"}', done)
        steady = human.split('{on "recovered_from_shaken"', 1)[1].split('{on "', 1)[0]
        self.assertIn('{tags remove "aio_morale_owned"}', steady)
        self.assertIn("aio_morale_watch_regroup", human)
        self.assertIn("{relation ally}", BEH.read_text(encoding="utf-8"))

    def test_surrender_requires_broken_and_failed_regroup(self) -> None:
        text = BEH.read_text(encoding="utf-8")
        surr = text.split("broken/surrender", 1)[1]
        self.assertIn("aio_morale_broken", surr)
        self.assertIn("aio_morale_regroup_failed", surr)
        self.assertNotIn("aio_morale_panic", surr.split("{actions", 1)[0])
        apply = HUMAN.read_text(encoding="utf-8").split('{on "aio_morale_surrender"', 1)[1]
        self.assertIn("{if rand", apply)
        self.assertNotIn("{delete}", text)
        self.assertNotIn('{player "0"}', text)
        self.assertNotIn("{control AI}", text)

    def test_one_surrender_authority(self) -> None:
        self.assertEqual(
            BEH.read_text(encoding="utf-8").count("broken/surrender"),
            2,
        )


if __name__ == "__main__":
    unittest.main()
