; Defensive support waves - the human defender's AI teammate reinforcement engine.
; Quadrant 2 of the four-way support parity: attack_support_waves.inc gives the human
; ATTACKER a teammate, enemy_defense_support.inc gives the AI defender a garrison on an
; attack mission, enemy_attack_support.inc gives the AI attacker pressure on a defence
; mission, and this file gives the human DEFENDER a friendly AI defender that actually
; reinforces the flags it is supposed to hold.
;
; Included by all fourteen CWA campaign_capture_the_flag.mi maps. Provably inert on a
; human-ATTACK mission: every single trigger carries user_is_defender$ == 1.
; garrison_init waits only for identity (id_defenderbot + enemy_spawnside).
; Edge-wave init also waits for prep_inform$ == 1 so marches do not start mid-placement.
;
; DELIVERY (proven, do not deviate): the off-map real-breed prototypes in
; attack_support_templates.inc are MOVED to the defender-side entry waypoint with
; {"placement"} and promoted in place. Never cloned: three promote designs each matched
; zero freshly created entities, because a new entity's provenance is invisible to every
; selector this format can express.
;
; POOL SHARING, deliberate and safe. This engine claims from the SAME parked NATO pool
; the attack-support engine uses. The two can never contend: attack support gates every
; trigger on user_is_defender$ == 0 and this engine gates every trigger on
; user_is_defender$ == 1, and a mission has exactly one value of that var. One mission
; therefore only ever has one claimant, so no budget arithmetic has to be shared and no
; second set of prototypes has to be parked. A claim strips the pool tag it took from,
; so the pool tag still means "still parked and available" for whichever engine is live.
;
; OWNERSHIP: resolve defense_support_owner$ then {player} switch 1-16 on that var.
; Prefer id_defenderbot$ if 1-16 and != id_1st_enemy$ (CWA 2v2 friendly AI defender).
; On vanilla 1v1 DefenderBotId is often the only bot = the enemy: REJECT it and fall
; back to id_1st_player$. If the human is 0, SKIP the transfer (stay P0; P0 vs P0
; is friendly). NEVER assign to id_1st_enemy$. NEVER guess player 3 in default.
; The engine will not accept a var in a {player} node. init logs the resolved
; slot with a per-value diagnostic timer so a live run pins the real number.
;
; FACTION: follows user_nation$
; AIRMOBILE PARITY (post-approval): deep LZ pads; NO aircraft entity (narrative only). (attack_support_air_*) on defender side.
; Test: attack_support_air_test$=1 -> defense_support/air_test at +30s (no prep wait).
; Ship: set air_test$=0; L2+ random airmobile via cmd 18. (1 rusa / 2 ukr / 3 nato / 4 prc fold). NATO keeps the
; specialty USMC/1AD/ARF/pzgren mix; other nations use ally_sup_* line/wpn pools.
;
; PREP PHASE. Edge reinforcement waves wait for prep_inform$ so they do not walk in
; during the player's placement. GARRISON + PATROL arm as soon as identity is ready
; (no prep wait) so the map feels alive while you dig in.
;
; WHAT ARRIVES, AND WHEN
;   garrison      ~6-10s after mission identity, one 4-man team per ACTIVE flag
;                 (not from wave budget); hold groups start patrolling immediately
;   patrols       hold groups re-order every ~90-150s across active flags + spare flags
;   opening wave  25/32/40s AFTER prep ends, one 4-man rifle team from the map edge
;   recurring     self-re-arming clock, randomized 110/145/170/200/230s cadence
;   budget        defense_level$ (CE campaign progression, 1-3): L1 4 waves, L2 6, L3 8,
;                 including the opening wave. Level 0 falls back to L1. defense_level$ is
;                 computed on defence missions too - CE's set_defense_level trigger is
;                 gated only on defense_level$ == 0 and carries its own
;                 user_is_defender$ == 1 branch, so it is not attack-only.
;   live cap      more than 14 live def_sup_src infantry defers the cycle WITHOUT
;                 consuming a wave, exactly as the attack-support cap does.
;
; COMPOSITION POOL, one drawn at random per wave, widening with the level:
;   1 rifle    4 USMC infantry     L1 L2 L3   (also the pool-short fallback)
;   2 mech     4 1AD infantry      L1 L2 L3
;   3 weapons  6 German pzgren        L2 L3
; Infantry only. The defender bot buys its own armour through the purchase economy and a
; second armour source unbalances it, which is the same call enemy_defense_support.inc
; made. The pzgren pool is only twelve deep, so at L3 it fields two draws and then the
; pool-short fallback steps the pick down to the always-deepest USMC team - a graceful
; path, not an error.
;
; WHAT THEY DO. Reinforcements MOVE in at the defender's own map edge, advance on one of
; the claimed ACTIVE flag points and take cover when they get there: this is a defence,
; so arrivals dig in rather than keep pushing. Three hold groups then re-order on their
; own modest 90-150s ladders, redistributing across the active flags so a wiped flag gets
; re-covered and the whole force does not sit on one objective. no_retreat is OFF - these
; are defenders that may give ground, not a suicide push.
;
; DIAGNOSTICS / support_debug$ - the ship toggle. Every on-screen {"timer" ...} line in
; this file is a developer diagnostic, never player-facing text, and every one of them is
; wrapped in the same gate:
;   {"switch"
;     {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} <timer> }
;     {"default"}
;   }
; support_debug$ is declared in dcg_vars.inc and nothing ever writes it, so it reads 0 and
; the shipped default is OFF: a player sees zero support timers on any of the fourteen maps.
; Flipping that one var to 1 - a single {"set_i" {var "support_debug$"} {op "="} {value 1}}
; anywhere that runs, or sc:SetVar("support_debug", 1) from Lua - restores every diagnostic
; in all four engines at once. Never add an ungated timer: tools/deploy_attack_support_probe.ps1
; and the test suite both fail the build the moment one appears.
; PLAYER ANNOUNCEMENTS / support_announce$ - separate gate (default ON via init).
; Player-facing radio uses support_announce$ + {"talk"} (soldier portrait), never support_debug$.
;
; ENTRY POINTS / triple pads. Each side's map-edge arrival is THREE waypoints, not one:
; attack_support_entry_a1/a2/a3 and b1/b2/b3, an equilateral triangle of side ~27m whose
; first point sits exactly on that side's spawn centroid. Point 1 is the same coordinate
; the legacy attack_support_entry_a / _b still carry, so those names survive as aliases
; and the patrol / roam move orders that use them are unchanged. All six are (re)built
; from the repo maps by tools/deploy_attack_support_probe.ps1, idempotently.
;
; ds_entry_next bumps defense_support_entry_rr$ (1 -> 2 -> 3 -> 1) once per placement batch, at the top
; of ds_place_at_entry, and the placement site is a three-case cascade on it. So consecutive
; batches never land on the same pad - including G1 and G2 of one wave, which is what was
; dropping bodies on top of each other and killing them on arrival.

			; Rotating entry point, bumped once per placement batch so no two
			; consecutive batches land on the same pad - including G1 and G2 of a
			; single wave, which is what was stacking bodies on top of each other.
			(define "ds_entry_next"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_entry_rr$"} {op ">="} {value 3}}
						{"set_i" {var "defense_support_entry_rr$"} {op "="} {value 1}}
					}
					{"default"
						{"set_i" {var "defense_support_entry_rr$"} {op "+"} {value 1}}
					}
				}
			)

			; Player-facing radio talk (support_announce$ default ON via init).
			; Stock {"talk"} so a live player soldier portrait delivers the line.
			(define "ds_announce_wave"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/support/wave_inbound"}
						}
					}
					{"default"}
				}
			)
			(define "ds_announce_defense"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/support/defense_reinforced"}
						}
					}
					{"default"}
				}
			)
			
			(define "ds_announce_airborne"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}}
								{"talk"
									{selector
										{ignore_captured_by_user 0}
										{amount 1}
										{tag player}
										{zone "gamezone"}
										{type human}
										{state "not dead"}
									}
									{text "mission/multi/support/airborne_inbound_rusa"}
								}
							}
							{"case"
								{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}}
								{"talk"
									{selector
										{ignore_captured_by_user 0}
										{amount 1}
										{tag player}
										{zone "gamezone"}
										{type human}
										{state "not dead"}
									}
									{text "mission/multi/support/airborne_inbound_ukr"}
								}
							}
							{"case"
								{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 4}}
								{"talk"
									{selector
										{ignore_captured_by_user 0}
										{amount 1}
										{tag player}
										{zone "gamezone"}
										{type human}
										{state "not dead"}
									}
									{text "mission/multi/support/airborne_inbound_prc"}
								}
							}
							{"default"
								{"talk"
									{selector
										{ignore_captured_by_user 0}
										{amount 1}
										{tag player}
										{zone "gamezone"}
										{type human}
										{state "not dead"}
									}
									{text "mission/multi/support/airborne_inbound_nato"}
								}
							}
						}
					}
					{"default"}
				}
			)

			(define "ds_announce_exhausted"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/support/waves_exhausted"}
						}
					}
					{"default"}
				}
			)

			; Delivery point: the DEFENDER's own map edge, which is the side the enemy
			; is NOT on. enemy_spawnside$ is published from conquest.lua's
			; mission-authority branch and on a defence mission the mission authority
			; is the enemy bot, i.e. the ATTACKER - so side 1 (a) means the attacker is
			; on a and the player/defender edge is b. That is the same reading
			; attack_support_waves.inc uses and the opposite of the reading
			; enemy_defense_support.inc uses, which delivers to the enemy's own edge.
			; place$ 1/2/3 = active flag af1/2/3 (garrison). place$ 0 = map-edge entry.
			(define "ds_place_one"
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag def_sup_deploy}}}
							{exclude {tag {tag def_sup_placed}}}
						}
						{amount 1}
					}
					{tag_add def_sup_place_one}
				}
				{"delay" {time 0.05}}
				{"switch"
					{"case"
						{condition {type entities} {selector {tag def_sup_place_one}}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "defense_support_place$"} {op "=="} {value 1}}
								{"placement"
									{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
									{target {ignore_captured_by_user 0} {tag def_sup_af1}}
								}
							}
							{"case"
								{condition {type cmp_i} {var "defense_support_place$"} {op "=="} {value 2}}
								{"placement"
									{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
									{target {ignore_captured_by_user 0} {tag def_sup_af2}}
								}
							}
							{"case"
								{condition {type cmp_i} {var "defense_support_place$"} {op "=="} {value 3}}
								{"placement"
									{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
									{target {ignore_captured_by_user 0} {tag def_sup_af3}}
								}
							}
							{"default"
								{"switch"
									{"case"
										{condition {type cmp_i} {var "defense_support_use_air$"} {op "=="} {value 1}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
												{"placement"
													{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
													{target_waypoint "attack_support_air_b1"}
												}
											}
											{"case"
												{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
												{"placement"
													{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
													{target_waypoint "attack_support_air_a1"}
												}
											}
											{"default"
												{"placement"
													{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
													{target_waypoint "attack_support_air_b1"}
												}
											}
										}
									}
									{"default"
								{"switch"
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "defense_support_entry_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
													{target_waypoint "attack_support_rear_b2"}
												}
											}
											{"case"
												{condition {type cmp_i} {var "defense_support_entry_rr$"} {op "=="} {value 3}}
												{"placement"
													{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
													{target_waypoint "attack_support_rear_b3"}
												}
											}
											{"default"
												{"placement"
													{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
													{target_waypoint "attack_support_rear_b1"}
												}
											}
										}
									}
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "defense_support_entry_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
													{target_waypoint "attack_support_rear_a2"}
												}
											}
											{"case"
												{condition {type cmp_i} {var "defense_support_entry_rr$"} {op "=="} {value 3}}
												{"placement"
													{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
													{target_waypoint "attack_support_rear_a3"}
												}
											}
											{"default"
												{"placement"
													{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
													{target_waypoint "attack_support_rear_a1"}
												}
											}
										}
									}
									{"default"
										{"switch"
											{"case"
												{condition {type cmp_i} {var "defense_support_entry_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
													{target_waypoint "attack_support_rear_b2"}
												}
											}
											{"case"
												{condition {type cmp_i} {var "defense_support_entry_rr$"} {op "=="} {value 3}}
												{"placement"
													{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
													{target_waypoint "attack_support_rear_b3"}
												}
											}
											{"default"
												{"placement"
													{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_place_one}}}}}
													{target_waypoint "attack_support_rear_b1"}
												}
											}
										}
									}
								}
									}
								}
							}
						}
						{"entity_state"
							{selector {tag def_sup_place_one}}
							{tag_add def_sup_placed}
							{tag_remove def_sup_place_one}
						}
						{"delay" {time 0.2}}
					}
					{"default"}
				}
			)

			(define "ds_place_at_entry"
				("ds_resolve_owner")
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_owner_fail$"} {op "=="} {value 1}}
					}
					{"default"
						("ds_entry_next")
						("ds_place_one")
						("ds_place_one")
						("ds_place_one")
						("ds_place_one")
						("ds_place_one")
						("ds_place_one")
						("ds_place_one")
						("ds_place_one")
						{"entity_state"
							{selector {tag def_sup_placed}}
							{tag_remove def_sup_placed}
						}
					}
				}
			)

			; Safe owner for FRIENDLY defense support. Never the enemy, never a guessed slot.
			; Prefer id_defenderbot$ if 1-16 and != id_1st_enemy$ (CWA 2v2 ally defender).
			; On vanilla 1v1 DefenderBotId is often the ONLY bot = the enemy: reject it.
			; Else id_1st_player$. If the human is 0, stay P0 (P0 vs P0 is friendly).
			; NEVER assign to id_1st_enemy$. NEVER guess player 3 in a default branch.
			(define "ds_resolve_owner"
				{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 0}}
				{"set_i" {var "defense_support_owner$"} {op "="} {value 0}}
				{"switch"
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 1}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 1}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 2}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 2}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 2}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 3}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 3}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 3}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 4}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 4}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 4}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 5}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 5}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 5}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 6}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 6}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 6}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 7}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 7}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 7}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 8}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 8}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 8}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 9}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 9}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 9}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 10}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 10}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 10}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 11}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 11}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 11}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 12}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 12}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 12}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 13}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 13}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 13}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 14}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 14}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 14}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 15}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 15}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 15}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 16}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 16}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 16}}
							}
						}
					}
					{"default"}
				}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_owner$"} {op ">"} {value 0}}
					}
					{"default"
						{"switch"
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 1}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 1}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 1}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 2}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 2}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 2}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 3}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 3}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 3}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 4}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 4}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 4}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 5}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 5}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 5}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 6}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 6}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 6}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 7}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 7}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 7}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 8}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 8}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 8}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 9}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 9}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 9}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 10}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 10}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 10}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 11}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 11}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 11}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 12}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 12}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 12}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 13}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 13}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 13}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 14}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 14}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 14}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 15}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 15}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 15}}
							}
						}
					}
					{"case" {condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 16}}
						{"switch"
							{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 16}}
								{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "defense_support_owner$"} {op "="} {value 16}}
							}
						}
					}
							{"default"}
						}
					}
				}
			)

			(define "ds_own_to_defenderbot"
				("ds_resolve_owner")
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_owner_fail$"} {op "=="} {value 1}}
					}
					{"default"
						{"switch"
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 1}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 2}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 3}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 4}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 5}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 6}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 7}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 8}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 9}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 10}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 11}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 12}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 13}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 14}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 15}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 16}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}} {operation set} {player "16"}}}
							{"default"}
						}
					}
				}
			)

			; Diagnostic only, fired once from init. Timer titles cannot interpolate a
			; var, so the resolved defender-bot slot is reported by spelling out the
			; sixteen possibilities. A live run reads the real number off this line.
			(define "ds_report_owner"
				("ds_resolve_owner")
				{"switch"
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 1}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 1"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 2}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 2"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 3}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 3"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 4}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 4"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 5}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 5"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 6}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 6"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 7}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 7"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 8}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 8"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 9}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 9"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 10}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 10"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 11}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 11"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 12}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 12"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 13}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 13"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 14}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 14"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 15}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 15"}}} {"default"}}}
					{"case" {condition {type cmp_i} {var "defense_support_owner$"} {op "=="} {value 16}} {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 16"}}} {"default"}}}
					{"default" {"switch" {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - UNRESOLVED"}}} {"default"}}}
				}
			)

			; Anchors, claimed once: flag activation is fixed for the mission. af1..af3
			; are the ACTIVE flag points, shuffled - a mission activates only ~2 of a
			; map's 2-5 flag_point entities and a squad sent to a dead objective just
			; stands there. Capture points are addressed as {tag flag}: the fpc1..fpc5
			; tags are missing from one of the fourteen maps entirely.
			(define "ds_claim_anchors"
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add def_sup_af1}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
								{tag {tag def_sup_af1}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add def_sup_af2}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
								{tag {tag def_sup_af1}}
								{tag {tag def_sup_af2}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add def_sup_af3}
				}
				{"delay" {time 0.05}}
				; Spare / inactive flags = roam anchors for patrol peels.
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{tag {tag def_sup_af1}}
								{tag {tag def_sup_af2}}
								{tag {tag def_sup_af3}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add def_sup_r1}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{tag {tag def_sup_af1}}
								{tag {tag def_sup_af2}}
								{tag {tag def_sup_af3}}
								{tag {tag def_sup_r1}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add def_sup_r2}
				}
				{"delay" {time 0.05}}
			)

			; Player nation -> faction_support_army$ (1 Kor / 3 ARF, shared with attack).
			(define "ds_resolve_army"
				{"switch"
					{"case" {condition {type cmp_i} {var "user_nation$"} {op "=="} {value 2}} {"set_i" {var "faction_support_army$"} {op "="} {value 3}}}
					{"case" {condition {type cmp_i} {var "user_nation$"} {op "=="} {value 6}} {"set_i" {var "faction_support_army$"} {op "="} {value 1}}}
					{"case" {condition {type cmp_i} {var "user_nation$"} {op "=="} {value 1}} {"set_i" {var "faction_support_army$"} {op "="} {value 1}}}
					{"case" {condition {type cmp_i} {var "user_nation$"} {op "=="} {value 5}} {"set_i" {var "faction_support_army$"} {op "="} {value 1}}}
					{"case" {condition {type cmp_i} {var "user_nation$"} {op "=="} {value 8}} {"set_i" {var "faction_support_army$"} {op "="} {value 1}}}
					{"case" {condition {type cmp_i} {var "user_nation$"} {op "=="} {value 3}} {"set_i" {var "faction_support_army$"} {op "="} {value 3}}}
					{"case" {condition {type cmp_i} {var "user_nation$"} {op "=="} {value 4}} {"set_i" {var "faction_support_army$"} {op "="} {value 3}}}
					{"case" {condition {type cmp_i} {var "user_nation$"} {op "=="} {value 7}} {"set_i" {var "faction_support_army$"} {op "="} {value 3}}}
					{"default"
						{"switch"
							{"case" {condition {type cmp_i} {var "user_nation_hint$"} {op "=="} {value 1}} {"set_i" {var "faction_support_army$"} {op "="} {value 1}}}
							{"default" {"set_i" {var "faction_support_army$"} {op "="} {value 3}}}
						}
					}
				}
			)

			(define "ds_claim_line"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_place$"} {op ">"} {value 0}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_%0_line}}}} {amount 4}}
							{tag_add def_sup_deploy}
							{tag_remove ally_sup_%0_line}
						}
					}
					{"case"
						{condition {type cmp_i} {var "defense_support_wave_num$"} {op "=="} {value 1}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_%0_line}}}} {amount 4}}
							{tag_add def_sup_deploy}
							{tag_remove ally_sup_%0_line}
						}
					}
					{"default"
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_%0_line}}}} {amount 6}}
							{tag_add def_sup_deploy}
							{tag_remove ally_sup_%0_line}
						}
					}
				}
			)
			(define "ds_poke_faction_line"
				{"trigger" {name "defense_support/ally_rusa_line"}}
				{"trigger" {name "defense_support/ally_nato_line"}}
			)
			(define "ds_poke_faction_air"
				{"trigger" {name "defense_support/ally_rusa_air"}}
				{"trigger" {name "defense_support/ally_nato_air"}}
			)
			(define "ds_poke_faction_veh"
				{"trigger" {name "defense_support/ally_rusa_veh"}}
				{"trigger" {name "defense_support/ally_nato_veh"}}
			)
			(define "ds_claim_veh"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_veh_left$"} {op "=="} {value 2}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_%0_veh1}}}}}
							{tag_add def_sup_deploy}
							{tag_remove ally_sup_%0_veh}
							{tag_remove ally_sup_%0_veh1}
						}
					}
					{"default"
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_%0_veh2}}}}}
							{tag_add def_sup_deploy}
							{tag_remove ally_sup_%0_veh}
							{tag_remove ally_sup_%0_veh2}
						}
					}
				}
			)
			(define "ds_claim_ifv"
				{"entity_state"
					{selector {source advanced} {group {select {tag {tag ally_sup_%0_ifv1}}}}}
					{tag_add def_sup_deploy}
					{tag_remove ally_sup_%0_ifv}
					{tag_remove ally_sup_%0_ifv1}
				}
			)

			; One 4-man line team onto the flag. Not from wave budget.
			(define "ds_pick_garrison"
				("ds_resolve_army")
				{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 10}}
				("ds_poke_faction_line")
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 10}}
						("ds_poke_faction_line")
					}
					{"default"}
				}
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
						("ds_announce_wave")
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "DEFENSE GARRISON POOL SHORT - SKIP FLAG"}}
							}
							{"default"}
						}
					}
					{"default"}
				}
			)

			; Hold group membership. Tag-swapped on purpose: a deployed body carries
			; def_sup_src and def_sup_hN but never a pool tag again, so no spawner can
			; re-pick a holder. The hold triggers are armed from map load and fire the
			; moment their group has a live member, so nothing has to poke them here.
			(define "ds_assign_group"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_group$"} {op "=="} {value 1}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag def_sup_deploy}}}}
							}
							{tag_add def_sup_h1}
						}
					}
					{"case"
						{condition {type cmp_i} {var "defense_support_group$"} {op "=="} {value 2}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag def_sup_deploy}}}}
							}
							{tag_add def_sup_h2}
						}
					}
					{"default"
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag def_sup_deploy}}}}
							}
							{tag_add def_sup_h3}
						}
					}
				}
			)

			; Promote + own + walk to a flag and dig in. def_sup_src is the live roster
			; marker and is never removed: the live-unit cap counts it. The wave advances
			; on the flag its hold group is responsible for, then takes cover - this is a
			; defence, so arrivals hold ground instead of pushing on.
			(define "ds_finish"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_place$"} {op ">"} {value 0}}
						("ds_announce_defense")
					}
					{"default"
						("ds_announce_wave")
					}
				}
				{"delay" {time 0.5}}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag def_sup_deploy}}}}
					}
					{tag_add def_sup_src}
					{tag_remove attack_support_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
				{"delay" {time 0.2}}
				("ds_own_to_defenderbot")
				{"delay" {time 0.4}}
				{"actor_state"
					{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
					{control AI}
					{ai_move {mode enable}}
					{weapon_prepare on}
					{fire_mode open}
					{move_mode free}
					{movement {speed normal} {kind normal} {type normal}}
					{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}
				}
				{"ables"
					{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
					{remove select}
				}
				; Leave the pad immediately — no multi-second stack dwell at entry.
				{"delay" {time 0.25}}
				("ds_assign_group")
				{"delay" {time 0.1}}
				; Advance on the flag this group holds, falling back to af1 on a map
				; where the mission activated fewer flag points than we have groups.
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_group$"} {op "=="} {value 2}}
						{"switch"
							{"case"
								{condition {type entities} {selector {tag def_sup_af2}}}
								{"action"
									{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
									{drop orders}
									{action advance}
									{target {ignore_captured_by_user 0} {tag def_sup_af2}}
								}
							}
							{"default"
								{"action"
									{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
									{drop orders}
									{action advance}
									{target {ignore_captured_by_user 0} {tag def_sup_af1}}
								}
							}
						}
					}
					{"case"
						{condition {type cmp_i} {var "defense_support_group$"} {op "=="} {value 3}}
						{"switch"
							{"case"
								{condition {type entities} {selector {tag def_sup_af3}}}
								{"action"
									{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
									{drop orders}
									{action advance}
									{target {ignore_captured_by_user 0} {tag def_sup_af3}}
								}
							}
							{"default"
								{"action"
									{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
									{drop orders}
									{action advance}
									{target {ignore_captured_by_user 0} {tag def_sup_af1}}
								}
							}
						}
					}
					{"default"
						{"action"
							{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
							{drop orders}
							{action advance}
							{target {ignore_captured_by_user 0} {tag def_sup_af1}}
						}
					}
				}
				; Garrison is already on the flag — dig in fast. Edge waves need walk time.
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_place$"} {op ">"} {value 0}}
						{"delay" {time 2}}
					}
					{"default"
						{"delay" {time 45}}
					}
				}
				{"actor_to_cover"
					{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
				}
				{"delay" {time 0.2}}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag def_sup_deploy}}}}
					}
					{tag_remove def_sup_deploy}
				}
				{"set_i" {var "defense_support_transferred$"} {op "="} {value 1}}
			)

