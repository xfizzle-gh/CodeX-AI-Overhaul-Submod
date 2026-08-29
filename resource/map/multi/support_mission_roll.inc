; Shared per-mission arm for the three LIVE support engines:
; enemy_defense_support, defense_support_waves, enemy_attack_support.
; Extracted from attack_support_waves.inc so that file can stay a pack-facing stub.
; Included from dcg_script.inc (pack maps include dcg_script after the wave files;
; triggers evaluate continuously, so include order does not starve the other engines).

			{"support_mission/roll"
				{condition
					{expression "1"}
					{terms
						{"1.cmp_i" {var "support_mission_roll_done$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"set_i" {var "support_mission_roll_done$"} {op "="} {value 1}}
					{"switch"
						{"case"
							{condition {type rand} {value 1.0}}
							{"set_i" {var "support_mission_enabled$"} {op "="} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 8} {title "SUPPORT MISSION ON (100% roll)"}}
								}
								{"default"}
							}
						}
						{"default"
							{"set_i" {var "support_mission_enabled$"} {op "="} {value 0}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 8} {title "SUPPORT MISSION OFF (100% roll)"}}
								}
								{"default"}
							}
						}
					}
				}
			}
