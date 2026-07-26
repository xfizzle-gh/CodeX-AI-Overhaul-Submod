; Skirmish budget with doctrine registration.
{budgets
	{vehicle
		{mode "standard"}
		{resource
			{start		%s}
			{finish		%f}
			{growTime	2400}
			{growTable	"0.80 0.80 0.80 0.80 0.80 0.80 0.80 0.80 0.80 0.82 0.84 0.86 0.88 0.9 0.90 0.9 0.85 0.8 0.75 0.7"}
			{payback
				{cp			"0		80		95		110		125		200	"}
				{restore	"1.00	0.8		0.6		0.4		0.25	0.10"}
			}
		}
		{buttons "inf1 inf2 empl vehicles tanks"}
	}
	{hero
		{resource
			{start 20}
		}
		{buttons "doctrine"}
		{tag "special"}
	}
}