; BEGIN FRIENDLY DEFENDER MOTOR ONE-SHOT — DERIVED FROM RUNTIME-PROVEN PATH

			; Mechanically derived from the live-proven friendly-attacker path.

(define "ds_place_motor_visible"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"placement"
							{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
							{target_waypoint "attack_support_entry_b"}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement"
							{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
							{target_waypoint "attack_support_entry_a"}
						}
					}
					{"default"
						{"placement"
							{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
							{target_waypoint "attack_support_entry_b"}
						}
					}
				}
			)

(define "ds_finish_motor"
				{"delay" {time 0.5}}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag def_sup_deploy}}}}
					}
					{tag_add def_sup_src}
					{tag_remove attack_support_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
				{"delay" {time 0.2}}
				("ds_own_to_defenderbot")
				{"delay" {time 0.4}}
				{"actor_state"
					{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
					{control AI}
					{ai_move {mode enable}}
					{weapon_prepare on}
					{fire_mode open}
					{move_mode free}
					{movement {speed normal} {kind normal} {type normal}}
					{ai {no_retreat on} {advance_ratio 1} {retreat_ratio 0}}
				}
				{"ables"
					{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
					{remove select}
				}
				{"delay" {time 0.25}}
				; Pick one active flag as the approach objective.
				{"entity_state"
					{selector {tag def_sup_motor_flag}}
					{tag_remove def_sup_motor_flag}
				}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude {state {state inactive}}}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add def_sup_motor_flag}
				}
				{"delay" {time 0.1}}
				; Truck drives toward the flag (standoff handled by emit after travel delay).
				{"action"
					{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_motor_hull}}}}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag def_sup_motor_flag}}
				}
				{"delay" {time 60}}
				; Dismount passengers only (crew stays).
				{"emit"
					{selector
						{ignore_captured_by_user 0}
						{tag def_sup_motor_hull}
						{type vehicle}
						{state inhabited}
					}
					{drop orders}
					{emit
						{mode passengers}
					}
				}
				{"delay" {time 3}}
				; Pax advance on foot; truck holds.
				{"action"
					{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_motor_pax}}}}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag def_sup_motor_flag}}
				}
				; Driver leaves the field instead of idling: drive for waypoint "0" and the
				; cleanup trigger below removes the hull 90s later, wreck included.
				{"entity_state"
					{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_motor_hull}}}}}
					{tag_add def_sup_motor_leaving}
				}
				{"action"
					{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_motor_hull}}}}}
					{drop orders}
					{action move}
					{waypoint "0"}
				}
				{"entity_state"
					{selector {tag def_sup_deploy}}
					{tag_remove def_sup_deploy}
				}
				{"entity_state"
					{selector {tag def_sup_motor_hull}}
					{tag_remove def_sup_motor_hull}
				}
				{"entity_state"
					{selector {tag def_sup_motor_pax}}
					{tag_remove def_sup_motor_pax}
				}
				{"entity_state"
					{selector {tag def_sup_motor_g1}}
					{tag_remove def_sup_motor_g1}
				}
				{"entity_state"
					{selector {tag def_sup_motor_g2}}
					{tag_remove def_sup_motor_g2}
				}
				{"entity_state"
					{selector {tag def_sup_motor_g3}}
					{tag_remove def_sup_motor_g3}
				}
				{"entity_state"
					{selector {tag def_sup_motor_g4}}
					{tag_remove def_sup_motor_g4}
				}
				{"set_i" {var "defense_support_transferred$"} {op "="} {value 1}}
			)

			(define "ds_poke_faction_motor"
				{"trigger" {name "defense_support/ally_rusa_motor"}}
				{"trigger" {name "defense_support/ally_nato_motor"}}
			)

