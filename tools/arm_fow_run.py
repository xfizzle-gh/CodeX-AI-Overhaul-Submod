"""Arm or disarm the allied-support FoW gate run.

A live run needs two mission variables forced on that the shipped tree deliberately
leaves at 0:

    allied_support_cmd_enable$  -> 1   birth actually dispatches
    support_debug$              -> 1   the stage timers become visible

GoH has no console for setting a mission variable, so the only way to arm a run is to
write those two set_i actions into birth_init. They live between RUN_ONLY_ARMING
markers and must never be committed - tests/test_allied_support_birth.py has two guards
that go red while they are present.

Because birth_init is in the shared include, arming applies to all fourteen CWA maps at
once. That matters: a Dynamic Conquest campaign picks the map for you.

Usage:
    python tools/arm_fow_run.py --status
    python tools/arm_fow_run.py --on
    python tools/arm_fow_run.py --off
"""
import argparse
import pathlib
import sys

BIRTH = pathlib.Path("resource/map/multi/allied_support_birth.inc")

ANCHOR = '\t\t\t\t\t{"set_i" {var "allied_support_cmd_spawn_probe$"} {op "="} {value 0}}\n'

BLOCK = (
    "; RUN_ONLY_ARMING - do NOT commit. Revert with:\n"
    ";   python tools/arm_fow_run.py --off\n"
    '\t\t\t\t\t{"set_i" {var "allied_support_cmd_enable$"} {op "="} {value 1}}\n'
    '\t\t\t\t\t{"set_i" {var "support_debug$"} {op "="} {value 1}}\n'
    "; END RUN_ONLY_ARMING\n"
)

MARKER = "RUN_ONLY_ARMING"


def _read(root: pathlib.Path) -> str:
    return (root / BIRTH).read_text(encoding="utf-8")


def _write(root: pathlib.Path, text: str) -> None:
    (root / BIRTH).write_text(text, encoding="utf-8")


def is_armed(text: str) -> bool:
    return MARKER in text


def arm(text: str) -> tuple[str, bool]:
    if is_armed(text):
        return text, False
    if ANCHOR not in text:
        raise SystemExit(
            "could not find the init anchor line in allied_support_birth.inc; "
            "arm by hand and check the file has not been restructured"
        )
    return text.replace(ANCHOR, ANCHOR + BLOCK, 1), True


def disarm(text: str) -> tuple[str, bool]:
    if not is_armed(text):
        return text, False
    kept, dropping = [], False
    for line in text.split("\n"):
        if MARKER in line and not line.lstrip().startswith("; END"):
            dropping = True
            continue
        if line.lstrip().startswith("; END " + MARKER):
            dropping = False
            continue
        if not dropping:
            kept.append(line)
    return "\n".join(kept), True


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--on", action="store_true", help="arm a live run")
    group.add_argument("--off", action="store_true", help="revert to the shipped default")
    group.add_argument("--status", action="store_true", help="report without changing anything")
    args = parser.parse_args()

    root = pathlib.Path(__file__).resolve().parent.parent
    text = _read(root)

    if args.status:
        if is_armed(text):
            print("ARMED    - enable$=1, support_debug$=1 on all 14 maps. 2 guard tests are red.")
        else:
            print("DISARMED - shipped default. A live run will do nothing.")
        return 0

    new, changed = (arm(text) if args.on else disarm(text))
    if changed:
        _write(root, new)
    state = "ARMED" if args.on else "DISARMED"
    print(f"{state}{'' if changed else ' (already)'} - {BIRTH.as_posix()}")
    if args.on:
        print("  applies to all 14 CWA maps via the shared include")
        print("  remember: python tools/arm_fow_run.py --off  before committing")
    return 0


if __name__ == "__main__":
    sys.exit(main())
