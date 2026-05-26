TwoIsland_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, TwoIsland_Frlg_OnTransition
	.byte 0

TwoIsland_Frlg_EventScript_ShopClerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart TwoIsland_Items_ShopInitial
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
TwoIsland_Items_ShopInitial:
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_FRESH_WATER
	pokemartlistend
TwoIsland_Frlg_OnTransition:
	setworldmapflag FLAG_WORLD_MAP_TWO_ISLAND
	end
