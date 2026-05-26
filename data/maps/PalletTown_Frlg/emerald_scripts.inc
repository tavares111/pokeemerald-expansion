PalletTown_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, PalletTown_Frlg_OnTransition
	.byte 0

PalletTown_Frlg_OnTransition:
	setworldmapflag FLAG_WORLD_MAP_PALLET_TOWN
	end
