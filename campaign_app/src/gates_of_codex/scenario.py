from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from .models import CampaignState
from .state_io import from_dict


def load_scenario(path: str | Path, *, campaign_name: str | None = None) -> CampaignState:
    source = Path(path)
    data: dict[str, Any] = json.loads(source.read_text(encoding="utf-8"))
    if campaign_name:
        data["campaign_name"] = campaign_name
    return from_dict(data)
