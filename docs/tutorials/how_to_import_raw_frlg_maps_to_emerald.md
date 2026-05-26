# How to import raw FRLG maps into Emerald

This guide is for importing FRLG map geometry into the Emerald build without importing FRLG scripts, NPCs, signs, or story events. Use this when you want the map layout and tiles to compile first, then build your own events later.

## Goal

A raw imported map should:

- Use the FRLG `map.bin`, `border.bin`, and tilesets.
- Be included in the Emerald map tables.
- Avoid referencing FRLG script labels.
- Have empty `object_events`, `coord_events`, and `bg_events` until you add your own.

## 1. Mark the map as Emerald-included

Edit the map JSON in `data/maps/<MapName>_Frlg/map.json`.

Add:

```json
"shared_scripts_map": "VictoryRoad_B1F",
"include_in_emerald": true,
"region": "REGION_KANTO"
```

Example:

```json
{
  "id": "MAP_ROUTE1",
  "name": "Route1_Frlg",
  "layout": "LAYOUT_ROUTE1",
  "shared_scripts_map": "VictoryRoad_B1F",
  "music": "MUS_RG_ROUTE1",
  ...
  "include_in_emerald": true,
  "region": "REGION_KANTO"
}
```

`include_in_emerald` tells `mapjson` to keep this Kanto map in an Emerald build. `shared_scripts_map` prevents the generated map header from referencing `<MapName>_Frlg_MapScripts`, so you do not need to include the FRLG script file.

## 2. Remove script-referencing events

For a raw import, keep these arrays empty:

```json
"object_events": [],
"coord_events": [],
"bg_events": []
```

You can keep `warp_events` and `connections` if you want the map to connect to other imported maps. If a warp points to a map that is not included in Emerald yet, either import that destination too or remove the warp for now.

## 3. Mark the layout as Emerald-included

Find the matching layout in `data/layouts/layouts.json`.

Add:

```json
"include_in_emerald": true
```

Keep:

```json
"layout_version": "frlg"
```

Example:

```json
{
  "id": "LAYOUT_ROUTE1",
  "name": "Route1_Layout",
  "width": 24,
  "height": 40,
  "border_width": 2,
  "border_height": 2,
  "primary_tileset": "gTileset_General_Frlg",
  "secondary_tileset": "gTileset_PalletTown",
  "border_filepath": "data/layouts/Route1_Frlg/border.bin",
  "blockdata_filepath": "data/layouts/Route1_Frlg/map.bin",
  "include_in_emerald": true,
  "layout_version": "frlg"
}
```

Keeping `layout_version: "frlg"` is important because the engine uses it to read FRLG layout dimensions, metatile counts, and palette counts correctly.

## 4. Make sure the tilesets are available

The Emerald build normally skips FRLG-only tileset structs. This branch uses `EM_INCLUDE_RAW_KANTO_FRLG` in `include/config/general.h` to expose the required FRLG tilesets for raw imports.

For one or two maps, expose only the tilesets used by those layouts. For a full FRLG bulk import, derive the list from every `layout_version: "frlg"` entry in `data/layouts/layouts.json`, then expose each referenced `primary_tileset` and `secondary_tileset` in:

- `src/data/tilesets/metatiles.h`
- `src/data/tilesets/headers.h`
- `include/tilesets.h`

The graphics data in `src/data/tilesets/graphics.h` is already globally available in this project.

## 5. Do not include the FRLG script file

Do not move the map's `scripts.inc` include out of the `.if IS_FRLG` block in `data/event_scripts.s`.

For raw imports, the map should use:

```json
"shared_scripts_map": "VictoryRoad_B1F"
```

That gives the map a valid empty `MapScripts` pointer while you build your own scripts later.

## 6. Resolve connection problems

If two imported maps do not connect when you walk across the edge, check both map JSON files.

The lower map should point up:

```json
"connections": [
  {
    "map": "MAP_ROUTE1",
    "offset": 0,
    "direction": "up"
  }
]
```

The upper map should point down:

```json
"connections": [
  {
    "map": "MAP_PALLET_TOWN",
    "offset": 0,
    "direction": "down"
  }
]
```

After editing connections, rebuild or regenerate the map data. The generated files should include matching entries like:

```asm
connection up, 0, MAP_ROUTE1
connection down, 0, MAP_PALLET_TOWN
```

If the generated connections look correct but the game still does not move to the expected map, check the map group table in `data/maps/groups.inc`. FRLG map IDs keep their original indexes from `include/constants/map_groups.h`. For example, `MAP_ROUTE1` is slot 19 in `gMapGroup_TownsAndRoutes_Frlg`, so the generated group must keep `NULL` placeholders before `Route1_Frlg`:

```asm
gMapGroup_TownsAndRoutes_Frlg::
    .4byte PalletTown_Frlg
    .4byte NULL
    ...
    .4byte Route1_Frlg
```

If `Route1_Frlg` appears immediately after `PalletTown_Frlg`, the group was compacted and the connection will point at the wrong slot. Regenerate the groups after applying the `mapjson` fix that preserves skipped map slots with `NULL`.

## 7. Import FRLG wild encounters

Wild encounters are defined in `src/data/wild_encounters.json`. The generated file `src/data/wild_encounters.h` is rebuilt from that JSON.

FRLG encounter data is already in the same JSON file. For Route 1, look for:

- `sRoute1_FireRed`
- `sRoute1_LeafGreen`

