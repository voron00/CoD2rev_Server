#include "../qcommon/qcommon.h"
#include "g_shared.h"

scr_method_t player_methods[] =
{
	{ "giveweapon", PlayerCmd_giveWeapon, qfalse, },
	{ "takeweapon", PlayerCmd_takeWeapon, qfalse, },
	{ "takeallweapons", PlayerCmd_takeAllWeapons, qfalse, },
	{ "getcurrentweapon", PlayerCmd_getCurrentWeapon, qfalse, },
	{ "getcurrentoffhand", PlayerCmd_getCurrentOffhand, qfalse, },
	{ "hasweapon", PlayerCmd_hasWeapon, qfalse, },
	{ "switchtoweapon", PlayerCmd_switchToWeapon, qfalse, },
	{ "switchtooffhand", PlayerCmd_switchToOffhand, qfalse, },
	{ "givestartammo", PlayerCmd_giveStartAmmo, qfalse, },
	{ "givemaxammo", PlayerCmd_giveMaxAmmo, qfalse, },
	{ "getfractionstartammo", PlayerCmd_getFractionStartAmmo, qfalse, },
	{ "getfractionmaxammo", PlayerCmd_getFractionMaxAmmo, qfalse, },
	{ "setorigin", PlayerCmd_setOrigin, qfalse, },
	{ "setplayerangles", PlayerCmd_setAngles, qfalse, },
	{ "getplayerangles", PlayerCmd_getAngles, qfalse, },
	{ "usebuttonpressed", PlayerCmd_useButtonPressed, qfalse, },
	{ "attackbuttonpressed", PlayerCmd_attackButtonPressed, qfalse, },
	{ "meleebuttonpressed", PlayerCmd_meleeButtonPressed, qfalse, },
	{ "playerads", PlayerCmd_playerADS, qfalse, },
	{ "isonground", PlayerCmd_isOnGround, qfalse, },
	{ "pingplayer", PlayerCmd_pingPlayer, qfalse, },
	{ "setviewmodel", PlayerCmd_SetViewmodel, qfalse, },
	{ "getviewmodel", PlayerCmd_GetViewmodel, qfalse, },
	{ "sayall", PlayerCmd_SayAll, qfalse, },
	{ "sayteam", PlayerCmd_SayTeam, qfalse, },
	{ "showscoreboard", PlayerCmd_showScoreboard, qfalse, },
	{ "setspawnweapon", PlayerCmd_setSpawnWeapon, qfalse, },
	{ "dropitem", PlayerCmd_dropItem, qfalse, },
	{ "finishplayerdamage", PlayerCmd_finishPlayerDamage, qfalse, },
	{ "suicide", PlayerCmd_Suicide, qfalse, },
	{ "openmenu", PlayerCmd_OpenMenu, qfalse, },
	{ "openmenunomouse", PlayerCmd_OpenMenuNoMouse, qfalse, },
	{ "closemenu", PlayerCmd_CloseMenu, qfalse, },
	{ "closeingamemenu", PlayerCmd_CloseInGameMenu, qfalse, },
	{ "freezecontrols", PlayerCmd_FreezeControls, qfalse, },
	{ "disableweapon", PlayerCmd_DisableWeapon, qfalse, },
	{ "enableweapon", PlayerCmd_EnableWeapon, qfalse, },
	{ "setreverb", PlayerCmd_SetReverb, qfalse, },
	{ "deactivatereverb", PlayerCmd_DeactivateReverb, qfalse, },
	{ "setchannelvolumes", PlayerCmd_SetChannelVolumes, qfalse, },
	{ "deactivatechannelvolumes", PlayerCmd_DeactivateChannelVolumes, qfalse, },
	{ "getweaponslotweapon", PlayerCmd_GetWeaponSlotWeapon, qfalse, },
	{ "setweaponslotweapon", PlayerCmd_SetWeaponSlotWeapon, qfalse, },
	{ "getweaponslotammo", PlayerCmd_GetWeaponSlotAmmo, qfalse, },
	{ "setweaponslotammo", PlayerCmd_SetWeaponSlotAmmo, qfalse, },
	{ "getweaponslotclipammo", PlayerCmd_GetWeaponSlotClipAmmo, qfalse, },
	{ "setweaponslotclipammo", PlayerCmd_SetWeaponSlotClipAmmo, qfalse, },
	{ "setweaponclipammo", PlayerCmd_SetWeaponClipAmmo, qfalse, },
	{ "iprintln", iclientprintln, qfalse, },
	{ "iprintlnbold", iclientprintlnbold, qfalse, },
	{ "spawn", PlayerCmd_spawn, qfalse, },
	{ "setentertime", PlayerCmd_setEnterTime, qfalse, },
	{ "cloneplayer", PlayerCmd_ClonePlayer, qfalse, },
	{ "setclientcvar", PlayerCmd_SetClientDvar, qfalse, },
	{ "islookingat", ScrCmd_IsLookingAt, qfalse, },
	{ "playlocalsound", ScrCmd_PlayLocalSound, qfalse, },
	{ "istalking", PlayerCmd_IsTalking, qfalse, },
	{ "allowspectateteam", PlayerCmd_AllowSpectateTeam, qfalse, },
	{ "getguid", PlayerCmd_GetGuid, qfalse, }
};

/*
===============
Player_GetMethod
===============
*/
void (*Player_GetMethod( const char **pName ))( scr_entref_t )
{
	for ( int i = 0; i < ARRAY_COUNT( player_methods ); i++ )
	{
		if ( !strcmp(*pName, player_methods[i].name) )
		{
			*pName = player_methods[i].name;
			return player_methods[i].call;
		}
	}

	return NULL;
}

/*
===============
BodyEnd
===============
*/
void BodyEnd( gentity_t *ent )
{
	ent->s.eFlags &= ~EF_BODY_START;
	ent->r.contents = CONTENTS_CORPSE;
	ent->r.svFlags = 0;
}

/*
===============
PlayerCmd_pingPlayer
===============
*/
void PlayerCmd_pingPlayer( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pSelf->client->ps.eFlags |= EF_TAUNT;
	pSelf->client->compassPingTime = level.time + 3000;
}

/*
===============
PlayerCmd_pingPlayer
===============
*/
void PlayerCmd_takeAllWeapons( scr_entref_t entref )
{
	gentity_t *pSelf;
	int weapIndex;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pSelf->client->ps.weapon = WP_NONE;

	for ( weapIndex = 1; weapIndex <= BG_GetNumWeapons(); weapIndex++ )
	{
		pSelf->client->ps.ammo[BG_AmmoForWeapon(weapIndex)] = 0;
		pSelf->client->ps.ammoclip[BG_ClipForWeapon(weapIndex)] = 0;

		BG_TakePlayerWeapon(&pSelf->client->ps, weapIndex);
	}
}

/*
===============
PlayerCmd_isOnGround
===============
*/
void PlayerCmd_isOnGround( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_AddBool( pSelf->client->ps.groundEntityNum != ENTITYNUM_NONE );
}

