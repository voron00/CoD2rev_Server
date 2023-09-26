#include "../qcommon/qcommon.h"
#include "g_shared.h"

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

void BodyEnd(gentity_s *ent)
{
	ent->s.eFlags &= ~0x80000u;
	ent->r.contents = 0x4000000;
	ent->r.svFlags = 0;
}

void PlayerCmd_giveWeapon(scr_entref_t entRef)
{
	gentity_s *pSelf;
	int hadWeapon;
	int ammoCount;
	int iWeaponIndex;
	const char *pszWeaponName;
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

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);
	hadWeapon = COM_BitTest(pSelf->client->ps.weapons, iWeaponIndex);
	weaponDef = BG_GetWeaponDef(iWeaponIndex);

	if ( BG_DoesWeaponNeedSlot(iWeaponIndex) && !BG_GetEmptySlotForWeapon(&pSelf->client->ps, iWeaponIndex) )
	{
		Scr_ParamError(0, va("Cannot give %s weapon %s without having an empty weapon slot - player currently has a %s and a %s\n",
		                     pSelf->client->sess.state.name, weaponDef->displayName, BG_GetWeaponDef(pSelf->client->ps.weaponslots[1])->displayName, BG_GetWeaponDef(pSelf->client->ps.weaponslots[2])->displayName));
	}

	if ( G_GivePlayerWeapon(&pSelf->client->ps, iWeaponIndex) )
	{
		SV_GameSendServerCommand(pSelf - g_entities, SV_CMD_CAN_IGNORE, va("%c \"%i\"", 73, 1));
	}

	ammoCount = weaponDef->startAmmo - pSelf->client->ps.ammo[weaponDef->ammoIndex];

	if ( ammoCount > 0 )
		Add_Ammo(pSelf, iWeaponIndex, ammoCount, hadWeapon == 0);
}

void PlayerCmd_takeWeapon(scr_entref_t entref)
{
	gentity_s *pSelf;
	int iWeaponIndex;
	const char *pszWeaponName;

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

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	pSelf->client->ps.ammo[BG_AmmoForWeapon(iWeaponIndex)] = 0;
	pSelf->client->ps.ammoclip[BG_ClipForWeapon(iWeaponIndex)] = 0;

	BG_TakePlayerWeapon(&pSelf->client->ps, iWeaponIndex);
}

void PlayerCmd_takeAllWeapons(scr_entref_t entref)
{
	gentity_s *pSelf;
	int iWeaponIndex;

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

	pSelf->client->ps.weapon = 0;

	for ( iWeaponIndex = 1; iWeaponIndex <= BG_GetNumWeapons(); ++iWeaponIndex )
	{
		pSelf->client->ps.ammo[BG_AmmoForWeapon(iWeaponIndex)] = 0;
		pSelf->client->ps.ammoclip[BG_ClipForWeapon(iWeaponIndex)] = 0;

		BG_TakePlayerWeapon(&pSelf->client->ps, iWeaponIndex);
	}
}

void PlayerCmd_getCurrentWeapon(scr_entref_t entref)
{
	gentity_s *pSelf;
	int iWeaponIndex;
	WeaponDef *weaponDef;

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

	if ( G_IsPlaying(pSelf) && (iWeaponIndex = pSelf->client->ps.weapon, iWeaponIndex > 0) )
	{
		weaponDef = BG_GetWeaponDef(iWeaponIndex);
		Scr_AddString(weaponDef->szInternalName);
	}
	else
	{
		Scr_AddString("none");
	}
}

void PlayerCmd_getCurrentOffhand(scr_entref_t entRef)
{
	gentity_s *pSelf;
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

	if ( G_IsPlaying(pSelf) && pSelf->client->ps.offHandIndex > 0 )
	{
		weaponDef = BG_GetWeaponDef(pSelf->client->ps.offHandIndex);
		Scr_AddString(weaponDef->szInternalName);
	}
	else
	{
		Scr_AddString("none");
	}
}

void PlayerCmd_hasWeapon(scr_entref_t entref)
{
	gentity_s *pSelf;
	int iWeaponIndex;
	const char *pszWeaponName;

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

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = BG_FindWeaponIndexForName(pszWeaponName);

	if ( iWeaponIndex && COM_BitTest(pSelf->client->ps.weapons, iWeaponIndex) )
		Scr_AddBool(1);
	else
		Scr_AddBool(0);
}

