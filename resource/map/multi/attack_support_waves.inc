; Attack support waves â€” the human attacker's AI teammate reinforcement engine.
; Included by all fourteen CWA campaign_capture_the_flag.mi maps. Runs ONLY when the
; human is attacking; attack_support.lua publishes id_attack_support$ /
; attack_support_ready$ / attack_support_use_mi$ and nothing here moves until it has.
;
; DELIVERY (proven, do not deviate): off-map real-breed prototypes in
; attack_support_templates.inc are MOVED to the attack-side entry waypoint with
; {"placement"} and promoted in place. Never cloned: three promote designs each
; matched zero freshly created entities, because a new entity's provenance is
; invisible to every selector this format can express. Ownership is a literal 1-16
; {player} switch on id_attack_support$ â€” the engine will not accept a var there.
;
; SCHEDULE
;   opening wave  30/38/45s after the identity is published, one small rifle team
;   recurring     self-re-arming clock, randomized 120/150/180/210/240s cadence (~20% each)
;                 (~20% tighter than the old 150-300s ladder so wave 2 is ~3-5 min, not 5-6)
;   budget        defense_level$ (CE campaign progression, 1-3): L1 4 waves,
;                 L2 6, L3 8 - including the opening wave. Level 0 falls back to L1.
;
; COMPOSITION POOL, one drawn at random per wave, widening with the level:
;   1 usmc     5 USMC infantry                        L1 L2 L3   (also the pool-short fallback)
;   2 1ad      5 1AD infantry                         L1 L2 L3
;   5 arf      5 ARF/NATO infantry                    L1 L2 L3
;   3 acav     4 1AD infantry + 1 crewed humvee          L2 L3
;   4 pzgren   6 German pzgren + 2 crewed humvees           L3
; Waves stay deliberately small: this is a fireteam of support, not a second army.
; Place is staggered one-body-at-a-time then immediate advance off the pad (no corpse piles).
;
; SAFETY GATES on every wave: the readiness gates above, the wave budget, and a
; live-unit cap (defer without consuming a wave while more than 14 attack-support
; infantry are alive). A composition whose pool cannot field it clears no command,
; so the pick falls back to the USMC rifle team and then gives up for that cycle.
;
; Vehicles deploy one at a time with a gap, because two humvees placed together
; clip and flip. Movement is {action advance} on a claimed flag after a slow
; {"placement"}, never a beeline from a raw coordinate.
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
; Player-facing radio uses support_announce$ + {"talk"} (soldier portrait), never support_debug$. Declared in dcg_vars.inc.
;
; ENTRY POINTS / triple pads. Each side's map-edge arrival is THREE waypoints, not one:
; attack_support_entry_a1/a2/a3 and b1/b2/b3, an equilateral triangle of side ~27m whose
; first point sits exactly on that side's spawn centroid. Point 1 is the same coordinate
; the legacy attack_support_entry_a / _b still carry, so those names survive as aliases
; and the patrol / roam move orders that use them are unchanged. All six are (re)built
; from the repo maps by tools/deploy_attack_support_probe.ps1, idempotently.
; FLANK PADS / attack_support_flank_<side>1/2 at ~50% depth. as_choose_entry rolls 25%
; per batch; enemy-near-pad guard falls back to main edge pads. Q1 only.
; AIRMOBILE (Phase 5 E1): narrative helo insert at attack_support_air_<side>1/2 (~65% depth).
; Production: L2+ ~15% roll, cap attack_support_air_left$=2, cmd 18.
; DAY-2 TEST: attack_support_air_test$=1 (default) forces first insert ~30s after arm.
; Set attack_support_air_test$=0 in init to restore production-only schedule.
;
; am_entry_next bumps attack_support_entry_rr$ (1 -> 2 -> 3 -> 1) once per placement batch, at the top
; of am_place_at_entry, and the placement site is a three-case cascade on it. So consecutive
; batches never land on the same pad - including G1 and G2 of one wave, which is what was
; dropping bodies on top of each other and killing them on arrival.

			; Rotating entry point, bumped once per placement batch so no two
			; consecutive batches land on the same pad - including G1 and G2 of a
			; single wave, which is what was stacking bodies on top of each other.
			(define "am_entry_next"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_entry_rr$"} {op ">="} {value 3}}
						{"set_i" {var "attack_support_entry_rr$"} {op "="} {value 1}}
					}
					{"default"
						{"set_i" {var "attack_support_entry_rr$"} {op "+"} {value 1}}
					}
				}
			)

			; Player-facing radio talk (support_announce$ default ON via init).
			; Uses stock {"talk"} so a live player soldier portrait delivers the line.
			; Distinct from support_debug$ diagnostics. Never mix the two gates.
			(define "as_announce_wave"
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
			(define "as_announce_vehicle"
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
							{text "mission/multi/support/vehicle_inbound"}
						}
					}
					{"default"}
				}
			)
			(define "as_announce_flank"
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
							{text "mission/multi/support/flank_inbound"}
						}
					}
					{"default"}
				}
			)

			(define "as_announce_motor"
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
							{text "mission/multi/support/motorized_inbound"}
						}
					}
					{"default"}
				}
			)

			(define "as_announce_airborne"
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

			(define "as_announce_exhausted"
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

			; Place a single currently-tagged body, then clear the one-shot tag.
			(define "am_place_one"
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag attack_support_deploy}}}
							{exclude {tag {tag attack_support_placed}}}
							; A {Link}-baked crew/rider is carried by its hull. Teleporting it on its own
							; overwrites its world transform with the pad point and it renders clipped into
							; the driver seat until it emits. Place the hull only.
							{exclude {tag {tag sup_linked}}}
						}
						{amount 1}
					}
					{tag_add attack_support_place_one}
				}
				{"delay" {time 0.05}}
				{"switch"
					{"case"
						{condition {type entities} {selector {tag attack_support_place_one}}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "attack_support_use_air$"} {op "=="} {value 1}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_flank_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_air_b2"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_air_b1"}
												}
											}
										}
									}
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_flank_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_air_a2"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_air_a1"}
												}
											}
										}
									}
									{"default"
										{"placement"
											{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
											{target_waypoint "attack_support_air_b1"}
										}
									}
								}
							}
							{"case"
								{condition {type cmp_i} {var "attack_support_use_flank$"} {op "=="} {value 1}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_flank_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_flank_b2"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_flank_b1"}
												}
											}
										}
									}
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_flank_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_flank_a2"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_flank_a1"}
												}
											}
										}
									}
									{"default"
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_flank_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_flank_b2"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_flank_b1"}
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
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_entry_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_b2"}
												}
											}
											{"case"
												{condition {type cmp_i} {var "attack_support_entry_rr$"} {op "=="} {value 3}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_b3"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_b1"}
												}
											}
										}
									}
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_entry_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_a2"}
												}
											}
											{"case"
												{condition {type cmp_i} {var "attack_support_entry_rr$"} {op "=="} {value 3}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_a3"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_a1"}
												}
											}
										}
									}
									{"default"
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_entry_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_b2"}
												}
											}
											{"case"
												{condition {type cmp_i} {var "attack_support_entry_rr$"} {op "=="} {value 3}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_b3"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_b1"}
												}
											}
										}
									}
								}
							}
						}
						{"entity_state"
							{selector {tag attack_support_place_one}}
							{tag_add attack_support_placed}
							{tag_remove attack_support_place_one}
						}
						{"delay" {time 0.2}}
					}
					{"default"}
				}
			)


			; Stagger up to 8 bodies onto the entry (same waypoint, spaced in time so
			; physics does not crush a full fireteam into one pile).

			; Pick main triple pads vs flank pads for this placement batch (attack only).
			; 25% flank roll; round-robin flank_rr 1/2; fall back to main if enemies near pad.
			(define "as_choose_entry"
				{"set_i" {var "attack_support_use_flank$"} {op "="} {value 0}}
				; Airmobile batch already chose LZ — do not also roll flank.
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_use_air$"} {op "=="} {value 1}}
					}
					{"default"
						{"switch"
							{"case"
								{condition {type rand} {value 0.25}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_flank_rr$"} {op ">="} {value 2}}
										{"set_i" {var "attack_support_flank_rr$"} {op "="} {value 1}}
									}
									{"default"
										{"set_i" {var "attack_support_flank_rr$"} {op "+"} {value 1}}
									}
								}
								; Safety: if any live bot human is already within 120 of the chosen
								; flank pad, stay on the main edge pads this wave.
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
										{"set_i" {var "attack_support_use_flank$"} {op "="} {value 0}}
									}
									{"default"
										{"set_i" {var "attack_support_use_flank$"} {op "="} {value 1}}
										("as_announce_flank")
									}
								}
							}
							{"default"
								{"set_i" {var "attack_support_use_flank$"} {op "="} {value 0}}
							}
						}
					}
				}
			)

			; Twelve single-body slots, not eight. A motorized package is the widest
			; batch this engine places: hull + 2 crew + up to 8 {Link}-baked riders =
			; 11 bodies. With only eight slots the leftovers stayed at their parked
			; corner already un-hidden and owned - visible strays that also ate the
			; live-unit cap. Slots past the batch cost one 0.05s tick each, because
			; the placement branch (and its 0.2s spacing) only runs when a body was
			; actually tagged.
			(define "am_place_at_entry"
				("as_choose_entry")
				("am_entry_next")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				{"entity_state"
					{selector {tag attack_support_placed}}
					{tag_remove attack_support_placed}
				}
			)

			(define "am_own_to_support"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "16"}}}
					{"default"
						{"player"
							{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
							{operation set}
							{player "3"}
						}
					}
				}
			)

			; Promote + own + slow MOVE (not a sprint advance). Split into staggered
			; fireteams so a wave does not walk one line. attack_support_src is the live
			; attack-support roster marker and is never removed: the live-unit cap counts it.
			(define "am_finish_deploy"
				{"delay" {time 0.5}}
				; Fresh scatter picks each deploy (clear previous wave flag tags).
				{"entity_state"
					{selector {tag attack_support_flag1}}
					{tag_remove attack_support_flag1}
				}
				{"entity_state"
					{selector {tag attack_support_flag2}}
					{tag_remove attack_support_flag2}
				}
				{"entity_state"
					{selector {tag attack_support_flag3}}
					{tag_remove attack_support_flag3}
				}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag attack_support_deploy}}}}
					}
					{tag_add attack_support_src}
					{tag_remove attack_support_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
				{"delay" {time 0.2}}
				("am_own_to_support")
				{"delay" {time 0.4}}
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
					{control AI}
					{ai_move {mode enable}}
					{weapon_prepare on}
					{fire_mode open}
					{move_mode free}
					{movement {speed normal} {kind normal} {type normal}}
					{ai {no_retreat on} {advance_ratio 1} {retreat_ratio 0}}
				}
				{"ables"
					{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
					{remove select}
				}
				; Brief settle only â€” long pad dwell was stacking corpses at the entry.
				{"delay" {time 0.25}}
				; Up to 3 active flags, shuffled â€” different elements peel different ways.
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
					{tag_add attack_support_flag1}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
								{tag {tag attack_support_flag1}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add attack_support_flag2}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
								{tag {tag attack_support_flag1}}
								{tag {tag attack_support_flag2}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add attack_support_flag3}
				}
				{"delay" {time 0.1}}
				; Split deploy into staggered fireteams so they do not laser as one blob.
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag attack_support_deploy}}}}
						{amount 2}
					}
					{tag_add attack_support_g1}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag attack_support_deploy}}}
							{exclude {tag {tag attack_support_g1}}}
						}
						{amount 2}
					}
					{tag_add attack_support_g2}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag attack_support_deploy}}}
							{exclude
								{tag {tag attack_support_g1}}
								{tag {tag attack_support_g2}}
							}
						}
						{amount 2}
					}
					{tag_add attack_support_g3}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag attack_support_deploy}}}
							{exclude
								{tag {tag attack_support_g1}}
								{tag {tag attack_support_g2}}
								{tag {tag attack_support_g3}}
							}
						}
					}
					{tag_add attack_support_g4}
				}
				; Get everyone OFF the pad immediately (staggered peels, no multi-second dwell).
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_g1}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag1}}
				}
				{"delay" {time 0.35}}
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_g2}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag2}}
				}
				{"delay" {time 0.35}}
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_g3}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag1}}
				}
				{"delay" {time 0.35}}
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_g4}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag3}}
				}
				{"delay" {time 0.2}}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag attack_support_deploy}}}}
					}
					{tag_remove attack_support_deploy}
					{tag_remove attack_support_g1}
					{tag_remove attack_support_g2}
					{tag_remove attack_support_g3}
					{tag_remove attack_support_g4}
				}
				{"entity_state"
					{selector {tag attack_support_flag3}}
					{tag_remove attack_support_flag3}
				}
				{"set_i" {var "attack_support_transferred$"} {op "="} {value 1}}
			)

			; Vehicle instances are consumed in declaration order, tracked by
			; attack_support_hmmwv_left$. Each humvee's crew is {Link}ed to that specific
			; hull, so an instance has to move as a whole - never select a count of bodies
			; out of a shared vehicle pool, or a hull arrives without its driver.
			(define "am_deploy_next_hmmwv"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value 4}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_hmmwv1}}}}
							}
							{tag_add attack_support_deploy}
						}
						{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value 3}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_hmmwv2}}}}
							}
							{tag_add attack_support_deploy}
						}
						{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value 2}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_hmmwv3}}}}
							}
							{tag_add attack_support_deploy}
						}
						{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value 1}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_hmmwv4}}}}
							}
							{tag_add attack_support_deploy}
						}
						{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}
					}
					{"default"}
				}
				{"delay" {time 0.1}}
				("am_place_at_entry")
				("am_finish_deploy")
			)

			; Player nation -> faction_support_army$ (1 rusa / 2 ukr / 3 nato / 4 prc).
			; Same fold as enemy bot_army: sov+pol->rusa, csa+frg->nato.
			(define "as_resolve_army"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 2}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 2}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 6}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 4}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 1}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 5}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 8}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 3}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 3}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 4}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 3}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 7}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 3}}
					}
					{"default"
						; Fall back to hint, then NATO.
						{"switch"
							{"case"
								{condition {type cmp_i} {var "user_nation_hint$"} {op "=="} {value 1}}
								{"set_i" {var "faction_support_army$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "faction_support_army$"} {op "="} {value 3}}
							}
						}
					}
				}
			)

			; cmd 10 line 11 wpn 12 recon 13 assault 14 eng 15 manpad 16 light veh
			; 17 IFV 18 airmobile 19 motorized truck (RUSA/UKR/NATO/PRC sx2190 passenger)
			; NATO specialty cmds 1-5 still used when faction_support_army$ == 3.
			(define "as_poke_faction_line"
				{"trigger" {name "attack_support/ally_rusa_line"}}
				{"trigger" {name "attack_support/ally_ukr_line"}}
				{"trigger" {name "attack_support/ally_prc_line"}}
				{"trigger" {name "attack_support/ally_nato_line"}}
			)
			(define "as_poke_faction_wpn"
				{"trigger" {name "attack_support/ally_rusa_wpn"}}
				{"trigger" {name "attack_support/ally_ukr_wpn"}}
				{"trigger" {name "attack_support/ally_prc_wpn"}}
				{"trigger" {name "attack_support/ally_nato_wpn"}}
			)
			(define "as_poke_faction_recon"
				{"trigger" {name "attack_support/ally_rusa_recon"}}
				{"trigger" {name "attack_support/ally_ukr_recon"}}
				{"trigger" {name "attack_support/ally_prc_recon"}}
				{"trigger" {name "attack_support/ally_nato_recon"}}
			)
			(define "as_poke_faction_assault"
				{"trigger" {name "attack_support/ally_rusa_assault"}}
				{"trigger" {name "attack_support/ally_ukr_assault"}}
				{"trigger" {name "attack_support/ally_prc_assault"}}
				{"trigger" {name "attack_support/ally_nato_assault"}}
			)
			(define "as_poke_faction_eng"
				{"trigger" {name "attack_support/ally_rusa_eng"}}
				{"trigger" {name "attack_support/ally_ukr_eng"}}
				{"trigger" {name "attack_support/ally_prc_eng"}}
				{"trigger" {name "attack_support/ally_nato_eng"}}
			)
			(define "as_poke_faction_manpad"
				{"trigger" {name "attack_support/ally_rusa_manpad"}}
				{"trigger" {name "attack_support/ally_ukr_manpad"}}
				{"trigger" {name "attack_support/ally_prc_manpad"}}
				{"trigger" {name "attack_support/ally_nato_manpad"}}
			)
			(define "as_poke_faction_air"
				{"trigger" {name "attack_support/ally_rusa_air"}}
				{"trigger" {name "attack_support/ally_ukr_air"}}
				{"trigger" {name "attack_support/ally_prc_air"}}
				{"trigger" {name "attack_support/ally_nato_air"}}
			)
			(define "as_poke_faction_ifv"
				{"trigger" {name "attack_support/ally_rusa_ifv"}}
				{"trigger" {name "attack_support/ally_ukr_ifv"}}
				{"trigger" {name "attack_support/ally_prc_ifv"}}
				{"trigger" {name "attack_support/ally_nato_ifv"}}
			)
			; PRC uses shaanxi_sx2190_passenger (group passengers). PLA IFVs still use group crew.
			(define "as_poke_faction_motor"
				{"trigger" {name "attack_support/ally_rusa_motor"}}
				{"trigger" {name "attack_support/ally_ukr_motor"}}
				{"trigger" {name "attack_support/ally_prc_motor"}}
				{"trigger" {name "attack_support/ally_nato_motor"}}
			)
			(define "as_poke_faction_veh"
				{"trigger" {name "attack_support/ally_ukr_veh"}}
				{"trigger" {name "attack_support/ally_nato_veh"}}
			)

			; Non-NATO hybrid pick by level. L1 line/recon; L2+ widens; L3 rare AA/veh.
			(define "as_pick_hybrid_non_nato"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
						{"switch"
							{"case"
								{condition {type rand} {value 0.20}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_motor_left$"} {op ">"} {value 0}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 19}}
										("as_poke_faction_motor")
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
										("as_poke_faction_line")
									}
								}
							}
							{"case"
								{condition {type rand} {value 0.15}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_air_left$"} {op ">"} {value 0}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 18}}
										("as_poke_faction_air")
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
										("as_poke_faction_line")
									}
								}
							}
							{"case"
								{condition {type rand} {value 0.15}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_ifv_left$"} {op ">"} {value 0}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 17}}
										("as_poke_faction_ifv")
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
										("as_poke_faction_line")
									}
								}
							}
							{"case"
								{condition {type rand} {value 0.1}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 16}}
								("as_poke_faction_veh")
							}
							{"case"
								{condition {type rand} {value 0.11}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 15}}
								("as_poke_faction_manpad")
							}
							{"case"
								{condition {type rand} {value 0.13}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 14}}
								("as_poke_faction_eng")
							}
							{"case"
								{condition {type rand} {value 0.21}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 13}}
								("as_poke_faction_assault")
							}
							{"case"
								{condition {type rand} {value 0.25}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 12}}
								("as_poke_faction_recon")
							}
							{"case"
								{condition {type rand} {value 0.4}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 11}}
								("as_poke_faction_wpn")
							}
							{"default"
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
								("as_poke_faction_line")
							}
						}
					}
					{"case"
						{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
						{"switch"
							{"case"
								{condition {type rand} {value 0.20}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_motor_left$"} {op ">"} {value 0}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 19}}
										("as_poke_faction_motor")
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
										("as_poke_faction_line")
									}
								}
							}
							{"case"
								{condition {type rand} {value 0.15}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_air_left$"} {op ">"} {value 0}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 18}}
										("as_poke_faction_air")
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
										("as_poke_faction_line")
									}
								}
							}
							{"case"
								{condition {type rand} {value 0.10}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_ifv_left$"} {op ">"} {value 0}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 17}}
										("as_poke_faction_ifv")
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
										("as_poke_faction_line")
									}
								}
							}
							{"case"
								{condition {type rand} {value 0.12}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 16}}
								("as_poke_faction_veh")
							}
							{"case"
								{condition {type rand} {value 0.12}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 14}}
								("as_poke_faction_eng")
							}
							{"case"
								{condition {type rand} {value 0.18}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 13}}
								("as_poke_faction_assault")
							}
							{"case"
								{condition {type rand} {value 0.28}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 12}}
								("as_poke_faction_recon")
							}
							{"case"
								{condition {type rand} {value 0.4}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 11}}
								("as_poke_faction_wpn")
							}
							{"default"
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
								("as_poke_faction_line")
							}
						}
					}
					{"default"
						{"switch"
							{"case"
								{condition {type rand} {value 0.35}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 12}}
								("as_poke_faction_recon")
							}
							{"default"
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
								("as_poke_faction_line")
							}
						}
					}
				}
			)

			(define "am_pick_composition"
				("as_resolve_army")
				{"switch"
					{"case"
						{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}}
						("as_pick_hybrid_non_nato")
					}
					{"case"
						{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}}
						("as_pick_hybrid_non_nato")
					}
					{"case"
						{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 4}}
						("as_pick_hybrid_non_nato")
					}
					; NATO: specialty mix + hybrid recon/eng/manpad/veh injected at L2/L3.
					{"default"
						{"switch"
							{"case"
								{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
								{"switch"
									{"case"
										{condition {type rand} {value 0.18}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_motor_left$"} {op ">"} {value 0}}
												{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 19}}
												("as_poke_faction_motor")
											}
											{"default"
												{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
												{"trigger" {name "attack_support/comp_usmc"}}
											}
										}
									}
									{"case"
										{condition {type rand} {value 0.1}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 16}}
										("as_poke_faction_veh")
									}
									{"case"
										{condition {type rand} {value 0.1}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 15}}
										("as_poke_faction_manpad")
									}
									{"case"
										{condition {type rand} {value 0.1}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 14}}
										("as_poke_faction_eng")
									}
									{"case"
										{condition {type rand} {value 0.14}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 12}}
										("as_poke_faction_recon")
									}
									{"case"
										{condition {type rand} {value 0.12}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 13}}
										("as_poke_faction_assault")
									}
									{"case"
										{condition {type rand} {value 0.18}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 4}}
										{"trigger" {name "attack_support/comp_pzgren"}}
									}
									{"case"
										{condition {type rand} {value 0.22}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 3}}
										{"trigger" {name "attack_support/comp_acav"}}
									}
									{"case"
										{condition {type rand} {value 0.3}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 5}}
										{"trigger" {name "attack_support/comp_arf"}}
									}
									{"case"
										{condition {type rand} {value 0.5}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 2}}
										{"trigger" {name "attack_support/comp_1ad"}}
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
										{"trigger" {name "attack_support/comp_usmc"}}
									}
								}
							}
							{"case"
								{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
								{"switch"
									{"case"
										{condition {type rand} {value 0.15}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_motor_left$"} {op ">"} {value 0}}
												{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 19}}
												("as_poke_faction_motor")
											}
											{"default"
												{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
												{"trigger" {name "attack_support/comp_usmc"}}
											}
										}
									}
									{"case"
										{condition {type rand} {value 0.1}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 16}}
										("as_poke_faction_veh")
									}
									{"case"
										{condition {type rand} {value 0.1}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 14}}
										("as_poke_faction_eng")
									}
									{"case"
										{condition {type rand} {value 0.15}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 12}}
										("as_poke_faction_recon")
									}
									{"case"
										{condition {type rand} {value 0.12}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 13}}
										("as_poke_faction_assault")
									}
									{"case"
										{condition {type rand} {value 0.22}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 3}}
										{"trigger" {name "attack_support/comp_acav"}}
									}
									{"case"
										{condition {type rand} {value 0.3}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 5}}
										{"trigger" {name "attack_support/comp_arf"}}
									}
									{"case"
										{condition {type rand} {value 0.5}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 2}}
										{"trigger" {name "attack_support/comp_1ad"}}
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
										{"trigger" {name "attack_support/comp_usmc"}}
									}
								}
							}
							{"default"
								{"switch"
									{"case"
										{condition {type rand} {value 0.2}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 12}}
										("as_poke_faction_recon")
									}
									{"case"
										{condition {type rand} {value 0.3}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 5}}
										{"trigger" {name "attack_support/comp_arf"}}
									}
									{"case"
										{condition {type rand} {value 0.5}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 2}}
										{"trigger" {name "attack_support/comp_1ad"}}
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
										{"trigger" {name "attack_support/comp_usmc"}}
									}
								}
							}
						}
					}
				}
				; Pool-short fallback: faction line, then give up.
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
						("as_poke_faction_line")
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "ATTACK SUPPORT POOL SHORT - FACTION LINE"}}
							}
							{"default"}
						}
					}
					{"default"}
				}
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
						("as_announce_wave")
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "ATTACK SUPPORT POOL EXHAUSTED"}}
							}
							{"default"}
						}
					}
					{"default"}
				}
			)

			; Arms once per attack mission. attack_support_use_mi$ is the enable switch:
			; attack_support.lua sets it alongside id_attack_support$, because MI delivery
			; is the only path that works on the attack support slot (Lua Spawn reports no
			; units available there). The clock stays parked behind attack_support_busy$ /
			; attack_support_next_ok$ until the opening wave has been issued, otherwise its
			; condition is already true here and it fires alongside init.
			{"attack_support/init"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "attack_support_armed$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"5.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						; Wait for the published side: placing before it is known drops the first
						; wave on the unpublished-side default pad, i.e. the enemy edge.
						{"6.cmp_i" {var "enemy_spawnside$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_armed$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_flank_rr$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_use_flank$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_ifv_left$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_motor_left$"} {op "="} {value 4}}
					{"set_i" {var "attack_support_motor_pkg$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_motor_first$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_motor_stage$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_motor_drive_t$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_motor_band$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_air_left$"} {op "="} {value 2}}
					; DAY-2 TEST default ON. Set value 0 to restore production airmobile schedule only.
					{"set_i" {var "attack_support_air_test$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
					{"set_i" {var "support_announce$"} {op "="} {value 1}}
					{"set_i" {var "support_e2_test$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_stage$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_fail$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_lz$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_flag$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_para_range_band$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_para_pass$"} {op "="} {value 0}}
					; "This leg is over" is ONE fact with ONE writer (e2_mark_leg_done). The
					; orphan sweep reads it as a tag on the aircraft, the combo transition reads
					; it as this var. Both start cleared.
					{"set_i" {var "support_e2_leg_done$"} {op "="} {value 0}}
					; Bounded-attempt counters for the flight-leg poll and the arrival long-stop.
					{"set_i" {var "support_e2_air_try$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_air_age$"} {op "="} {value 0}}
					; Match clock, one step per 10 seconds. 18 = 180s, the para dispatch floor.
					{"set_i" {var "support_e2_clock_t$"} {op "="} {value 0}}
					{"trigger" {name "attack_support/e2_clock"}}
					{"set_i" {var "attack_support_busy$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_next_ok$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"set_i" {var "attack_support_wave_num$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_hmmwv_left$"} {op "="} {value 4}}
					{"set_i" {var "attack_support_ukr_veh_left$"} {op "="} {value 3}}
					{"set_i" {var "attack_support_nato_veh_left$"} {op "="} {value 2}}
					{"set_i" {var "attack_support_stage$"} {op "="} {value 1}}
					; Wave budget by campaign progression. defense_level$ is computed by the
					; CE mission setup (1-3, roughly campaign games 0 / 6 / 12).
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
							{"set_i" {var "attack_support_waves_left$"} {op "="} {value 8}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 10} {title "ATTACK SUPPORT ARMED - L3, 8 WAVES"}}
								}
								{"default"}
							}
						}
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
							{"set_i" {var "attack_support_waves_left$"} {op "="} {value 6}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 10} {title "ATTACK SUPPORT ARMED - L2, 6 WAVES"}}
								}
								{"default"}
							}
						}
						{"default"
							{"set_i" {var "attack_support_waves_left$"} {op "="} {value 4}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 10} {title "ATTACK SUPPORT ARMED - L1, 4 WAVES"}}
								}
								{"default"}
							}
						}
					}
					; Opening wave ~30s in. DAY-2 TEST (air_test$=1): force airmobile.
					; Set attack_support_air_test$=0 in init to restore production pick only.
					{"delay" {time 30}}
					{"set_i" {var "attack_support_wave_num$"} {op "+"} {value 1}}
					{"set_i" {var "attack_support_waves_left$"} {op "-"} {value 1}}
					("as_resolve_army")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "attack_support_air_test$"} {op "=="} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "attack_support_air_left$"} {op ">"} {value 0}}
									{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 18}}
									("as_poke_faction_air")
								}
								{"default"
									("am_pick_composition")
								}
							}
						}
						{"default"
							("am_pick_composition")
						}
					}
					; Hand the cadence over only once the opening wave has finished landing.
					{"delay" {time 20}}
					{"set_i" {var "attack_support_next_ok$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_busy$"} {op "="} {value 0}}
					{"trigger" {name "attack_support/clock"}}
				}
			}

			; Self-re-arming cadence. One cycle = randomized wait, then either a
			; live-unit-cap defer (no wave consumed) or one randomly picked wave.
			; When the budget runs out the condition stops matching and the loop ends.
			{"attack_support/clock"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "attack_support_next_ok$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "attack_support_busy$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "attack_support_waves_left$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_busy$"} {op "="} {value 1}}
					; Randomized 120-240s cadence (~20% tighter): ~20% per bucket.
					{"switch"
						{"case"
							{condition {type rand} {value 0.2}}
							{"delay" {time 120}}
						}
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 150}}
						}
						{"case"
							{condition {type rand} {value 0.33}}
							{"delay" {time 180}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 210}}
						}
						{"default"
							{"delay" {time 240}}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						; LIVE-UNIT CAP. Skip this cycle without consuming a wave while the
						; teammate still has a platoon on the field. Counted on the simple
						; selector form the mission scripts use for live units - the advanced
						; selector's prop/state decorations zero the match on these entities.
						{"case"
							{condition
								{type entities}
								{selector
									{ignore_captured_by_user 0}
									{tag attack_support_src}
									{type human}
									{state "not dead"}
								}
								{count {op ">"} {value 14}}
							}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 8} {title "ATTACK SUPPORT NEAR CAP DEFER"}}
								}
								{"default"}
							}
						}
						{"default"
							{"set_i" {var "attack_support_wave_num$"} {op "+"} {value 1}}
							{"set_i" {var "attack_support_waves_left$"} {op "-"} {value 1}}
							("am_pick_composition")
						}
					}
					{"delay" {time 0.1}}
					{"set_i" {var "attack_support_busy$"} {op "="} {value 0}}
					{"trigger" {name "attack_support/clock"}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "attack_support_waves_left$"} {op "=="} {value 0}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 10} {title "ATTACK SUPPORT WAVES EXHAUSTED"}}
								}
								{"default"}
							}
						("as_announce_exhausted")
						}
						{"default"}
					}
				}
			}

			; COMPOSITION 1 â€” 5 USMC infantry. Also the pool-short fallback, so this pool
			; is the deepest. Taking the bodies strips the pool tag, so the gate below can
			; simply count the tag to know what is still parked and available.
			{"attack_support/comp_usmc"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_usmc}}
							{count {op ">="} {value 5}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 11}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - USMC RIFLE TEAM"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_usmc}}}}
							{amount 5}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_usmc}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 12}}
				}
			}

			; COMPOSITION 5 â€” 5 ARF / generic NATO infantry (European ARF kit).
			{"attack_support/comp_arf"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 5}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_arf}}
							{count {op ">="} {value 5}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 51}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - ARF RIFLE TEAM"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_arf}}}}
							{amount 5}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_arf}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 52}}
				}
			}

			; COMPOSITION 2 â€” 5 1AD infantry.
			{"attack_support/comp_1ad"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_1ad}}
							{count {op ">="} {value 5}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 21}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - 1AD RIFLE TEAM"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_1ad}}}}
							{amount 5}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_1ad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 22}}
				}
			}

			; COMPOSITION 3 (level 2+) â€” 4 1AD infantry, then one crewed humvee behind
			; them. Infantry first: a vehicle placed into the same spot clips and flips.
			{"attack_support/comp_acav"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 3}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_1ad}}
							{count {op ">="} {value 4}}
						}
						{"5.cmp_i" {var "attack_support_hmmwv_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 31}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - 1AD TEAM + HUMVEE"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_1ad}}}}
							{amount 4}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_1ad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 6} {title "ATTACK SUPPORT HUMVEE MOVING UP"}}
						}
						{"default"}
					}
					("am_deploy_next_hmmwv")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 32}}
				}
			}

			; COMPOSITION 4 (level 3) â€” 6 German pzgren, then two crewed humvees one at a
			; time with a gap between them.
			{"attack_support/comp_pzgren"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 4}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_pzgd}}
							{count {op ">="} {value 6}}
						}
						{"5.cmp_i" {var "attack_support_hmmwv_left$"} {op ">"} {value 1}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 41}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PZGREN + 2 HUMVEES"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_pzgd}}}}
							{amount 6}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_pzgd}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 6} {title "ATTACK SUPPORT HUMVEE 1 MOVING UP"}}
						}
						{"default"}
					}
					("am_deploy_next_hmmwv")
					{"delay" {time 6}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 6} {title "ATTACK SUPPORT HUMVEE 2 MOVING UP"}}
						}
						{"default"}
					}
					("am_deploy_next_hmmwv")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 42}}
				}
			}

			; ===== FACTION LINE / WPN (player nation pools) =====
			; cmd 10 = line (4), cmd 11 = wpn (4). Only the matching army trigger clears.

			{"attack_support/ally_rusa_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 10}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_line}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA LINE"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_line}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_line}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_ukr_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 10}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_ukr_line}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR LINE"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_line}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_line}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_prc_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 10}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_prc_line}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC LINE"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_line}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_line}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 10}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_line}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO LINE"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_line}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_line}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_rusa_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 11}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_wpn}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA WEAPONS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_wpn}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_wpn}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_ukr_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 11}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_ukr_wpn}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR WEAPONS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_wpn}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_wpn}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_prc_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 11}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_prc_wpn}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC WEAPONS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_wpn}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_wpn}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 11}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_wpn}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO WEAPONS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_wpn}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_wpn}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}

			; ===== HYBRID: recon(3) assault(4) eng(3) manpad(2) light veh =====
			{"attack_support/ally_rusa_recon"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 12}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_recon}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA RECON"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_recon}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_recon}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_ukr_recon"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 12}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_ukr_recon}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR RECON"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_recon}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_recon}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_prc_recon"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 12}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_prc_recon}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC RECON"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_recon}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_recon}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_recon"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 12}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_recon}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO RECON"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_recon}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_recon}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_rusa_assault"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 13}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_assault}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA ASSAULT"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_assault}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_assault}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_ukr_assault"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 13}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_ukr_assault}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR ASSAULT"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_assault}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_assault}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_prc_assault"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 13}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_prc_assault}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC ASSAULT"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_assault}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_assault}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_assault"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 13}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_assault}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO ASSAULT"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_assault}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_assault}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_rusa_eng"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 14}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_eng}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA ENGINEERS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_eng}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_eng}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_ukr_eng"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 14}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_ukr_eng}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR ENGINEERS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_eng}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_eng}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_prc_eng"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 14}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_prc_eng}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC ENGINEERS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_eng}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_eng}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_eng"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 14}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_eng}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO ENGINEERS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_eng}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_eng}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_rusa_manpad"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 15}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_manpad}} {count {op ">="} {value 2}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA MANPAD"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_manpad}}}} {amount 2}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_manpad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_ukr_manpad"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 15}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_ukr_manpad}} {count {op ">="} {value 2}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR MANPAD"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_manpad}}}} {amount 2}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_manpad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_prc_manpad"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 15}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_prc_manpad}} {count {op ">="} {value 2}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC MANPAD"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_manpad}}}} {amount 2}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_manpad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_manpad"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 15}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_manpad}} {count {op ">="} {value 2}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO MANPAD"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_manpad}}}} {amount 2}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_manpad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			; Light wheels: one linked hull+crew instance at a time (UKR humvee / NATO fennek).
			{"attack_support/ally_ukr_veh"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 16}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_ukr_veh_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR HUMVEE"}}
						}
						{"default"}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "attack_support_ukr_veh_left$"} {op "=="} {value 3}}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_ukr_veh1}}}}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_ukr_veh}
								{tag_remove ally_sup_ukr_veh1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "attack_support_ukr_veh_left$"} {op "=="} {value 2}}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_ukr_veh2}}}}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_ukr_veh}
								{tag_remove ally_sup_ukr_veh2}
							}
						}
						{"case"
							{condition {type cmp_i} {var "attack_support_ukr_veh_left$"} {op "=="} {value 1}}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_ukr_veh3}}}}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_ukr_veh}
								{tag_remove ally_sup_ukr_veh3}
							}
						}
						{"default"}
					}
					{"set_i" {var "attack_support_ukr_veh_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_veh"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 16}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_nato_veh_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO FENNEK"}}
						}
						{"default"}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "attack_support_nato_veh_left$"} {op "=="} {value 2}}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_nato_veh1}}}}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_nato_veh}
								{tag_remove ally_sup_nato_veh1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "attack_support_nato_veh_left$"} {op "=="} {value 1}}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_nato_veh2}}}}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_nato_veh}
								{tag_remove ally_sup_nato_veh2}
							}
						}
						{"default"}
					}
					{"set_i" {var "attack_support_nato_veh_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			; ===== RARE IFV (cmd 17) =====

			{"attack_support/ally_rusa_ifv"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 17}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_ifv_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_rusa_ifv_inf}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA IFV"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_ifv_inf}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_ifv_inf}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_ifv1}}}}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_ifv}
						{tag_remove ally_sup_rusa_ifv1}
					}
					{"set_i" {var "attack_support_ifv_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}

			{"attack_support/ally_ukr_ifv"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 17}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_ifv_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_ukr_ifv_inf}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR IFV"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_ifv_inf}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_ifv_inf}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_ifv1}}}}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_ifv}
						{tag_remove ally_sup_ukr_ifv1}
					}
					{"set_i" {var "attack_support_ifv_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}

			{"attack_support/ally_prc_ifv"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 17}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_ifv_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_prc_ifv_inf}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC IFV"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_ifv_inf}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_ifv_inf}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_ifv1}}}}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_ifv}
						{tag_remove ally_sup_prc_ifv1}
					}
					{"set_i" {var "attack_support_ifv_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}

			{"attack_support/ally_nato_ifv"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 17}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_ifv_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_nato_ifv_inf}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO IFV"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_ifv_inf}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_ifv_inf}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_ifv1}}}}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_ifv}
						{tag_remove ally_sup_nato_ifv1}
					}
					{"set_i" {var "attack_support_ifv_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			; ===== AIRMOBILE INSERT (cmd 18, Phase 5 E1) =====

			{"attack_support/ally_rusa_air"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 18}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_air_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_airborne")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT AIRMOBILE INSERT"}}
						}
						{"default"}
					}
					{"delay" {time 4.5}}
					; Prefer recon fireteam; fall back to line if recon pool short.
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_rusa_recon}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_rusa_recon}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_rusa_recon}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_rusa_line}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_rusa_line}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_rusa_line}
							}
						}
						{"default"
							{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "ATTACK SUPPORT AIRMOBILE POOL SHORT"}}
							}
							{"default"}
						}
						}
					}
					{"switch"
						{"case"
							{condition {type entities} {selector {tag attack_support_deploy}}}
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
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
								}
								{"default"
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 1}}
									{"switch"
										{"case"
											{condition {type cmp_i} {var "attack_support_flank_rr$"} {op ">="} {value 2}}
											{"set_i" {var "attack_support_flank_rr$"} {op "="} {value 1}}
										}
										{"default"
											{"set_i" {var "attack_support_flank_rr$"} {op "+"} {value 1}}
										}
									}
								}
							}
							{"set_i" {var "attack_support_air_left$"} {op "-"} {value 1}}
							{"delay" {time 0.1}}
							("am_place_at_entry")
							("am_finish_deploy")
							{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
						}
						{"default"}
					}
				}
			}

			{"attack_support/ally_ukr_air"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 18}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_air_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_airborne")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT AIRMOBILE INSERT"}}
						}
						{"default"}
					}
					{"delay" {time 4.5}}
					; Prefer recon fireteam; fall back to line if recon pool short.
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_ukr_recon}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_ukr_recon}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_ukr_recon}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_ukr_line}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_ukr_line}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_ukr_line}
							}
						}
						{"default"
							{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "ATTACK SUPPORT AIRMOBILE POOL SHORT"}}
							}
							{"default"}
						}
						}
					}
					{"switch"
						{"case"
							{condition {type entities} {selector {tag attack_support_deploy}}}
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
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
								}
								{"default"
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 1}}
									{"switch"
										{"case"
											{condition {type cmp_i} {var "attack_support_flank_rr$"} {op ">="} {value 2}}
											{"set_i" {var "attack_support_flank_rr$"} {op "="} {value 1}}
										}
										{"default"
											{"set_i" {var "attack_support_flank_rr$"} {op "+"} {value 1}}
										}
									}
								}
							}
							{"set_i" {var "attack_support_air_left$"} {op "-"} {value 1}}
							{"delay" {time 0.1}}
							("am_place_at_entry")
							("am_finish_deploy")
							{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
						}
						{"default"}
					}
				}
			}

			{"attack_support/ally_prc_air"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 18}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_air_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_airborne")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT AIRMOBILE INSERT"}}
						}
						{"default"}
					}
					{"delay" {time 4.5}}
					; Prefer recon fireteam; fall back to line if recon pool short.
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_prc_recon}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_prc_recon}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_prc_recon}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_prc_line}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_prc_line}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_prc_line}
							}
						}
						{"default"
							{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "ATTACK SUPPORT AIRMOBILE POOL SHORT"}}
							}
							{"default"}
						}
						}
					}
					{"switch"
						{"case"
							{condition {type entities} {selector {tag attack_support_deploy}}}
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
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
								}
								{"default"
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 1}}
									{"switch"
										{"case"
											{condition {type cmp_i} {var "attack_support_flank_rr$"} {op ">="} {value 2}}
											{"set_i" {var "attack_support_flank_rr$"} {op "="} {value 1}}
										}
										{"default"
											{"set_i" {var "attack_support_flank_rr$"} {op "+"} {value 1}}
										}
									}
								}
							}
							{"set_i" {var "attack_support_air_left$"} {op "-"} {value 1}}
							{"delay" {time 0.1}}
							("am_place_at_entry")
							("am_finish_deploy")
							{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
						}
						{"default"}
					}
				}
			}

			{"attack_support/ally_nato_air"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 18}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_air_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_airborne")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT AIRMOBILE INSERT"}}
						}
						{"default"}
					}
					{"delay" {time 4.5}}
					; Prefer recon fireteam; fall back to line if recon pool short.
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_nato_recon}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_nato_recon}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_nato_recon}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_nato_line}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_nato_line}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_nato_line}
							}
						}
						{"default"
							{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "ATTACK SUPPORT AIRMOBILE POOL SHORT"}}
							}
							{"default"}
						}
						}
					}
					{"switch"
						{"case"
							{condition {type entities} {selector {tag attack_support_deploy}}}
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
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
								}
								{"default"
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 1}}
									{"switch"
										{"case"
											{condition {type cmp_i} {var "attack_support_flank_rr$"} {op ">="} {value 2}}
											{"set_i" {var "attack_support_flank_rr$"} {op "="} {value 1}}
										}
										{"default"
											{"set_i" {var "attack_support_flank_rr$"} {op "+"} {value 1}}
										}
									}
								}
							}
							{"set_i" {var "attack_support_air_left$"} {op "-"} {value 1}}
							{"delay" {time 0.1}}
							("am_place_at_entry")
							("am_finish_deploy")
							{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
						}
						{"default"}
					}
				}
			}

			; ===== E2 REAL AIR INSERT PROBES =====
			; Default-off live flight proof. Gate 1 flies a helo; gate 2 is reserved for
			; Task 4's real paradrop. Forced probes never touch production wave budgets.

			(define "e2_reset_target"
				{"entity_state" {selector {tag support_e2_flag_target}} {tag_remove support_e2_flag_target}}
				{"set_i" {var "support_e2_flag$"} {op "="} {value 0}}
				{"set_i" {var "support_e2_lz$"} {op "="} {value 0}}
				; Closest-approach tracker state belongs to one run-in only.
				{"set_i" {var "support_e2_para_range_band$"} {op "="} {value 0}}
				{"set_i" {var "support_e2_para_pass$"} {op "="} {value 0}}
			)

			(define "e2_announce_helo"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector {ignore_captured_by_user 0} {amount 1} {tag player} {zone "gamezone"} {type human} {state "not dead"}}
							{text "mission/multi/support/e2_helo_inbound"}
						}
					}
					{"default"}
				}
			)

			(define "e2_announce_failed"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector {ignore_captured_by_user 0} {amount 1} {tag player} {zone "gamezone"} {type human} {state "not dead"}}
							{text "mission/multi/support/e2_insert_failed"}
						}
					}
					{"default"}
				}
			)

			; Both tags are required: support_e2_aircraft also exists on unclaimed parked
			; templates, which must never be deleted by another package's cleanup.
			; ===== E2 EVIDENCE GATES =====
			; A stage may only advance on proof that the entity that stage talks about
			; actually exists. Every gate below counts CLAIMED entities; none of them is a
			; timer. The live defect these close: a claim that returned nothing still walked
			; the whole ladder to stage 70, and every action in between silently no-opped, so
			; the run reported a clean flight that no player ever saw.
			;
			; Fail codes (stable - append only, never renumber):
			;   1  faction/mode switch fell through: army has no package of this kind
			;   2  child trigger claimed nothing (stage still 10 one second after dispatch)
			;   3  no active flag to fly at
			;   4  both deep LZ pads unsafe (helo leg falls back to the entry standoff)
			;   5  helo never reached the LZ inside the arrival window
			;   6  para plane never released its jumpers inside the window
			;   7  no paratrooper survived the landing
			;   8  owner id unresolved, so the package could not be transferred
			;   9  the claim produced no aircraft at all
			;   10 the claimed aircraft was not operatable when the flight leg began
			;   11 the insert landed no live body
			;   12 faction army still unresolved at dispatch
			;   13 the parked airframe was absent at dispatch: this package's own pool
			;      tag and support_e2_aircraft never intersected on one live entity, so
			;      there was nothing for the claim selector to take
			;   14 the clone never arrived: {"actor_to_waypoint"} ... {clone} produced no
			;      entity for the destination waypoint's {commands} block to tag
			;
			; 13 vs 9 is the discriminator for an airframe whose def loads but whose parked
			; actor may never instantiate: 13 means the entity was never there, 9 means it
			; was there and the claim selector still did not take it. The probes below run
			; BEFORE any claim, which is the only point at which the two are separable.
			(define "e2_prove_park_rusa_helo"
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector {ignore_captured_by_user 0} {tag support_e2_rusa_helo} {tag support_e2_aircraft}}
							{count {op ">="} {value 1}}
						}
					}
					{"default"
						{"set_i" {var "support_e2_fail$"} {op "="} {value 13}}
						("e2_fail_and_cleanup")
					}
				}
			)

			(define "e2_prove_park_ukr_helo"
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector {ignore_captured_by_user 0} {tag support_e2_ukr_helo} {tag support_e2_aircraft}}
							{count {op ">="} {value 1}}
						}
					}
					{"default"
						{"set_i" {var "support_e2_fail$"} {op "="} {value 13}}
						("e2_fail_and_cleanup")
					}
				}
			)

			(define "e2_prove_park_nato_helo"
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector {ignore_captured_by_user 0} {tag support_e2_nato_helo} {tag support_e2_aircraft}}
							{count {op ">="} {value 1}}
						}
					}
					{"default"
						{"set_i" {var "support_e2_fail$"} {op "="} {value 13}}
						("e2_fail_and_cleanup")
					}
				}
			)

			(define "e2_prove_park_prc_helo"
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector {ignore_captured_by_user 0} {tag support_e2_prc_helo} {tag support_e2_aircraft}}
							{count {op ">="} {value 1}}
						}
					}
					{"default"
						{"set_i" {var "support_e2_fail$"} {op "="} {value 13}}
						("e2_fail_and_cleanup")
					}
				}
			)

			(define "e2_prove_park_rusa_para"
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector {ignore_captured_by_user 0} {tag support_e2_rusa_para} {tag support_e2_aircraft}}
							{count {op ">="} {value 1}}
						}
					}
					{"default"
						{"set_i" {var "support_e2_fail$"} {op "="} {value 13}}
						("e2_fail_and_cleanup")
					}
				}
			)

			(define "e2_prove_park_ukr_para"
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector {ignore_captured_by_user 0} {tag support_e2_ukr_para} {tag support_e2_aircraft}}
							{count {op ">="} {value 1}}
						}
					}
					{"default"
						{"set_i" {var "support_e2_fail$"} {op "="} {value 13}}
						("e2_fail_and_cleanup")
					}
				}
			)

			(define "e2_prove_park_nato_para"
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector {ignore_captured_by_user 0} {tag support_e2_nato_para} {tag support_e2_aircraft}}
							{count {op ">="} {value 1}}
						}
					}
					{"default"
						{"set_i" {var "support_e2_fail$"} {op "="} {value 13}}
						("e2_fail_and_cleanup")
					}
				}
			)

