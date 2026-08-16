#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RUNTIME = ROOT / "resource/map/multi/attack_support_waves.inc"
TESTS = ROOT / "tests/test_allied_support_shared_fow.py"


def patch_runtime() -> None:
    text = RUNTIME.read_text(encoding="utf-8-sig")
    start = text.find('(define "am_finish_deploy"')
    end = text.find('(define "am_deploy_next_hmmwv"', start)
    if start < 0 or end <= start:
        raise RuntimeError("could not isolate am_finish_deploy")

    finish = text[start:end]
    if '{"autoassign"' in finish:
        raise RuntimeError("am_finish_deploy already contains autoassign")

    cleanup_tag = finish.rfind("{tag_remove attack_support_deploy}")
    cleanup_entity = finish.rfind('{"entity_state"', 0, cleanup_tag)
    if cleanup_tag < 0 or cleanup_entity < 0:
        raise RuntimeError("final deploy cleanup not found")

    actor_pos = finish.find('{"actor_state"')
    ables_pos = finish.find('{"ables"', actor_pos)
    if not (0 <= actor_pos < ables_pos < cleanup_entity):
        raise RuntimeError("actor_state/ables/cleanup ordering is not canonical")
    if "{control AI}" not in finish[actor_pos:ables_pos]:
        raise RuntimeError("AI control marker missing before terrain FoW handoff")
    if "{remove select}" not in finish[ables_pos:cleanup_entity]:
        raise RuntimeError("selection lock marker missing before terrain FoW handoff")

    handoff = (
        "\t\t\t\t; Final player handoff for terrain FoW. Keep AI control and selection disabled above.\n"
        "\t\t\t\t{\"autoassign\"\n"
        "\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}\n"
        "\t\t\t\t}\n"
    )
    finish = finish[:cleanup_entity] + handoff + finish[cleanup_entity:]
    RUNTIME.write_text(text[:start] + finish + text[end:], encoding="utf-8", newline="")


def patch_tests() -> None:
    text = TESTS.read_text(encoding="utf-8")
    if "test_attack_support_autoassigns_after_ai_and_selection_lock" in text:
        raise RuntimeError("terrain FoW regression test already exists")

    marker = "    def test_all_four_support_quadrants_have_correct_ai_ownership(self) -> None:\n"
    if marker not in text:
        raise RuntimeError("test insertion marker missing")

    test = '''    def test_attack_support_autoassigns_after_ai_and_selection_lock(self) -> None:
        finish_start = self.attack_waves_inc.find('(define "am_finish_deploy"')
        finish_end = self.attack_waves_inc.find('(define "am_deploy_next_hmmwv"', finish_start)
        self.assertGreater(finish_start, 0)
        self.assertGreater(finish_end, finish_start)
        finish = self.attack_waves_inc[finish_start:finish_end]

        actor_pos = finish.find('{"actor_state"')
        ables_pos = finish.find('{"ables"', actor_pos)
        autoassign_pos = finish.find('{"autoassign"', ables_pos)
        cleanup_pos = finish.find('{tag_remove attack_support_deploy}', autoassign_pos)

        self.assertGreater(actor_pos, 0)
        self.assertGreater(ables_pos, actor_pos)
        self.assertGreater(autoassign_pos, ables_pos)
        self.assertGreater(cleanup_pos, autoassign_pos)
        self.assertIn('{control AI}', finish[actor_pos:ables_pos])
        self.assertIn('{remove select}', finish[ables_pos:autoassign_pos])
        self.assertIn(
            '{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}',
            finish[autoassign_pos:cleanup_pos],
        )
        self.assertEqual(finish.count('{"autoassign"'), 1)

'''
    TESTS.write_text(text.replace(marker, test + marker, 1), encoding="utf-8", newline="")


if __name__ == "__main__":
    patch_runtime()
    patch_tests()
    print("terrain_fow_patch=applied")
