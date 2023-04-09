#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

#ifdef TESTING_LIBRARY
#define g_scr_data (*(scr_data_t*)( 0x0879C780 ))
#else
extern scr_data_t g_scr_data;
#endif

#ifdef TESTING_LIBRARY
#define level_bgs (*((bgs_t*)( 0x0859EA40 )))
#else
extern bgs_t level_bgs;
#endif

scr_method_t player_methods[] =
{
	{ "giveweapon", PlayerCmd_giveWeapon, 0, },
	{ "takeweapon", PlayerCmd_takeWeapon, 0, },
	{ "takeallweapons", PlayerCmd_takeAllWeapons, 0, },
	{ "getcurrentweapon", PlayerCmd_getCurrentWeapon, 0, },
	{ "getcurrentoffhand", PlayerCmd_getCurrentOffhand, 0, },
	{ "hasweapon", PlayerCmd_hasWeapon, 0, },
	{ "switchtoweapon", PlayerCmd_switchToWeapon, 0, },
	{ "switchtooffhand", PlayerCmd_switchToOffhand, 0, },
	{ "givestartammo", PlayerCmd_giveStartAmmo, 0, },
	{ "givemaxammo", PlayerCmd_giveMaxAmmo, 0, },
	{ "getfractionstartammo", PlayerCmd_getFractionStartAmmo, 0, },
	{ "getfractionmaxammo", PlayerCmd_getFractionMaxAmmo, 0, },
	{ "setorigin", PlayerCmd_setOrigin, 0, },
	{ "setplayerangles", PlayerCmd_setAngles, 0, },
	{ "getplayerangles", PlayerCmd_getAngles, 0, },
	{ "usebuttonpressed", PlayerCmd_useButtonPressed, 0, },
	{ "attackbuttonpressed", PlayerCmd_attackButtonPressed, 0, },
	{ "meleebuttonpressed", PlayerCmd_meleeButtonPressed, 0, },
	{ "playerads", PlayerCmd_playerADS, 0, },
	{ "isonground", PlayerCmd_isOnGround, 0, },
	{ "pingplayer", PlayerCmd_pingPlayer, 0, },
	{ "setviewmodel", PlayerCmd_SetViewmodel, 0, },
	{ "getviewmodel", PlayerCmd_GetViewmodel, 0, },
	{ "sayall", PlayerCmd_SayAll, 0, },
	{ "sayteam", PlayerCmd_SayTeam, 0, },
	{ "showscoreboard", PlayerCmd_showScoreboard, 0, },
	{ "setspawnweapon", PlayerCmd_setSpawnWeapon, 0, },
	{ "dropitem", PlayerCmd_dropItem, 0, },
	{ "finishplayerdamage", PlayerCmd_finishPlayerDamage, 0, },
	{ "suicide", PlayerCmd_Suicide, 0, },
	{ "openmenu", PlayerCmd_OpenMenu, 0, },
	{ "openmenunomouse", PlayerCmd_OpenMenuNoMouse, 0, },
	{ "closemenu", PlayerCmd_CloseMenu, 0, },
	{ "closeingamemenu", PlayerCmd_CloseInGameMenu, 0, },
	{ "freezecontrols", PlayerCmd_FreezeControls, 0, },
	{ "disableweapon", PlayerCmd_DisableWeapon, 0, },
	{ "enableweapon", PlayerCmd_EnableWeapon, 0, },
	{ "setreverb", PlayerCmd_SetReverb, 0, },
	{ "deactivatereverb", PlayerCmd_DeactivateReverb, 0, },
	{ "setchannelvolumes", PlayerCmd_SetChannelVolumes, 0, },
	{ "deactivatechannelvolumes", PlayerCmd_DeactivateChannelVolumes, 0, },
	{ "getweaponslotweapon", PlayerCmd_GetWeaponSlotWeapon, 0, },
	{ "setweaponslotweapon", PlayerCmd_SetWeaponSlotWeapon, 0, },
	{ "getweaponslotammo", PlayerCmd_GetWeaponSlotAmmo, 0, },
	{ "setweaponslotammo", PlayerCmd_SetWeaponSlotAmmo, 0, },
	{ "getweaponslotclipammo", PlayerCmd_GetWeaponSlotClipAmmo, 0, },
	{ "setweaponslotclipammo", PlayerCmd_SetWeaponSlotClipAmmo, 0, },
	{ "setweaponclipammo", PlayerCmd_SetWeaponClipAmmo, 0, },
	{ "iprintln", iclientprintln, 0, },
	{ "iprintlnbold", iclientprintlnbold, 0, },
	{ "spawn", PlayerCmd_spawn, 0, },
	{ "setentertime", PlayerCmd_setEnterTime, 0, },
	{ "cloneplayer", PlayerCmd_ClonePlayer, 0, },
	{ "setclientcvar", PlayerCmd_SetClientDvar, 0, },
	{ "islookingat", ScrCmd_IsLookingAt, 0, },
	{ "playlocalsound", ScrCmd_PlayLocalSound, 0, },
	{ "istalking", PlayerCmd_IsTalking, 0, },
	{ "allowspectateteam", PlayerCmd_AllowSpectateTeam, 0, },
	{ "getguid", PlayerCmd_GetGuid, 0, },
};