; Only a claimed helicopter may open the flight leg. Without this the
			; {"action" move} had no target, the 40s window expired and the run blamed the
			; flight (fail 5) for an aircraft that was never there.
			;
			; ===== BARE-TAG EVIDENCE, SECOND LIVE PROOF (2026-07-30) =====
			; This gate used to carry a SIMPLE-selector {state operatable} decoration and it
			; matched nothing on a helicopter the player watched fly across the map and hover
			; over the objective. The run recorded fail 10 ("the claimed aircraft was not
			; operatable") at dispatch, one second after the clone arrived, while the aircraft
			; was demonstrably alive and airborne - and because the flight leg was abandoned,
			; the departure order and the delete were never issued either and the hull hovered
			; there for the rest of the mission.
			; That is the same failure the deploy guard has banned in its ADVANCED form,
			; the doubly-nested advanced spelling, since the first proof: a state decoration on a
			; selector zeroes the match on these units. The simple form is now banned too.
			; Every E2 liveness/existence gate below is therefore a BARE-TAG count. Where a
			; gate genuinely has to exclude corpses it uses the advanced
			; {group {select ...} {exclude {state {state dead}} {state {state inactive}}}}
			; form, which is proven in this file, never a selector decoration.
			; ===== THE LIVE-ARRIVAL INTERLOCK (2026-07-30, THIRD LIVE PROOF) =====
			; A HEALTHY FLYING AIRCRAFT IS UN-FAILABLE. The run that forced this rule: the
			; player watched the clone spawn and fly, and the mirror still reported fail 10
			; from the {"default"} branch below. e2_prove_arrival had already passed, so the
			; arrival existed - what stopped holding was the CO-RESIDENCE of the three
			; lifecycle tags on it. Independent evidence for the same loss is recorded a few
			; hundred lines down, on e2_order_aircraft_exit: "the claim-keyed order matched
			; nothing and the aircraft never left the objective", where the claim-keyed
			; selector is {tag support_e2_aircraft} {tag support_e2_claim} - two of these
			; same three tags - while the arrival-keyed order, on the tag the numeric
			; waypoint's own {commands} block wrote, did match. So the tags this engine adds
			; across the promote/own window are the ones that do not survive; the waypoint's
			; own tag does.
			;
			; Three things follow, and all three are implemented:
			;   1. The lifecycle tags are RE-ASSERTED off support_e2_arrival after the
			;      ownership transfer and after the {"actor_state"} step, not only before.
			;   2. The gate is sampled by a bounded, self-re-arming POLL rather than once.
			;   3. No "the aircraft is not there" code (9, 10, 14) may be written while an
			;      entity carrying support_e2_arrival exists and is not dead. Liveness is
			;      expressed with the ADVANCED exclude form that is proven in this file,
			;      never a simple-selector state decoration.
			(define "e2_fly_helo_or_fail"
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft} {tag support_e2_claim}}
							{count {op ">="} {value 1}}
						}
						("e2_order_aircraft_lz")
						{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}
					}
					{"default"
					{"switch"
						{"case"
							; A live arrival exists: this leg has a real aircraft in the air and
							; may not be failed, whatever the bookkeeping tags say. The poll
							; re-arms and tries again.
							{condition
								{type entities}
								{selector
									{source advanced}
									{group
										{select {tag {tag support_e2_arrival}}}
										{exclude {state {state dead}} {state {state inactive}}}
									}
								}
								{count {op ">="} {value 1}}
							}
						}
						{"default"
						; An earlier code wins: 14 (the clone never arrived) must not be
						; overwritten by 10 ("the aircraft was not operatable").
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
								{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}
							}
							{"default"}
						}
					}
					}
				}
				}
			)

			(define "e2_fly_para_or_fail"
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector {ignore_captured_by_user 0} {tag support_e2_plane} {tag support_e2_aircraft} {tag support_e2_claim}}
							{count {op ">="} {value 1}}
						}
						{"action"
							{selector {ignore_captured_by_user 0} {tag support_e2_plane} {tag support_e2_aircraft} {tag support_e2_claim}}
							{drop orders}
							{action move}
							{target {ignore_captured_by_user 0} {tag support_e2_flag_target}}
						}
						{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}
					}
					{"default"
					{"switch"
						{"case"
							; Same interlock as the helo gate: a live transport is un-failable.
							{condition
								{type entities}
								{selector
									{source advanced}
									{group
										{select {tag {tag support_e2_arrival}}}
										{exclude {state {state dead}} {state {state inactive}}}
									}
								}
								{count {op ">="} {value 1}}
							}
							; The plane is real and flying; order it at the objective off the
							; provenance tag, which no promote step can take away.
							{"action"
								{selector {ignore_captured_by_user 0} {tag support_e2_arrival}}
								{drop orders}
								{action move}
								{target {ignore_captured_by_user 0} {tag support_e2_flag_target}}
							}
							; ...AND OPEN THE RUN-IN. This branch used to order the plane and leave
							; the stage at 20, which is why the 2026-07-31 run flew a C-130 across the
							; whole map and dropped nobody: the range poll, all three release monitors
							; and the stage-30 liveness monitor are ALL gated on stage 30, so none of
							; them ever armed and e2_para_band/e2_para_pass sat at 0 for the entire
							; flight. The evidence for the write is the entity proof directly above -
							; the same reason the helo leg poll opens its leg on a live arrival rather
							; than failing a healthy hull.
							{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}
						}
						{"default"
						; An earlier code wins: 14 (the clone never arrived) must not be
						; overwritten by 10 ("the aircraft was not operatable").
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
								{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}
							}
							{"default"}
						}
					}
					}
				}
				}
			)

			; The lifecycle tags, re-asserted off the ONE handle that provably survives the
			; ownership transfer: support_e2_arrival, written by the numeric waypoint's own
			; {commands} block. This is the same shape as as_finish_motor's re-assert of
			; attack_support_deploy from the package's own marks before ownership and AI
			; control are set - the fix that stopped trucks being left neutral and motionless.
			(define "e2_reassert_arrival_helo"
				{"entity_state"
					{selector {tag support_e2_arrival}}
					{tag_add support_e2_helo}
					{tag_add support_e2_aircraft}
					{tag_add support_e2_claim}
					{tag_add attack_support_src}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
			)

			(define "e2_reassert_arrival_plane"
				{"entity_state"
					{selector {tag support_e2_arrival}}
					{tag_add support_e2_plane}
					{tag_add support_e2_aircraft}
					{tag_add support_e2_claim}
					{tag_add attack_support_src}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
			)

			; ===== THE ONE DEFINITION OF "THIS LEG IS OVER" =====
			; Written by, and only by, the paths that genuinely end a leg: both cleanups
			; (which every completion, timeout and real failure routes through) and the
			; combo leftover-retirement. It is published twice on purpose, because the two
			; readers can only see one form each:
			;   - as a TAG on the aircraft, for the orphan sweep, which has no way to ask
			;     "is this particular hull's leg finished" other than off the hull itself;
			;   - as a VAR, for the combo transition, which has to know the helo leg is over
			;     at a moment when the aircraft has already been deleted.
			; The global stage variable is NOT that fact: stage reaches 70 on the combo
			; transition too, and keying the sweep on it deleted a healthy mid-mission
			; aircraft in the 2026-07-30 run.
			(define "e2_mark_leg_done"
				{"entity_state" {selector {tag support_e2_arrival}} {tag_add support_e2_leg_done}}
				{"set_i" {var "support_e2_leg_done$"} {op "="} {value 1}}
			)

			(define "e2_arm_helo_leg"
				{"trigger" {name "attack_support/e2_helo_leg_poll"}}
			)

			; Stage 50 means "emitted passengers are owned and moving". It may only be
			; reached after a body receives support_e2_pax on the grounded unload path; an
			; already-set fail code still wins.
			;
			; ===== THE LANDED-EVIDENCE CHECK, KEYED ON THE SAME TAG AS THE TRANSFER =====
			; This gate used to count support_e2_team through
			; {exclude {state {state dead}} {state {state inactive}}}, and on 2026-07-31 it
			; returned ZERO while the player was looking at four troops standing at the LZ.
			; e2_place_one re-adds support_e2_team to every body it places, so the tag was
			; certainly there; the only other term in that condition is the state exclusion,
			; which is the same class of decoration that has already zeroed two E2 matches on
			; units that were demonstrably alive. So the corpse filter is gone and the count
			; is a BARE TAG count - and it costs nothing, because support_e2_pax is not a
			; claim marker a corpse could still be carrying from before the drop: it is
			; written by the one {"entity_state"} that completes a MOVE placement, so
			; counting it IS counting deliveries.
			;
			; Ownership is transferred first, on that same tag. NO E2 BODY MAY BE DELIVERED
			; WITHOUT AN OWNERSHIP TRANSFER: both delivery paths (the LZ unload and the
			; entry standoff) end here, so this is the one place that has to guarantee it.
			(define "e2_finish_team_or_fail"
				("e2_own_pax")
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector {ignore_captured_by_user 0} {tag support_e2_pax}}
							{count {op ">="} {value 1}}
						}
						("e2_order_team")
						{"set_i" {var "support_e2_stage$"} {op "="} {value 50}}
					}
					{"default"
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
								{"set_i" {var "support_e2_fail$"} {op "="} {value 11}}
							}
							{"default"}
						}
					}
				}
			)

			; Two sweeps, deliberately, and neither can touch a parked template.
			;   1. The claim-keyed one. Both tags are required: support_e2_aircraft also
			;      exists on unclaimed parked templates, which must never be deleted by
			;      another package's cleanup.
			;   2. The provenance-keyed backstop. support_e2_arrival is written ONLY by the
			;      numeric waypoint's own {commands} block, so it exists on every dispatched
			;      clone and on nothing else in the mission - no parked template can ever
			;      carry it. Live 2026-07-30: a leg that abandoned itself at dispatch left a
			;      flying clone behind because the claim-keyed sweep matched nothing, and the
			;      player watched it hover over the objective for the rest of the mission.
			;      An orphaned aircraft is the one outcome this system may never produce.
			(define "e2_delete_aircraft"
				{"delete" {selector {ignore_captured_by_user 0} {tag support_e2_aircraft} {tag support_e2_claim}}}
				{"delete" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}}}
			)

			(define "e2_fail_and_cleanup"
				; The leg is genuinely over: mark it BEFORE the delete, so that if the delete
				; does not take, the hull is left carrying its own terminal marker and the
				; orphan sweep can retire it.
				("e2_mark_leg_done")
				("e2_delete_aircraft")
				{"entity_state" {selector {tag support_e2_src}} {tag_remove support_e2_src}}
				; The provenance tag is the orphan sweep's ONLY handle on a dispatched clone.
				; Stripping it unconditionally would blind the sweep to exactly the hull it
				; exists for - one that survived the delete above. So it is stripped only when
				; nothing live is still carrying it; otherwise the hull keeps it, keeps the
				; terminal marker set moments ago, and the sweep retires it.
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector
								{source advanced}
								{group
									{select {tag {tag support_e2_arrival}}}
									{exclude {state {state dead}} {state {state inactive}}}
								}
							}
							{count {op ">="} {value 1}}
						}
					}
					{"default"
						{"entity_state" {selector {tag support_e2_arrival}} {tag_remove support_e2_arrival}}
					}
				}
				{"entity_state" {selector {tag support_e2_claim}} {tag_remove support_e2_claim}}
				{"entity_state" {selector {tag support_e2_team}} {tag_remove support_e2_team}}
				{"entity_state" {selector {tag support_e2_pax}} {tag_remove support_e2_pax}}
				{"entity_state" {selector {tag support_e2_helo}} {tag_remove support_e2_helo}}
				{"entity_state" {selector {tag support_e2_plane}} {tag_remove support_e2_plane}}
				{"entity_state" {selector {tag support_e2_released}} {tag_remove support_e2_released}}
				{"entity_state" {selector {tag support_e2_lz_marker}} {tag_remove support_e2_lz_marker}}
				{"entity_state" {selector {tag support_e2_lz_done}} {tag_remove support_e2_lz_done}}
				{"entity_state" {selector {tag support_e2_place_one}} {tag_remove support_e2_place_one}}
				{"entity_state" {selector {tag support_e2_placed}} {tag_remove support_e2_placed}}
				{"entity_state" {selector {tag support_e2_flag_target}} {tag_remove support_e2_flag_target}}
				{"set_i" {var "support_e2_stage$"} {op "="} {value 70}}
				("e2_announce_failed")
			)

			(define "e2_complete_cleanup"
				("e2_mark_leg_done")
				("e2_delete_aircraft")
				{"entity_state" {selector {tag support_e2_src}} {tag_remove support_e2_src}}
				; The provenance tag is the orphan sweep's ONLY handle on a dispatched clone.
				; Stripping it unconditionally would blind the sweep to exactly the hull it
				; exists for - one that survived the delete above. So it is stripped only when
				; nothing live is still carrying it; otherwise the hull keeps it, keeps the
				; terminal marker set moments ago, and the sweep retires it.
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector
								{source advanced}
								{group
									{select {tag {tag support_e2_arrival}}}
									{exclude {state {state dead}} {state {state inactive}}}
								}
							}
							{count {op ">="} {value 1}}
						}
					}
					{"default"
						{"entity_state" {selector {tag support_e2_arrival}} {tag_remove support_e2_arrival}}
					}
				}
				{"entity_state" {selector {tag support_e2_claim}} {tag_remove support_e2_claim}}
				{"entity_state" {selector {tag support_e2_pax}} {tag_remove support_e2_pax}}
				{"entity_state" {selector {tag support_e2_helo}} {tag_remove support_e2_helo}}
				{"entity_state" {selector {tag support_e2_plane}} {tag_remove support_e2_plane}}
				{"entity_state" {selector {tag support_e2_released}} {tag_remove support_e2_released}}
				{"entity_state" {selector {tag support_e2_lz_marker}} {tag_remove support_e2_lz_marker}}
				{"entity_state" {selector {tag support_e2_lz_done}} {tag_remove support_e2_lz_done}}
				{"entity_state" {selector {tag support_e2_place_one}} {tag_remove support_e2_place_one}}
				{"entity_state" {selector {tag support_e2_placed}} {tag_remove support_e2_placed}}
				{"entity_state" {selector {tag support_e2_flag_target}} {tag_remove support_e2_flag_target}}
				{"set_i" {var "support_e2_stage$"} {op "="} {value 70}}
			)

			(define "e2_choose_flag"
				("e2_reset_target")
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag flag}}} {exclude {state {state inactive}}}}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add support_e2_flag_target}
				}
				{"delay" {time 0.1}}
				{"switch"
					{"case"
						{condition {type entities} {selector {tag support_e2_flag_target}}}
						; Some managed maps do not expose numbered capture tags. A value of 1
						; therefore records the honest portable fact: one active target was selected.
						{"set_i" {var "support_e2_flag$"} {op "="} {value 1}}
					}
					{"default"
						; The caller owns the single abort/cleanup path.
						{"set_i" {var "support_e2_fail$"} {op "="} {value 3}}
					}
				}
			)

			(define "e2_own_current"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "16"}}}
					{"default" {"set_i" {var "support_e2_fail$"} {op "="} {value 8}}}
				}
			)

			; ===== THE DELIVERED BODIES GET AN OWNER, AFTER THEY LAND =====
			; Live 2026-07-31: the helicopter flew its whole route, four troops appeared at
			; the LZ, and every one of them was an UNAFFILIATED WHITE DOT on the minimap -
			; while the same run recorded fail 11, "the insert landed no live body". One
			; defect with two faces: nothing had transferred the passengers.
			;
			; e2_own_current runs at stage 20, when the four bodies are still HIDDEN and
			; INACTIVE parked templates - and it was the only ownership step the insert had.
			; The proven order in this engine is the opposite one: am_finish_deploy and
			; as_finish_motor both place first, strip {hidden}/{inactive} second, and only
			; THEN call am_own_to_support, and those are the paths that have never produced
			; a neutral body. e2_own_current is kept exactly as it is (it is also the early
			; fail-8 guard on an unresolved id); this is the transfer that actually lands,
			; taken on support_e2_pax, which exists only on a body that has been placed and
			; unhidden.
			;
			; Literal 1-16 cases with a fail-closed default: an unresolved id transfers
			; nothing and records 8 rather than guessing a player.
			(define "e2_own_pax"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_pax}} {operation set} {player "16"}}}
					{"default" {"set_i" {var "support_e2_fail$"} {op "="} {value 8}}}
				}
			)

			(define "e2_place_one"
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag support_e2_team}}} {exclude {tag {tag support_e2_placed}}}}
						{amount 1}
					}
					{tag_add support_e2_place_one}
					{tag_remove support_e2_team}
				}
				{"delay" {time 0.05}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"switch"
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_air_a2"}}}
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 1}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_air_a1"}}}
							{"default" {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target {ignore_captured_by_user 0} {amount 1} {tag spawn_a}}}}
						}
					}
					{"default"
						{"switch"
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_air_b2"}}}
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 1}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_air_b1"}}}
							{"default" {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target {ignore_captured_by_user 0} {amount 1} {tag spawn_b}}}}
						}
					}
				}
				{"entity_state"
					{selector {tag support_e2_place_one}}
					{tag_add support_e2_team}
					{tag_add support_e2_placed}
					; THE PAX TAG. Written at the one instant a body provably exists on the
					; ground as a delivered unit: by the same {"entity_state"} that takes it out
					; of {hidden}/{inactive} once the MOVE placement has landed. It is the handle
					; e2_own_pax transfers ownership on, the handle e2_order_team orders, and the
					; handle e2_finish_team_or_fail counts - so "this body was delivered", "this
					; body belongs to the support player" and "this body has an order" are one
					; fact with one witness instead of three tags that can drift apart.
					{tag_add support_e2_pax}
					{tag_add attack_support_src}
					{tag_remove ally_sup_tpl}
					{tag_remove support_e2_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
					{tag_remove support_e2_place_one}
				}
				{"delay" {time 0.5}}
			)

			(define "e2_place_one_entry"
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag support_e2_team}}} {exclude {tag {tag support_e2_placed}}}}
						{amount 1}
					}
					{tag_add support_e2_place_one}
					{tag_remove support_e2_team}
				}
				{"delay" {time 0.05}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_entry_b1"}}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_entry_a1"}}
					}
					{"default"
						{"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_entry_b1"}}
					}
				}
				{"entity_state"
					{selector {tag support_e2_place_one}}
					{tag_add support_e2_team}
					{tag_add support_e2_placed}
					; THE PAX TAG. Written at the one instant a body provably exists on the
					; ground as a delivered unit: by the same {"entity_state"} that takes it out
					; of {hidden}/{inactive} once the MOVE placement has landed. It is the handle
					; e2_own_pax transfers ownership on, the handle e2_order_team orders, and the
					; handle e2_finish_team_or_fail counts - so "this body was delivered", "this
					; body belongs to the support player" and "this body has an order" are one
					; fact with one witness instead of three tags that can drift apart.
					{tag_add support_e2_pax}
					{tag_add attack_support_src}
					{tag_remove ally_sup_tpl}
					{tag_remove support_e2_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
					{tag_remove support_e2_place_one}
				}
				{"delay" {time 0.5}}
			)

			; Ordered on the PAX tag, not on support_e2_team. The team tag is added at claim
			; time on a still-parked template, and e2_place_one removes and re-adds it around
			; every placement; support_e2_pax exists only on a body that has actually landed.
			(define "e2_order_team"
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag support_e2_pax}}
					{control AI}
					{ai_move {mode enable}}
					{weapon_prepare on}
					{fire_mode open}
					{move_mode free}
					{movement {speed normal} {kind normal} {type normal}}
					{ai {no_retreat on} {advance_ratio 1} {retreat_ratio 0}}
				}
				{"action"
					{selector {ignore_captured_by_user 0} {tag support_e2_pax}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag support_e2_flag_target}}
				}
			)

			; Probe each numbered air pad independently with the attested near condition.
			; If both are unsafe, lz=0 means infantry-only standoff at the corresponding
			; numbered entry waypoint and fail code 4.
			(define "e2_claim_lz_marker"
				{"entity_state"
					{selector {ignore_captured_by_user 0} {tag support_e2_marker_tpl} {amount 1}}
					{tag_add support_e2_lz_marker}
				}
			)

			(define "e2_place_lz_marker"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"switch"
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {target_waypoint "attack_support_air_b2"}}}
							{"default" {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {target_waypoint "attack_support_air_b1"}}}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"switch"
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {target_waypoint "attack_support_air_a2"}}}
							{"default" {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {target_waypoint "attack_support_air_a1"}}}
						}
					}
					{"default"
						{"switch"
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {target_waypoint "attack_support_air_b2"}}}
							{"default" {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {target_waypoint "attack_support_air_b1"}}}
						}
					}
				}
			)

			; Anchor each safety query to the candidate pad with one still-hidden claimed
			; team human. The inactive marker cannot match the operatable-units query itself.
			(define "e2_choose_lz"
				("e2_claim_lz_marker")
				{"switch"
					{"case" {condition {type rand} {value 0.5}} {"set_i" {var "support_e2_lz$"} {op "="} {value 2}}}
					{"default" {"set_i" {var "support_e2_lz$"} {op "="} {value 1}}}
				}
				("e2_place_lz_marker")
				{"delay" {time 0.05}}
				{"switch"
					{"case"
						{condition
							{type near}
							{units {ignore_captured_by_user 0} {tag _bot} {type human} {state operatable}}
							{near_to {ignore_captured_by_user 0} {tag support_e2_lz_marker}}
							{distance 120}
						}
						{"switch"
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 1}} {"set_i" {var "support_e2_lz$"} {op "="} {value 2}}}
							{"default" {"set_i" {var "support_e2_lz$"} {op "="} {value 1}}}
						}
						("e2_place_lz_marker")
						{"delay" {time 0.05}}
						{"switch"
							{"case"
								{condition
									{type near}
									{units {ignore_captured_by_user 0} {tag _bot} {type human} {state operatable}}
									{near_to {ignore_captured_by_user 0} {tag support_e2_lz_marker}}
									{distance 120}
								}
								{"set_i" {var "support_e2_fail$"} {op "="} {value 4}}
								{"set_i" {var "support_e2_lz$"} {op "="} {value 0}}
							}
							{"default"}
						}
					}
					{"default"}
				}
			)

			; ===== E2 AIRCRAFT DISPATCH: CLONE + TELEPORT + WAYPOINT COMMANDS =====
			; This is the base game's OWN aircraft call-in recipe, adopted verbatim in
			; shape. Three parts, and all three are required:
			;
			;   1. {"actor_to_waypoint"} with {source advanced}, an {include {tag {tag
			;      hidden}}} clause (without it the selector cannot reach a hidden parked
			;      template at all), {amount 1}, a NUMERIC {waypoint}, {clone} and
			;      {approach "safe teleport & rotate"}.
			;   2. The destination waypoint's own {commands} block, which the deploy
			;      generator writes into every managed map. It runs ON THE ARRIVING ACTOR
			;      and is how the base game defeats "a clone carries none of the parked
			;      template's tags": it re-tags the arrival, sets its actor state and
			;      issues the first move order.
			;   3. This engine then promotes and refines what the commands block tagged.
			;
			; {clone} is banned everywhere else in this repo by the marching orders and
			; the ban is test-pinned. It is authorised for E2 AIRCRAFT DISPATCH ONLY, on
			; the base game's own evidence. Nothing else in resource/ may use it.
			;
			; The consequence worth stating: the parked originals are never consumed, so
			; every aircraft call-in is repeatable and aircraft cost the pool nothing. The
			; only budgeted part of an E2 insert is the infantry it delivers.
			;
			; support_e2_src marks the ONE parked hull this dispatch will copy. It is not
			; a claim: the hull keeps its pool tag and stays parked. Cleared immediately
			; after the clone so a later dispatch starts from a clean pool.
			(define "e2_clone_aircraft"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"actor_to_waypoint"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{select {tag {tag support_e2_src}}}
									{include {tag {tag hidden}}}
								}
								{amount 1}
							}
							{waypoint "21"}
							{clone}
							{approach "safe teleport & rotate"}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"actor_to_waypoint"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{select {tag {tag support_e2_src}}}
									{include {tag {tag hidden}}}
								}
								{amount 1}
							}
							{waypoint "22"}
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
									{select {tag {tag support_e2_src}}}
									{include {tag {tag hidden}}}
								}
								{amount 1}
							}
							{waypoint "22"}
							{clone}
							{approach "safe teleport & rotate"}
						}
					}
				}
				{"delay" {time 1}}
				{"entity_state" {selector {tag support_e2_src}} {tag_remove support_e2_src}}
			)

			; The arrival is whatever the waypoint {commands} block tagged. It is a fresh
			; entity: it is NOT hidden, it is NOT a template and it carries no pool tag,
			; so promotion is only about ownership and the engine's own bookkeeping tags.
			(define "e2_promote_arrival"
				{"entity_state"
					{selector {tag support_e2_arrival}}
					{tag_add support_e2_claim}
					{tag_add support_e2_aircraft}
					{tag_add attack_support_src}
					{tag_remove ally_sup_tpl}
					{tag_remove support_e2_tpl}
					{tag_remove support_e2_src}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
				; Both bounded counters belong to ONE arrival: the flight-leg poll's attempt
				; count and the long-stop's age. A new aircraft starts both at zero.
				{"set_i" {var "support_e2_air_try$"} {op "="} {value 0}}
				{"set_i" {var "support_e2_air_age$"} {op "="} {value 0}}
			)

			(define "e2_own_arrival"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {operation set} {player "16"}}}
					{"default" {"set_i" {var "support_e2_fail$"} {op "="} {value 8}}}
				}
			)

			; Fail 14 = the clone never arrived: actor_to_waypoint produced nothing the
			; waypoint {commands} block could tag. That is distinct from 13 (the parked
			; hull was absent before the dispatch) and from 9 (an arrival existed but the
			; leg selector did not take it).
			(define "e2_prove_arrival"
				{"switch"
					{"case"
						{condition
							{type entities}
							{selector {ignore_captured_by_user 0} {tag support_e2_arrival}}
							{count {op ">="} {value 1}}
						}
					}
					{"default" {"set_i" {var "support_e2_fail$"} {op "="} {value 14}}}
				}
			)

			(define "e2_arrival_is_helo"
				{"entity_state" {selector {tag support_e2_arrival}} {tag_add support_e2_helo}}
			)

			(define "e2_arrival_is_plane"
				{"entity_state" {selector {tag support_e2_arrival}} {tag_add support_e2_plane}}
			)

			; Each case issues the LZ order TWICE, against two different selectors, for the
			; same reason e2_order_aircraft_exit and e2_delete_aircraft each sweep twice: the
			; helo-keyed order is this engine's own bookkeeping and the arrival-keyed order is
			; the provenance backstop that no promote step and no ownership transfer can take
			; away. Re-issuing the same {drop orders} {action move} to the same waypoint is
			; idempotent.
			(define "e2_order_aircraft_lz"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}}
								{"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_air_b2"}}
								{"action" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {drop orders} {action move} {waypoint "attack_support_air_b2"}}
							}
							{"default" {"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_air_b1"}} {"action" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {drop orders} {action move} {waypoint "attack_support_air_b1"}}}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}}
								{"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_air_a2"}}
								{"action" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {drop orders} {action move} {waypoint "attack_support_air_a2"}}
							}
							{"default" {"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_air_a1"}} {"action" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {drop orders} {action move} {waypoint "attack_support_air_a1"}}}
						}
					}
					{"default" {"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_air_b1"}} {"action" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {drop orders} {action move} {waypoint "attack_support_air_b1"}}}
				}
			)


			; ===== TRUE HELICOPTER TRANSPORT: DESCEND, GROUND, EMIT, LIFT =====
			; Shipped evidence used here:
			;   * Gostomel routes lower real Mi-8/Mi-17 helicopters with the air_state command
			;     before their waypoint-command passenger emit.
			;   * Code:X helicopter.inc's continuously re-arming altitude_checker writes
			;     w81_landing only below altitude 5 and removes it again in flight.
			; The LZ trigger therefore requests altitude 0, but delivery is not credited
			; until the aircraft itself reports w81_landing. No timed MOVE placement can
			; satisfy that gate.
			(define "e2_descend_helo"
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag support_e2_arrival}}
					{movement {speed sneak}}
				}
				{"air_state"
					{selector {ignore_captured_by_user 0} {tag support_e2_arrival}}
					{altitude 0}
				}
			)

			; Emit only the four linked passenger templates. {crew {tag ...}} is used as
			; the filter it is in shipped missions, never as a tag assignment. The chosen
			; named LZ waypoint supplies a ground destination, matching Gostomel's emit.
			(define "e2_emit_helo_pax"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}}
								{"emit" {selector {ignore_captured_by_user 0} {tag support_e2_arrival} {type vehicle} {state inhabited}} {waypoint "attack_support_air_b2"} {crew {tag support_e2_helo_pax}} {emit {mode passengers}}}
							}
							{"default" {"emit" {selector {ignore_captured_by_user 0} {tag support_e2_arrival} {type vehicle} {state inhabited}} {waypoint "attack_support_air_b1"} {crew {tag support_e2_helo_pax}} {emit {mode passengers}}}}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}}
								{"emit" {selector {ignore_captured_by_user 0} {tag support_e2_arrival} {type vehicle} {state inhabited}} {waypoint "attack_support_air_a2"} {crew {tag support_e2_helo_pax}} {emit {mode passengers}}}
							}
							{"default" {"emit" {selector {ignore_captured_by_user 0} {tag support_e2_arrival} {type vehicle} {state inhabited}} {waypoint "attack_support_air_a1"} {crew {tag support_e2_helo_pax}} {emit {mode passengers}}}}
						}
					}
					{"default" {"emit" {selector {ignore_captured_by_user 0} {tag support_e2_arrival} {type vehicle} {state inhabited}} {waypoint "attack_support_air_b1"} {crew {tag support_e2_helo_pax}} {emit {mode passengers}}}}
				}
			)

			; The parked originals remain hidden off-map. Only emitted copies are visible,
			; so the advanced exclusion is the provenance boundary between the reusable
			; pool and this delivery.
			(define "e2_promote_helo_pax"
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag support_e2_helo_pax}}}
							{exclude {tag {tag hidden}}}
						}
					}
					{tag_add support_e2_pax}
					{tag_add support_e2_team}
					{tag_add attack_support_src}
					{tag_remove support_e2_helo_pax}
					{tag_remove support_e2_tpl}
					{tag_remove ally_sup_tpl}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
			)

			; Raise the grounded helicopter back to its verified travel altitude before
			; issuing the off-map route. altitude_checker removes w81_landing as it climbs.
			(define "e2_lift_helo"
				{"air_state"
					{selector {ignore_captured_by_user 0} {tag support_e2_arrival}}
					{altitude 22}
				}
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag support_e2_arrival}}
					{movement {speed fast}}
				}
			)

			; Exit across the map to the OPPOSITE side's numeric altitude node, so the
			; aircraft flies through and off rather than turning back over the objective.
			; 23/24 are the altitude-carrying pair (z in the position, no radius and
			; no commands block) - the same shape the base game gives its mid-route air
			; nodes. The engine deletes the hull afterwards regardless.
			;
			; Each case issues the departure TWICE, against two different selectors, for the
			; same reason e2_delete_aircraft sweeps twice: the claim-keyed order is the
			; engine's own bookkeeping and the arrival-keyed order is the provenance backstop
			; that no parked template can satisfy. Live 2026-07-30: the claim-keyed order
			; matched nothing and the aircraft never left the objective. Re-issuing the same
			; {drop orders} {action move} to the same waypoint is idempotent.
			(define "e2_order_aircraft_exit"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"action" {selector {ignore_captured_by_user 0} {tag support_e2_aircraft} {tag support_e2_claim}} {drop orders} {action move} {waypoint "23"}}
						{"action" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {drop orders} {action move} {waypoint "23"}}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"action" {selector {ignore_captured_by_user 0} {tag support_e2_aircraft} {tag support_e2_claim}} {drop orders} {action move} {waypoint "24"}}
						{"action" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {drop orders} {action move} {waypoint "24"}}
					}
					{"default"
						{"action" {selector {ignore_captured_by_user 0} {tag support_e2_aircraft} {tag support_e2_claim}} {drop orders} {action move} {waypoint "23"}}
						{"action" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {drop orders} {action move} {waypoint "23"}}
					}
				}
			)

			{"attack_support/e2_helo_rusa"
				{condition
					{expression "1 & 2 & 3 & 4 & (5 | 11) & 6 & 7 & 8 & 9 & 10"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_e2_test$"} {op "=="} {value 1}}
						{"11.cmp_i" {var "support_e2_test$"} {op "=="} {value 3}}
						{"6.cmp_i" {var "support_e2_stage$"} {op "=="} {value 10}}
						{"7.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"8.entities" {selector {tag support_e2_rusa_helo}} {count {op ">="} {value 1}}}
						{"9.entities" {selector {tag support_e2_rusa_helo_crew}} {count {op ">="} {value 2}}}
						{"10.entities" {selector {tag support_e2_rusa_helo_team}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					("e2_prove_park_rusa_helo")
					; Nothing may be claimed until the parked airframe is proved present.
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
							; The aircraft, pilots and four seat-linked passengers are CLONED as one
							; reusable transport package. The parked source keeps every pool tag; only
							; the emitted clone passengers become support units at the grounded LZ.
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag support_e2_rusa_helo}}}} {amount 1}}
								{tag_add support_e2_src}
							}
							("e2_choose_flag")
							; Stage 20 claims "a target has been accepted". The evidence for that claim is
							; the target entity itself, not the absence of a failure so far.
							{"switch"
								{"case"
									{condition {type entities} {selector {tag support_e2_flag_target}} {count {op ">="} {value 1}}}
									{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}
									{"switch"
										{"case"
											{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
											("e2_choose_lz")
											{"switch"
												{"case"
																	{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 4}}
																	; Both deep pads are unsafe. A true transport does not detach its
																	; still-linked passengers and fake an insert at the entry edge.
																	("e2_announce_failed")
																	("e2_complete_cleanup")
																}
												{"default"
													; Clone + teleport to the numeric air-entry waypoint. That waypoint's own
													; {commands} block re-tags the arrival, sets its actor state and issues the
													; first move order. No air_state verb and no placement verb: altitude is
													; parked chassis snapshot's job and the teleport is the dispatch verb's.
													("e2_clone_aircraft")
													("e2_prove_arrival")
													("e2_arrival_is_helo")
													("e2_promote_arrival")
													("e2_own_arrival")
													{"actor_state"
														{selector {ignore_captured_by_user 0} {tag support_e2_arrival}}
														{drop sensor}
														{control AI}
														{movement {speed fast}}
													}
													; RE-ASSERT AFTER THE OWNERSHIP TRANSFER, not only before it. e2_promote_arrival
													; ran before ("e2_own_arrival") and before the {"actor_state"} step, and the
													; 2026-07-30 run proves its tags did not survive that window: the claim-keyed
													; selectors matched nothing on a helicopter the player watched fly, while the
													; arrival tag the numeric waypoint wrote still matched. So the lifecycle tags
													; are written again here, off that surviving handle.
													("e2_reassert_arrival_helo")
													; The clone was tagged by the waypoint {commands} block and re-tagged twice
													; since; give the tags a tick to become readable before anything reads them.
													{"delay" {time 0.5}}
													; Evidence gate: a claimed helicopter, or fail 10 and no flight leg.
													("e2_fly_helo_or_fail")
													; ONE SAMPLE IS NOT EVIDENCE. If the first read misses, the leg is not failed:
													; attack_support/e2_helo_leg_poll re-asserts the tags and re-reads them, up to a
													; bounded number of attempts, and opens the flight leg as soon as the evidence is
													; co-resident. Only an exhausted bound with NO live arrival may write fail 10.
													("e2_arm_helo_leg")
													; Long enough for the whole bounded poll (20 attempts at 0.5s) plus trigger
													; latency to conclude one way or the other before this list judges the leg.
													{"delay" {time 25}}
													{"switch"
														{"case"
															{condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 30}}
															; From here the unload chain is armed off the AIRCRAFT'S ARRIVAL, never
															; off this action list. attack_support/e2_helo_lz fires on a near check
															; against the LZ marker and owns stages 40 and 60;
															; attack_support/e2_helo_timeout closes the window. Both always end in a
															; departure order and a delete, so no evidence gate can orphan a hull.
														}
														{"default"
															; The flight leg was refused (fail 10, or an earlier code). The leg is
															; over - but the clone is real and in the air, so it is sent off the map
															; and deleted before anything else. This is the path that used to just
															; abandon the aircraft.
															("e2_order_aircraft_exit")
															{"delay" {time 10}}
															("e2_fail_and_cleanup")
														}
													}
												}
											}
										}
										{"default" ("e2_fail_and_cleanup")}
									}
								}
								{"default" ("e2_fail_and_cleanup")}
							}
						}
						{"default"}
					}
				}
			}
			{"attack_support/e2_helo_prc"
				{condition
					{expression "1 & 2 & 3 & 4 & (5 | 11) & 6 & 7 & 8 & 9 & 10"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_e2_test$"} {op "=="} {value 1}}
						{"11.cmp_i" {var "support_e2_test$"} {op "=="} {value 3}}
						{"6.cmp_i" {var "support_e2_stage$"} {op "=="} {value 10}}
						{"7.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"8.entities" {selector {tag support_e2_prc_helo}} {count {op ">="} {value 1}}}
						{"9.entities" {selector {tag support_e2_prc_helo_crew}} {count {op ">="} {value 2}}}
						{"10.entities" {selector {tag support_e2_prc_helo_team}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					("e2_prove_park_prc_helo")
					; Nothing may be claimed until the parked airframe is proved present.
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
							; The aircraft, pilots and four seat-linked passengers are CLONED as one
							; reusable transport package. The parked source keeps every pool tag; only
							; the emitted clone passengers become support units at the grounded LZ.
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag support_e2_prc_helo}}}} {amount 1}}
								{tag_add support_e2_src}
							}
							("e2_choose_flag")
							; Stage 20 claims "a target has been accepted". The evidence for that claim is
							; the target entity itself, not the absence of a failure so far.
							{"switch"
								{"case"
									{condition {type entities} {selector {tag support_e2_flag_target}} {count {op ">="} {value 1}}}
									{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}
									{"switch"
										{"case"
											{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
											("e2_choose_lz")
											{"switch"
												{"case"
																	{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 4}}
																	; Both deep pads are unsafe. A true transport does not detach its
																	; still-linked passengers and fake an insert at the entry edge.
																	("e2_announce_failed")
																	("e2_complete_cleanup")
																}
												{"default"
													; Clone + teleport to the numeric air-entry waypoint. That waypoint's own
													; {commands} block re-tags the arrival, sets its actor state and issues the
													; first move order. No air_state verb and no placement verb: altitude is
													; parked chassis snapshot's job and the teleport is the dispatch verb's.
													("e2_clone_aircraft")
													("e2_prove_arrival")
													("e2_arrival_is_helo")
													("e2_promote_arrival")
													("e2_own_arrival")
													{"actor_state"
														{selector {ignore_captured_by_user 0} {tag support_e2_arrival}}
														{drop sensor}
														{control AI}
														{movement {speed fast}}
													}
													; RE-ASSERT AFTER THE OWNERSHIP TRANSFER, not only before it. e2_promote_arrival
													; ran before ("e2_own_arrival") and before the {"actor_state"} step, and the
													; 2026-07-30 run proves its tags did not survive that window: the claim-keyed
													; selectors matched nothing on a helicopter the player watched fly, while the
													; arrival tag the numeric waypoint wrote still matched. So the lifecycle tags
													; are written again here, off that surviving handle.
													("e2_reassert_arrival_helo")
													; The clone was tagged by the waypoint {commands} block and re-tagged twice
													; since; give the tags a tick to become readable before anything reads them.
													{"delay" {time 0.5}}
													; Evidence gate: a claimed helicopter, or fail 10 and no flight leg.
													("e2_fly_helo_or_fail")
													; ONE SAMPLE IS NOT EVIDENCE. If the first read misses, the leg is not failed:
													; attack_support/e2_helo_leg_poll re-asserts the tags and re-reads them, up to a
													; bounded number of attempts, and opens the flight leg as soon as the evidence is
													; co-resident. Only an exhausted bound with NO live arrival may write fail 10.
													("e2_arm_helo_leg")
													; Long enough for the whole bounded poll (20 attempts at 0.5s) plus trigger
													; latency to conclude one way or the other before this list judges the leg.
													{"delay" {time 25}}
													{"switch"
														{"case"
															{condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 30}}
															; From here the unload chain is armed off the AIRCRAFT'S ARRIVAL, never
															; off this action list. attack_support/e2_helo_lz fires on a near check
															; against the LZ marker and owns stages 40 and 60;
															; attack_support/e2_helo_timeout closes the window. Both always end in a
															; departure order and a delete, so no evidence gate can orphan a hull.
														}
														{"default"
															; The flight leg was refused (fail 10, or an earlier code). The leg is
															; over - but the clone is real and in the air, so it is sent off the map
															; and deleted before anything else. This is the path that used to just
															; abandon the aircraft.
															("e2_order_aircraft_exit")
															{"delay" {time 10}}
															("e2_fail_and_cleanup")
														}
													}
												}
											}
										}
										{"default" ("e2_fail_and_cleanup")}
									}
								}
								{"default" ("e2_fail_and_cleanup")}
							}
						}
						{"default"}
					}
				}
			}


			{"attack_support/e2_helo_ukr"
				{condition
					{expression "1 & 2 & 3 & 4 & (5 | 11) & 6 & 7 & 8 & 9 & 10"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_e2_test$"} {op "=="} {value 1}}
						{"11.cmp_i" {var "support_e2_test$"} {op "=="} {value 3}}
						{"6.cmp_i" {var "support_e2_stage$"} {op "=="} {value 10}}
						{"7.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"8.entities" {selector {tag support_e2_ukr_helo}} {count {op ">="} {value 1}}}
						{"9.entities" {selector {tag support_e2_ukr_helo_crew}} {count {op ">="} {value 2}}}
						{"10.entities" {selector {tag support_e2_ukr_helo_team}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					("e2_prove_park_ukr_helo")
					; Nothing may be claimed until the parked airframe is proved present.
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
							; The aircraft, pilots and four seat-linked passengers are CLONED as one
							; reusable transport package. The parked source keeps every pool tag; only
							; the emitted clone passengers become support units at the grounded LZ.
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag support_e2_ukr_helo}}}} {amount 1}}
								{tag_add support_e2_src}
							}
							("e2_choose_flag")
							; Stage 20 claims "a target has been accepted". The evidence for that claim is
							; the target entity itself, not the absence of a failure so far.
							{"switch"
								{"case"
									{condition {type entities} {selector {tag support_e2_flag_target}} {count {op ">="} {value 1}}}
									{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}
									{"switch"
										{"case"
											{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
											("e2_choose_lz")
											{"switch"
												{"case"
																	{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 4}}
																	; Both deep pads are unsafe. A true transport does not detach its
																	; still-linked passengers and fake an insert at the entry edge.
																	("e2_announce_failed")
																	("e2_complete_cleanup")
																}
												{"default"
													; Clone + teleport to the numeric air-entry waypoint. That waypoint's own
													; {commands} block re-tags the arrival, sets its actor state and issues the
													; first move order. No air_state verb and no placement verb: altitude is
													; parked chassis snapshot's job and the teleport is the dispatch verb's.
													("e2_clone_aircraft")
													("e2_prove_arrival")
													("e2_arrival_is_helo")
													("e2_promote_arrival")
													("e2_own_arrival")
													{"actor_state"
														{selector {ignore_captured_by_user 0} {tag support_e2_arrival}}
														{drop sensor}
														{control AI}
														{movement {speed fast}}
													}
													; RE-ASSERT AFTER THE OWNERSHIP TRANSFER, not only before it. e2_promote_arrival
													; ran before ("e2_own_arrival") and before the {"actor_state"} step, and the
													; 2026-07-30 run proves its tags did not survive that window: the claim-keyed
													; selectors matched nothing on a helicopter the player watched fly, while the
													; arrival tag the numeric waypoint wrote still matched. So the lifecycle tags
													; are written again here, off that surviving handle.
													("e2_reassert_arrival_helo")
													; The clone was tagged by the waypoint {commands} block and re-tagged twice
													; since; give the tags a tick to become readable before anything reads them.
													{"delay" {time 0.5}}
													; Evidence gate: a claimed helicopter, or fail 10 and no flight leg.
													("e2_fly_helo_or_fail")
													; ONE SAMPLE IS NOT EVIDENCE. If the first read misses, the leg is not failed:
													; attack_support/e2_helo_leg_poll re-asserts the tags and re-reads them, up to a
													; bounded number of attempts, and opens the flight leg as soon as the evidence is
													; co-resident. Only an exhausted bound with NO live arrival may write fail 10.
													("e2_arm_helo_leg")
													; Long enough for the whole bounded poll (20 attempts at 0.5s) plus trigger
													; latency to conclude one way or the other before this list judges the leg.
													{"delay" {time 25}}
													{"switch"
														{"case"
															{condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 30}}
															; From here the unload chain is armed off the AIRCRAFT'S ARRIVAL, never
															; off this action list. attack_support/e2_helo_lz fires on a near check
															; against the LZ marker and owns stages 40 and 60;
															; attack_support/e2_helo_timeout closes the window. Both always end in a
															; departure order and a delete, so no evidence gate can orphan a hull.
														}
														{"default"
															; The flight leg was refused (fail 10, or an earlier code). The leg is
															; over - but the clone is real and in the air, so it is sent off the map
															; and deleted before anything else. This is the path that used to just
															; abandon the aircraft.
															("e2_order_aircraft_exit")
															{"delay" {time 10}}
															("e2_fail_and_cleanup")
														}
													}
												}
											}
										}
										{"default" ("e2_fail_and_cleanup")}
									}
								}
								{"default" ("e2_fail_and_cleanup")}
							}
						}
						{"default"}
					}
				}
			}


			{"attack_support/e2_helo_nato"
				{condition
					{expression "1 & 2 & 3 & 4 & (5 | 11) & 6 & 7 & 8 & 9 & 10"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_e2_test$"} {op "=="} {value 1}}
						{"11.cmp_i" {var "support_e2_test$"} {op "=="} {value 3}}
						{"6.cmp_i" {var "support_e2_stage$"} {op "=="} {value 10}}
						{"7.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"8.entities" {selector {tag support_e2_nato_helo}} {count {op ">="} {value 1}}}
						{"9.entities" {selector {tag support_e2_nato_helo_crew}} {count {op ">="} {value 2}}}
						{"10.entities" {selector {tag support_e2_nato_helo_team}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					("e2_prove_park_nato_helo")
					; Nothing may be claimed until the parked airframe is proved present.
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
							; The aircraft, pilots and four seat-linked passengers are CLONED as one
							; reusable transport package. The parked source keeps every pool tag; only
							; the emitted clone passengers become support units at the grounded LZ.
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag support_e2_nato_helo}}}} {amount 1}}
								{tag_add support_e2_src}
							}
							("e2_choose_flag")
							; Stage 20 claims "a target has been accepted". The evidence for that claim is
							; the target entity itself, not the absence of a failure so far.
							{"switch"
								{"case"
									{condition {type entities} {selector {tag support_e2_flag_target}} {count {op ">="} {value 1}}}
									{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}
									{"switch"
										{"case"
											{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
											("e2_choose_lz")
											{"switch"
												{"case"
																	{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 4}}
																	; Both deep pads are unsafe. A true transport does not detach its
																	; still-linked passengers and fake an insert at the entry edge.
																	("e2_announce_failed")
																	("e2_complete_cleanup")
																}
												{"default"
													; Clone + teleport to the numeric air-entry waypoint. That waypoint's own
													; {commands} block re-tags the arrival, sets its actor state and issues the
													; first move order. No air_state verb and no placement verb: altitude is
													; parked chassis snapshot's job and the teleport is the dispatch verb's.
													("e2_clone_aircraft")
													("e2_prove_arrival")
													("e2_arrival_is_helo")
													("e2_promote_arrival")
													("e2_own_arrival")
													{"actor_state"
														{selector {ignore_captured_by_user 0} {tag support_e2_arrival}}
														{drop sensor}
														{control AI}
														{movement {speed fast}}
													}
													; RE-ASSERT AFTER THE OWNERSHIP TRANSFER, not only before it. e2_promote_arrival
													; ran before ("e2_own_arrival") and before the {"actor_state"} step, and the
													; 2026-07-30 run proves its tags did not survive that window: the claim-keyed
													; selectors matched nothing on a helicopter the player watched fly, while the
													; arrival tag the numeric waypoint wrote still matched. So the lifecycle tags
													; are written again here, off that surviving handle.
													("e2_reassert_arrival_helo")
													; The clone was tagged by the waypoint {commands} block and re-tagged twice
													; since; give the tags a tick to become readable before anything reads them.
													{"delay" {time 0.5}}
													; Evidence gate: a claimed helicopter, or fail 10 and no flight leg.
													("e2_fly_helo_or_fail")
													; ONE SAMPLE IS NOT EVIDENCE. If the first read misses, the leg is not failed:
													; attack_support/e2_helo_leg_poll re-asserts the tags and re-reads them, up to a
													; bounded number of attempts, and opens the flight leg as soon as the evidence is
													; co-resident. Only an exhausted bound with NO live arrival may write fail 10.
													("e2_arm_helo_leg")
													; Long enough for the whole bounded poll (20 attempts at 0.5s) plus trigger
													; latency to conclude one way or the other before this list judges the leg.
													{"delay" {time 25}}
													{"switch"
														{"case"
															{condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 30}}
															; From here the unload chain is armed off the AIRCRAFT'S ARRIVAL, never
															; off this action list. attack_support/e2_helo_lz fires on a near check
															; against the LZ marker and owns stages 40 and 60;
															; attack_support/e2_helo_timeout closes the window. Both always end in a
															; departure order and a delete, so no evidence gate can orphan a hull.
														}
														{"default"
															; The flight leg was refused (fail 10, or an earlier code). The leg is
															; over - but the clone is real and in the air, so it is sent off the map
															; and deleted before anything else. This is the path that used to just
															; abandon the aircraft.
															("e2_order_aircraft_exit")
															{"delay" {time 10}}
															("e2_fail_and_cleanup")
														}
													}
												}
											}
										}
										{"default" ("e2_fail_and_cleanup")}
									}
								}
								{"default" ("e2_fail_and_cleanup")}
							}
						}
						{"default"}
					}
				}
			}


			; ===== E2 HELO ARRIVAL: THE UNLOAD CHAIN IS ARMED OFF THE AIRCRAFT =====
			; It used to hang off a fixed 40-second delay inside the dispatch action list,
			; followed by a near check. When the dispatch-time evidence gate false-failed
			; (2026-07-30, fail 10 on a helicopter the player watched fly), that whole action
			; list abandoned the leg - and the physical clone simply kept the last order it
			; had, the one the waypoint {commands} block gave it. It hovered over the
			; objective for the rest of the mission: no unload, no departure, no delete.
			;
			; These two triggers are the base game's own shape instead. e2_helo_lz is a
			; radius/near condition on the ARRIVED, re-tagged aircraft and owns stages 40 and
			; 60; e2_helo_timeout closes the window. Both are top-level condition-waiters,
			; exactly like attack_support/e2_para_release_* and attack_support/e2_para_alive,
			; so nothing has to remember to arm them. Whichever fires, the aircraft is ordered
			; off the map and deleted - a leg that fails its evidence gate still departs.
			; support_e2_lz_done makes the unload one-shot per leg; both cleanups clear it.
			{"attack_support/e2_helo_lz"
				{condition
					{expression "1 & 2 & (3 | 4) & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_stage$"} {op "=="} {value 30}}
						{"3.cmp_i" {var "support_e2_test$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "support_e2_test$"} {op "=="} {value 3}}
						{"5.entities" {selector {source advanced} {group {select {tag {tag support_e2_arrival}}} {exclude {tag {tag support_e2_lz_done}}}}}}
						{"6.near" {units {ignore_captured_by_user 0} {tag support_e2_arrival}} {near_to {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {distance 120}}
					}
				}
				{actions
					{"entity_state" {selector {tag support_e2_arrival}} {tag_add support_e2_lz_done}}
					("e2_reassert_arrival_helo")
					("e2_announce_helo")
					; Stage 35 is a descent in progress. No troop exists on the ground yet.
					{"set_i" {var "support_e2_stage$"} {op "="} {value 35}}
					("e2_descend_helo")
				}
			}

			; Delivery is credited only after Code:X helicopter.inc reports the aircraft
			; below its landing threshold. This is the actual land/disembark boundary.
			{"attack_support/e2_helo_grounded"
				{condition
					{expression "1 & 2 & (3 | 4) & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_stage$"} {op "=="} {value 35}}
						{"3.cmp_i" {var "support_e2_test$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "support_e2_test$"} {op "=="} {value 3}}
						{"5.entities" {selector {ignore_captured_by_user 0} {tag support_e2_arrival} {tag w81_landing}} {count {op ">="} {value 1}}}
						{"6.near" {units {ignore_captured_by_user 0} {tag support_e2_arrival}} {near_to {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {distance 120}}
					}
				}
				{actions
					("e2_reassert_arrival_helo")
					{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}
					("e2_emit_helo_pax")
					{"delay" {time 1}}
					("e2_promote_helo_pax")
					("e2_finish_team_or_fail")
					("e2_lift_helo")
					{"delay" {time 4}}
					("e2_order_aircraft_exit")
					{"set_i" {var "support_e2_stage$"} {op "="} {value 60}}
					{"delay" {time 60}}
					{"switch"
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}} ("e2_complete_cleanup")}
						{"default" ("e2_fail_and_cleanup")}
					}
				}
			}

			; The arrival window. Armed for the whole of stage 30, so its clock starts the
			; moment the flight leg opens. If e2_helo_lz got there first the stage has already
			; left 30 and this does nothing; otherwise the helo never reached the LZ (fail 5,
			; and only when nothing earlier already failed, so 9/10/13/14 are never buried).
			{"attack_support/e2_helo_timeout"
				{condition
					{expression "1 & 2 & (3 | 4)"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_stage$"} {op "=="} {value 30}}
						{"3.cmp_i" {var "support_e2_test$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "support_e2_test$"} {op "=="} {value 3}}
					}
				}
				{actions
					{"delay" {time 60}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 30}}
							{"switch" {"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}} {"set_i" {var "support_e2_fail$"} {op "="} {value 5}}} {"default"}}
							("e2_order_aircraft_exit")
							{"delay" {time 10}}
							("e2_fail_and_cleanup")
						}
						{"case"
							; The aircraft reached the LZ but never crossed the grounded evidence gate.
							{condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 35}}
							{"switch" {"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}} {"set_i" {var "support_e2_fail$"} {op "="} {value 5}}} {"default"}}
							("e2_lift_helo")
							{"delay" {time 4}}
							("e2_order_aircraft_exit")
							{"delay" {time 10}}
							("e2_fail_and_cleanup")
						}
						{"default"}
					}
				}
			}

			; ===== THE BOUNDED FLIGHT-LEG POLL =====
			; The gate that wrote fail 10 on a healthy flying helicopter was a ONE-SHOT sample
			; taken 0.5s after an ownership transfer that the tags did not survive. This is the
			; same gate, re-read: it re-asserts the lifecycle tags off support_e2_arrival and
			; re-samples, up to support_e2_air_try$ = 20 attempts at 0.5s each, and opens the
			; flight leg the moment the evidence is co-resident.
			;
			; The bound is not a licence to fail. When it is exhausted the poll asks the only
			; question that actually matters - is there a live aircraft in the sky - in the
			; ADVANCED exclude form proven in this file, and if there is, the leg is OPENED,
			; because a healthy flying helicopter must be un-failable. Fail 10 is reachable only
			; from the branch where the bound is exhausted AND nothing carrying support_e2_arrival
			; is alive - the one state in which "the aircraft was not there" is a true statement.
			{"attack_support/e2_helo_leg_poll"
				{condition
					{expression "1 & 2 & (3 | 4) & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						; Stage 20 is "a target is accepted and the aircraft has been dispatched".
						; The moment the leg opens, stage leaves 20 and this poll stops.
						{"2.cmp_i" {var "support_e2_stage$"} {op "=="} {value 20}}
						{"3.cmp_i" {var "support_e2_test$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "support_e2_test$"} {op "=="} {value 3}}
						{"5.entities" {selector {tag support_e2_arrival}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					("e2_reassert_arrival_helo")
					{"delay" {time 0.5}}
					("e2_fly_helo_or_fail")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 30}}
							; The leg is open. The attempt counter belongs to one arrival only.
							{"set_i" {var "support_e2_air_try$"} {op "="} {value 0}}
						}
						{"default"
							{"set_i" {var "support_e2_air_try$"} {op "+"} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_e2_air_try$"} {op "<"} {value 20}}
									{"trigger" {name "attack_support/e2_helo_leg_poll"}}
								}
								{"default"
									{"switch"
										{"case"
											{condition
												{type entities}
												{selector
													{source advanced}
													{group
														{select {tag {tag support_e2_arrival}}}
														{exclude {state {state dead}} {state {state inactive}}}
													}
												}
												{count {op ">="} {value 1}}
											}
											; The bookkeeping tags never became readable, but there IS an
											; aircraft in the air. That is the evidence that counts: open the
											; leg rather than fail a healthy hull. e2_order_aircraft_lz issues
											; its move arrival-keyed as well as helo-keyed, so the order lands
											; whatever the bookkeeping tags did.
											("e2_order_aircraft_lz")
											{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}
											{"set_i" {var "support_e2_air_try$"} {op "="} {value 0}}
										}
										{"default"
											; Bound exhausted and nothing alive carries the provenance tag.
											; Only here is "the aircraft was not there" a true statement. An
											; earlier code still wins.
											{"switch"
												{"case"
													{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
													{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}
												}
												{"default"}
											}
										}
									}
								}
							}
						}
					}
				}
			}

			; ===== THE HARD INTERLOCK =====
			; A HEALTHY FLYING AIRCRAFT IS UN-FAILABLE, enforced as a standing trigger rather
			; than trusted to every call site. The three codes it retires are exactly the codes
			; that ASSERT THE AIRCRAFT IS NOT THERE - 9 ("the claim produced no aircraft"),
			; 10 ("the claimed aircraft was not operatable") and 14 ("the clone never arrived").
			; Each is a factual claim about the world, and each is false whenever an entity
			; carrying support_e2_arrival exists and is not dead. So while such an entity exists
			; and its own leg has not been marked terminal, those three codes are cleared
			; wherever they came from.
			;
			; It deliberately does NOT touch the outcome codes (4 both pads unsafe, 5 never
			; reached the LZ, 6 never released, 7 no survivor, 11 no live body). Those say
			; nothing about the hull's existence, they are written only after the leg has been
			; ordered off the map, and erasing them would fabricate a success - the exact class
			; of lie this whole evidence ladder exists to prevent.
			;
			; Liveness is the ADVANCED {group {select ...}} {exclude {state {state dead}}
			; {state {state inactive}}} form proven in this file. Never a simple-selector state
			; decoration: that spelling is what zeroed the match and produced fail 10 in the
			; first place, and the deploy bans it on every support_e2_ selector.
			{"attack_support/e2_fail_interlock"
				{condition
					{expression "1 & (2 | 5 | 6) & 3 & !4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_fail$"} {op "=="} {value 9}}
						{"5.cmp_i" {var "support_e2_fail$"} {op "=="} {value 10}}
						{"6.cmp_i" {var "support_e2_fail$"} {op "=="} {value 14}}
						{"3.entities" {selector {source advanced} {group {select {tag {tag support_e2_arrival}}} {exclude {state {state dead}} {state {state inactive}}}}} {count {op ">="} {value 1}}}
						; ...and this aircraft's own leg has not been declared over yet.
						{"4.entities" {selector {tag support_e2_arrival} {tag support_e2_leg_done}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "support_e2_fail$"} {op "="} {value 0}}
					{"delay" {time 1}}
					{"trigger" {name "attack_support/e2_fail_interlock"}}
				}
			}

			; ===== THE ARRIVAL LONG-STOP =====
			; The guarantee that no aircraft is ever left flying forever, keyed to THAT
			; AIRCRAFT'S OWN LEG rather than to the global stage variable. The stage var is not
			; a statement about a hull: it reaches 70 on the combo transition too, which is how
			; the previous sweep came to delete a healthy mid-mission helicopter.
			;
			; This polls in 20s steps and counts only CONSECUTIVE observations of an arrival
			; whose leg is still un-terminated; a leg that ends resets the count, because the
			; terminal marker is then present. 12 consecutive steps is 240 seconds - longer than
			; the longest legitimate leg in this system (helo: a 60s arrival window plus a 60s
			; post-unload cleanup; para: 90s plus a 29s survivor window plus 10s) - after which
			; the leg is declared over and the orphan sweep below retires the hull.
			{"attack_support/e2_arrival_longstop"
				{condition
					{expression "1 & 2 & !3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.entities" {selector {tag support_e2_arrival}} {count {op ">="} {value 1}}}
						{"3.entities" {selector {tag support_e2_arrival} {tag support_e2_leg_done}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"delay" {time 20}}
					{"switch"
						{"case"
							; The leg ended during the step: nothing to long-stop.
							{condition {type entities} {selector {tag support_e2_arrival} {tag support_e2_leg_done}} {count {op ">="} {value 1}}}
							{"set_i" {var "support_e2_air_age$"} {op "="} {value 0}}
						}
						{"case"
							{condition {type entities} {selector {tag support_e2_arrival}} {count {op ">="} {value 1}}}
							{"set_i" {var "support_e2_air_age$"} {op "+"} {value 1}}
						}
						{"default" {"set_i" {var "support_e2_air_age$"} {op "="} {value 0}}}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_air_age$"} {op ">="} {value 12}}
							("e2_mark_leg_done")
							{"set_i" {var "support_e2_air_age$"} {op "="} {value 0}}
						}
						{"default"}
					}
					{"trigger" {name "attack_support/e2_arrival_longstop"}}
				}
			}

			; ===== THE MATCH CLOCK =====
			; SetVar takes integers only, so elapsed time is carried the way this engine already
			; carries it everywhere else: as a counted step, one per fixed {"delay"}. Same shape
			; as as_finish_motor's drive clock (attack_support_motor_drive_t$, one step per 7
			; real seconds) and the same self-re-arming loop as attack_support/clock and
			; attack_support/e2_para_range. One step per 10 seconds, armed once by
			; attack_support/init, so support_e2_clock_t$ >= 18 reads "at least 180 seconds of
			; match time have passed". No float timer and nothing to publish from Lua.
			{"attack_support/e2_clock"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_armed$"} {op "=="} {value 1}}
					}
				}
				{actions
					{"delay" {time 10}}
					{"set_i" {var "support_e2_clock_t$"} {op "+"} {value 1}}
					{"trigger" {name "attack_support/e2_clock"}}
				}
			}

			; LAST-RESORT ORPHAN SWEEP. Nothing above may leave an aircraft in the sky, but
			; this is the guarantee rather than the intention: any dispatched clone whose OWN
			; LEG has reached a terminal state is ordered off the map and deleted.
			;
			; IT KEYS ON THE TERMINAL MARKER, NEVER ON THE GLOBAL STAGE VARIABLE. It used to
			; fire on support_e2_stage$ >= 70 plus the mere presence of an arrival - and stage
			; reaches 70 on the combo transition, while the helo leg's aircraft was still alive
			; and mid-mission. Live 2026-07-30: it deleted a healthy in-flight helicopter and
			; the player watched it be replaced by the paratrooper spawn. A stage number is a
			; statement about the ENGINE; support_e2_leg_done is a statement about THIS HULL,
			; written only by e2_mark_leg_done, which only the paths that genuinely end a leg
			; call: both cleanups (every completion, timeout and real failure routes through one
			; of them), the combo leftover retirement, and the arrival long-stop above.
			;
			; Both tags are required. support_e2_arrival is written only by the numeric
			; waypoint's own {commands} block, so this can never reach a parked template; the
			; terminal marker is what stops it reaching a live one. It re-arms itself, so it
			; stands for the whole mission.
			{"attack_support/e2_orphan_sweep"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.entities" {selector {tag support_e2_arrival} {tag support_e2_leg_done}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					; OFF THE MAP FIRST, DELETE SECOND, and long enough to actually get there.
					; The transit allowance is the helo leg's 60s - the one the player watched a
					; helicopter complete on 2026-07-31 - not the 20s that had a C-130 vanish in
					; mid-map on the same run.
					("e2_order_aircraft_exit")
					{"delay" {time 60}}
					{"delete" {selector {ignore_captured_by_user 0} {tag support_e2_arrival} {tag support_e2_leg_done}}}
					{"entity_state" {selector {tag support_e2_leg_done}} {tag_remove support_e2_arrival}}
					{"entity_state" {selector {tag support_e2_leg_done}} {tag_remove support_e2_leg_done}}
					{"trigger" {name "attack_support/e2_orphan_sweep"}}
				}
			}

			(define "e2_trigger_helo_by_army"
				{"switch"
					{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}} {"trigger" {name "attack_support/e2_helo_rusa"}}}
					{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}} {"trigger" {name "attack_support/e2_helo_ukr"}}}
					{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 3}} {"trigger" {name "attack_support/e2_helo_nato"}}}
					; PLA Army Aviation transport (Mi-171 adaptation). PRC has no fixed-wing
					; call-in per its doctrine note, so it routes here but not in the para switch.
					{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 4}} {"trigger" {name "attack_support/e2_helo_prc"}}}
					; 12 keeps "army not published yet" apart from "this army has no such
					; package" (1, e.g. PRC has no fixed-wing call-in by doctrine).
					{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 0}} {"set_i" {var "support_e2_fail$"} {op "="} {value 12}} ("e2_fail_and_cleanup")}
					{"default" {"set_i" {var "support_e2_fail$"} {op "="} {value 1}} ("e2_fail_and_cleanup")}
				}
			)

			(define "e2_trigger_para_by_army"
				; The para leg always starts from a clean code. e2_dispatch clears it and the
				; combo transition clears it again on the way through, yet the live 2026-07-30
				; run read fail 10 for the whole para leg - so the reset is also asserted here,
				; at the last point before a faction child can run. A leg that inherits another
				; leg's code is a leg whose diagnostics are unreadable.
				{"set_i" {var "support_e2_fail$"} {op "="} {value 0}}
				{"switch"
					{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}} {"trigger" {name "attack_support/e2_para_rusa"}}}
					{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}} {"trigger" {name "attack_support/e2_para_ukr"}}}
					{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 3}} {"trigger" {name "attack_support/e2_para_nato"}}}
					; 12 keeps "army not published yet" apart from "this army has no such
					; package" (1, e.g. PRC has no fixed-wing call-in by doctrine).
					{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 0}} {"set_i" {var "support_e2_fail$"} {op "="} {value 12}} ("e2_fail_and_cleanup")}
					{"default" {"set_i" {var "support_e2_fail$"} {op "="} {value 1}} ("e2_fail_and_cleanup")}
				}
			)

			; Term 7 is why no aircraft ever flew: faction_support_army$ is published by
			; the faction resolve about half a minute into the mission, but the dispatcher
			; armed within the first few seconds and read 0. Both by_army switches then took
			; their default, failed the leg and cleaned up before a single hull was claimed.
			{"attack_support/e2_dispatch"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6 & 7"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_e2_stage$"} {op "=="} {value 0}}
						{"6.cmp_i" {var "support_e2_test$"} {op ">"} {value 0}}
						{"7.cmp_i" {var "faction_support_army$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "support_e2_fail$"} {op "="} {value 0}}
					; "The leg is over" belongs to ONE leg. Clearing it here is what stops the
					; para leg inheriting the helo leg's terminal marker, and it is the same
					; reason the fail code is cleared on the line above.
					{"set_i" {var "support_e2_leg_done$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_air_try$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_air_age$"} {op "="} {value 0}}
					; Reserve atomically before explicit child-trigger dispatch.
					{"set_i" {var "support_e2_stage$"} {op "="} {value 10}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_test$"} {op "=="} {value 1}}
							("e2_trigger_helo_by_army")
						}
						{"case"
							{condition {type cmp_i} {var "support_e2_test$"} {op "=="} {value 3}}
							("e2_trigger_helo_by_army")
						}
						{"case"
							{condition {type cmp_i} {var "support_e2_test$"} {op "=="} {value 2}}
							("e2_trigger_para_by_army")
						}
						{"default"
							{"set_i" {var "support_e2_fail$"} {op "="} {value 1}}
							("e2_fail_and_cleanup")
						}
					}
					; A supported trigger claims immediately. Remaining at zero means its
					; exact hull/crew/team pool condition failed.
					{"delay" {time 1}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 10}}
							{"set_i" {var "support_e2_fail$"} {op "="} {value 2}}
							("e2_fail_and_cleanup")
						}
						{"default"}
					}
				}
			}

			; ===== THE PARA LEG DISPATCHES AT THE 3-MINUTE MARK, AND NEVER PREEMPTS =====
			; Two new terms, both tightening. Terms 3 and 4 are unchanged pins.
			;
			;   5. THE HELO LEG IS ACTUALLY OVER. support_e2_leg_done$ is the var half of
			;      the same terminal marker the orphan sweep reads off the hull, written by
			;      the one define e2_mark_leg_done, so "the leg is over" has exactly one
			;      definition in this engine. The stage variable alone was never that
			;      statement: it reaches 70 on this very transition.
			;   6. THE 180-SECOND FLOOR. support_e2_clock_t$ counts one step per 10 seconds
			;      from attack_support/e2_clock, so >= 18 is >= 180 seconds of match time.
			;      SetVar takes integers only; this is the same counted-step form the
			;      motorised drive clock already uses.
			;
			; The clock is a FLOOR ON THE PARA DISPATCH, never a ceiling on the helo leg:
			; nothing here interrupts a live helo leg, and if that leg is still flying at
			; 180s its insert and exit run to completion while the para leg waits. Both
			; terms are standing conditions, so the transition fires on whichever lands last.
			{"attack_support/e2_combo_transition"
				{condition
					{expression "1 & 2 & !3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "support_e2_test$"} {op "=="} {value 3}}
						{"2.cmp_i" {var "support_e2_stage$"} {op "=="} {value 70}}
						{"3.entities" {selector {tag support_e2_claim}}}
						{"4.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"5.cmp_i" {var "support_e2_leg_done$"} {op "=="} {value 1}}
						{"6.cmp_i" {var "support_e2_clock_t$"} {op ">="} {value 18}}
					}
				}
				{actions
					; SetVar takes integers only. The var-to-var copy that used to stand here
					; never landed, so the helo leg reported combo_helo_fail 0 whatever it did -
					; a fabricated success. Fold the code through literal cases instead.
					{"switch"
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 0}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 1}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 1}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 2}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 2}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 3}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 3}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 4}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 4}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 5}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 5}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 6}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 6}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 7}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 7}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 8}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 8}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 9}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 9}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 10}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 10}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 11}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 11}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 12}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 12}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 13}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 13}}}
						{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 14}} {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 14}}}
						{"default" {"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 99}}}
					}
					{"set_i" {var "support_e2_fail$"} {op "="} {value 0}}
					; The helo leg's terminal marker has been consumed by the condition above;
					; leg two starts without it. e2_dispatch asserts the same reset.
					{"set_i" {var "support_e2_leg_done$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_test$"} {op "="} {value 2}}
					{"set_i" {var "support_e2_stage$"} {op "="} {value 0}}
					; The dispatcher fires once; resetting stage alone will not re-arm it.
					{"trigger" {name "attack_support/e2_dispatch"}}
				}
			}

			; The transition above requires that NO claimed entity remains (term 3), which is
			; correct and stays exactly as it is. But a helo leg that orphaned a claimed
			; aircraft satisfied that term forever, so the transition could not fire and the
			; para leg either never started or started carrying the helo leg's fail code.
			; This retires the leftover instead of relaxing the pin: order it off the map,
			; delete it, strip the leg's own bookkeeping tags, then re-arm the transition.
			; Its condition is the exact complement of the transition's, so the two can never
			; both be true.
			{"attack_support/e2_combo_clear"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 3}}
						{"3.cmp_i" {var "support_e2_stage$"} {op "=="} {value 70}}
						{"4.entities" {selector {tag support_e2_claim}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					; Retiring a leftover IS ending that leg, so it goes through the one
					; definition of "over" like every other terminal path.
					("e2_mark_leg_done")
					("e2_order_aircraft_exit")
					{"delay" {time 10}}
					("e2_delete_aircraft")
					{"entity_state" {selector {tag support_e2_claim}} {tag_remove support_e2_claim}}
					{"entity_state" {selector {tag support_e2_arrival}} {tag_remove support_e2_arrival}}
					{"entity_state" {selector {tag support_e2_helo}} {tag_remove support_e2_helo}}
					{"entity_state" {selector {tag support_e2_plane}} {tag_remove support_e2_plane}}
					{"entity_state" {selector {tag support_e2_lz_done}} {tag_remove support_e2_lz_done}}
					{"trigger" {name "attack_support/e2_combo_transition"}}
				}
			}

			; ===== E2 PARADROP =====
			; Gate 2 is a real aircraft/ejection proof. The dispatcher exclusively reserves
			; stage 10; faction children claim once and release monitors own stage 30.

			(define "e2_announce_para"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector {ignore_captured_by_user 0} {amount 1} {tag player} {zone "gamezone"} {type human} {state "not dead"}}
							{text "mission/multi/support/e2_para_inbound"}
						}
					}
					{"default"}
				}
			)