/*
===============
PlayerCmd_playerADS
===============
*/
void PlayerCmd_playerADS( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_AddFloat( pSelf->client->ps.fWeaponPosFrac );
}

/*
===============
PlayerCmd_meleeButtonPressed
===============
*/
void PlayerCmd_meleeButtonPressed( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_AddBool( ( pSelf->client->buttonsSinceLastFrame | pSelf->client->buttons ) & BUTTON_MELEE ? true : false );
}

/*
===============
PlayerCmd_attackButtonPressed
===============
*/
void PlayerCmd_attackButtonPressed( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_AddBool( ( pSelf->client->buttonsSinceLastFrame | pSelf->client->buttons ) & BUTTON_ATTACK ? true : false );
}

/*
===============
PlayerCmd_useButtonPressed
===============
*/
void PlayerCmd_useButtonPressed( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_AddBool( ( pSelf->client->buttonsSinceLastFrame | pSelf->client->buttons ) & ( BUTTON_USE | BUTTON_USERELOAD ) ? true : false );
}

/*
===============
PlayerCmd_hasWeapon
===============
*/
void PlayerCmd_hasWeapon( scr_entref_t entref )
{
	gentity_t *pSelf;
	int iWeaponIndex;
	const char *pszWeaponName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = BG_FindWeaponIndexForName(pszWeaponName);

	Scr_AddBool( iWeaponIndex && Com_BitCheck( pSelf->client->ps.weapons, iWeaponIndex ) );
}

/*
===============
ClientPlaying
===============
*/
static qboolean ClientPlaying( gentity_t *pSelf )
{
	assert(pSelf->client);
	assert(pSelf->client->sess.connected != CON_DISCONNECTED);

	return pSelf->client->sess.sessionState == SESS_STATE_PLAYING;
}

/*
===============
PlayerCmd_getCurrentOffhand
===============
*/
void PlayerCmd_getCurrentOffhand( scr_entref_t entref )
{
	gentity_t *pSelf;
	WeaponDef *weapDef;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	if ( ClientPlaying(pSelf) && pSelf->client->ps.offHandIndex > 0 )
	{
		weapDef = BG_GetWeaponDef(pSelf->client->ps.offHandIndex);
		Scr_AddString(weapDef->szInternalName);
	}
	else
	{
		Scr_AddString("none");
	}
}

/*
===============
PlayerCmd_getCurrentWeapon
===============
*/
void PlayerCmd_getCurrentWeapon( scr_entref_t entref )
{
	gentity_t *pSelf;
	WeaponDef *weapDef;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	if ( ClientPlaying(pSelf) && pSelf->client->ps.weapon > 0 )
	{
		weapDef = BG_GetWeaponDef(pSelf->client->ps.weapon);
		Scr_AddString(weapDef->szInternalName);
	}
	else
	{
		Scr_AddString("none");
	}
}

/*
===============
PlayerCmd_getFractionMaxAmmo
===============
*/
void PlayerCmd_getFractionMaxAmmo( scr_entref_t entref )
{
	gentity_t *pSelf;
	float fAmmoFrac;
	int iWeaponIndex;
	const char *pszWeaponName;
	WeaponDef *weapDef;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( !Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		Scr_AddFloat(1.0f);
		return;
	}

	weapDef = BG_GetWeaponDef(iWeaponIndex);

	if ( BG_GetAmmoTypeMax(weapDef->ammoIndex) <= 0 )
	{
		Scr_AddFloat(1.0f);
		return;
	}

	if ( pSelf->client->ps.ammo[weapDef->ammoIndex] <= 0 )
	{
		Scr_AddFloat(0.0f);
		return;
	}

	fAmmoFrac = (float)pSelf->client->ps.ammo[weapDef->ammoIndex] / (float)BG_GetAmmoTypeMax(weapDef->ammoIndex);
	Scr_AddFloat(fAmmoFrac);
}

/*
===============
PlayerCmd_getFractionStartAmmo
===============
*/
void PlayerCmd_getFractionStartAmmo( scr_entref_t entref )
{
	gentity_t *pSelf;
	float fAmmoFrac;
	int iWeaponIndex;
	const char *pszWeaponName;
	WeaponDef *weapDef;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( !Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		Scr_AddFloat(1.0f);
		return;
	}

	weapDef = BG_GetWeaponDef(iWeaponIndex);

	if ( weapDef->startAmmo <= 0 )
	{
		Scr_AddFloat(1.0f);
		return;
	}

	if ( pSelf->client->ps.ammo[weapDef->ammoIndex] <= 0 )
	{
		Scr_AddFloat(0.0f);
		return;
	}

	fAmmoFrac = (float)pSelf->client->ps.ammo[weapDef->ammoIndex] / (float)weapDef->startAmmo;
	Scr_AddFloat(fAmmoFrac);
}

/*
===============
PlayerCmd_giveMaxAmmo
===============
*/
void PlayerCmd_giveMaxAmmo( scr_entref_t entref )
{
	gentity_t *pSelf;
	int ammoGive;
	int iWeaponIndex;
	const char *pszWeaponName;
	WeaponDef *weapDef;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( !Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		return;
	}

	weapDef = BG_GetWeaponDef(iWeaponIndex);
	ammoGive = BG_GetAmmoTypeMax(weapDef->ammoIndex) - pSelf->client->ps.ammo[weapDef->ammoIndex];

	if ( ammoGive > 0 )
	{
		Add_Ammo(pSelf, iWeaponIndex, ammoGive, qfalse);
	}
}

/*
===============
PlayerCmd_giveStartAmmo
===============
*/
void PlayerCmd_giveStartAmmo( scr_entref_t entref )
{
	gentity_t *pSelf;
	int ammoGive;
	int iWeaponIndex;
	const char *pszWeaponName;
	WeaponDef *weapDef;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( !Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		return;
	}

	weapDef = BG_GetWeaponDef(iWeaponIndex);
	ammoGive = weapDef->startAmmo - pSelf->client->ps.ammo[weapDef->ammoIndex];

	if ( ammoGive > 0 )
	{
		Add_Ammo(pSelf, iWeaponIndex, ammoGive, qfalse);
	}
}

/*
===============
PlayerCmd_switchToOffhand
===============
*/
void PlayerCmd_switchToOffhand( scr_entref_t entref )
{
	gentity_t *pSelf;
	int iWeaponIndex;
	const char *pszWeaponName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( !iWeaponIndex )
	{
		Scr_ParamError(0, va("unknown weapon '%s'", pszWeaponName));
		return;
	}

	if ( !Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		Scr_AddBool(false);
		return;
	}

	G_SetEquippedOffHand(entref.entnum, iWeaponIndex);
	Scr_AddBool(true);
}

/*
===============
PlayerCmd_switchToWeapon
===============
*/
void PlayerCmd_switchToWeapon( scr_entref_t entref )
{
	gentity_t *pSelf;
	int iWeaponIndex;
	const char *pszWeaponName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( !iWeaponIndex )
	{
		Scr_ParamError(0, va("unknown weapon '%s'", pszWeaponName));
		return;
	}

	if ( !Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		Scr_AddBool(false);
		return;
	}

	G_SelectWeaponIndex(entref.entnum, iWeaponIndex);
	Scr_AddBool(true);
}

