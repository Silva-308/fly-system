// =======================================================================================
// fly.inc - Player flight system for SA-MP.
//
// Author: Rodrigues (CrS)
// Credits: Norck (https://sampforum.blast.hk/member.php?action=profile&uid=36448)

#if defined _FLY_INCLUDED
    #endinput
#endif
#define _FLY_INCLUDED

#include <a_samp>

#if !defined FLY_SHOW_TUTORIAL
    #error Define FLY_SHOW_TUTORIAL (0, 1 or 2) before including 'fly.inc'
#endif
#if FLY_SHOW_TUTORIAL < 0 || FLY_SHOW_TUTORIAL > 2
    #error FLY_SHOW_TUTORIAL must be 0, 1 or 2
#endif

#if !defined FLY_IS_MOBILE
    #error Define FLY_IS_MOBILE (0 or 1) before including 'fly.inc'
#endif
#if FLY_IS_MOBILE < 0 || FLY_IS_MOBILE > 1
    #error FLY_IS_MOBILE must be 0 or 1
#endif

static const Float:FLY_GRAVITY_OFFSET = 0.035;
static const Float:FLY_VERTICAL_SPEED = 0.25;
static const Float:FLY_WALK_DIVIDER = 5.0;
static const Float:FLY_SPRINT_MULTIPLIER = 4.0;
static const Float:FLY_LOOKAT_DISTANCE = 500.0;
static const FLY_ANIM_SKYDIVE_IDX = 959;
static const Float:FLY_ANIM_SPEED = 6.1;
static const FLY_KEY_UP = 4;
static const FLY_KEY_DOWN = 128;

#if FLY_IS_MOBILE == 1
static const Float:FLY_ROTATION_STEP = 10.0;
static const FLY_UI_BOX_COLOR = 0x2F4F4FFF;
#endif

#if FLY_SHOW_TUTORIAL == 1
static const FLY_MSG_COLOR = 0xB9DA66FF;
#endif

static Float:cfg_speed_default = 1.0;
static Float:cfg_speed_min = 0.2;
static Float:cfg_speed_max = 3.0;
static Float:cfg_speed_step = 0.2;
static cfg_timer_ms = 150;

static bool:player_flying[MAX_PLAYERS];
static fly_timer_id[MAX_PLAYERS] = {-1, ...};
static Float:fly_speed[MAX_PLAYERS];

#if FLY_IS_MOBILE == 1
static bool:fly_forward[MAX_PLAYERS],
    bool:fly_up[MAX_PLAYERS],
    bool:fly_down[MAX_PLAYERS];

static PlayerText:btn_forward[MAX_PLAYERS],
    PlayerText:btn_up[MAX_PLAYERS],
    PlayerText:btn_down[MAX_PLAYERS],
    PlayerText:btn_fast[MAX_PLAYERS],
    PlayerText:btn_slow[MAX_PLAYERS],
    PlayerText:btn_left[MAX_PLAYERS],
    PlayerText:btn_right[MAX_PLAYERS];
#endif

forward Fly_Update(playerid);

