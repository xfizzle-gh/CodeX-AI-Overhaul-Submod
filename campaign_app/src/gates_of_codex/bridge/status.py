from __future__ import annotations

import random
import re
import time
from dataclasses import dataclass, field

from ..models import PendingBattle


STATUS_TEMPLATE = """{saveinfo
\t{version 9}
\t{gameVersion \"%GAME_VERSION%\"}
\t{timestamp %TIMESTAMP%}
\t{mp %MP%}
\t{sp %SP%}
\t{ap %AP%}
\t{rp %RP%}
\t{seed %SEED%}
\t{name \"%SAVE_NAME%\"}
\t{army %ARMY%}
\t{enemyArmy %ENEMY_ARMY%}
\t{difficulty %DIFFICULTY%}
\t{duration %DURATION%}
\t{resources %RESOURCES%}
\t{fogofwar %FOG_OF_WAR%}
\t{manualControlMode %MANUAL_CONTROL_MODE%}
\t{selectedMapPoint %SELECTED_MAP_POINT%}
\t%PLAYER_STANCE%
\t{region %REGION%}
\t{playedGames %PLAYED_GAMES%}
\t{wonGames %WON_GAMES%}
\t%UNLOCKED_RESEARCH_BLOCK%
\t{mapPoints
\t\t{
\t\t\t{name hq_a}
\t\t\t{landscape %LANDSCAPE%}
\t\t\t{gamemode %GAMEMODE%}
\t\t\t{ownerTeam a}
\t\t\t{adjacentMaps {\"hq_b\"}}
\t\t\t{risk standard}
\t\t\t{reward none}
\t\t\t{map \"%MAP_STRING%\"}
\t\t\t{texmod %TEXMOD%}
\t\t}
\t\t{
\t\t\t{name hq_b}
\t\t\t{landscape %LANDSCAPE%}
\t\t\t{gamemode %GAMEMODE%}
\t\t\t{ownerTeam b}
\t\t\t{adjacentMaps {\"hq_a\"}}
\t\t\t{risk standard}
\t\t\t{reward none}
\t\t\t{map \"%MAP_STRING%\"}
\t\t\t{texmod %TEXMOD%}
\t\t}
\t}
\t{roundsHistory}
}
"""


@dataclass(slots=True)
class BattleStatusOptions:
    map_string: str
    game_version: str = "1.062.0"
    save_name_prefix: str = "GatesOfCodeX_Battle"
    difficulty: str = "normal"
    duration: str = "standard"
    resources: str = "standard"
    fog_of_war: str = "fog_realistic"
    manual_control_mode: str = "direct"
    region: str = "ostfront"
    landscape: str = "wood"
    texmod: str = "camo"
    game_mode: str = "campaign_capture_the_flag"
    selected_map_point: str = "hq_b"
    mp: int = 400
    sp: int = 0
    ap: int = 0
    rp: int = 0
    played_games: int = 0
    won_games: int = 0
    unlocked_research: list[str] = field(default_factory=list)
    timestamp: int | None = None
    seed: int | None = None


@dataclass(frozen=True, slots=True)
class StatusResult:
    played_games: int
    won_games: int

    def player_won_since(self, previous: "StatusResult") -> bool:
        return self.won_games > previous.won_games


class StatusBuilder:
    _played = re.compile(r"\{playedGames\s+(\d+)\}")
    _won = re.compile(r"\{wonGames\s+(\d+)\}")

    def build(self, pending: PendingBattle, options: BattleStatusOptions) -> str:
        if not options.map_string.strip():
            raise ValueError("A GoH map string is required")
        player_army = pending.player_faction.value
        enemy_army = (
            pending.defender_faction.value
            if pending.player_is_attacker
            else pending.attacker_faction.value
        )
        timestamp = options.timestamp if options.timestamp is not None else int(time.time())
        seed = options.seed if options.seed is not None else random.randint(1, 2_147_483_647)
        save_name = f"{options.save_name_prefix}_{pending.battle_id}"
        replacements = {
            "%GAME_VERSION%": options.game_version,
            "%TIMESTAMP%": str(timestamp),
            "%MP%": str(options.mp),
            "%SP%": str(options.sp),
            "%AP%": str(options.ap),
            "%RP%": str(options.rp),
            "%SEED%": str(seed),
            "%SAVE_NAME%": save_name,
            "%ARMY%": player_army,
            "%ENEMY_ARMY%": enemy_army,
            "%DIFFICULTY%": options.difficulty,
            "%DURATION%": options.duration,
            "%RESOURCES%": options.resources,
            "%FOG_OF_WAR%": options.fog_of_war,
            "%MANUAL_CONTROL_MODE%": options.manual_control_mode,
            "%SELECTED_MAP_POINT%": options.selected_map_point,
            "%PLAYER_STANCE%": "{attacking}" if pending.player_is_attacker else "{attacking 0}",
            "%REGION%": options.region,
            "%PLAYED_GAMES%": str(options.played_games),
            "%WON_GAMES%": str(options.won_games),
            "%UNLOCKED_RESEARCH_BLOCK%": self._research_block(options.unlocked_research),
            "%LANDSCAPE%": options.landscape,
            "%GAMEMODE%": options.game_mode,
            "%MAP_STRING%": self._escape(options.map_string),
            "%TEXMOD%": options.texmod,
        }
        text = STATUS_TEMPLATE
        for marker, value in replacements.items():
            text = text.replace(marker, value)
        unresolved = sorted(set(re.findall(r"%[A-Z0-9_]+%", text)))
        if unresolved:
            raise ValueError(f"Unresolved status placeholders: {', '.join(unresolved)}")
        return text

    def parse_result(self, status_text: str) -> StatusResult:
        played = self._played.search(status_text)
        won = self._won.search(status_text)
        if not played or not won:
            raise ValueError("GoH status is missing playedGames or wonGames")
        return StatusResult(played_games=int(played.group(1)), won_games=int(won.group(1)))

    @staticmethod
    def _research_block(keys: list[str]) -> str:
        unique = sorted({key.strip() for key in keys if key.strip()})
        if not unique:
            return "{unlockedResearch\n\t}"
        lines = ["{unlockedResearch"]
        lines.extend(f'\t\t{{\"{StatusBuilder._escape(key)}\"}}' for key in unique)
        lines.append("\t}")
        return "\n".join(lines)

    @staticmethod
    def _escape(value: str) -> str:
        return value.replace("\\", "\\\\").replace('"', '\\"').replace("\r", "").replace("\n", "\\n")