/*
===============
PlayerCmd_takeWeapon
===============
*/
void PlayerCmd_takeWeapon( scr_entref_t entref )
{
	gentity_t *pSelf;
	int iWeaponIndex;
	const char *pszWeaponName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	pSelf->client->ps.ammo[BG_AmmoForWeapon(iWeaponIndex)] = 0;
	pSelf->client->ps.ammoclip[BG_ClipForWeapon(iWeaponIndex)] = 0;

	BG_TakePlayerWeapon(&pSelf->client->ps, iWeaponIndex);
}

/*
===============
PlayerCmd_giveWeapon
===============
*/
void PlayerCmd_giveWeapon( scr_entref_t entref )
{
	gentity_t *pSelf;
	int hadWeapon;
	int ammoGive;
	int iWeaponIndex;
	const char *pszWeaponName;
	WeaponDef *weaponDef;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	hadWeapon = Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex);
	weaponDef = BG_GetWeaponDef(iWeaponIndex);

	if ( BG_DoesWeaponNeedSlot(iWeaponIndex) && !BG_GetEmptySlotForWeapon(&pSelf->client->ps, iWeaponIndex) )
	{
		Scr_ParamError(0, va("Cannot give %s weapon %s without having an empty weapon slot - player currently has a %s and a %s\n",
		                     pSelf->client->sess.state.name,
		                     weaponDef->displayName,
		                     BG_GetWeaponDef(pSelf->client->ps.weaponslots[SLOT_PRIMARY])->displayName,
		                     BG_GetWeaponDef(pSelf->client->ps.weaponslots[SLOT_PRIMARYB])->displayName));

		return;
	}

	if ( G_GivePlayerWeapon(&pSelf->client->ps, iWeaponIndex) )
	{
		SV_GameSendServerCommand(pSelf - g_entities, SV_CMD_CAN_IGNORE, va("%c \"%i\"", 73, 1));
	}

	ammoGive = weaponDef->startAmmo - pSelf->client->ps.ammo[weaponDef->ammoIndex];

	if ( ammoGive > 0 )
	{
		Add_Ammo(pSelf, iWeaponIndex, ammoGive, hadWeapon == qfalse);
	}
}

/*
===============
PlayerCmd_EnableWeapon
===============
*/
void PlayerCmd_EnableWeapon( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pSelf->client->ps.pm_flags &= ~PMF_DISABLEWEAPON;
}

/*
===============
PlayerCmd_DisableWeapon
===============
*/
void PlayerCmd_DisableWeapon( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pSelf->client->ps.pm_flags |= PMF_DISABLEWEAPON;
}

/*
===============
PlayerCmd_FreezeControls
===============
*/
void PlayerCmd_FreezeControls( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pSelf->client->bFrozen = Scr_GetInt(0) ? qtrue : qfalse; // fix: boolean value
}

/*
===============
PlayerCmd_setEnterTime
===============
*/
void PlayerCmd_setEnterTime( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pSelf->client->sess.enterTime = Scr_GetInt(0);
}

/*
===============
PlayerCmd_setAngles
===============
*/
void PlayerCmd_setAngles( scr_entref_t entref )
{
	gentity_t *pSelf;
	vec3_t angles;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_GetVector(0, angles);
	SetClientViewAngle(pSelf, angles);
}

/*
===============
PlayerCmd_CloseInGameMenu
===============
*/
void PlayerCmd_CloseInGameMenu( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c", 75));
}

/*
===============
PlayerCmd_CloseMenu
===============
*/
void PlayerCmd_CloseMenu( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c", 117));
}

/*
===============
PlayerCmd_showScoreboard
===============
*/
void PlayerCmd_showScoreboard( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Cmd_Score_f(pSelf);
}

/*
===============
PlayerCmd_AllowSpectateTeam
===============
*/
void PlayerCmd_AllowSpectateTeam( scr_entref_t entref )
{
	gentity_t *pSelf;
	int teamBit = 0;
	unsigned short teamString;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	teamString = Scr_GetConstString(0);

	if ( teamString == scr_const.none )
	{
		teamBit = 1;
	}
	else if ( teamString == scr_const.axis )
	{
		teamBit = 2;
	}
	else if ( teamString == scr_const.allies )
	{
		teamBit = 4;
	}
	else if ( teamString == scr_const.freelook )
	{
		teamBit = 16;
	}
	else
	{
		Scr_ParamError(0, "team must be \"axis\", \"allies\", \"none\", or \"freelook\"");
	}

	if ( Scr_GetInt(1) )
		pSelf->client->sess.noSpectate &= ~teamBit;
	else
		pSelf->client->sess.noSpectate |= teamBit;
}

/*
===============
PlayerCmd_DeactivateChannelVolumes
===============
*/
void PlayerCmd_DeactivateChannelVolumes( scr_entref_t entref )
{
	gentity_t *pSelf;
	unsigned short priorityString;
	int paramNum;
	float fadetime = 0;
	int priority = SND_CHANNELVOLPRIO_NONE;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	paramNum = Scr_GetNumParam();

	if ( paramNum != 1 )
	{
		if ( paramNum != 2 )
		{
			Scr_Error(
			    "USAGE: player deactivatechannelvolumes(\"priority\", fadetime = 0);\n"
			    "Valid priorities are \"snd_channelvolprio_holdbreath\", \"snd_channelvolprio_pain\", or \"snd_channelvolprio_she"
			    "llshock\", fadetime is the time spent fading to the next lowest active reverb priority level in seconds\n");
			return;
		}

		fadetime = Scr_GetFloat(1);
	}

	priorityString = Scr_GetConstString(0);

	if ( priorityString == scr_const.snd_channelvolprio_holdbreath )
	{
		priority = SND_CHANNELVOLPRIO_HOLDBREATH;
	}
	else if ( priorityString == scr_const.snd_channelvolprio_pain )
	{
		priority = SND_CHANNELVOLPRIO_PAIN;
	}
	else if ( priorityString == scr_const.snd_channelvolprio_shellshock )
	{
		priority = SND_CHANNELVOLPRIO_SHELLSHOCK;
	}
	else
	{
		Scr_Error("priority must be \'snd_channelvolprio_holdbreath\', \'snd_channelvolprio_pain\', or \'snd_channelvolprio_shellshock\'\n");
	}

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i %g", 70, priority, fadetime));
}