{"defense_support/ally_rusa_motor"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_wave_cmd$"} {op "=="} {value 19}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "defense_support_motor_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_rusa_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					("ds_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "DEFENSE SUPPORT MOTORIZED INSERT"}}
						}
						{"default"}
					}
					; Claim whole package (hull + linked crew + pax).
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_motor}}}}}
						{tag_add def_sup_deploy}
						{tag_remove ally_sup_rusa_motor}
						{tag_remove ally_sup_rusa_motor_crew}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_motor_hull}}}}}
						{tag_add def_sup_motor_hull}
						{tag_remove ally_sup_rusa_motor_hull}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_motor_pax}}}}}
						{tag_add def_sup_motor_pax}
						{tag_remove ally_sup_rusa_motor_pax}
					}
					{"set_i" {var "defense_support_motor_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("ds_place_motor_visible")
					("ds_finish_motor")
				}
			}



{"defense_support/ally_nato_motor"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_wave_cmd$"} {op "=="} {value 19}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "defense_support_motor_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_nato_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					("ds_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "DEFENSE SUPPORT MOTORIZED INSERT"}}
						}
						{"default"}
					}
					; Claim whole package (hull + linked crew + pax).
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_motor}}}}}
						{tag_add def_sup_deploy}
						{tag_remove ally_sup_nato_motor}
						{tag_remove ally_sup_nato_motor_crew}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_motor_hull}}}}}
						{tag_add def_sup_motor_hull}
						{tag_remove ally_sup_nato_motor_hull}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_motor_pax}}}}}
						{tag_add def_sup_motor_pax}
						{tag_remove ally_sup_nato_motor_pax}
					}
					{"set_i" {var "defense_support_motor_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("ds_place_motor_visible")
					("ds_finish_motor")
				}
			}

			; Stage-2 validation only: exactly one friendly defender truck at +30s.
