from __future__ import annotations

import os
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


@dataclass(frozen=True, slots=True)
class CodeXPaths:
    steam_library: Path | None = None
    game_directory: Path | None = None
    code_x_directory: Path | None = None
    profile_directory: Path | None = None


class CodeXLocator:
    _vdf_path = re.compile(r'"path"\s*"([^"]+)"', re.IGNORECASE)

    def find(self, *, home: str | Path | None = None) -> CodeXPaths:
        home_path = Path(home).expanduser() if home else Path.home()
        libraries = list(self.find_steam_libraries(home_path))
        game_directory = self.find_game_directory(libraries)
        code_x_directory = self.find_code_x_directory(libraries, game_directory)
        profile_directory = self.find_profile_directory(home_path)
        return CodeXPaths(
            steam_library=self._library_for(game_directory, libraries),
            game_directory=game_directory,
            code_x_directory=code_x_directory,
            profile_directory=profile_directory,
        )

    def find_steam_libraries(self, home: Path | None = None) -> Iterable[Path]:
        candidates: list[Path] = []
        env_path = os.environ.get("STEAM_PATH")
        if env_path:
            candidates.append(Path(env_path))

        if os.name == "nt":
            for variable in ("PROGRAMFILES(X86)", "PROGRAMFILES"):
                base = os.environ.get(variable)
                if base:
                    candidates.append(Path(base) / "Steam")
            candidates.extend(
                [Path("C:/Steam"), Path("D:/Steam"), Path("D:/SteamLibrary")]
            )
        else:
            user_home = home or Path.home()
            candidates.extend(
                [
                    user_home / ".steam/steam",
                    user_home / ".local/share/Steam",
                ]
            )

        seen: set[Path] = set()
        roots: list[Path] = []
        for candidate in candidates:
            resolved = candidate.expanduser()
            if resolved in seen or not (resolved / "steamapps").is_dir():
                continue
            seen.add(resolved)
            roots.append(resolved)
            vdf = resolved / "steamapps/libraryfolders.vdf"
            if vdf.is_file():
                try:
                    text = vdf.read_text(encoding="utf-8", errors="replace")
                except OSError:
                    continue
                for raw in self._vdf_path.findall(text):
                    library = Path(raw.replace("\\\\", "\\"))
                    if library not in seen and (library / "steamapps").is_dir():
                        seen.add(library)
                        roots.append(library)
        return roots

    @staticmethod
    def find_game_directory(libraries: Iterable[Path]) -> Path | None:
        for library in libraries:
            candidate = library / "steamapps/common/Call to Arms - Gates of Hell"
            if (candidate / "call_to_arms.exe").is_file() or (candidate / "resource").is_dir():
                return candidate
        return None

    def find_code_x_directory(
        self, libraries: Iterable[Path], game_directory: Path | None = None
    ) -> Path | None:
        candidates: list[Path] = []
        env_path = os.environ.get("CODEX_MOD_PATH")
        if env_path:
            candidates.append(Path(env_path))
        for library in libraries:
            workshop = library / "steamapps/workshop/content/400750"
            if workshop.is_dir():
                candidates.extend(path for path in workshop.iterdir() if path.is_dir())
        if game_directory:
            local_mods = game_directory / "mods"
            if local_mods.is_dir():
                candidates.extend(path for path in local_mods.iterdir() if path.is_dir())

        scored: list[tuple[int, Path]] = []
        for candidate in candidates:
            mod_info = candidate / "mod.info"
            resource = candidate / "resource"
            if not mod_info.is_file() or not resource.is_dir():
                continue
            try:
                text = mod_info.read_text(encoding="utf-8", errors="replace").lower()
            except OSError:
                continue
            score = 0
            if "code:x" in text:
                score += 100
            if "codex" in text:
                score += 80
            if "modern" in text:
                score += 10
            if (resource / "script/multiplayer/units/nato/2022s.nato.lua").is_file():
                score += 50
            if score:
                scored.append((score, candidate))
        return max(scored, default=(0, None), key=lambda item: item[0])[1]

    @staticmethod
    def find_profile_directory(home: Path | None = None) -> Path | None:
        user_home = home or Path.home()
        roots = [user_home / "Documents/My Games/gates of hell/profiles"]
        local_app_data = os.environ.get("LOCALAPPDATA")
        if local_app_data:
            roots.extend(
                [
                    Path(local_app_data) / "Digitalmindsoft/Gates of Hell/profiles",
                    Path(local_app_data) / "Digitalmindsoft/Call to Arms - Gates of Hell/profiles",
                ]
            )
        candidates: list[Path] = []
        for root in roots:
            if not root.is_dir():
                continue
            for profile in root.iterdir():
                if profile.is_dir() and (profile / "campaign").is_dir():
                    candidates.append(profile)
        if not candidates:
            return None
        return max(candidates, key=lambda path: path.stat().st_mtime)

    @staticmethod
    def _library_for(game_directory: Path | None, libraries: Iterable[Path]) -> Path | None:
        if game_directory is None:
            return None
        for library in libraries:
            try:
                game_directory.relative_to(library)
            except ValueError:
                continue
            return library
        return None
