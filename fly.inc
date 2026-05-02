// =======================================================================================
// fly.inc - Sistema de voo para jogadores.
// Reescrito por: Rodrigues (CrS)
// =======================================================================================

#if defined _FLY_INCLUDED
    #endinput
#endif
#define _FLY_INCLUDED

#if !defined FLY_SHOW_TUTORIAL
    #error Defina FLY_SHOW_TUTORIAL (0, 1 ou 2) acima de 'fly.inc'
#endif

#if FLY_SHOW_TUTORIAL < 0 || FLY_SHOW_TUTORIAL > 2
    #error FLY_SHOW_TUTORIAL deve ser 0, 1 ou 2
#endif


#if !defined FLY_IS_MOBILE
    #error Defina FLY_IS_MOBILE (0 ou 1) acima de 'fly.inc'
#endif

#if FLY_IS_MOBILE < 0 || FLY_IS_MOBILE > 1
    #error FLY_IS_MOBILE deve ser 0 ou 1
#endif

// # Variables 
static bool:gPlayerFlying[MAX_PLAYERS] = {false, ...};

static 
    bool:gFlyForward[MAX_PLAYERS],
    bool:gFlyUp[MAX_PLAYERS],
    bool:gFlyDown[MAX_PLAYERS],
    Float:gFlySpeed[MAX_PLAYERS] = {1.0, ...};

static 
    PlayerText:gFlyBtn_Forward[MAX_PLAYERS],
    PlayerText:gFlyBtn_Up[MAX_PLAYERS],
    PlayerText:gFlyBtn_Down[MAX_PLAYERS],
    PlayerText:gFlyBtn_Fast[MAX_PLAYERS],
    PlayerText:gFlyBtn_Slow[MAX_PLAYERS],
    PlayerText:gFlyBtn_Left[MAX_PLAYERS],
    PlayerText:gFlyBtn_Right[MAX_PLAYERS];


// # Forwards 
forward Fly_Update(playerid);
forward Fly_OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid);


// # Functions
stock bool:Fly_Start(playerid)
{
    if (gPlayerFlying[playerid])
        return false;

    gPlayerFlying[playerid] = true;

    new Float:flyPos[3];
    GetPlayerPos(playerid, flyPos[0], flyPos[1], flyPos[2]);
    SetPlayerPos(playerid, flyPos[0], flyPos[1], flyPos[2] + 5.0);

    #if defined FLY_SHOW_TUTORIAL

        #if FLY_SHOW_TUTORIAL == 1
            #if defined FLY_IS_MOBILE
                #if FLY_IS_MOBILE == 1
                    SendClientMessage(playerid, 0xB9DA66FF, "[FLY]: Você entrou no modo de voo. Se mova clicando nas textdraws.");
                #elseif FLY_IS_MOBILE == 0
                    SendClientMessage(playerid, 0xB9DA66FF, "[FLY]: Você entrou no modo de voo. Se mova usando WS/SPACE/MOUSE DIR/ESQ.");
                #endif
            #endif
        #elseif FLY_SHOW_TUTORIAL == 2
            GameTextForPlayer(playerid, "~g~~h~MODO VOO HABILITADO", 3000, 4);
        #endif

    #endif

    Fly_Update(playerid);

    #if defined FLY_IS_MOBILE
        Fly_CreateUI(playerid);
    #endif

    return true;
}

stock bool:Fly_Stop(playerid)
{
    if (!gPlayerFlying[playerid])
        return false;

    gPlayerFlying[playerid] = false;
    ClearAnimations(playerid, 1);

    #if defined FLY_IS_MOBILE
        #if FLY_IS_MOBILE == 1
            Fly_DestroyUI(playerid);
        #endif
    #endif
    
    return true;
}

