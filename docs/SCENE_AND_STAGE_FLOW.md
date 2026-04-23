# Scene And Stage Flow

## Scene Flow

The current runtime flow is:

1. `MainMenuScene`
2. `StageSelectScene`
3. `BattleScene`
4. Battle result overlay inside `BattleScene`

Scene switching is coordinated by `App`.

`App` does not immediately swap scenes when a button callback requests a transition during an active update.
Instead, it stores the next scene and applies the change after the current update finishes.

This avoids invalidating the currently running scene object during its own update loop.

## Stage File

Stage data is loaded from:

`Resources/Data/Stages.json`

Current schema:

```json
{
  "stages": [
    {
      "id": "stage_001",
      "displayName": "First Battle",
      "background": "/Backgrounds/background.png",
      "enemyBaseHp": 1000.0,
      "waves": [
        {
          "triggerTime": 2.0,
          "spawns": [
            { "unit": "Doge", "count": 1, "interval": 0.0 }
          ]
        }
      ]
    }
  ]
}
```

## Wave Execution

`WaveSpawner` tracks:

- elapsed battle time
- which waves have been triggered
- active spawn jobs generated from wave entries

Each wave is activated once `elapsed >= triggerTime`.
Each spawn job then creates units using:

- `unit`
- `count`
- `interval`

## Battle Ownership

`BattleScene` currently owns:

- stage background setup
- base creation
- `UnitManager`
- `UIManager`
- `WaveSpawner`
- `CatCannonEffect`
- money accumulation and Worker Cat level tracking
- battle result overlay

This keeps stage-specific logic inside the battle scene rather than in `App`.