/*
===============
PlayerCmd_DeactivateReverb
===============
*/
void PlayerCmd_DeactivateReverb( scr_entref_t entref )
{
	gentity_t *pSelf;
	unsigned short priorityString;
	int paramNum;
	float fadetime = 0;
	int priority = SND_ENVEFFECTPRIO_NONE;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	paramNum = Scr_GetNumParam();

	if ( paramNum != 1 )
	{
		if ( paramNum != 2 )
		{
			Scr_Error(
			    "USAGE: player deactivateReverb(\"priority\", fadetime = 0);\n"
			    "Valid priorities are \"snd_enveffectsprio_level\" or \"snd_enveffectsprio_shellshock\", fadetime is the time spe"
			    "nt fading to the next lowest active reverb priority level in seconds\n");
			return;
		}

		fadetime = Scr_GetFloat(1);
	}

	priorityString = Scr_GetConstString(0);

	if ( priorityString == scr_const.snd_enveffectsprio_level )
	{
		priority = SND_ENVEFFECTPRIO_LEVEL;
	}
	else if ( priorityString == scr_const.snd_enveffectsprio_shellshock )
	{
		priority = SND_ENVEFFECTPRIO_SHELLSHOCK;
	}
	else
	{
		Scr_Error("priority must be \'snd_enveffectsprio_level\' or \'snd_enveffectsprio_shellshock\'\n");
	}

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i %g", 68, priority, fadetime));
}

/*
===============
PlayerCmd_SetReverb
===============
*/
void PlayerCmd_SetReverb( scr_entref_t entref )
{
	gentity_t *pSelf;
	int paramNum;
	float fadetime = 0;
	float wetlevel = 0.5f;
	float drylevel = 1.0f;
	int priority = SND_ENVEFFECTPRIO_NONE;
	const char *roomtype;
	unsigned short priorityString;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	paramNum = Scr_GetNumParam();

	if ( paramNum == 3 )
	{
		drylevel = Scr_GetFloat(2);
	}
	else if ( paramNum > 3 )
	{
		if ( paramNum != 4 )
		{
			if ( paramNum != 5 )
			{
				Scr_Error(
				    "USAGE: player setReverb(\"priority\", \"roomtype\", drylevel = 1.0, wetlevel = 0.5, fadetime = 0);\n"
				    "Valid priorities are \"snd_enveffectsprio_level\" or \"snd_enveffectsprio_shellshock\", dry level is a float f"
				    "rom 0 (no source sound) to 1 (full source sound), wetlevel is a float from 0 (no effect) to 1 (full effect), f"
				    "adetime is in sec and modifies drylevel and wetlevel\n");
				return;
			}

			fadetime = Scr_GetFloat(4);
		}

		wetlevel = Scr_GetFloat(3);
		drylevel = Scr_GetFloat(2);
	}
	else if ( paramNum != 2 )
	{
		Scr_Error(
		    "USAGE: player setReverb(\"priority\", \"roomtype\", drylevel = 1.0, wetlevel = 0.5, fadetime = 0);\n"
		    "Valid priorities are \"snd_enveffectsprio_level\" or \"snd_enveffectsprio_shellshock\", dry level is a float f"
		    "rom 0 (no source sound) to 1 (full source sound), wetlevel is a float from 0 (no effect) to 1 (full effect), f"
		    "adetime is in sec and modifies drylevel and wetlevel\n");
		return;
	}

	roomtype = Scr_GetString(1);
	priorityString = Scr_GetConstString(0);

	if ( priorityString == scr_const.snd_enveffectsprio_level )
	{
		priority = SND_ENVEFFECTPRIO_LEVEL;
	}
	else if ( priorityString == scr_const.snd_enveffectsprio_shellshock )
	{
		priority = SND_ENVEFFECTPRIO_SHELLSHOCK;
	}
	else
	{
		Scr_Error("priority must be 'snd_enveffectsprio_level' or 'snd_enveffectsprio_shellshock'\n");
	}

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i \"%s\" %g %g %g", 114, priority, roomtype, drylevel, wetlevel, fadetime));
}

/*
===============
PlayerCmd_SetChannelVolumes
===============
*/
void PlayerCmd_SetChannelVolumes( scr_entref_t entref )
{
	gentity_t *pSelf;
	const char *shockName;
	int paramNum;
	int shockIndex;
	float fadetime = 0;
	int priority = SND_CHANNELVOLPRIO_NONE;
	unsigned short priorityString;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	paramNum = Scr_GetNumParam();

	if ( paramNum != 2 )
	{
		if ( paramNum != 3 )
		{
			Scr_Error(
			    "USAGE: player setchannelvolumes(\"priority\", \"shock name\", fadetime = 0);\n"
			    "Valid priorities are \"snd_channelvolprio_holdbreath\", \"snd_channelvolprio_pain\", or \"snd_channelvolprio_she"
			    "llshock\", fadetime is in sec\n");
			return;
		}

		fadetime = Scr_GetFloat(2);
	}

	shockName = Scr_GetString(1);
	shockIndex = G_FindConfigstringIndex(shockName, CS_SHELLSHOCKS - 1, MAX_SHELLSHOCKS, qfalse, NULL);
	priorityString = Scr_GetConstString(0);

	if ( priorityString == scr_const.snd_channelvolprio_holdbreath )
	{
		priority = SND_CHANNELVOLPRIO_HOLDBREATH;
	}
	else if ( priorityString == scr_const.snd_channelvolprio_pain )
	{
		priority = SND_CHANNELVOLPRIO_PAIN;
	}
	else if ( priorityString == scr_const.snd_channelvolprio_shellshock )
	{
		priority = SND_CHANNELVOLPRIO_SHELLSHOCK;
	}
	else
	{
		Scr_Error("priority must be 'snd_channelvolprio_holdbreath', 'snd_channelvolprio_pain', or 'snd_channelvolprio_shellshock'\n");
	}

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i %i %g", 69, priority, shockIndex, fadetime));
}

/*
===============
PlayerCmd_GetGuid
===============
*/
void PlayerCmd_GetGuid( scr_entref_t entref )
{
	gentity_t *pSelf;
	int guid;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	if ( Scr_GetNumParam() > 0 )
	{
		Scr_Error("USAGE: self getGuid()\n");
	}

	guid = SV_GetGuid(entref.entnum);
	Scr_AddInt(guid);
}

/*
===============
ScrCmd_PlayLocalSound
===============
*/
void ScrCmd_PlayLocalSound( scr_entref_t entref )
{
	gentity_t *pSelf;
	int soundIndex;
	const char *aliasName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	aliasName = Scr_GetString(0);
	soundIndex = G_SoundAliasIndex(aliasName);

	SV_GameSendServerCommand(entref.entnum, SV_CMD_CAN_IGNORE, va("%c %i", 115, soundIndex));
}

/*
===============
ScrCmd_IsLookingAt
===============
*/
void ScrCmd_IsLookingAt( scr_entref_t entref )
{
	gentity_t *pSelf;
	gentity_t *pOther;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pOther = Scr_GetEntity(0);
	Scr_AddBool( pSelf->client->pLookatEnt == pOther );
}

/*
===============
PlayerCmd_IsTalking
===============
*/
void PlayerCmd_IsTalking( scr_entref_t entref )
{
	gentity_t *pSelf;
	int elapsedTime;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	elapsedTime = level.time - pSelf->client->lastVoiceTime;
	Scr_AddBool( (unsigned)elapsedTime >= g_voiceChatTalkingDuration->current.integer );
}