void PlayerCmd_giveWeapon(scr_entref_t entRef)
{
	WeaponDef *weapDef2;
	WeaponDef *weapDef1;
	gentity_s *pSelf;
	gclient_s *client;
	int hadWeapon;
	int ammoCount;
	int weapon;
	const char *weaponName;
	WeaponDef *weaponDef;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		pSelf = 0;
	}
	else
	{
		pSelf = &g_entities[entRef.entnum];

		if ( !pSelf->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	weaponName = Scr_GetString(0);
	weapon = G_GetWeaponIndexForName(weaponName);
	client = pSelf->client;
	hadWeapon = COM_BitTest(client->ps.weapons, weapon);
	weaponDef = BG_GetWeaponDef(weapon);

	if ( BG_DoesWeaponNeedSlot(weapon) && !BG_GetEmptySlotForWeapon(&client->ps, weapon) )
	{
		weapDef2 = BG_GetWeaponDef(client->ps.weaponslots[2]);
		weapDef1 = BG_GetWeaponDef(client->ps.weaponslots[1]);
		Scr_ParamError(0, va("Cannot give %s weapon %s without having an empty weapon slot - player currently has a %s and a %s\n", pSelf->client->sess.state.name, weaponDef->szDisplayName, weapDef1->szDisplayName, weapDef2->szDisplayName));
	}

	if ( G_GivePlayerWeapon(&client->ps, weapon) )
	{
		SV_GameSendServerCommand(pSelf - g_entities, 0, va("%c \"%i\"", 73, 1));
	}

	ammoCount = weaponDef->startAmmo - client->ps.ammo[weaponDef->ammoIndex];

	if ( ammoCount > 0 )
		Add_Ammo(pSelf, weapon, ammoCount, hadWeapon == 0);
}

void PlayerCmd_takeWeapon(scr_entref_t entref)
{
	gclient_s *client;
	gentity_s *ent;
	int weaponIndex;
	const char *weaponName;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	weaponName = Scr_GetString(0);
	weaponIndex = G_GetWeaponIndexForName(weaponName);

	client = ent->client;
	client->ps.ammo[BG_AmmoForWeapon(weaponIndex)] = 0;
	client = ent->client;
	client->ps.ammoclip[BG_ClipForWeapon(weaponIndex)] = 0;

	BG_TakePlayerWeapon(&ent->client->ps, weaponIndex);
}

void PlayerCmd_takeAllWeapons(scr_entref_t entref)
{
	gclient_s *client;
	gentity_s *ent;
	int weaponIndex;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	ent->client->ps.weapon = 0;

	for ( weaponIndex = 1; weaponIndex <= BG_GetNumWeapons(); ++weaponIndex )
	{
		client = ent->client;
		client->ps.ammo[BG_AmmoForWeapon(weaponIndex)] = 0;
		client = ent->client;
		client->ps.ammoclip[BG_ClipForWeapon(weaponIndex)] = 0;

		BG_TakePlayerWeapon(&ent->client->ps, weaponIndex);
	}
}

void PlayerCmd_getCurrentWeapon(scr_entref_t entref)
{
	gentity_s *ent;
	int weaponIndex;
	WeaponDef *weaponDef;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	if ( G_IsPlaying(ent) && (weaponIndex = ent->client->ps.weapon, weaponIndex > 0) )
	{
		weaponDef = BG_GetWeaponDef(weaponIndex);
		Scr_AddString(weaponDef->szInternalName);
	}
	else
	{
		Scr_AddString("none");
	}
}

void PlayerCmd_getCurrentOffhand(scr_entref_t entRef)
{
	gentity_s *ent;
	WeaponDef *weaponDef;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	if ( G_IsPlaying(ent) && ent->client->ps.offHandIndex > 0 )
	{
		weaponDef = BG_GetWeaponDef(ent->client->ps.offHandIndex);
		Scr_AddString(weaponDef->szInternalName);
	}
	else
	{
		Scr_AddString("none");
	}
}

void PlayerCmd_hasWeapon(scr_entref_t entref)
{
	gentity_s *ent;
	int index;
	const char *weaponName;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	weaponName = Scr_GetString(0);
	index = BG_FindWeaponIndexForName(weaponName);

	if ( index && COM_BitTest(ent->client->ps.weapons, index) )
		Scr_AddBool(1);
	else
		Scr_AddBool(0);
}

void PlayerCmd_switchToWeapon(scr_entref_t entRef)
{
	gentity_s *ent;
	int weaponIndex;
	const char *weaponName;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	weaponName = Scr_GetString(0);
	weaponIndex = G_GetWeaponIndexForName(weaponName);

	if ( !weaponIndex )
	{
		Scr_ParamError(0, va("unknown weapon '%s'", weaponName));
	}

	if ( COM_BitTest(ent->client->ps.weapons, weaponIndex) )
	{
		SendWeaponChangeInfo(entRef.entnum, weaponIndex);
		Scr_AddBool(1);
	}
	else
	{
		Scr_AddBool(0);
	}
}

void PlayerCmd_switchToOffhand(scr_entref_t entref)
{
	gentity_s *ent;
	int index;
	const char *weaponName;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	weaponName = Scr_GetString(0);
	index = G_GetWeaponIndexForName(weaponName);

	if ( !index )
	{
		Scr_ParamError(0, va("unknown weapon '%s'", weaponName));
	}

	if ( COM_BitTest(ent->client->ps.weapons, index) )
	{
		G_SetEquippedOffHand(entref.entnum, index);
		Scr_AddBool(1);
	}
	else
	{
		Scr_AddBool(0);
	}
}

void PlayerCmd_giveStartAmmo(scr_entref_t entref)
{
	gentity_s *ent;
	int ammo;
	int weaponIndex;
	const char *weaponName;
	WeaponDef *weaponDef;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	weaponName = Scr_GetString(0);
	weaponIndex = G_GetWeaponIndexForName(weaponName);

	if ( COM_BitTest(ent->client->ps.weapons, weaponIndex) )
	{
		weaponDef = BG_GetWeaponDef(weaponIndex);
		ammo = weaponDef->startAmmo - ent->client->ps.ammo[weaponDef->ammoIndex];

		if ( ammo > 0 )
			Add_Ammo(ent, weaponIndex, ammo, 0);
	}
}

void PlayerCmd_giveMaxAmmo(scr_entref_t entRef)
{
	gentity_s *ent;
	int ammo;
	int weaponIndex;
	const char *weaponName;
	WeaponDef *weaponDef;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	weaponName = Scr_GetString(0);
	weaponIndex = G_GetWeaponIndexForName(weaponName);

	if ( COM_BitTest(ent->client->ps.weapons, weaponIndex) )
	{
		weaponDef = BG_GetWeaponDef(weaponIndex);
		ammo = BG_GetMaxAmmo(weaponDef->ammoIndex) - ent->client->ps.ammo[weaponDef->ammoIndex];

		if ( ammo > 0 )
			Add_Ammo(ent, weaponIndex, ammo, 0);
	}
}

void PlayerCmd_getFractionStartAmmo(scr_entref_t entRef)
{
	gentity_s *ent;
	float ammo;
	int weaponIndex;
	const char *weaponName;
	WeaponDef *weaponDef;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	weaponName = Scr_GetString(0);
	weaponIndex = G_GetWeaponIndexForName(weaponName);

	if ( COM_BitTest(ent->client->ps.weapons, weaponIndex)
	        && (weaponDef = BG_GetWeaponDef(weaponIndex), weaponDef->startAmmo > 0) )
	{
		if ( ent->client->ps.ammo[weaponDef->ammoIndex] > 0 )
		{
			ammo = (long double)ent->client->ps.ammo[weaponDef->ammoIndex] / (long double)weaponDef->startAmmo;
			Scr_AddFloat(ammo);
		}
		else
		{
			Scr_AddFloat(0.0);
		}
	}
	else
	{
		Scr_AddFloat(1.0);
	}
}

void PlayerCmd_getFractionMaxAmmo(scr_entref_t entRef)
{
	gentity_s *ent;
	float ammo;
	int weaponIndex;
	const char *weaponName;
	WeaponDef *weaponDef;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	weaponName = Scr_GetString(0);
	weaponIndex = G_GetWeaponIndexForName(weaponName);

	if ( COM_BitTest(ent->client->ps.weapons, weaponIndex)
	        && (weaponDef = BG_GetWeaponDef(weaponIndex), BG_GetMaxAmmo(weaponDef->ammoIndex) > 0) )
	{
		if ( ent->client->ps.ammo[weaponDef->ammoIndex] > 0 )
		{
			ammo = (long double)ent->client->ps.ammo[weaponDef->ammoIndex] / (long double)BG_GetMaxAmmo(weaponDef->ammoIndex);
			Scr_AddFloat(ammo);
		}
		else
		{
			Scr_AddFloat(0.0);
		}
	}
	else
	{
		Scr_AddFloat(1.0);
	}
}

void PlayerCmd_setOrigin(scr_entref_t entRef)
{
	gentity_s *ent;
	vec3_t origin;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	Scr_GetVector(0, origin);
	SV_UnlinkEntity(ent);
	VectorCopy(origin, ent->client->ps.origin);
	ent->client->ps.origin[2] = ent->client->ps.origin[2] + 1.0;
	ent->client->ps.eFlags ^= 2u;
	BG_PlayerStateToEntityState(&ent->client->ps, &ent->s, 1, 1u);
	VectorCopy(ent->client->ps.origin, ent->r.currentOrigin);
	SV_LinkEntity(ent);
}

void PlayerCmd_setAngles(scr_entref_t entref)
{
	gentity_s *ent;
	vec3_t angles;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	Scr_GetVector(0, angles);
	SetClientViewAngle(ent, angles);
}

void PlayerCmd_getAngles(scr_entref_t entref)
{
	gentity_s *ent;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	Scr_AddVector(ent->client->ps.viewangles);
}

void PlayerCmd_useButtonPressed(scr_entref_t entref)
{
	gentity_s *ent;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	if ( ((ent->client->buttonsSinceLastFrame | ent->client->buttons) & 0x28) != 0 )
		Scr_AddInt(1);
	else
		Scr_AddInt(0);
}

void PlayerCmd_attackButtonPressed(scr_entref_t entref)
{
	gentity_s *ent;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	if ( ((LOBYTE(ent->client->buttonsSinceLastFrame) | LOBYTE(ent->client->buttons)) & 1) != 0 )
		Scr_AddInt(1);
	else
		Scr_AddInt(0);
}

void PlayerCmd_meleeButtonPressed(scr_entref_t entref)
{
	gentity_s *ent;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	if ( ((ent->client->buttonsSinceLastFrame | ent->client->buttons) & 4) != 0 )
		Scr_AddInt(1);
	else
		Scr_AddInt(0);
}

void PlayerCmd_playerADS(scr_entref_t entref)
{
	gentity_s *ent;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	Scr_AddFloat(ent->client->ps.fWeaponPosFrac);
}

void PlayerCmd_isOnGround(scr_entref_t entref)
{
	gentity_s *ent;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	if ( ent->client->ps.groundEntityNum == 1023 )
		Scr_AddInt(0);
	else
		Scr_AddInt(1);
}

void PlayerCmd_pingPlayer(scr_entref_t entRef)
{
	gentity_s *ent;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	ent->client->ps.eFlags |= 0x400000u;
	ent->client->compassPingTime = level.time + 3000;
}

void PlayerCmd_SetViewmodel(scr_entref_t entRef)
{
	gentity_s *ent;
	const char *modelName;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	modelName = Scr_GetString(0);

	if ( !modelName || !*modelName )
		Scr_ParamError(0, "usage: setviewmodel(<model name>)");

	ent->client->sess.viewmodelIndex = G_ModelIndex(modelName);
}

void PlayerCmd_GetViewmodel(scr_entref_t entRef)
{
	gentity_s *ent;
	const char *modelName;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	modelName = G_ModelName(ent->client->sess.viewmodelIndex);
	Scr_AddString(modelName);
}

void PlayerCmd_SayAll(scr_entref_t entref)
{
	int paramNum;
	gentity_s *pSelf;
	char szString[1024];

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		pSelf = 0;
	}
	else
	{
		pSelf = &g_entities[entref.entnum];

		if ( !pSelf->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	paramNum = Scr_GetNumParam();
	Scr_ConstructMessageString(0, paramNum - 1, "Client Chat Message", &szString[1], 0x3FFu);
	szString[0] = 20;
	G_Say(pSelf, 0, 0, szString);
}

void PlayerCmd_SayTeam(scr_entref_t entref)
{
	int paramNum;
	gentity_s *pSelf;
	char szString[1024];

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		pSelf = 0;
	}
	else
	{
		pSelf = &g_entities[entref.entnum];

		if ( !pSelf->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	paramNum = Scr_GetNumParam();
	Scr_ConstructMessageString(0, paramNum - 1, "Client Chat Message", &szString[1], 0x3FFu);
	szString[0] = 20;
	G_Say(pSelf, 0, 1, szString);
}

void PlayerCmd_showScoreboard(scr_entref_t entref)
{
	gentity_s *ent;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	Cmd_Score_f(ent);
}

void PlayerCmd_setSpawnWeapon(scr_entref_t entRef)
{
	gentity_s *ent;
	unsigned int weaponIndex;
	const char *weaponName;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	weaponName = Scr_GetString(0);
	weaponIndex = G_GetWeaponIndexForName(weaponName);

	if ( BG_IsWeaponValid(&ent->client->ps, weaponIndex) )
	{
		ent->client->ps.weapon = weaponIndex;
		ent->client->ps.weaponstate = WEAPON_READY;
	}
}

void PlayerCmd_dropItem(scr_entref_t entref)
{
	gentity_s *pSelf;
	const gitem_s *item;
	gentity_s *pEnt;
	unsigned int dropTag;
	int iWeaponIndex;
	const char *pszItemName;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		pSelf = 0;
	}
	else
	{
		pSelf = &g_entities[entref.entnum];

		if ( !pSelf->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	pszItemName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszItemName);

	if ( iWeaponIndex )
	{
		if ( Scr_GetNumParam() <= 1 )
			dropTag = scr_const.tag_weapon_right;
		else
			dropTag = Scr_GetConstLowercaseString(1u);

		pEnt = Drop_Weapon(pSelf, iWeaponIndex, dropTag);
	}
	else
	{
		item = G_FindItem(pszItemName);

		if ( item )
			pEnt = Drop_Item(pSelf, item, 0.0, 0);
		else
			pEnt = 0;
	}

	GScr_AddEntity(pEnt);
}

extern dvar_t *g_knockback;
void PlayerCmd_finishPlayerDamage(scr_entref_t entRef)
{
	const char *mod;
	const char *weaponName;
	unsigned short hitloc;
	gentity_s *tempEnt;
	gclient_s *client;
	float knockback;
	gentity_s *ent;
	float flinch;
	float dmgTime;
	float maxTime;
	void (*pain)(struct gentity_s *, struct gentity_s *, int, const float *, const int, const float *, int); // [esp+58h] [ebp-B0h]
	void (*die)(struct gentity_s *, struct gentity_s *, struct gentity_s *, int, int, const int, const float *, int, int); // [esp+5Ch] [ebp-ACh]
	int iSurfType;
	int maxDmg;
	vec3_t velocaityScale;
	vec3_t localdir;
	float dmgRange;
	int minDmg;
	int hitlocStr;
	int weaponIndex;
	int modIndex;
	int dflags;
	int damage;
	const float *point;
	vec3_t vPoint;
	vec_t *dir;
	vec3_t vDir;
	gentity_s *attacker;
	gentity_s *inflictor;

	inflictor = &g_entities[1022];
	attacker = &g_entities[1022];
	dir = 0;
	point = 0;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	damage = Scr_GetInt(2u);

	if ( damage > 0 )
	{
		if ( Scr_GetType(0) && Scr_GetPointerType(0) == VAR_ENTITY )
			inflictor = Scr_GetEntity(1u);

		if ( Scr_GetType(1u) && Scr_GetPointerType(1u) == VAR_ENTITY )
			attacker = Scr_GetEntity(1u);

		dflags = Scr_GetInt(3u);
		mod = Scr_GetString(4u);
		modIndex = G_IndexForMeansOfDeath(mod);
		weaponName = Scr_GetString(5u);
		weaponIndex = G_GetWeaponIndexForName(weaponName);

		if ( Scr_GetType(6u) )
		{
			Scr_GetVector(6u, vPoint);
			point = vPoint;
		}

		if ( Scr_GetType(7u) )
		{
			Scr_GetVector(7u, vDir);
			dir = vDir;
		}

		hitloc = Scr_GetConstString(8u);
		hitlocStr = G_GetHitLocationIndexFromString(hitloc);
		iSurfType = Scr_GetInt(9u);

		if ( dir )
			Vec3NormalizeTo(dir, localdir);
		else
			VectorClear(localdir);

		if ( (ent->flags & 8) != 0 || (dflags & 4) != 0 )
		{
			minDmg = 0;
		}
		else
		{
			dmgRange = 0.30000001;

			if ( (ent->client->ps.pm_flags & 1) != 0 )
			{
				dmgRange = 0.02;
			}
			else if ( (ent->client->ps.pm_flags & 2) != 0 )
			{
				dmgRange = 0.15000001;
			}

			minDmg = (int)((long double)damage * dmgRange);

			if ( minDmg > 60 )
				minDmg = 60;

			if ( minDmg )
			{
				if ( (ent->client->ps.eFlags & 0x300) == 0 )
				{
					knockback = (long double)minDmg * g_knockback->current.decimal / 250.0;
					VectorScale(localdir, knockback, velocaityScale);
					VectorAdd(ent->client->ps.velocity, velocaityScale, ent->client->ps.velocity);

					if ( !ent->client->ps.pm_time )
					{
						maxDmg = 2 * minDmg;

						if ( 2 * minDmg <= 49 )
							maxDmg = 50;

						if ( maxDmg > 200 )
							maxDmg = 200;

						ent->client->ps.pm_time = maxDmg;
						ent->client->ps.pm_flags |= 0x400u;
					}
				}
			}
		}

		if ( (ent->flags & 1) == 0 )
		{
			if ( weaponIndex && BG_GetWeaponDef(weaponIndex)->weaponType == WEAPTYPE_BULLET )
			{
				if ( BG_GetWeaponDef(weaponIndex)->rifleBullet )
					tempEnt = G_TempEntity(vPoint, EV_SHOTGUN_HIT);
				else
					tempEnt = G_TempEntity(vPoint, EV_BULLET_HIT_LARGE);

				tempEnt->s.eventParm = DirToByte(localdir);
				tempEnt->s.scale = DirToByte(localdir);
				tempEnt->s.surfType = 7;
				tempEnt->s.otherEntityNum = attacker->s.number;
				tempEnt->r.clientMask[ent->client->ps.clientNum >> 5] |= 1 << (ent->client->ps.clientNum & 0x1F);

				if ( BG_GetWeaponDef(weaponIndex)->rifleBullet )
					tempEnt = G_TempEntity(vPoint, EV_BULLET_HIT_CLIENT_LARGE);
				else
					tempEnt = G_TempEntity(vPoint, EV_BULLET_HIT_CLIENT_SMALL);

				tempEnt->s.surfType = 7;
				tempEnt->s.otherEntityNum = attacker->s.number;
				tempEnt->s.clientNum = ent->client->ps.clientNum;
				tempEnt->r.clientMask[0] = -1;
				tempEnt->r.clientMask[1] = -1;
				tempEnt->r.clientMask[ent->client->ps.clientNum >> 5] &= ~(1 << (ent->client->ps.clientNum & 0x1F));
			}

			ent->client->damage_blood += damage;

			if ( dir )
			{
				VectorCopy(localdir, ent->client->damage_from);
				ent->client->damage_fromWorld = 0;
			}
			else
			{
				VectorCopy(ent->r.currentOrigin, ent->client->damage_from);
				ent->client->damage_fromWorld = 1;
			}

			if ( (ent->flags & 2) != 0 && ent->health - damage <= 0 )
				damage = ent->health - 1;

			maxTime = player_dmgtimer_maxTime->current.decimal;
			dmgTime = (long double)damage * player_dmgtimer_timePerPoint->current.decimal;
			ent->client->ps.damageTimer += (int)dmgTime;

			if ( dir )
			{
				client = ent->client;
				client->ps.flinchYaw = (int)vectoyaw(dir);
			}
			else
			{
				ent->client->ps.flinchYaw = 0;
			}

			flinch = ent->client->ps.viewangles[1];

			if ( flinch < 0.0 )
				flinch = flinch + 360.0;

			ent->client->ps.flinchYaw -= (int)flinch;

			if ( (long double)ent->client->ps.damageTimer > maxTime )
				ent->client->ps.damageTimer = (int)maxTime;

			ent->client->ps.damageDuration = ent->client->ps.damageTimer;
			ent->health -= damage;
			Scr_AddEntity(attacker);
			Scr_AddInt(damage);
			Scr_Notify(ent, scr_const.damage, 2u);

			if ( ent->health > 0 )
			{
				pain = entityHandlers[ent->handler].pain;

				if ( pain )
					pain(ent, attacker, damage, point, modIndex, localdir, hitlocStr);
			}
			else
			{
				if ( ent->health < -999 )
					ent->health = -999;

				die = entityHandlers[ent->handler].die;

				if ( die )
					die(ent, inflictor, attacker, damage, modIndex, weaponIndex, localdir, hitlocStr, iSurfType);

				if ( !ent->r.inuse )
					return;
			}

			ent->client->ps.stats[0] = ent->health;
		}
	}
}

void PlayerCmd_Suicide(scr_entref_t entref)
{
	gclient_s *client;
	gentity_s *ent;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	ent->flags &= 0xFFFFFFFC;
	client = ent->client;
	ent->health = 0;
	client->ps.stats[0] = 0;
	player_die(ent, ent, ent, 100000, 12, 0, 0, HITLOC_NONE, 0);
}

void PlayerCmd_OpenMenu(scr_entref_t entref)
{
	const char *menuName;
	gentity_s *ent;
	int menuIndex;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	if ( ent->client->sess.connected == CON_CONNECTED )
	{
		menuName = Scr_GetString(0);
		menuIndex = GScr_GetScriptMenuIndex(menuName);
		SV_GameSendServerCommand(entref.entnum, 1, va("%c %i", 116, menuIndex));
		Scr_AddInt(1);
	}
	else
	{
		Scr_AddInt(0);
	}
}

void PlayerCmd_OpenMenuNoMouse(scr_entref_t entref)
{
	const char *menuName;
	gentity_s *ent;
	int menuIndex;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	if ( ent->client->sess.connected == CON_CONNECTED )
	{
		menuName = Scr_GetString(0);
		menuIndex = GScr_GetScriptMenuIndex(menuName);
		SV_GameSendServerCommand(entref.entnum, 1, va("%c %i 1", 116, menuIndex));
		Scr_AddInt(1);
	}
	else
	{
		Scr_AddInt(0);
	}
}

void PlayerCmd_CloseMenu(scr_entref_t entref)
{
	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
	}

	SV_GameSendServerCommand(entref.entnum, 1, va("%c", 117));
}

void PlayerCmd_CloseInGameMenu(scr_entref_t entref)
{
	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
	}

	SV_GameSendServerCommand(entref.entnum, 1, va("%c", 75));
}

void PlayerCmd_FreezeControls(scr_entref_t entref)
{
	gclient_s *client;
	gentity_s *ent;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	client = ent->client;
	client->bFrozen = Scr_GetInt(0);
}

void PlayerCmd_DisableWeapon(scr_entref_t entref)
{
	gentity_s *ent;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	ent->client->ps.pm_flags |= 0x4000000u;
}

void PlayerCmd_EnableWeapon(scr_entref_t entref)
{
	gentity_s *ent;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	ent->client->ps.pm_flags &= ~0x4000000u;
}

void PlayerCmd_SetReverb(scr_entref_t entref)
{
	unsigned short priorityString;
	int paramNum;
	const char *roomtype;
	float fadetime;
	float wetlevel;
	float drylevel;
	int priority;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
	}

	fadetime = 0.0;
	drylevel = 1.0;
	wetlevel = 0.5;
	paramNum = Scr_GetNumParam();
	if ( paramNum != 3 )
	{
		if ( paramNum < 4 )
		{
			if ( paramNum != 2 )
			{
error:
				Scr_Error(
				    "USAGE: player setReverb(\"priority\", \"roomtype\", drylevel = 1.0, wetlevel = 0.5, fadetime = 0);\nValid priorities are \"snd_enveffectsprio_level\" or \"snd_enveffectsprio_shellsho ck\", dry level is a float from 0 (no source sound) to 1 (full source sound), wetlevel is a float from 0 (no effect) to 1 (full effect), fadetime is in sec and modifies drylevel and w etlevel\n"
				);
				return;
			}
			goto settype;
		}
		if ( paramNum != 4 )
		{
			if ( paramNum != 5 ) goto error;
			fadetime = Scr_GetFloat(4);
		}
		wetlevel = Scr_GetFloat(3);
	}
	drylevel = Scr_GetFloat(2);
settype:
	roomtype = Scr_GetString(1);
	priorityString = Scr_GetConstString(0);
	priority = 1;
	if ( priorityString == scr_const.snd_enveffectsprio_level )
	{
		priority = 1;
	}
	else if ( priorityString == scr_const.snd_enveffectsprio_shellshock )
	{
		priority = 2;
	}
	else
	{
		Scr_Error("priority must be \'snd_enveffectsprio_level\' or \'snd_enveffectsprio_shellshock\'\n");
	}
	SV_GameSendServerCommand(entref.entnum, 1, va("%c %i \"%s\" %g %g %g", 114, priority, roomtype, drylevel, wetlevel, fadetime));
}

