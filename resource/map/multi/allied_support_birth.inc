; Allied support command - actor birth.
;
; Births one support actor by CLONING a parked prototype from the existing off-map pool
; onto birth pad 31 (side a) or 32 (side b). The pad's own {commands} block tags the
; ARRIVING actor - see the pads in each campaign_capture_the_flag.mi. That is the entire
; reason this works: a freshly created entity's provenance is invisible to every selector
; this format can express, so nothing here ever tries to select one.
;
; Prototype originals never move, so birth is repeatable and consumes nothing from the
; player's roster. Clones inherit no tags at all - neither runtime tags nor static {Tags} -
; so the pool count in attack_support_templates.inc is unaffected.
;
; FAIL CLOSED. If the arrival is not tagged, the wave ends with fail code 2. There is no
; fallback, and in particular nothing here may recover by selecting a recently-appeared or
; otherwise "new-looking" human. That recovery shape produces telemetry indistinguishable
; from success while proving nothing, which is the exact false positive #112 exposed.
;
; Gated by allied_support_cmd_enable$ (default 0) and interlocked with
; allied_support_cmd_spawn_probe$: if both are 1 this refuses to run with fail code 5, so a
; fog-of-war observation is always attributable to one mechanism.
;
; Birth fires ONCE per mission, deliberately not from the wave clock. The deliverable is a
; fog-of-war reading on a single isolated actor; driving this from the schedule, pool
; selection, level budget and live-unit cap would give a failure five explanations instead
; of one. Wave integration belongs to the commander work, after birth is proven.

	{"allied_support_cmd/birth_init"
		{condition {terms {"1.event" {id "init"}}}}
		{actions
			{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 0}}
			{"set_i" {var "allied_support_cmd_fail$"} {op "="} {value 0}}
			{"set_i" {var "allied_support_cmd_fow_continue$"} {op "="} {value 0}}
			{"set_i" {var "allied_support_cmd_gate_auto$"} {op "="} {value 0}}
		}
	}

	{"allied_support_cmd/birth_dispatch"
		{condition
			{terms
				{"1.cmp_i" {var "allied_support_cmd_enable$"} {op "=="} {value 1}}
				{"2.cmp_i" {var "allied_support_cmd_stage$"} {op "=="} {value 0}}
				{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
			}
		}
		{actions
			{"switch"
				{"case"
					{condition {type cmp_i} {var "allied_support_cmd_spawn_probe$"} {op "=="} {value 1}}
					{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 90}}
					{"set_i" {var "allied_support_cmd_fail$"} {op "="} {value 5}}
				}
				{"default"
					{"switch"
						{"case"
							{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
							{"actor_to_waypoint"
								{selector
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{select {tag {tag attack_support_inf_usmc}}}
										{include {tag {tag hidden}}}
									}
									{amount 1}
								}
								{waypoint "32"}
								{clone}
								{approach "safe teleport & rotate"}
							}
						}
						{"default"
							{"actor_to_waypoint"
								{selector
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{select {tag {tag attack_support_inf_usmc}}}
										{include {tag {tag hidden}}}
									}
									{amount 1}
								}
								{waypoint "31"}
								{clone}
								{approach "safe teleport & rotate"}
							}
						}
					}
					{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 10}}
				}
			}
		}
	}

	{"allied_support_cmd/birth_verify"
		{condition
			{terms
				{"1.cmp_i" {var "allied_support_cmd_stage$"} {op "=="} {value 10}}
			}
		}
		{actions
			{"delay" {time 3.0}}
			{"switch"
				{"case"
					{condition
						{type entities}
						{selector
							{ignore_captured_by_user 0}
							{tag allied_support_cmd_fresh}
						}
						{count {op ">"} {value 0}}
					}
					{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 20}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {mode show} {text "ALLIED SUPPORT: arrival tagged, stage 20"}}
						}
						{"default"}
					}
				}
				{"default"
					{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 90}}
					{"set_i" {var "allied_support_cmd_fail$"} {op "="} {value 2}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {mode show} {text "ALLIED SUPPORT: FAIL 2 arrival not tagged"}}
						}
						{"default"}
					}
				}
			}
		}
	}