; The plane is deleted at 90 seconds even while the landing window remains open.
			; A survivor may arrive for 30 more seconds; otherwise fail 7 is preserved.
			(define "e2_para_settle"
				{"delay" {time 90}}
				("e2_delete_aircraft")
				{"switch"
					{"case"
						{condition {type near} {units {ignore_captured_by_user 0} {tag paratrooper_need_orders} {type human} {state "not dead"}} {near_to {ignore_captured_by_user 0} {tag support_e2_flag_target}} {distance 6000}}
						{"set_i" {var "support_e2_stage$"} {op "="} {value 60}}
						("e2_complete_cleanup")
					}
					{"default"
						{"delay" {time 29}}
						{"switch"
							{"case"
								{condition {type near} {units {ignore_captured_by_user 0} {tag paratrooper_need_orders} {type human} {state "not dead"}} {near_to {ignore_captured_by_user 0} {tag support_e2_flag_target}} {distance 6000}}
								{"set_i" {var "support_e2_stage$"} {op "="} {value 60}}
								("e2_complete_cleanup")
							}
							{"default"
								{"set_i" {var "support_e2_fail$"} {op "="} {value 7}}
								{"set_i" {var "support_e2_stage$"} {op "="} {value 60}}
								("e2_fail_and_cleanup")
							}
						}
					}
				}
			)

			; ===== THE PARA EXIT EVIDENCE GATE =====
			; Live 2026-07-31: the para leg walked stage 20 -> 60 -> 70 with
			; e2_para_band 0, e2_para_pass 0 and fail 0. Nobody jumped, nothing was
			; recorded, and the ledger read like a clean run. That is the fabricated-success
			; pattern this project has already banned twice, and it was reachable because
			; the "the gate refused the run-in" branch wrote stage 60 without writing any
			; code at all.
			;
			; So the para leg may close only with EVIDENCE, and this define is called
			; immediately before every stage-60 write on the exit path:
			;   - a RECORDED RELEASE. support_e2_released is written by the release monitor
			;     before the cargo-bay effect and before the emit, so it exists on every leg
			;     that actually dropped, and e2_para_settle owns the outcome from there
			;     (a live jumper near the flag, or fail 7).
			;   - or a RECORDED FAIL. If neither holds, 6 is written here: "never approached
			;     the release band" is exactly the true statement about a leg that reached
			;     its exit with no release to show for it.
			; An earlier code still wins, so 9/10/14 are never buried under 6.
			(define "e2_para_require_release_or_fail"
				{"switch"
					{"case"
						{condition {type entities} {selector {tag support_e2_released}} {count {op ">="} {value 1}}}
					}
					{"default"
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
								{"set_i" {var "support_e2_fail$"} {op "="} {value 6}}
							}
							{"default"}
						}
					}
				}
			)

			; attack_support/e2_para_landed RETIRED (2026-07-30, clone restructure).
			; It selected landed jumpers by support_e2_para_pax + support_e2_claim. Under
			; the clone dispatch the jumpers are copies made by {clone} and carry NONE of
			; this engine's tags, so that selector can never match, and there is no
			; selector form that can tell one aircraft's untagged jumpers from anyone
			; else's. Ordering them is therefore left to CE's own paratrooper lifecycle,
			; which already owns every entity it tags paratrooper_need_orders; this engine
			; only PROVES they landed (see e2_para_settle above) and never orders them, so
			; the ordering conflict the plan warns about cannot arise.
			{"attack_support/e2_para_rusa"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6 & 7 & 8 & 9 & 10"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"6.cmp_i" {var "support_e2_stage$"} {op "=="} {value 10}}
						{"7.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"8.entities" {selector {tag support_e2_rusa_para}} {count {op ">="} {value 1}}}
						{"9.entities" {selector {tag support_e2_rusa_para_crew}} {count {op ">="} {value 5}}}
						{"10.entities" {selector {tag support_e2_rusa_para_pax}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					("e2_prove_park_rusa_para")
					; Nothing may be claimed until the parked airframe is proved present.
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
							; Clone dispatch: the parked plane, its linked crew and its linked jumpers
							; all stay parked and keep their pool tags, so the para leg is repeatable and
							; costs the pool nothing. The clone carries copies of both, and the copies in
							; the seats are what the release ejects.
							{"entity_state" {selector {source advanced} {group {select {tag {tag support_e2_rusa_para}}}} {amount 1}} {tag_add support_e2_src}}
							("e2_choose_flag")
							; Stage 20 claims "a target has been accepted". The evidence for that claim is
							; the target entity itself, not the absence of a failure so far.
							{"switch"
								{"case"
									{condition {type entities} {selector {tag support_e2_flag_target}} {count {op ">="} {value 1}}}
									{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}
									("e2_own_current")
									{"switch"
										{"case"
											{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
											; Clone + teleport; the numeric waypoint's {commands} block re-tags the arrival
											; and starts it toward the chosen flag. No air_state verb: altitude belongs to
											; the parked chassis snapshot.
											("e2_clone_aircraft")
											("e2_prove_arrival")
											("e2_arrival_is_plane")
											("e2_promote_arrival")
											("e2_own_arrival")
											{"actor_state" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {drop sensor} {control AI} {movement {speed fast}}}
											; RE-ASSERT AFTER THE OWNERSHIP TRANSFER. Same window, same loss, same fix as the
											; helo leg: the promote step ran before ("e2_own_arrival") and before the
											; {"actor_state"} step, and its tags did not survive it. support_e2_arrival did.
											("e2_reassert_arrival_plane")
											{"delay" {time 0.5}}
											; Evidence gate: a live claimed transport, or fail 10 and no run-in.
											("e2_fly_para_or_fail")
											("e2_announce_para")
											{"delay" {time 90}}
											{"switch"
												{"case" {condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 30}} ("e2_para_require_release_or_fail") ("e2_order_aircraft_exit") {"set_i" {var "support_e2_stage$"} {op "="} {value 60}} {"delay" {time 60}} ("e2_fail_and_cleanup")}
												; Stage still 20 means e2_fly_para_or_fail refused the run-in - which now
												; requires that there was no live arrival either, because the branch that
												; sees one opens the leg. This case used to write stage 60 and NOTHING
												; else, which is how the 2026-07-31 run reported a clean para leg that
												; never dropped anyone. It is evidence-gated now like every other exit.
												{"case" {condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 20}} ("e2_para_require_release_or_fail") ("e2_order_aircraft_exit") {"set_i" {var "support_e2_stage$"} {op "="} {value 60}} {"delay" {time 60}} ("e2_fail_and_cleanup")}
												{"default"}
											}
										}
										{"default" ("e2_fail_and_cleanup")}
									}
								}
								{"default" ("e2_fail_and_cleanup")}
							}
						}
						{"default"}
					}
				}
			}

			{"attack_support/e2_para_ukr"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6 & 7 & 8 & 9 & 10"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"6.cmp_i" {var "support_e2_stage$"} {op "=="} {value 10}}
						{"7.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"8.entities" {selector {tag support_e2_ukr_para}} {count {op ">="} {value 1}}}
						{"9.entities" {selector {tag support_e2_ukr_para_crew}} {count {op ">="} {value 3}}}
						{"10.entities" {selector {tag support_e2_ukr_para_pax}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					("e2_prove_park_ukr_para")
					; Nothing may be claimed until the parked airframe is proved present.
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
							; Clone dispatch: the parked plane, its linked crew and its linked jumpers
							; all stay parked and keep their pool tags, so the para leg is repeatable and
							; costs the pool nothing. The clone carries copies of both, and the copies in
							; the seats are what the release ejects.
							{"entity_state" {selector {source advanced} {group {select {tag {tag support_e2_ukr_para}}}} {amount 1}} {tag_add support_e2_src}}
							("e2_choose_flag")
							; Stage 20 claims "a target has been accepted". The evidence for that claim is
							; the target entity itself, not the absence of a failure so far.
							{"switch"
								{"case"
									{condition {type entities} {selector {tag support_e2_flag_target}} {count {op ">="} {value 1}}}
									{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}
									("e2_own_current")
									{"switch"
										{"case"
											{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
											; Clone + teleport; the numeric waypoint's {commands} block re-tags the arrival
											; and starts it toward the chosen flag. No air_state verb: altitude belongs to
											; the parked chassis snapshot.
											("e2_clone_aircraft")
											("e2_prove_arrival")
											("e2_arrival_is_plane")
											("e2_promote_arrival")
											("e2_own_arrival")
											{"actor_state" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {drop sensor} {control AI} {movement {speed fast}}}
											; RE-ASSERT AFTER THE OWNERSHIP TRANSFER. Same window, same loss, same fix as the
											; helo leg: the promote step ran before ("e2_own_arrival") and before the
											; {"actor_state"} step, and its tags did not survive it. support_e2_arrival did.
											("e2_reassert_arrival_plane")
											{"delay" {time 0.5}}
											; Evidence gate: a live claimed transport, or fail 10 and no run-in.
											("e2_fly_para_or_fail")
											("e2_announce_para")
											{"delay" {time 90}}
											{"switch"
												{"case" {condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 30}} ("e2_para_require_release_or_fail") ("e2_order_aircraft_exit") {"set_i" {var "support_e2_stage$"} {op "="} {value 60}} {"delay" {time 60}} ("e2_fail_and_cleanup")}
												; Stage still 20 means e2_fly_para_or_fail refused the run-in - which now
												; requires that there was no live arrival either, because the branch that
												; sees one opens the leg. This case used to write stage 60 and NOTHING
												; else, which is how the 2026-07-31 run reported a clean para leg that
												; never dropped anyone. It is evidence-gated now like every other exit.
												{"case" {condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 20}} ("e2_para_require_release_or_fail") ("e2_order_aircraft_exit") {"set_i" {var "support_e2_stage$"} {op "="} {value 60}} {"delay" {time 60}} ("e2_fail_and_cleanup")}
												{"default"}
											}
										}
										{"default" ("e2_fail_and_cleanup")}
									}
								}
								{"default" ("e2_fail_and_cleanup")}
							}
						}
						{"default"}
					}
				}
			}

			{"attack_support/e2_para_nato"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6 & 7 & 8 & 9 & 10"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"6.cmp_i" {var "support_e2_stage$"} {op "=="} {value 10}}
						{"7.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"8.entities" {selector {tag support_e2_nato_para}} {count {op ">="} {value 1}}}
						{"9.entities" {selector {tag support_e2_nato_para_crew}} {count {op ">="} {value 3}}}
						{"10.entities" {selector {tag support_e2_nato_para_pax}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					("e2_prove_park_nato_para")
					; Nothing may be claimed until the parked airframe is proved present.
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
							; Clone dispatch: the parked plane, its linked crew and its linked jumpers
							; all stay parked and keep their pool tags, so the para leg is repeatable and
							; costs the pool nothing. The clone carries copies of both, and the copies in
							; the seats are what the release ejects.
							{"entity_state" {selector {source advanced} {group {select {tag {tag support_e2_nato_para}}}} {amount 1}} {tag_add support_e2_src}}
							("e2_choose_flag")
							; Stage 20 claims "a target has been accepted". The evidence for that claim is
							; the target entity itself, not the absence of a failure so far.
							{"switch"
								{"case"
									{condition {type entities} {selector {tag support_e2_flag_target}} {count {op ">="} {value 1}}}
									{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}
									("e2_own_current")
									{"switch"
										{"case"
											{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
											; Clone + teleport; the numeric waypoint's {commands} block re-tags the arrival
											; and starts it toward the chosen flag. No air_state verb: altitude belongs to
											; the parked chassis snapshot.
											("e2_clone_aircraft")
											("e2_prove_arrival")
											("e2_arrival_is_plane")
											("e2_promote_arrival")
											("e2_own_arrival")
											{"actor_state" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}} {drop sensor} {control AI} {movement {speed fast}}}
											; RE-ASSERT AFTER THE OWNERSHIP TRANSFER. Same window, same loss, same fix as the
											; helo leg: the promote step ran before ("e2_own_arrival") and before the
											; {"actor_state"} step, and its tags did not survive it. support_e2_arrival did.
											("e2_reassert_arrival_plane")
											{"delay" {time 0.5}}
											; Evidence gate: a live claimed transport, or fail 10 and no run-in.
											("e2_fly_para_or_fail")
											("e2_announce_para")
											{"delay" {time 90}}
											{"switch"
												{"case" {condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 30}} ("e2_para_require_release_or_fail") ("e2_order_aircraft_exit") {"set_i" {var "support_e2_stage$"} {op "="} {value 60}} {"delay" {time 60}} ("e2_fail_and_cleanup")}
												; Stage still 20 means e2_fly_para_or_fail refused the run-in - which now
												; requires that there was no live arrival either, because the branch that
												; sees one opens the leg. This case used to write stage 60 and NOTHING
												; else, which is how the 2026-07-31 run reported a clean para leg that
												; never dropped anyone. It is evidence-gated now like every other exit.
												{"case" {condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 20}} ("e2_para_require_release_or_fail") ("e2_order_aircraft_exit") {"set_i" {var "support_e2_stage$"} {op "="} {value 60}} {"delay" {time 60}} ("e2_fail_and_cleanup")}
												{"default"}
											}
										}
										{"default" ("e2_fail_and_cleanup")}
									}
								}
								{"default" ("e2_fail_and_cleanup")}
							}
						}
						{"default"}
					}
				}
			}

			; ===== WHAT THE BAND ACTUALLY MEASURES (corrected 2026-07-31) =====
			; It reported 0 for an entire overflight the player watched. Two faults, both
			; in the selector rather than in the rings:
			;   1. It asked for support_e2_plane AND support_e2_claim on one entity. That is
			;      the same co-residence the 2026-07-30 helo run proved does not survive the
			;      ownership transfer, and the 2026-07-31 para run proved it again from the
			;      other side: e2_fly_para_or_fail's claim-keyed case matched nothing on a
			;      C-130 that flew the whole map.
			;   2. It carried a {state "not dead"} decoration - a state decoration on a
			;      support_e2_ selector, the class that has already zeroed two E2 matches on
			;      units that were demonstrably alive.
			; It now uses the ONE near-check shape with a live proof in this file: bare
			; support_e2_arrival units against a bare tag reference, which is exactly what
			; attack_support/e2_helo_lz term 6 fired on when the helicopter reached its LZ.
			;
			; REFERENCE. support_e2_flag_target is written by e2_choose_flag and is a
			; precondition of stage 20, so it provably exists for the whole of stage 30 - and
			; the run-in order itself resolves against it, which is why the plane flew at all.
			; Only e2_reset_target and the two cleanups take it off.
			;
			; UNITS. Map coordinates are DECIMETRES (dcg_[cwa71]_fields spans ~11400 x 6200
			; units with its two capture points 1530 apart), so the 1000/2000/3000/4000 rings
			; are 100 m / 200 m / 300 m / 400 m and the outer ring is a fifth of the short
			; axis of the map - reachable by any run-in that crosses the objective at all.
			;
			; Coarse closest-approach tracker for the run-in. SetVar takes integers only, so
			; the range trend is carried as a band number: 1 = inside 1000, 2 = inside 2000,
			; 3 = inside 3000, 4 = inside the 4000 outer ring. support_e2_para_range_band$
			; keeps the TIGHTEST band reached so far (0 = the plane has never been inside the
			; outer ring). A poll that reports a LOOSER band than the recorded best, while the
			; plane is still inside the outer ring, means the run-in has passed its closest
			; point - that is the release cue, and it grants support_e2_para_pass$ = 1. Band 1
			; grants it outright: 1000 units is as close as this tracker can resolve, so a
			; plane that arrives overhead and parks there still drops instead of hanging.
			; Both vars are bounded: e2_reset_target clears them per leg and the
			; support_e2_released tag stops any monitor firing twice.
			(define "e2_para_range_poll"
				{"switch"
					{"case"
						{condition
							{type near}
							{units {ignore_captured_by_user 0} {tag support_e2_arrival}}
							{near_to {ignore_captured_by_user 0} {tag support_e2_flag_target}}
							{distance 1000}
						}
						{"set_i" {var "support_e2_para_range_band$"} {op "="} {value 1}}
						{"set_i" {var "support_e2_para_pass$"} {op "="} {value 1}}
					}
					{"case"
						{condition
							{type near}
							{units {ignore_captured_by_user 0} {tag support_e2_arrival}}
							{near_to {ignore_captured_by_user 0} {tag support_e2_flag_target}}
							{distance 2000}
						}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_para_range_band$"} {op "=="} {value 1}}
								{"set_i" {var "support_e2_para_pass$"} {op "="} {value 1}}
							}
							{"default" {"set_i" {var "support_e2_para_range_band$"} {op "="} {value 2}}}
						}
					}
					{"case"
						{condition
							{type near}
							{units {ignore_captured_by_user 0} {tag support_e2_arrival}}
							{near_to {ignore_captured_by_user 0} {tag support_e2_flag_target}}
							{distance 3000}
						}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_para_range_band$"} {op "=="} {value 1}}
								{"set_i" {var "support_e2_para_pass$"} {op "="} {value 1}}
							}
							{"case"
								{condition {type cmp_i} {var "support_e2_para_range_band$"} {op "=="} {value 2}}
								{"set_i" {var "support_e2_para_pass$"} {op "="} {value 1}}
							}
							{"default" {"set_i" {var "support_e2_para_range_band$"} {op "="} {value 3}}}
						}
					}
					{"case"
						{condition
							{type near}
							{units {ignore_captured_by_user 0} {tag support_e2_arrival}}
							{near_to {ignore_captured_by_user 0} {tag support_e2_flag_target}}
							{distance 4000}
						}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_para_range_band$"} {op "=="} {value 1}}
								{"set_i" {var "support_e2_para_pass$"} {op "="} {value 1}}
							}
							{"case"
								{condition {type cmp_i} {var "support_e2_para_range_band$"} {op "=="} {value 2}}
								{"set_i" {var "support_e2_para_pass$"} {op "="} {value 1}}
							}
							{"case"
								{condition {type cmp_i} {var "support_e2_para_range_band$"} {op "=="} {value 3}}
								{"set_i" {var "support_e2_para_pass$"} {op "="} {value 1}}
							}
							{"default" {"set_i" {var "support_e2_para_range_band$"} {op "="} {value 4}}}
						}
					}
					; Outside the outer ring: no trend, and fail 6 stays on the table.
					{"default"}
				}
			)

			; Each monitor is independently gated and faction-specific. The inner release
			; tag is added before the effect, so a second monitor can never repeat it.
			;
			; RELEASE BAND (widened 2026-07-30 after a live run sat at stage 30 for ~92s and
			; then reported fail 6). The old band was 1500-2500: map coordinates are
			; DECIMETRES, so that is a 1000-unit-wide shell only 150-250m out, crossed by the
			; airborne chassis at ~264 units/s - under 4 seconds of dwell, and never entered at
			; all if the run-in's closest approach was 2600. The band is now 600-4000, a
			; 3400-unit annulus (~13s of dwell) that an off-axis run-in cannot skirt, and term
			; 8 is the bounded closest-approach fallback described above, so a plane that steps
			; into the 600 hole or parks over the flag still drops. Fail 6 stays meaningful: a
			; plane that never gets inside 4000 satisfies neither term 5 nor term 8, never
			; releases, and is still reported as "never approached".
			{"attack_support/e2_para_release_rusa"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 7"}
					{terms
						{"7.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"1.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"2.cmp_i" {var "support_e2_stage$"} {op "=="} {value 30}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.entities" {selector {source advanced} {group {select {tag {tag support_e2_arrival}}} {exclude {tag {tag support_e2_released}}}}}}
						{"5.near" {units {ignore_captured_by_user 0} {tag support_e2_arrival}} {near_to {ignore_captured_by_user 0} {tag support_e2_flag_target}} {distance 1500}}
					}
				}
				{actions
					; THE ONE-SHOT GUARD, FIRST. Vanilla makes its release one-shot with a
					; second tag written AFTER the effect (ai_para_assigned2); this engine has
					; to write its guard BEFORE, because term 4 of this trigger is the exclude
					; that stops it re-firing and a trigger tick can fall inside a 0.1s gap.
					; That is the ONLY deviation from the shipped invocation below.
					{"entity_state" {selector {tag support_e2_arrival}} {tag_add support_e2_released}}
					; ===== THE SHIPPED PARADROP CALL-IN, MATCHED VERBATIM (2026-07-31) =====
					; Code:X own conquest paradrop is the trigger
					; "[ordos] paratrooper detector script for dcg" in
					; resource/map/multi/forest_/campaign_capture_the_flag.mi:31963-32127 and
					; .../bakhmut_1/campaign_capture_the_flag.mi:17293-17384. Its ENTIRE release
					; is ONE verb - an effect with an advanced {group {select ...}} selector and
					; {effect drop_paratrooper} - followed by {"delay" {time 0.1}} and a tag swap
					; that makes it one-shot. There is no second verb of any kind in it. We now
					; fire exactly that and nothing else.
					;
					; WHY ONE VERB IS ENOUGH. The chain the effect starts, read end to end:
					;   1. {on "drop_paratrooper"} on both airframes (Code:X
					;      set/interaction_entity/airborne_modern.inc:1629 "il-76td_para",
					;      :1996 "c130_para", :1692/:1809 the _ai variants) is
					;      {damage_report "body" "component/cargobay_opened"} + {call "open_cargo"}.
					;   2. open_cargo (Airborne_M.inc:3645) plays {ani_play "cargo_open" callback}.
					;      The `callback` keyword is the load-bearing part: it is what makes
					;      animation_end fire at all.
					;   3. {on animation_end "cargo_open"} (Airborne_M.inc:3765-3793) branches on
					;      the cargo-platform tags. Both our airframes remove "place1_busy" on
					;      spawn, so both take the final else - {call "drop_desant1"}, then
					;      drop_desant2 and drop_desant3 one and two seconds later.
					;   4. drop_desantN is the seat unlink: {with_linked_entity "seatNN" {unlink}}
					;      over seat00-seat15/16-31/32-47 for the il-76td group
					;      (airborne_modern.inc:139-198) and seat02-seat11/seat12-seat27 for the
					;      c130 group (:1928-1980). Our pax sit in seat01-seat04 (il-76td_para)
					;      and seat02-seat05 (c130_para) - inside drop_desant1 on both.
					;   5. THE CANOPY IS THE JUMPER OWN DOING. A body leaving an "il-76td"/"c130"
					;      hull above altitude 8 runs human.inc {on board out}
					;      ("paradrop_crew_member_with_alt_r", Code:X human.inc:1494-1547): it tags
					;      itself heli_paradrop<N> and SPAWNS a "parachute_new" tagged
					;      heli_parashute<N>, N a random slot 0-7. Our parked planes sit at
					;      {Altitude 65}, so the altitude-40 branch is taken. The two entities are
					;      NOT connected until a mission trigger links them - see the parachute
					;      linkers further down this file, which is the other half of this fix.
					;
					; WHY THE OLD EMIT PAIRING WAS THE DEFECT. It force-unloaded every seat one
					; second after the effect - before step 2 finished animating, so steps 3 and 4
					; never ran at all - and it drove the release through a code path the shipped
					; call-in never uses. The visible result was a plane that opened its bay and
					; put bodies on the ground with no canopy between.
					{"effect" {selector {source advanced} {group {select {tag {tag support_e2_arrival}}}}} {effect drop_paratrooper}}
					{"delay" {time 0.1}}
					; LET THE CASCADE RUN BEFORE THE PLANE IS SENT AWAY. eject_troopers staggers
					; its unlinks from 1.0s to 11.0s after drop_paratroopers, so the departure
					; order may not be issued on the same tick as the release or the jumpers leave
					; a plane that is already turning for the exit node. 15s covers the whole
					; cascade and still sits far inside e2_para_settle's 90s and the 240s long-stop.
					{"delay" {time 15}}
					{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}
					("e2_order_aircraft_exit")
					("e2_para_settle")
				}
			}

			{"attack_support/e2_para_release_ukr"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 7"}
					{terms
						{"7.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"1.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"2.cmp_i" {var "support_e2_stage$"} {op "=="} {value 30}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.entities" {selector {source advanced} {group {select {tag {tag support_e2_arrival}}} {exclude {tag {tag support_e2_released}}}}}}
						{"5.near" {units {ignore_captured_by_user 0} {tag support_e2_arrival}} {near_to {ignore_captured_by_user 0} {tag support_e2_flag_target}} {distance 1500}}
					}
				}
				{actions
					; THE ONE-SHOT GUARD, FIRST. Vanilla makes its release one-shot with a
					; second tag written AFTER the effect (ai_para_assigned2); this engine has
					; to write its guard BEFORE, because term 4 of this trigger is the exclude
					; that stops it re-firing and a trigger tick can fall inside a 0.1s gap.
					; That is the ONLY deviation from the shipped invocation below.
					{"entity_state" {selector {tag support_e2_arrival}} {tag_add support_e2_released}}
					; ===== THE SHIPPED PARADROP CALL-IN, MATCHED VERBATIM (2026-07-31) =====
					; Code:X own conquest paradrop is the trigger
					; "[ordos] paratrooper detector script for dcg" in
					; resource/map/multi/forest_/campaign_capture_the_flag.mi:31963-32127 and
					; .../bakhmut_1/campaign_capture_the_flag.mi:17293-17384. Its ENTIRE release
					; is ONE verb - an effect with an advanced {group {select ...}} selector and
					; {effect drop_paratrooper} - followed by {"delay" {time 0.1}} and a tag swap
					; that makes it one-shot. There is no second verb of any kind in it. We now
					; fire exactly that and nothing else.
					;
					; WHY ONE VERB IS ENOUGH. The chain the effect starts, read end to end:
					;   1. {on "drop_paratrooper"} on both airframes (Code:X
					;      set/interaction_entity/airborne_modern.inc:1629 "il-76td_para",
					;      :1996 "c130_para", :1692/:1809 the _ai variants) is
					;      {damage_report "body" "component/cargobay_opened"} + {call "open_cargo"}.
					;   2. open_cargo (Airborne_M.inc:3645) plays {ani_play "cargo_open" callback}.
					;      The `callback` keyword is the load-bearing part: it is what makes
					;      animation_end fire at all.
					;   3. {on animation_end "cargo_open"} (Airborne_M.inc:3765-3793) branches on
					;      the cargo-platform tags. Both our airframes remove "place1_busy" on
					;      spawn, so both take the final else - {call "drop_desant1"}, then
					;      drop_desant2 and drop_desant3 one and two seconds later.
					;   4. drop_desantN is the seat unlink: {with_linked_entity "seatNN" {unlink}}
					;      over seat00-seat15/16-31/32-47 for the il-76td group
					;      (airborne_modern.inc:139-198) and seat02-seat11/seat12-seat27 for the
					;      c130 group (:1928-1980). Our pax sit in seat01-seat04 (il-76td_para)
					;      and seat02-seat05 (c130_para) - inside drop_desant1 on both.
					;   5. THE CANOPY IS THE JUMPER OWN DOING. A body leaving an "il-76td"/"c130"
					;      hull above altitude 8 runs human.inc {on board out}
					;      ("paradrop_crew_member_with_alt_r", Code:X human.inc:1494-1547): it tags
					;      itself heli_paradrop<N> and SPAWNS a "parachute_new" tagged
					;      heli_parashute<N>, N a random slot 0-7. Our parked planes sit at
					;      {Altitude 65}, so the altitude-40 branch is taken. The two entities are
					;      NOT connected until a mission trigger links them - see the parachute
					;      linkers further down this file, which is the other half of this fix.
					;
					; WHY THE OLD EMIT PAIRING WAS THE DEFECT. It force-unloaded every seat one
					; second after the effect - before step 2 finished animating, so steps 3 and 4
					; never ran at all - and it drove the release through a code path the shipped
					; call-in never uses. The visible result was a plane that opened its bay and
					; put bodies on the ground with no canopy between.
					{"effect" {selector {source advanced} {group {select {tag {tag support_e2_arrival}}}}} {effect drop_paratrooper}}
					{"delay" {time 0.1}}
					; LET THE CASCADE RUN BEFORE THE PLANE IS SENT AWAY. eject_troopers staggers
					; its unlinks from 1.0s to 11.0s after drop_paratroopers, so the departure
					; order may not be issued on the same tick as the release or the jumpers leave
					; a plane that is already turning for the exit node. 15s covers the whole
					; cascade and still sits far inside e2_para_settle's 90s and the 240s long-stop.
					{"delay" {time 15}}
					{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}
					("e2_order_aircraft_exit")
					("e2_para_settle")
				}
			}

			{"attack_support/e2_para_release_nato"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 7"}
					{terms
						{"7.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"1.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"2.cmp_i" {var "support_e2_stage$"} {op "=="} {value 30}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.entities" {selector {source advanced} {group {select {tag {tag support_e2_arrival}}} {exclude {tag {tag support_e2_released}}}}}}
						{"5.near" {units {ignore_captured_by_user 0} {tag support_e2_arrival}} {near_to {ignore_captured_by_user 0} {tag support_e2_flag_target}} {distance 1500}}
					}
				}
				{actions
					; THE ONE-SHOT GUARD, FIRST. Vanilla makes its release one-shot with a
					; second tag written AFTER the effect (ai_para_assigned2); this engine has
					; to write its guard BEFORE, because term 4 of this trigger is the exclude
					; that stops it re-firing and a trigger tick can fall inside a 0.1s gap.
					; That is the ONLY deviation from the shipped invocation below.
					{"entity_state" {selector {tag support_e2_arrival}} {tag_add support_e2_released}}
					; ===== THE SHIPPED PARADROP CALL-IN, MATCHED VERBATIM (2026-07-31) =====
					; Code:X own conquest paradrop is the trigger
					; "[ordos] paratrooper detector script for dcg" in
					; resource/map/multi/forest_/campaign_capture_the_flag.mi:31963-32127 and
					; .../bakhmut_1/campaign_capture_the_flag.mi:17293-17384. Its ENTIRE release
					; is ONE verb - an effect with an advanced {group {select ...}} selector and
					; {effect drop_paratrooper} - followed by {"delay" {time 0.1}} and a tag swap
					; that makes it one-shot. There is no second verb of any kind in it. We now
					; fire exactly that and nothing else.
					;
					; WHY ONE VERB IS ENOUGH. The chain the effect starts, read end to end:
					;   1. {on "drop_paratrooper"} on both airframes (Code:X
					;      set/interaction_entity/airborne_modern.inc:1629 "il-76td_para",
					;      :1996 "c130_para", :1692/:1809 the _ai variants) is
					;      {damage_report "body" "component/cargobay_opened"} + {call "open_cargo"}.
					;   2. open_cargo (Airborne_M.inc:3645) plays {ani_play "cargo_open" callback}.
					;      The `callback` keyword is the load-bearing part: it is what makes
					;      animation_end fire at all.
					;   3. {on animation_end "cargo_open"} (Airborne_M.inc:3765-3793) branches on
					;      the cargo-platform tags. Both our airframes remove "place1_busy" on
					;      spawn, so both take the final else - {call "drop_desant1"}, then
					;      drop_desant2 and drop_desant3 one and two seconds later.
					;   4. drop_desantN is the seat unlink: {with_linked_entity "seatNN" {unlink}}
					;      over seat00-seat15/16-31/32-47 for the il-76td group
					;      (airborne_modern.inc:139-198) and seat02-seat11/seat12-seat27 for the
					;      c130 group (:1928-1980). Our pax sit in seat01-seat04 (il-76td_para)
					;      and seat02-seat05 (c130_para) - inside drop_desant1 on both.
					;   5. THE CANOPY IS THE JUMPER OWN DOING. A body leaving an "il-76td"/"c130"
					;      hull above altitude 8 runs human.inc {on board out}
					;      ("paradrop_crew_member_with_alt_r", Code:X human.inc:1494-1547): it tags
					;      itself heli_paradrop<N> and SPAWNS a "parachute_new" tagged
					;      heli_parashute<N>, N a random slot 0-7. Our parked planes sit at
					;      {Altitude 65}, so the altitude-40 branch is taken. The two entities are
					;      NOT connected until a mission trigger links them - see the parachute
					;      linkers further down this file, which is the other half of this fix.
					;
					; WHY THE OLD EMIT PAIRING WAS THE DEFECT. It force-unloaded every seat one
					; second after the effect - before step 2 finished animating, so steps 3 and 4
					; never ran at all - and it drove the release through a code path the shipped
					; call-in never uses. The visible result was a plane that opened its bay and
					; put bodies on the ground with no canopy between.
					{"effect" {selector {source advanced} {group {select {tag {tag support_e2_arrival}}}}} {effect drop_paratrooper}}
					{"delay" {time 0.1}}
					; LET THE CASCADE RUN BEFORE THE PLANE IS SENT AWAY. eject_troopers staggers
					; its unlinks from 1.0s to 11.0s after drop_paratroopers, so the departure
					; order may not be issued on the same tick as the release or the jumpers leave
					; a plane that is already turning for the exit node. 15s covers the whole
					; cascade and still sits far inside e2_para_settle's 90s and the 240s long-stop.
					{"delay" {time 15}}
					{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}
					("e2_order_aircraft_exit")
					("e2_para_settle")
				}
			}


			; ===== E2 LANDED PARATROOPER TAKEOVER =====
			; The aircraft interaction tags each jumper paratrooper_need_orders. Collect
			; bodies only after they have unlinked from their canopy, transfer them to the
			; support player and order them toward the selected flag. Autoreset catches each
			; member of the stick as it finishes landing.
			{"attack_support/e2_para_takeover"
				{autoreset}
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "support_e2_stage$"} {op "=="} {value 40}}
						{"4.entities"
							{selector
								{source advanced}
								{group
									{select {tag {tag paratrooper_need_orders}}}
									{exclude
										{tag {tag support_e2_pax}}
										{state {state linked}}
										{state {state inactive}}
										{state {state dead}}
									}
								}
							}
							{count {op ">="} {value 1}}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag paratrooper_need_orders}}}
								{exclude
									{tag {tag support_e2_pax}}
									{state {state linked}}
									{state {state inactive}}
									{state {state dead}}
								}
							}
						}
						{tag_add support_e2_pax}
						{tag_add support_e2_team}
						{tag_add attack_support_src}
						{inactive off}
						{impregnability disabled}
						{discovered on}
					}
					("e2_own_pax")
					{"delay" {time 0.2}}
					("e2_order_team")
				}
			}


			; ===== THE PARACHUTE LINKERS (2026-07-31) =====
			; THE PIECE THAT WAS MISSING, AND THE WHOLE REASON THE DROP PRODUCED NO CANOPIES.
			;
			; Nothing in MI ever attaches a jumper to a parachute. When a body with the
			; able_paradrop link leaves an "il-76td"/"c130" hull above altitude 8, the human's
			; OWN {on board out} handler (Code:X set/interaction_entity/human.inc:1494-1547)
			; runs ("paradrop_crew_member_with_alt_r"): it tags the falling man
			; heli_paradrop<N> and SPAWNS a separate "parachute_new" entity tagged
			; heli_parashute<N>, where N is a random slot 0-7. The two entities exist side by
			; side and are NOT connected. The canopy only appears once a MISSION TRIGGER
			; {"linker"}s them together - and no map this deploy manages had one: a census of
			; all fourteen dcg_[cwa71]_* maps returns zero "service/paradrop/" triggers.
			;
			; The shipped originals are "service/paradrop/0".."/7" plus the "..._" overflow
			; purge, in Code:X's own conquest maps:
			;   resource/map/multi/forest_/campaign_capture_the_flag.mi:18461-18640 (slot 0)
			;   and :18641, :18779, :18917, :19055, :19193, :19331, :19469 (slots 1-7)
			;   resource/map/multi/bakhmut_1/campaign_capture_the_flag.mi:10162-11026
			; The bodies below are those triggers copied verbatim - same {autoreset}, same
			; count "==" 1 / ">" 1 split, same doubled linker+strip+re-arm action list, same
			; {sort {type entity} {entity {tag ...}}} pairing on both sides of the {"linker"}.
			;
			; TWO deliberate differences, and only two:
			;   1. THE NAMES ARE OURS. The tags are load-bearing and must be exact, because
			;      human.inc and parachute_new write and read them; the TRIGGER names are not,
			;      and reusing a shipped trigger name is banned - a map that ever ships
			;      "service/paradrop/N" would otherwise race this engine for the same chutes.
			;   2. THE E2 GATE. Terms 1 and 2 keep them inert exactly like every other new
			;      trigger in this engine: nothing arms at support_e2_test$ == 0, and the para
			;      leg is mode 2 (the combo transition switches into 2 before the para leg).
			;
			; None of this can reach a body that did not just leave one of our own aircraft:
			; heli_paradrop<N>/heli_parashute<N> are written by the engine's own entity
			; handlers at the instant of a jump and stripped again the moment the pair is
			; linked, so there is nothing standing for a later selector to catch.
			{"attack_support/e2_paradrop_link_0"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop0}
							}
							{count
								{op "=="}
								{value 1}
							}
						}
					}
				}
				{actions
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute0}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop0}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop0}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute0}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop0}
						}
						{tag_remove heli_paradrop0}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_0"}
					}
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute0}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop0}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop0}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute0}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop0}
						}
						{tag_remove heli_paradrop0}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_0"}
					}
				}
			}
			{"attack_support/e2_paradrop_purge_0"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop0}
							}
							{count
								{op ">"}
								{value 1}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{tag heli_paradrop0}
						}
						{tag_remove heli_paradrop0}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_0"}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop0}
						}
						{tag_remove heli_paradrop0}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_0"}
					}
				}
			}
			{"attack_support/e2_paradrop_link_1"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop1}
							}
							{count
								{op "=="}
								{value 1}
							}
						}
					}
				}
				{actions
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute1}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop1}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop1}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute1}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop1}
						}
						{tag_remove heli_paradrop1}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_1"}
					}
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute1}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop1}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop1}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute1}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop1}
						}
						{tag_remove heli_paradrop1}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_1"}
					}
				}
			}
			{"attack_support/e2_paradrop_purge_1"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop1}
							}
							{count
								{op ">"}
								{value 1}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{tag heli_paradrop1}
						}
						{tag_remove heli_paradrop1}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_1"}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop1}
						}
						{tag_remove heli_paradrop1}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_1"}
					}
				}
			}
			{"attack_support/e2_paradrop_link_2"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop2}
							}
							{count
								{op "=="}
								{value 1}
							}
						}
					}
				}
				{actions
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute2}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop2}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop2}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute2}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop2}
						}
						{tag_remove heli_paradrop2}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_2"}
					}
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute2}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop2}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop2}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute2}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop2}
						}
						{tag_remove heli_paradrop2}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_2"}
					}
				}
			}
			{"attack_support/e2_paradrop_purge_2"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop2}
							}
							{count
								{op ">"}
								{value 1}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{tag heli_paradrop2}
						}
						{tag_remove heli_paradrop2}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_2"}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop2}
						}
						{tag_remove heli_paradrop2}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_2"}
					}
				}
			}
			{"attack_support/e2_paradrop_link_3"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop3}
							}
							{count
								{op "=="}
								{value 1}
							}
						}
					}
				}
				{actions
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute3}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop3}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop3}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute3}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop3}
						}
						{tag_remove heli_paradrop3}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_3"}
					}
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute3}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop3}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop3}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute3}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop3}
						}
						{tag_remove heli_paradrop3}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_3"}
					}
				}
			}
			{"attack_support/e2_paradrop_purge_3"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop3}
							}
							{count
								{op ">"}
								{value 1}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{tag heli_paradrop3}
						}
						{tag_remove heli_paradrop3}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_3"}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop3}
						}
						{tag_remove heli_paradrop3}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_3"}
					}
				}
			}
			{"attack_support/e2_paradrop_link_4"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop4}
							}
							{count
								{op "=="}
								{value 1}
							}
						}
					}
				}
				{actions
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute4}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop4}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop4}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute4}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop4}
						}
						{tag_remove heli_paradrop4}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_4"}
					}
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute4}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop4}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop4}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute4}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop4}
						}
						{tag_remove heli_paradrop4}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_4"}
					}
				}
			}
			{"attack_support/e2_paradrop_purge_4"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop4}
							}
							{count
								{op ">"}
								{value 1}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{tag heli_paradrop4}
						}
						{tag_remove heli_paradrop4}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_4"}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop4}
						}
						{tag_remove heli_paradrop4}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_4"}
					}
				}
			}
			{"attack_support/e2_paradrop_link_5"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop5}
							}
							{count
								{op "=="}
								{value 1}
							}
						}
					}
				}
				{actions
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute5}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop5}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop5}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute5}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop5}
						}
						{tag_remove heli_paradrop5}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_5"}
					}
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute5}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop5}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop5}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute5}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop5}
						}
						{tag_remove heli_paradrop5}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_5"}
					}
				}
			}
			{"attack_support/e2_paradrop_purge_5"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop5}
							}
							{count
								{op ">"}
								{value 1}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{tag heli_paradrop5}
						}
						{tag_remove heli_paradrop5}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_5"}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop5}
						}
						{tag_remove heli_paradrop5}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_5"}
					}
				}
			}
			{"attack_support/e2_paradrop_link_6"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop6}
							}
							{count
								{op "=="}
								{value 1}
							}
						}
					}
				}
				{actions
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute6}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop6}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop6}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute6}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop6}
						}
						{tag_remove heli_paradrop6}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_6"}
					}
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute6}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop6}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop6}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute6}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop6}
						}
						{tag_remove heli_paradrop6}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_6"}
					}
				}
			}
			{"attack_support/e2_paradrop_purge_6"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop6}
							}
							{count
								{op ">"}
								{value 1}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{tag heli_paradrop6}
						}
						{tag_remove heli_paradrop6}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_6"}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop6}
						}
						{tag_remove heli_paradrop6}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_6"}
					}
				}
			}
			{"attack_support/e2_paradrop_link_7"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop7}
							}
							{count
								{op "=="}
								{value 1}
							}
						}
					}
				}
				{actions
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute7}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop7}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop7}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute7}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop7}
						}
						{tag_remove heli_paradrop7}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_7"}
					}
					{"linker"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag heli_parashute7}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag heli_paradrop7}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{group
								{select
									{tag
										{tag heli_paradrop7}
									}
								}
								{exclude
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
							{sort
								{type entity}
								{entity
									{tag heli_parashute7}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop7}
						}
						{tag_remove heli_paradrop7}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_link_7"}
					}
				}
			}
			{"attack_support/e2_paradrop_purge_7"
				{autoreset}
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.entities"
							{selector
								{tag heli_paradrop7}
							}
							{count
								{op ">"}
								{value 1}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{tag heli_paradrop7}
						}
						{tag_remove heli_paradrop7}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_7"}
					}
					{"entity_state"
						{selector
							{tag heli_paradrop7}
						}
						{tag_remove heli_paradrop7}
					}
					{"delay"
						{time 0.01}
					}
					{"trigger"
						{name "attack_support/e2_paradrop_purge_7"}
					}
				}
			}
			; Drives the closest-approach tracker for as long as stage 30 is open. The
			; engine arms a trigger once, so the poll re-arms itself explicitly (same shape
			; as attack_support/e2_para_landed) and stops the moment stage leaves 30 -
			; release sets 40, the timeout sets 60 and e2_para_alive sets 60.
			{"attack_support/e2_para_range"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "support_e2_stage$"} {op "=="} {value 30}}
					}
				}
				{actions
					("e2_para_range_poll")
					{"delay" {time 0.5}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 30}}
							{"trigger" {name "attack_support/e2_para_range"}}
						}
						{"default"}
					}
				}
			}

			; Stage 30 means "a claimed transport is flying the run-in". The live defect this
			; closes: e2_fly_para_or_fail proved the aircraft ONCE, and nothing afterwards ever
			; re-proved it, so the leg sat at stage 30 for 92 seconds with nothing in the sky
			; and blamed the release band (fail 6) for an aircraft that may not have been
			; there. This monitor is armed for the whole of stage 30 and fires the instant the
			; proof stops holding: 9 if no claimed aircraft exists at all, 10 if the hull is
			; still there but no longer operatable. Stage 30 is therefore unsustainable
			; without a live aircraft, and fail 6 is left to mean only "never approached".
			{"attack_support/e2_para_alive"
				{condition
					{expression "1 & 2 & 3 & !4 & !5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "support_e2_test$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "support_e2_stage$"} {op "=="} {value 30}}
						{"4.entities" {selector {ignore_captured_by_user 0} {tag support_e2_plane} {tag support_e2_aircraft} {tag support_e2_claim}} {count {op ">="} {value 1}}}
						; THE LIVE-ARRIVAL INTERLOCK, as a condition term. Term 4 is a bookkeeping
						; question and the 2026-07-30 run proved those tags can stop being
						; co-resident on an aircraft that is demonstrably flying. Term 5 is the
						; question about the world, on the provenance tag the numeric waypoint
						; wrote, in the advanced exclude form. While a live arrival exists this
						; monitor cannot fire at all, so neither 9 nor 10 can be written under it.
						{"5.entities" {selector {source advanced} {group {select {tag {tag support_e2_arrival}}} {exclude {state {state dead}} {state {state inactive}}}}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {ignore_captured_by_user 0} {tag support_e2_plane} {tag support_e2_aircraft} {tag support_e2_claim}}
								{count {op ">="} {value 1}}
							}
							{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}
						}
						{"default" {"set_i" {var "support_e2_fail$"} {op "="} {value 9}}}
					}
					{"set_i" {var "support_e2_stage$"} {op "="} {value 60}}
					("e2_fail_and_cleanup")
				}
			}

			; ===== MOTORIZED INSERT (cmd 19) =====
			; Truck + linked crew/pax place at entry, drive toward flag, emit passengers,
			; pax advance, truck holds. Cap attack_support_motor_left$=1. All four factions.
			; Truck-to-objective distance the moment before the passengers come out, as a
			; bounded integer band: 1 = inside 600, 2 = inside 1500, 3 = inside 4000,
			; 0 = further out than that (or no hull left to measure). SetVar takes integers
			; only, so the distance is carried as a band number and never as a coordinate.
			; Together with attack_support_motor_drive_t$ this settles "did the truck actually
			; drive?" from game.log alone: drive_t 4 with band 0 is a hull that never left the pad.
			;
			; UNITS (corrected 2026-07-30). Map coordinates are DECIMETRES - the same finding
			; that widened the E2 para release band, and independently checked here against
			; dcg_[cwa71]_fields: the map spans about 11400 x 6200 units, its two capture points
			; sit 1530 units apart and the entry pads carry {radius 150}. The bands used to read
			; 60/150/400, i.e. 6 m / 15 m / 40 m, and an entry pad is roughly 5000 units from the
			; nearest flag - so band 0 was the only outcome a real drive could ever produce, on
			; every engine, and it reported 0 on a Q4 drive the user watched succeed. The rings
			; are now 600/1500/4000 units = 60 m / 150 m / 400 m, which is what the numbers above
			; always meant, and band 0 once again means "barely left the pad".
			;
			; REFERENCE TAG (corrected 2026-07-30). This measures against the flag the hull was
			; actually ordered to - the motor path's own attack_support_motor_flag - not the
			; shared wave-flag tag the infantry path re-points on every wave.
						; LIVE-PROOF REFERENCE (2026-07-31). The dedicated motor objective remains
			; the order target, but tags added to capture-point entities did not survive as a
			; reliable near_to reference: successful drives still reported band 0. The proof
			; therefore measures the hull against the nearest active capture flag itself. A
			; truck that drove toward its chosen flag must approach one member of that set; a
			; truck that stayed at the edge remains outside all three rings.