void PlayerCmd_DeactivateReverb(scr_entref_t entref)
{
	unsigned short priorityString;
	int paramNum;
	float fadetime;
	int priority;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
	}

	fadetime = 0.0;
	paramNum = Scr_GetNumParam();
	if ( paramNum != 1 )
	{
		if ( paramNum != 2 )
		{
			Scr_Error(
			    "USAGE: player deactivateReverb(\"priority\", fadetime = 0);\nValid priorities are \"snd_enveffectsprio_level\" or \"snd_enveffectsprio_shellshock\", fadetime is the time spent fading t o the next lowest active reverb priority level in seconds\n"
			);
			return;
		}
		fadetime = Scr_GetFloat(1);
	}
	priorityString = Scr_GetConstString(0);
	priority = 1;
	if ( priorityString == scr_const.snd_enveffectsprio_level )
	{
		priority = 1;
	}
	else if ( priorityString == scr_const.snd_enveffectsprio_shellshock )
	{
		priority = 2;
	}
	else
	{
		Scr_Error("priority must be \'snd_enveffectsprio_level\' or \'snd_enveffectsprio_shellshock\'\n");
	}
	SV_GameSendServerCommand(entref.entnum, 1, va("%c %i %g", 68, priority, fadetime));
}

void PlayerCmd_SetChannelVolumes(scr_entref_t entref)
{
	unsigned short priorityString;
	int paramNum;
	float fadetime;
	int priority;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
	}

	fadetime = 0.0;
	paramNum = Scr_GetNumParam();
	if ( paramNum != 2 )
	{
		if ( paramNum != 3 )
		{
			Scr_Error(
			    "USAGE: player setchannelvolumes(\"priority\", \"shock name\", fadetime = 0);\nValid priorities are \"snd_channelvolprio_holdbreath\", \"snd_channelvolprio_pain\", or \"snd_channelvolpr io_shellshock\", fadetime is in sec\n"
			);
			return;
		}
		fadetime = Scr_GetFloat(2);
	}
	priorityString = Scr_GetConstString(0);
	priority = 1;
	if ( priorityString == scr_const.snd_channelvolprio_holdbreath )
	{
		priority = 1;
	}
	else if ( priorityString == scr_const.snd_channelvolprio_pain )
	{
		priority = 2;
	}
	else if ( priorityString == scr_const.snd_channelvolprio_shellshock )
	{
		priority = 3;
	}
	else
	{
		Scr_Error("priority must be \'snd_channelvolprio_holdbreath\', \'snd_channelvolprio_pain\', or \'snd_channelvolprio_shellshock\'\n");
	}
	SV_GameSendServerCommand(entref.entnum, 1, va("%c %i %i %g", 69, priority, G_FindConfigstringIndex(Scr_GetString(1), 1166, 16, 0, NULL), fadetime));
}

