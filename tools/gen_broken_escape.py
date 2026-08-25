"""Rewrite ce_broken_behavior_triggers.inc: Broken own-rear escape, no POW."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "resource/map/multi/ce/ce_broken_behavior_triggers.inc"

NEW_COMMENT = """\
; Broken escape: own spawn-side rear. Morale owns movement only while Broken.
; Writers must exclude aio_morale_owned. No player seizure. No POW/surrender.
; One-shot fast retreat: actor_state speed fast -> claim aio_morale_retreat_issued
; -> 0.25s -> dest move to spawn_a/spawn_b from enemy_spawnside$ + current side.
; Arrival at own rear rallies to Shaken and releases ownership.
; Wave tag aio_morale_retreat_wave lets later breaks claim without re-moving runners.
; Hop-in tags aio_morale_needs_resume; hop-out one-shot reclaims wave and re-invokes escape.
"""

TAB = "\t"


def I(n: int, s: str) -> str:
    pad = TAB * n
    return "\n".join(pad + line if line else "" for line in s.splitlines())


def tag(name: str) -> str:
    return "{tag\n\t{tag %s}\n}" % name


def state(name: str) -> str:
    return "{state\n\t{state %s}\n}" % name


def prop_human() -> str:
    return "{prop\n\t{prop human}\n}"


def rel_ally(slot: int) -> str:
    return '{relation\n\t{relation ally}\n\t{player "%d"}\n}' % slot


def selector(select: str, include: list[str], exclude: list[str]) -> str:
    lines = [
        "{selector",
        "\t{source advanced}",
        "\t{group",
        "\t\t{select",
        I(3, tag(select)),
        "\t\t}",
        "\t\t{include",
    ]
    for item in include:
        lines.append(I(3, item))
    lines += ["\t\t}", "\t\t{exclude"]
    for item in exclude:
        lines.append(I(3, item))
    lines += ["\t\t}", "\t}", "}"]
    return "\n".join(lines)


DEAD = [state("dead"), state("inactive")]
CTRL = DEAD + [state("user_control")]
HUMAN_BROKEN = [tag("aio_morale_broken"), prop_human()]


def move_action(kind: str, slot: int, pad: str) -> str:
    if kind == "ally":
        include = HUMAN_BROKEN + [
            tag("aio_morale_retreat_issued"),
            tag("aio_morale_retreat_wave"),
            rel_ally(slot),
        ]
        exclude = list(CTRL)
    else:
        include = HUMAN_BROKEN + [
            tag("aio_morale_retreat_issued"),
            tag("aio_morale_retreat_wave"),
        ]
        exclude = CTRL + [rel_ally(slot)]
    sel = selector("aio_morale_owned", include, exclude)
    target = """\
{target
	{source advanced}
	{group
		{select
			{tag
				{tag spawn_%s}
			}
		}
	}
	{amount 1}
}""" % pad
    return "\n".join(
        [
            '{"action"',
            I(1, sel),
            "\t{drop orders}",
            "\t{action move}",
            I(1, target),
            "}",
        ]
    )


def slot_case(slot: int, ally_pad: str, foe_pad: str) -> str:
    return "\n".join(
        [
            '{"case"',
            '\t{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value %d}}' % slot,
            I(1, move_action("ally", slot, ally_pad)),
            I(1, move_action("foe", slot, foe_pad)),
            "}",
        ]
    )


def spawn_case(op: str, value: int, ally_pad: str, foe_pad: str) -> str:
    cases = [slot_case(s, ally_pad, foe_pad) for s in range(1, 17)]
    default = "\n".join(
        [
            '{"default"',
            I(1, move_action("ally", 1, ally_pad)),
            I(1, move_action("foe", 1, foe_pad)),
            "}",
        ]
    )
    inner = "\n".join(
        ['{"switch"'] + [I(1, c) for c in cases] + [I(1, default), "}"]
    )
    return "\n".join(
        [
            '{"case"',
            '\t{condition {type cmp_i} {var "enemy_spawnside$"} {op "%s"} {value %d}}'
            % (op, value),
            I(1, inner),
            "}",
        ]
    )


def rally_effect(kind: str, slot: int) -> str:
    if kind == "ally":
        include = HUMAN_BROKEN + [
            tag("aio_morale_retreat_issued"),
            tag("aio_morale_at_rear"),
            rel_ally(slot),
        ]
        exclude = list(DEAD)
    else:
        include = HUMAN_BROKEN + [
            tag("aio_morale_retreat_issued"),
            tag("aio_morale_at_rear"),
        ]
        exclude = DEAD + [rel_ally(slot)]
    sel = selector("aio_morale_owned", include, exclude)
    return "\n".join(
        [
            '{"effect"',
            I(1, sel),
            "\t{effect aio_morale_rally}",
            "}",
        ]
    )


def arrive_slot_case(slot: int, want_ally: bool) -> str:
    kind = "ally" if want_ally else "foe"
    return "\n".join(
        [
            '{"case"',
            '\t{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value %d}}' % slot,
            I(1, rally_effect(kind, slot)),
            "}",
        ]
    )


def arrive_spawn_arm(op: str, value: int, want_ally: bool) -> str:
    cases = [arrive_slot_case(s, want_ally) for s in range(1, 17)]
    default = "\n".join(
        [
            '{"default"',
            I(1, rally_effect("ally" if want_ally else "foe", 1)),
            "}",
        ]
    )
    inner = "\n".join(
        ['{"switch"'] + [I(1, c) for c in cases] + [I(1, default), "}"]
    )
    return "\n".join(
        [
            '{"case"',
            '\t{condition {type cmp_i} {var "enemy_spawnside$"} {op "%s"} {value %d}}'
            % (op, value),
            I(1, inner),
            "}",
        ]
    )


def see_actors(pad: str) -> str:
    sel = selector(
        "aio_morale_owned",
        HUMAN_BROKEN + [tag("aio_morale_retreat_issued")],
        CTRL,
    )
    return """\
{"2.see_actors"
%s
	{enemy
		{source advanced}
		{group
			{select
				{tag
					{tag spawn_%s}
				}
			}
		}
	}
	{distance
		{mode near_than}
		{meters 25}
	}
	{detection located}
	{tag
		{"tag pair"}
		{"for selector" aio_morale_at_rear}
	}
}""" % (I(1, sel), pad)


def arrive_trigger(pad: str, ally_when_le1: bool) -> str:
    return """
			{"conquest_enhanced_mechanics/broken/arrive_%s"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
%s
					}
				}
				{actions
					{"switch"
%s
%s
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/arrive_%s"}
					}
				}
			}