Those entries use `"map": "MAP_ROUTE1"`, but their labels contain `FireRed` or `LeafGreen`, so the generator wraps them in `#ifdef FIRERED` or `#ifdef LEAFGREEN`. To use the data in Emerald, copy one of those entries and give it an Emerald-neutral `base_label`.

Example for the imported `Route1_Frlg`:

```json
{
  "map": "MAP_ROUTE1",
  "base_label": "gRoute1Frlg",
  "land_mons": {
    "encounter_rate": 21,
    "mons": [
      { "min_level": 3, "max_level": 3, "species": "SPECIES_PIDGEY" },
      { "min_level": 3, "max_level": 3, "species": "SPECIES_RATTATA" },
      { "min_level": 3, "max_level": 3, "species": "SPECIES_PIDGEY" },
      { "min_level": 3, "max_level": 3, "species": "SPECIES_RATTATA" },
      { "min_level": 2, "max_level": 2, "species": "SPECIES_PIDGEY" },
      { "min_level": 2, "max_level": 2, "species": "SPECIES_RATTATA" },
      { "min_level": 3, "max_level": 3, "species": "SPECIES_PIDGEY" },
      { "min_level": 3, "max_level": 3, "species": "SPECIES_RATTATA" },
      { "min_level": 4, "max_level": 4, "species": "SPECIES_PIDGEY" },
      { "min_level": 4, "max_level": 4, "species": "SPECIES_RATTATA" },
      { "min_level": 5, "max_level": 5, "species": "SPECIES_PIDGEY" },
      { "min_level": 4, "max_level": 4, "species": "SPECIES_RATTATA" }
    ]
  }
}
```

Use `MAP_ROUTE1`, not `Route1_Frlg`, because encounters are matched by map constant. For land encounters, keep exactly 12 entries.

After editing the JSON, rebuild:

```sh
make -j 16
```

The build will regenerate `src/data/wild_encounters.h`. You can also regenerate only that file with:

```sh
python3 tools/wild_encounters/wild_encounters_to_header.py
```

If the encounter table compiles but no wild battles happen in-game, check the imported grass metatiles. The grass tiles must have a wild-grass metatile behavior, otherwise the map has encounter data but no tile behavior that triggers land encounters.

## 8. Add heal and Fly support later

Raw imports intentionally avoid FRLG scripts, so Pokémon Centers and Fly destinations will not fully work until you add small Emerald-compatible scripts.

Heal locations and Fly locations are related, but they are not the same system:

- A heal location controls where `setrespawn`, whiteout, and Fly should place the player.
- A Fly location also needs a world-map flag so the region map marks the city as visited and selectable.

For a Pokémon Center, add your own nurse object and a minimal script that calls `setrespawn HEAL_LOCATION_*`. For a Fly destination, add a minimal outdoor map transition script that calls `setworldmapflag FLAG_WORLD_MAP_*`.

See `docs/tutorials/how_to_add_heal_and_fly_locations_for_imported_frlg_maps.md` for a full Viridian City example.

## 9. Add marts later

Raw imports also remove Mart clerks. To restore shopping without importing FRLG story events, add one clerk object and a small Emerald-only script that uses `pokemart` with the original FRLG item list.

Do not include the full original FRLG Mart script if it contains story logic. For example, `ViridianCity_Mart_Frlg` has Oak's Parcel logic in its original script; for a raw Emerald import, use a simple clerk script that sells items immediately.

See `docs/tutorials/how_to_add_marts_for_imported_frlg_maps.md` for the minimal Mart pattern.

## 10. Bulk-import all FRLG maps

For importing every FRLG map at once, use the same rules as the single-map steps, but apply them mechanically:

- For every `data/maps/*_Frlg/map.json`, set `include_in_emerald: true`, `region: "REGION_KANTO"`, and `shared_scripts_map: "VictoryRoad_B1F"`.
- Empty `object_events`, `coord_events`, and `bg_events`.
- Keep `warp_events` and `connections`.
- In `data/layouts/layouts.json`, set `include_in_emerald: true` for every layout with `layout_version: "frlg"`.
- Expose every FRLG tileset referenced by those layouts under `EM_INCLUDE_RAW_KANTO_FRLG`.
- Duplicate FireRed wild encounter entries with Emerald-neutral `base_label` values. Leave LeafGreen entries unchanged unless you specifically want LeafGreen encounter tables.
- If generated heal locations reference removed FRLG nurse NPCs, set those raw-import `respawn_npc` values in `src/data/heal_locations.json` to `LOCALID_NONE`.

Preserve any manual connection edits you made before the bulk import. For example, if you removed an upper connection from `Route1_Frlg`, skip `data/maps/Route1_Frlg/map.json` in the bulk script so that change is not overwritten.

After bulk JSON edits, regenerate the derived data from WSL:

```sh
tools/mapjson/mapjson groups emerald data/maps/map_groups.json data/maps/*/map.json data/maps include/constants
python3 tools/wild_encounters/wild_encounters_to_header.py
```

Then run a full build.

## 11. Build

From WSL:

```sh
make -j 16
```

If the build fails with an undefined `gTileset_*` symbol, the map's layout uses a tileset that has not been exposed to Emerald yet. Add that tileset to the guarded raw Kanto tileset block, then rebuild.

If the build fails with an undefined script or `LOCALID_*` symbol, the map still has an event reference. Remove the event for now or replace it with your own Emerald script/event.