void PlayerCmd_DeactivateChannelVolumes(scr_entref_t entref)
{
	unsigned short priorityString;
	int paramNum;
	float fadetime;
	int priority;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
	}

	fadetime = 0.0;
	paramNum = Scr_GetNumParam();
	if ( paramNum != 1 )
	{
		if ( paramNum != 2 )
		{
			Scr_Error(
			    "USAGE: player deactivatechannelvolumes(\"priority\", fadetime = 0);\nValid priorities are \"snd_channelvolprio_holdbreath\", \"snd_channelvolprio_pain\", or \"snd_channelvolprio_shells hock\", fadetime is the time spent fading to the next lowest active reverb priority level in seconds\n"
			);
			return;
		}
		fadetime = Scr_GetFloat(1);
	}
	priorityString = Scr_GetConstString(0);
	priority = 1;
	if ( priorityString == scr_const.snd_channelvolprio_holdbreath )
	{
		priority = 1;
	}
	else if ( priorityString == scr_const.snd_channelvolprio_pain )
	{
		priority = 2;
	}
	else if ( priorityString == scr_const.snd_channelvolprio_shellshock )
	{
		priority = 3;
	}
	else
	{
		Scr_Error("priority must be \'snd_channelvolprio_holdbreath\', \'snd_channelvolprio_pain\', or \'snd_channelvolprio_shellshock\'\n");
	}
	SV_GameSendServerCommand(entref.entnum, 1, va("%c %i %g", 70, priority, fadetime));
}

