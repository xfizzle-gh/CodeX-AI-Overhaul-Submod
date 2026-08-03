from __future__ import annotations

from dataclasses import asdict, dataclass
from pathlib import Path

from .codex.catalog import CodeXCatalogScanner
from .codex.locator import CodeXLocator, CodeXPaths
from .launcher import find_game_executable


@dataclass(frozen=True, slots=True)
class DiagnosticReport:
    game_directory: str = ""
    game_executable: str = ""
    code_x_directory: str = ""
    profile_directory: str = ""
    catalog_units: int = 0
    catalog_files: int = 0
    warnings: tuple[str, ...] = ()

    def to_dict(self) -> dict[str, object]:
        return asdict(self)


def diagnose(
    *,
    game_directory: str | Path | None = None,
    code_x_directory: str | Path | None = None,
    profile_directory: str | Path | None = None,
) -> DiagnosticReport:
    located: CodeXPaths = CodeXLocator().find()
    game = Path(game_directory) if game_directory else located.game_directory
    codex = Path(code_x_directory) if code_x_directory else located.code_x_directory
    profile = Path(profile_directory) if profile_directory else located.profile_directory
    warnings: list[str] = []

    executable = ""
    if game:
        try:
            executable = str(find_game_executable(game))
        except FileNotFoundError as exc:
            warnings.append(str(exc))
    else:
        warnings.append("Gates of Hell installation was not found")

    unit_count = 0
    file_count = 0
    if codex:
        try:
            catalog = CodeXCatalogScanner().scan(codex)
            unit_count = len(catalog.units)
            file_count = catalog.scanned_files
            warnings.extend(catalog.warnings)
        except (FileNotFoundError, OSError, ValueError) as exc:
            warnings.append(str(exc))
    else:
        warnings.append("Code:X installation was not found")

    if not profile:
        warnings.append("Gates of Hell profile directory was not found")

    return DiagnosticReport(
        game_directory=str(game or ""),
        game_executable=executable,
        code_x_directory=str(codex or ""),
        profile_directory=str(profile or ""),
        catalog_units=unit_count,
        catalog_files=file_count,
        warnings=tuple(warnings),
    )
