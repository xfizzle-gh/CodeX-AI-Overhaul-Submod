(define "team_a"
	{unitset {value "2022s"}}
	{commandPoints "*=%cp_a"}
	(include "../../global_parameters/unitlimits.inc")
	(include "budget_attacker_custom.inc" s(%start_a) f(%finish_a) r(%reinforcement_a))
)
(define "team_b"
	{unitset {value "2022s"}}
	{commandPoints "*=%cp_b"}
	(include "../../global_parameters/unitlimits.inc")
	(include "budget_defender_custom.inc" s(%start_b) f(%finish_b))
)
(include "bots_frontlines.inc")