void PlayerCmd_GetWeaponSlotWeapon(scr_entref_t entref)
{
	gentity_s *ent;
	WeaponDef *weaponDef;
	int slot;
	unsigned short slotName;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	if ( !G_IsPlaying(ent) )
		goto out;

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	if ( ent->client->ps.weaponslots[slot] )
	{
		weaponDef = BG_GetWeaponDef(ent->client->ps.weaponslots[slot]);
		Scr_AddString(weaponDef->szInternalName);
	}
	else
	{
out:
		Scr_AddConstString(scr_const.none);
	}
}

void PlayerCmd_SetWeaponSlotWeapon(scr_entref_t entref)
{
	gentity_s *pSelf;
	int primary;
	int hadWeapon;
	WeaponDef *weaponDef;
	const char *weaponName;
	int ammo;
	int weaponIndex;
	int slotNameStr;
	unsigned short slotNameConst;

	primary = 0;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		pSelf = 0;
	}
	else
	{
		pSelf = &g_entities[entref.entnum];

		if ( !pSelf->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	slotNameConst = Scr_GetConstString(0);
	slotNameStr = BG_GetWeaponSlotForName(SL_ConvertToString(slotNameConst));

	if ( !slotNameStr )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotNameConst)));
	}

	weaponName = Scr_GetString(1u);

	if ( I_stricmp(weaponName, "none") )
	{
		weaponIndex = G_GetWeaponIndexForName(weaponName);

		if ( !weaponIndex )
		{
			Scr_ParamError(1, va("Unknown weapon %s.", weaponName));
		}

		weaponDef = BG_GetWeaponDef(weaponIndex);

		if ( weaponDef->weaponSlot != slotNameStr
		        && (weaponDef->weaponSlot != 1 && weaponDef->weaponSlot != 2 || slotNameStr != 1 && slotNameStr != 2) )
		{
			Scr_ParamError(1, va("Weapon %s goes in the %s weaponslot, not the %s weaponslot.", weaponName, BG_GetWeaponSlotNameForIndex(weaponDef->weaponSlot), BG_GetWeaponSlotNameForIndex(slotNameStr)));
		}
	}
	else
	{
		weaponIndex = 0;
		weaponDef = 0;
	}

	if ( pSelf->client->ps.weaponslots[slotNameStr] )
		BG_TakePlayerWeapon(&pSelf->client->ps, pSelf->client->ps.weaponslots[slotNameStr]);

	if ( weaponIndex )
	{
		if ( slotNameStr == 2 && !pSelf->client->ps.weaponslots[1] )
			primary = 1;

		hadWeapon = COM_BitTest(pSelf->client->ps.weapons, weaponIndex);
		G_GivePlayerWeapon(&pSelf->client->ps, weaponIndex);

		if ( primary )
		{
			pSelf->client->ps.weaponslots[2] = pSelf->client->ps.weaponslots[1];
			pSelf->client->ps.weaponslots[1] = 0;
		}

		ammo = weaponDef->startAmmo - pSelf->client->ps.ammo[weaponDef->ammoIndex];

		if ( ammo > 0 )
			Add_Ammo(pSelf, weaponIndex, ammo, hadWeapon == 0);
	}
}