void PlayerCmd_switchToWeapon(scr_entref_t entRef)
{
	gentity_s *pSelf;
	int iWeaponIndex;
	const char *pszWeaponName;

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

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( !iWeaponIndex )
	{
		Scr_ParamError(0, va("unknown weapon '%s'", pszWeaponName));
	}

	if ( COM_BitTest(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		G_SelectWeaponIndex(entRef.entnum, iWeaponIndex);
		Scr_AddBool(1);
	}
	else
	{
		Scr_AddBool(0);
	}
}

void PlayerCmd_switchToOffhand(scr_entref_t entref)
{
	gentity_s *pSelf;
	int iWeaponIndex;
	const char *pszWeaponName;

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

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( !iWeaponIndex )
	{
		Scr_ParamError(0, va("unknown weapon '%s'", pszWeaponName));
	}

	if ( COM_BitTest(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		G_SetEquippedOffHand(entref.entnum, iWeaponIndex);
		Scr_AddBool(1);
	}
	else
	{
		Scr_AddBool(0);
	}
}

void PlayerCmd_giveStartAmmo(scr_entref_t entref)
{
	gentity_s *pSelf;
	int ammoCount;
	int iWeaponIndex;
	const char *pszWeaponName;
	WeaponDef *weaponDef;

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

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( COM_BitTest(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		weaponDef = BG_GetWeaponDef(iWeaponIndex);
		ammoCount = weaponDef->startAmmo - pSelf->client->ps.ammo[weaponDef->ammoIndex];

		if ( ammoCount > 0 )
			Add_Ammo(pSelf, iWeaponIndex, ammoCount, 0);
	}
}

void PlayerCmd_giveMaxAmmo(scr_entref_t entRef)
{
	gentity_s *pSelf;
	int ammoCount;
	int iWeaponIndex;
	const char *pszWeaponName;
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

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( COM_BitTest(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		weaponDef = BG_GetWeaponDef(iWeaponIndex);
		ammoCount = BG_GetMaxAmmo(weaponDef->ammoIndex) - pSelf->client->ps.ammo[weaponDef->ammoIndex];

		if ( ammoCount > 0 )
			Add_Ammo(pSelf, iWeaponIndex, ammoCount, 0);
	}
}

void PlayerCmd_getFractionStartAmmo(scr_entref_t entRef)
{
	gentity_s *pSelf;
	float fAmmoFrac;
	int iWeaponIndex;
	const char *pszWeaponName;
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

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( COM_BitTest(pSelf->client->ps.weapons, iWeaponIndex)
	        && (weaponDef = BG_GetWeaponDef(iWeaponIndex), weaponDef->startAmmo > 0) )
	{
		if ( pSelf->client->ps.ammo[weaponDef->ammoIndex] > 0 )
		{
			fAmmoFrac = (float)pSelf->client->ps.ammo[weaponDef->ammoIndex] / (float)weaponDef->startAmmo;
			Scr_AddFloat(fAmmoFrac);
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
	gentity_s *pSelf;
	float fAmmoFrac;
	int iWeaponIndex;
	const char *pszWeaponName;
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

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( COM_BitTest(pSelf->client->ps.weapons, iWeaponIndex)
	        && (weaponDef = BG_GetWeaponDef(iWeaponIndex), BG_GetMaxAmmo(weaponDef->ammoIndex) > 0) )
	{
		if ( pSelf->client->ps.ammo[weaponDef->ammoIndex] > 0 )
		{
			fAmmoFrac = (float)pSelf->client->ps.ammo[weaponDef->ammoIndex] / (float)BG_GetMaxAmmo(weaponDef->ammoIndex);
			Scr_AddFloat(fAmmoFrac);
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
	gentity_s *pSelf;
	vec3_t vNewOrigin;

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

	Scr_GetVector(0, vNewOrigin);
	SV_UnlinkEntity(pSelf);
	VectorCopy(vNewOrigin, pSelf->client->ps.origin);
	pSelf->client->ps.origin[2] = pSelf->client->ps.origin[2] + 1.0;
	pSelf->client->ps.eFlags ^= 2u;
	BG_PlayerStateToEntityState(&pSelf->client->ps, &pSelf->s, 1, 1u);
	VectorCopy(pSelf->client->ps.origin, pSelf->r.currentOrigin);
	SV_LinkEntity(pSelf);
}

void PlayerCmd_setAngles(scr_entref_t entref)
{
	gentity_s *pSelf;
	vec3_t angles;

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

	Scr_GetVector(0, angles);
	SetClientViewAngle(pSelf, angles);
}

void PlayerCmd_getAngles(scr_entref_t entref)
{
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

	Scr_AddVector(pSelf->client->ps.viewangles);
}

void PlayerCmd_useButtonPressed(scr_entref_t entref)
{
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

	Scr_AddBool((pSelf->client->buttonsSinceLastFrame | pSelf->client->buttons) & (KEY_MASK_USE | KEY_MASK_USERELOAD) ? true : false);
}

void PlayerCmd_attackButtonPressed(scr_entref_t entref)
{
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

	Scr_AddBool((pSelf->client->buttonsSinceLastFrame | pSelf->client->buttons) & KEY_MASK_FIRE ? true : false);
}

void PlayerCmd_meleeButtonPressed(scr_entref_t entref)
{
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

	Scr_AddBool((pSelf->client->buttonsSinceLastFrame | pSelf->client->buttons) & KEY_MASK_MELEE ? true : false);
}

void PlayerCmd_playerADS(scr_entref_t entref)
{
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

	Scr_AddFloat(pSelf->client->ps.fWeaponPosFrac);
}

void PlayerCmd_isOnGround(scr_entref_t entref)
{
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

	Scr_AddBool( pSelf->client->ps.groundEntityNum != 1023 );
}

void PlayerCmd_pingPlayer(scr_entref_t entRef)
{
	gentity_s *pSelf;

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

	pSelf->client->ps.eFlags |= 0x400000u;
	pSelf->client->compassPingTime = level.time + 3000;
}

void PlayerCmd_SetViewmodel(scr_entref_t entRef)
{
	gentity_s *pSelf;
	const char *modelName;

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

	modelName = Scr_GetString(0);

	if ( !modelName || !*modelName )
		Scr_ParamError(0, "usage: setviewmodel(<model name>)");

	pSelf->client->sess.viewmodelIndex = G_ModelIndex(modelName);
}

void PlayerCmd_GetViewmodel(scr_entref_t entRef)
{
	gentity_s *pSelf;
	const char *modelName;

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

	modelName = G_ModelName(pSelf->client->sess.viewmodelIndex);
	Scr_AddString(modelName);
}

void PlayerCmd_SayAll(scr_entref_t entref)
{
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

	Scr_ConstructMessageString(0, Scr_GetNumParam() - 1, "Client Chat Message", &szString[1], sizeof(szString) - 1);
	szString[0] = 20;
	G_Say(pSelf, 0, 0, szString);
}

void PlayerCmd_SayTeam(scr_entref_t entref)
{
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

	Scr_ConstructMessageString(0, Scr_GetNumParam() - 1, "Client Chat Message", &szString[1], sizeof(szString) - 1);
	szString[0] = 20;
	G_Say(pSelf, 0, 1, szString);
}

void PlayerCmd_showScoreboard(scr_entref_t entref)
{
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

	Cmd_Score_f(pSelf);
}

void PlayerCmd_setSpawnWeapon(scr_entref_t entRef)
{
	gentity_s *pSelf;
	unsigned int iWeaponIndex;
	const char *pszWeaponName;

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

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( BG_IsWeaponValid(&pSelf->client->ps, iWeaponIndex) )
	{
		pSelf->client->ps.weapon = iWeaponIndex;
		pSelf->client->ps.weaponstate = WEAPON_READY;
	}
}

void PlayerCmd_dropItem(scr_entref_t entref)
{
	gentity_s *pSelf;
	gitem_s *item;
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
	const char *pszModName;
	const char *pszWeaponName;
	gentity_s *tempEnt;
	float knockback;
	gentity_s *pSelf;
	float flinchYawDir;
	float dmgTime;
	float maxTime;
	void (*pain)(struct gentity_s *, struct gentity_s *, int, const float *, const int, const float *, int);
	void (*die)(struct gentity_s *, struct gentity_s *, struct gentity_s *, int, int, const int, const float *, int, int);
	int psTimeOffset;
	int maxDmg;
	vec3_t kVel;
	vec3_t localdir;
	float dmgScale;
	int minDmg;
	int hitloc;
	int iWeaponIndex;
	int iModIndex;
	int dflags;
	int damage;
	const float *point;
	vec3_t vPoint;
	vec_t *dir;
	vec3_t vDir;
	gentity_s *inflictor;
	gentity_s *attacker;

	inflictor = &g_entities[1022];
	attacker = &g_entities[1022];

	dir = 0;
	point = 0;

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

	damage = Scr_GetInt(2u);

	if ( damage > 0 )
	{
		if ( Scr_GetType(0) && Scr_GetPointerType(0) == VAR_ENTITY )
			inflictor = Scr_GetEntity(0);

		if ( Scr_GetType(1u) && Scr_GetPointerType(1u) == VAR_ENTITY )
			attacker = Scr_GetEntity(1u);

		dflags = Scr_GetInt(3u);
		pszModName = Scr_GetString(4u);
		iModIndex = G_IndexForMeansOfDeath(pszModName);
		pszWeaponName = Scr_GetString(5u);
		iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

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

		hitloc = G_GetHitLocationIndexFromString(Scr_GetConstString(8u));
		psTimeOffset = Scr_GetInt(9u);

		if ( dir )
			Vec3NormalizeTo(dir, localdir);
		else
			VectorClear(localdir);

		if ( (pSelf->flags & 8) != 0 || (dflags & 4) != 0 )
		{
			minDmg = 0;
		}
		else
		{
			dmgScale = 0.30000001;

			if ( (pSelf->client->ps.pm_flags & 1) != 0 )
			{
				dmgScale = 0.02;
			}
			else if ( (pSelf->client->ps.pm_flags & 2) != 0 )
			{
				dmgScale = 0.15000001;
			}

			minDmg = (int)((float)damage * dmgScale);

			if ( minDmg > 60 )
				minDmg = 60;

			if ( minDmg )
			{
				if ( (pSelf->client->ps.eFlags & 0x300) == 0 )
				{
					knockback = (float)minDmg * g_knockback->current.decimal / 250.0;
					VectorScale(localdir, knockback, kVel);
					VectorAdd(pSelf->client->ps.velocity, kVel, pSelf->client->ps.velocity);

					if ( !pSelf->client->ps.pm_time )
					{
						maxDmg = 2 * minDmg;

						if ( 2 * minDmg <= 49 )
							maxDmg = 50;

						if ( maxDmg > 200 )
							maxDmg = 200;

						pSelf->client->ps.pm_time = maxDmg;
						pSelf->client->ps.pm_flags |= 0x400u;
					}
				}
			}
		}

		if ( (pSelf->flags & 1) == 0 )
		{
			if ( iWeaponIndex && BG_GetWeaponDef(iWeaponIndex)->weaponType == WEAPTYPE_BULLET )
			{
				if ( BG_GetWeaponDef(iWeaponIndex)->rifleBullet )
					tempEnt = G_TempEntity(vPoint, EV_SHOTGUN_HIT);
				else
					tempEnt = G_TempEntity(vPoint, EV_BULLET_HIT_LARGE);

				tempEnt->s.eventParm = DirToByte(localdir);
				tempEnt->s.scale = DirToByte(localdir);
				tempEnt->s.surfType = 7;
				tempEnt->s.otherEntityNum = attacker->s.number;
				tempEnt->r.clientMask[pSelf->client->ps.clientNum >> 5] |= 1 << (pSelf->client->ps.clientNum & 0x1F);

				if ( BG_GetWeaponDef(iWeaponIndex)->rifleBullet )
					tempEnt = G_TempEntity(vPoint, EV_BULLET_HIT_CLIENT_LARGE);
				else
					tempEnt = G_TempEntity(vPoint, EV_BULLET_HIT_CLIENT_SMALL);

				tempEnt->s.surfType = 7;
				tempEnt->s.otherEntityNum = attacker->s.number;
				tempEnt->s.clientNum = pSelf->client->ps.clientNum;
				tempEnt->r.clientMask[0] = -1;
				tempEnt->r.clientMask[1] = -1;
				tempEnt->r.clientMask[pSelf->client->ps.clientNum >> 5] &= ~(1 << (pSelf->client->ps.clientNum & 0x1F));
			}

			pSelf->client->damage_blood += damage;

			if ( dir )
			{
				VectorCopy(localdir, pSelf->client->damage_from);
				pSelf->client->damage_fromWorld = 0;
			}
			else
			{
				VectorCopy(pSelf->r.currentOrigin, pSelf->client->damage_from);
				pSelf->client->damage_fromWorld = 1;
			}

			if ( (pSelf->flags & 2) != 0 && pSelf->health - damage <= 0 )
				damage = pSelf->health - 1;

			maxTime = player_dmgtimer_maxTime->current.decimal;
			dmgTime = (float)damage * player_dmgtimer_timePerPoint->current.decimal;
			pSelf->client->ps.damageTimer += (int)dmgTime;

			if ( dir )
			{
				pSelf->client->ps.flinchYaw = (int)vectoyaw(dir);
			}
			else
			{
				pSelf->client->ps.flinchYaw = 0;
			}

			flinchYawDir = pSelf->client->ps.viewangles[1];

			if ( flinchYawDir < 0.0 )
				flinchYawDir = flinchYawDir + 360.0;

			pSelf->client->ps.flinchYaw -= (int)flinchYawDir;

			if ( (float)pSelf->client->ps.damageTimer > maxTime )
				pSelf->client->ps.damageTimer = (int)maxTime;

			pSelf->client->ps.damageDuration = pSelf->client->ps.damageTimer;
			pSelf->health -= damage;

			Scr_AddEntity(attacker);
			Scr_AddInt(damage);
			Scr_Notify(pSelf, scr_const.damage, 2u);

			if ( pSelf->health > 0 )
			{
				pain = entityHandlers[pSelf->handler].pain;

				if ( pain )
					pain(pSelf, attacker, damage, point, iModIndex, localdir, hitloc);
			}
			else
			{
				if ( pSelf->health < -999 )
					pSelf->health = -999;

				die = entityHandlers[pSelf->handler].die;

				if ( die )
					die(pSelf, inflictor, attacker, damage, iModIndex, iWeaponIndex, localdir, hitloc, psTimeOffset);

				if ( !pSelf->r.inuse )
					return;
			}

			pSelf->client->ps.stats[STAT_HEALTH] = pSelf->health;
		}
	}
}

void PlayerCmd_Suicide(scr_entref_t entref)
{
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

	pSelf->flags &= 0xFFFFFFFC;
	pSelf->health = 0;
	pSelf->client->ps.stats[STAT_HEALTH] = 0;

	player_die(pSelf, pSelf, pSelf, 100000, 12, 0, 0, HITLOC_NONE, 0);
}

void PlayerCmd_OpenMenu(scr_entref_t entref)
{
	const char *menuName;
	gentity_s *pSelf;
	int iMenuIndex;

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

	if ( pSelf->client->sess.connected == CON_CONNECTED )
	{
		menuName = Scr_GetString(0);
		iMenuIndex = GScr_GetScriptMenuIndex(menuName);
		SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i", 116, iMenuIndex));
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
	gentity_s *pSelf;
	int iMenuIndex;

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

	if ( pSelf->client->sess.connected == CON_CONNECTED )
	{
		menuName = Scr_GetString(0);
		iMenuIndex = GScr_GetScriptMenuIndex(menuName);
		SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i 1", 116, iMenuIndex));
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

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c", 117));
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

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c", 75));
}

void PlayerCmd_FreezeControls(scr_entref_t entref)
{
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

	pSelf->client->bFrozen = Scr_GetInt(0);
}

void PlayerCmd_DisableWeapon(scr_entref_t entref)
{
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

	pSelf->client->ps.pm_flags |= 0x4000000u;
}

void PlayerCmd_EnableWeapon(scr_entref_t entref)
{
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

	pSelf->client->ps.pm_flags &= ~0x4000000u;
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
	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i \"%s\" %g %g %g", 114, priority, roomtype, drylevel, wetlevel, fadetime));
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
	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i %g", 68, priority, fadetime));
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
	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i %i %g", 69, priority, G_FindConfigstringIndex(Scr_GetString(1), 1166, 16, 0, NULL), fadetime));
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
	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i %g", 70, priority, fadetime));
}

void PlayerCmd_GetWeaponSlotWeapon(scr_entref_t entref)
{
	gentity_s *pSelf;
	WeaponDef *weaponDef;
	int slot;
	unsigned short slotNameConst;

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

	if ( !G_IsPlaying(pSelf) )
	{
		Scr_AddConstString(scr_const.none);
		return;
	}

	slotNameConst = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotNameConst));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotNameConst)));
	}

	if ( pSelf->client->ps.weaponslots[slot] )
	{
		weaponDef = BG_GetWeaponDef(pSelf->client->ps.weaponslots[slot]);
		Scr_AddString(weaponDef->szInternalName);
	}
	else
	{
		Scr_AddConstString(scr_const.none);
	}
}

void PlayerCmd_SetWeaponSlotWeapon(scr_entref_t entref)
{
	gentity_s *pSelf;
	int isOnlyWeapon;
	int hadWeapon;
	WeaponDef *weaponDef;
	const char *pszWeaponName;
	int ammoCount;
	int iWeaponIndex;
	int slot;
	unsigned short slotNameConst;

	isOnlyWeapon = 0;

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
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotNameConst));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotNameConst)));
	}

	pszWeaponName = Scr_GetString(1u);

	if ( I_stricmp(pszWeaponName, "none") )
	{
		iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

		if ( !iWeaponIndex )
		{
			Scr_ParamError(1, va("Unknown weapon %s.", pszWeaponName));
		}

		weaponDef = BG_GetWeaponDef(iWeaponIndex);

		if ( weaponDef->weaponSlot != slot
		        && (weaponDef->weaponSlot != SLOT_PRIMARY && weaponDef->weaponSlot != SLOT_PRIMARYB || slot != SLOT_PRIMARY && slot != SLOT_PRIMARYB) )
		{
			Scr_ParamError(1, va("Weapon %s goes in the %s weaponslot, not the %s weaponslot.", pszWeaponName, BG_GetWeaponSlotNameForIndex(weaponDef->weaponSlot), BG_GetWeaponSlotNameForIndex(slot)));
		}
	}
	else
	{
		iWeaponIndex = 0;
		weaponDef = 0;
	}

	if ( pSelf->client->ps.weaponslots[slot] )
		BG_TakePlayerWeapon(&pSelf->client->ps, pSelf->client->ps.weaponslots[slot]);

	if ( iWeaponIndex )
	{
		if ( slot == SLOT_PRIMARYB && !pSelf->client->ps.weaponslots[1] )
			isOnlyWeapon = 1;

		hadWeapon = COM_BitTest(pSelf->client->ps.weapons, iWeaponIndex);
		G_GivePlayerWeapon(&pSelf->client->ps, iWeaponIndex);

		if ( isOnlyWeapon )
		{
			pSelf->client->ps.weaponslots[2] = pSelf->client->ps.weaponslots[1];
			pSelf->client->ps.weaponslots[1] = 0;
		}

		ammoCount = weaponDef->startAmmo - pSelf->client->ps.ammo[weaponDef->ammoIndex];

		if ( ammoCount > 0 )
			Add_Ammo(pSelf, iWeaponIndex, ammoCount, hadWeapon == 0);
	}
}

