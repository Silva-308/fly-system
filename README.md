# Fly

Flight system for SA-MP players. Supports both keyboard and touch (TextDraw) control schemes, with compile-time configuration for tutorial style and platform.

## Requirements

- SA-MP server with Pawn compiler
- `a_samp` include (standard SA-MP library)

## Installation

Place `fly.inc` inside your `pawno/include/` folder and add the required defines before the include directive in your gamemode or filterscript.

## Configuration
 
Both defines are mandatory. The compiler will raise a descriptive `#error` if either is missing or out of range.
 
```pawn
#define FLY_SHOW_TUTORIAL <0|1|2>
#define FLY_IS_MOBILE <0|1>
//
#include <fly>
```
 
`FLY_SHOW_TUTORIAL` controls whether and how a tutorial message is shown when a player enters flight mode.
 
| Value | Behaviour |
|-------|-----------|
| `0` | No message |
| `1` | Message via `SendClientMessage` |
| `2` | Message via `GameTextForPlayer` |
 
`FLY_IS_MOBILE` selects the control scheme.
 
| Value | Behaviour |
|-------|-----------|
| `0` | Keyboard — W to move forward, left mouse button to ascend, right mouse button to descend, Shift to sprint |
| `1` | Touch — on-screen TextDraw buttons for all directions and speed control |
 
Setting `FLY_IS_MOBILE` to `0` compiles out all TextDraw code entirely, producing a smaller binary with no unused symbols.

## API

```pawn
// Starts flight mode for the player.
// Returns true on success, false if the player was already flying.
bool:Fly_Start(playerid)

// Stops flight mode for the player.
// Returns true on success, false if the player was not flying.
bool:Fly_Stop(playerid)

// Returns true if the player is currently in flight mode.
bool:Fly_Is_Flying(playerid)

// Returns the player's current flight speed multiplier.
Float:Fly_Get_Speed(playerid)

// Sets the player's flight speed multiplier (clamped to the configured range).
Fly_Set_Speed(playerid, Float:speed)
```

## Configuration API

These functions adjust global flight behaviour at runtime and take effect on the next `Fly_Start` call (active flights are not interrupted).

```pawn
// Sets the speed assigned to a player when Fly_Start is called.
// Clamped to the current speed range.
Fly_Config_Set_Default_Speed(Float:speed)
Float:Fly_Config_Get_Default_Speed()
 
// Sets the minimum and maximum speed multiplier.
// Automatically adjusts the default speed if it falls outside the new range.
Fly_Config_Set_Speed_Range(Float:min_speed, Float:max_speed)
Float:Fly_Config_Get_Speed_Min()
Float:Fly_Config_Get_Speed_Max()
 
// Sets the speed increment per button press (mobile only).
Fly_Config_Set_Speed_Step(Float:step)
Float:Fly_Config_Get_Speed_Step()
 
// Sets the update tick interval in milliseconds (50–500).
// Lower values produce smoother movement at the cost of higher CPU usage.
Fly_Config_Set_Timer_Ms(ms)
Fly_Config_Get_Timer_Ms()
```

## Usage example

```pawn
#define FLY_SHOW_TUTORIAL 1
#define FLY_IS_MOBILE 0
//
#include <fly>

public OnGameModeInit() {
    Fly_Config_Set_Speed_Range(0.2, 5.0);
    Fly_Config_Set_Default_Speed(1.5);

    return 1;
}

public OnPlayerCommandText(playerid, cmdtext[]) {
    if (!strcmp(cmdtext, "/fly", true)) {
        if (Fly_Is_Flying(playerid)) {
            Fly_Stop(playerid);

            SendClientMessage(playerid, -1, "Flight mode disabled.");
        }
        else
            Fly_Start(playerid);

        return 1;
    }
    
    return 0;
}
```

## Credits

- Author: Rodrigues (CrS)
- Credits: [Norck](https://sampforum.blast.hk/member.php?action=profile&uid=36448)