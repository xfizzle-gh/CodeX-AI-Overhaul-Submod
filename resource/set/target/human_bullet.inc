{selector
	{unit "human" "bullet"}

	{group
		{ignore {class proj_rus_antirad}}
		{ignore {class proj_usa_antirad}}
		{sort {threating}{in_range 35}}
		{sort {attacking}{in_range 35}}
		{sort distance}
	}
	{group
		{ignore {class proj_rus_antirad}}
		{ignore {class proj_usa_antirad}}
		{sort {class airborne}}
		{sort {class human} {attacking}}
		{sort {in_range 40} {class tank_heavy}}
		{sort {in_range 40} {class tank_medium}}
		{sort {in_range 40} {class armoredcar}}
		{sort {in_range 40} {class armoredcar_l}}
		{sort {in_range 40} {class car}}
		{sort {in_range 40} {class tank_light}}
		{sort {in_range 40} {class cannon_small}}
		{sort {in_range 40} {class cannon_big}}
		{sort {in_range 40} {class cannon_zenite}}
		{sort {in_range 40} {class cannon_bullet}}
		{sort {in_range 40} {class howitzer}}
		{sort {in_range 50} {threating} {class tank_heavy}}
		{sort {in_range 50} {threating} {class tank_medium}}
		{sort {in_range 50} {threating} {class armoredcar}}
		{sort {in_range 50} {threating} {class armoredcar_l}}
		{sort {in_range 50} {threating} {class car}}
		{sort {in_range 50} {threating} {class tank_light}}
		{sort {in_range 50} {threating} {class cannon_small}}
		{sort {in_range 50} {threating} {class cannon_big}}
		{sort {in_range 50} {threating} {class cannon_zenite}}
		{sort {in_range 50} {threating} {class cannon_bullet}}
		{sort {in_range 50} {threating} {class howitzer}}
		{sort {class human}}
		{sort {class tank_heavy}}
		{sort {class tank_medium}}
		{sort {class armoredcar}}
		{sort {class armoredcar_l}}
		{sort {class car}}
		{sort {class tank_light}}
		{sort {class cannon_small}}
		{sort {class cannon_big}}
		{sort {class cannon_zenite}}
		{sort {class cannon_bullet}}
		{sort {class howitzer}}
		{sort distance}
		{sort {in_range 110}}
		{ignore {class human} {weapon "proj_pointer"}}
	}
}
