; Defensive support waves - the human defender's AI teammate reinforcement engine.
; Quadrant 2 of the four-way support parity: attack_support_waves.inc gives the human
; ATTACKER a teammate, enemy_defense_support.inc gives the AI defender a garrison on an
; attack mission, enemy_attack_support.inc gives the AI attacker pressure on a defence
; mission, and this file gives the human DEFENDER a friendly AI defender that actually
; reinforces the flags it is supposed to hold.
;
; Included by all fourteen CWA campaign_capture_the_flag.mi maps. Provably inert on a
; human-ATTACK mission: every single trigger carries user_is_defender$ == 1, and
; defense_support/init additionally waits for enemy_spawnside$ > 0 and prep_inform$ == 1.
; enemy_spawnside$ is the readiness proof: conquest.lua writes it immediately after
; user_is_defender$ inside the same mission-authority call, so a positive spawn side
; proves the perspective vars were actually published rather than merely defaulting to 0.
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
; OWNERSHIP is a literal 1-16 {player} switch on id_defenderbot$ - the engine will not
; accept a var in a {player} node. id_defenderbot$ is conquest.lua's DefenderBotId
; (BotApi.Conquest.DefenderBotId, falling back to CampaignDefenderBotId), published by
; publishConquestIds from every bot because it is perspective-neutral. It is the only
; published id that can name a friendly AI defender: id_1st_player$ is the human's own
; slot, id_1st_enemy$ is the enemy bot, and id_attack_support$ only exists on an attack
; mission. UNLIKE the other three engines this one does NOT guess in its default branch:
; if id_defenderbot$ is not a value the switch spells out, ds_own_to_defenderbot sets
; defense_support_owner_fail$ and transfers nothing, so a wave is stranded off-map at
; player 0 rather than handed to whoever happens to sit in slot 3. init logs the resolved
; slot with a per-value diagnostic timer so a live run pins the real number.
;
; FACTION: NATO only. This campaign's player side is NATO, so the shared pool's USMC /
; 1AD / pzgren teams are already the right nationality. That is a v1 limitation, not a
; design: a non-NATO player side would get NATO-breed reinforcements. Fixing it means a
; player-nation pool switch in the way enemy_attack_support.inc switches on bot_army$.
;
; PREP PHASE. A defence mission has a real preparation phase (preparationTime 480 in
; campaign_capture_the_flag.set) and prep_inform$ goes to 1 when it ends
; (conquest.lua OnPrepTimeOver). Nothing here pre-places anything: garrison-style
; delivery would drop AI bodies into the middle of the player's own prep placement, so
; every spawner waits for prep to be over.
;
; WHAT ARRIVES, AND WHEN
;   opening wave  25/32/40s after prep ends, one 4-man USMC rifle team
;   recurring     self-re-arming clock, randomized 140/180/215/250/290s cadence
;                 (weighted {type rand} cascade, ~20% per bucket)
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

			; Delivery point: the DEFENDER's own map edge, which is the side the enemy
			; is NOT on. enemy_spawnside$ is published from conquest.lua's
			; mission-authority branch and on a defence mission the mission authority
			; is the enemy bot, i.e. the ATTACKER - so side 1 (a) means the attacker is
			; on a and the player/defender edge is b. That is the same reading
			; attack_support_waves.inc uses and the opposite of the reading
			; enemy_defense_support.inc uses, which delivers to the enemy's own edge.
			(define "ds_place_at_entry"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"placement"
							{selector
								{source advanced}
								{group {select {tag {tag def_sup_deploy}}}}
							}
							{target_waypoint "attack_support_entry_b"}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement"
							{selector
								{source advanced}
								{group {select {tag {tag def_sup_deploy}}}}
							}
							{target_waypoint "attack_support_entry_a"}
						}
					}
					{"default"
						{"placement"
							{selector
								{source advanced}
								{group {select {tag {tag def_sup_deploy}}}}
							}
							{target_waypoint "attack_support_entry_b"}
						}
					}
				}
			)

			; Ownership. The engine will not accept a var in a {player} node, so all
			; sixteen slots are spelled out against id_defenderbot$. The default branch
			; deliberately does NOT transfer: an unresolved defender bot would otherwise
			; hand a wave to a guessed slot, which on a defence mission could be the
			; attacker. defense_support_owner_fail$ records that instead, and the wave
			; simply stays at player 0 where it can do no harm.
			(define "ds_own_to_defenderbot"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag def_sup_deploy}} {operation set} {player "16"}}}
					{"default"
						{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}
						{"timer" {time 10} {title "DEFENSE SUPPORT OWNER UNRESOLVED - NO TRANSFER"}}
					}
				}
			)

			; Diagnostic only, fired once from init. Timer titles cannot interpolate a
			; var, so the resolved defender-bot slot is reported by spelling out the
			; sixteen possibilities. A live run reads the real number off this line.
			(define "ds_report_owner"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 1}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 1"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 2}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 2"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 3}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 3"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 4}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 4"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 5}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 5"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 6}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 6"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 7}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 7"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 8}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 8"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 9}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 9"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 10}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 10"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 11}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 11"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 12}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 12"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 13}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 13"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 14}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 14"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 15}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 15"}}}
					{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 16}} {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - SLOT 16"}}}
					{"default" {"timer" {time 10} {title "DEFENSE SUPPORT OWNER - UNRESOLVED"}}}
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
					{selector {ignore_captured_by_user 0} {tag def_sup_deploy}}
					{control AI}
					{ai_move {mode enable}}
					{weapon_prepare on}
					{fire_mode open}
					{move_mode free}
					{movement {speed normal} {kind normal} {type normal}}
					{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}
				}
				{"ables"
					{selector {ignore_captured_by_user 0} {tag def_sup_deploy}}
					{remove select}
				}
				{"delay" {time 1}}
				("ds_assign_group")
				{"delay" {time 0.2}}
				; Advance on the flag this group holds, falling back to af1 on a map
				; where the mission activated fewer flag points than we have groups.
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_group$"} {op "=="} {value 2}}
						{"switch"
							{"case"
								{condition {type entities} {selector {tag def_sup_af2}}}
								{"action"
									{selector {ignore_captured_by_user 0} {tag def_sup_deploy}}
									{drop orders}
									{action advance}
									{target {ignore_captured_by_user 0} {tag def_sup_af2}}
								}
							}
							{"default"
								{"action"
									{selector {ignore_captured_by_user 0} {tag def_sup_deploy}}
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
									{selector {ignore_captured_by_user 0} {tag def_sup_deploy}}
									{drop orders}
									{action advance}
									{target {ignore_captured_by_user 0} {tag def_sup_af3}}
								}
							}
							{"default"
								{"action"
									{selector {ignore_captured_by_user 0} {tag def_sup_deploy}}
									{drop orders}
									{action advance}
									{target {ignore_captured_by_user 0} {tag def_sup_af1}}
								}
							}
						}
					}
					{"default"
						{"action"
							{selector {ignore_captured_by_user 0} {tag def_sup_deploy}}
							{drop orders}
							{action advance}
							{target {ignore_captured_by_user 0} {tag def_sup_af1}}
						}
					}
				}
				; Long enough to walk in from the map edge, then dig in where they landed.
				{"delay" {time 45}}
				{"actor_to_cover"
					{selector {ignore_captured_by_user 0} {tag def_sup_deploy}}
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

			; One composition per wave, drawn at random from the pool this campaign level
			; unlocks. The command IS the composition id; the matching trigger clears it
			; as its first action, so a command still standing a few seconds later means
			; that composition's pool could not field the wave.
			(define "ds_pick_composition"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
						{"switch"
							{"case"
								{condition {type rand} {value 0.4}}
								{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 3}}
								{"trigger" {name "defense_support/comp_pzgd"}}
							}
							{"case"
								{condition {type rand} {value 0.5}}
								{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 2}}
								{"trigger" {name "defense_support/comp_1ad"}}
							}
							{"default"
								{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 1}}
								{"trigger" {name "defense_support/comp_usmc"}}
							}
						}
					}
					{"case"
						{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
						{"switch"
							{"case"
								{condition {type rand} {value 0.25}}
								{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 3}}
								{"trigger" {name "defense_support/comp_pzgd"}}
							}
							{"case"
								{condition {type rand} {value 0.5}}
								{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 2}}
								{"trigger" {name "defense_support/comp_1ad"}}
							}
							{"default"
								{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 1}}
								{"trigger" {name "defense_support/comp_usmc"}}
							}
						}
					}
					; Level 1, and level 0 when CE has not published a level yet:
					; plain rifle teams.
					{"default"
						{"switch"
							{"case"
								{condition {type rand} {value 0.5}}
								{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 2}}
								{"trigger" {name "defense_support/comp_1ad"}}
							}
							{"default"
								{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 1}}
								{"trigger" {name "defense_support/comp_usmc"}}
							}
						}
					}
				}
				; Pool-short fallback: step down to the always-deepest USMC rifle team,
				; then give up on this cycle rather than spin.
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_wave_cmd$"} {op ">"} {value 1}}
						{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 1}}
						{"trigger" {name "defense_support/comp_usmc"}}
						{"timer" {time 6} {title "DEFENSE SUPPORT POOL SHORT - RIFLE TEAM INSTEAD"}}
					}
					{"default"}
				}
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_support_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
						{"timer" {time 6} {title "DEFENSE SUPPORT POOL EXHAUSTED"}}
					}
					{"default"}
				}
			)

			; Arms once per defence mission. prep_inform$ == 1 is the prep-phase gate:
			; a defence mission really does run a preparation phase, and dropping AI
			; bodies into it would land them on top of the player's own placement.
			; enemy_spawnside$ > 0 is the readiness proof - conquest.lua writes it
			; immediately after user_is_defender$ inside the same mission-authority
			; call, so a positive spawn side proves the perspective vars are real rather
			; than merely defaulting. The clock stays parked behind
			; defense_support_busy$ / defense_support_next_ok$ until the opening wave has
			; been issued, otherwise its condition is already true here and it fires
			; alongside init.
			{"defense_support/init"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "defense_support_armed$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "prep_inform$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "enemy_spawnside$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "defense_support_armed$"} {op "="} {value 1}}
					{"set_i" {var "defense_support_busy$"} {op "="} {value 1}}
					{"set_i" {var "defense_support_next_ok$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_wave_num$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_group$"} {op "="} {value 1}}
					{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_stage$"} {op "="} {value 1}}
					("ds_report_owner")
					; Wave budget by campaign progression. defense_level$ is computed by
					; the CE mission setup (1-3, roughly campaign games 0 / 6 / 12) on
					; defence missions as well as attack ones.
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
							{"set_i" {var "defense_support_waves_left$"} {op "="} {value 8}}
							{"timer" {time 10} {title "DEFENSE SUPPORT ARMED - L3, 8 WAVES"}}
						}
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
							{"set_i" {var "defense_support_waves_left$"} {op "="} {value 6}}
							{"timer" {time 10} {title "DEFENSE SUPPORT ARMED - L2, 6 WAVES"}}
						}
						{"default"
							{"set_i" {var "defense_support_waves_left$"} {op "="} {value 4}}
							{"timer" {time 10} {title "DEFENSE SUPPORT ARMED - L1, 4 WAVES"}}
						}
					}
					("ds_claim_anchors")
					; Opening wave: one small USMC rifle team, 25-40s after prep ends.
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
					{"set_i" {var "defense_support_wave_num$"} {op "+"} {value 1}}
					{"set_i" {var "defense_support_waves_left$"} {op "-"} {value 1}}
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 1}}
					{"trigger" {name "defense_support/comp_usmc"}}
					; Hand the cadence over only once the opening wave has been issued.
					{"delay" {time 20}}
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
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "defense_support_next_ok$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_busy$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "defense_support_waves_left$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"5.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "defense_support_busy$"} {op "="} {value 1}}
					; Randomized 140-290s cadence: weighted cascade, ~20% per bucket.
					{"switch"
						{"case"
							{condition {type rand} {value 0.2}}
							{"delay" {time 140}}
						}
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 180}}
						}
						{"case"
							{condition {type rand} {value 0.33}}
							{"delay" {time 215}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 250}}
						}
						{"default"
							{"delay" {time 290}}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						; LIVE-UNIT CAP. Skip this cycle without consuming a wave while
						; the friendly defender still has a platoon on the field. Counted
						; on the simple selector form the mission scripts use for live
						; units - the advanced selector's prop/state decorations zero the
						; match on these entities.
						{"case"
							{condition
								{type entities}
								{selector
									{ignore_captured_by_user 0}
									{tag def_sup_src}
									{type human}
									{state "not dead"}
								}
								{count {op ">"} {value 14}}
							}
							{"timer" {time 8} {title "DEFENSE SUPPORT NEAR CAP DEFER"}}
						}
						{"default"
							{"set_i" {var "defense_support_wave_num$"} {op "+"} {value 1}}
							{"set_i" {var "defense_support_waves_left$"} {op "-"} {value 1}}
							; Rotate the hold group so waves spread over the flags.
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
						}
					}
					{"delay" {time 0.1}}
					{"set_i" {var "defense_support_busy$"} {op "="} {value 0}}
					{"trigger" {name "defense_support/clock"}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_support_waves_left$"} {op "=="} {value 0}}
							{"timer" {time 10} {title "DEFENSE SUPPORT WAVES EXHAUSTED"}}
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
					{"switch"
						{"case"
							{condition {type rand} {value 0.34}}
							{"action"
								{selector {ignore_captured_by_user 0} {tag def_sup_h1}}
								{drop orders}
								{action advance}
								{target {ignore_captured_by_user 0} {tag def_sup_af1}}
							}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag def_sup_af2}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag def_sup_h1}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag def_sup_af2}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag def_sup_h1}}
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
										{selector {ignore_captured_by_user 0} {tag def_sup_h1}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag def_sup_af3}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag def_sup_h1}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag def_sup_af1}}
									}
								}
							}
						}
					}
					; Walk in, then dig in on the objective rather than mill about on it.
					{"delay" {time 40}}
					{"actor_to_cover"
						{selector {ignore_captured_by_user 0} {tag def_sup_h1}}
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
					{"switch"
						{"case"
							{condition {type rand} {value 0.34}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag def_sup_af2}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag def_sup_h2}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag def_sup_af2}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag def_sup_h2}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag def_sup_af1}}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"action"
								{selector {ignore_captured_by_user 0} {tag def_sup_h2}}
								{drop orders}
								{action advance}
								{target {ignore_captured_by_user 0} {tag def_sup_af1}}
							}
						}
						{"default"
							{"switch"
								{"case"
									{condition {type entities} {selector {tag def_sup_af3}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag def_sup_h2}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag def_sup_af3}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag def_sup_h2}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag def_sup_af1}}
									}
								}
							}
						}
					}
					; Walk in, then dig in on the objective rather than mill about on it.
					{"delay" {time 40}}
					{"actor_to_cover"
						{selector {ignore_captured_by_user 0} {tag def_sup_h2}}
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
					{"switch"
						{"case"
							{condition {type rand} {value 0.34}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag def_sup_af3}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag def_sup_h3}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag def_sup_af3}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag def_sup_h3}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag def_sup_af1}}
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
										{selector {ignore_captured_by_user 0} {tag def_sup_h3}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag def_sup_af2}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag def_sup_h3}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag def_sup_af1}}
									}
								}
							}
						}
						{"default"
							{"action"
								{selector {ignore_captured_by_user 0} {tag def_sup_h3}}
								{drop orders}
								{action advance}
								{target {ignore_captured_by_user 0} {tag def_sup_af1}}
							}
						}
					}
					; Walk in, then dig in on the objective rather than mill about on it.
					{"delay" {time 40}}
					{"actor_to_cover"
						{selector {ignore_captured_by_user 0} {tag def_sup_h3}}
					}
					{"delay" {time 0.1}}
					{"trigger" {name "defense_support/hold_3"}}
				}
			}

			; COMPOSITION 1 - 4 USMC infantry. Also the pool-short fallback, so this
			; pool is the deepest one the shared NATO pool ships. Taking the bodies
			; strips the pool tag, so the gate below can simply count the tag to know
			; what is still parked and available.
			{"defense_support/comp_usmc"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_wave_cmd$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_usmc}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_stage$"} {op "="} {value 11}}
					{"timer" {time 8} {title "DEFENSE SUPPORT WAVE - USMC RIFLE TEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_usmc}}}}
							{amount 4}
						}
						{tag_add def_sup_deploy}
						{tag_remove attack_support_inf_usmc}
					}
					{"delay" {time 0.1}}
					("ds_place_at_entry")
					("ds_finish")
					{"set_i" {var "defense_support_stage$"} {op "="} {value 12}}
				}
			}

			; COMPOSITION 2 - 4 1AD infantry.
			{"defense_support/comp_1ad"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_wave_cmd$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_1ad}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_stage$"} {op "="} {value 21}}
					{"timer" {time 8} {title "DEFENSE SUPPORT WAVE - 1AD RIFLE TEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_1ad}}}}
							{amount 4}
						}
						{tag_add def_sup_deploy}
						{tag_remove attack_support_inf_1ad}
					}
					{"delay" {time 0.1}}
					("ds_place_at_entry")
					("ds_finish")
					{"set_i" {var "defense_support_stage$"} {op "="} {value 22}}
				}
			}

			; COMPOSITION 3 (level 2+) - 6 German pzgren, the heavy hold team. The shared
			; pzgren pool is only twelve deep, so this fields two draws per mission and
			; then the pool-short fallback steps the pick down to the USMC rifle team.
			{"defense_support/comp_pzgd"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "defense_support_wave_cmd$"} {op "=="} {value 3}}
						{"3.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_pzgd}}
							{count {op ">="} {value 6}}
						}
					}
				}
				{actions
					{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "defense_support_stage$"} {op "="} {value 31}}
					{"timer" {time 8} {title "DEFENSE SUPPORT WAVE - PZGREN HOLD TEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_pzgd}}}}
							{amount 6}
						}
						{tag_add def_sup_deploy}
						{tag_remove attack_support_inf_pzgd}
					}
					{"delay" {time 0.1}}
					("ds_place_at_entry")
					("ds_finish")
					{"set_i" {var "defense_support_stage$"} {op "="} {value 32}}
				}
			}