/*
===============
PlayerCmd_SetWeaponSlotAmmo
===============
*/
void PlayerCmd_SetWeaponSlotAmmo( scr_entref_t entref )
{
	gentity_t *pSelf;
	int clipIndex;
	int ammoIndex;
	int ammoCount;
	int iWeaponIndex;
	int slot;
	unsigned short slotName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	ammoCount = Scr_GetInt(1);
	iWeaponIndex = pSelf->client->ps.weaponslots[slot];

	if ( !iWeaponIndex )
	{
		return;
	}

	if ( BG_WeaponIsClipOnly(iWeaponIndex) )
	{
		clipIndex = BG_ClipForWeapon(iWeaponIndex);

		if ( !clipIndex )
		{
			return;
		}

		if ( ammoCount >= 0 )
		{
			if ( ammoCount > BG_GetAmmoClipSize(clipIndex) )
				ammoCount = BG_GetAmmoClipSize(clipIndex);
		}
		else
		{
			ammoCount = 0;
		}

		pSelf->client->ps.ammoclip[clipIndex] = ammoCount;
	}
	else
	{
		ammoIndex = BG_AmmoForWeapon(iWeaponIndex);

		if ( !ammoIndex )
		{
			return;
		}

		if ( ammoCount >= 0 )
		{
			if ( ammoCount > BG_GetAmmoTypeMax(ammoIndex) )
				ammoCount = BG_GetAmmoTypeMax(ammoIndex);
		}
		else
		{
			ammoCount = 0;
		}

		pSelf->client->ps.ammo[ammoIndex] = ammoCount;
	}
}

/*
===============
PlayerCmd_SetWeaponSlotClipAmmo
===============
*/
void PlayerCmd_SetWeaponSlotClipAmmo( scr_entref_t entref )
{
	gentity_t *pSelf;
	int clipIndex;
	int ammoCount;
	int iWeaponIndex;
	int slot;
	unsigned short slotName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	ammoCount = Scr_GetInt(1);
	iWeaponIndex = pSelf->client->ps.weaponslots[slot];

	if ( !iWeaponIndex )
	{
		Scr_AddInt(0);
		return;
	}

	clipIndex = BG_ClipForWeapon(pSelf->client->ps.weaponslots[slot]);

	if ( !clipIndex )
	{
		Scr_AddInt(0);
		return;
	}

	if ( ammoCount < 0 )
		ammoCount = 0;

	if ( ammoCount > BG_GetAmmoClipSize(clipIndex) )
		ammoCount = BG_GetAmmoClipSize(clipIndex);

	pSelf->client->ps.ammoclip[clipIndex] = ammoCount;
}

/*
===============
PlayerCmd_GetWeaponSlotClipAmmo
===============
*/
void PlayerCmd_GetWeaponSlotClipAmmo( scr_entref_t entref )
{
	gentity_t *pSelf;
	int clipIndex;
	int iWeaponIndex;
	int slot;
	unsigned short slotName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	if ( !ClientPlaying(pSelf) )
	{
		Scr_AddInt(0);
		return;
	}

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	iWeaponIndex = pSelf->client->ps.weaponslots[slot];

	if ( !iWeaponIndex )
	{
		Scr_AddInt(0);
		return;
	}

	clipIndex = BG_ClipForWeapon(pSelf->client->ps.weaponslots[slot]);

	if ( !clipIndex )
	{
		Scr_AddInt(0);
		return;
	}

	Scr_AddInt(pSelf->client->ps.ammoclip[clipIndex]);
}

/*
===============
PlayerCmd_GetWeaponSlotAmmo
===============
*/
void PlayerCmd_GetWeaponSlotAmmo( scr_entref_t entref )
{
	gentity_t *pSelf;
	int slot;
	int iWeaponIndex;
	unsigned short slotName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	if ( !ClientPlaying(pSelf) )
	{
		Scr_AddInt(0);
		return;
	}

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	iWeaponIndex = pSelf->client->ps.weaponslots[slot];

	if ( !iWeaponIndex )
	{
		Scr_AddInt(0);
		return;
	}

	if ( BG_WeaponIsClipOnly(iWeaponIndex) )
	{
		Scr_AddInt(pSelf->client->ps.ammoclip[BG_ClipForWeapon(iWeaponIndex)]);
	}
	else
	{
		Scr_AddInt(pSelf->client->ps.ammo[BG_AmmoForWeapon(iWeaponIndex)]);
	}
}

/*
===============
PlayerCmd_GetWeaponSlotWeapon
===============
*/
void PlayerCmd_GetWeaponSlotWeapon( scr_entref_t entref )
{
	gentity_t *pSelf;
	WeaponDef *weapDef;
	int iWeaponIndex;
	int slot;
	unsigned short slotName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	if ( !ClientPlaying(pSelf) )
	{
		Scr_AddConstString(scr_const.none);
		return;
	}

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	iWeaponIndex = pSelf->client->ps.weaponslots[slot];

	if ( !iWeaponIndex )
	{
		Scr_AddConstString(scr_const.none);
		return;
	}

	weapDef = BG_GetWeaponDef(pSelf->client->ps.weaponslots[slot]);
	Scr_AddString(weapDef->szInternalName);
}

/*
===============
PlayerCmd_OpenMenuNoMouse
===============
*/
void PlayerCmd_OpenMenuNoMouse( scr_entref_t entref )
{
	gentity_t *pSelf;
	const char *menuName;
	int iMenuIndex;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	if ( pSelf->client->sess.connected != CON_CONNECTED )
	{
		Scr_AddInt(0);
		return;
	}

	menuName = Scr_GetString(0);
	iMenuIndex = GScr_GetScriptMenuIndex(menuName);

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i 1", 116, iMenuIndex));
	Scr_AddInt(1);
}

/*
===============
PlayerCmd_OpenMenu
===============
*/
void PlayerCmd_OpenMenu( scr_entref_t entref )
{
	gentity_t *pSelf;
	const char *menuName;
	int iMenuIndex;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	if ( pSelf->client->sess.connected != CON_CONNECTED )
	{
		Scr_AddInt(0);
		return;
	}

	menuName = Scr_GetString(0);
	iMenuIndex = GScr_GetScriptMenuIndex(menuName);

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i", 116, iMenuIndex));
	Scr_AddInt(1);
}

/*
===============
PlayerCmd_GetViewmodel
===============
*/
void PlayerCmd_GetViewmodel( scr_entref_t entref )
{
	gentity_t *pSelf;
	const char *modelName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	modelName = G_ModelName(pSelf->client->sess.viewmodelIndex);
	Scr_AddString(modelName);
}

/*
===============
PlayerCmd_SetViewmodel
===============
*/
void PlayerCmd_SetViewmodel( scr_entref_t entref )
{
	gentity_t *pSelf;
	const char *modelName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	modelName = Scr_GetString(0);

	if ( !modelName || !modelName[0] )
		Scr_ParamError(0, "usage: setviewmodel(<model name>)");

	pSelf->client->sess.viewmodelIndex = G_ModelIndex(modelName);
}

/*
===============
PlayerCmd_getAngles
===============
*/
void PlayerCmd_getAngles( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_AddVector(pSelf->client->ps.viewangles);
}

