import sys
import pathlib

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent / "tools"))

from verify_waypoint_band import waypoint_names, band_is_free


def test_waypoint_names_parses_named_and_numeric_entries():
    text = """
    {waypoints
        {"attack_support_entry_a"
            {position -5691.72 -800.01 0.00}
            {radius 150}
        }
        {"21"
            {position -6545.48 -920.01 0.00}
            {radius 800}
            {commands
                {"entity_state"
                    {tag_add support_e2_arrival}
                }
            }
        }
    }
    """
    assert waypoint_names(text) == {"attack_support_entry_a", "21"}


def test_waypoint_names_ignores_declarations_outside_the_waypoints_block():
    text = """
    {Human "mp/nato/2022s/usmc_medic" 0xaf24
        {Position -3120 -35100}
    }
    {waypoints
        {"only_this_one"
            {position 0 0 0}
        }
    }
    """
    assert waypoint_names(text) == {"only_this_one"}


def test_existing_band_is_reported_as_occupied(map_files, mod_root):
    result = band_is_free(mod_root, ["21", "22"])
    assert result["21"], "waypoint 21 is known to exist and must be reported occupied"
    assert result["22"], "waypoint 22 is known to exist and must be reported occupied"


def test_chosen_birth_band_is_free(map_files, mod_root):
    result = band_is_free(mod_root, ["31", "32"])
    assert result == {"31": [], "32": []}, f"chosen birth band is not free: {result}"