""" % (
        pad,
        I(6, see_actors(pad)),
        I(6, arrive_spawn_arm("<=", 1, ally_when_le1)),
        I(6, arrive_spawn_arm("==", 2, not ally_when_le1)),
        pad,
    )


def escape_trigger() -> str:
    cond_sel = selector("aio_morale_owned", HUMAN_BROKEN, CTRL)
    new_sel = selector(
        "aio_morale_owned",
        HUMAN_BROKEN,
        CTRL + [tag("aio_morale_retreat_issued")],
    )
    clear_wave = selector(
        "aio_morale_retreat_wave",
        HUMAN_BROKEN + [tag("aio_morale_retreat_issued")],
        list(DEAD),
    )
    return """
			{"conquest_enhanced_mechanics/broken/escape"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
%s
						}
					}
				}
				{actions
					{"actor_state"
%s
						{drop orders}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{movement
							{speed fast}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 4}
						}
					}
					{"entity_state"
%s
						{tag_add aio_morale_retreat_wave}
					}
					{"entity_state"
%s
						{tag_add aio_morale_retreat_issued}
					}
					{"delay"
						{time 0.25}
					}
					{"switch"
%s
%s
					}
					{"entity_state"
%s
						{tag_remove aio_morale_retreat_wave}
					}
					{"set_i"
						{var "ce_morale_diag_retreat$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/escape"}
					}
				}
			}
""" % (
        I(7, cond_sel),
        I(6, new_sel),
        I(6, new_sel),
        I(6, new_sel),
        I(6, spawn_case("<=", 1, "b", "a")),
        I(6, spawn_case("==", 2, "a", "b")),
        I(6, clear_wave),
    )


def control_hold_trigger() -> str:
    sel = selector(
        "aio_morale_owned",
        HUMAN_BROKEN + [tag("aio_morale_retreat_issued"), state("user_control")],
        list(DEAD),
    )
    return """
			{"conquest_enhanced_mechanics/broken/control_hold"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
%s
						}
					}
				}
				{actions
					{"entity_state"
%s
						{tag_add aio_morale_needs_resume}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/control_hold"}
					}
				}
			}
