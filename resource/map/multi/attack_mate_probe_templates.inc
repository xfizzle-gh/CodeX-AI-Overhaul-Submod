; Off-map prototype pool for the attack-mate probe. REAL BREEDS, deliberately.
; Every selector anomaly this probe has hit - {include {prop {prop human}}}
; matching zero, state excludes matching zero, clones being unfindable - showed up
; only on the breed-less {Human ""} templates in allied_support_templates.inc.
; Indomitus and DOTD only ever exercise this machinery on real-breed actors, so the
; probe now uses its own real-breed pool and leaves the breed-less one untouched.
;
; Breed path "mp/nato/2022s/1ad_rifleman" is verified twice over:
;   - the file exists: <Code:X base 3261086933>/resource/set/breed/mp/nato/2022s/
;     1ad_rifleman.set, a {breed} with behaviour soldier, armors, perks and an
;     m4a1_v3b inventory;
;   - the string is already used verbatim by this mod's own purchase list,
;     resource/set/multiplayer/units/conquest/inf_nato.set:155.
; The path-to-file rule ({Human "<p>"} -> resource/set/breed/<p>.set) is proven by
; DOTD: {Human "dotd/-common/-civilians/civilian_00"} resolves to
; <DOTD 3456171322>/resource/set/breed/dotd/-common/-civilians/civilian_00.set.
; 2022s NATO on a cwa71 map is an accepted era mismatch for a test-only probe.
;
; Entry form mirrors DOTD's parked-prototype preset (_presets/setup_infected.inc):
; bare entity + {Position} + {Player 0} + {MID} + {Able "-select"}, and NO
; {Inventory} block - 69 of its 91 parked entries carry none, so the breed's own
; loadout stands. That is the point of switching off the empty {Inventory {box
; {clear}}} form used by the breed-less pool.
;
; Parked at y -35100 like the defense pool, but x -3000..-2700 so the two pools
; never overlap. Tagged attack_mate_tpl, which nothing else in the mod reads.
	{Human "mp/nato/2022s/1ad_rifleman" 0xaf11
		{Position -3000 -35100}
		{Player 0}
		{MID 9011}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_rifleman" 0xaf12
		{Position -2900 -35100}
		{Player 0}
		{MID 9012}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_rifleman" 0xaf13
		{Position -2800 -35100}
		{Player 0}
		{MID 9013}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_rifleman" 0xaf14
		{Position -2700 -35100}
		{Player 0}
		{MID 9014}
		{Able "-select"}
	}
	{Tags "attack_mate_tpl" "hidden" 0xaf11}
	{Tags "attack_mate_tpl" "hidden" 0xaf12}
	{Tags "attack_mate_tpl" "hidden" 0xaf13}
	{Tags "attack_mate_tpl" "hidden" 0xaf14}
