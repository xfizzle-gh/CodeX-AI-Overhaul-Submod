{Modifiers
	{modifier
		{name aio_morale_shaken}
		{filter
			{include
				{tag
					{tag aio_morale_shaken}
				}
			}
			{exclude
				{state
					{state dead}
				}
				{state
					{state inactive}
				}
			}
		}
		{parameters
			{accuracy
				{place "*"}
				{scale 0.75}
			}
		}
	}
	{modifier
		{name aio_morale_panic}
		{filter
			{include
				{tag
					{tag aio_morale_panic}
				}
			}
			{exclude
				{state
					{state dead}
				}
				{state
					{state inactive}
				}
			}
		}
		{parameters
			{accuracy
				{place "*"}
				{scale 0.5}
			}
			{aim_range
				{place "*"}
				{scale 0.8}
			}
		}
	}
	{modifier
		{name aio_morale_broken}
		{filter
			{include
				{tag
					{tag aio_morale_broken}
				}
			}
			{exclude
				{state
					{state dead}
				}
				{state
					{state inactive}
				}
			}
		}
		{parameters
			{accuracy
				{place "*"}
				{scale 0.35}
			}
			{aim_range
				{place "*"}
				{scale 0.65}
			}
		}
	}
}
