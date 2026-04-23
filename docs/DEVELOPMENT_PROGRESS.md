# Development Progress

## Summary

The project has moved from a single hard-coded battle prototype to a scene-based structure.

Completed in the current codebase:

- Main menu scene
- Stage select scene
- Battle scene extraction from `App`
- Stage data loading from `Stages.json`
- Wave-based enemy spawning
- Battle result overlay with retry and navigation actions

## Completed Features

### Core Battle

- 10 cat units and 1 enemy unit defined in `Resources/Data/Units.json`
- Data-driven unit stats and animation frame counts
- Combat resolution in `UnitManager`
- Area attack support
- Knockback support
- Death animation and cleanup
- Enemy and player base destruction ending the battle
- **Cat Cannon**: Area damage and knockback ability with cooldown, featuring a sweeping purple laser and sequential explosion effects.

### UI and Flow

- Main menu with `Start` and `Exit`
- Stage select screen with a text-based stage list
- Battle result overlay with:
  - Retry
  - Back to Stage Select
  - Back to Main Menu
- Battle deployment UI and money display remain active inside battle
- **Worker Cat Upgrade System**: Upgradeable money generation rate and max money cap.
- **Cat Cannon UI**: Dynamic cooldown button that charges up and flashes when ready.

### Stage System

- `Resources/Data/Stages.json` introduced
- Each stage currently supports:
  - `id`
  - `displayName`
  - `background`
  - `enemyBaseHp`
  - `waves`
- Wave entries currently support:
  - `triggerTime`
  - `spawns`
  - per-spawn `unit`, `count`, `interval`

### Tools
- **SpriteProcessor**: A Python-based automation tool to split and crop `.webp`/`.png` sprite sheets into individual transparent frames for easy engine integration.

## Architecture Update

### App

`App` now owns scene switching instead of directly owning battle logic.

- `App::ChangeScene(...)` performs deferred scene switching
- This avoids switching scenes while a current scene update is still executing

### Scenes

New scene classes:

- `MainMenuScene`
- `StageSelectScene`
- `BattleScene`

Shared interface:

- `IScene`

### Stage Data

New stage-related modules:

- `StageData`
- `StageLoader`
- `WaveSpawner`

## Known Limitations

- Menu and stage select use temporary text UI instead of finalized art-driven widgets
- Stage backgrounds are temporary integrated assets
- Available unit lists are not stage-specific yet
- Money rules are still global, not per-stage
- Enemy wave rules are time-triggered only for now

## Next Tasks

- Expand `Stages.json` to support richer wave conditions and metadata
- Move menu and select UI from temporary text overlay to final interface assets
- Add clearer battle result presentation
- Add stage descriptions or preview metadata in stage select
- Consider stage-specific available unit lists
- Consider stage-specific economy settings if needed later
