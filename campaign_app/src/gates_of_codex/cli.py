from __future__ import annotations

import argparse
import json
import sys
from dataclasses import asdict
from importlib.resources import files
from pathlib import Path

from .bridge.status import BattleStatusOptions
from .campaign import CampaignEngine
from .codex.catalog import CodeXCatalogScanner
from .doctor import diagnose
from .launcher import launch_game
from .scenario import load_scenario
from .service import GatesOfCodeXService
from .starter import populate_valid_rosters
from .state_io import load, save


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="gates-of-codex",
        description="Strategic campaign bridge for Gates of Hell and Code:X",
    )
    sub = parser.add_subparsers(dest="command", required=True)

    doctor = sub.add_parser("doctor", help="Locate and validate game data")
    doctor.add_argument("--game")
    doctor.add_argument("--codex")
    doctor.add_argument("--profile")

    scan = sub.add_parser("scan", help="Scan Code:X units")
    scan.add_argument("--codex", required=True)
    scan.add_argument("--output")

    new = sub.add_parser("new", help="Create a four-faction campaign")
    new.add_argument("--output", required=True)
    new.add_argument("--codex", required=True)
    new.add_argument("--name", default="Gates of CodeX Campaign")
    new.add_argument("--scenario")

    show = sub.add_parser("show", help="Show campaign state")
    show.add_argument("state")

    move = sub.add_parser("move", help="Move or attack an adjacent province")
    move.add_argument("state")
    move.add_argument("battalion")
    move.add_argument("target")

    auto = sub.add_parser("auto-resolve", help="Resolve the pending battle")
    auto.add_argument("state")

    end = sub.add_parser("end-turn", help="Advance to the next faction")
    end.add_argument("state")

    export = sub.add_parser("export-battle", help="Write a Code:X campaign.sav")
    export.add_argument("state")
    export.add_argument("--codex", required=True)
    export.add_argument("--save", required=True)
    export.add_argument("--map", required=True, dest="map_string")
    export.add_argument("--played-games", type=int, default=0)
    export.add_argument("--won-games", type=int, default=0)
    export.add_argument("--game-version", default="1.062.0")

    imported = sub.add_parser("import-battle", help="Import a completed Code:X battle")
    imported.add_argument("state")
    imported.add_argument("--save", required=True)
    imported.add_argument("--manifest")

    launch = sub.add_parser("launch", help="Launch Gates of Hell")
    launch.add_argument("--game", required=True)
    launch.add_argument("game_args", nargs=argparse.REMAINDER)

    ui = sub.add_parser("ui", help="Open the desktop campaign map")
    ui.add_argument("state", nargs="?")
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        if args.command == "doctor":
            _print_json(
                diagnose(
                    game_directory=args.game,
                    code_x_directory=args.codex,
                    profile_directory=args.profile,
                ).to_dict()
            )
        elif args.command == "scan":
            catalog = CodeXCatalogScanner().scan(args.codex)
            if args.output:
                catalog.save(args.output)
            _print_json(
                {
                    "units": len(catalog.units),
                    "scanned_files": catalog.scanned_files,
                    "signature": catalog.source_signature,
                    "warnings": catalog.warnings,
                }
            )
        elif args.command == "new":
            scenario_path = args.scenario or str(
                files("gates_of_codex").joinpath("data/four_faction.json")
            )
            state = load_scenario(scenario_path, campaign_name=args.name)
            catalog = CodeXCatalogScanner().scan(args.codex)
            populate_valid_rosters(state, catalog)
            state.code_x_directory = str(Path(args.codex).resolve())
            save(state, args.output)
            _print_json({"campaign": args.output, "battalions": len(state.battalions)})
        elif args.command == "show":
            _print_json(load(args.state).to_dict())
        elif args.command == "move":
            state = load(args.state)
            result = CampaignEngine(state).move_or_attack(args.battalion, args.target)
            save(state, args.state)
            _print_json(
                {
                    "moved": result.moved,
                    "pending_battle": (
                        result.pending_battle.battle_id if result.pending_battle else None
                    ),
                }
            )
        elif args.command == "auto-resolve":
            state = load(args.state)
            winner = CampaignEngine(state).auto_resolve_pending_battle()
            save(state, args.state)
            _print_json({"winner": winner.value})
        elif args.command == "end-turn":
            state = load(args.state)
            faction = CampaignEngine(state).end_turn()
            save(state, args.state)
            _print_json({"turn": state.turn_number, "current_faction": faction.value})
        elif args.command == "export-battle":
            manifest = GatesOfCodeXService().export_pending_battle(
                state_path=args.state,
                code_x_directory=args.codex,
                save_path=args.save,
                options=BattleStatusOptions(
                    map_string=args.map_string,
                    played_games=args.played_games,
                    won_games=args.won_games,
                    game_version=args.game_version,
                ),
            )
            _print_json(asdict(manifest))
        elif args.command == "import-battle":
            result = GatesOfCodeXService().import_completed_battle(
                state_path=args.state,
                save_path=args.save,
                manifest_path=args.manifest,
            )
            _print_json(
                {
                    "winner": result.winner.value,
                    "player_won": result.player_won,
                    "survivor_counts": result.survivor_counts,
                }
            )
        elif args.command == "launch":
            process = launch_game(args.game, arguments=args.game_args)
            _print_json({"pid": process.pid})
        elif args.command == "ui":
            from .gui import main as gui_main

            gui_main(args.state)
        return 0
    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1


def _print_json(value: object) -> None:
    print(json.dumps(value, indent=2, ensure_ascii=False, default=str))


if __name__ == "__main__":
    raise SystemExit(main())
