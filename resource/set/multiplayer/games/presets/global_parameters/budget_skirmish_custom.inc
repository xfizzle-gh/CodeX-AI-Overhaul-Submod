; Skirmish custom budget with doctrine registration.
{budgets
	{vehicle
		{mode "custom"}
		{resource
			{start		%s}
			{finish		%f}
			{growTime	2400}
			{growTable	"0.80 0.80 0.80 0.80 0.80 0.80 0.80 0.80 0.80 0.82 0.84 0.86 0.88 0.9 0.90 0.9 0.85 0.8 0.75 0.7"}
			{payback
				{cp			"0		80		95		110		125		200	"}
				{restore	"1.00	0.8		0.6		0.4		0.25	0.10"}
			}
			{limitmin	1000}
			{limitmax	50000}
			{speedset	"2000 2500 3000 3500 4000 4500 5000 6000 10000"}
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
