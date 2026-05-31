# DexNav

The DexNav lets the player register a wild species from the current map, search for it from the start menu, and find hidden Pokemon while walking. Its configuration lives in [`include/config/dexnav.h`](../../include/config/dexnav.h), and the search implementation lives in [`src/dexnav.c`](../../src/dexnav.c).

## Basic Configuration

Enable the feature with:

```c
#define DEXNAV_ENABLED TRUE
```

When `DEXNAV_ENABLED` is `TRUE`, the following flags and vars must be non-zero:

```c
#define DN_FLAG_SEARCHING       FLAG_UNUSED_0x8E5
#define DN_FLAG_DEXNAV_GET      FLAG_UNUSED_0x8E6
#define DN_FLAG_DETECTOR_MODE   FLAG_UNUSED_0x8E7
#define DN_VAR_SPECIES          VAR_UNUSED_0x40FE
#define DN_VAR_STEP_COUNTER     VAR_UNUSED_0x40FF
```

These values can be changed to any unused persistent flags and vars in your project. Do not use temp flags or temp vars here, because DexNav state needs to survive map transitions and normal gameplay flow.

The meanings are:

- `DN_FLAG_SEARCHING`: set while an active DexNav search is in progress.
- `DN_FLAG_DEXNAV_GET`: unlocks the DexNav start-menu option.
- `DN_FLAG_DETECTOR_MODE`: allows hidden Pokemon and detector-mode icons.
- `DN_VAR_SPECIES`: stores the currently registered DexNav species and encounter environment.
- `DN_VAR_STEP_COUNTER`: counts steps toward hidden Pokemon checks.

If the build fails with a negative-size static assert such as `DNFlagSearching_Must_Not_Be_Zero`, one of the required flags or vars is still set to `0`.

## Unlocking DexNav

The start menu only shows DexNav when `DN_FLAG_DEXNAV_GET` is set. Hidden Pokemon detector mode also requires `DN_FLAG_DETECTOR_MODE`.

For example, to unlock both from a Professor Birch aide after the player already has the Pokedex:

```asm
goto_if_unset FLAG_SYS_POKEDEX_GET, SomeOtherAideText
setflag DN_FLAG_DEXNAV_GET
setflag DN_FLAG_DETECTOR_MODE
```

This can be useful for hacks that enable DexNav after players may already have started a save file. You can instead set `DN_FLAG_DETECTOR_MODE` later if you want detector mode to be a separate upgrade.

## Search Levels

Search levels are controlled by:

```c
#define USE_DEXNAV_SEARCH_LEVELS TRUE
```

When enabled, DexNav stores one byte per species in `SaveBlock3`:

```c
u8 dexNavSearchLevels[NUM_SPECIES];
```

This is required for hidden ability odds to increase above search level 0. It also changes save compatibility and may require updating the expected `SaveBlock3` size in [`test/save.c`](../../test/save.c). Make sure `sizeof(struct SaveBlock3)` still fits within the `SaveBlock3` save space.

DexNav increments the searched species level after a DexNav battle ends in a win or capture. The chain also increases on win or capture.

Chain persistence is controlled by:

```c
#define DEXNAV_PERSIST_CHAIN_ON_WARP TRUE
```

When `DEXNAV_PERSIST_CHAIN_ON_WARP` is `TRUE` (default in this project):

- The chain **survives map warps** (routes, towns, Pokémon Centers, etc.).
- An **active search still ends** on warp; press Start and search again after returning.
- The chain **resets** when the player runs or loses a DexNav battle, flees during search (moved too fast, timeout, lost signal), or **registers a different species** with R in the DexNav UI.

When `DEXNAV_PERSIST_CHAIN_ON_WARP` is `FALSE`, the chain resets on every map warp (closer to ORAS behavior).

## Hidden Abilities

DexNav hidden abilities are generated in `DexNavGetAbilityNum()`. Slot `2` is only selected when all of these are true:

- The search level rolls the hidden ability chance.
- The species has a non-empty hidden ability in slot `2`.
- The player has already caught that species.

The default hidden ability chances are:

| Search Level | Hidden Ability Chance |
| ------------ | --------------------- |
| 0-4          | 0%                    |
| 5-9          | 0%                    |
| 10-24        | 5%                    |
| 25-49        | 15%                   |
| 50-99        | 20%                   |
| 100+         | 23%                   |

If the hidden ability roll fails, DexNav picks a normal ability. If the species has a second normal ability, it randomly picks slot `0` or slot `1`; otherwise it uses slot `0`.

## Searchable Species

DexNav searches species from the current map's wild encounter data. If a species is not present in a searchable encounter table, DexNav will not find it just because it exists in species data.

For example, Blaziken has Speed Boost in hidden ability slot `2`:

```c
.abilities = { ABILITY_BLAZE, ABILITY_NONE, ABILITY_SPEED_BOOST },
```

DexNav can only roll that slot for a Torchic, Combusken, or Blaziken encounter if that species is available through DexNav-searchable wild data and has already been caught in the Pokedex.

## Testing Checklist

1. Build after enabling DexNav and assigning non-zero flags and vars.
2. Start a save where DexNav is unlocked, or use a debug script that sets `DN_FLAG_DEXNAV_GET` and `DN_FLAG_DETECTOR_MODE`.
3. Search for a species that is present in the current map's wild encounters.
4. Catch that species at least once so hidden ability information is unlocked.
5. Raise its search level to at least 10.
6. Continue searching until the hidden ability chance succeeds.

For quick validation, temporarily use a species already in the local wild encounter table and with a known hidden ability. If you are testing a starter such as Torchic, add it to a searchable wild encounter table first or test through another acquisition method.
