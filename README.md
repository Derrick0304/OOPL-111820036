# OOPL-111820036

Battle Cats-inspired prototype built with the PTSD engine for OOPL.

## Current State

The project currently includes:

- Data-driven unit definitions from `Resources/Data/Units.json`
- A playable battle scene with unit spawning, combat, knockback, and base defeat
- A main menu scene
- A stage select scene
- Externalized stage configuration from `Resources/Data/Stages.json`
- Wave-based enemy spawning per stage

## Scene Flow

The current scene flow is:

1. Main Menu
2. Stage Select
3. Battle Scene
4. Battle Result Overlay

The battle result overlay currently supports:

- Retry
- Back to Stage Select
- Back to Main Menu

## Build

This project is expected to run in `Debug`.

```sh
cmake -DCMAKE_BUILD_TYPE=Debug -B build
cmake --build build
```

## Data Files

- `Resources/Data/Units.json`: unit stats, animation counts, cost, cooldown, icon path
- `Resources/Data/Stages.json`: stage list, background, enemy base HP, enemy waves

## Important Source Files

- `src/App.cpp`: app lifecycle and deferred scene switching
- `src/Scene/MainMenuScene.cpp`: main menu
- `src/Scene/StageSelectScene.cpp`: stage selection
- `src/Scene/BattleScene.cpp`: battle scene and battle result overlay
- `src/Stage/StageLoader.cpp`: stage data loading
- `src/Battle/WaveSpawner.cpp`: stage wave execution
- `src/UnitManager.cpp`: collision, combat, cleanup
- `src/UIManager.cpp`: battle UI and unit deployment buttons

## Notes

- The project currently uses text-based UI for menu and stage selection over temporary background images.
- Menu and stage selection backgrounds were prepared from local assets and converted to PNG for runtime loading.
- Existing uncommitted asset or balance changes in the workspace are not part of this summary.
