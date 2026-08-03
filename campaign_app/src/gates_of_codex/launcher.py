from __future__ import annotations

import subprocess
from pathlib import Path
from typing import Sequence


EXECUTABLE_CANDIDATES = (
    "call_to_arms.exe",
    "binaries/x64/call_to_arms.exe",
    "binaries/x64/call_to_arms_x64.exe",
)


def find_game_executable(game_directory: str | Path) -> Path:
    root = Path(game_directory)
    for relative in EXECUTABLE_CANDIDATES:
        candidate = root / relative
        if candidate.is_file():
            return candidate
    raise FileNotFoundError(f"Could not find the Gates of Hell executable under {root}")


def launch_game(
    game_directory: str | Path,
    *,
    arguments: Sequence[str] = (),
) -> subprocess.Popen[bytes]:
    executable = find_game_executable(game_directory)
    return subprocess.Popen(
        [str(executable), *arguments],
        cwd=executable.parent,
        close_fds=True,
    )