void PlayerCmd_GetWeaponSlotAmmo(scr_entref_t entref)
{
	gentity_s *ent;
	int clip;
	int ammo;
	int weaponIndex;
	int slot;
	unsigned short slotNameConst;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	if ( !G_IsPlaying(ent) )
		goto out;

	slotNameConst = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotNameConst));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotNameConst)));
	}

	weaponIndex = ent->client->ps.weaponslots[slot];

	if ( ent->client->ps.weaponslots[slot] )
	{
		if ( BG_WeaponIsClipOnly(weaponIndex) )
		{
			clip = BG_ClipForWeapon(weaponIndex);
			Scr_AddInt(ent->client->ps.ammoclip[clip]);
		}
		else
		{
			ammo = BG_AmmoForWeapon(weaponIndex);
			Scr_AddInt(ent->client->ps.ammo[ammo]);
		}
	}
	else
	{
out:
		Scr_AddInt(0);
	}
}

void PlayerCmd_SetWeaponSlotAmmo(scr_entref_t entref)
{
	gentity_s *ent;
	int clipIndex;
	int ammoIndex;
	int ammo;
	int weaponIndex;
	int slot;
	unsigned short slotNameConst;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	slotNameConst = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotNameConst));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotNameConst)));
	}

	ammo = Scr_GetInt(1u);
	weaponIndex = ent->client->ps.weaponslots[slot];

	if ( ent->client->ps.weaponslots[slot] )
	{
		if ( BG_WeaponIsClipOnly(weaponIndex) )
		{
			clipIndex = BG_ClipForWeapon(weaponIndex);

			if ( clipIndex )
			{
				if ( ammo >= 0 )
				{
					if ( ammo > BG_GetClipSize(clipIndex) )
						ammo = BG_GetClipSize(clipIndex);
				}
				else
				{
					ammo = 0;
				}

				ent->client->ps.ammoclip[clipIndex] = ammo;
			}
		}
		else
		{
			ammoIndex = BG_AmmoForWeapon(weaponIndex);

			if ( ammoIndex )
			{
				if ( ammo >= 0 )
				{
					if ( ammo > BG_GetMaxAmmo(ammoIndex) )
						ammo = BG_GetMaxAmmo(ammoIndex);
				}
				else
				{
					ammo = 0;
				}

				ent->client->ps.ammo[ammoIndex] = ammo;
			}
		}
	}
}