/*
===============
PlayerCmd_setOrigin
===============
*/
void PlayerCmd_setOrigin( scr_entref_t entref )
{
	gentity_t *pSelf;
	vec3_t vNewOrigin;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_GetVector(0, vNewOrigin);

	SV_UnlinkEntity(pSelf);
	VectorCopy(vNewOrigin, pSelf->client->ps.origin);

	pSelf->client->ps.origin[2] += 1.0f;
	pSelf->client->ps.eFlags ^= EF_TELEPORT_BIT;

	BG_PlayerStateToEntityState(&pSelf->client->ps, &pSelf->s, qtrue, PMOVE_HANDLER_SERVER);

	VectorCopy(pSelf->client->ps.origin, pSelf->r.currentOrigin);
	SV_LinkEntity(pSelf);
}

/*
===============
PlayerCmd_SetWeaponClipAmmo
===============
*/
void PlayerCmd_SetWeaponClipAmmo( scr_entref_t entref )
{
	gentity_t *pSelf;
	int clipIndex;
	int ammoCount;
	int weaponIndex;
	const char *weapName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	weapName = Scr_GetString(0);
	ammoCount = Scr_GetInt(1);

	weaponIndex = G_GetWeaponIndexForName(weapName);

	if ( !weaponIndex )
	{
		Scr_AddInt(0);
		return;
	}

	clipIndex = BG_ClipForWeapon(weaponIndex);

	if ( !clipIndex )
	{
		Scr_AddInt(0);
		return;
	}

	if ( ammoCount < 0 )
		ammoCount = 0;

	if ( ammoCount > BG_GetAmmoClipSize(clipIndex) )
		ammoCount = BG_GetAmmoClipSize(clipIndex);

	pSelf->client->ps.ammoclip[clipIndex] = ammoCount;
}

/*
===============
PlayerCmd_SetWeaponSlotWeapon
===============
*/
void PlayerCmd_SetWeaponSlotWeapon( scr_entref_t entref )
{
	gentity_t *pSelf;
	qboolean isOnlyWeapon = qfalse;
	qboolean hadWeapon = qfalse;
	WeaponDef *weapDef;
	const char *pszWeaponName;
	int ammoCount;
	int iWeaponIndex;
	int slot;
	unsigned short slotName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	pszWeaponName = Scr_GetString(1);

	if ( !I_stricmp(pszWeaponName, "none") )
	{
		iWeaponIndex = WP_NONE;
	}
	else
	{
		iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

		if ( !iWeaponIndex )
		{
			Scr_ParamError(1, va("Unknown weapon %s.", pszWeaponName));
		}

		weapDef = BG_GetWeaponDef(iWeaponIndex);

		if ( weapDef->weaponSlot != slot )
		{
			if ( weapDef->weaponSlot != SLOT_PRIMARY && weapDef->weaponSlot != SLOT_PRIMARYB || slot != SLOT_PRIMARY && slot != SLOT_PRIMARYB )
			{
				Scr_ParamError(1, va("Weapon %s goes in the %s weaponslot, not the %s weaponslot.", pszWeaponName, BG_GetWeaponSlotNameForIndex(weapDef->weaponSlot), BG_GetWeaponSlotNameForIndex(slot)));
			}
		}
	}

	if ( pSelf->client->ps.weaponslots[slot] )
	{
		BG_TakePlayerWeapon(&pSelf->client->ps, pSelf->client->ps.weaponslots[slot]);
	}

	if ( !iWeaponIndex )
	{
		return;
	}

	if ( slot == SLOT_PRIMARYB && !pSelf->client->ps.weaponslots[SLOT_PRIMARY] )
	{
		isOnlyWeapon = qtrue;
	}

	hadWeapon = Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex);
	G_GivePlayerWeapon(&pSelf->client->ps, iWeaponIndex);

	if ( isOnlyWeapon )
	{
		pSelf->client->ps.weaponslots[SLOT_PRIMARYB] = pSelf->client->ps.weaponslots[SLOT_PRIMARY];
		pSelf->client->ps.weaponslots[SLOT_PRIMARY]  = WP_NONE;
	}

	ammoCount = weapDef->startAmmo - pSelf->client->ps.ammo[weapDef->ammoIndex];

	if ( ammoCount > 0 )
	{
		Add_Ammo(pSelf, iWeaponIndex, ammoCount, hadWeapon == qfalse);
	}
}

/*
===============
PlayerCmd_setSpawnWeapon
===============
*/
void PlayerCmd_setSpawnWeapon( scr_entref_t entref )
{
	gentity_t *pSelf;
	int iWeaponIndex;
	const char *pszWeaponName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( !BG_IsWeaponValid(&pSelf->client->ps, iWeaponIndex) )
	{
		return;
	}

	pSelf->client->ps.weapon = iWeaponIndex;
	pSelf->client->ps.weaponstate = WEAPON_READY;
}

/*
===============
PlayerCmd_ClonePlayer
===============
*/
void PlayerCmd_ClonePlayer( scr_entref_t entref )
{
	gentity_t *pSelf;
	int i;
	XAnimTree *tree;
	corpseInfo_t *corpseInfo;
	int corpseIndex;
	DObj *obj;
	gclient_t *client;
	gentity_t *body;
	int deathAnimDuration;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	deathAnimDuration = Scr_GetInt(0);

	client = pSelf->client;
	assert(client);
	assert(client->sess.connected != CON_DISCONNECTED);

	body = G_SpawnPlayerClone();

	body->s.clientNum = client->ps.clientNum;
	body->s.eFlags = client->ps.eFlags & ~EF_TELEPORT_BIT | body->s.eFlags & EF_TELEPORT_BIT | ( EF_DEAD | EF_BODY_START );

	G_SetOrigin(body, client->ps.origin);
	G_SetAngle(body, pSelf->r.currentAngles);

	body->s.pos.trType = TR_GRAVITY;
	body->s.pos.trTime = level.time;

	VectorCopy(client->ps.velocity, body->s.pos.trDelta);
	assert(!IS_NAN((body->s.pos.trDelta)[0]) && !IS_NAN((body->s.pos.trDelta)[1]) && !IS_NAN((body->s.pos.trDelta)[2]));

	body->s.eType = ET_PLAYER_CORPSE;
	body->physicsObject = qtrue;

	obj = Com_GetServerDObj(client->ps.clientNum);
	tree = DObjGetTree(obj);

	for ( i = 0; i < 2; ++i )
	{
		if ( body->s.pos.trDelta[i] > g_clonePlayerMaxVelocity->current.decimal )
		{
			body->s.pos.trDelta[i] = g_clonePlayerMaxVelocity->current.decimal;
		}
	}
	assert(!IS_NAN((body->s.pos.trDelta)[0]) && !IS_NAN((body->s.pos.trDelta)[1]) && !IS_NAN((body->s.pos.trDelta)[2]));

	body->corpse.deathAnimStartTime = level.time;

	corpseIndex = G_GetFreePlayerCorpseIndex();
	corpseInfo = &g_scr_data.playerCorpseInfo[corpseIndex];

	g_scr_data.playerCorpseInfo[corpseIndex].entnum = body->s.number;

	corpseInfo->time = level.time;
	corpseInfo->falling = qtrue;

	assert(client->ps.clientNum >= 0 && client->ps.clientNum < MAX_CLIENTS);

	corpseInfo->ci = level_bgs.clientinfo[client->ps.clientNum];
	corpseInfo->ci.pXAnimTree = corpseInfo->tree;

	XAnimCloneAnimTree(tree, g_scr_data.playerCorpseInfo[corpseIndex].tree);

	body->s.groundEntityNum = ENTITYNUM_NONE;
	assert(!body->r.svFlags);
	body->r.svFlags = SVF_BODY;

	VectorCopy(pSelf->r.mins, body->r.mins);
	VectorCopy(pSelf->r.maxs, body->r.maxs);

	VectorCopy(pSelf->r.absmin, body->r.absmin);
	VectorCopy(pSelf->r.absmax, body->r.absmax);

	body->s.legsAnim = client->ps.legsAnim;
	body->s.torsoAnim = client->ps.torsoAnim;

	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	body->r.contents = CONTENTS_CLIPSHOT | CONTENTS_CORPSE;

	SV_LinkEntity(body);

	body->nextthink = level.time + deathAnimDuration;
	body->handler = ENT_HANDLER_PLAYER_CLONE;

	GScr_AddEntity(body);
}