void PlayerCmd_GetWeaponSlotAmmo(scr_entref_t entref)
{
	gentity_s *pSelf;
	int slot;
	int iWeaponIndex;
	unsigned short slotNameConst;

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

	if ( !G_IsPlaying(pSelf) )
	{
		Scr_AddInt(0);
		return;
	}

	slotNameConst = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotNameConst));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotNameConst)));
	}

	iWeaponIndex = pSelf->client->ps.weaponslots[slot];

	if ( pSelf->client->ps.weaponslots[slot] )
	{
		if ( BG_WeaponIsClipOnly(iWeaponIndex) )
		{
			Scr_AddInt(pSelf->client->ps.ammoclip[BG_ClipForWeapon(iWeaponIndex)]);
		}
		else
		{
			Scr_AddInt(pSelf->client->ps.ammo[BG_AmmoForWeapon(iWeaponIndex)]);
		}
	}
	else
	{
		Scr_AddInt(0);
	}
}

void PlayerCmd_SetWeaponSlotAmmo(scr_entref_t entref)
{
	gentity_s *pSelf;
	int clipIndex;
	int ammoIndex;
	int ammoCount;
	int iWeaponIndex;
	int slot;
	unsigned short slotNameConst;

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
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotNameConst));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotNameConst)));
	}

	ammoCount = Scr_GetInt(1u);
	iWeaponIndex = pSelf->client->ps.weaponslots[slot];

	if ( pSelf->client->ps.weaponslots[slot] )
	{
		if ( BG_WeaponIsClipOnly(iWeaponIndex) )
		{
			clipIndex = BG_ClipForWeapon(iWeaponIndex);

			if ( clipIndex )
			{
				if ( ammoCount >= 0 )
				{
					if ( ammoCount > BG_GetClipSize(clipIndex) )
						ammoCount = BG_GetClipSize(clipIndex);
				}
				else
				{
					ammoCount = 0;
				}

				pSelf->client->ps.ammoclip[clipIndex] = ammoCount;
			}
		}
		else
		{
			ammoIndex = BG_AmmoForWeapon(iWeaponIndex);

			if ( ammoIndex )
			{
				if ( ammoCount >= 0 )
				{
					if ( ammoCount > BG_GetMaxAmmo(ammoIndex) )
						ammoCount = BG_GetMaxAmmo(ammoIndex);
				}
				else
				{
					ammoCount = 0;
				}

				pSelf->client->ps.ammo[ammoIndex] = ammoCount;
			}
		}
	}
}

