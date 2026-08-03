from __future__ import annotations

import unittest

from gates_of_codex.cli import build_parser


class CliTests(unittest.TestCase):
    def test_commands_parse(self) -> None:
        parser = build_parser()
        self.assertEqual("doctor", parser.parse_args(["doctor"]).command)
        self.assertEqual(
            "export-battle",
            parser.parse_args(
                [
                    "export-battle",
                    "state.json",
                    "--codex",
                    "mod",
                    "--save",
                    "campaign.sav",
                    "--map",
                    "multi/test",
                ]
            ).command,
        )


if __name__ == "__main__":
    unittest.main()