void PlayerCmd_GetWeaponSlotClipAmmo(scr_entref_t entref)
{
	gentity_s *ent;
	int clipIndex;
	int slot;
	unsigned short slotNameConst;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	if ( !G_IsPlaying(ent) )
		goto out;

	slotNameConst = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotNameConst));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotNameConst)));
	}

	if ( ent->client->ps.weaponslots[slot] && (clipIndex = BG_ClipForWeapon(ent->client->ps.weaponslots[slot])) != 0 )
		Scr_AddInt(ent->client->ps.ammoclip[clipIndex]);
	else
out:
		Scr_AddInt(0);
}

void PlayerCmd_SetWeaponSlotClipAmmo(scr_entref_t entref)
{
	gentity_s *ent;
	int clipIndex;
	int ammo;
	int slot;
	unsigned short slotNameConst;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	slotNameConst = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotNameConst));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotNameConst)));
	}

	ammo = Scr_GetInt(1u);

	if ( ent->client->ps.weaponslots[slot] )
	{
		clipIndex = BG_ClipForWeapon(ent->client->ps.weaponslots[slot]);

		if ( clipIndex )
		{
			if ( ammo < 0 )
				ammo = 0;

			if ( ammo > BG_GetClipSize(clipIndex) )
				ammo = BG_GetClipSize(clipIndex);

			ent->client->ps.ammoclip[clipIndex] = ammo;
		}
	}
	else
	{
		Scr_AddInt(0);
	}
}

void PlayerCmd_SetWeaponClipAmmo(scr_entref_t entref)
{
	gentity_s *ent;
	int clipIndex;
	int ammo;
	int weaponIndex;
	const char *weaponName;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	weaponName = Scr_GetString(0);
	ammo = Scr_GetInt(1u);
	weaponIndex = G_GetWeaponIndexForName(weaponName);

	if ( weaponIndex )
	{
		clipIndex = BG_ClipForWeapon(weaponIndex);

		if ( clipIndex )
		{
			if ( ammo < 0 )
				ammo = 0;

			if ( ammo > BG_GetClipSize(clipIndex) )
				ammo = BG_GetClipSize(clipIndex);

			ent->client->ps.ammoclip[clipIndex] = ammo;
		}
	}
	else
	{
		Scr_AddInt(0);
	}
}

void iclientprintln(scr_entref_t entref)
{
	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
	}

	Scr_MakeGameMessage(entref.entnum, va("%c", 102));
}

void iclientprintlnbold(scr_entref_t entref)
{
	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
	}

	Scr_MakeGameMessage(entref.entnum, va("%c", 103));
}