(define "as_motor_band"
				{"set_i" {var "attack_support_motor_band$"} {op "="} {value 0}}
				{"switch"
					{"case" {condition {type near} {units {ignore_captured_by_user 0} {tag attack_support_motor_hull}} {near_to {ignore_captured_by_user 0} {tag attack_support_motor_flag}} {distance 600}} {"set_i" {var "attack_support_motor_band$"} {op "="} {value 1}}}
					{"case" {condition {type near} {units {ignore_captured_by_user 0} {tag attack_support_motor_hull}} {near_to {ignore_captured_by_user 0} {tag attack_support_motor_flag}} {distance 1500}} {"set_i" {var "attack_support_motor_band$"} {op "="} {value 2}}}
					{"case" {condition {type near} {units {ignore_captured_by_user 0} {tag attack_support_motor_hull}} {near_to {ignore_captured_by_user 0} {tag attack_support_motor_flag}} {distance 4000}} {"set_i" {var "attack_support_motor_band$"} {op "="} {value 3}}}
					{"default"}
				}
			)

			(define "as_finish_motor"
				; Motorized stage code, log-only (mirrored by attack_support.lua). 0 idle, 1 claimed,
				; 2 placed/promoted, 3 driving to the flag, 4 passengers emitted. Plain set_i:
				; nothing on screen, so no support_debug$ gate.
				{"delay" {time 0.5}}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag attack_support_deploy}}}}
					}
					{tag_add attack_support_src}
					{tag_remove attack_support_tpl}
					; The motorized packages come out of faction_support_templates.inc, whose
					; prototypes are parked under ally_sup_tpl - not the attack_support_tpl the
					; infantry pool uses. Q3 and Q4 already strip the right one; this path only
					; stripped the infantry marker, so a dispatched truck kept a template tag.
					{tag_remove ally_sup_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
				{"delay" {time 0.2}}
				; An infantry wave that finishes during this truck's placement window strips
				; the shared deploy tag from EVERY body carrying it - this hull and its riders
				; included. With four trucks a mission instead of one that window is hit often.
				; Re-assert the tag from the package's own hull/pax/crew marks before ownership
				; and AI control are set, or the truck is left neutral, driverless and motionless
				; - which is exactly what "the passengers appeared with no drive phase" looks
				; like from the outside.
				{"entity_state" {selector {tag attack_support_motor_hull}} {tag_add attack_support_deploy}}
				{"entity_state" {selector {tag attack_support_motor_pax}} {tag_add attack_support_deploy}}
				{"entity_state" {selector {tag attack_support_motor_crew}} {tag_add attack_support_deploy}}
				("am_own_to_support")
				{"delay" {time 0.4}}
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
					{control AI}
					{ai_move {mode enable}}
					{weapon_prepare on}
					{fire_mode open}
					{move_mode free}
					{movement {speed normal} {kind normal} {type normal}}
					{ai {no_retreat on} {advance_ratio 1} {retreat_ratio 0}}
				}
				{"ables"
					{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
					{remove select}
				}
				{"set_i" {var "attack_support_motor_stage$"} {op "="} {value 2}}
				; Drive telemetry starts at zero for every truck.
				{"set_i" {var "attack_support_motor_drive_t$"} {op "="} {value 0}}
				{"set_i" {var "attack_support_motor_band$"} {op "="} {value 0}}
				{"delay" {time 0.25}}
				; Pick one active flag as the approach objective, into a MOTOR-ONLY tag.
				;
				; This used to be attack_support_flag1 - the generic wave-flag tag that
				; am_finish_deploy (the infantry path of this same engine) clears at the top of
				; every wave and only re-points about 1.3s later. Nothing serialises the two:
				; infantry waves run off the wave ladder and trucks off attack_support/motor_clock,
				; four of each per mission. An infantry finisher that starts inside this pick's
				; window leaves the tag empty, the advance order below resolves to nothing, and
				; the hull is left with no order at all - crew seated, drive clock running, emit
				; firing on schedule, truck stationary. The pax order 31s later was exposed for
				; that entire span and would follow whichever flag the last infantry wave picked.
				; Q2 (def_sup_motor_flag) and Q4 (enemy_def_motor_flag) already own a dedicated
				; tag; this is the same shared-tag collision class as the deploy-tag race.
				{"entity_state"
					{selector {tag attack_support_motor_flag}}
					{tag_remove attack_support_motor_flag}
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
					{tag_add attack_support_motor_flag}
				}
				{"delay" {time 0.1}}
				; Truck drives toward the flag (standoff handled by emit after travel delay).
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_motor_hull}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_motor_flag}}
				}
				{"set_i" {var "attack_support_motor_stage$"} {op "="} {value 3}}
				; DRIVE CLOCK. The 28s standoff is unchanged - it is the live-proven sequence
				; from the original motorized insert, byte for byte - but it is now readable
				; from game.log. attack_support_motor_drive_t$ steps 0 -> 4, one step per 7 seconds
				; that actually elapsed, so an emit observed at drive_t < 4 proves the delay did
				; not run, and an emit at drive_t 4 with band 0 proves the hull never moved.
				{"delay" {time 7}}
				{"set_i" {var "attack_support_motor_drive_t$"} {op "="} {value 1}}
				{"delay" {time 7}}
				{"set_i" {var "attack_support_motor_drive_t$"} {op "="} {value 2}}
				{"delay" {time 7}}
				{"set_i" {var "attack_support_motor_drive_t$"} {op "="} {value 3}}
				{"delay" {time 7}}
				{"set_i" {var "attack_support_motor_drive_t$"} {op "="} {value 4}}
				("as_motor_band")
				; Dismount passengers only (crew stays).
				{"emit"
					{selector
						{ignore_captured_by_user 0}
						{tag attack_support_motor_hull}
						{type vehicle}
						{state inhabited}
					}
					{drop orders}
					{emit
						{mode passengers}
					}
				}
				{"set_i" {var "attack_support_motor_stage$"} {op "="} {value 4}}
				{"delay" {time 3}}
				; Pax advance on foot; truck holds. Same motor-only objective the hull drove at,
				; so the dismounts finish the approach instead of chasing the infantry's flag.
				{"action"
					{selector
						{source advanced}
						{ignore_captured_by_user 0}
						{group
							{select {tag {tag attack_support_motor_pax}}}
							{exclude {state {state linked}} {state {state inactive}} {state {state dead}}}
						}
					}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_motor_flag}}
				}
				; Driver leaves the field instead of idling: drive for waypoint "0" and the
				; cleanup trigger below removes the hull 45s later, wreck included.
				{"entity_state"
					{selector {ignore_captured_by_user 0} {tag attack_support_motor_hull}}
					{tag_add am_motor_leaving}
				}
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_motor_hull}}
					{drop orders}
					{action move}
					{waypoint "0"}
				}
				; Scoped strip. The unscoped form cleared the deploy tag from every body that
				; carried it, which disarmed any infantry wave that happened to be placing at
				; the same moment - the same collision as above, in the other direction.
				{"entity_state"
					{selector {tag attack_support_motor_hull}}
					{tag_remove attack_support_deploy}
				}
				{"entity_state"
					{selector {tag attack_support_motor_pax}}
					{tag_remove attack_support_deploy}
				}
				{"entity_state"
					{selector {tag attack_support_motor_crew}}
					{tag_remove attack_support_deploy}
					{tag_remove attack_support_motor_crew}
				}
				{"entity_state"
					{selector {tag attack_support_motor_hull}}
					{tag_remove attack_support_motor_hull}
				}
				{"entity_state"
					{selector {tag attack_support_motor_pax}}
					{tag_remove attack_support_motor_pax}
				}
				{"entity_state"
					{selector {tag attack_support_g1}}
					{tag_remove attack_support_g1}
				}
				{"entity_state"
					{selector {tag attack_support_g2}}
					{tag_remove attack_support_g2}
				}
				{"entity_state"
					{selector {tag attack_support_g3}}
					{tag_remove attack_support_g3}
				}
				{"entity_state"
					{selector {tag attack_support_g4}}
					{tag_remove attack_support_g4}
				}
				{"set_i" {var "attack_support_transferred$"} {op "="} {value 1}}
			)


			{"attack_support/ally_rusa_motor"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 19}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_motor_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_rusa_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_motor")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT MOTORIZED INSERT"}}
						}
						{"default"}
					}
					; Claim whole package (hull + linked crew + pax).
				{"switch"
					{"case"
						{condition {type entities} {selector {tag ally_sup_rusa_p1_hull}} {count {op ">="} {value 1}}}
						; Numbered package: one hull, ITS {Link}-baked crew and its own riders. The case is
						; keyed on the package still being parked, not on the cursor: two engines run on every
						; mission and a mirror matchup (NATO vs CSA, RUSA vs SOV) folds both of them onto the
						; same faction pool. Whoever claims first strips the numbered tags, so the other engine
						; falls through to the next package instead of claiming a truck that is already on the
						; road. Bare single-tag selects only; the deploy tag is what routes the crew through the
						; ownership switch.
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_rusa_p1_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_rusa_p1_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_rusa_p1_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_rusa_motor_hull}
							{tag_remove ally_sup_rusa_p1_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_rusa_p1_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_rusa_motor_pax}
							{tag_remove ally_sup_rusa_p1_pax}
						}
					}
					{"case"
						{condition {type entities} {selector {tag ally_sup_rusa_p2_hull}} {count {op ">="} {value 1}}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_rusa_p2_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_rusa_p2_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_rusa_p2_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_rusa_motor_hull}
							{tag_remove ally_sup_rusa_p2_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_rusa_p2_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_rusa_motor_pax}
							{tag_remove ally_sup_rusa_p2_pax}
						}
					}
					{"case"
						{condition {type entities} {selector {tag ally_sup_rusa_p3_hull}} {count {op ">="} {value 1}}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_rusa_p3_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_rusa_p3_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_rusa_p3_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_rusa_motor_hull}
							{tag_remove ally_sup_rusa_p3_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_rusa_p3_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_rusa_motor_pax}
							{tag_remove ally_sup_rusa_p3_pax}
						}
					}
					{"case"
						{condition {type entities} {selector {tag ally_sup_rusa_p4_hull}} {count {op ">="} {value 1}}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_rusa_p4_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_rusa_p4_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_rusa_p4_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_rusa_motor_hull}
							{tag_remove ally_sup_rusa_p4_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_rusa_p4_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_rusa_motor_pax}
							{tag_remove ally_sup_rusa_p4_pax}
						}
					}
					{"default"}
				}
				{"set_i" {var "attack_support_motor_pkg$"} {op "+"} {value 1}}
					{"set_i" {var "attack_support_motor_left$"} {op "-"} {value 1}}
					{"set_i" {var "attack_support_motor_stage$"} {op "="} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("as_finish_motor")
				}
			}

			{"attack_support/ally_ukr_motor"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 19}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_motor_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_ukr_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_motor")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT MOTORIZED INSERT"}}
						}
						{"default"}
					}
					; Claim whole package (hull + linked crew + pax).
				{"switch"
					{"case"
						{condition {type entities} {selector {tag ally_sup_ukr_p1_hull}} {count {op ">="} {value 1}}}
						; Numbered package: one hull, ITS {Link}-baked crew and its own riders. The case is
						; keyed on the package still being parked, not on the cursor: two engines run on every
						; mission and a mirror matchup (NATO vs CSA, RUSA vs SOV) folds both of them onto the
						; same faction pool. Whoever claims first strips the numbered tags, so the other engine
						; falls through to the next package instead of claiming a truck that is already on the
						; road. Bare single-tag selects only; the deploy tag is what routes the crew through the
						; ownership switch.
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_ukr_p1_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_ukr_p1_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_ukr_p1_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_ukr_motor_hull}
							{tag_remove ally_sup_ukr_p1_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_ukr_p1_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_ukr_motor_pax}
							{tag_remove ally_sup_ukr_p1_pax}
						}
					}
					{"case"
						{condition {type entities} {selector {tag ally_sup_ukr_p2_hull}} {count {op ">="} {value 1}}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_ukr_p2_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_ukr_p2_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_ukr_p2_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_ukr_motor_hull}
							{tag_remove ally_sup_ukr_p2_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_ukr_p2_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_ukr_motor_pax}
							{tag_remove ally_sup_ukr_p2_pax}
						}
					}
					{"case"
						{condition {type entities} {selector {tag ally_sup_ukr_p3_hull}} {count {op ">="} {value 1}}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_ukr_p3_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_ukr_p3_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_ukr_p3_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_ukr_motor_hull}
							{tag_remove ally_sup_ukr_p3_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_ukr_p3_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_ukr_motor_pax}
							{tag_remove ally_sup_ukr_p3_pax}
						}
					}
					{"case"
						{condition {type entities} {selector {tag ally_sup_ukr_p4_hull}} {count {op ">="} {value 1}}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_ukr_p4_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_ukr_p4_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_ukr_p4_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_ukr_motor_hull}
							{tag_remove ally_sup_ukr_p4_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_ukr_p4_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_ukr_motor_pax}
							{tag_remove ally_sup_ukr_p4_pax}
						}
					}
					{"default"}
				}
				{"set_i" {var "attack_support_motor_pkg$"} {op "+"} {value 1}}
					{"set_i" {var "attack_support_motor_left$"} {op "-"} {value 1}}
					{"set_i" {var "attack_support_motor_stage$"} {op "="} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("as_finish_motor")
				}
			}


			{"attack_support/ally_prc_motor"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 19}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_motor_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_prc_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_motor")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT MOTORIZED INSERT"}}
						}
						{"default"}
					}
				{"switch"
					{"case"
						{condition {type entities} {selector {tag ally_sup_prc_p1_hull}} {count {op ">="} {value 1}}}
						; Numbered package: one hull, ITS {Link}-baked crew and its own riders. The case is
						; keyed on the package still being parked, not on the cursor: two engines run on every
						; mission and a mirror matchup (NATO vs CSA, RUSA vs SOV) folds both of them onto the
						; same faction pool. Whoever claims first strips the numbered tags, so the other engine
						; falls through to the next package instead of claiming a truck that is already on the
						; road. Bare single-tag selects only; the deploy tag is what routes the crew through the
						; ownership switch.
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_prc_p1_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_prc_p1_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_prc_p1_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_prc_motor_hull}
							{tag_remove ally_sup_prc_p1_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_prc_p1_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_prc_motor_pax}
							{tag_remove ally_sup_prc_p1_pax}
						}
					}
					{"case"
						{condition {type entities} {selector {tag ally_sup_prc_p2_hull}} {count {op ">="} {value 1}}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_prc_p2_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_prc_p2_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_prc_p2_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_prc_motor_hull}
							{tag_remove ally_sup_prc_p2_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_prc_p2_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_prc_motor_pax}
							{tag_remove ally_sup_prc_p2_pax}
						}
					}
					{"case"
						{condition {type entities} {selector {tag ally_sup_prc_p3_hull}} {count {op ">="} {value 1}}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_prc_p3_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_prc_p3_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_prc_p3_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_prc_motor_hull}
							{tag_remove ally_sup_prc_p3_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_prc_p3_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_prc_motor_pax}
							{tag_remove ally_sup_prc_p3_pax}
						}
					}
					{"case"
						{condition {type entities} {selector {tag ally_sup_prc_p4_hull}} {count {op ">="} {value 1}}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_prc_p4_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_prc_p4_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_prc_p4_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_prc_motor_hull}
							{tag_remove ally_sup_prc_p4_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_prc_p4_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_prc_motor_pax}
							{tag_remove ally_sup_prc_p4_pax}
						}
					}
					{"default"}
				}
				{"set_i" {var "attack_support_motor_pkg$"} {op "+"} {value 1}}
					{"set_i" {var "attack_support_motor_left$"} {op "-"} {value 1}}
					{"set_i" {var "attack_support_motor_stage$"} {op "="} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("as_finish_motor")
				}
			}

			{"attack_support/ally_nato_motor"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 19}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_motor_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_nato_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_motor")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT MOTORIZED INSERT"}}
						}
						{"default"}
					}
					; Claim whole package (hull + linked crew + pax).
				{"switch"
					{"case"
						{condition {type entities} {selector {tag ally_sup_nato_p1_hull}} {count {op ">="} {value 1}}}
						; Numbered package: one hull, ITS {Link}-baked crew and its own riders. The case is
						; keyed on the package still being parked, not on the cursor: two engines run on every
						; mission and a mirror matchup (NATO vs CSA, RUSA vs SOV) folds both of them onto the
						; same faction pool. Whoever claims first strips the numbered tags, so the other engine
						; falls through to the next package instead of claiming a truck that is already on the
						; road. Bare single-tag selects only; the deploy tag is what routes the crew through the
						; ownership switch.
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_nato_p1_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_nato_p1_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_nato_p1_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_nato_motor_hull}
							{tag_remove ally_sup_nato_p1_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_nato_p1_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_nato_motor_pax}
							{tag_remove ally_sup_nato_p1_pax}
						}
					}
					{"case"
						{condition {type entities} {selector {tag ally_sup_nato_p2_hull}} {count {op ">="} {value 1}}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_nato_p2_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_nato_p2_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_nato_p2_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_nato_motor_hull}
							{tag_remove ally_sup_nato_p2_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_nato_p2_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_nato_motor_pax}
							{tag_remove ally_sup_nato_p2_pax}
						}
					}
					{"case"
						{condition {type entities} {selector {tag ally_sup_nato_p3_hull}} {count {op ">="} {value 1}}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_nato_p3_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_nato_p3_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_nato_p3_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_nato_motor_hull}
							{tag_remove ally_sup_nato_p3_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_nato_p3_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_nato_motor_pax}
							{tag_remove ally_sup_nato_p3_pax}
						}
					}
					{"case"
						{condition {type entities} {selector {tag ally_sup_nato_p4_hull}} {count {op ">="} {value 1}}}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_nato_p4_crew}}}}}
							{tag_add attack_support_deploy}
							{tag_add attack_support_motor_crew}
							{tag_remove ally_sup_nato_p4_crew}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_nato_p4_hull}}}}}
							{tag_add attack_support_motor_hull}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_nato_motor_hull}
							{tag_remove ally_sup_nato_p4_hull}
						}
						{"entity_state"
							{selector {source advanced} {group {select {tag {tag ally_sup_nato_p4_pax}}}}}
							{tag_add attack_support_motor_pax}
							{tag_add attack_support_deploy}
							{tag_remove ally_sup_nato_motor_pax}
							{tag_remove ally_sup_nato_p4_pax}
						}
					}
					{"default"}
				}
				{"set_i" {var "attack_support_motor_pkg$"} {op "+"} {value 1}}
					{"set_i" {var "attack_support_motor_left$"} {op "-"} {value 1}}
					{"set_i" {var "attack_support_motor_stage$"} {op "="} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("as_finish_motor")
				}
			}

			; TEST ONLY - forced motorized inserts, staggered. Gate attack_support_motor_test$:
			;   1 = force two trucks (armed+15s, +60s), bypassing the L2+ gate and the roll
			;   0 = production only (L2+ random roll, cap 1) - flip in attack_support/init
			; The 45s spacing exceeds one truck's full drive+emit sequence, so two trucks
			; never share a pad or an unload zone.
						; Motorised inserts run on their own randomised clock, independent of the wave
			; ladder so truck and infantry arrivals never sync. First truck 90-150s after the
			; engine arms, then every 180-300s until the per-mission budget of four is spent.
			; The pool parks four packages per faction, so every one of those can be filled.
			{"attack_support/motor_clock"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "attack_support_armed$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "attack_support_motor_left$"} {op ">"} {value 0}}
						{"3.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"4.cmp_i" {var "faction_support_army$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"switch"
						{"case"
							{condition {type cmp_i} {var "attack_support_motor_first$"} {op "=="} {value 0}}
							{"set_i" {var "attack_support_motor_first$"} {op "="} {value 1}}
							{"switch"
								{"case" {condition {type rand} {value 0.34}} {"delay" {time 62}}}
								{"case" {condition {type rand} {value 0.5}} {"delay" {time 88}}}
								{"default" {"delay" {time 114}}}
							}
						}
						{"default"
							{"switch"
								{"case" {condition {type rand} {value 0.34}} {"delay" {time 185}}}
								{"case" {condition {type rand} {value 0.5}} {"delay" {time 250}}}
								{"default" {"delay" {time 310}}}
							}
						}
					}
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 19}}
					("as_poke_faction_motor")
					{"delay" {time 20}}
					{"trigger" {name "attack_support/motor_clock"}}
				}
			}

			; One-shot per truck: give the departing hull 45s of driving, then remove it
			; (dead or alive) so trucks never accumulate. Fires only when a hull is leaving.
			{"attack_support/motor_cleanup"
				{autoreset}
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.entities" {selector {tag am_motor_leaving}} {count {op ">"} {value 0}}}
					}
				}
				{actions
					{"delay" {time 45}}
					{"delete" {selector {ignore_captured_by_user 0} {tag am_motor_leaving}}}
				}
			}