public Fly_Update(playerid)
{
    if (!IsPlayerConnected(playerid))
        return false;

    new keys, updown, leftright;
    GetPlayerKeys(playerid, keys, updown, leftright);

    new 
        Float:playerPos[3],
        Float:frontPos[3],
        Float:verticalPos[3];

    GetPlayerPos(playerid, playerPos[0], playerPos[1], playerPos[2]);
    GetPlayerCameraFrontVector(playerid, frontPos[0], frontPos[1], frontPos[2]);

    new Float:speed = gFlySpeed[playerid];

    if (updown < 0 || gFlyForward[playerid])
    {
        verticalPos[0] = frontPos[0];
        verticalPos[1] = frontPos[1];
    }

    verticalPos[2] = 0.0;

    if (keys & 4 || gFlyUp[playerid])
    {
        verticalPos[2] = 0.25;
        verticalPos[2] *= speed;
    }
    else if (keys & 128 || gFlyDown[playerid])
    {
        verticalPos[2] = -0.25;
        verticalPos[2] *= speed;
    }
    else
    {
        verticalPos[2] = 0.035;
    }

    verticalPos[0] *= speed;
    verticalPos[1] *= speed;

    if (keys & KEY_WALK)
    {
        verticalPos[0] /= 5.0;
        verticalPos[1] /= 5.0;
        verticalPos[2] /= 5.0;
    }

    if (keys & KEY_SPRINT)
    {
        verticalPos[0] *= 4.0;
        verticalPos[1] *= 4.0;
        verticalPos[2] *= 4.0;
    }

    SetPlayerVelocity(playerid, verticalPos[0], verticalPos[1], verticalPos[2]);

    if (verticalPos[0] != 0.0 || verticalPos[1] != 0.0)
    {
        new Float:camPos[3];
        GetPlayerCameraPos(playerid, camPos[0], camPos[1], camPos[2]);

        Fly_LookAt(playerid, frontPos[0] * 500.0 + camPos[0], frontPos[1] * 500.0 + camPos[1]);

        if (GetPlayerAnimationIndex(playerid) != 959)
            ApplyAnimation(playerid, "PARACHUTE", "FALL_SkyDive_Accel", 6.1, true, true, true, true, false, true);
    }
    else
    {
        if (GetPlayerAnimationIndex(playerid) == 959)
            ApplyAnimation(playerid, "PARACHUTE", "PARA_steerR", 6.1, true, true, true, true, false, true);
    }

    #if defined FLY_IS_MOBILE
        #if FLY_IS_MOBILE == 1
            static stringSpeed[15];
            format(stringSpeed, sizeof(stringSpeed), "IR (%.1fx)", speed);
            PlayerTextDrawSetString(playerid, gFlyBtn_Forward[playerid], stringSpeed);

            gFlyForward[playerid] = false;
            gFlyUp[playerid] = false;
            gFlyDown[playerid] = false;
        #endif
    #endif

    if (gPlayerFlying[playerid])
        SetTimerEx("Fly_Update", 150, false, "i", playerid);

    return true;
}

static stock Fly_LookAt(playerid, Float:x, Float:y)
{
    new Float:playerPos[4];
    GetPlayerPos(playerid, playerPos[0], playerPos[1], playerPos[2]);

    playerPos[3] = floatabs(atan((y - playerPos[1]) / (x - playerPos[0])));

    if (x <= playerPos[0] && y >= playerPos[1]) playerPos[3] = 180.0 - playerPos[3];
    else if (x < playerPos[0] && y < playerPos[1]) playerPos[3] += 180.0;
    else if (x >= playerPos[0] && y <= playerPos[1]) playerPos[3] = 360.0 - playerPos[3];

    playerPos[3] -= 90.0;

    if (playerPos[3] >= 360.0)
        playerPos[3] -= 360.0;

    SetPlayerFacingAngle(playerid, playerPos[3]);
    return true;
}