/*
===============
PlayerCmd_SayTeam
===============
*/
void PlayerCmd_SayTeam( scr_entref_t entref )
{
	gentity_t *pSelf;
	char szString[MAX_STRING_CHARS];

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_ConstructMessageString(0, Scr_GetNumParam() - 1, "Client Chat Message", &szString[1], sizeof(szString) - 1);
	szString[0] = 20;

	G_Say(pSelf, NULL, SAY_TEAM, szString);
}

/*
===============
PlayerCmd_SayAll
===============
*/
void PlayerCmd_SayAll( scr_entref_t entref )
{
	gentity_t *pSelf;
	char szString[MAX_STRING_CHARS];

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_ConstructMessageString(0, Scr_GetNumParam() - 1, "Client Chat Message", &szString[1], sizeof(szString) - 1);
	szString[0] = 20;

	G_Say(pSelf, NULL, SAY_ALL, szString);
}

/*
===============
PlayerCmd_SetClientDvar
===============
*/
void PlayerCmd_SetClientDvar( scr_entref_t entref )
{
	gentity_t *pSelf;
	char szOutString[MAX_STRING_CHARS];
	char szString[MAX_STRING_CHARS];
	char *pCh;
	char *pszText;
	const char *pszDvar;
	int i;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pszDvar = Scr_GetString(0);

	if ( Scr_GetType(1) == VAR_ISTRING )
	{
		Scr_ConstructMessageString(1, Scr_GetNumParam() - 1, "Client Dvar Value", szString, sizeof(szString));
		pszText = szString;
	}
	else
	{
		pszText = (char *)Scr_GetString(1);
	}

	if ( !Dvar_IsValidName(pszDvar) )
	{
		Scr_Error(va("Dvar %s has an invalid dvar name", pszDvar));
		return;
	}

	pCh = szOutString;
	memset(szOutString, 0, sizeof(szOutString));

	for ( i = 0; i < sizeof(szOutString) - 1; i++, pCh++ )
	{
		if ( !pszText[i] )
		{
			break;
		}

		*pCh = I_CleanChar(pszText[i]);

		if ( *pCh == '\"' )
		{
			*pCh = '\'';
		}
	}

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %s \"%s\"", 118, pszDvar, szOutString));
}

/*
===============
iclientprintlnbold
===============
*/
void iclientprintlnbold( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_MakeGameMessage(entref.entnum, va("%c", 103));
}

/*
===============
iclientprintln
===============
*/
void iclientprintln( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_MakeGameMessage(entref.entnum, va("%c", 102));
}

/*
===============
ApplyKnockBack
===============
*/
static void ApplyKnockBack( gentity_t *pSelf, int damage, vec3_t localdir, int dflags )
{
	vec3_t kvel;
	int knockback;
	int t;
	float knockbackMod;

	if ( pSelf->flags & FL_NO_KNOCKBACK )
	{
		return;
	}

	if ( dflags & DFLAGS_NO_KNOCKBACK )
	{
		return;
	}

	// figure momentum add, even if the damage won't be taken
	knockbackMod = 0.30000001;

	if ( pSelf->client->ps.pm_flags & PMF_PRONE )
	{
		knockbackMod = 0.02;
	}
	else if ( pSelf->client->ps.pm_flags & PMF_DUCKED )
	{
		knockbackMod = 0.15000001;
	}

	knockback = (int)(damage * knockbackMod);

	if ( knockback > 60 )
	{
		knockback = 60;
	}

	if ( !knockback )
	{
		return;
	}

	if ( pSelf->client->ps.eFlags & EF_TURRET_ACTIVE )
	{
		return;
	}

	VectorScale(localdir, knockback * g_knockback->current.decimal / 250, kvel);
	VectorAdd(pSelf->client->ps.velocity, kvel, pSelf->client->ps.velocity);

	if ( pSelf->client->ps.pm_time )
	{
		return;
	}

	// set the timer so that the other client can't cancel
	// out the movement immediately
	t = knockback * 2;

	if ( t < 50 )
	{
		t = 50;
	}
	if ( t > 200 )
	{
		t = 200;
	}
	pSelf->client->ps.pm_time = t;
	pSelf->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
}

