# How to add marts for imported FRLG maps

This guide explains how to restore shop behavior on raw imported FRLG maps without importing the original FRLG story scripts.

Use this after the map already builds in Emerald with `include_in_emerald: true`.

## Minimal Mart pattern

For a raw import, restore only the clerk and the item list:

- Add one clerk object to the shop map.
- Create an Emerald-only `emerald_scripts.inc`.
- Copy the original FRLG item list, but not the original story scenes.
- Include the new script from `data/event_scripts.s`.
- Remove `"shared_scripts_map": "VictoryRoad_B1F"` from that shop map.

Example clerk object:

```json
{
  "local_id": "LOCALID_PEWTER_CITY_MART_CLERK",
  "type": "object",
  "graphics_id": "OBJ_EVENT_GFX_MART_EMPLOYEE",
  "x": 1,
  "y": 3,
  "elevation": 3,
  "movement_type": "MOVEMENT_TYPE_FACE_RIGHT",
  "movement_range_x": 0,
  "movement_range_y": 0,
  "trainer_type": "TRAINER_TYPE_NONE",
  "trainer_sight_or_berry_tree_id": "0",
  "script": "PewterCity_Mart_Frlg_EventScript_Clerk",
  "flag": "0"
}
```

Example script:

```asm
PewterCity_Mart_Frlg_MapScripts::
    .byte 0

PewterCity_Mart_Frlg_EventScript_Clerk::
    lock
    faceplayer
    message gText_HowMayIServeYou
    waitmessage
    pokemart PewterCity_Mart_Items
    msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
    release
    end

    .align 2
PewterCity_Mart_Items:
    .2byte ITEM_POKE_BALL
    .2byte ITEM_POTION
    .2byte ITEM_ANTIDOTE
    .2byte ITEM_PARALYZE_HEAL
    .2byte ITEM_AWAKENING
    .2byte ITEM_BURN_HEAL
    .2byte ITEM_ESCAPE_ROPE
    .2byte ITEM_REPEL
    pokemartlistend
```

Use `pokemartlistend` at the end of the list. Do not copy the original FRLG `release` and `end` lines that sometimes appear after `ITEM_NONE`; those are script commands, not item-list entries.

## What to copy from FRLG

The original FRLG `scripts.inc` files are useful as references for item lists.

Copy:

- The `pokemart <ItemListLabel>` target.
- The `.2byte ITEM_*` entries up to `ITEM_NONE`.

Do not copy:

- Story scenes, such as Oak's Parcel in `ViridianCity_Mart_Frlg`.
- NPC dialogue that is not needed for shopping.
- FRLG-only helper scripts or variables unless you intentionally port that feature.

## Special cases

Some FRLG shops are not plain `*_Mart_Frlg` maps:

- `IndigoPlateau_PokemonCenter_1F_Frlg` has both a nurse and a shop clerk in the same map.
- `CeladonCity_DepartmentStore_2F_Frlg` has two shop lists, one for items and one for TMs.
- `CeladonCity_DepartmentStore_4F_Frlg` has a shop list for dolls, mail, and evolution stones.
- `TrainerTower_Lobby_Frlg` has a shop list in the lobby.
- `TwoIsland_Frlg` has an outdoor shop with multiple original progression-based lists.

For raw Emerald imports, start with one always-available list. For Two Island, the simplest option is the original initial list. You can later add variables or flags if you want the shop inventory to expand over time.

## Build and test

Run:

```sh
make -j 16
```

Then test in-game:

1. Enter the shop map.
2. Talk to the clerk.
3. Confirm the item list opens.
4. Buy an item and confirm the game returns to the clerk cleanly.

If the build fails with an undefined `*_MapScripts` label, the map probably no longer has `shared_scripts_map` but its `emerald_scripts.inc` was not included from `data/event_scripts.s`.

If the shop opens but the clerk is unreachable, adjust the clerk `x`, `y`, `elevation`, or facing direction in `map.json`.