; BEGIN FOUR-QUADRANT NORMAL TRANSPORT PATROL — DS

			; Ordinary combat transport: no timed emit, turnaround, withdrawal,

			; deletion, or cleanup. Route targets flag-perimeter waypoints only.

			(define "transport_ds_place"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"placement"
							{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
							{target_waypoint "attack_support_entry_b1"}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement"
							{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
							{target_waypoint "attack_support_entry_a1"}
						}
					}
					{"default"
						{"placement"
							{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
							{target_waypoint "attack_support_entry_b1"}
						}
					}
				}
			)

			(define "transport_ds_issue_patrol"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "transport_ds_step$"} {op "=="} {value 1}}
						{"action"
							{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag transport_ds_hull}}}}}
							{drop orders}
							{action move}
							{waypoint "transport_patrol_flag_1"}
						}
						{"set_i" {var "transport_ds_step$"} {op "="} {value 2}}
					}
					{"case"
						{condition {type cmp_i} {var "transport_ds_step$"} {op "=="} {value 2}}
						{"action"
							{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag transport_ds_hull}}}}}
							{drop orders}
							{action move}
							{waypoint "transport_patrol_flag_2"}
						}
						{"set_i" {var "transport_ds_step$"} {op "="} {value 3}}
					}
					{"case"
						{condition {type cmp_i} {var "transport_ds_step$"} {op "=="} {value 3}}
						{"action"
							{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag transport_ds_hull}}}}}
							{drop orders}
							{action move}
							{waypoint "transport_patrol_flag_3"}
						}
						{"set_i" {var "transport_ds_step$"} {op "="} {value 4}}
					}
					{"case"
						{condition {type cmp_i} {var "transport_ds_step$"} {op "=="} {value 4}}
						{"action"
							{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag transport_ds_hull}}}}}
							{drop orders}
							{action move}
							{waypoint "transport_patrol_flag_4"}
						}
						{"set_i" {var "transport_ds_step$"} {op "="} {value 5}}
					}
					{"case"
						{condition {type cmp_i} {var "transport_ds_step$"} {op "=="} {value 5}}
						{"action"
							{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag transport_ds_hull}}}}}
							{drop orders}
							{action move}
							{waypoint "transport_patrol_flag_5"}
						}
						{"set_i" {var "transport_ds_step$"} {op "="} {value 1}}
					}
					{"default"
						{"set_i" {var "transport_ds_step$"} {op "="} {value 1}}
					}
				}
			)

			{"defense_support/normal_transport_rusa"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_armed$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "transport_ds_done$"} {op "=="} {value 0}}
						{"6.entities" {selector {tag ally_sup_rusa_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "transport_ds_done$"} {op "="} {value 1}}
					{"delay" {time 45}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_motor}}}}}
						{tag_add def_sup_deploy}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_motor_hull}}}}}
						{tag_add transport_ds_hull}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_motor_pax}}}}}
						{tag_add transport_ds_pax}
					}
					("transport_ds_place")
					{"entity_state"
						{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
						{tag_remove ally_sup_tpl}
						{tag_remove ally_sup_rusa_motor}
						{tag_remove hidden}
						{inactive off}
						{impregnability disabled}
						{discovered on}
					}
					{"delay" {time 0.2}}
					("ds_own_to_defenderbot")
					{"delay" {time 0.4}}
					{"actor_state"
						{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
						{control AI}
						{ai_move {mode enable}}
						{weapon_prepare on}
						{fire_mode open}
						{move_mode free}
						{movement {speed normal} {kind normal} {type normal}}
						{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}
					}
					{"set_i" {var "transport_ds_step$"} {op "="} {value 1}}
					("transport_ds_issue_patrol")
					{"entity_state"
						{selector {tag def_sup_deploy}}
						{tag_remove def_sup_deploy}
					}
					{"delay" {time 45}}
					{"trigger" {name "defense_support/normal_transport_patrol"}}
				}
			}


			{"defense_support/normal_transport_nato"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_armed$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "transport_ds_done$"} {op "=="} {value 0}}
						{"6.entities" {selector {tag ally_sup_nato_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "transport_ds_done$"} {op "="} {value 1}}
					{"delay" {time 45}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_motor}}}}}
						{tag_add def_sup_deploy}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_motor_hull}}}}}
						{tag_add transport_ds_hull}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_motor_pax}}}}}
						{tag_add transport_ds_pax}
					}
					("transport_ds_place")
					{"entity_state"
						{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
						{tag_remove ally_sup_tpl}
						{tag_remove ally_sup_nato_motor}
						{tag_remove hidden}
						{inactive off}
						{impregnability disabled}
						{discovered on}
					}
					{"delay" {time 0.2}}
					("ds_own_to_defenderbot")
					{"delay" {time 0.4}}
					{"actor_state"
						{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_deploy}}}}}
						{control AI}
						{ai_move {mode enable}}
						{weapon_prepare on}
						{fire_mode open}
						{move_mode free}
						{movement {speed normal} {kind normal} {type normal}}
						{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}
					}
					{"set_i" {var "transport_ds_step$"} {op "="} {value 1}}
					("transport_ds_issue_patrol")
					{"entity_state"
						{selector {tag def_sup_deploy}}
						{tag_remove def_sup_deploy}
					}
					{"delay" {time 45}}
					{"trigger" {name "defense_support/normal_transport_patrol"}}
				}
			}


			; Route continues only while at least one tagged passenger remains within
			; eight metres of the hull. Normal AI dismount under contact breaks that
			; proximity and therefore stops all further scripted route orders.
			{"defense_support/normal_transport_patrol"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "transport_ds_done$"} {op "=="} {value 1}}
						{"3.entities" {selector {tag transport_ds_hull} {state operatable}} {count {op ">="} {value 1}}}
						{"4.near"
							{units {ignore_captured_by_user 0} {tag transport_ds_pax} {state operatable}}
							{near_to {ignore_captured_by_user 0} {tag transport_ds_hull} {state operatable}}
							{distance 80}
						}
					}
				}
				{actions
					("transport_ds_issue_patrol")
					{"delay" {time 45}}
					{"trigger" {name "defense_support/normal_transport_patrol"}}
				}
			}

