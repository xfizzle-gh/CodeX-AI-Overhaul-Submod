from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MAP_NAMES = [
    "dcg_[cwa71]_airbase",
    "dcg_[cwa71]_border",
    "dcg_[cwa71]_europe",
    "dcg_[cwa71]_factory",
    "dcg_[cwa71]_fields",
    "dcg_[cwa71]_fulda",
    "dcg_[cwa71]_grassland",
    "dcg_[cwa71]_industrial",
    "dcg_[cwa71]_monastery",
    "dcg_[cwa71]_outback",
    "dcg_[cwa71]_stasis",
    "dcg_[cwa71]_train_station",
    "dcg_[cwa71]_winds_valley",
    "dcg_[cwa71]_woodland",
]
BREED = "mp/nato/2022s/inf2_rifleman"
TAG = "allied_support_explicit_template"


def free_handle(text: str, start: int) -> str:
    used = {int(value, 16) for value in re.findall(r"0x([0-9a-fA-F]+)", text)}
    candidate = start
    while candidate in used:
        candidate += 1
    return f"0x{candidate:x}"


def free_mid(text: str, start: int) -> int:
    used = {int(value) for value in re.findall(r"\{MID\s+(\d+)\}", text)}
    candidate = start
    while candidate in used:
        candidate += 1
    return candidate


def patch_map(path: Path, index: int) -> None:
    text = path.read_text(encoding="utf-8")
    if TAG in text:
        return

    handle = free_handle(text, 0xAF10 + index * 0x10)
    mid = free_mid(text, 9901 + index)
    actor = f'''\t{{Human "{BREED}" {handle}
\t\t{{Position -35000 -35000}}
\t\t{{Volume "ram"
\t\t\t{{able {{visible 0}}{{bullet 0}}{{throwing 0}}{{obstacle 0}}{{contact 0}}{{contact_ground 0}}{{blast 0}}{{select 0}}{{touch 0}}{{blockcamera 0}}}}
\t\t\t{{disabled}}
\t\t}}
\t\t{{Player 0}}
\t\t{{MID {mid}}}
\t\t{{FsmState "stand_noaim"}}
\t}}
\t{{Tags "{TAG}" "not_delete" "hidden" {handle}}}
'''

    match = re.search(r"^\t\{Tags\s", text, flags=re.MULTILINE)
    if not match:
        raise RuntimeError(f"No top-level Tags section in {path}")
    text = text[: match.start()] + actor + text[match.start() :]
    path.write_text(text, encoding="utf-8")


def ownership_cases() -> str:
    blocks: list[str] = []
    for player_id in range(1, 17):
        blocks.append(
            f'''\t\t\t\t\t\t{{"case"
\t\t\t\t\t\t\t{{condition {{type cmp_i}} {{var "id_defenderbot$"}} {{op "=="}} {{value {player_id}}}}}
\t\t\t\t\t\t\t{{"player" {{selector {{tag allied_wave_fresh}}}} {{operation set}} {{player "{player_id}"}}}}
\t\t\t\t\t\t\t{{"entity_state" {{selector {{tag allied_wave_fresh}}}} {{tag_add allied_support_owner_{player_id}}} {{tag_add allied_support_explicit_owned}}}}
\t\t\t\t\t\t}}'''
        )
    return "\n".join(blocks)


