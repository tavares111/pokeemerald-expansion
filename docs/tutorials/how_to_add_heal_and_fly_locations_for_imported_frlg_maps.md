# How to add heal and Fly locations for imported FRLG maps

This guide explains how to turn a raw imported FRLG map into a working Emerald heal and Fly destination without importing the original FRLG story scripts or NPC events.

Use this after the raw map already builds with:

- `include_in_emerald: true`
- `region: "REGION_KANTO"`
- working warps and connections
- empty or custom `object_events`, `coord_events`, and `bg_events`

## Heal locations vs Fly locations

Heal locations and Fly locations share some data, but they are unlocked differently.

- `setrespawn HEAL_LOCATION_*` updates the player's last heal point after using a Pokémon Center nurse.
- `src/data/heal_locations.json` defines the heal point, whiteout target, and optional nurse NPC used after whiteout.
- `src/region_map.c` maps a `MAPSEC_*` to the heal location that Fly should use.
- `setworldmapflag FLAG_WORLD_MAP_*` marks a Kanto map section as visited so the Fly map can select it.
- `include/constants/flags.h` must give imported Kanto world-map flags real Emerald flag values. If a flag is `0`, it cannot track a visited location correctly.

For example, Viridian City uses:

- `MAPSEC_VIRIDIAN_CITY`
- `HEAL_LOCATION_VIRIDIAN_CITY`
- `FLAG_WORLD_MAP_VIRIDIAN_CITY`

## 1. Add or verify the heal location

Heal locations live in `src/data/heal_locations.json`.

For Viridian City, the entry should point the heal location at the outdoor city map and the whiteout respawn at the Pokémon Center:

```json
{
  "id": "HEAL_LOCATION_VIRIDIAN_CITY",
  "map": "MAP_VIRIDIAN_CITY",
  "x": 27,
  "y": 27,
  "respawn_map": "MAP_VIRIDIAN_CITY_POKEMON_CENTER_1F",
  "respawn_npc": "LOCALID_VIRIDIAN_NURSE"
}
```

The `map`, `x`, and `y` fields are where Fly and normal heal warps place the player. The `respawn_map` and `respawn_npc` fields are used when the player whites out and should appear in front of the nurse.

If the Pokémon Center does not have a nurse object yet, use `LOCALID_NONE` temporarily for `respawn_npc`. Once you add the nurse, restore the real local ID.

## 2. Add the nurse object

In the Pokémon Center's `map.json`, add a nurse object with a stable `LOCALID_*`.

Example from `data/maps/ViridianCity_PokemonCenter_1F_Frlg/map.json`:

```json
{
  "local_id": "LOCALID_VIRIDIAN_NURSE",
  "type": "object",
  "graphics_id": "OBJ_EVENT_GFX_NURSE",
  "x": 7,
  "y": 2,
  "elevation": 3,
  "movement_type": "MOVEMENT_TYPE_FACE_DOWN",
  "movement_range_x": 0,
  "movement_range_y": 0,
  "trainer_type": "TRAINER_TYPE_NONE",
  "trainer_sight_or_berry_tree_id": "0",
  "script": "ViridianCity_PokemonCenter_1F_Frlg_EventScript_Nurse",
  "flag": "0"
}
```

The `local_id` must match the `respawn_npc` in `src/data/heal_locations.json`.

## 3. Add a minimal Pokémon Center script

For a raw import, do not include the full FRLG Pokémon Center script. Add a small Emerald-compatible script instead.

Example: `data/maps/ViridianCity_PokemonCenter_1F_Frlg/emerald_scripts.inc`

```asm
ViridianCity_PokemonCenter_1F_Frlg_MapScripts::
    map_script MAP_SCRIPT_ON_TRANSITION, ViridianCity_PokemonCenter_1F_Frlg_OnTransition
    map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
    .byte 0

ViridianCity_PokemonCenter_1F_Frlg_OnTransition:
    setrespawn HEAL_LOCATION_VIRIDIAN_CITY
    end

ViridianCity_PokemonCenter_1F_Frlg_EventScript_Nurse::
    setvar VAR_0x800B, LOCALID_VIRIDIAN_NURSE
    call Common_EventScript_PkmnCenterNurse
    waitmessage
    waitbuttonpress
    release
    end
```

Then include it from `data/event_scripts.s` with an Emerald-only guard:

```asm
.if !IS_FRLG && EM_INCLUDE_RAW_KANTO_FRLG
    .include "data/maps/ViridianCity_PokemonCenter_1F_Frlg/emerald_scripts.inc"
.endif
```

Because this Pokémon Center now has its own `MapScripts` label, remove `"shared_scripts_map": "VictoryRoad_B1F"` from that Pokémon Center's `map.json` if it is present.

## 4. Verify Fly has a heal destination

Fly uses `src/region_map.c` to convert a map section into a destination.