""" % (
        I(7, sel),
        I(6, sel),
    )


def resume_trigger() -> str:
    sel = selector(
        "aio_morale_needs_resume",
        HUMAN_BROKEN
        + [tag("aio_morale_owned"), tag("aio_morale_retreat_issued")],
        CTRL,
    )
    return """
			{"conquest_enhanced_mechanics/broken/resume"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
%s
						}
					}
				}
				{actions
					{"delay"
						{time 0.5}
					}
					{"actor_state"
%s
						{drop orders}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{movement
							{speed fast}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 4}
						}
					}
					{"entity_state"
%s
						{tag_add aio_morale_retreat_wave}
					}
					{"entity_state"
%s
						{tag_remove aio_morale_needs_resume}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/escape"}
					}
				}
			}
""" % (
        I(7, sel),
        I(6, sel),
        I(6, sel),
        I(6, sel),
    )


def cleanup_trigger() -> str:
    def groups(*tags: str) -> str:
        chunks = []
        for name in tags:
            for st in ("dead", "inactive"):
                chunks.append(
                    """\
{group
	{select
		{tag
			{tag %s}
		}
	}
	{include
		{state
			{state %s}
		}
	}
}"""
                    % (name, st)
                )
        return "\n".join(chunks)

    tags = (
        "aio_morale_owned",
        "aio_morale_broken",
        "aio_morale_retreat_issued",
        "aio_morale_retreat_wave",
        "aio_morale_at_rear",
        "aio_morale_needs_resume",
    )

    def strip(name: str) -> str:
        return """\
{"entity_state"
	{selector
		{source advanced}
%s
	}
	{tag_remove %s}
}""" % (I(2, groups(name)), name)

    return """
			{"conquest_enhanced_mechanics/broken/cleanup_dead"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
%s
							}
						}
					}
				}
				{actions
%s
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/cleanup_dead"}
					}
				}
			}
""" % (
        I(8, groups(*tags)),
        I(5, "\n".join(strip(t) for t in tags)),
    )


def patch_acquire(header_acq: str) -> str:
    surrender_block = """\
										{tag
											{tag aio_morale_surrendering}
										}
"""
    header_acq = header_acq.replace(surrender_block, "")

    marker = "{tag_add aio_morale_regrouping}"
    if marker in header_acq:
        idx = header_acq.find(marker)
        block_start = header_acq.rfind('{"entity_state"', 0, idx)
        block_end = header_acq.find('\t\t\t\t\t{"actor_state"', idx)
        if block_start != -1 and block_end != -1:
            header_acq = header_acq[:block_start] + header_acq[block_end:]

    if "{effect aio_morale_watch_regroup}" in header_acq:
        idx = header_acq.find("{effect aio_morale_watch_regroup}")
        block_start = header_acq.rfind('\t\t\t\t\t{"effect"', 0, idx)
        block_end = header_acq.find('\t\t\t\t\t{"set_i"', idx)
        if block_start != -1 and block_end != -1:
            header_acq = header_acq[:block_start] + header_acq[block_end:]

    owned_excl = """									{tag
										{tag aio_morale_owned}
									}
"""
    issued_excl = """									{tag
										{tag aio_morale_owned}
									}
									{tag
										{tag aio_morale_retreat_issued}
									}
"""
    header_acq = header_acq.replace(owned_excl, issued_excl, 1)

    header_acq = header_acq.replace(
        """						{drop orders}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 4}
						}""",
        """						{drop orders}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{movement
							{speed fast}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 4}
						}""",
        1,
    )
    header_acq = header_acq.replace(
        """						{move_mode free}
						{ai_move
							{mode enable}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 4}
						}""",
        """						{move_mode free}
						{ai_move
							{mode enable}
						}
						{movement
							{speed fast}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 4}
						}""",
        1,
    )
    return header_acq


def main() -> None:
    text = SRC.read_text(encoding="utf-8")
    rest = text.split('{"conquest_enhanced_mechanics/broken/rally"', 1)[0]
    lines = rest.splitlines(True)
    i = 0
    while i < len(lines) and lines[i].startswith(";"):
        i += 1
    header_acq = NEW_COMMENT + patch_acquire("".join(lines[i:]))
    body = (
        header_acq
        + escape_trigger()
        + control_hold_trigger()
        + resume_trigger()
        + arrive_trigger("a", ally_when_le1=False)
        + arrive_trigger("b", ally_when_le1=True)
        + cleanup_trigger()
    )
    while "\n\n\n" in body:
        body = body.replace("\n\n\n", "\n\n")
    SRC.write_text(body, encoding="utf-8")
    print("wrote", SRC, "bytes", SRC.stat().st_size, "lines", body.count("\n"))


if __name__ == "__main__":
    main()