def write_shared_trigger() -> None:
    content = f'''; One-shot Indomitus-style proof using one explicit named actor embedded in every CWA map.
; Test only. Human defense, DefenderBot ownership, waypoint 1 / fpc1.
; No CE template-pool fallback and no repeating loop.

\t\t\t{{"allied_support/explicit_actor_once"
\t\t\t\t{{condition
\t\t\t\t\t{{expression "1 & 2 & 3"}}
\t\t\t\t\t{{terms
\t\t\t\t\t\t{{"1.cmp_i" {{var "user_is_defender$"}} {{op "=="}} {{value 1}}}}
\t\t\t\t\t\t{{"2.cmp_i" {{var "id_defenderbot$"}} {{op ">"}} {{value 0}}}}
\t\t\t\t\t\t{{"3.cmp_i" {{var "prep_inform$"}} {{op "=="}} {{value 1}}}}
\t\t\t\t\t}}
\t\t\t\t}}
\t\t\t\t{{actions
\t\t\t\t\t{{"delay" {{time 10}}}}
\t\t\t\t\t{{"placement"
\t\t\t\t\t\t{{selector
\t\t\t\t\t\t\t{{source advanced}}
\t\t\t\t\t\t\t{{group
\t\t\t\t\t\t\t\t{{select {{tag {{tag {TAG}}}}}}}
\t\t\t\t\t\t\t\t{{include {{prop {{prop human}}}}}}
\t\t\t\t\t\t\t\t{{exclude
\t\t\t\t\t\t\t\t\t{{state {{state dead}}}}
\t\t\t\t\t\t\t\t\t{{zone {{zone "gamezone"}}}}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t{{amount 1}}
\t\t\t\t\t\t}}
\t\t\t\t\t\t{{target_waypoint "1"}}
\t\t\t\t\t\t{{clone}}
\t\t\t\t\t}}
\t\t\t\t\t{{"delay" {{time 0.5}}}}
\t\t\t\t\t{{"entity_state"
\t\t\t\t\t\t{{selector
\t\t\t\t\t\t\t{{source advanced}}
\t\t\t\t\t\t\t{{group
\t\t\t\t\t\t\t\t{{select {{tag {{tag {TAG}}}}}}}
\t\t\t\t\t\t\t\t{{include
\t\t\t\t\t\t\t\t\t{{prop {{prop human}}}}
\t\t\t\t\t\t\t\t\t{{zone {{zone "gamezone"}}}}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t{{exclude {{state {{state dead}}}}}}
\t\t\t\t\t\t\t}}
\t\t\t\t\t\t}}
\t\t\t\t\t\t{{tag_add allied_wave_fresh}}
\t\t\t\t\t\t{{tag_add allied_support}}
\t\t\t\t\t\t{{tag_add cmp_def}}
\t\t\t\t\t\t{{tag_add _def}}
\t\t\t\t\t\t{{tag_add _ai_defender}}
\t\t\t\t\t\t{{tag_add allied_support_explicit_clone}}
\t\t\t\t\t\t{{tag_remove {TAG}}}
\t\t\t\t\t\t{{tag_remove not_delete}}
\t\t\t\t\t\t{{tag_remove hidden}}
\t\t\t\t\t\t{{inactive off}}
\t\t\t\t\t}}
\t\t\t\t\t{{"delay" {{time 0.1}}}}
\t\t\t\t\t{{"switch"
{ownership_cases()}
\t\t\t\t\t\t{{"default"
\t\t\t\t\t\t\t{{"entity_state" {{selector {{tag allied_wave_fresh}}}} {{tag_add allied_support_owner_unsupported}}}}
\t\t\t\t\t\t}}
\t\t\t\t\t}}
\t\t\t\t\t{{"actor_state"
\t\t\t\t\t\t{{selector {{tag allied_wave_fresh}} {{type human}}}}
\t\t\t\t\t\t{{control AI}}
\t\t\t\t\t\t{{weapon_prepare on}}
\t\t\t\t\t\t{{fire_mode open}}
\t\t\t\t\t\t{{move_mode free}}
\t\t\t\t\t\t{{movement {{speed assault}}}}
\t\t\t\t\t\t{{ai {{no_retreat on}}}}
\t\t\t\t\t}}
\t\t\t\t\t{{"ables"
\t\t\t\t\t\t{{selector {{tag allied_wave_fresh}} {{type human}}}}
\t\t\t\t\t\t{{remove select}}
\t\t\t\t\t}}
\t\t\t\t\t{{"action"
\t\t\t\t\t\t{{selector {{tag allied_wave_fresh}} {{type human}}}}
\t\t\t\t\t\t{{drop orders}}
\t\t\t\t\t\t{{action advance}}
\t\t\t\t\t\t{{target {{tag fpc1}}}}
\t\t\t\t\t}}
\t\t\t\t\t{{"entity_state" {{selector {{tag allied_wave_fresh}}}} {{tag_remove allied_wave_fresh}}}}
\t\t\t\t}}
\t\t\t}}
'''
    path = ROOT / "resource/map/multi/allied_support_waves.inc"
    path.write_text(content, encoding="utf-8")