For Viridian City, `sMapHealLocations` should include:

```c
[MAPSEC_VIRIDIAN_CITY] = {MAP_GROUP(MAP_VIRIDIAN_CITY), MAP_NUM(MAP_VIRIDIAN_CITY), HEAL_LOCATION_VIRIDIAN_CITY},
```

With this entry, selecting Viridian City on the Fly map will use `HEAL_LOCATION_VIRIDIAN_CITY`.

## 5. Give the Kanto world-map flag a real Emerald value

The Fly map checks whether the city has been visited.

For Viridian City, `src/region_map.c` contains:

```c
case MAPSEC_VIRIDIAN_CITY:
    return FlagGet(FLAG_WORLD_MAP_VIRIDIAN_CITY) ? MAPSECTYPE_CITY_CANFLY : MAPSECTYPE_CITY_CANTFLY;
```

That only works if `FLAG_WORLD_MAP_VIRIDIAN_CITY` is a real flag in `include/constants/flags.h`.

In this project, many FRLG world-map flags are currently placeholders in the Emerald flag file:

```c
#define FLAG_WORLD_MAP_VIRIDIAN_CITY 0
```

Before using the flag in Emerald, assign it to an unused Emerald flag slot. Do this carefully so it does not overlap existing flags or trainer flags.

## 6. Add a minimal outdoor Fly unlock script

The outdoor city map must set the world-map flag when the player enters the city.

For Viridian City, create an Emerald-only script such as `data/maps/ViridianCity_Frlg/emerald_scripts.inc`:

```asm
ViridianCity_Frlg_MapScripts::
    map_script MAP_SCRIPT_ON_TRANSITION, ViridianCity_Frlg_OnTransition
    .byte 0

ViridianCity_Frlg_OnTransition:
    setworldmapflag FLAG_WORLD_MAP_VIRIDIAN_CITY
    end
```

Then include it from `data/event_scripts.s`:

```asm
.if !IS_FRLG && EM_INCLUDE_RAW_KANTO_FRLG
    .include "data/maps/ViridianCity_Frlg/emerald_scripts.inc"
.endif
```

Because the outdoor map now has its own `MapScripts` label, remove `"shared_scripts_map": "VictoryRoad_B1F"` from `data/maps/ViridianCity_Frlg/map.json`.

## 7. Build and test

Run a full build:

```sh
make -j 16
```

Then test in-game:

1. Enter `ViridianCity_Frlg` at least once so the transition script runs.
2. Use the Pokémon Center nurse so `setrespawn HEAL_LOCATION_VIRIDIAN_CITY` runs.
3. Open Fly from an outdoor map that allows Fly.
4. Confirm Viridian City is selectable and sends the player to the Viridian heal location.

If you are testing with an old save, the Fly flag may not already be set. Enter Viridian City again after rebuilding, or use a debug script to set `FLAG_WORLD_MAP_VIRIDIAN_CITY`.

## Restoring all imported FRLG Centers

When restoring many Centers, repeat the same minimal pattern for each `*_PokemonCenter_1F_Frlg` map:

1. Add one nurse object to the Center's `map.json`.
2. Give that object a unique `LOCALID_*_NURSE`.
3. Point the object script at `<MapName>_EventScript_Nurse`.
4. Remove `"shared_scripts_map": "VictoryRoad_B1F"` from that Center map.
5. Add an Emerald-only `emerald_scripts.inc` with `setrespawn HEAL_LOCATION_*` and the common nurse call.
6. Include that script from `data/event_scripts.s` under `!IS_FRLG && EM_INCLUDE_RAW_KANTO_FRLG`.
7. Update `src/data/heal_locations.json` so `respawn_npc` matches the nurse object's local ID.

The `respawn_npc` local ID must exist on the `respawn_map`. If it does not, the generated heal-location data can compile incorrectly or whiteout can fail to target the nurse.

Use unique local ID names. For example, if Hoenn already uses `LOCALID_LEAGUE_NURSE`, use a separate imported-map name such as `LOCALID_INDIGO_PLATEAU_NURSE`.

## Troubleshooting

If the Pokémon Center heals but whiteout fails, check that `respawn_npc` matches a real `LOCALID_*` object on the `respawn_map`.

If Fly opens but Viridian City is not selectable, check that `FLAG_WORLD_MAP_VIRIDIAN_CITY` is nonzero in `include/constants/flags.h` and that the outdoor transition script runs.

If Viridian City is selectable but warps to the wrong place, check the `MAPSEC_VIRIDIAN_CITY` entry in `src/region_map.c` and the coordinates in `HEAL_LOCATION_VIRIDIAN_CITY`.

If the build fails with an undefined `*_MapScripts` symbol, the map probably no longer has `shared_scripts_map` but its Emerald script was not included from `data/event_scripts.s`.