void PlayerCmd_spawn(scr_entref_t entref)
{
	gentity_s *pSelf;
	vec3_t spawn_angles;
	vec3_t spawn_origin;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		pSelf = 0;
	}
	else
	{
		pSelf = &g_entities[entref.entnum];

		if ( !pSelf->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	Scr_GetVector(0, spawn_origin);
	Scr_GetVector(1u, spawn_angles);

	ClientSpawn(pSelf, spawn_origin, spawn_angles);
}

void PlayerCmd_setEnterTime(scr_entref_t entref)
{
	gclient_s *client;
	gentity_s *ent;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	client = ent->client;
	client->sess.enterTime = Scr_GetInt(0);
}

extern dvar_t *g_clonePlayerMaxVelocity;
void PlayerCmd_ClonePlayer(scr_entref_t entRef)
{
	gentity_s *entity;
	int i;
	XAnimTree_s *tree;
	corpseInfo_t *corpse;
	int index;
	DObj_s *obj;
	gclient_s *client;
	gentity_s *ent;
	int duration;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		entity = 0;
	}
	else
	{
		entity = &g_entities[entRef.entnum];

		if ( !entity->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entRef.entnum));
		}
	}

	duration = Scr_GetInt(0);
	client = entity->client;
	ent = G_SpawnPlayerClone();
	ent->s.clientNum = client->ps.clientNum;
	ent->s.eFlags = client->ps.eFlags & 0xFFFFFFFD | ent->s.eFlags & 2 | 0xA0000;
	G_SetOrigin(ent, client->ps.origin);
	G_SetAngle(ent, entity->r.currentAngles);
	ent->s.pos.trType = TR_GRAVITY;
	ent->s.pos.trTime = level.time;
	VectorCopy(client->ps.velocity, ent->s.pos.trDelta);
	ent->s.eType = ET_PLAYER_CORPSE;
	ent->physicsObject = 1;
	obj = Com_GetServerDObj(client->ps.clientNum);
	tree = DObjGetTree(obj);

	for ( i = 0; i < 2; ++i )
	{
		if ( ent->s.pos.trDelta[i] > g_clonePlayerMaxVelocity->current.decimal )
			ent->s.pos.trDelta[i] = g_clonePlayerMaxVelocity->current.decimal;
	}

	ent->corpse.deathAnimStartTime = level.time;
	index = G_GetFreePlayerCorpseIndex();
	corpse = &g_scr_data.playerCorpseInfo[index];
	g_scr_data.playerCorpseInfo[index].entnum = ent->s.number;
	corpse->time = level.time;
	corpse->falling = 1;
	memcpy(&corpse->ci, &level_bgs.clientinfo[client->ps.clientNum], sizeof(corpse->ci));
	corpse->ci.pXAnimTree = corpse->tree;
	XAnimCloneAnimTree(tree, g_scr_data.playerCorpseInfo[index].tree);
	ent->s.groundEntityNum = 1023;
	ent->r.svFlags = 2;
	VectorCopy(entity->r.mins, ent->r.mins);
	VectorCopy(entity->r.maxs, ent->r.maxs);
	VectorCopy(entity->r.absmin, ent->r.absmin);
	VectorCopy(entity->r.absmax, ent->r.absmax);
	ent->s.legsAnim = client->ps.legsAnim;
	ent->s.torsoAnim = client->ps.torsoAnim;
	ent->clipmask = 65537;
	ent->r.contents = 67117056;
	SV_LinkEntity(ent);
	ent->nextthink = level.time + duration;
	ent->handler = 12;

	GScr_AddEntity(ent);
}

void PlayerCmd_SetClientDvar(scr_entref_t entref)
{
	unsigned int paramNum;
	char c;
	char szOutString[1024];
	char szString[1024];
	char *pCh;
	char *pszText;
	const char *pszDvar;
	size_t len;
	int i;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
	}

	pszDvar = Scr_GetString(0);

	if ( Scr_GetType(1u) == VAR_ISTRING )
	{
		paramNum = Scr_GetNumParam();
		Scr_ConstructMessageString(1, paramNum - 1, "Client Dvar Value", szString, 1024);
		pszText = szString;
	}
	else
	{
		pszText = (char *)Scr_GetString(1u);
	}

	len = strlen(pszText);

	if ( Dvar_IsValidName(pszDvar) )
	{
		pCh = szOutString;
		memset(szOutString, 0, sizeof(szOutString));
		i = 0;

		while ( i <= 0x1FFF && pszText[i] )
		{
			c = I_CleanChar(pszText[i]);
			*pCh = c;
			if ( *pCh == 34 )
				*pCh = 39;
			++i;
			++pCh;
		}

		SV_GameSendServerCommand(entref.entnum, 1, va("%c %s \"%s\"", 118, pszDvar, szOutString));
	}
	else
	{
		Scr_Error(va("Dvar %s has an invalid dvar name", pszDvar));
	}
}

void ScrCmd_IsLookingAt(scr_entref_t entref)
{
	gclient_s *client;
	gentity_s *pOther;
	gentity_s *pSelf;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		pSelf = 0;
	}
	else
	{
		pSelf = &g_entities[entref.entnum];

		if ( !pSelf->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	client = pSelf->client;
	pOther = Scr_GetEntity(0);
	Scr_AddInt(client->pLookatEnt == pOther);
}

void ScrCmd_PlayLocalSound(scr_entref_t entref)
{
	unsigned char soundIndex;
	const char *soundName;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
	}

	soundName = Scr_GetString(0);
	soundIndex = G_SoundAliasIndex(soundName);

	SV_GameSendServerCommand(entref.entnum, 0, va("%c %i", 115, soundIndex));
}

extern dvar_t *g_voiceChatTalkingDuration;
void PlayerCmd_IsTalking(scr_entref_t entref)
{
	gentity_s *ent;
	int elapsedTime;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	elapsedTime = level.time - ent->client->lastVoiceTime;

	if ( elapsedTime < 0 || elapsedTime >= g_voiceChatTalkingDuration->current.integer )
		Scr_AddInt(0);
	else
		Scr_AddInt(1);
}

void PlayerCmd_AllowSpectateTeam(scr_entref_t entref)
{
	gclient_s *client;
	int spectate;
	gentity_s *ent;
	int teamBit;
	unsigned short teamString;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}

	teamString = Scr_GetConstString(0);
	teamBit = 0;

	if ( teamString == scr_const.axis )
	{
		teamBit = 2;
	}
	else if ( teamString == scr_const.allies )
	{
		teamBit = 4;
	}
	else if ( teamString == scr_const.none )
	{
		teamBit = 1;
	}
	else if ( teamString == scr_const.freelook )
	{
		teamBit = 16;
	}
	else
	{
		Scr_ParamError(0, "team must be \"axis\", \"allies\", \"none\", or \"freelook\"");
	}
	if ( Scr_GetInt(1u) )
	{
		client = ent->client;
		spectate = client->sess.noSpectate & ~teamBit;
	}
	else
	{
		client = ent->client;
		spectate = client->sess.noSpectate | teamBit;
	}

	client->sess.noSpectate = spectate;
}

void PlayerCmd_GetGuid(scr_entref_t entref)
{
	int guid;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(va("entity %i is not a player", entref.entnum));
	}

	if ( Scr_GetNumParam() )
		Scr_Error("USAGE: self getGuid()\n");

	guid = SV_GetGuid(entref.entnum);
	Scr_AddInt(guid);
}

void (*Player_GetMethod(const char **pName))(scr_entref_t)
{
	const char *name;
	unsigned int i;

	name = *pName;

	for ( i = 0; i < COUNT_OF(player_methods); ++i )
	{
		if ( !strcmp(name, player_methods[i].name) )
		{
			*pName = player_methods[i].name;
			return player_methods[i].call;
		}
	}

	return NULL;
}

void BodyEnd(gentity_s *ent)
{
	ent->s.eFlags &= ~0x80000u;
	ent->r.contents = 0x4000000;
	ent->r.svFlags = 0;
}