def write_test() -> None:
    content = '''from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAP_NAMES = [
    "dcg_[cwa71]_airbase", "dcg_[cwa71]_border", "dcg_[cwa71]_europe",
    "dcg_[cwa71]_factory", "dcg_[cwa71]_fields", "dcg_[cwa71]_fulda",
    "dcg_[cwa71]_grassland", "dcg_[cwa71]_industrial", "dcg_[cwa71]_monastery",
    "dcg_[cwa71]_outback", "dcg_[cwa71]_stasis", "dcg_[cwa71]_train_station",
    "dcg_[cwa71]_winds_valley", "dcg_[cwa71]_woodland",
]
BREED = "mp/nato/2022s/inf2_rifleman"
TAG = "allied_support_explicit_template"
MISSIONS = [ROOT / "resource/map/multi" / name / "campaign_capture_the_flag.mi" for name in MAP_NAMES]
WAVES = ROOT / "resource/map/multi/allied_support_waves.inc"


class ExplicitCwaSupportProofTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.missions = {path.parent.name: path.read_text(encoding="utf-8") for path in MISSIONS}
        cls.waves = WAVES.read_text(encoding="utf-8")

    def test_all_fourteen_maps_embed_one_named_template(self) -> None:
        self.assertEqual(len(self.missions), 14)
        handles = []
        mids = []
        for name, mission in self.missions.items():
            with self.subTest(map=name):
                self.assertEqual(mission.count(f'Human "{BREED}"'), 1)
                self.assertEqual(mission.count(TAG), 1)
                self.assertEqual(mission.count("allied_support_waves.inc"), 1)
                match = re.search(rf'Human "{re.escape(BREED)}" (0x[0-9a-f]+).*?\{{Position -35000 -35000\}}.*?\{{Player 0\}}.*?\{{MID (\d+)\}}', mission, re.S)
                self.assertIsNotNone(match)
                handles.append(match.group(1))
                mids.append(match.group(2))
                self.assertIn(f'{{Tags "{TAG}" "not_delete" "hidden" {match.group(1)}}}', mission)
                self.assertIn('{"1"', mission)
        self.assertEqual(len(set(handles)), 14)
        self.assertEqual(len(set(mids)), 14)

    def test_shared_trigger_uses_only_explicit_actor(self) -> None:
        for marker in (
            'allied_support/explicit_actor_once',
            f'{{tag {{tag {TAG}}}}}',
            '{target_waypoint "1"}',
            '{clone}',
            '{zone {zone "gamezone"}}',
            '{tag_add allied_wave_fresh}',
            '{tag_add allied_support_explicit_clone}',
            f'{{tag_remove {TAG}}}',
            '{tag_remove not_delete}',
            '{tag_remove hidden}',
            '{inactive off}',
            '{control AI}',
            '{remove select}',
            '{action advance}',
            '{target {tag fpc1}}',
        ):
            self.assertIn(marker, self.waves)
        self.assertNotIn('allied_support_template', self.waves.replace(TAG, ''))
        self.assertNotIn('allied_support_diag_source', self.waves)
        self.assertNotIn('{"timer"', self.waves)
        self.assertNotIn('{control user}', self.waves)
        self.assertNotIn('{"trigger" {name "allied_support/explicit_actor_once"}', self.waves)

    def test_defenderbot_ownership_cases_cover_ids_1_to_16(self) -> None:
        for player_id in range(1, 17):
            self.assertIn(f'{{value {player_id}}}', self.waves)
            self.assertIn(f'{{player "{player_id}"}}', self.waves)
            self.assertIn(f'allied_support_owner_{player_id}', self.waves)

    def test_delimiters_balance(self) -> None:
        for text in (*self.missions.values(), self.waves):
            self.assertEqual(text.count("{"), text.count("}"))
            self.assertEqual(text.count("("), text.count(")"))


if __name__ == "__main__":
    unittest.main()
'''
    (ROOT / "tests/test_woodland_support_probe.py").write_text(content, encoding="utf-8")


for idx, map_name in enumerate(MAP_NAMES):
    patch_map(ROOT / "resource/map/multi" / map_name / "campaign_capture_the_flag.mi", idx)
write_shared_trigger()
write_test()
