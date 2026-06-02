OldaleTown_EventScript_TravelToViridian::
	lock
	faceplayer
	msgbox HoennKantoTravel_Text_TravelToViridian, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, HoennKantoTravel_EventScript_DeclineTravel
	msgbox HoennKantoTravel_Text_SetSailForViridian, MSGBOX_DEFAULT
	closemessage
	warp MAP_VIRIDIAN_CITY, 30, 20
	waitstate
	releaseall
	end

ViridianCity_Frlg_EventScript_TravelToOldale::
	lock
	faceplayer
	msgbox HoennKantoTravel_Text_TravelToOldale, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, HoennKantoTravel_EventScript_DeclineTravel
	msgbox HoennKantoTravel_Text_SetSailForOldale, MSGBOX_DEFAULT
	closemessage
	warp MAP_OLDALE_TOWN, 10, 12
	waitstate
	releaseall
	end

HoennKantoTravel_EventScript_DeclineTravel::
	release
	end

HoennKantoTravel_Text_TravelToViridian:
	.string "SAILOR: I can take you to\n"
	.string "VIRIDIAN CITY in KANTO.\p"
	.string "Would you like to go?$"

HoennKantoTravel_Text_SetSailForViridian:
	.string "SAILOR: All aboard!\n"
	.string "Next stop, VIRIDIAN CITY!$"

HoennKantoTravel_Text_TravelToOldale:
	.string "SAILOR: Need a ride back to\n"
	.string "OLDALE TOWN in HOENN?\p"
	.string "Would you like to go?$"

HoennKantoTravel_Text_SetSailForOldale:
	.string "SAILOR: All aboard!\n"
	.string "Next stop, OLDALE TOWN!$"
