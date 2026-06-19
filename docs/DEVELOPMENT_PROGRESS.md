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

- **StartScene (開始介面)**：極簡首頁，使用 `ChapterSelectSceneBackgrounds.png` 作為背景，中間偏下配置 Play 按鈕進入主選單。
- **MainMenuScene 視覺重構**：
  - 移除了中間的 "Battle Cats" 大標題，改為雙門扉中央對接樣式。
  - 左側排版三大按鈕：`Start!!`（進入選章節）、`Upgrade`（貓咪升級）、`Equip`（隊伍編組）。
  - 左下角移植圓形返回按鈕 `Btn_Back_Circle.png`，點擊可流暢返回 `StartScene`。
  - 頂部與底部加上深色邊框（`UI_Border.png`），右上角顯示 `XP` 數值，右下角顯示 `Cat Food` (貓罐頭) 數值，與選關介面視覺統一。
- **ChapterSelectScene Carousel**：實作 4 個方形按鈕的水平拖曳選單（返回、Chapter 1~3），置中放大 1.1x 且旁側縮小 0.8x，點擊各按鈕即可直接跳轉至對應章節或返回主選單。
- **StageSelect Carousel**：A horizontal scrolling stage selector with mouse drag support, auto-snapping, and dynamic scaling feedback.
- **Battle Start System**: A dedicated deployment button in the stage select screen that triggers the currently centered stage.
- **UI Layout Externalization**: All major UI positions (Start, Battle, Menu, Select, Chapter Select) are now loaded from `Resources/Data/UI_Layout.json`
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
- **UI Layout Editor**: A Python/Tkinter tool to visually adjust UI positions, supporting multiple scenes and dynamic element addition.

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

- Stage backgrounds are temporary integrated assets
- Available unit lists are not stage-specific yet
- Money rules are still global, not per-stage
- Enemy wave rules are time-triggered only for now

## Next Tasks

- Expand `Stages.json` to support richer wave conditions and metadata
- Add clearer battle result presentation
- Add stage descriptions or preview metadata in stage select
- Consider stage-specific available unit lists
- Consider stage-specific economy settings if needed later