#if FLY_IS_MOBILE == 1
static stock Fly_Create_UI(playerid) {
    btn_up[playerid] = CreatePlayerTextDraw(playerid, 595.0, 344.0, "LD_BEAT:up");
    PlayerTextDrawTextSize(playerid, btn_up[playerid], 25.0, 25.0);
    PlayerTextDrawAlignment(playerid, btn_up[playerid], 1);
    PlayerTextDrawColor(playerid, btn_up[playerid], -1);
    PlayerTextDrawSetShadow(playerid, btn_up[playerid], 0);
    PlayerTextDrawSetOutline(playerid, btn_up[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, btn_up[playerid], 255);
    PlayerTextDrawFont(playerid, btn_up[playerid], 4);
    PlayerTextDrawSetProportional(playerid, btn_up[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, btn_up[playerid], 1);
    PlayerTextDrawShow(playerid, btn_up[playerid]);

    btn_down[playerid] = CreatePlayerTextDraw(playerid, 595.0, 398.0, "LD_BEAT:down");
    PlayerTextDrawTextSize(playerid, btn_down[playerid], 25.0, 25.0);
    PlayerTextDrawAlignment(playerid, btn_down[playerid], 1);
    PlayerTextDrawColor(playerid, btn_down[playerid], -1);
    PlayerTextDrawSetShadow(playerid, btn_down[playerid], 0);
    PlayerTextDrawSetOutline(playerid, btn_down[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, btn_down[playerid], 255);
    PlayerTextDrawFont(playerid, btn_down[playerid], 4);
    PlayerTextDrawSetProportional(playerid, btn_down[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, btn_down[playerid], 1);
    PlayerTextDrawShow(playerid, btn_down[playerid]);

    btn_fast[playerid] = CreatePlayerTextDraw(playerid, 553.0, 353.0, "+ SPD");
    PlayerTextDrawLetterSize(playerid, btn_fast[playerid], 0.250, 1.099);
    PlayerTextDrawTextSize(playerid, btn_fast[playerid], 10.0, 41.0);
    PlayerTextDrawAlignment(playerid, btn_fast[playerid], 2);
    PlayerTextDrawColor(playerid, btn_fast[playerid], -1);
    PlayerTextDrawUseBox(playerid, btn_fast[playerid], 1);
    PlayerTextDrawBoxColor(playerid, btn_fast[playerid], FLY_UI_BOX_COLOR);
    PlayerTextDrawSetShadow(playerid, btn_fast[playerid], 0);
    PlayerTextDrawSetOutline(playerid, btn_fast[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, btn_fast[playerid], 255);
    PlayerTextDrawFont(playerid, btn_fast[playerid], 1);
    PlayerTextDrawSetProportional(playerid, btn_fast[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, btn_fast[playerid], 1);
    PlayerTextDrawShow(playerid, btn_fast[playerid]);

    btn_slow[playerid] = CreatePlayerTextDraw(playerid, 553.0, 404.0, "- SPD");
    PlayerTextDrawLetterSize(playerid, btn_slow[playerid], 0.250, 1.099);
    PlayerTextDrawTextSize(playerid, btn_slow[playerid], 10.0, 41.0);
    PlayerTextDrawAlignment(playerid, btn_slow[playerid], 2);
    PlayerTextDrawColor(playerid, btn_slow[playerid], -1);
    PlayerTextDrawUseBox(playerid, btn_slow[playerid], 1);
    PlayerTextDrawBoxColor(playerid, btn_slow[playerid], FLY_UI_BOX_COLOR);
    PlayerTextDrawSetShadow(playerid, btn_slow[playerid], 0);
    PlayerTextDrawSetOutline(playerid, btn_slow[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, btn_slow[playerid], 255);
    PlayerTextDrawFont(playerid, btn_slow[playerid], 1);
    PlayerTextDrawSetProportional(playerid, btn_slow[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, btn_slow[playerid], 1);
    PlayerTextDrawShow(playerid, btn_slow[playerid]);

    btn_forward[playerid] = CreatePlayerTextDraw(playerid, 553.0, 378.0, "GO (1.0x)");
    PlayerTextDrawLetterSize(playerid, btn_forward[playerid], 0.250, 1.099);
    PlayerTextDrawTextSize(playerid, btn_forward[playerid], 10.0, 41.0);
    PlayerTextDrawAlignment(playerid, btn_forward[playerid], 2);
    PlayerTextDrawColor(playerid, btn_forward[playerid], -1);
    PlayerTextDrawUseBox(playerid, btn_forward[playerid], 1);
    PlayerTextDrawBoxColor(playerid, btn_forward[playerid], FLY_UI_BOX_COLOR);
    PlayerTextDrawSetShadow(playerid, btn_forward[playerid], 0);
    PlayerTextDrawSetOutline(playerid, btn_forward[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, btn_forward[playerid], 255);
    PlayerTextDrawFont(playerid, btn_forward[playerid], 1);
    PlayerTextDrawSetProportional(playerid, btn_forward[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, btn_forward[playerid], 1);
    PlayerTextDrawShow(playerid, btn_forward[playerid]);

    btn_left[playerid] = CreatePlayerTextDraw(playerid, 430.0, 370.0, "LD_BEAT:left");
    PlayerTextDrawTextSize(playerid, btn_left[playerid], 25.0, 25.0);
    PlayerTextDrawAlignment(playerid, btn_left[playerid], 1);
    PlayerTextDrawColor(playerid, btn_left[playerid], -1);
    PlayerTextDrawSetShadow(playerid, btn_left[playerid], 0);
    PlayerTextDrawSetOutline(playerid, btn_left[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, btn_left[playerid], 255);
    PlayerTextDrawFont(playerid, btn_left[playerid], 4);
    PlayerTextDrawSetProportional(playerid, btn_left[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, btn_left[playerid], 1);
    PlayerTextDrawShow(playerid, btn_left[playerid]);

    btn_right[playerid] = CreatePlayerTextDraw(playerid, 478.0, 370.0, "LD_BEAT:right");
    PlayerTextDrawTextSize(playerid, btn_right[playerid], 25.0, 25.0);
    PlayerTextDrawAlignment(playerid, btn_right[playerid], 1);
    PlayerTextDrawColor(playerid, btn_right[playerid], -1);
    PlayerTextDrawSetShadow(playerid, btn_right[playerid], 0);
    PlayerTextDrawSetOutline(playerid, btn_right[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, btn_right[playerid], 255);
    PlayerTextDrawFont(playerid, btn_right[playerid], 4);
    PlayerTextDrawSetProportional(playerid, btn_right[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, btn_right[playerid], 1);
    PlayerTextDrawShow(playerid, btn_right[playerid]);

    SelectTextDraw(playerid, 0xFFFFFFFF);
}

static stock Fly_Destroy_UI(playerid) {
    PlayerTextDrawDestroy(playerid, btn_forward[playerid]);
    PlayerTextDrawDestroy(playerid, btn_up[playerid]);
    PlayerTextDrawDestroy(playerid, btn_down[playerid]);
    PlayerTextDrawDestroy(playerid, btn_fast[playerid]);
    PlayerTextDrawDestroy(playerid, btn_slow[playerid]);
    PlayerTextDrawDestroy(playerid, btn_left[playerid]);
    PlayerTextDrawDestroy(playerid, btn_right[playerid]);

    btn_forward[playerid] = PlayerText:INVALID_TEXT_DRAW;
    btn_up[playerid] = PlayerText:INVALID_TEXT_DRAW;
    btn_down[playerid] = PlayerText:INVALID_TEXT_DRAW;
    btn_fast[playerid] = PlayerText:INVALID_TEXT_DRAW;
    btn_slow[playerid] = PlayerText:INVALID_TEXT_DRAW;
    btn_left[playerid] = PlayerText:INVALID_TEXT_DRAW;
    btn_right[playerid] = PlayerText:INVALID_TEXT_DRAW;

    CancelSelectTextDraw(playerid);
}

static stock Fly_Refresh_Speed_Label(playerid) {
    new speed_str[16];
    format(speed_str, sizeof(speed_str), "GO (%.1fx)", fly_speed[playerid]);

    PlayerTextDrawSetString(playerid, btn_forward[playerid], speed_str);
}

static stock Fly_Reset_Mobile_Flags(playerid) {
    fly_forward[playerid] = false;
    fly_up[playerid] = false;
    fly_down[playerid] = false;
}
#endif

static stock Float:Fly_Clamp_Speed(Float:value, Float:min_val, Float:max_val) {
    if (value < min_val)
        return min_val;

    if (value > max_val)
        return max_val;

    return value;
}

static stock Fly_Look_At(playerid, Float:target_x, Float:target_y) {
    new Float:pos_x, Float:pos_y, Float:pos_z;
    GetPlayerPos(playerid, pos_x, pos_y, pos_z);

    new Float:angle = floatabs(atan((target_y - pos_y) / (target_x - pos_x)));

    if (target_x <= pos_x && target_y >= pos_y)
        angle = 180.0 - angle;
    else if (target_x < pos_x && target_y < pos_y)
        angle += 180.0;
    else if (target_x >= pos_x && target_y <= pos_y)
        angle = 360.0 - angle;

    angle -= 90.0;

    if (angle >= 360.0)
        angle -= 360.0;

    SetPlayerFacingAngle(playerid, angle);
}

static stock Fly_Process_Input(playerid, Float:front_x, Float:front_y, &Float:vel_x, &Float:vel_y, &Float:vel_z) {
    new keys, updown, leftright;
    GetPlayerKeys(playerid, keys, updown, leftright);

    new Float:speed = fly_speed[playerid];

    vel_x = 0.0;
    vel_y = 0.0;
    vel_z = FLY_GRAVITY_OFFSET;

#if FLY_IS_MOBILE == 1
    new bool:move_forward = (updown < 0 || fly_forward[playerid]);
#else
    new bool:move_forward = (updown < 0);
#endif

    if (move_forward) {
        vel_x = front_x;
        vel_y = front_y;
    }

#if FLY_IS_MOBILE == 1
    new bool:move_up = (keys & FLY_KEY_UP || fly_up[playerid]);
    new bool:move_down = (keys & FLY_KEY_DOWN || fly_down[playerid]);
#else
    new bool:move_up = bool:(keys & FLY_KEY_UP);
    new bool:move_down = bool:(keys & FLY_KEY_DOWN);
#endif

    if (move_up)
        vel_z = FLY_VERTICAL_SPEED * speed;
    else if (move_down)
        vel_z = -(FLY_VERTICAL_SPEED) * speed;

    vel_x *= speed;
    vel_y *= speed;

    if (keys & KEY_WALK) {
        vel_x /= FLY_WALK_DIVIDER;
        vel_y /= FLY_WALK_DIVIDER;
        vel_z /= FLY_WALK_DIVIDER;
    }
    else if (keys & KEY_SPRINT) {
        vel_x *= FLY_SPRINT_MULTIPLIER;
        vel_y *= FLY_SPRINT_MULTIPLIER;
        vel_z *= FLY_SPRINT_MULTIPLIER;
    }
}

static stock Fly_Sync_Animation(playerid, Float:vel_x, Float:vel_y) {
    new bool:is_moving = (vel_x != 0.0 || vel_y != 0.0);
    new anim_idx = GetPlayerAnimationIndex(playerid);

    if (is_moving && anim_idx != FLY_ANIM_SKYDIVE_IDX)
        ApplyAnimation(playerid, "PARACHUTE", "FALL_SkyDive_Accel", FLY_ANIM_SPEED, true, true, true, true, false, true);
    else if (!is_moving && anim_idx == FLY_ANIM_SKYDIVE_IDX)
        ApplyAnimation(playerid, "PARACHUTE", "PARA_steerR", FLY_ANIM_SPEED, true, true, true, true, false, true);
}

static stock Fly_Apply_Orientation(playerid, Float:front_x, Float:front_y, Float:vel_x, Float:vel_y) {
    if (vel_x == 0.0 && vel_y == 0.0)
        return;

    new Float:cam_x, Float:cam_y, Float:cam_z;
    GetPlayerCameraPos(playerid, cam_x, cam_y, cam_z);

    Fly_Look_At(playerid, front_x * FLY_LOOKAT_DISTANCE + cam_x, front_y * FLY_LOOKAT_DISTANCE + cam_y);
}

static stock Fly_Reset_State(playerid) {
    player_flying[playerid] = false;
    fly_speed[playerid] = cfg_speed_default;
    fly_timer_id[playerid] = -1;

#if FLY_IS_MOBILE == 1
    fly_forward[playerid] = false;
    fly_up[playerid] = false;
    fly_down[playerid] = false;
#endif
}

stock bool:Fly_Start(playerid) {
    if (player_flying[playerid])
        return false;

    player_flying[playerid] = true;
    fly_speed[playerid] = cfg_speed_default;

    new Float:pos_x, Float:pos_y, Float:pos_z;

    GetPlayerPos(playerid, pos_x, pos_y, pos_z);
    SetPlayerPos(playerid, pos_x, pos_y, pos_z + 5.0);

#if FLY_SHOW_TUTORIAL == 1
#if FLY_IS_MOBILE == 1
    SendClientMessage(playerid, FLY_MSG_COLOR, "[Fly]: Flight mode enabled. Use the on-screen buttons to move.");
#else
    SendClientMessage(playerid, FLY_MSG_COLOR, "[Fly]: Flight mode enabled. W to move, LMB to ascend, RMB to descend, Shift to sprint.");
#endif
#elseif FLY_SHOW_TUTORIAL == 2
    GameTextForPlayer(playerid, "~g~~h~FLIGHT MODE ENABLED", 3000, 4);
#endif

#if FLY_IS_MOBILE == 1
    Fly_Create_UI(playerid);
#endif

    fly_timer_id[playerid] = SetTimerEx("Fly_Update", cfg_timer_ms, false, "i", playerid);

    return true;
}

stock bool:Fly_Stop(playerid) {
    if (!player_flying[playerid])
        return false;

    if (fly_timer_id[playerid] != -1)
        KillTimer(fly_timer_id[playerid]);

    ClearAnimations(playerid, 1);

#if FLY_IS_MOBILE == 1
    Fly_Destroy_UI(playerid);
#endif

    Fly_Reset_State(playerid);

    return true;
}

stock bool:Fly_Is_Flying(playerid)
    return player_flying[playerid];

stock Float:Fly_Get_Speed(playerid)
    return fly_speed[playerid];

stock Fly_Set_Speed(playerid, Float:speed) {
    fly_speed[playerid] = Fly_Clamp_Speed(speed, cfg_speed_min, cfg_speed_max);
}

stock Fly_Config_Set_Default_Speed(Float:speed) {
    if (speed < cfg_speed_min)
        speed = cfg_speed_min;

    if (speed > cfg_speed_max)
        speed = cfg_speed_max;

    cfg_speed_default = speed;
}

stock Float:Fly_Config_Get_Default_Speed()
    return cfg_speed_default;

stock Fly_Config_Set_Timer_Ms(ms) {
    if (ms < 50)
        ms = 50;

    if (ms > 500)
        ms = 500;

    cfg_timer_ms = ms;
}

stock Fly_Config_Get_Timer_Ms()
    return cfg_timer_ms;

stock Fly_Config_Set_Speed_Range(Float:min_speed, Float:max_speed) {
    if (min_speed < 0.1)
        min_speed = 0.1;

    if (max_speed < min_speed)
        max_speed = min_speed;

    cfg_speed_min = min_speed;
    cfg_speed_max = max_speed;

    if (cfg_speed_default < cfg_speed_min)
        cfg_speed_default = cfg_speed_min;

    if (cfg_speed_default > cfg_speed_max)
        cfg_speed_default = cfg_speed_max;
}

stock Float:Fly_Config_Get_Speed_Min()
    return cfg_speed_min;

stock Float:Fly_Config_Get_Speed_Max()
    return cfg_speed_max;

stock Fly_Config_Set_Speed_Step(Float:step) {
    if (step < 0.1)
        step = 0.1;

    cfg_speed_step = step;
}

stock Float:Fly_Config_Get_Speed_Step()
    return cfg_speed_step;

public Fly_Update(playerid) {
    if (!IsPlayerConnected(playerid))
        return false;

    new Float:front_x, Float:front_y, Float:front_z;
    GetPlayerCameraFrontVector(playerid, front_x, front_y, front_z);

    new Float:vel_x, Float:vel_y, Float:vel_z;
    Fly_Process_Input(playerid, front_x, front_y, vel_x, vel_y, vel_z);

    SetPlayerVelocity(playerid, vel_x, vel_y, vel_z);

    Fly_Sync_Animation(playerid, vel_x, vel_y);
    Fly_Apply_Orientation(playerid, front_x, front_y, vel_x, vel_y);

#if FLY_IS_MOBILE == 1
    Fly_Refresh_Speed_Label(playerid);
    Fly_Reset_Mobile_Flags(playerid);
#endif

    if (player_flying[playerid])
        fly_timer_id[playerid] = SetTimerEx("Fly_Update", cfg_timer_ms, false, "i", playerid);

    return true;
}

public OnPlayerDisconnect(playerid, reason) {
    if (player_flying[playerid])
        Fly_Stop(playerid);
    else
        Fly_Reset_State(playerid);

#if defined Fly_OnPlayerDisconnect
    return Fly_OnPlayerDisconnect(playerid, reason);
#else
    return true;
#endif
}

#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect Fly_OnPlayerDisconnect
forward Fly_OnPlayerDisconnect(playerid, reason);

#if FLY_IS_MOBILE == 1
public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid) {
    if (!player_flying[playerid])
#if defined Fly_OnPlayerClickPlayerTextDraw
        return Fly_OnPlayerClickPlayerTextDraw(playerid, playertextid);
#else
        return true;
#endif

    if (playertextid == btn_forward[playerid])
        fly_forward[playerid] = true;
    else if (playertextid == btn_up[playerid])
        fly_up[playerid] = true;
    else if (playertextid == btn_down[playerid])
        fly_down[playerid] = true;
    else if (playertextid == btn_fast[playerid])
        fly_speed[playerid] = Fly_Clamp_Speed(fly_speed[playerid] + cfg_speed_step, cfg_speed_min, cfg_speed_max);
    else if (playertextid == btn_slow[playerid])
        fly_speed[playerid] = Fly_Clamp_Speed(fly_speed[playerid] - cfg_speed_step, cfg_speed_min, cfg_speed_max);
    else if (playertextid == btn_left[playerid]) {
        new Float:angle;

        GetPlayerFacingAngle(playerid, angle);
        SetPlayerFacingAngle(playerid, angle + (FLY_ROTATION_STEP * fly_speed[playerid]));
        SetCameraBehindPlayer(playerid);
    }
    else if (playertextid == btn_right[playerid]) {
        new Float:angle;

        GetPlayerFacingAngle(playerid, angle);
        SetPlayerFacingAngle(playerid, angle - (FLY_ROTATION_STEP * fly_speed[playerid]));
        SetCameraBehindPlayer(playerid);
    }

#if defined Fly_OnPlayerClickPlayerTextDraw
    return Fly_OnPlayerClickPlayerTextDraw(playerid, playertextid);
#else
    return true;
#endif
}

#if defined _ALS_OnPlayerClickPlayerTextDraw
    #undef OnPlayerClickPlayerTextDraw
#else
    #define _ALS_OnPlayerClickPlayerTextDraw
#endif
#define OnPlayerClickPlayerTextDraw Fly_OnPlayerClickPlayerTextDraw
forward Fly_OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid);

#endif