void PlayerCmd_GetWeaponSlotClipAmmo(scr_entref_t entref)
{
	gentity_s *pSelf;
	int clipIndex;
	int slot;
	unsigned short slotNameConst;

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

	if ( !G_IsPlaying(pSelf) )
	{
		Scr_AddInt(0);
		return;
	}

	slotNameConst = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotNameConst));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotNameConst)));
	}

	if ( pSelf->client->ps.weaponslots[slot] && (clipIndex = BG_ClipForWeapon(pSelf->client->ps.weaponslots[slot])) != 0 )
		Scr_AddInt(pSelf->client->ps.ammoclip[clipIndex]);
	else

		Scr_AddInt(0);
}

void PlayerCmd_SetWeaponSlotClipAmmo(scr_entref_t entref)
{
	gentity_s *pSelf;
	int clipIndex;
	int ammoCount;
	int slot;
	unsigned short slotNameConst;

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
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotNameConst));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotNameConst)));
	}

	ammoCount = Scr_GetInt(1u);

	if ( pSelf->client->ps.weaponslots[slot] )
	{
		clipIndex = BG_ClipForWeapon(pSelf->client->ps.weaponslots[slot]);

		if ( clipIndex )
		{
			if ( ammoCount < 0 )
				ammoCount = 0;

			if ( ammoCount > BG_GetClipSize(clipIndex) )
				ammoCount = BG_GetClipSize(clipIndex);

			pSelf->client->ps.ammoclip[clipIndex] = ammoCount;
		}
	}
	else
	{
		Scr_AddInt(0);
	}
}