static stock Fly_CreateUI(playerid)
{
    gFlyBtn_Up[playerid] = CreatePlayerTextDraw(playerid, 595.000, 344.000, "LD_BEAT:up");
    PlayerTextDrawTextSize(playerid, gFlyBtn_Up[playerid], 25.000, 25.000);
    PlayerTextDrawAlignment(playerid, gFlyBtn_Up[playerid], 1);
    PlayerTextDrawColor(playerid, gFlyBtn_Up[playerid], -1);
    PlayerTextDrawSetShadow(playerid, gFlyBtn_Up[playerid], 0);
    PlayerTextDrawSetOutline(playerid, gFlyBtn_Up[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, gFlyBtn_Up[playerid], 255);
    PlayerTextDrawFont(playerid, gFlyBtn_Up[playerid], 4);
    PlayerTextDrawSetProportional(playerid, gFlyBtn_Up[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, gFlyBtn_Up[playerid], 1);
    PlayerTextDrawShow(playerid, gFlyBtn_Up[playerid]);

    gFlyBtn_Down[playerid] = CreatePlayerTextDraw(playerid, 595.000, 398.000, "LD_BEAT:down");
    PlayerTextDrawTextSize(playerid, gFlyBtn_Down[playerid], 25.000, 25.000);
    PlayerTextDrawAlignment(playerid, gFlyBtn_Down[playerid], 1);
    PlayerTextDrawColor(playerid, gFlyBtn_Down[playerid], -1);
    PlayerTextDrawSetShadow(playerid, gFlyBtn_Down[playerid], 0);
    PlayerTextDrawSetOutline(playerid, gFlyBtn_Down[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, gFlyBtn_Down[playerid], 255);
    PlayerTextDrawFont(playerid, gFlyBtn_Down[playerid], 4);
    PlayerTextDrawSetProportional(playerid, gFlyBtn_Down[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, gFlyBtn_Down[playerid], 1);
    PlayerTextDrawShow(playerid, gFlyBtn_Down[playerid]);

    gFlyBtn_Fast[playerid] = CreatePlayerTextDraw(playerid, 553.000, 353.000, "+ VEL");
    PlayerTextDrawLetterSize(playerid, gFlyBtn_Fast[playerid], 0.250, 1.099);
    PlayerTextDrawTextSize(playerid, gFlyBtn_Fast[playerid], 10.000, 41.000);
    PlayerTextDrawAlignment(playerid, gFlyBtn_Fast[playerid], 2);
    PlayerTextDrawColor(playerid, gFlyBtn_Fast[playerid], -1);
    PlayerTextDrawUseBox(playerid, gFlyBtn_Fast[playerid], 1);
    PlayerTextDrawBoxColor(playerid, gFlyBtn_Fast[playerid], 793726975);
    PlayerTextDrawSetShadow(playerid, gFlyBtn_Fast[playerid], 0);
    PlayerTextDrawSetOutline(playerid, gFlyBtn_Fast[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, gFlyBtn_Fast[playerid], 255);
    PlayerTextDrawFont(playerid, gFlyBtn_Fast[playerid], 1);
    PlayerTextDrawSetProportional(playerid, gFlyBtn_Fast[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, gFlyBtn_Fast[playerid], 1);
    PlayerTextDrawShow(playerid, gFlyBtn_Fast[playerid]);

    gFlyBtn_Slow[playerid] = CreatePlayerTextDraw(playerid, 553.000, 404.000, "- VEL");
    PlayerTextDrawLetterSize(playerid, gFlyBtn_Slow[playerid], 0.250, 1.099);
    PlayerTextDrawTextSize(playerid, gFlyBtn_Slow[playerid], 10.000, 41.000);
    PlayerTextDrawAlignment(playerid, gFlyBtn_Slow[playerid], 2);
    PlayerTextDrawColor(playerid, gFlyBtn_Slow[playerid], -1);
    PlayerTextDrawUseBox(playerid, gFlyBtn_Slow[playerid], 1);
    PlayerTextDrawBoxColor(playerid, gFlyBtn_Slow[playerid], 793726975);
    PlayerTextDrawSetShadow(playerid, gFlyBtn_Slow[playerid], 0);
    PlayerTextDrawSetOutline(playerid, gFlyBtn_Slow[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, gFlyBtn_Slow[playerid], 255);
    PlayerTextDrawFont(playerid, gFlyBtn_Slow[playerid], 1);
    PlayerTextDrawSetProportional(playerid, gFlyBtn_Slow[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, gFlyBtn_Slow[playerid], 1);
    PlayerTextDrawShow(playerid, gFlyBtn_Slow[playerid]);

    gFlyBtn_Forward[playerid] = CreatePlayerTextDraw(playerid, 553.000, 378.000, "IR (1.0x)");
    PlayerTextDrawLetterSize(playerid, gFlyBtn_Forward[playerid], 0.250, 1.099);
    PlayerTextDrawTextSize(playerid, gFlyBtn_Forward[playerid], 10.000, 41.000);
    PlayerTextDrawAlignment(playerid, gFlyBtn_Forward[playerid], 2);
    PlayerTextDrawColor(playerid, gFlyBtn_Forward[playerid], -1);
    PlayerTextDrawUseBox(playerid, gFlyBtn_Forward[playerid], 1);
    PlayerTextDrawBoxColor(playerid, gFlyBtn_Forward[playerid], 793726975);
    PlayerTextDrawSetShadow(playerid, gFlyBtn_Forward[playerid], 0);
    PlayerTextDrawSetOutline(playerid, gFlyBtn_Forward[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, gFlyBtn_Forward[playerid], 255);
    PlayerTextDrawFont(playerid, gFlyBtn_Forward[playerid], 1);
    PlayerTextDrawSetProportional(playerid, gFlyBtn_Forward[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, gFlyBtn_Forward[playerid], 1);
    PlayerTextDrawShow(playerid, gFlyBtn_Forward[playerid]);

    gFlyBtn_Left[playerid] = CreatePlayerTextDraw(playerid, 430.000, 370.000, "LD_BEAT:left");
    PlayerTextDrawTextSize(playerid, gFlyBtn_Left[playerid], 25.000, 25.000);
    PlayerTextDrawAlignment(playerid, gFlyBtn_Left[playerid], 1);
    PlayerTextDrawColor(playerid, gFlyBtn_Left[playerid], -1);
    PlayerTextDrawSetShadow(playerid, gFlyBtn_Left[playerid], 0);
    PlayerTextDrawSetOutline(playerid, gFlyBtn_Left[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, gFlyBtn_Left[playerid], 255);
    PlayerTextDrawFont(playerid, gFlyBtn_Left[playerid], 4);
    PlayerTextDrawSetProportional(playerid, gFlyBtn_Left[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, gFlyBtn_Left[playerid], 1);
    PlayerTextDrawShow(playerid, gFlyBtn_Left[playerid]);

    gFlyBtn_Right[playerid] = CreatePlayerTextDraw(playerid, 478.000, 370.000, "LD_BEAT:right");
    PlayerTextDrawTextSize(playerid, gFlyBtn_Right[playerid], 25.000, 25.000);
    PlayerTextDrawAlignment(playerid, gFlyBtn_Right[playerid], 1);
    PlayerTextDrawColor(playerid, gFlyBtn_Right[playerid], -1);
    PlayerTextDrawSetShadow(playerid, gFlyBtn_Right[playerid], 0);
    PlayerTextDrawSetOutline(playerid, gFlyBtn_Right[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, gFlyBtn_Right[playerid], 255);
    PlayerTextDrawFont(playerid, gFlyBtn_Right[playerid], 4);
    PlayerTextDrawSetProportional(playerid, gFlyBtn_Right[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, gFlyBtn_Right[playerid], 1);
    PlayerTextDrawShow(playerid, gFlyBtn_Right[playerid]);

    SelectTextDraw(playerid, 0xFFFFFFFF);
    return true;
}

static stock Fly_DestroyUI(playerid)
{
    PlayerTextDrawHide(playerid, gFlyBtn_Forward[playerid]);
    PlayerTextDrawHide(playerid, gFlyBtn_Up[playerid]);
    PlayerTextDrawHide(playerid, gFlyBtn_Down[playerid]);
    PlayerTextDrawHide(playerid, gFlyBtn_Fast[playerid]);
    PlayerTextDrawHide(playerid, gFlyBtn_Slow[playerid]);
    PlayerTextDrawHide(playerid, gFlyBtn_Left[playerid]);
    PlayerTextDrawHide(playerid, gFlyBtn_Right[playerid]);

    CancelSelectTextDraw(playerid);
    return true;
}

#if defined Fly_OnPlayerClickPlayerTextDraw
    forward Fly_OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid);
#endif

public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
    if (playertextid == gFlyBtn_Forward[playerid]) gFlyForward[playerid] = true;
    else if (playertextid == gFlyBtn_Up[playerid]) gFlyUp[playerid] = true;
    else if (playertextid == gFlyBtn_Down[playerid]) gFlyDown[playerid] = true;

    else if (playertextid == gFlyBtn_Fast[playerid])
    {
        gFlySpeed[playerid] += 0.2;
        if (gFlySpeed[playerid] > 3.0) gFlySpeed[playerid] = 3.0;
    }
    else if (playertextid == gFlyBtn_Slow[playerid])
    {
        gFlySpeed[playerid] -= 0.2;
        if (gFlySpeed[playerid] < 0.2) gFlySpeed[playerid] = 0.2;
    }
    else if (playertextid == gFlyBtn_Left[playerid])
    {
        new Float:ang;
        GetPlayerFacingAngle(playerid, ang);
        SetPlayerFacingAngle(playerid, ang + (10.0 * gFlySpeed[playerid]));
        SetCameraBehindPlayer(playerid);
    }
    else if (playertextid == gFlyBtn_Right[playerid])
    {
        new Float:ang;
        GetPlayerFacingAngle(playerid, ang);
        SetPlayerFacingAngle(playerid, ang - (10.0 * gFlySpeed[playerid]));
        SetCameraBehindPlayer(playerid);
    }

    if (funcidx("Fly_OnPlayerClickPlayerTextDraw") != -1)
        return CallLocalFunction("Fly_OnPlayerClickPlayerTextDraw", "ii", playerid, _:playertextid);

    return true;
}