; END FOUR-QUADRANT NORMAL TRANSPORT PATROL — DS

{"defense_support/motor_cleanup"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.entities" {selector {tag def_sup_motor_leaving}} {count {op ">"} {value 0}}}
					}
				}
				{actions
					{"delay" {time 90}}
					{"delete" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_motor_leaving}}}}}}
				}
			}

; END FRIENDLY DEFENDER MOTOR ONE-SHOT

			; Player defense calls this as wave 2 then wave 4 only.
			(define "ds_pick_composition"
				("ds_resolve_army")
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_wave_num$"} {op "<="} {value 2}}
						{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 10}}
						("ds_poke_faction_line")
					}
					{"default"
						{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 16}}
						("ds_poke_faction_veh")
					}
				}
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 10}}
						("ds_poke_faction_line")
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "DEFENSE SUPPORT POOL SHORT - FACTION LINE"}}
							}
							{"default"}
						}
					}
					{"default"}
				}
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
						("ds_announce_wave")
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "DEFENSE SUPPORT POOL EXHAUSTED"}}
							}
							{"default"}
						}
					}
					{"default"}
				}
			)

			; GARRISON + PATROL — arms as soon as defender identity is known (no prep wait).
			; Makes the map feel alive while the player places during prep.
			; Phase 4: L2+ crewless weapon on defended flags.
			; Parked prototypes in flag_props_templates.inc - claim by static tag,
			; placement MOVE to flag anchor (same pipeline as every other pool).
			; No runtime spawn verb. No entity-name selectors. Tag flag_prop on claimed props.
			; The supply half of Phase 4 is RETIRED and deliberately absent here: every
			; flag now carries a real flagpoint_ammo supply point linked into its own
			; built-in "ammo" placer slot by the deploy script, which is the vanilla
			; mechanism, follows the flag when it changes hands and needs no engine at
			; all. A prop crate on top of it would be a second, worse supply source.

			(define "ds_props_af1"
				{"if"
					{condition {type entities} {selector {tag def_sup_af1}}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op ">="} {value 1}}	; weapons at every level (was 2) - user 2026-07-30
							{"switch"
								{"case"
									{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select {tag {tag flag_prop_wpn_rusa}}}
												{exclude {tag {tag flag_prop}}}
											}
											{amount 1}
										}
										{tag_add flag_prop}
										{tag_add flag_prop_move}
										{tag_remove flag_prop_wpn_rusa}
									}
									{"delay" {time 0.05}}
									{"placement"
										{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag flag_prop_move}}}}}
										{target {ignore_captured_by_user 0} {tag def_sup_af1}}
									}
									{"entity_state"
										{selector {tag flag_prop_move}}
										{tag_remove flag_prop_move}
									}
								}
								{"case"
									{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select {tag {tag flag_prop_wpn_ukr}}}
												{exclude {tag {tag flag_prop}}}
											}
											{amount 1}
										}
										{tag_add flag_prop}
										{tag_add flag_prop_move}
										{tag_remove flag_prop_wpn_ukr}
									}
									{"delay" {time 0.05}}
									{"placement"
										{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag flag_prop_move}}}}}
										{target {ignore_captured_by_user 0} {tag def_sup_af1}}
									}
									{"entity_state"
										{selector {tag flag_prop_move}}
										{tag_remove flag_prop_move}
									}
								}
								{"case"
									{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 4}}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select {tag {tag flag_prop_wpn_prc}}}
												{exclude {tag {tag flag_prop}}}
											}
											{amount 1}
										}
										{tag_add flag_prop}
										{tag_add flag_prop_move}
										{tag_remove flag_prop_wpn_prc}
									}
									{"delay" {time 0.05}}
									{"placement"
										{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag flag_prop_move}}}}}
										{target {ignore_captured_by_user 0} {tag def_sup_af1}}
									}
									{"entity_state"
										{selector {tag flag_prop_move}}
										{tag_remove flag_prop_move}
									}
								}
								{"default"
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select {tag {tag flag_prop_wpn_nato}}}
												{exclude {tag {tag flag_prop}}}
											}
											{amount 1}
										}
										{tag_add flag_prop}
										{tag_add flag_prop_move}
										{tag_remove flag_prop_wpn_nato}
									}
									{"delay" {time 0.05}}
									{"placement"
										{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag flag_prop_move}}}}}
										{target {ignore_captured_by_user 0} {tag def_sup_af1}}
									}
									{"entity_state"
										{selector {tag flag_prop_move}}
										{tag_remove flag_prop_move}
									}
								}
							}
						}
						{"default"}
					}
				}
			)

			(define "ds_props_af2"
				{"if"
					{condition {type entities} {selector {tag def_sup_af2}}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op ">="} {value 1}}	; weapons at every level (was 2) - user 2026-07-30
							{"switch"
								{"case"
									{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select {tag {tag flag_prop_wpn_rusa}}}
												{exclude {tag {tag flag_prop}}}
											}
											{amount 1}
										}
										{tag_add flag_prop}
										{tag_add flag_prop_move}
										{tag_remove flag_prop_wpn_rusa}
									}
									{"delay" {time 0.05}}
									{"placement"
										{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag flag_prop_move}}}}}
										{target {ignore_captured_by_user 0} {tag def_sup_af2}}
									}
									{"entity_state"
										{selector {tag flag_prop_move}}
										{tag_remove flag_prop_move}
									}
								}
								{"case"
									{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select {tag {tag flag_prop_wpn_ukr}}}
												{exclude {tag {tag flag_prop}}}
											}
											{amount 1}
										}
										{tag_add flag_prop}
										{tag_add flag_prop_move}
										{tag_remove flag_prop_wpn_ukr}
									}
									{"delay" {time 0.05}}
									{"placement"
										{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag flag_prop_move}}}}}
										{target {ignore_captured_by_user 0} {tag def_sup_af2}}
									}
									{"entity_state"
										{selector {tag flag_prop_move}}
										{tag_remove flag_prop_move}
									}
								}
								{"case"
									{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 4}}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select {tag {tag flag_prop_wpn_prc}}}
												{exclude {tag {tag flag_prop}}}
											}
											{amount 1}
										}
										{tag_add flag_prop}
										{tag_add flag_prop_move}
										{tag_remove flag_prop_wpn_prc}
									}
									{"delay" {time 0.05}}
									{"placement"
										{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag flag_prop_move}}}}}
										{target {ignore_captured_by_user 0} {tag def_sup_af2}}
									}
									{"entity_state"
										{selector {tag flag_prop_move}}
										{tag_remove flag_prop_move}
									}
								}
								{"default"
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select {tag {tag flag_prop_wpn_nato}}}
												{exclude {tag {tag flag_prop}}}
											}
											{amount 1}
										}
										{tag_add flag_prop}
										{tag_add flag_prop_move}
										{tag_remove flag_prop_wpn_nato}
									}
									{"delay" {time 0.05}}
									{"placement"
										{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag flag_prop_move}}}}}
										{target {ignore_captured_by_user 0} {tag def_sup_af2}}
									}
									{"entity_state"
										{selector {tag flag_prop_move}}
										{tag_remove flag_prop_move}
									}
								}
							}
						}
						{"default"}
					}
				}
			)

			(define "ds_props_af3"
				{"if"
					{condition {type entities} {selector {tag def_sup_af3}}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op ">="} {value 1}}	; weapons at every level (was 2) - user 2026-07-30
							{"switch"
								{"case"
									{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select {tag {tag flag_prop_wpn_rusa}}}
												{exclude {tag {tag flag_prop}}}
											}
											{amount 1}
										}
										{tag_add flag_prop}
										{tag_add flag_prop_move}
										{tag_remove flag_prop_wpn_rusa}
									}
									{"delay" {time 0.05}}
									{"placement"
										{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag flag_prop_move}}}}}
										{target {ignore_captured_by_user 0} {tag def_sup_af3}}
									}
									{"entity_state"
										{selector {tag flag_prop_move}}
										{tag_remove flag_prop_move}
									}
								}
								{"case"
									{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select {tag {tag flag_prop_wpn_ukr}}}
												{exclude {tag {tag flag_prop}}}
											}
											{amount 1}
										}
										{tag_add flag_prop}
										{tag_add flag_prop_move}
										{tag_remove flag_prop_wpn_ukr}
									}
									{"delay" {time 0.05}}
									{"placement"
										{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag flag_prop_move}}}}}
										{target {ignore_captured_by_user 0} {tag def_sup_af3}}
									}
									{"entity_state"
										{selector {tag flag_prop_move}}
										{tag_remove flag_prop_move}
									}
								}
								{"case"
									{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 4}}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select {tag {tag flag_prop_wpn_prc}}}
												{exclude {tag {tag flag_prop}}}
											}
											{amount 1}
										}
										{tag_add flag_prop}
										{tag_add flag_prop_move}
										{tag_remove flag_prop_wpn_prc}
									}
									{"delay" {time 0.05}}
									{"placement"
										{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag flag_prop_move}}}}}
										{target {ignore_captured_by_user 0} {tag def_sup_af3}}
									}
									{"entity_state"
										{selector {tag flag_prop_move}}
										{tag_remove flag_prop_move}
									}
								}
								{"default"
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select {tag {tag flag_prop_wpn_nato}}}
												{exclude {tag {tag flag_prop}}}
											}
											{amount 1}
										}
										{tag_add flag_prop}
										{tag_add flag_prop_move}
										{tag_remove flag_prop_wpn_nato}
									}
									{"delay" {time 0.05}}
									{"placement"
										{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag flag_prop_move}}}}}
										{target {ignore_captured_by_user 0} {tag def_sup_af3}}
									}
									{"entity_state"
										{selector {tag flag_prop_move}}
										{tag_remove flag_prop_move}
									}
								}
							}
						}
						{"default"}
					}
				}
			)

			(define "ds_place_flag_props"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "flag_props_done$"} {op "=="} {value 0}}
						{"set_i" {var "flag_props_done$"} {op "="} {value 1}}
						("ds_resolve_army")
						("ds_props_af1")
						("ds_props_af2")
						("ds_props_af3")
					}
					{"default"}
				}
			)

			; DAY-2 / parity test: airmobile on player DEFENSE ~30s after identity (no prep wait).
			; Shared Day-2 toggle attack_support_air_test$ (set in garrison_init AND attack init): 1=force, 0=production-only (L2+ random in pick).
			{"defense_support/air_test"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6 & 7"}
					{terms
						{"1.cmp_i" {var "defense_support_air_test_done$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_spawnside$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_air_test$"} {op "=="} {value 1}}
						{"6.cmp_i" {var "support_mission_enabled$"} {op "=="} {value 1}}
						{"7.cmp_i" {var "disable_allied_support$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"set_i" {var "defense_support_air_test_done$"} {op "="} {value 1}}
					{"set_i" {var "support_announce$"} {op "="} {value 1}}
					{"set_i" {var "defense_support_air_left$"} {op "="} {value 0}}
					{"delay" {time 30}}
					("ds_resolve_army")
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 18}}
					("ds_poke_faction_air")
				}
			}

			{"defense_support/garrison_init"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "defense_support_garrison_done$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_spawnside$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_mission_enabled$"} {op "=="} {value 1}}
						{"6.cmp_i" {var "disable_allied_support$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"set_i" {var "defense_support_garrison_done$"} {op "="} {value 1}}
					{"set_i" {var "support_announce$"} {op "="} {value 1}}
					{"set_i" {var "defense_support_air_left$"} {op "="} {value 1}}
					{"set_i" {var "defense_support_use_air$"} {op "="} {value 0}}
					; DAY-2 TEST default ON (shared toggle). Set value 0 for production-only air rolls.
					{"set_i" {var "attack_support_air_test$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_place$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_group$"} {op "="} {value 1}}
					("ds_resolve_owner")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 10} {title "DEFENSE GARRISON + PATROL ARMING"}}
						}
						{"default"}
					}
					("ds_report_owner")
					{"delay" {time 2}}
					("ds_claim_anchors")
					{"delay" {time 4}}
					{"if"
						{condition {type entities} {selector {tag def_sup_af1}}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 8} {title "DEFENSE GARRISON AT FLAG 1"}}
							}
							{"default"}
						}
						{"set_i" {var "defense_support_place$"} {op "="} {value 1}}
						{"set_i" {var "defense_support_group$"} {op "="} {value 1}}
						("ds_pick_garrison")
					}
					{"delay" {time 5}}
					{"if"
						{condition {type entities} {selector {tag def_sup_af2}}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 8} {title "DEFENSE GARRISON AT FLAG 2"}}
							}
							{"default"}
						}
						{"set_i" {var "defense_support_place$"} {op "="} {value 2}}
						{"set_i" {var "defense_support_group$"} {op "="} {value 2}}
						("ds_pick_garrison")
					}
					{"delay" {time 5}}
					{"if"
						{condition {type entities} {selector {tag def_sup_af3}}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 8} {title "DEFENSE GARRISON AT FLAG 3"}}
							}
							{"default"}
						}
						{"set_i" {var "defense_support_place$"} {op "="} {value 3}}
						{"set_i" {var "defense_support_group$"} {op "="} {value 3}}
						("ds_pick_garrison")
					}
					{"delay" {time 2}}
					("ds_place_flag_props")
					{"set_i" {var "defense_support_place$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_group$"} {op "="} {value 1}}
					{"set_i" {var "defense_support_stage$"} {op "="} {value 1}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "DEFENSE GARRISON LIVE - PATROLS ACTIVE"}}
						}
						{"default"}
					}
				}
			}

			; EDGE WAVES — wait for prep so reinforcements do not march in during placement.
			{"defense_support/init"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6 & 7"}
					{terms
						{"1.cmp_i" {var "defense_support_armed$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "prep_inform$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "enemy_spawnside$"} {op ">"} {value 0}}
						{"6.cmp_i" {var "support_mission_enabled$"} {op "=="} {value 1}}
						{"7.cmp_i" {var "disable_allied_support$"} {op "=="} {value 0}}
					}
				}
				{actions
					("ds_resolve_owner")
					{"set_i" {var "defense_support_armed$"} {op "="} {value 1}}
					{"set_i" {var "defense_support_air_left$"} {op "="} {value 1}}
					{"set_i" {var "defense_support_use_air$"} {op "="} {value 0}}
					{"set_i" {var "support_announce$"} {op "="} {value 1}}
					{"set_i" {var "defense_support_busy$"} {op "="} {value 1}}
					{"set_i" {var "defense_support_next_ok$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					("ds_announce_wave")
					{"set_i" {var "defense_support_wave_num$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_place$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_waves_left$"} {op "="} {value 2}}
					{"set_i" {var "defense_support_veh_left$"} {op "="} {value 2}}
					; If garrison lagged (identity late), claim anchors now.
					{"switch"
						{"case"
							{condition {type entities} {selector {tag def_sup_af1}}}
						}
						{"default"
							("ds_claim_anchors")
						}
					}
					{"switch"
						{"case"
							{condition {type rand} {value 0.34}}
							{"delay" {time 25}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 32}}
						}
						{"default"
							{"delay" {time 40}}
						}
					}
					{"set_i" {var "defense_support_wave_num$"} {op "="} {value 2}}
					{"set_i" {var "defense_support_waves_left$"} {op "-"} {value 1}}
					{"set_i" {var "defense_support_group$"} {op "="} {value 1}}
					("ds_pick_composition")
					{"delay" {time 16}}
					{"set_i" {var "defense_support_next_ok$"} {op "="} {value 1}}
					{"set_i" {var "defense_support_busy$"} {op "="} {value 0}}
					{"trigger" {name "defense_support/clock"}}
					{"set_i" {var "defense_support_stage$"} {op "="} {value 2}}
				}
			}

			; Self-re-arming cadence. One cycle = randomized wait, then either a
			; live-unit-cap defer (no wave consumed) or one randomly picked wave.
			; Arrivals rotate through the three hold groups so successive waves spread
			; over the active flags instead of stacking on one. When the budget runs out
			; the condition stops matching and the loop ends.
			{"defense_support/clock"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "defense_support_next_ok$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_busy$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "defense_support_waves_left$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"5.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"6.cmp_i" {var "disable_allied_support$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"set_i" {var "defense_support_busy$"} {op "="} {value 1}}
					; Randomized 110-230s cadence (~20% tighter): ~20% per bucket.
					{"switch"
						{"case"
							{condition {type rand} {value 0.2}}
							{"delay" {time 115}}
						}
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 145}}
						}
						{"case"
							{condition {type rand} {value 0.33}}
							{"delay" {time 170}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 200}}
						}
						{"default"
							{"delay" {time 230}}
						}
					}
					{"delay" {time 0.1}}
					{"set_i" {var "defense_support_wave_num$"} {op "="} {value 4}}
					{"set_i" {var "defense_support_waves_left$"} {op "-"} {value 1}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_support_group$"} {op "=="} {value 1}}
							{"set_i" {var "defense_support_group$"} {op "="} {value 2}}
						}
						{"case"
							{condition {type cmp_i} {var "defense_support_group$"} {op "=="} {value 2}}
							{"set_i" {var "defense_support_group$"} {op "="} {value 3}}
						}
						{"default"
							{"set_i" {var "defense_support_group$"} {op "="} {value 1}}
						}
					}
					("ds_pick_composition")
					{"delay" {time 0.1}}
					{"set_i" {var "defense_support_busy$"} {op "="} {value 0}}
					{"trigger" {name "defense_support/clock"}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_support_waves_left$"} {op "=="} {value 0}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 10} {title "DEFENSE SUPPORT WAVES EXHAUSTED"}}
								}
								{"default"}
							}
						("ds_announce_exhausted")
						}
						{"default"}
					}
				}
			}

			; HOLD GROUP 1. Self-re-arming re-order on its own modest 90-150s ladder, so
			; the three groups never move in step. Roughly two thirds of all re-orders
			; keep the group on its own flag, the rest redistribute it onto another
			; active flag - a wiped objective gets re-covered without the whole force
			; abandoning the one it is on. Every re-order ends in cover, because this is
			; a defence. The condition simply stops matching once the group is wiped,
			; and matches again if a later wave joins the group.
						{"defense_support/hold_1"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag def_sup_h1}
								{type human}
								{state "not dead"}
							}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 90}}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"delay" {time 110}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 130}}
						}
						{"default"
							{"delay" {time 150}}
						}
					}
					{"delay" {time 0.1}}
					; 50% hold/redistribute on active flags (then cover); 50% patrol roam.
					{"switch"
						{"case"
							{condition {type rand} {value 0.5}}
							{"switch"
								{"case"
									{condition {type rand} {value 0.34}}
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_af1}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af1}}
											}
										}
										{"default"
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af2}}
											}
										}
									}
								}
								{"case"
									{condition {type rand} {value 0.5}}
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_af2}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af2}}
											}
										}
										{"default"
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af1}}
											}
										}
									}
								}
								{"default"
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_af3}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af3}}
											}
										}
										{"default"
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af1}}
											}
										}
									}
								}
							}
							{"delay" {time 35}}
							{"actor_to_cover"
								{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
							}
						}
						{"default"
							; Roam: spare flags r1/r2, else defender-side entry pads only.
							{"switch"
								{"case"
									{condition {type rand} {value 0.34}}
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_r1}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_r1}}
											}
										}
										{"default"
											{"switch"
												{"case"
													{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
													{"action"
														{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
														{drop orders}
														{action move}
														{waypoint "attack_support_entry_b1"}
													}
												}
												{"default"
													{"action"
														{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
														{drop orders}
														{action move}
														{waypoint "attack_support_entry_a1"}
													}
												}
											}
										}
									}
								}
								{"case"
									{condition {type rand} {value 0.5}}
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_r2}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_r2}}
											}
										}
										{"default"
											{"switch"
												{"case"
													{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
													{"action"
														{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
														{drop orders}
														{action move}
														{waypoint "attack_support_entry_b2"}
													}
												}
												{"default"
													{"action"
														{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
														{drop orders}
														{action move}
														{waypoint "attack_support_entry_a2"}
													}
												}
											}
										}
									}
								}
								{"default"
									{"switch"
										{"case"
											{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
												{drop orders}
												{action move}
												{waypoint "attack_support_entry_b3"}
											}
										}
										{"default"
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h1}}}}}
												{drop orders}
												{action move}
												{waypoint "attack_support_entry_a3"}
											}
										}
									}
								}
							}
						}
					}
					{"delay" {time 0.1}}
					{"trigger" {name "defense_support/hold_1"}}
				}
			}


			; HOLD GROUP 2. Self-re-arming re-order on its own modest 90-150s ladder, so
			; the three groups never move in step. Roughly two thirds of all re-orders
			; keep the group on its own flag, the rest redistribute it onto another
			; active flag - a wiped objective gets re-covered without the whole force
			; abandoning the one it is on. Every re-order ends in cover, because this is
			; a defence. The condition simply stops matching once the group is wiped,
			; and matches again if a later wave joins the group.
						{"defense_support/hold_2"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag def_sup_h2}
								{type human}
								{state "not dead"}
							}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 90}}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"delay" {time 110}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 130}}
						}
						{"default"
							{"delay" {time 150}}
						}
					}
					{"delay" {time 0.1}}
					; 50% hold/redistribute on active flags (then cover); 50% patrol roam.
					{"switch"
						{"case"
							{condition {type rand} {value 0.5}}
							{"switch"
								{"case"
									{condition {type rand} {value 0.34}}
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_af1}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af1}}
											}
										}
										{"default"
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af2}}
											}
										}
									}
								}
								{"case"
									{condition {type rand} {value 0.5}}
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_af2}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af2}}
											}
										}
										{"default"
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af1}}
											}
										}
									}
								}
								{"default"
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_af3}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af3}}
											}
										}
										{"default"
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af1}}
											}
										}
									}
								}
							}
							{"delay" {time 35}}
							{"actor_to_cover"
								{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
							}
						}
						{"default"
							; Roam: spare flags r1/r2, else defender-side entry pads only.
							{"switch"
								{"case"
									{condition {type rand} {value 0.34}}
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_r1}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_r1}}
											}
										}
										{"default"
											{"switch"
												{"case"
													{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
													{"action"
														{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
														{drop orders}
														{action move}
														{waypoint "attack_support_entry_b1"}
													}
												}
												{"default"
													{"action"
														{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
														{drop orders}
														{action move}
														{waypoint "attack_support_entry_a1"}
													}
												}
											}
										}
									}
								}
								{"case"
									{condition {type rand} {value 0.5}}
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_r2}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_r2}}
											}
										}
										{"default"
											{"switch"
												{"case"
													{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
													{"action"
														{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
														{drop orders}
														{action move}
														{waypoint "attack_support_entry_b2"}
													}
												}
												{"default"
													{"action"
														{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
														{drop orders}
														{action move}
														{waypoint "attack_support_entry_a2"}
													}
												}
											}
										}
									}
								}
								{"default"
									{"switch"
										{"case"
											{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
												{drop orders}
												{action move}
												{waypoint "attack_support_entry_b3"}
											}
										}
										{"default"
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h2}}}}}
												{drop orders}
												{action move}
												{waypoint "attack_support_entry_a3"}
											}
										}
									}
								}
							}
						}
					}
					{"delay" {time 0.1}}
					{"trigger" {name "defense_support/hold_2"}}
				}
			}


			; HOLD GROUP 3. Self-re-arming re-order on its own modest 90-150s ladder, so
			; the three groups never move in step. Roughly two thirds of all re-orders
			; keep the group on its own flag, the rest redistribute it onto another
			; active flag - a wiped objective gets re-covered without the whole force
			; abandoning the one it is on. Every re-order ends in cover, because this is
			; a defence. The condition simply stops matching once the group is wiped,
			; and matches again if a later wave joins the group.
						{"defense_support/hold_3"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag def_sup_h3}
								{type human}
								{state "not dead"}
							}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 90}}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"delay" {time 110}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 130}}
						}
						{"default"
							{"delay" {time 150}}
						}
					}
					{"delay" {time 0.1}}
					; 50% hold/redistribute on active flags (then cover); 50% patrol roam.
					{"switch"
						{"case"
							{condition {type rand} {value 0.5}}
							{"switch"
								{"case"
									{condition {type rand} {value 0.34}}
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_af1}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af1}}
											}
										}
										{"default"
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af2}}
											}
										}
									}
								}
								{"case"
									{condition {type rand} {value 0.5}}
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_af2}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af2}}
											}
										}
										{"default"
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af1}}
											}
										}
									}
								}
								{"default"
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_af3}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af3}}
											}
										}
										{"default"
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_af1}}
											}
										}
									}
								}
							}
							{"delay" {time 35}}
							{"actor_to_cover"
								{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
							}
						}
						{"default"
							; Roam: spare flags r1/r2, else defender-side entry pads only.
							{"switch"
								{"case"
									{condition {type rand} {value 0.34}}
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_r1}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_r1}}
											}
										}
										{"default"
											{"switch"
												{"case"
													{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
													{"action"
														{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
														{drop orders}
														{action move}
														{waypoint "attack_support_entry_b1"}
													}
												}
												{"default"
													{"action"
														{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
														{drop orders}
														{action move}
														{waypoint "attack_support_entry_a1"}
													}
												}
											}
										}
									}
								}
								{"case"
									{condition {type rand} {value 0.5}}
									{"switch"
										{"case"
											{condition {type entities} {selector {tag def_sup_r2}}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag def_sup_r2}}
											}
										}
										{"default"
											{"switch"
												{"case"
													{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
													{"action"
														{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
														{drop orders}
														{action move}
														{waypoint "attack_support_entry_b2"}
													}
												}
												{"default"
													{"action"
														{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
														{drop orders}
														{action move}
														{waypoint "attack_support_entry_a2"}
													}
												}
											}
										}
									}
								}
								{"default"
									{"switch"
										{"case"
											{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
												{drop orders}
												{action move}
												{waypoint "attack_support_entry_b3"}
											}
										}
										{"default"
											{"action"
												{selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag def_sup_h3}}}}}
												{drop orders}
												{action move}
												{waypoint "attack_support_entry_a3"}
											}
										}
									}
								}
							}
						}
					}
					{"delay" {time 0.1}}
					{"trigger" {name "defense_support/hold_3"}}
				}
			}


			; COMPOSITION 1 - 4 USMC infantry. Also the pool-short fallback, so this
			; pool is the deepest one the shared NATO pool ships. Taking the bodies
			; strips the pool tag, so the gate below can simply count the tag to know
			; what is still parked and available.

			; COMPOSITION 2 - 4 1AD infantry.

			; COMPOSITION 5 - 4 ARF / NATO infantry.

			; COMPOSITION 3 (level 2+) - 6 German pzgren, the heavy hold team. The shared
			; pzgren pool is only twelve deep, so this fields two draws per mission and
			; then the pool-short fallback steps the pick down to the USMC rifle team.

			; ===== FACTION LINE / WPN (player nation) — defense =====
			{"defense_support/ally_rusa_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_wave_cmd$"} {op "=="} {value 10}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_line}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					("ds_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "DEFENSE SUPPORT - RUSA LINE"}}
						}
						{"default"}
					}
					("ds_claim_line" args rusa)
					{"delay" {time 0.1}}
					("ds_place_at_entry")
					("ds_finish")
				}
			}
			{"defense_support/ally_nato_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_wave_cmd$"} {op "=="} {value 10}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_line}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					("ds_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "DEFENSE SUPPORT - NATO LINE"}}
						}
						{"default"}
					}
					("ds_claim_line" args nato)
					{"delay" {time 0.1}}
					("ds_place_at_entry")
					("ds_finish")
				}
			}

			{"defense_support/ally_rusa_veh"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_wave_cmd$"} {op "=="} {value 16}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "defense_support_veh_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_line}}}} {amount 6}}
						{tag_add def_sup_deploy}
						{tag_remove ally_sup_rusa_line}
					}
					("ds_claim_veh" args rusa)
					{"set_i" {var "defense_support_veh_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("ds_place_at_entry")
					("ds_finish")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_support_wave_num$"} {op ">="} {value 4}}
							("ds_claim_ifv" args rusa)
							{"delay" {time 0.5}}
							("ds_place_at_entry")
							("ds_finish")
						}
						{"default"}
					}
				}
			}
			{"defense_support/ally_nato_veh"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_wave_cmd$"} {op "=="} {value 16}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "defense_support_veh_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_line}}}} {amount 6}}
						{tag_add def_sup_deploy}
						{tag_remove ally_sup_nato_line}
					}
					("ds_claim_veh" args nato)
					{"set_i" {var "defense_support_veh_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("ds_place_at_entry")
					("ds_finish")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_support_wave_num$"} {op ">="} {value 4}}
							("ds_claim_ifv" args nato)
							{"delay" {time 0.5}}
							("ds_place_at_entry")
							("ds_finish")
						}
						{"default"}
					}
				}
			}

			; ===== DEFENSE AIRMOBILE (cmd 18, parity with attack) =====

			{"defense_support/ally_rusa_air"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_wave_cmd$"} {op "=="} {value 18}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "defense_support_air_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_place$"} {op "="} {value 0}}
					("ds_announce_airborne")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "DEFENSE SUPPORT AIRMOBILE INSERT"}}
						}
						{"default"}
					}
					{"delay" {time 4.5}}
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_rusa_line}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_rusa_line}}}} {amount 4}}
								{tag_add def_sup_deploy}
								{tag_remove ally_sup_rusa_line}
							}
						}
						{"default"}
					}
					{"switch"
						{"case"
							{condition {type entities} {selector {tag def_sup_deploy}}}
							; Enemy near LZ guard — if bots already within 120, fall back to edge pads.
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{type human}
											{state "not dead"}
											{tag _bot}
											{distance 120}
										}
										{count {op ">"} {value 0}}
									}
									{"set_i" {var "defense_support_use_air$"} {op "="} {value 0}}
								}
								{"default"
									{"set_i" {var "defense_support_use_air$"} {op "="} {value 1}}
								}
							}
							{"set_i" {var "defense_support_air_left$"} {op "-"} {value 1}}
							{"set_i" {var "defense_support_group$"} {op "="} {value 1}}
							{"delay" {time 0.1}}
							("ds_place_at_entry")
							("ds_finish")
							{"set_i" {var "defense_support_use_air$"} {op "="} {value 0}}
						}
						{"default"}
					}
				}
			}



			{"defense_support/ally_nato_air"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_wave_cmd$"} {op "=="} {value 18}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "defense_support_air_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_place$"} {op "="} {value 0}}
					("ds_announce_airborne")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "DEFENSE SUPPORT AIRMOBILE INSERT"}}
						}
						{"default"}
					}
					{"delay" {time 4.5}}
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_nato_line}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_nato_line}}}} {amount 4}}
								{tag_add def_sup_deploy}
								{tag_remove ally_sup_nato_line}
							}
						}
						{"default"}
					}
					{"switch"
						{"case"
							{condition {type entities} {selector {tag def_sup_deploy}}}
							; Enemy near LZ guard — if bots already within 120, fall back to edge pads.
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{type human}
											{state "not dead"}
											{tag _bot}
											{distance 120}
										}
										{count {op ">"} {value 0}}
									}
									{"set_i" {var "defense_support_use_air$"} {op "="} {value 0}}
								}
								{"default"
									{"set_i" {var "defense_support_use_air$"} {op "="} {value 1}}
								}
							}
							{"set_i" {var "defense_support_air_left$"} {op "-"} {value 1}}
							{"set_i" {var "defense_support_group$"} {op "="} {value 1}}
							{"delay" {time 0.1}}
							("ds_place_at_entry")
							("ds_finish")
							{"set_i" {var "defense_support_use_air$"} {op "="} {value 0}}
						}
						{"default"}
					}
				}
			}

