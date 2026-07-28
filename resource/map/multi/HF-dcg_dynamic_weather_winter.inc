			{"environment_dynamic/winter"
				{condition
				}
				{actions
					{"delay"
						{time 3}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.3}
							}
							{"set_i"
								{var "dynamic_weather_case$"}
								{op "="}
								{value 1}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.3}
							}
							{"set_i"
								{var "dynamic_weather_case$"}
								{op "="}
								{value 2}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.3}
							}
							{"set_i"
								{var "dynamic_weather_case$"}
								{op "="}
								{value 3}
							}
						}
						{"default"
							{"set_i"
								{var "dynamic_weather_case$"}
								{op "="}
								{value 4}
							}
						}
					}
					{"delay"
						{time 10}
					}
					{"event"
						{event "dynamic_weather_failsafe"}
						{mode set}
						{delay 0}
					}
				}
			}
			{"environment_dynamic/winter/case 1"
				{condition
					{terms
						{"1.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 1}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"switch" ;//morning
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/01_dawn"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/02_morning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/03_crispmorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/04_earlyhazymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/05_latehazymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/06_dustymorning"}
								{time 1}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/07_foggymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/08_palemorning"}
								{time 1}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/10_coldmorning"}
								{time 1}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//day	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_blizzard_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_1"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_2"}
								{time 60}
								{apply}
							}
						}			
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_3"}
								{time 60}
								{apply}
							}
						}						
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/25_palesnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/26_slush"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/27_mistysnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/28_lightsnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/29_crispsnow"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/30_brightsnow"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/31_windysnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/32_snowstorm"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/21_overcast"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/west/wip/winter/day_cloudy_1"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//evening
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/evening_1"}
								{time 60}
								{apply}
							}
						}		
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/west/wip/winter/evening_1"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 750}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 300}
							}
						}
						{"default"
							{"delay"
								{time 600}
							}
						}
					}
					{"switch" ;//night
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/35b_snowymoonlight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/36_snowynight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/night_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_1"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_snow_1"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/38_midnight"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 1200}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//morning
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/01_dawn"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/02_morning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/03_crispmorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/04_earlyhazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/05_latehazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/06_dustymorning"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/07_foggymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/08_palemorning"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/10_coldmorning"}
								{time 60}
								{apply}
							}
						}
					}
				}
			}
			{"environment_dynamic/winter/case 2"
				{condition
					{terms
						{"1.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 2}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"switch" ;//day
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_blizzard_2"}
								{time 1}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_1"}
								{time 1}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_2"}
								{time 1}
								{apply}
							}
						}			
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_3"}
								{time 1}
								{apply}
							}
						}						
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/25_palesnow"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/26_slush"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/27_mistysnow"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/28_lightsnow"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/29_crispsnow"}
								{time 1}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/30_brightsnow"}
								{time 1}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/31_windysnow"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/32_snowstorm"}
								{time 1}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/21_overcast"}
								{time 1}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/west/wip/winter/day_cloudy_1"}
								{time 1}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//evening
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/evening_1"}
								{time 60}
								{apply}
							}
						}		
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/west/wip/winter/evening_1"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 750}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 300}
							}
						}
						{"default"
							{"delay"
								{time 600}
							}
						}
					}
					{"switch" ;//night
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/35b_snowymoonlight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/36_snowynight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/night_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_1"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_snow_1"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/38_midnight"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 1200}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//morning
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/01_dawn"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/02_morning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/03_crispmorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/04_earlyhazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/05_latehazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/06_dustymorning"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/07_foggymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/08_palemorning"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/10_coldmorning"}
								{time 60}
								{apply}
							}
						}
					}
				}
			}
			{"environment_dynamic/winter/case 3"
				{condition
					{terms
						{"1.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 3}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"switch" ;//evening
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/evening_1"}
								{time 1}
								{apply}
							}
						}		
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/west/wip/winter/evening_1"}
								{time 1}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 750}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 300}
							}
						}
						{"default"
							{"delay"
								{time 600}
							}
						}
					}
					{"switch" ;//night
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/35b_snowymoonlight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/36_snowynight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/night_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_1"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_snow_1"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/38_midnight"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 1200}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//morning
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/01_dawn"}
								{time 0.25}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/02_morning"}
								{time 0.25}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/03_crispmorning"}
								{time 0.25}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/04_earlyhazymorning"}
								{time 0.25}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/05_latehazymorning"}
								{time 0.25}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/06_dustymorning"}
								{time 0.25}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/07_foggymorning"}
								{time 0.25}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/08_palemorning"}
								{time 0.25}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/10_coldmorning"}
								{time 0.25}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//day	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_blizzard_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_1"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_2"}
								{time 60}
								{apply}
							}
						}			
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_3"}
								{time 60}
								{apply}
							}
						}						
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/25_palesnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/26_slush"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/27_mistysnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/28_lightsnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/29_crispsnow"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/30_brightsnow"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/31_windysnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/32_snowstorm"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/21_overcast"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/west/wip/winter/day_cloudy_1"}
								{time 60}
								{apply}
							}
						}
					}
				}
			}
			{"environment_dynamic/winter/case 4"
				{condition
					{terms
						{"1.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 4}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"switch" ;//night
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/35b_snowymoonlight"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/36_snowynight"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/night_1"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/night_2"}
								{time 1}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_1"}
								{time 1}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_2"}
								{time 1}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_snow_1"}
								{time 1}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/38_midnight"}
								{time 1}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 1200}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//morning
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/01_dawn"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/02_morning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/03_crispmorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/04_earlyhazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/05_latehazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/06_dustymorning"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/07_foggymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/08_palemorning"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/10_coldmorning"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//day	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_blizzard_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_1"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_2"}
								{time 60}
								{apply}
							}
						}			
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_3"}
								{time 60}
								{apply}
							}
						}						
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/25_palesnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/26_slush"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/27_mistysnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/28_lightsnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/29_crispsnow"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/30_brightsnow"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/31_windysnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/32_snowstorm"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/21_overcast"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/west/wip/winter/day_cloudy_1"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//evening
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/evening_1"}
								{time 60}
								{apply}
							}
						}		
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/west/wip/winter/evening_1"}
								{time 60}
								{apply}
							}
						}
					}
				}
			}
			
			{"environment_dynamic/summer/case failsafe"
				{condition
					{expression "!1 & !2 & !3 & !4 & 5"}
					{terms
						{"1.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 1}
						}
						{"2.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 2}
						}
						{"3.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 3}
						}
						{"4.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 4}
						}
						{"5.event"
							{id "dynamic_weather_failsafe"}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"switch" ;//morning
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/01_dawn"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/02_morning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/03_crispmorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/04_earlyhazymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/05_latehazymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/06_dustymorning"}
								{time 1}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/07_foggymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/08_palemorning"}
								{time 1}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/10_coldmorning"}
								{time 1}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//day	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_blizzard_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_1"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_2"}
								{time 60}
								{apply}
							}
						}			
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/day_snow_3"}
								{time 60}
								{apply}
							}
						}						
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/25_palesnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/26_slush"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/27_mistysnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/28_lightsnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/29_crispsnow"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/30_brightsnow"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/31_windysnow"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/32_snowstorm"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/21_overcast"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/west/wip/winter/day_cloudy_1"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//evening
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/evening_1"}
								{time 60}
								{apply}
							}
						}		
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/west/wip/winter/evening_1"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 750}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 300}
							}
						}
						{"default"
							{"delay"
								{time 600}
							}
						}
					}
					{"switch" ;//night
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/35b_snowymoonlight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/36_snowynight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/winter/night_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_1"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/multi/west/winter/night_snow_1"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/38_midnight"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 1200}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//morning
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/01_dawn"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/02_morning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/03_crispmorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/04_earlyhazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/05_latehazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/06_dustymorning"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/07_foggymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/08_palemorning"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/10_coldmorning"}
								{time 60}
								{apply}
							}
						}
					}
				}
			}