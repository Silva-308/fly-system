// =======================================================================================
// fly.inc - Sistema de voo para jogadores.
// Author original: Norck
// Edited by: Rodrigues (CrS)
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

// =======================================================================================
// Vars
// =======================================================================================

static bool:gPlayerFlying[MAX_PLAYERS] = {false, ...};

forward Fly_Update(playerid);

// =======================================================================================
// Functions
// =======================================================================================

stock bool:Fly_Start(playerid)
{
    if (gPlayerFlying[playerid])
        return false;

    gPlayerFlying[playerid] = true;

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    SetPlayerPos(playerid, x, y, z + 5.0);

    ApplyAnimation(playerid, "PARACHUTE", "PARA_steerR", 6.1, true, true, true, true, false, true);

    Fly_Update(playerid);

    #if defined FLY_SHOW_TUTORIAL

        #if FLY_SHOW_TUTORIAL == 1
            SendClientMessage(playerid, 0xB9DA66FF, "[FLY]: Você entrou no modo de voo. Use WASD para se mover.");
            SendClientMessage(playerid, 0xB9DA66FF, "[FLY]: MOUSE DIREITO sobe, ESQUERDO desce.");
            SendClientMessage(playerid, 0xB9DA66FF, "[FLY]: ESPAÇO acelera, ALT desacelera.");
        #elseif FLY_SHOW_TUTORIAL == 2
            GameTextForPlayer(playerid, "~g~~h~~h~Modo voo ativado!~n~Use WASD para mover.", 3000, 4);
        #endif

    #endif

    return true;
}

stock bool:Fly_Stop(playerid)
{
    if (!gPlayerFlying[playerid])
        return false;

    gPlayerFlying[playerid] = false;
    ClearAnimations(playerid, 1);
    return true;
}

public Fly_Update(playerid)
{
    if (!IsPlayerConnected(playerid))
        return true;

    new keys, ud, lr;
    GetPlayerKeys(playerid, keys, ud, lr);

    new Float:vx, Float:vy, Float:vz;
    new Float:x, Float:y, Float:z;

    GetPlayerPos(playerid, x, y, z);

    // Movimento frente
    if (ud < 0)
    {
        new Float:fx, Float:fy, Float:fz;
        GetPlayerCameraFrontVector(playerid, fx, fy, fz);
        vx = fx + 0.1;
        vy = fy + 0.1;
    }

    // Vertical
    if (keys & 128) // descer
    {
        vz = -0.2;
    }
    else if (keys & KEY_FIRE) // subir
    {
        vz = 0.2;
    }
    else
    {
        vz = 0.015; // Mantem parado
    }

    // Lento
    if (keys & KEY_WALK)
    {
        vx /= 5.0;
        vy /= 5.0;
        vz /= 5.0;
    }

    // Rápido
    if (keys & KEY_SPRINT)
    {
        vx *= 4.0;
        vy *= 4.0;
        vz *= 4.0;
    }

    SetPlayerVelocity(playerid, vx, vy, vz);

    // Animação
    if (vx == 0.0 && vy == 0.0)
    {
        if (GetPlayerAnimationIndex(playerid) == 959)
            ApplyAnimation(playerid, "PARACHUTE", "PARA_steerR", 6.1, true, true, true, true, false, true);
    }
    else
    {
        new Float:fx, Float:fy, Float:fz;
        GetPlayerCameraFrontVector(playerid, fx, fy, fz);
        GetPlayerCameraPos(playerid, x, y, z);

        Fly_LookAt(playerid, fx * 500.0 + x, fy * 500.0 + y);

        if (GetPlayerAnimationIndex(playerid) != 959)
            ApplyAnimation(playerid, "PARACHUTE", "FALL_SkyDive_Accel", 6.1, true, true, true, true, false, true);
    }

    if (gPlayerFlying[playerid])
        SetTimerEx("Fly_Update", 50, false, "i", playerid);

    return true;
}


static stock Fly_LookAt(playerid, Float:x, Float:y)
{
    new Float:px, Float:py, Float:angle;
    GetPlayerPos(playerid, px, py, angle);

    angle = floatabs(atan((y - py) / (x - px)));

    if (x <= px && y >= py) angle = 180.0 - angle;
    else if (x < px && y < py) angle += 180.0;
    else if (x >= px && y <= py) angle = 360.0 - angle;

    angle -= 90.0;

    if (angle >= 360.0)
        angle -= 360.0;

    SetPlayerFacingAngle(playerid, angle);
    return true;
}