/*
===============
PlayerCmd_finishPlayerDamage
===============
*/
void PlayerCmd_finishPlayerDamage( scr_entref_t entref )
{
	const char *mod;
	const char *weapName;
	unsigned short hitLocStr;
	gentity_t *pSelf;
	float flinchYawDir;
	float time_per_point;
	float max_damage_time;
	void (*pain)(gentity_t *, gentity_t *, int, const float *, const int, const float *, int);
	void (*die)(gentity_t *, gentity_t *, gentity_t *, int, int, const int, const float *, int, int);
	gentity_t *tempBulletHitEntity;
	gentity_t *tent;
	int iSurfType;
	vec3_t localdir;
	int hitLoc;
	int iWeapon;
	int modIndex;
	int dflags;
	int damage;
	float *point;
	vec3_t vPoint;
	float *dir;
	vec3_t vDir;
	gentity_t *attacker;
	gentity_t *inflictor;

	inflictor = &g_entities[ENTITYNUM_WORLD];
	attacker = &g_entities[ENTITYNUM_WORLD];

	dir = NULL;
	point = NULL;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	damage = Scr_GetInt(2);

	if ( damage <= 0 )
	{
		return;
	}

	if ( Scr_GetType(0) != VAR_UNDEFINED && Scr_GetPointerType(0) == VAR_ENTITY )
	{
		inflictor = Scr_GetEntity(1);
	}

	if ( Scr_GetType(1) != VAR_UNDEFINED && Scr_GetPointerType(1) == VAR_ENTITY )
	{
		attacker = Scr_GetEntity(1);
	}

	dflags = Scr_GetInt(3);

	mod = Scr_GetString(4);
	modIndex = G_IndexForMeansOfDeath(mod);

	weapName = Scr_GetString(5);
	iWeapon = G_GetWeaponIndexForName(weapName);

	if ( Scr_GetType(6) != VAR_UNDEFINED )
	{
		Scr_GetVector(6, vPoint);
		point = vPoint;
	}

	if ( Scr_GetType(7) != VAR_UNDEFINED )
	{
		Scr_GetVector(7, vDir);
		dir = vDir;
	}

	hitLocStr = Scr_GetConstString(8);
	hitLoc = G_GetHitLocationIndexFromString(hitLocStr);

	iSurfType = Scr_GetInt(9);

	if ( dir )
		Vec3NormalizeTo(dir, localdir);
	else
		VectorClear(localdir);

	ApplyKnockBack(pSelf, damage, localdir, dflags);

	if ( pSelf->flags & FL_GODMODE )
	{
		return;
	}

	if ( iWeapon && BG_GetWeaponDef(iWeapon)->weaponType == WEAPTYPE_BULLET )
	{
		if ( BG_GetWeaponDef(iWeapon)->rifleBullet )
			tempBulletHitEntity = G_TempEntity(vPoint, EV_SHOTGUN_HIT);
		else
			tempBulletHitEntity = G_TempEntity(vPoint, EV_BULLET_HIT_LARGE);

		tempBulletHitEntity->s.eventParm = DirToByte(localdir);
		tempBulletHitEntity->s.scale = DirToByte(localdir);
		tempBulletHitEntity->s.surfType = SURF_TYPE_FLESH;
		tempBulletHitEntity->s.otherEntityNum = attacker->s.number;
		assert(tempBulletHitEntity->r.clientMask[ 0 ] == 0);
		tempBulletHitEntity->r.clientMask[pSelf->client->ps.clientNum >> 5] |= 1 << (pSelf->client->ps.clientNum & 0x1F);

		if ( BG_GetWeaponDef(iWeapon)->rifleBullet )
			tent = G_TempEntity(vPoint, EV_BULLET_HIT_CLIENT_LARGE);
		else
			tent = G_TempEntity(vPoint, EV_BULLET_HIT_CLIENT_SMALL);

		tent->s.surfType = SURF_TYPE_FLESH;
		tent->s.otherEntityNum = attacker->s.number;
		tent->s.clientNum = pSelf->client->ps.clientNum;
		tent->r.clientMask[0] = -1;
		tent->r.clientMask[1] = -1;
		tent->r.clientMask[pSelf->client->ps.clientNum >> 5] &= ~(1 << (pSelf->client->ps.clientNum & 0x1F));
	}

	pSelf->client->damage_blood += damage;

	if ( dir )
	{
		VectorCopy(localdir, pSelf->client->damage_from);
		pSelf->client->damage_fromWorld = qfalse;
	}
	else
	{
		VectorCopy(pSelf->r.currentOrigin, pSelf->client->damage_from);
		pSelf->client->damage_fromWorld = qtrue;
	}

	if ( pSelf->flags & FL_DEMI_GODMODE && pSelf->health - damage <= 0 )
	{
		damage = pSelf->health - 1;
	}

	max_damage_time = player_dmgtimer_maxTime->current.decimal;
	time_per_point = damage * player_dmgtimer_timePerPoint->current.decimal;

	pSelf->client->ps.damageTimer += (int)time_per_point;

	if ( dir )
	{
		pSelf->client->ps.flinchYaw = (int)vectoyaw(dir);
	}
	else
	{
		pSelf->client->ps.flinchYaw = 0;
	}

	flinchYawDir = pSelf->client->ps.viewangles[YAW];

	if ( flinchYawDir < 0 )
	{
		flinchYawDir = flinchYawDir + 360;
	}

	pSelf->client->ps.flinchYaw -= (int)flinchYawDir;

	if ( pSelf->client->ps.damageTimer > max_damage_time )
	{
		pSelf->client->ps.damageTimer = (int)max_damage_time;
	}

	pSelf->client->ps.damageDuration = pSelf->client->ps.damageTimer;
	pSelf->health -= damage;

	Scr_AddEntity(attacker);
	Scr_AddInt(damage);

	Scr_Notify(pSelf, scr_const.damage, 2);

	if ( pSelf->health > 0 )
	{
		pain = entityHandlers[pSelf->handler].pain;

		if ( pain )
		{
			pain(pSelf, attacker, damage, point, modIndex, localdir, hitLoc);
		}
	}
	else
	{
		if ( pSelf->health < -999 )
		{
			pSelf->health = -999;
		}

		die = entityHandlers[pSelf->handler].die;

		if ( die )
		{
			die(pSelf, inflictor, attacker, damage, modIndex, iWeapon, localdir, hitLoc, iSurfType);
		}

		assert(pSelf->r.inuse);
	}

	pSelf->client->ps.stats[STAT_HEALTH] = pSelf->health;
}

/*
===============
PlayerCmd_Suicide
===============
*/
void PlayerCmd_Suicide( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pSelf->flags &= ~( FL_GODMODE | FL_DEMI_GODMODE );
	pSelf->health = 0;
	pSelf->client->ps.stats[STAT_HEALTH] = 0;

	player_die(pSelf, pSelf, pSelf, 100000, MOD_SUICIDE, WP_NONE, NULL, HITLOC_NONE, 0);
}

/*
===============
PlayerCmd_dropItem
===============
*/
void PlayerCmd_dropItem( scr_entref_t entref )
{
	gentity_t *pSelf;
	gitem_t *pItem;
	gentity_t *pEnt;
	unsigned int dropTag;
	int iWeaponIndex;
	const char *pszItemName;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	pszItemName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszItemName);

	if ( iWeaponIndex )
	{
		if ( Scr_GetNumParam() > 1 )
			dropTag = Scr_GetConstLowercaseString(1);
		else
			dropTag = scr_const.tag_weapon_right;

		pEnt = Drop_Weapon(pSelf, iWeaponIndex, dropTag);
	}
	else
	{
		pItem = G_FindItem(pszItemName);

		if ( pItem )
			pEnt = Drop_Item(pSelf, pItem, 0, qfalse);
		else
			pEnt = NULL;
	}

	GScr_AddEntity(pEnt);
}

/*
===============
PlayerCmd_spawn
===============
*/
void PlayerCmd_spawn( scr_entref_t entref )
{
	gentity_t *pSelf;
	vec3_t spawn_angles;
	vec3_t spawn_origin;

	if ( entref.classnum != CLASS_NUM_ENTITY )
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
		return;
	}

	assert(entref.entnum < MAX_GENTITIES);
	pSelf = &g_entities[entref.entnum];

	if ( pSelf->client == NULL )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		return;
	}

	Scr_GetVector(0, spawn_origin);
	Scr_GetVector(1, spawn_angles);

	ClientSpawn(pSelf, spawn_origin, spawn_angles);
}