void PlayerCmd_SetWeaponClipAmmo(scr_entref_t entref)
{
	gentity_s *pSelf;
	int clipIndex;
	int ammoCount;
	int weaponIndex;
	const char *weaponName;

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

	weaponName = Scr_GetString(0);
	ammoCount = Scr_GetInt(1u);
	weaponIndex = G_GetWeaponIndexForName(weaponName);

	if ( weaponIndex )
	{
		clipIndex = BG_ClipForWeapon(weaponIndex);

		if ( clipIndex )
		{
			if ( ammoCount < 0 )
				ammoCount = 0;

			if ( ammoCount > BG_GetClipSize(clipIndex) )
				ammoCount = BG_GetClipSize(clipIndex);

			pSelf->client->ps.ammoclip[clipIndex] = ammoCount;
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

	pSelf->client->sess.enterTime = Scr_GetInt(0);
}

extern dvar_t *g_clonePlayerMaxVelocity;
void PlayerCmd_ClonePlayer(scr_entref_t entRef)
{
	gentity_s *pSelf;
	int i;
	XAnimTree_s *tree;
	corpseInfo_t *corpseInfo;
	int corpseIndex;
	DObj_s *obj;
	gentity_s *body;
	int deathAnimDuration;

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

	deathAnimDuration = Scr_GetInt(0);
	body = G_SpawnPlayerClone();
	body->s.clientNum = pSelf->client->ps.clientNum;
	body->s.eFlags = pSelf->client->ps.eFlags & 0xFFFFFFFD | body->s.eFlags & 2 | 0xA0000;
	G_SetOrigin(body, pSelf->client->ps.origin);
	G_SetAngle(body, pSelf->r.currentAngles);
	body->s.pos.trType = TR_GRAVITY;
	body->s.pos.trTime = level.time;
	VectorCopy(pSelf->client->ps.velocity, body->s.pos.trDelta);
	body->s.eType = ET_PLAYER_CORPSE;
	body->physicsObject = 1;
	obj = Com_GetServerDObj(pSelf->client->ps.clientNum);
	tree = DObjGetTree(obj);

	for ( i = 0; i < 2; ++i )
	{
		if ( body->s.pos.trDelta[i] > g_clonePlayerMaxVelocity->current.decimal )
			body->s.pos.trDelta[i] = g_clonePlayerMaxVelocity->current.decimal;
	}

	body->corpse.deathAnimStartTime = level.time;
	corpseIndex = G_GetFreePlayerCorpseIndex();
	corpseInfo = &g_scr_data.playerCorpseInfo[corpseIndex];
	g_scr_data.playerCorpseInfo[corpseIndex].entnum = body->s.number;
	corpseInfo->time = level.time;
	corpseInfo->falling = 1;
	memcpy(&corpseInfo->ci, &level_bgs.clientinfo[pSelf->client->ps.clientNum], sizeof(corpseInfo->ci));
	corpseInfo->ci.pXAnimTree = corpseInfo->tree;
	XAnimCloneAnimTree(tree, g_scr_data.playerCorpseInfo[corpseIndex].tree);
	body->s.groundEntityNum = 1023;
	body->r.svFlags = 2;
	VectorCopy(pSelf->r.mins, body->r.mins);
	VectorCopy(pSelf->r.maxs, body->r.maxs);
	VectorCopy(pSelf->r.absmin, body->r.absmin);
	VectorCopy(pSelf->r.absmax, body->r.absmax);
	body->s.legsAnim = pSelf->client->ps.legsAnim;
	body->s.torsoAnim = pSelf->client->ps.torsoAnim;
	body->clipmask = 65537;
	body->r.contents = CONTENTS_CORPSE | CONTENTS_CLIPSHOT;
	SV_LinkEntity(body);
	body->nextthink = level.time + deathAnimDuration;
	body->handler = ENT_HANDLER_PLAYER_CLONE;

	GScr_AddEntity(body);
}

void PlayerCmd_SetClientDvar(scr_entref_t entref)
{
	char c;
	char szOutString[1024];
	char szString[1024];
	char *pCh;
	char *pszText;
	const char *pszDvar;
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
		Scr_ConstructMessageString(1, Scr_GetNumParam() - 1, "Client Dvar Value", szString, 1024);
		pszText = szString;
	}
	else
	{
		pszText = (char *)Scr_GetString(1u);
	}

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

		SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %s \"%s\"", 118, pszDvar, szOutString));
	}
	else
	{
		Scr_Error(va("Dvar %s has an invalid dvar name", pszDvar));
	}
}

void ScrCmd_IsLookingAt(scr_entref_t entref)
{
	gentity_s *pSelf;
	gentity_s *pOther;

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

	pOther = Scr_GetEntity(0);
	Scr_AddInt(pSelf->client->pLookatEnt == pOther);
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

	SV_GameSendServerCommand(entref.entnum, SV_CMD_CAN_IGNORE, va("%c %i", 115, soundIndex));
}

extern dvar_t *g_voiceChatTalkingDuration;
void PlayerCmd_IsTalking(scr_entref_t entref)
{
	gentity_s *pSelf;
	int elapsedTime;

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

	elapsedTime = level.time - pSelf->client->lastVoiceTime;

	if ( elapsedTime < 0 || elapsedTime >= g_voiceChatTalkingDuration->current.integer )
		Scr_AddInt(0);
	else
		Scr_AddInt(1);
}

void PlayerCmd_AllowSpectateTeam(scr_entref_t entref)
{
	gentity_s *pSelf;
	int teamBit;
	unsigned short teamString;

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
		pSelf->client->sess.noSpectate &= ~teamBit;
	else
		pSelf->client->sess.noSpectate |= teamBit;
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
