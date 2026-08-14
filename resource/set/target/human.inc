{selector
	{unit "human"}

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
		{sort {class helicopter}}
		{sort {class human} {weapon "bullet"} {in_range 250}}
		{sort {class human} {weapon} {in_range 200}}
		{sort {class human}}
		{sort {class tank_heavy} {in_range 120}}
		{sort {class tank_medium} {in_range 120}}
		{sort {class tank_light} {in_range 120}}
		{sort {class armoredcar} {in_range 120}}
		{sort {class armoredcar_l} {in_range 120}}
		{sort {class car} {in_range 200}}
		{sort {class cannon_small}}
		{sort {class cannon_big}}
		{sort {class cannon_zenite}}
		{sort {class cannon_bullet}}
		{sort {class howitzer}}
		{sort distance}
	}
}
