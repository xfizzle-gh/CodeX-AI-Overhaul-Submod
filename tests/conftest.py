import pathlib
import pytest

MAP_NAMES = [
    "airbase", "border", "europe", "factory", "fields", "fulda", "grassland",
    "industrial", "monastery", "outback", "stasis", "train_station",
    "winds_valley", "woodland",
]


@pytest.fixture(scope="session")
def mod_root() -> pathlib.Path:
    return pathlib.Path(__file__).resolve().parent.parent


@pytest.fixture(scope="session")
def map_files(mod_root: pathlib.Path) -> list[pathlib.Path]:
    paths = [
        mod_root / "resource" / "map" / "multi" / f"dcg_[cwa71]_{name}"
        / "campaign_capture_the_flag.mi"
        for name in MAP_NAMES
    ]
    missing = [str(p) for p in paths if not p.is_file()]
    assert not missing, f"missing map files: {missing}"
    return paths
