#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../clientscript/clientscript_public.h"

#ifdef TESTING_LIBRARY
#define g_scr_data (*(scr_data_t*)( 0x0879C780 ))
#else
scr_data_t g_scr_data;
#endif

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
#define bg_itemlist ((gitem_t*)( 0x08164C20 ))
#else
extern gitem_t bg_itemlist[];
#endif

#ifdef TESTING_LIBRARY
#define bgs (*((bgs_t**)( 0x0855A4E0 )))
#else
extern bgs_t *bgs;
#endif

#ifdef TESTING_LIBRARY
#define level_bgs (*((bgs_t*)( 0x0859EA40 )))
#else
extern bgs_t level_bgs;
#endif

scr_function_t functions[] =
{
	{ "print", print, 1, },
	{ "println", println, 1, },
	{ "iprintln", iprintln, 0, },
	{ "iprintlnbold", iprintlnbold, 0, },
	{ "print3d", print3d, 1, },
	{ "line", GScr_line, 1, },
	{ "getent", Scr_GetEnt, 0, },
	{ "getentarray", Scr_GetEntArray, 0, },
	{ "spawn", GScr_Spawn, 0, },
	{ "spawnturret", GScr_SpawnTurret, 0, },
	{ "precacheturret", GScr_PrecacheTurret, 0, },
	{ "spawnstruct", Scr_AddStruct, 0, },
	{ "assert", assertCmd, 1, },
	{ "assertex", assertexCmd, 1, },
	{ "assertmsg", assertmsgCmd, 1, },
	{ "isdefined", GScr_IsDefined, 0, },
	{ "isstring", GScr_IsString, 0, },
	{ "isalive", GScr_IsAlive, 0, },
	{ "getcvar", GScr_GetDvar, 0, },
	{ "getcvarint", GScr_GetDvarInt, 0, },
	{ "getcvarfloat", GScr_GetDvarFloat, 0, },
	{ "setcvar", GScr_SetDvar, 0, },
	{ "gettime", GScr_GetTime, 0, },
	{ "getentbynum", Scr_GetEntByNum, 1, },
	{ "getweaponmodel", Scr_GetWeaponModel, 0, },
	{ "getanimlength", GScr_GetAnimLength, 0, },
	{ "animhasnotetrack", GScr_AnimHasNotetrack, 0, },
	{ "getbrushmodelcenter", GScr_GetBrushModelCenter, 0, },
	{ "objective_add", Scr_Objective_Add, 0, },
	{ "objective_delete", Scr_Objective_Delete, 0, },
	{ "objective_state", Scr_Objective_State, 0, },
	{ "objective_icon", Scr_Objective_Icon, 0, },
	{ "objective_position", Scr_Objective_Position, 0, },
	{ "objective_onentity", Scr_Objective_OnEntity, 0, },
	{ "objective_current", Scr_Objective_Current, 0, },
	{ "bullettrace", Scr_BulletTrace, 0, },
	{ "bullettracepassed", Scr_BulletTracePassed, 0, },
	{ "sighttracepassed", Scr_SightTracePassed, 0, },
	{ "physicstrace", Scr_PhysicsTrace, 0, },
	{ "getmovedelta", GScr_GetMoveDelta, 0, },
	{ "getangledelta", GScr_GetAngleDelta, 0, },
	{ "getnorthyaw", GScr_GetNorthYaw, 0, },
	{ "randomint", Scr_RandomInt, 0, },
	{ "randomfloat", Scr_RandomFloat, 0, },
	{ "randomintrange", Scr_RandomIntRange, 0, },
	{ "randomfloatrange", Scr_RandomFloatRange, 0, },
	{ "sin", GScr_sin, 0, },
	{ "cos", GScr_cos, 0, },
	{ "tan", GScr_tan, 0, },
	{ "asin", GScr_asin, 0, },
	{ "acos", GScr_acos, 0, },
	{ "atan", GScr_atan, 0, },
	{ "int", GScr_CastInt, 0, },
	{ "distance", Scr_Distance, 0, },
	{ "distancesquared", Scr_DistanceSquared, 0, },
	{ "length", Scr_Length, 0, },
	{ "lengthsquared", Scr_LengthSquared, 0, },
	{ "closer", Scr_Closer, 0, },
	{ "vectordot", Scr_VectorDot, 0, },
	{ "vectornormalize", Scr_VectorNormalize, 0, },
	{ "vectortoangles", Scr_VectorToAngles, 0, },
	{ "anglestoup", Scr_AnglesToUp, 0, },
	{ "anglestoright", Scr_AnglesToRight, 0, },
	{ "anglestoforward", Scr_AnglesToForward, 0, },
	{ "issubstr", Scr_IsSubStr, 0, },
	{ "getsubstr", Scr_GetSubStr, 0, },
	{ "tolower", Scr_ToLower, 0, },
	{ "strtok", Scr_StrTok, 0, },
	{ "musicplay", Scr_MusicPlay, 0, },
	{ "musicstop", Scr_MusicStop, 0, },
	{ "soundfade", Scr_SoundFade, 0, },
	{ "ambientplay", Scr_AmbientPlay, 0, },
	{ "ambientstop", Scr_AmbientStop, 0, },
	{ "precachemodel", Scr_PrecacheModel, 0, },
	{ "precacheshellshock", Scr_PrecacheShellShock, 0, },
	{ "precacheitem", Scr_PrecacheItem, 0, },
	{ "precacheshader", Scr_PrecacheShader, 0, },
	{ "precachestring", Scr_PrecacheString, 0, },
	{ "precacherumble", Scr_PrecacheRumble, 0, },
	{ "loadfx", Scr_LoadFX, 0, },
	{ "playfx", Scr_PlayFX, 0, },
	{ "playfxontag", Scr_PlayFXOnTag, 0, },
	{ "playloopedfx", Scr_PlayLoopedFX, 0, },
	{ "setcullfog", Scr_SetLinearFog, 0, },
	{ "setexpfog", Scr_SetExponentialFog, 0, },
	{ "grenadeexplosioneffect", Scr_GrenadeExplosionEffect, 0, },
	{ "radiusdamage", GScr_RadiusDamage, 0, },
	{ "setplayerignoreradiusdamage", GScr_SetPlayerIgnoreRadiusDamage, 0, },
	{ "getnumparts", GScr_GetNumParts, 0, },
	{ "getpartname", GScr_GetPartName, 0, },
	{ "earthquake", GScr_Earthquake, 0, },
	{ "newhudelem", GScr_NewHudElem, 0, },
	{ "newclienthudelem", GScr_NewClientHudElem, 0, },
	{ "newteamhudelem", GScr_NewTeamHudElem, 0, },
	{ "resettimeout", Scr_ResetTimeout, 0, },
	{ "isplayer", GScr_IsPlayer, 0, },
	{ "isplayernumber", GScr_IsPlayerNumber, 0, },
	{ "setwinningplayer", GScr_SetWinningPlayer, 0, },
	{ "setwinningteam", GScr_SetWinningTeam, 0, },
	{ "announcement", GScr_Announcement, 0, },
	{ "clientannouncement", GScr_ClientAnnouncement, 0, },
	{ "getteamscore", GScr_GetTeamScore, 0, },
	{ "setteamscore", GScr_SetTeamScore, 0, },
	{ "setclientnamemode", GScr_SetClientNameMode, 0, },
	{ "updateclientnames", GScr_UpdateClientNames, 0, },
	{ "getteamplayersalive", GScr_GetTeamPlayersAlive, 0, },
	{ "objective_team", GScr_Objective_Team, 0, },
	{ "logprint", GScr_LogPrint, 0, },
	{ "worldentnumber", GScr_WorldEntNumber, 0, },
	{ "obituary", GScr_Obituary, 0, },
	{ "positionwouldtelefrag", GScr_positionWouldTelefrag, 0, },
	{ "getstarttime", GScr_getStartTime, 0, },
	{ "precachemenu", GScr_PrecacheMenu, 0, },
	{ "precachestatusicon", GScr_PrecacheStatusIcon, 0, },
	{ "precacheheadicon", GScr_PrecacheHeadIcon, 0, },
	{ "map_restart", GScr_MapRestart, 0, },
	{ "exitlevel", GScr_ExitLevel, 0, },
	{ "addtestclient", GScr_AddTestClient, 0, },
	{ "makecvarserverinfo", GScr_MakeDvarServerInfo, 0, },
	{ "setarchive", GScr_SetArchive, 0, },
	{ "allclientsprint", GScr_AllClientsPrint, 0, },
	{ "clientprint", GScr_ClientPrint, 0, },
	{ "mapexists", GScr_MapExists, 0, },
	{ "isvalidgametype", GScr_IsValidGameType, 0, },
	{ "matchend", GScr_MatchEnd, 0, },
	{ "setplayerteamrank", GScr_SetPlayerTeamRank, 0, },
	{ "sendranks", GScr_SendXboxLiveRanks, 0, },
	{ "setvotestring", GScr_SetVoteString, 0, },
	{ "setvotetime", GScr_SetVoteTime, 0, },
	{ "setvoteyescount", GScr_SetVoteYesCount, 0, },
	{ "setvotenocount", GScr_SetVoteNoCount, 0, },
	{ "openfile", GScr_OpenFile, 0, },
	{ "closefile", GScr_CloseFile, 0, },
	{ "fprintln", GScr_FPrintln, 0, },
	{ "freadln", GScr_FReadLn, 0, },
	{ "fgetarg", GScr_FGetArg, 0, },
	{ "kick", GScr_KickPlayer, 0, },
	{ "ban", GScr_BanPlayer, 0, },
	{ "map", GScr_LoadMap, 0, },
	{ "playrumbleonpos", Scr_PlayRumbleOnPos, 0, },
	{ "playlooprumbleonpos", Scr_PlayLoopRumbleOnPos, 0, },
	{ "stopallrumbles", Scr_StopAllRumbles, 0, },
	{ "soundexists", ScrCmd_SoundExists, 0, },
	{ "issplitscreen", Scr_IsSplitscreen, 0, },
	{ "endparty", GScr_EndXboxLiveLobby, 0, },
};

scr_method_t methods[] =
{
	{ "attach", ScrCmd_attach, 0, },
	{ "detach", ScrCmd_detach, 0, },
	{ "detachall", ScrCmd_detachAll, 0, },
	{ "getattachsize", ScrCmd_GetAttachSize, 0, },
	{ "getattachmodelname", ScrCmd_GetAttachModelName, 0, },
	{ "getattachtagname", ScrCmd_GetAttachTagName, 0, },
	{ "getattachignorecollision", ScrCmd_GetAttachIgnoreCollision, 0, },
	{ "getammocount", GScr_GetAmmoCount, 0, },
	{ "getclanid", ScrCmd_GetClanId, 0, },
	{ "getclanname", ScrCmd_GetClanName, 0, },
	{ "getclandescription", ScrCmd_GetClanDescription, 0, },
	{ "getclanmotto", ScrCmd_GetClanMotto, 0, },
	{ "getclanurl", ScrCmd_GetClanURL, 0, },
	{ "linkto", ScrCmd_LinkTo, 0, },
	{ "unlink", ScrCmd_Unlink, 0, },
	{ "enablelinkto", ScrCmd_EnableLinkTo, 0, },
	{ "getorigin", ScrCmd_GetOrigin, 0, },
	{ "geteye", ScrCmd_GetEye, 0, },
	{ "useby", ScrCmd_UseBy, 0, },
	{ "setstablemissile", Scr_SetStableMissile, 0, },
	{ "istouching", ScrCmd_IsTouching, 0, },
	{ "playsound", ScrCmd_PlaySound, 0, },
	{ "playsoundasmaster", ScrCmd_PlaySoundAsMaster, 0, },
	{ "playloopsound", ScrCmd_PlayLoopSound, 0, },
	{ "stoploopsound", ScrCmd_StopLoopSound, 0, },
	{ "playrumble", ScrCmd_PlayRumble, 0, },
	{ "playlooprumble", ScrCmd_PlayLoopRumble, 0, },
	{ "stoprumble", ScrCmd_StopRumble, 0, },
	{ "delete", ScrCmd_Delete, 0, },
	{ "setmodel", ScrCmd_SetModel, 0, },
	{ "getnormalhealth", ScrCmd_GetNormalHealth, 0, },
	{ "setnormalhealth", ScrCmd_SetNormalHealth, 0, },
	{ "show", ScrCmd_Show, 0, },
	{ "hide", ScrCmd_Hide, 0, },
	{ "showtoplayer", ScrCmd_ShowToPlayer, 0, },
	{ "setcontents", ScrCmd_SetContents, 0, },
	{ "setcursorhint", GScr_SetCursorHint, 0, },
	{ "sethintstring", GScr_SetHintString, 0, },
	{ "shellshock", GScr_ShellShock, 0, },
	{ "stopshellshock", GScr_StopShellShock, 0, },
	{ "viewkick", GScr_ViewKick, 0, },
	{ "localtoworldcoords", GScr_LocalToWorldCoords, 0, },
	{ "setrightarc", GScr_SetRightArc, 0, },
	{ "setleftarc", GScr_SetLeftArc, 0, },
	{ "settoparc", GScr_SetTopArc, 0, },
	{ "setbottomarc", GScr_SetBottomArc, 0, },
	{ "getentitynumber", GScr_GetEntityNumber, 0, },
	{ "enablegrenadetouchdamage", GScr_EnableGrenadeTouchDamage, 0, },
	{ "disablegrenadetouchdamage", GScr_DisableGrenadeTouchDamage, 0, },
	{ "enablegrenadebounce", GScr_EnableGrenadeBounce, 0, },
	{ "disablegrenadebounce", GScr_DisableGrenadeBounce, 0, },
	{ "enableaimassist", GScr_EnableAimAssist, 0, },
	{ "disableaimassist", GScr_DisableAimAssist, 0, },
	{ "placespawnpoint", GScr_PlaceSpawnPoint, 0, },
	{ "updatescores", GScr_UpdateScores, 0, },
	{ "setteamfortrigger", GScr_SetTeamForTrigger, 0, },
	{ "clientclaimtrigger", GScr_ClientClaimTrigger, 0, },
	{ "clientreleasetrigger", GScr_ClientReleaseTrigger, 0, },
	{ "releaseclaimedtrigger", GScr_ReleaseClaimedTrigger, 0, },
};

const char *modNames[] =
{
	"MOD_UNKNOWN",
	"MOD_PISTOL_BULLET",
	"MOD_RIFLE_BULLET",
	"MOD_GRENADE",
	"MOD_GRENADE_SPLASH",
	"MOD_PROJECTILE",
	"MOD_PROJECTILE_SPLASH",
	"MOD_MELEE",
	"MOD_HEAD_SHOT",
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TRIGGER_HURT",
	"MOD_EXPLOSIVE",
};

vec3_t playerMins = { -15.0, -15.0, 0.0 };
vec3_t playerMaxs = { 15.0, 15.0, 70.0 };

gentity_t* GetEntity(scr_entref_t entRef)
{
	if ( entRef.classnum == CLASS_NUM_ENTITY )
		return &g_entities[entRef.entnum];

	Scr_ObjectError("not an entity");
	return 0;
}

gentity_t* GetPlayerEntity(scr_entref_t entref)
{
	const char *classname;
	const char *targetname;
	gentity_s *ent;

	ent = GetEntity(entref);

	if ( !ent->client )
	{
		if ( ent->targetname )
			targetname = SL_ConvertToString(ent->targetname);
		else
			targetname = "<undefined>";

		classname = SL_ConvertToString(ent->classname);
		Scr_Error(va("only valid on players; called on entity %i at %.0f %.0f %.0f classname %s targetname %s\n", entref.entnum, ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2], classname, targetname));
	}

	return ent;
}

char* Scr_GetGameTypeNameForScript(const char *pszGameTypeScript)
{
	int i;

	for ( i = 0; i < g_scr_data.gametype.iNumGameTypes; ++i )
	{
		if ( !Q_stricmp(g_scr_data.gametype.list[i].pszScript, pszGameTypeScript) )
		{
			return g_scr_data.gametype.list[i].pszName;
		}
	}

	return 0;
}

qboolean Scr_IsValidGameType(const char *pszGameType)
{
	return Scr_GetGameTypeNameForScript(pszGameType) != 0;
}

void Scr_ValidateLocalizedStringRef(int parmIndex, const char *token, int tokenLen)
{
	int charIter;

	if ( tokenLen > 1 )
	{
		for ( charIter = 0; charIter < tokenLen; ++charIter )
		{
			if ( !isalnum(token[charIter]) && token[charIter] != '_' )
			{
				Scr_ParamError(parmIndex, va("Illegal localized string reference: %s must contain only alpha-numeric characters and underscores", token));
			}
		}
	}
}

extern dvar_t *loc_warnings;
extern dvar_t *loc_warningsAsErrors;
void Scr_ConstructMessageString(int firstParmIndex, int lastParmIndex, const char *errorContext, char *string, unsigned int stringLimit)
{
	unsigned int charIndex;
	unsigned int tokenLen;
	int type;
	gentity_t *ent;
	int parmIndex;
	const char *token;
	unsigned int stringLen;

	stringLen = 0;

	for ( parmIndex = firstParmIndex; parmIndex <= lastParmIndex; ++parmIndex )
	{
		type = Scr_GetType(parmIndex);

		if ( type == VAR_ISTRING )
		{
			token = Scr_GetIString(parmIndex);
			tokenLen = strlen(token);
			Scr_ValidateLocalizedStringRef(parmIndex, token, tokenLen);

			if ( stringLen + tokenLen + 1 >= stringLimit )
			{
				Scr_ParamError(parmIndex, va("%s is too long. Max length is %i\n", errorContext, stringLimit));
			}

			if ( stringLen )
			{
				string[stringLen++] = 20;
			}
		}
		else if ( type != VAR_OBJECT || Scr_GetPointerType(parmIndex) != VAR_ENTITY )
		{
			token = Scr_GetString(parmIndex);
			tokenLen = strlen(token);

			for ( charIndex = 0; charIndex < tokenLen; ++charIndex )
			{
				if ( token[charIndex] == 20 || token[charIndex] == 21 || token[charIndex] == 22 )
				{
					Scr_ParamError(parmIndex, va("bad escape character (%i) present in string", token[charIndex]));
				}

				if ( isalpha(token[charIndex]) )
				{
					if ( loc_warnings->current.boolean )
					{
						if ( loc_warningsAsErrors->current.boolean )
						{
							Scr_LocalizationError(parmIndex, va("non-localized %s strings are not allowed to have letters in them: \"%s\"", errorContext, token));
						}
						else
						{
							Com_Printf("WARNING: Non-localized %s string is not allowed to have letters in it. Must be changed over to a localized string: \"%s\"\n",
							           errorContext, token);
						}
					}
					break;
				}
			}

			if ( stringLen + tokenLen + 1 >= stringLimit )
			{
				Scr_ParamError(parmIndex, va("%s is too long. Max length is %i\n", errorContext, stringLimit));
			}

			if ( tokenLen )
			{
				string[stringLen++] = 21;
			}
		}
		else
		{
			ent = Scr_GetEntity(parmIndex);

			if ( !ent->client )
			{
				Scr_ParamError(parmIndex, "Entity is not a player");
			}

			token = va("%s^7", ent->client->sess.state.name);
			tokenLen = strlen(token);

			if ( stringLen + tokenLen + 1 >= stringLimit )
			{
				Scr_ParamError(parmIndex, va("%s is too long. Max length is %i\n", errorContext, stringLimit));
			}

			if ( tokenLen )
			{
				string[stringLen++] = 21;
			}
		}

		for ( charIndex = 0; charIndex < tokenLen; ++charIndex )
		{
			if ( token[charIndex] != 20 && token[charIndex] != 21 && token[charIndex] != 22 )
			{
				string[stringLen] = token[charIndex];
			}
			else
			{
				string[stringLen] = '.';
			}

			++stringLen;
		}
	}

	string[stringLen] = 0;
}

void Scr_SetOrigin(gentity_s *ent, int offset)
{
	vec3_t origin;

	Scr_GetVector(0, origin);
	G_SetOrigin(ent, origin);

	if ( ent->r.linked )
		SV_LinkEntity(ent);
}

void Scr_SetAngles(gentity_s *ent, int offset)
{
	vec3_t angles;

	Scr_GetVector(0, angles);
	G_SetAngle(ent, angles);
}

void Scr_SetHealth(gentity_s *ent, int offset)
{
	int health;

	health = Scr_GetInt(0);

	if ( ent->client )
	{
		ent->health = health;
		ent->client->ps.stats[0] = health;
	}
	else
	{
		ent->maxHealth = health;
		ent->health = health;
	}
}

void GScr_AddVector(const float *vVec)
{
	if ( vVec )
		Scr_AddVector(vVec);
	else
		Scr_AddUndefined();
}

void GScr_AddEntity(gentity_s *pEnt)
{
	if ( pEnt )
		Scr_AddEntity(pEnt);
	else
		Scr_AddUndefined();
}

void Scr_PlayerConnect(gentity_s *self)
{
	unsigned short callback;

	callback = Scr_ExecEntThread(self, g_scr_data.gametype.playerconnect, 0);
	Scr_FreeThread(callback);
}

unsigned int GScr_AllocString(const char *string)
{
	return Scr_AllocString(string);
}

int GScr_GetHeadIconIndex(const char *pszIcon)
{
	char dest[MAX_STRING_CHARS];
	int i;

	if ( !*pszIcon )
		return 0;

	for ( i = 0; i < 15; ++i )
	{
		SV_GetConfigstring(i + 31, dest, 1024);

		if ( !I_stricmp(dest, pszIcon) )
			return i + 1;
	}

	Scr_Error(va("Head icon '%s' was not precached\n", pszIcon));
	return 0;
}

int GScr_GetStatusIconIndex(const char *pszIcon)
{
	char dest[MAX_STRING_CHARS];
	int i;

	if ( !*pszIcon )
		return 0;

	for ( i = 0; i < 8; ++i )
	{
		SV_GetConfigstring(i + 23, dest, 1024);

		if ( !I_stricmp(dest, pszIcon) )
			return i + 1;
	}

	Scr_Error(va("Status icon '%s' was not precached\n", pszIcon));
	return 0;
}

int G_GetHintStringIndex(int *piIndex, const char *pszString)
{
	char string[1024];
	int i;

	for ( i = 0; i < 32; ++i )
	{
		SV_GetConfigstring(i + 1278, string, 1024);

		if ( !string[0] )
		{
			SV_SetConfigstring(i + 1278, pszString);
			*piIndex = i;
			return 1;
		}

		if ( !strcmp(pszString, string) )
		{
			*piIndex = i;
			return 1;
		}
	}

	*piIndex = -1;
	return 0;
}

void Scr_PlayerDamage(gentity_s *self, gentity_s *inflictor, gentity_s *attacker, int damage, int dflags, unsigned int meansOfDeath, int iWeapon, const float *vPoint, const float *vDir, int hitLoc, int timeOffset)
{
	unsigned short hitLocStr;
	WeaponDef *weaponDef;
	unsigned short callback;

	Scr_AddInt(timeOffset);
	hitLocStr = G_GetHitLocationString(hitLoc);
	Scr_AddConstString(hitLocStr);
	GScr_AddVector(vDir);
	GScr_AddVector(vPoint);
	weaponDef = BG_GetWeaponDef(iWeapon);
	Scr_AddString(weaponDef->szInternalName);

	if ( meansOfDeath < 0xF )
		Scr_AddString(modNames[meansOfDeath]);
	else
		Scr_AddString("badMOD");

	Scr_AddInt(dflags);
	Scr_AddInt(damage);
	GScr_AddEntity(attacker);
	GScr_AddEntity(inflictor);
	callback = Scr_ExecEntThread(self, g_scr_data.gametype.playerdamage, 0xAu);
	Scr_FreeThread(callback);
}

void Scr_PlayerKilled(gentity_s *self, gentity_s *inflictor, gentity_s *attacker, int damage, unsigned int meansOfDeath, int iWeapon, const float *vDir, int hitLoc, int psTimeOffset, int deathAnimDuration)
{
	unsigned short hitLocStr;
	WeaponDef *weaponDef;
	unsigned short callback;

	Scr_AddInt(deathAnimDuration);
	Scr_AddInt(psTimeOffset);
	hitLocStr = G_GetHitLocationString(hitLoc);
	Scr_AddConstString(hitLocStr);
	GScr_AddVector(vDir);
	weaponDef = BG_GetWeaponDef(iWeapon);
	Scr_AddString(weaponDef->szInternalName);

	if ( meansOfDeath < 0xF )
		Scr_AddString(modNames[meansOfDeath]);
	else
		Scr_AddString("badMOD");

	Scr_AddInt(damage);
	GScr_AddEntity(attacker);
	GScr_AddEntity(inflictor);
	callback = Scr_ExecEntThread(self, g_scr_data.gametype.playerkilled, 9u);
	Scr_FreeThread(callback);
}

void Scr_PlayerDisconnect(gentity_s *self)
{
	unsigned short callback;

	callback = Scr_ExecEntThread(self, g_scr_data.gametype.playerdisconnect, 0);
	Scr_FreeThread(callback);
}

void Scr_PlayerVote(gentity_s *self, const char *option)
{
	Scr_AddString(option);
	Scr_Notify(self, scr_const.vote, 1u);
}

void Scr_VoteCalled(gentity_s *self, const char *command, const char *param1, const char *param2)
{
	Scr_AddString(param2);
	Scr_AddString(param1);
	Scr_AddString(command);
	Scr_Notify(self, scr_const.call_vote, 3u);
}

extern dvar_t *g_no_script_spam;
void print()
{
	const char *str;
	signed int i;
	signed int paramnum;

	if ( !g_no_script_spam->current.boolean )
	{
		paramnum = Scr_GetNumParam();

		for ( i = 0; i < paramnum; ++i )
		{
			str = Scr_GetDebugString(i);
			Com_Printf("%s", str);
		}
	}
}

void println()
{
	if ( !g_no_script_spam->current.boolean )
	{
		print();
		Com_Printf("\n");
	}
}

void Scr_MakeGameMessage(int iClientNum, const char *pszCmd)
{
	unsigned int paramnum;
	char string[1024];

	paramnum = Scr_GetNumParam();
	Scr_ConstructMessageString(0, paramnum - 1, "Game Message", string, 0x400u);
	SV_GameSendServerCommand(iClientNum, 0, va("%s \"%s\"", pszCmd, string));
}

void iprintln()
{
	Scr_MakeGameMessage(-1, va("%c", 102));
}

void iprintlnbold()
{
	Scr_MakeGameMessage(-1, va("%c", 103));
}

void print3d()
{
	;
}

void GScr_line()
{
	;
}

void GScr_Spawn()
{
	int iSpawnFlags;
	gentity_s *ent;
	vec3_t origin;
	unsigned short classname;

	classname = Scr_GetConstString(0);
	Scr_GetVector(1u, origin);

	if ( Scr_GetNumParam() <= 2 )
		iSpawnFlags = 0;
	else
		iSpawnFlags = Scr_GetInt(2u);

	ent = G_Spawn();
	Scr_SetString(&ent->classname, classname);
	VectorCopy(origin, ent->r.currentOrigin);
	ent->spawnflags = iSpawnFlags;

	if ( G_CallSpawnEntity(ent) )
	{
		Scr_AddEntity(ent);
	}
	else
	{
		Scr_Error(va("unable to spawn \"%s\" entity", SL_ConvertToString(classname)));
	}
}

void GScr_SpawnTurret()
{
	const char *weaponName;
	gentity_s *ent;
	vec3_t origin;
	unsigned short classname;

	classname = Scr_GetConstString(0);
	Scr_GetVector(1u, origin);
	weaponName = Scr_GetString(2u);
	ent = G_Spawn();
	Scr_SetString(&ent->classname, classname);
	VectorCopy(origin, ent->r.currentOrigin);
	G_SpawnTurret(ent, weaponName);
	Scr_AddEntity(ent);
}

void GScr_PrecacheTurret()
{
	const char *turretInfo;

	if ( !level.initializing )
		Scr_Error("precacheTurret must be called before any wait statements in the level script\n");

	turretInfo = Scr_GetString(0);
	G_GetWeaponIndexForName(turretInfo);
}

void Scr_AddStruct()
{
	unsigned int id;

	id = AllocObject();
	Scr_AddObject(id);
	RemoveRefToObject(id);
}

void assertCmd()
{
	if ( !Scr_GetInt(0) )
		Scr_Error("assert fail");
}

void assertexCmd()
{
	const char *string;

	if ( !Scr_GetInt(0) )
	{
		string = Scr_GetString(1u);
		Scr_Error(va("assert fail: %s", string));
	}
}

void assertmsgCmd()
{
	const char *string;

	string = Scr_GetString(0);
	Scr_Error(va("assert fail: %s", string));
}

void GScr_IsDefined()
{
	int defined;
	int type;
	int pointer_type;

	type = Scr_GetType(0);

	if ( type == VAR_OBJECT )
	{
		pointer_type = Scr_GetPointerType(0);
		defined = 0;

		if ( pointer_type <= VAR_ARRAY && pointer_type != VAR_REMOVED_ENTITY )
			defined = 1;

		Scr_AddInt(defined);
	}
	else
	{
		Scr_AddInt(type != VAR_UNDEFINED);
	}
}

void GScr_IsString()
{
	int type;

	type = Scr_GetType(0);
	Scr_AddInt(type == VAR_STRING);
}

void GScr_IsAlive()
{
	if ( Scr_GetType(0) == VAR_OBJECT && Scr_GetPointerType(0) == VAR_ENTITY && Scr_GetEntity(0)->health > 0 )
		Scr_AddInt(1);
	else
		Scr_AddInt(0);
}

void GScr_GetDvar()
{
	const char *string;
	const char *dvarName;

	dvarName = Scr_GetString(0);
	string = Dvar_GetVariantString(dvarName);
	Scr_AddString(string);
}

void GScr_GetDvarInt()
{
	int value;
	const char *string;
	const char *dvarName;

	dvarName = Scr_GetString(0);
	string = Dvar_GetVariantString(dvarName);
	value = atoi(string);
	Scr_AddInt(value);
}

void GScr_GetDvarFloat()
{
	float value;
	const char *string;
	const char *dvarName;

	dvarName = Scr_GetString(0);
	string = Dvar_GetVariantString(dvarName);
	value = atof(string);
	Scr_AddFloat(value);
}

void GScr_SetDvar()
{
	unsigned int paramnum;
	char chr;
	int norestart;
	dvar_s *var;
	const char *pName;
	const char *dvarName;
	char outString[1024];
	char string[1024];
	char *pString;
	int type;
	int count;

	dvarName = Scr_GetString(0);
	type = Scr_GetType(1u);

	if ( type == VAR_ISTRING )
	{
		paramnum = Scr_GetNumParam();
		Scr_ConstructMessageString(1, paramnum - 1, "Dvar Value", string, sizeof(string));
		pName = string;
	}
	else
	{
		pName = Scr_GetString(1u);
	}

	pString = outString;
	memset(outString, 0, sizeof(outString));
	count = 0;

	while ( count <= 0x1FFF && pName[count] )
	{
		chr = I_CleanChar(pName[count]);
		*pString = chr;

		if ( *pString == 34 )
			*pString = 39;

		++count;
		++pString;
	}

	if ( Dvar_IsValidName(dvarName) )
	{
		norestart = 0;

		if ( Scr_GetNumParam() > 2 && Scr_GetInt(2u) )
			norestart = 1;

		Dvar_SetFromStringByName(dvarName, pName);

		if ( norestart )
		{
			var = Dvar_FindVar(dvarName);
			Dvar_AddFlags(var, 0x400u);
		}
	}
	else
	{
		Scr_Error(va("Dvar %s has an invalid dvar name", dvarName));
	}
}

void GScr_GetTime()
{
	Scr_AddInt(level.time);
}

void Scr_GetEntByNum()
{
	unsigned int num;

	num = Scr_GetInt(0);

	if ( num < 1024 )
	{
		if ( g_entities[num].r.inuse )
			Scr_AddEntity(&g_entities[num]);
	}
}

void Scr_GetWeaponModel()
{
	WeaponDef *weaponDef;
	int iWeaponIndex;
	const char *pszWeaponName;

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( iWeaponIndex )
	{
		weaponDef = BG_GetWeaponDef(iWeaponIndex);
		Scr_AddString(weaponDef->worldModel);
	}
	else
	{
		if ( *pszWeaponName )
		{
			if ( I_stricmp(pszWeaponName, "none") )
			{
				Com_Printf(va("unknown weapon '%s' in getWeaponModel\n", pszWeaponName));
			}
		}

		Scr_AddString("");
	}
}

void GScr_GetAnimLength()
{
	float length;
	scr_anim_s anim;
	XAnim_s *xanim;

	Scr_GetAnim(&anim, 0, 0);
	xanim = Scr_GetAnims(anim.tree);

	if ( !XAnimIsPrimitive(xanim, anim.index) )
		Scr_ParamError(0, "non-primitive animation has no concept of length");

	length = XAnimGetLength(xanim, anim.index);
	Scr_AddFloat(length);
}

void GScr_AnimHasNotetrack()
{
	XAnim_s *xanim;
	bool exist;
	scr_anim_s anim;
	unsigned short name;

	Scr_GetAnim(&anim, 0, 0);
	name = Scr_GetConstString(1u);
	xanim = Scr_GetAnims(anim.tree);
	exist = XAnimNotetrackExists(xanim, anim.index, name);

	Scr_AddBool(exist);
}

void GScr_GetBrushModelCenter()
{
	gentity_s *ent;
	vec3_t vCenter;

	ent = Scr_GetEntity(0);
	VectorAdd(ent->r.absmin, ent->r.absmax, vCenter);
	VectorScale(vCenter, 0.5, vCenter);

	Scr_AddVector(vCenter);
}

int ObjectiveStateIndexFromString(int *piStateIndex, unsigned int stateString)
{
	if ( stateString == scr_const.empty )
	{
		*piStateIndex = OBJST_EMPTY;
		return 1;
	}

	if ( stateString == scr_const.invisible )
	{
		*piStateIndex = OBJST_INVISIBLE;
		return 1;
	}

	if ( stateString == scr_const.current )
	{
		*piStateIndex = OBJST_CURRENT;
		return 1;
	}

	*piStateIndex = OBJST_EMPTY;
	return 0;
}

void SetObjectiveIcon(objective_t *obj, int paramNum)
{
	const char *shaderName;
	int i;

	shaderName = Scr_GetString(paramNum);

	for ( i = 0; shaderName[i]; ++i )
	{
		if ( shaderName[i] < 32 || shaderName[i] == 127 )
		{
			Scr_ParamError(3, va("Illegal character '%c'(ascii %i) in objective icon name: %s\n", shaderName[i], (unsigned char)shaderName[i], shaderName));
		}
	}

	if ( i > 63 )
	{
		Scr_ParamError(3, va("Objective icon name is too long (> %i): %s\n", 63, shaderName));
	}

	obj->icon = G_ShaderIndex(shaderName);
}

void ClearObjective_OnEntity(objective_t *obj)
{
	gentity_s *ent;

	if ( obj->entNum != 1023 )
	{
		ent = &g_entities[obj->entNum];

		if ( ent->r.inuse )
			ent->r.svFlags &= ~0x10u;

		obj->entNum = 1023;
	}
}

void Scr_Objective_Add()
{
	int state;
	unsigned short stateName;
	objective_t *obj;
	unsigned int objNum;
	int paramnum;

	paramnum = Scr_GetNumParam();

	if ( paramnum <= 1 )
		Scr_Error(
		    "objective_add needs at least the first two parameters out of its parameter list of: index state [string] [position]\n");

	objNum = Scr_GetInt(0);

	if ( objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	obj = &level.objectives[objNum];
	ClearObjective_OnEntity(obj);
	stateName = Scr_GetConstString(1u);

	if ( !ObjectiveStateIndexFromString(&state, stateName) )
	{
		Scr_ParamError(1, va("Illegal objective state \"%s\". Valid states are \"empty\", \"invisible\", \"current\"\n", SL_ConvertToString(stateName)));
	}

	obj->state = state;

	if ( paramnum > 2 )
	{
		Scr_GetVector(2u, obj->origin);

		obj->origin[0] = (float)(int)obj->origin[0];
		obj->origin[1] = (float)(int)obj->origin[1];
		obj->origin[2] = (float)(int)obj->origin[2];

		obj->entNum = 1023;

		if ( paramnum > 3 )
			SetObjectiveIcon(obj, 3);
	}

	obj->teamNum = 0;
}

void ClearObjective(objective_t *obj)
{
	obj->state = OBJST_EMPTY;
	VectorClear(obj->origin);
	obj->entNum = 1023;
	obj->teamNum = 0;
	obj->icon = 0;
}

void Scr_Objective_Delete()
{
	unsigned int objNum;

	objNum = Scr_GetInt(0);

	if ( objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	ClearObjective_OnEntity(&level.objectives[objNum]);
	ClearObjective(&level.objectives[objNum]);
}

void Scr_Objective_State()
{
	unsigned short stateName;
	int state;
	objective_t *obj;
	unsigned int objNum;

	objNum = Scr_GetInt(0);

	if ( objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	obj = &level.objectives[objNum];
	stateName = Scr_GetConstString(1u);

	if ( !ObjectiveStateIndexFromString(&state, stateName) )
	{
		Scr_ParamError(1, va("Illegal objective state \"%s\". Valid states are \"empty\", \"invisible\", \"current\"\n", Scr_GetString(1u)));
	}

	obj->state = state;

	if ( state == OBJST_EMPTY || state == OBJST_INVISIBLE )
		ClearObjective_OnEntity(obj);
}

void Scr_Objective_Icon()
{
	unsigned int objNum;

	objNum = Scr_GetInt(0);

	if ( objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	SetObjectiveIcon(&level.objectives[objNum], 1);
}

void Scr_Objective_Position()
{
	objective_t *obj;
	unsigned int objNum;

	objNum = Scr_GetInt(0);

	if ( objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	obj = &level.objectives[objNum];
	ClearObjective_OnEntity(obj);
	Scr_GetVector(1u, obj->origin);

	obj->origin[0] = (float)(int)obj->origin[0];
	obj->origin[1] = (float)(int)obj->origin[1];
	obj->origin[2] = (float)(int)obj->origin[2];
}

void Scr_Objective_OnEntity()
{
	gentity_s *ent;
	unsigned int objNum;

	objNum = Scr_GetInt(0);

	if ( objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	ClearObjective_OnEntity(&level.objectives[objNum]);
	ent = Scr_GetEntity(1u);
	ent->r.svFlags |= 0x10u;
	level.objectives[objNum].entNum = ent->s.number;
}

void Scr_Objective_Current()
{
	int makeCurrent[MAX_OBJECTIVES];
	objective_t *obj;
	unsigned int objNum;
	int numParam;
	int i;

	numParam = Scr_GetNumParam();
	memset(makeCurrent, 0, sizeof(makeCurrent));

	for ( i = 0; i < numParam; ++i )
	{
		objNum = Scr_GetInt(i);

		if ( objNum >= MAX_OBJECTIVES )
		{
			Scr_ParamError(i, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
		}

		makeCurrent[objNum] = 1;
	}

	for ( objNum = 0; objNum < MAX_OBJECTIVES; ++objNum )
	{
		obj = &level.objectives[objNum];

		if ( makeCurrent[objNum] )
		{
			obj->state = OBJST_CURRENT;
		}
		else if ( obj->state == OBJST_CURRENT )
		{
			obj->state = OBJST_ACTIVE;
		}
	}
}

void Scr_BulletTrace()
{
	const char *surfaceName;
	vec3_t vNorm;
	vec3_t value;
	int iSurfaceTypeIndex;
	trace_t trace;
	int contentmask;
	int iIgnoreEntNum;
	gentity_s *pIgnoreEnt;
	vec3_t vEnd;
	vec3_t vStart;

	pIgnoreEnt = 0;
	iIgnoreEntNum = 1023;
	contentmask = 41953329;

	Scr_GetVector(0, vStart);
	Scr_GetVector(1, vEnd);

	if ( !Scr_GetInt(2) )
		contentmask &= ~0x2000000u;

	if ( Scr_GetType(3) == VAR_OBJECT && Scr_GetPointerType(3) == VAR_ENTITY )
	{
		pIgnoreEnt = Scr_GetEntity(3);
		iIgnoreEntNum = pIgnoreEnt->s.number;
	}

	G_LocationalTrace(&trace, vStart, vEnd, iIgnoreEntNum, contentmask, 0);
	Scr_MakeArray();
	Scr_AddFloat(trace.fraction);
	Scr_AddArrayStringIndexed(scr_const.fraction);
	Vec3Lerp(vStart, vEnd, trace.fraction, value);
	Scr_AddVector(value);
	Scr_AddArrayStringIndexed(scr_const.position);

	if ( trace.hitId == 1023 || trace.hitId == 1022 )
		Scr_AddUndefined();
	else
		Scr_AddEntity(&g_entities[trace.hitId]);

	Scr_AddArrayStringIndexed(scr_const.entity);

	if ( trace.fraction >= 1.0 )
	{
		VectorSubtract(vEnd, vStart, vNorm);
		Vec3Normalize(vNorm);
		Scr_AddVector(vNorm);
		Scr_AddArrayStringIndexed(scr_const.normal);
		Scr_AddConstString(scr_const.none);
		Scr_AddArrayStringIndexed(scr_const.surfacetype);
	}
	else
	{
		Scr_AddVector(trace.normal);
		Scr_AddArrayStringIndexed(scr_const.normal);
		iSurfaceTypeIndex = (trace.surfaceFlags & 0x1F00000) >> 20;
		surfaceName = Com_SurfaceTypeToName(iSurfaceTypeIndex);
		Scr_AddString(surfaceName);
		Scr_AddArrayStringIndexed(scr_const.surfacetype);
	}
}

void Scr_BulletTracePassed()
{
	bool passed;
	int iClipMask;
	int hitnum;
	vec3_t vEnd;
	vec3_t vStart;

	hitnum = 1023;
	iClipMask = 41953329;

	Scr_GetVector(0, vStart);
	Scr_GetVector(1u, vEnd);

	if ( !Scr_GetInt(2u) )
		iClipMask = 8398897;

	if ( Scr_GetType(3u) == VAR_OBJECT && Scr_GetPointerType(3u) == VAR_ENTITY )
		hitnum = Scr_GetEntity(3u)->s.number;

	passed = G_LocationalTracePassed(vStart, vEnd, hitnum, iClipMask);
	Scr_AddBool(passed);
}

void Scr_SightTracePassed()
{
	int hitNum;
	int iClipMask;
	int iIgnoreEntNum;
	gentity_s *pIgnoreEnt;
	vec3_t vEnd;
	vec3_t vStart;

	pIgnoreEnt = 0;
	iIgnoreEntNum = 1023;
	iClipMask = 41949187;

	Scr_GetVector(0, vStart);
	Scr_GetVector(1u, vEnd);

	if ( !Scr_GetInt(2u) )
		iClipMask &= ~0x2000000u;

	if ( Scr_GetType(3u) == VAR_OBJECT && Scr_GetPointerType(3u) == VAR_ENTITY )
	{
		pIgnoreEnt = Scr_GetEntity(3u);
		iIgnoreEntNum = pIgnoreEnt->s.number;
	}

	G_SightTrace(&hitNum, vStart, vEnd, iIgnoreEntNum, iClipMask);
#ifndef DEDICATED
	if ( !hitNum )
		hitNum = SV_FX_GetVisibility(vStart, vEnd) < 0.2;
#endif
	Scr_AddBool(hitNum == 0);
}

void Scr_PhysicsTrace()
{
	trace_t trace;
	vec3_t endpos;
	vec3_t end;
	vec3_t start;

	Scr_GetVector(0, start);
	Scr_GetVector(1u, end);
	G_TraceCapsule(&trace, start, vec3_origin, vec3_origin, end, 1023, 8519697);
	Vec3Lerp(start, end, trace.fraction, endpos);
	Scr_AddVector(endpos);
}

void GScr_GetMoveDelta()
{
	XAnim_s *xanim;
	unsigned int paramNum;
	scr_anim_s pAnim;
	float endTime;
	float startTime;
	vec3_t trans;
	vec2_t rot;

	startTime = 0.0;
	endTime = 1.0;
	paramNum = Scr_GetNumParam();

	if ( paramNum != 1 )
	{
		if ( paramNum != 2 )
		{
			endTime = Scr_GetFloat(2u);

			if ( endTime < 0.0 || endTime > 1.0 )
				Scr_ParamError(2, "end time must be between 0 and 1");
		}

		startTime = Scr_GetFloat(1u);

		if ( startTime < 0.0 || startTime > 1.0 )
			Scr_ParamError(1, "start time must be between 0 and 1");
	}

	Scr_GetAnim(&pAnim, 0, 0);
	xanim = Scr_GetAnims(pAnim.tree);
	XAnimGetRelDelta(xanim, pAnim.index, rot, trans, startTime, endTime);
	Scr_AddVector(trans);
}

void GScr_GetAngleDelta()
{
	XAnim_s *xanim;
	float yaw;
	unsigned int paramNum;
	scr_anim_s pAnim;
	float endTime;
	float startTime;
	vec3_t trans;
	vec2_t rot;

	startTime = 0.0;
	endTime = 1.0;
	paramNum = Scr_GetNumParam();

	if ( paramNum != 1 )
	{
		if ( paramNum != 2 )
		{
			endTime = Scr_GetFloat(2u);
			if ( endTime < 0.0 || endTime > 1.0 )
				Scr_ParamError(2, "end time must be between 0 and 1");
		}

		startTime = Scr_GetFloat(1u);

		if ( startTime < 0.0 || startTime > 1.0 )
			Scr_ParamError(1, "start time must be between 0 and 1");
	}

	Scr_GetAnim(&pAnim, 0, 0);
	xanim = Scr_GetAnims(pAnim.tree);
	XAnimGetRelDelta(xanim, pAnim.index, rot, trans, startTime, endTime);
	yaw = RotationToYaw(rot);
	Scr_AddFloat(yaw);
}

void GScr_GetNorthYaw()
{
	char northYawString[32];

	SV_GetConfigstring(11, northYawString, 32);
	Scr_AddFloat(atof(northYawString));
}

void Scr_RandomInt()
{
	int iRand;
	int iMax;

	iMax = Scr_GetInt(0);

	if ( iMax > 0 )
	{
		iRand = irand(0, iMax);
		Scr_AddInt(iRand);
	}
	else
	{
		Com_Printf("RandomInt parm: %d  ", iMax);
		Scr_Error("RandomInt parm must be positive integer.\n");
	}
}

void Scr_RandomFloat()
{
	float fRand;
	float fMax;

	fMax = Scr_GetFloat(0);
	fRand = flrand(0.0, fMax);
	Scr_AddFloat(fRand);
}

void Scr_RandomIntRange()
{
	int iRand;
	int iMax;
	int iMin;

	iMin = Scr_GetInt(0);
	iMax = Scr_GetInt(1u);

	if ( iMax <= iMin )
	{
		Com_Printf("RandomIntRange parms: %d %d ", iMin, iMax);
		Scr_Error("RandomIntRange range must be positive integer.\n");
	}

	iRand = irand(iMin, iMax);
	Scr_AddInt(iRand);
}

void Scr_RandomFloatRange()
{
	float fRand;
	float fMin;
	float fMax;

	fMin = Scr_GetFloat(0);
	fMax = Scr_GetFloat(1u);

	if ( fMin > fMax )
	{
		Com_Printf("Scr_RandomFloatRange parms: %f %f ", fMin, fMax);
		Scr_Error("Scr_RandomFloatRange range must be positive float.\n");
	}

	fRand = flrand(fMin, fMax);
	Scr_AddFloat(fRand);
}

void GScr_sin()
{
	long double number;
	float value;

	number = Scr_GetFloat(0);
	value = sin(number * 0.0174532925199433);
	Scr_AddFloat(value);
}

void GScr_cos()
{
	long double number;
	float value;

	number = Scr_GetFloat(0);
	value = cos(number * 0.0174532925199433);
	Scr_AddFloat(value);
}

void GScr_tan()
{
	long double number;
	float sinT;
	float cosT;

	number = Scr_GetFloat(0) * 0.0174532925199433;

	cosT = cos(number);
	sinT = sin(number);

	if ( cosT == 0.0 )
		Scr_Error("divide by 0");

	Scr_AddFloat(sinT / cosT);
}

void GScr_asin()
{
	float number;
	float value;

	number = Scr_GetFloat(0);

	if ( number < -1.0 || number > 1.0 )
	{
		Scr_Error(va("%g out of range", number));
	}

	value = asin(number) * 57.29577951308232;
	Scr_AddFloat(value);
}

void GScr_acos()
{
	float number;
	float value;

	number = Scr_GetFloat(0);

	if ( number < -1.0 || number > 1.0 )
	{
		Scr_Error(va("%g out of range", number));
	}

	value = acos(number) * 57.29577951308232;
	Scr_AddFloat(value);
}

void GScr_atan()
{
	long double number;
	float value;

	number = Scr_GetFloat(0);
	value = atan(number) * 57.29577951308232;
	Scr_AddFloat(value);
}

void GScr_CastInt()
{
	int type;
	int intValue;
	int floatValue;
	int stringValue;

	type = Scr_GetType(0);

	if ( type == VAR_FLOAT )
	{
		floatValue = (int)Scr_GetFloat(0);
		Scr_AddInt(floatValue);
		return;
	}

	if ( type > VAR_FLOAT )
	{
		if ( type == VAR_INTEGER )
		{
			intValue = Scr_GetInt(0);
			Scr_AddInt(intValue);
			return;
		}
	}
	else if ( type == VAR_STRING )
	{
		stringValue = atoi(Scr_GetString(0));
		Scr_AddInt(stringValue);
		return;
	}

	Scr_ParamError(0, va("cannot cast %s to int", Scr_GetTypeName(0)));
}

void Scr_Distance()
{
	float distance;
	vec3_t a;
	vec3_t b;

	Scr_GetVector(0, a);
	Scr_GetVector(1u, b);
	distance = Vec3Distance(a, b);

	Scr_AddFloat(distance);
}

void Scr_DistanceSquared()
{
	float distance;
	vec3_t a;
	vec3_t b;

	Scr_GetVector(0, a);
	Scr_GetVector(1u, b);
	distance = Vec3DistanceSq(a, b);

	Scr_AddFloat(distance);
}

void Scr_Length()
{
	float len;
	vec3_t vector;

	Scr_GetVector(0, vector);
	len = VectorLength(vector);

	Scr_AddFloat(len);
}

void Scr_LengthSquared()
{
	float len;
	vec3_t vector;

	Scr_GetVector(0, vector);
	len = VectorLengthSquared(vector);

	Scr_AddFloat(len);
}

void Scr_Closer()
{
	float fDistBSqrd;
	float vB[3];
	float fDistASqrd;
	float vRef[3];
	float vA[3];

	Scr_GetVector(0, vRef);
	Scr_GetVector(1u, vA);
	Scr_GetVector(2u, vB);

	fDistASqrd = Vec3DistanceSq(vA, vRef);
	fDistBSqrd = Vec3DistanceSq(vB, vRef);

	Scr_AddInt(fDistBSqrd > fDistASqrd);
}

void Scr_VectorDot()
{
	float distance;
	vec3_t a;
	vec3_t b;

	Scr_GetVector(0, a);
	Scr_GetVector(1u, b);
	distance = DotProduct(a, b);

	Scr_AddFloat(distance);
}

void Scr_VectorNormalize()
{
	vec3_t a;
	vec3_t b;

	Scr_GetVector(0, a);
	VectorCopy(a, b);
	Vec3Normalize(b);

	Scr_AddVector(b);
}

void Scr_VectorToAngles()
{
	vec3_t vector;
	vec3_t angles;

	Scr_GetVector(0, vector);
	vectoangles(vector, angles);

	Scr_AddVector(angles);
}

void Scr_AnglesToUp()
{
	vec3_t angles;
	vec3_t up;

	Scr_GetVector(0, angles);
	AngleVectors(angles, 0, 0, up);
	Scr_AddVector(up);
}

void Scr_AnglesToRight()
{
	vec3_t angles;
	vec3_t right;

	Scr_GetVector(0, angles);
	AngleVectors(angles, 0, right, 0);
	Scr_AddVector(right);
}

void Scr_AnglesToForward()
{
	vec3_t angles;
	vec3_t forward;

	Scr_GetVector(0, angles);
	AngleVectors(angles, forward, 0, 0);
	Scr_AddVector(forward);
}

void Scr_IsSubStr()
{
	const char *string;
	const char *substring;
	const char *str;

	string = Scr_GetString(1u);
	substring = Scr_GetString(0);
	str = strstr(substring, string);

	Scr_AddBool(str != 0);
}

void Scr_GetSubStr()
{
	int intValue;
	char c;
	int i;
	int source;
	int start;
	const char *string;
	char tempString[1024];

	string = Scr_GetString(0);
	start = Scr_GetInt(1u);

	if ( Scr_GetNumParam() <= 2 )
		intValue = 0x7FFFFFFF;
	else
		intValue = Scr_GetInt(2u);

	source = start;

	for ( i = 0; source < intValue; ++i )
	{
		if ( i > 1023 )
			Scr_Error("string too long");

		c = string[source];

		if ( !c )
			break;

		tempString[i] = c;
		++source;
	}

	tempString[i] = 0;
	Scr_AddString(tempString);
}

void Scr_ToLower()
{
	char c;
	int i;
	const char *string;
	char tempString[1024];

	string = Scr_GetString(0);

	for ( i = 0; i < 1024; ++i )
	{
		c = tolower(*string);
		tempString[i] = c;

		if ( !c )
		{
			Scr_AddString(tempString);
			return;
		}

		++string;
	}

	Scr_Error("string too long");
}

void Scr_StrTok()
{
	signed int j;
	char c;
	int dest;
	int i;
	signed int len;
	const char *tok;
	const char *string;
	char tempString[1024];

	string = Scr_GetString(0);
	tok = Scr_GetString(1u);
	len = strlen(tok);
	dest = 0;

	Scr_MakeArray();

	for ( i = 0; ; ++i )
	{
		c = string[i];

		if ( !c )
			break;

		for ( j = 0; j < len; ++j )
		{
			if ( c == tok[j] )
			{
				if ( dest )
				{
					tempString[dest] = 0;
					Scr_AddString(tempString);
					Scr_AddArray();
					dest = 0;
				}

				goto skip;
			}
		}

		tempString[dest] = c;

		if ( ++dest > 1023 )
			Scr_Error("string too long");
skip:
		;
	}

	if ( dest )
	{
		tempString[dest] = 0;
		Scr_AddString(tempString);
		Scr_AddArray();
	}
}

void Scr_MusicPlay()
{
	SV_GameSendServerCommand(-1, 1, va("%c %s", 111, Scr_GetString(0)));
}

void Scr_MusicStop()
{
	unsigned int paramNum;
	int iFadeTime;

	paramNum = Scr_GetNumParam();

	if ( paramNum )
	{
		if ( paramNum != 1 )
		{
			Scr_Error(va("USAGE: musicStop([fadetime]);\n"));
			return;
		}

		iFadeTime = floorf(Scr_GetFloat(0) * 1000.0);
	}
	else
	{
		iFadeTime = 0;
	}

	if ( iFadeTime < 0 )
	{
		Scr_Error(va("musicStop: fade time must be >= 0\n"));
	}

	SV_GameSendServerCommand(-1, 1, va("%c %i", 112, iFadeTime));
}

void Scr_SoundFade()
{
	int iFadeTime;
	float fTargetVol;

	fTargetVol = Scr_GetFloat(0);

	if ( Scr_GetNumParam() <= 1 )
		iFadeTime = 0;
	else
		iFadeTime = (int)(Scr_GetFloat(1u) * 1000.0);

	SV_GameSendServerCommand(-1, 1, va("%c %f %i\n", 113, fTargetVol, iFadeTime));
}

void Scr_AmbientPlay()
{
	unsigned int paramNum;
	const char *name;
	int iFadeTime;

	iFadeTime = 0;
	paramNum = Scr_GetNumParam();

	if ( paramNum != 1 )
	{
		if ( paramNum != 2 )
		{
			Scr_Error(va("USAGE: ambientPlay(alias_name, <fadetime>);\n"));
			return;
		}

		iFadeTime = floorf(Scr_GetFloat(1u) * 1000.0);
	}

	name = Scr_GetString(0);

	if ( !*name )
	{
		Scr_Error(va("ambientPlay: alias name cannot be the empty string... use stop or fade version\n"));
	}

	if ( iFadeTime < 0 )
	{
		Scr_Error(va("ambientPlay: fade time must be >= 0\n"));
	}

	SV_SetConfigstring(3u, va("n\\%s\\t\\%i", name, level.time + iFadeTime));
}

void Scr_AmbientStop()
{
	unsigned int paramNum;
	int iFadeTime;

	paramNum = Scr_GetNumParam();

	if ( paramNum )
	{
		if ( paramNum != 1 )
		{
			Scr_Error(va("USAGE: ambientStop(<fadetime>);\n"));
			return;
		}

		iFadeTime = floorf(Scr_GetFloat(0) * 1000.0);
	}
	else
	{
		iFadeTime = 0;
	}

	if ( iFadeTime < 0 )
	{
		Scr_Error(va("ambientStop: fade time must be >= 0\n"));
	}

	SV_SetConfigstring(3u, va("t\\%i", level.time + iFadeTime));
}

void Scr_PrecacheModel()
{
	const char *name;

	if ( !level.initializing )
		Scr_Error("precacheModel must be called before any wait statements in the gametype or level script\n");

	name = Scr_GetString(0);
	G_ModelIndex(name);
}

void Scr_PrecacheShellShock()
{
	const char *name;

	if ( !level.initializing )
		Scr_Error("precacheShellShock must be called before any wait statements in the gametype or level script\n");

	name = Scr_GetString(0);
	G_ShellShockIndex(name);
}

void Scr_PrecacheItem()
{
	gitem_s *item;
	const char *itemName;

	if ( !level.initializing )
		Scr_Error("precacheItem must be called before any wait statements in the gametype or level script\n");

	itemName = Scr_GetString(0);
	item = G_FindItem(itemName);

	if ( !item )
	{
		Scr_ParamError(0, va("unknown item '%s'", itemName));
	}

	RegisterItem(item - bg_itemlist, 1);
}

void Scr_PrecacheShader()
{
	const char *name;

	if ( !level.initializing )
		Scr_Error("precacheShader must be called before any wait statements in the gametype or level script\n");

	name = Scr_GetString(0);

	if ( !*name )
		Scr_ParamError(0, "Shader name string is empty");

	G_ShaderIndex(name);
}

void Scr_PrecacheString()
{
	const char *string;

	if ( !level.initializing )
		Scr_Error("precacheString must be called before any wait statements in the gametype or level script\n");

	string = Scr_GetIString(0);

	if ( *string )
		G_LocalizedStringIndex(string);
}

void Scr_PrecacheRumble()
{
	;
}

void Scr_LoadFX()
{
	const char *name;
	int index;

	name = Scr_GetString(0);
	index = G_EffectIndex(name);

	if ( !index && !level.initializing )
		Scr_Error(
		    "loadFx must be called before any wait statements in the gametype or level script, or on an already loaded effect\n");

	Scr_AddInt(index);
}

void Scr_FxParamError(const char *errorString, int fxId)
{
	char fxName[1024];

	if ( fxId )
		SV_GetConfigstring(fxId + 846, fxName, 1024);
	else
		strcpy(fxName, "not successfully loaded");

	Scr_Error(va("%s (effect = %s)\n", errorString, fxName));
}

void Scr_PlayFX()
{
	float scale;
	float axis[3][3];
	vec3_t pos;
	float length;
	int paramNum;
	int fxId;
	gentity_s *ent;

	paramNum = Scr_GetNumParam();

	if ( paramNum <= 1 || paramNum > 4 )
		Scr_Error("Incorrect number of parameters");

	fxId = Scr_GetInt(0);
	Scr_GetVector(1u, pos);
	ent = G_TempEntity(pos, EV_PLAY_FX);
	ent->s.eventParm = (unsigned char)fxId;

	if ( paramNum == 2 )
	{
		ent->s.apos.trBase[0] = -90.0;
	}
	else
	{
		Scr_GetVector(2u, axis[0]);
		length = VectorLength(axis[0]);

		if ( length == 0.0 )
			Scr_FxParamError("playFx called with (0 0 0) forward direction", fxId);

		scale = 1.0 / length;
		VectorScale(axis[0], scale, axis[0]);

		if ( paramNum == 3 )
		{
			vectoangles(axis[0], ent->s.apos.trBase);
		}
		else
		{
			Scr_GetVector(3u, axis[2]);
			length = VectorLength(axis[2]);

			if ( length == 0.0 )
				Scr_FxParamError("playFx called with (0 0 0) up direction", fxId);

			scale = 1.0 / length;
			VectorScale(axis[2], scale, axis[2]);
			Vec3Cross(axis[2], axis[0], axis[1]);
			length = VectorLength(axis[1]);

			if ( length >= 0.001 )
			{
				if ( length < 0.99900001 )
				{
					scale = 1.0 / length;
					VectorScale(axis[1], scale, axis[1]);
					Vec3Cross(axis[0], axis[1], axis[2]);
				}
			}
			else
			{
				Scr_FxParamError("playFx called an up direction 0 or 180 degrees from forward", fxId);
			}

			AxisToAngles(axis, ent->s.apos.trBase);
		}
	}
}

void Scr_PlayFXOnTag()
{
	unsigned int tag;
	gentity_s *ent;
	int csIndex;
	int fxId;

	if ( Scr_GetNumParam() != 3 )
		Scr_Error("USAGE: playFxOnTag <effect id from loadFx> <entity> <tag name>");

	fxId = Scr_GetInt(0);

	if ( fxId <= 0 || fxId > 63 )
	{
		Scr_ParamError(0, va("effect id %i is invalid\n", fxId));
	}

	ent = Scr_GetEntity(1u);

	if ( !ent->model )
		Scr_ParamError(1, "cannot play fx on entity with no model");

	tag = Scr_GetConstLowercaseString(2u);

	if ( strchr(SL_ConvertToString(tag), 34) )
		Scr_ParamError(2, "cannot use \" characters in tag names\n");

	if ( SV_DObjGetBoneIndex(ent, tag) < 0 )
	{
		SV_DObjDumpInfo(ent);
		Scr_ParamError(2, va("tag '%s' does not exist on entity with model '%s'", SL_ConvertToString(tag), G_ModelName(ent->model)));
	}

	csIndex = G_FindConfigstringIndex(va("%02d%s", fxId, SL_ConvertToString(tag)), 910, 256, 1, 0);
	G_AddEvent(ent, EV_PLAY_FX_ON_TAG, csIndex);
}

void Scr_PlayLoopedFX()
{
	unsigned int paramNum;
	char tempString[1024];
	int repeat;
	float cullDist;
	vec3_t vDir;
	vec3_t vAxis;
	vec3_t vOrigin;
	int fxId;
	gentity_s *ent;

	if ( Scr_GetNumParam() <= 2 || Scr_GetNumParam() > 6 )
		Scr_Error(
		    "USAGE: playLoopedFx <effect id from loadFx> <repeat delay> <vector position of effect> <optional cull distance (0 "
		    "= never cull)> <optional forward vector> <optional up vector>");

	VectorClear(vAxis);
	VectorClear(vDir);
	cullDist = 0.0;
	fxId = Scr_GetInt(0);
	paramNum = Scr_GetNumParam();

	if ( paramNum == 5 )
	{
LABEL_14:
		Scr_GetVector(4u, vAxis);

		if ( Vec3Normalize(vAxis) == 0.0 )
		{
			if ( fxId )
				SV_GetConfigstring(fxId + 846, tempString, 1024);
			else
				strcpy(tempString, "not successfully loaded");

			Scr_Error(va("playLoopedFx called with (0 0 0) forward direction (effect = %s)\n", tempString));
		}

		goto LABEL_19;
	}

	if ( paramNum > 5 )
	{
		if ( paramNum != 6 )
			goto LABEL_20;

		Scr_GetVector(5u, vDir);

		if ( Vec3Normalize(vDir) == 0.0 )
		{
			if ( fxId )
				SV_GetConfigstring(fxId + 846, tempString, 1024);
			else
				strcpy(tempString, "not successfully loaded");

			Scr_Error(va("playLoopedFx called with (0 0 0) up direction (effect = %s)\n", tempString));
		}

		goto LABEL_14;
	}

	if ( paramNum == 4 )
LABEL_19:
		cullDist = Scr_GetFloat(3u);
LABEL_20:
	Scr_GetVector(2u, vOrigin);
	repeat = floorf(Scr_GetFloat(1u) * 1000.0);

	if ( repeat <= 0 )
	{
		Scr_Error(va("playLoopedFx called with %f s repeat (should be >= 0.0005s)\n", float(repeat)));
	}

	ent = G_Spawn();
	ent->s.eType = ET_LOOP_FX;
	ent->r.svFlags |= 8u;
	ent->s.scale = (unsigned char)fxId;
	G_SetOrigin(ent, vOrigin);
	VectorCopy(vAxis, ent->s.origin2);
	ent->s.eventParm = DirToByte(vDir);
	ent->s.angles2[0] = cullDist;
	ent->s.angles2[1] = (float)repeat;
	SV_LinkEntity(ent);
	Scr_AddEntity(ent);
}

void Scr_SetFog(const char *cmd, float start, float density, float heightDensity, float r, float g, float b, float time)
{
	if ( start < 0.0 )
	{
		Scr_Error(va("%s: near distance must be >= 0", cmd));
	}

	if ( start >= density )
	{
		Scr_Error(va("%s: near distance must be less than far distance", cmd));
	}

	if ( r < 0.0 || r > 1.0 || g < 0.0 || g > 1.0 || b < 0.0 || b > 1.0 )
	{
		Scr_Error(va("%s: red/green/blue color components must be in the range [0, 1]", cmd));
	}

	if ( time < 0.0 )
	{
		Scr_Error(va("%s: transition time must be >= 0 seconds", cmd));
	}

	G_setfog(va("%g %g %g %g %g %g %.0f", start, density, heightDensity, r, g, b, (double)(time * 1000.0)));
}

void Scr_SetLinearFog()
{
	float time;
	float b;
	float g;
	float r;
	float density;
	float start;

	if ( Scr_GetNumParam() != 6 )
		Scr_Error("USAGE: setCullFog(near distance, far distance, red, green, blue, transition time);\n");

	start = Scr_GetFloat(0);
	density = Scr_GetFloat(1u);
	r = Scr_GetFloat(2u);
	g = Scr_GetFloat(3u);
	b = Scr_GetFloat(4u);
	time = Scr_GetFloat(5u);

	Scr_SetFog("setCullFog", start, density, 1.0, r, g, b, time);
}

void Scr_SetExponentialFog()
{
	float time;
	float b;
	float g;
	float r;
	float density;

	if ( Scr_GetNumParam() != 5 )
		Scr_Error(
		    "USAGE: setExpFog(density, red, green, blue, transition time);\n"
		    "Density must be greater than 0 and less than 1, and typically less than .001.  For example, .0002 means the fog ge"
		    "ts .02%% more dense for every 1 unit of distance (about 1%% thicker every 50 units of distance)\n");

	density = Scr_GetFloat(0);
	r = Scr_GetFloat(1u);
	g = Scr_GetFloat(2u);
	b = Scr_GetFloat(3u);
	time = Scr_GetFloat(4u);

	if ( density <= 0.0 || density >= 1.0 )
		Scr_Error("setExpFog: distance must be greater than 0 and less than 1");

	Scr_SetFog("setExpFog", 0.0, 1.0, density, r, g, b, time);
}

void Scr_GrenadeExplosionEffect()
{
	trace_t trace;
	gentity_s *ent;
	vec3_t vEnd;
	vec3_t vPos;
	vec3_t vDir;
	vec3_t vOrg;

	Scr_GetVector(0, vOrg);
	VectorCopy(vOrg, vPos);
	vPos[2] = vPos[2] + 1.0;
	ent = G_TempEntity(vPos, EV_GRENADE_EXPLODE);
	VectorSet(vDir, 0.0, 0.0, 1.0);
	ent->s.eventParm = DirToByte(vDir);
	VectorCopy(vPos, vEnd);
	vEnd[2] = vEnd[2] - 17.0;

	G_TraceCapsule(&trace, vPos, vec3_origin, vec3_origin, vEnd, 1023, 2065);
	ent->s.surfType = (trace.surfaceFlags & 0x1F00000) >> 20;
}

void GScr_RadiusDamage()
{
	float min_damage;
	float max_damage;
	float range;
	vec3_t origin;

	Scr_GetVector(0, origin);
	range = Scr_GetFloat(1u);
	max_damage = Scr_GetFloat(2u);
	min_damage = Scr_GetFloat(3u);

	level.bPlayerIgnoreRadiusDamage = level.bPlayerIgnoreRadiusDamageLatched;
	G_RadiusDamage(origin, 0, &g_entities[1022], max_damage, min_damage, range, 0, 14);
	level.bPlayerIgnoreRadiusDamage = 0;
}

void GScr_SetPlayerIgnoreRadiusDamage()
{
	level.bPlayerIgnoreRadiusDamageLatched = Scr_GetInt(0);
}

void GScr_GetNumParts()
{
	const char *name;
	int numBones;
	XModel *model;

	name = Scr_GetString(0);
	model = SV_XModelGet(name);
	numBones = XModelNumBones(model);

	Scr_AddInt(numBones);
}

void GScr_GetPartName()
{
	const char *name;
	unsigned short boneName;
	unsigned int numBones;
	unsigned int partIndex;
	XModel *model;

	name = Scr_GetString(0);
	model = SV_XModelGet(name);
	partIndex = Scr_GetInt(1u);
	numBones = XModelNumBones(model);

	if ( partIndex >= numBones )
	{
		Scr_ParamError(1, va("index out of range (0 - %d)", numBones - 1));
	}

	boneName = XModelBoneNames(model)[partIndex];

	if ( !boneName )
		Scr_ParamError(0, "bad model");

	Scr_AddConstString(boneName);
}

void GScr_Earthquake()
{
	float time;
	float radius;
	vec3_t source;
	int duration;
	float scale;
	gentity_s *ent;

	scale = Scr_GetFloat(0);
	time = Scr_GetFloat(1u) * 1000.0;
	duration = floorf(time);
	Scr_GetVector(2u, source);
	radius = Scr_GetFloat(3u);

	if ( scale <= 0.0 )
		Scr_ParamError(0, "Scale must be greater than 0");

	if ( duration <= 0 )
		Scr_ParamError(1, "duration must be greater than 0");

	if ( radius <= 0.0 )
		Scr_ParamError(3, "Radius must be greater than 0");

	ent = G_TempEntity(source, EV_EARTHQUAKE);
	ent->s.angles2[0] = scale;
	ent->s.time = duration;
	ent->s.angles2[1] = radius;
}

void GScr_IsPlayer()
{
	if ( Scr_GetType(0) == VAR_OBJECT && Scr_GetPointerType(0) == VAR_ENTITY && Scr_GetEntity(0)->client )
		Scr_AddInt(1);
	else
		Scr_AddInt(0);
}

void GScr_IsPlayerNumber()
{
	if ( (unsigned int)Scr_GetInt(0) < MAX_CLIENTS )
		Scr_AddInt(1);
	else
		Scr_AddInt(0);
}

void GScr_SetWinningPlayer()
{
	char buffer[1024];
	char *pszWinner;
	int iWinner;

	iWinner = Scr_GetEntity(0)->s.number + 1;
	SV_GetConfigstring(22, buffer, 1024);
	pszWinner = va("%i", iWinner);

	if ( I_stricmp(Info_ValueForKey(buffer, "winner"), pszWinner) )
	{
		Info_SetValueForKey(buffer, "winner", pszWinner);
		SV_SetConfigstring(0x16u, buffer);
	}
}

void GScr_SetWinningTeam()
{
	char buffer[1024];
	char *pszWinner;
	int iWinner;
	unsigned short team;

	team = Scr_GetConstString(0);

	if ( team == scr_const.allies )
	{
		iWinner = -2;
	}
	else if ( team == scr_const.axis )
	{
		iWinner = -1;
	}
	else
	{
		if ( team != scr_const.none )
		{
			Scr_ParamError(0, va("Illegal team string '%s'. Must be allies, axis, or none.", SL_ConvertToString(team)));
			return;
		}

		iWinner = 0;
	}

	SV_GetConfigstring(22, buffer, 1024);
	pszWinner = va("%i", iWinner);

	if ( I_stricmp(Info_ValueForKey(buffer, "winner"), pszWinner) )
	{
		Info_SetValueForKey(buffer, "winner", pszWinner);
		SV_SetConfigstring(0x16u, buffer);
	}
}

void GScr_Announcement()
{
	unsigned int paramNum;
	char string[1024];

	paramNum = Scr_GetNumParam();
	Scr_ConstructMessageString(0, paramNum - 1, "Announcement", string, 1024);
	SV_GameSendServerCommand(-1, 0, va("%c \"%s\" 2", 99, string));
}

void GScr_ClientAnnouncement()
{
	unsigned int paramNum;
	char string[1024];
	gentity_s *ent;

	ent = Scr_GetEntity(0);
	paramNum = Scr_GetNumParam();
	Scr_ConstructMessageString(1, paramNum - 1, "Announcement", string, 1024);
	SV_GameSendServerCommand(ent->s.number, 0, va("%c \"%s\" 2", 99, string));
}

void GScr_GetTeamScore()
{
	unsigned short team;

	team = Scr_GetConstString(0);

	if ( team != scr_const.allies && team != scr_const.axis )
	{
		Scr_Error(va("Illegal team string '%s'. Must be allies, or axis.", SL_ConvertToString(team)));
	}

	if ( team == scr_const.allies )
		Scr_AddInt(level.teamScores[2]);
	else
		Scr_AddInt(level.teamScores[1]);
}

void GScr_SetTeamScore()
{
	int score;
	unsigned short team;
	char *pszScore;

	team = Scr_GetConstString(0);

	if ( team != scr_const.allies && team != scr_const.axis )
	{
		Scr_Error(va("Illegal team string '%s'. Must be allies, or axis.", SL_ConvertToString(team)));
	}

	score = Scr_GetInt(1u);

	if ( team == scr_const.allies )
	{
		level.teamScores[2] = score;
		pszScore = va("%c %i", 72, score);
	}
	else
	{
		level.teamScores[1] = score;
		pszScore = va("%c %i", 71, score);
	}

	SV_GameSendServerCommand(-1, 0, pszScore);
	level.bUpdateScoresForIntermission = 1;
}

void GScr_SetClientNameMode()
{
	unsigned short mode;

	mode = Scr_GetConstString(0);

	if ( mode == scr_const.auto_change )
	{
		level.manualNameChange = 0;
	}
	else if ( mode == scr_const.manual_change )
	{
		level.manualNameChange = 1;
	}
	else
	{
		Scr_Error("Unknown mode");
	}
}

void GScr_UpdateClientNames()
{
	int clientNum;
	char oldname[32];
	gclient_s *client;

	if ( !level.manualNameChange )
		Scr_Error("Only works in [manual_change] mode");

	clientNum = 0;
	client = level.clients;

	while ( clientNum < level.maxclients )
	{
		if ( client->sess.connected == CS_CONNECTED )
		{
			if ( strcmp(client->sess.state.name, client->sess.name) )
			{
				I_strncpyz(oldname, client->sess.state.name, 32);
				I_strncpyz(client->sess.state.name, client->sess.name, 32);
				ClientUserinfoChanged(clientNum);
			}
		}

		++clientNum;
		++client;
	}
}

extern dvar_t *g_maxclients;
void GScr_GetTeamPlayersAlive()
{
	gentity_s *ent;
	int iLivePlayers;
	int iTeamNum;
	int count;
	unsigned short team;

	team = Scr_GetConstString(0);

	if ( team != scr_const.allies && team != scr_const.axis )
	{
		Scr_Error(va("Illegal team string '%s'. Must be allies, or axis.", SL_ConvertToString(team)));
	}

	if ( team == scr_const.allies )
		iTeamNum = TEAM_ALLIES;
	else
		iTeamNum = TEAM_AXIS;

	count = 0;

	for ( iLivePlayers = 0; iLivePlayers < g_maxclients->current.integer; ++iLivePlayers )
	{
		ent = &g_entities[iLivePlayers];

		if ( ent->r.inuse && ent->client->sess.state.team == iTeamNum && ent->health > 0 )
			++count;
	}

	Scr_AddInt(count);
}

void GScr_Objective_Team()
{
	unsigned short team;
	unsigned int objNum;

	objNum = Scr_GetInt(0);

	if ( objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	team = Scr_GetConstString(1u);

	if ( team == scr_const.allies )
	{
		level.objectives[objNum].teamNum = 2;
	}
	else if ( team == scr_const.axis )
	{
		level.objectives[objNum].teamNum = 1;
	}
	else if ( team == scr_const.none )
	{
		level.objectives[objNum].teamNum = 0;
	}
	else
	{
		Scr_ParamError(1, va("Illegal team string '%s'. Must be allies, axis, or none.", SL_ConvertToString(team)));
	}
}

void GScr_LogPrint()
{
	unsigned int len;
	int iStringLen;
	char string[1024];
	int iNumParms;
	int i;
	const char *pszToken;

	string[0] = 0;
	iStringLen = 0;
	iNumParms = Scr_GetNumParam();

	for ( i = 0; i < iNumParms; ++i )
	{
		pszToken = Scr_GetString(i);
		len = strlen(pszToken);

		if ( (int)(len + iStringLen) >= 1024 )
			break;

		I_strncat(string, 1024, pszToken);
		iStringLen += len;
	}

	G_LogPrintf(string);
}

void GScr_WorldEntNumber()
{
	Scr_AddInt(1022);
}

int G_IndexForMeansOfDeath(const char *name)
{
	int i;

	for ( i = 0; i < COUNT_OF(modNames); ++i )
	{
		if ( !I_stricmp(name, modNames[i]) )
			return i;
	}

	Com_Printf("Unknown means of death string '%s'\n", name);
	return 0;
}

int GScr_GetScriptMenuIndex(const char *pszMenu)
{
	char szConfigString[1024];
	int i;

	for ( i = 0; i < 32; ++i )
	{
		SV_GetConfigstring(i + 1246, szConfigString, 1024);

		if ( !I_stricmp(szConfigString, pszMenu) )
			return i;
	}

	Scr_Error(va("Menu '%s' was not precached\n", pszMenu));
	return 0;
}

void GScr_Obituary()
{
	gentity_s *ent;
	int iMODNum;
	int iWeaponNum;
	const char *pszMod;
	const char *pszWeapon;
	gentity_s *pOtherEnt;

	pszWeapon = Scr_GetString(2u);
	iWeaponNum = G_GetWeaponIndexForName(pszWeapon);
	pszMod = Scr_GetString(3u);
	iMODNum = G_IndexForMeansOfDeath(pszMod);
	pOtherEnt = Scr_GetEntity(0);

	ent = G_TempEntity(vec3_origin, EV_OBITUARY);
	ent->s.otherEntityNum = pOtherEnt->s.number;

	if ( Scr_GetType(1u) == VAR_OBJECT && Scr_GetPointerType(1u) == VAR_ENTITY )
		ent->s.attackerEntityNum = Scr_GetEntity(1u)->s.number;
	else
		ent->s.attackerEntityNum = 1022;

	ent->r.svFlags = 8;

	if ( iMODNum == MOD_MELEE
	        || iMODNum == MOD_HEAD_SHOT
	        || iMODNum == MOD_SUICIDE
	        || iMODNum == MOD_FALLING
	        || iMODNum == MOD_CRUSH )
	{
		ent->s.eventParm = iMODNum | 0x80;
	}
	else
	{
		ent->s.eventParm = iWeaponNum;
	}
}

void GScr_positionWouldTelefrag()
{
	vec3_t vPos;
	vec3_t maxs;
	vec3_t mins;
	gentity_s *ent;
	int entityList[1024];
	int entities;
	int i;

	Scr_GetVector(0, vPos);
	VectorAdd(vPos, playerMins, mins);
	VectorAdd(vPos, playerMaxs, maxs);

	entities = CM_AreaEntities((const vec3_t *)mins, (const vec3_t *)maxs, entityList, 1024, 0x2000000);

	for ( i = 0; i < entities; ++i )
	{
		ent = &g_entities[entityList[i]];

		if ( ent->client && ent->client->ps.pm_type <= PM_INTERMISSION )
		{
			Scr_AddInt(1);
			return;
		}
	}

	Scr_AddInt(0);
}

void GScr_getStartTime()
{
	Scr_AddInt(level.startTime);
}

void GScr_PrecacheMenu()
{
	char szConfigString[1024];
	const char *pszNewMenu;
	int iConfigNum;

	pszNewMenu = Scr_GetString(0);

	for ( iConfigNum = 0; iConfigNum < 32; ++iConfigNum )
	{
		SV_GetConfigstring(iConfigNum + 1246, szConfigString, 1024);

		if ( !I_stricmp(szConfigString, pszNewMenu) )
		{
			Com_DPrintf("Script tried to precache the menu '%s' more than once\n", pszNewMenu);
			return;
		}
	}

	for ( iConfigNum = 0; iConfigNum < 32; ++iConfigNum )
	{
		SV_GetConfigstring(iConfigNum + 1246, szConfigString, 1024);

		if ( !szConfigString[0] )
			break;
	}

	if ( iConfigNum == 32 )
	{
		Scr_Error(va("Too many menus precached. Max allowed menus is %i", 32));
	}

	SV_SetConfigstring(iConfigNum + 1246, pszNewMenu);
}

void GScr_PrecacheStatusIcon()
{
	char szConfigString[1024];
	const char *pszNewIcon;
	int iConfigNum;

	pszNewIcon = Scr_GetString(0);

	for ( iConfigNum = 0; iConfigNum < 8; ++iConfigNum )
	{
		SV_GetConfigstring(iConfigNum + 23, szConfigString, 1024);

		if ( !I_stricmp(szConfigString, pszNewIcon) )
		{
			Com_DPrintf("Script tried to precache the player status icon '%s' more than once\n", pszNewIcon);
			return;
		}
	}

	for ( iConfigNum = 0; iConfigNum < 8; ++iConfigNum )
	{
		SV_GetConfigstring(iConfigNum + 23, szConfigString, 1024);

		if ( !szConfigString[0] )
			break;
	}

	if ( iConfigNum == 8 )
	{
		Scr_Error(va("Too many player status icons precached. Max allowed is %i", 8));
	}

	SV_SetConfigstring(iConfigNum + 23, pszNewIcon);
}

void GScr_PrecacheHeadIcon()
{
	char szConfigString[1024];
	const char *pszNewIcon;
	int iConfigNum;

	pszNewIcon = Scr_GetString(0);

	for ( iConfigNum = 0; iConfigNum < 15; ++iConfigNum )
	{
		SV_GetConfigstring(iConfigNum + 31, szConfigString, 1024);

		if ( !I_stricmp(szConfigString, pszNewIcon) )
		{
			Com_DPrintf("Script tried to precache the player head icon '%s' more than once\n", pszNewIcon);
			return;
		}
	}

	for ( iConfigNum = 0; iConfigNum < 15; ++iConfigNum )
	{
		SV_GetConfigstring(iConfigNum + 31, szConfigString, 1024);

		if ( !szConfigString[0] )
			break;
	}

	if ( iConfigNum == 15 )
	{
		Scr_Error(va("Too many player head icons precached. Max allowed is %i", 15));
	}

	SV_SetConfigstring(iConfigNum + 31, pszNewIcon);
}

void GScr_MapRestart()
{
	if ( level.finished )
	{
		if ( level.finished == 1 )
			Scr_Error("map_restart already called");
		else
			Scr_Error("exitlevel already called");
	}

	level.finished = 1;
	level.savePersist = 0;

	if ( Scr_GetNumParam() )
		level.savePersist = Scr_GetInt(0);

	Cbuf_ExecuteText(2, "fast_restart\n");
}

void GScr_ExitLevel()
{
	if ( level.finished )
	{
		if ( level.finished == 1 )
			Scr_Error("map_restart already called");
		else
			Scr_Error("exitlevel already called");
	}

	level.finished = 3;
	level.savePersist = 0;

	if ( Scr_GetNumParam() )
		level.savePersist = Scr_GetInt(0);

	//SV_MatchEnd();
	ExitLevel();
}

void GScr_AddTestClient()
{
	gentity_s *ent;

	ent = SV_AddTestClient();

	if ( ent )
		Scr_AddEntity(ent);
}

void CleanDvarValue(const char *dvarValue, char *outString, int size)
{
	int i;

	for ( i = 0; i < size - 1 && dvarValue[i]; ++i )
	{
		*outString = I_CleanChar(dvarValue[i]);

		if ( *outString == 34 )
			*outString = 39;

		++outString;
	}

	*outString = 0;
}

void GScr_MakeDvarServerInfo()
{
	int paramNum;
	char string[1024];
	char outString[1024];
	const char *dvarName;
	int type;
	dvar_s *dvar;
	const char *dvarValue;

	dvarName = Scr_GetString(0);

	if ( !Dvar_IsValidName(dvarName) )
	{
		Scr_Error(va("Dvar %s has an invalid dvar name", dvarName));
	}

	dvar = Dvar_FindVar(dvarName);

	if ( dvar )
	{
		Dvar_AddFlags(dvar, 0x100u);
	}
	else
	{
		type = Scr_GetType(1u);

		if ( type == VAR_ISTRING )
		{
			paramNum = Scr_GetNumParam();
			Scr_ConstructMessageString(1, paramNum - 1, "Dvar Value", string, 1024);
			dvarValue = string;
		}
		else
		{
			dvarValue = Scr_GetString(1u);
		}

		CleanDvarValue(dvarValue, outString, 1024);
		Dvar_RegisterString(dvarName, dvarValue, 0x4100u);
	}
}

void GScr_SetArchive()
{
	int enabled;

	enabled = Scr_GetInt(0);
	SV_EnableArchivedSnapshot(enabled);
}

void GScr_AllClientsPrint()
{
	const char *msg;

	if ( Scr_GetNumParam() )
	{
		msg = Scr_GetString(0);
		SV_GameSendServerCommand(-1, 0, va("%c \"%s\"", 101, msg));
	}
}

void GScr_ClientPrint()
{
	const char *msg;
	gentity_s *ent;

	if ( Scr_GetNumParam() )
	{
		ent = Scr_GetEntity(0);
		msg = Scr_GetString(1u);
		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"%s\"", 101, msg));
	}
}

void GScr_MapExists()
{
	const char *mapname;

	if ( Scr_GetNumParam() )
	{
		mapname = Scr_GetString(0);

		if ( SV_MapExists(mapname) )
			Scr_AddInt(1);
		else
			Scr_AddInt(0);
	}
}

void GScr_IsValidGameType()
{
	const char *gametype;

	if ( Scr_GetNumParam() )
	{
		gametype = Scr_GetString(0);

		if ( Scr_IsValidGameType(gametype) )
			Scr_AddInt(1);
		else
			Scr_AddInt(0);
	}
}

void GScr_MatchEnd()
{
	;
}

void GScr_SetPlayerTeamRank()
{
	;
}

void GScr_SendXboxLiveRanks()
{
	;
}

void GScr_SetVoteString()
{
	const char *string;

	if ( Scr_GetNumParam() )
	{
		string = Scr_GetString(0);

		SV_SetConfigstring(0x10u, string);
		SV_SetConfigstring(0xFu,  va("%i", level.voteTime));
		SV_SetConfigstring(0x11u, va("%i", level.voteYes));
		SV_SetConfigstring(0x12u, va("%i", level.voteNo));
	}
}

void GScr_SetVoteTime()
{
	int time;

	if ( Scr_GetNumParam() )
	{
		time = Scr_GetInt(0);

		SV_SetConfigstring(0xFu,  va("%i", time));
		SV_SetConfigstring(0x11u, va("%i", level.voteYes));
		SV_SetConfigstring(0x12u, va("%i", level.voteNo));
	}
}

void GScr_SetVoteYesCount()
{
	int count;

	if ( Scr_GetNumParam() )
	{
		count = Scr_GetInt(0);

		SV_SetConfigstring(0x11u, va("%i", count));
		SV_SetConfigstring(0x12u, va("%i", level.voteNo));
	}
}

void GScr_SetVoteNoCount()
{
	int count;

	if ( Scr_GetNumParam() )
	{
		count = Scr_GetInt(0);

		SV_SetConfigstring(0x11u, va("%i", level.voteYes));
		SV_SetConfigstring(0x12u, va("%i", count));
	}
}

void GScr_OpenFile()
{
	int len;
	int filenum;
	fileHandle_t tempFile;
	fileHandle_t *f;
	const char *mode;
	const char *filename;

	f = 0;

	if ( Scr_GetNumParam() > 1 )
	{
		filename = Scr_GetString(0);
		mode = Scr_GetString(1u);

		for ( filenum = 0; filenum <= 0; ++filenum )
		{
			if ( level.openScriptIOFileHandles[filenum] < 0 )
			{
				f = &level.openScriptIOFileHandles[filenum];
				break;
			}
		}

		if ( !f )
		{
			Com_Printf("OpenFile failed.  %i files already open\n", 1);
			Scr_AddInt(-1);
			return;
		}

		if ( !strcmp(mode, "read") )
		{
			len = FS_FOpenFileByMode(va("%s/%s", "scriptdata", filename), &tempFile, FS_READ);

			if ( len >= 0 )
			{
				level.openScriptIOFileBuffers[filenum] = (char *)Z_MallocInternal(len + 1);
				FS_Read(level.openScriptIOFileBuffers[filenum], len, tempFile);
				FS_FCloseFile(tempFile);
				level.openScriptIOFileBuffers[filenum][len] = 0;
				Com_BeginParseSession(filename);
				Com_SetCSV(1);
				level.currentScriptIOLineMark[filenum].lines = 0;
				Scr_AddInt(filenum);
				return;
			}

			goto error;
		}

		if ( !strcmp(mode, "write") )
		{
			if ( FS_FOpenFileByMode(va("%s/%s", "scriptdata", filename), f, FS_WRITE) >= 0 )
			{
errornum:
				Scr_AddInt(filenum);
				return;
			}
error:
			Scr_AddInt(-1);
			return;
		}

		if ( !strcmp(mode, "append") )
		{
			if ( FS_FOpenFileByMode(va("%s/%s", "scriptdata", filename), f, FS_APPEND) >= 0 )
				goto errornum;
			goto error;
		}

		Com_Printf("Valid openfile modes are 'write', 'read', and 'append'\n");
		Scr_AddInt(-1);
	}
}

void GScr_CloseFile()
{
	unsigned int filenum;

	if ( Scr_GetNumParam() )
	{
		filenum = Scr_GetInt(0);

		if ( filenum >= 2 )
		{
			Com_Printf("CloseFile failed, invalid file number %i\n", filenum);
			Scr_AddInt(-1);
			return;
		}

		if ( level.openScriptIOFileHandles[filenum] <= 0 )
		{
			if ( !level.openScriptIOFileBuffers[filenum] )
			{
				Com_Printf("CloseFile failed, file number %i was not open\n", filenum);
				Scr_AddInt(-1);
				return;
			}

			Com_EndParseSession();
			Z_FreeInternal(level.openScriptIOFileBuffers[filenum]);
			level.openScriptIOFileBuffers[filenum] = 0;
		}
		else
		{
			FS_FCloseFile(level.openScriptIOFileHandles[filenum]);
			level.openScriptIOFileHandles[filenum] = -1;
		}

		Scr_AddInt(1);
	}
}

void GScr_FPrintln()
{
	int len;
	unsigned int paramNum;
	const char *s;
	unsigned int arg;
	unsigned int filenum;

	if ( Scr_GetNumParam() > 1 )
	{
		filenum = Scr_GetInt(0);

		if ( filenum < 2 )
		{
			if ( level.openScriptIOFileHandles[filenum] >= 0 )
			{
				for ( arg = 1; arg < Scr_GetNumParam(); ++arg )
				{
					s = Scr_GetString(arg);
					len = strlen(s);
					FS_Write(s, len, level.openScriptIOFileHandles[filenum]);
					FS_Write(",", 1, level.openScriptIOFileHandles[filenum]);
				}

				paramNum = Scr_GetNumParam();
				Scr_AddInt(paramNum - 1);
			}
			else
			{
				Com_Printf("FPrintln failed, file number %i was not open for writing\n", filenum);
				Scr_AddInt(-1);
			}
		}
		else
		{
			Com_Printf("FPrintln failed, invalid file number %i\n", filenum);
			Scr_AddInt(-1);
		}
	}
	else
	{
		Com_Printf("fprintln requires at least 2 parameters (file, output)\n");
		Scr_AddInt(-1);
	}
}

void GScr_FReadLn()
{
	int argcount;
	const char *token;
	int eof;
	unsigned int filenum;

	if ( Scr_GetNumParam() )
	{
		filenum = Scr_GetInt(0);

		if ( filenum < 2 )
		{
			if ( level.openScriptIOFileBuffers[filenum] )
			{
				token = level.openScriptIOFileBuffers[filenum];

				if ( level.currentScriptIOLineMark[filenum].lines )
				{
					Com_ParseReturnToMark(&token, &level.currentScriptIOLineMark[filenum]);
					Com_SkipRestOfLine(&token);
					Com_ParseSetMark(&token, &level.currentScriptIOLineMark[filenum]);
					eof = *Com_Parse(&token) == 0;
					Com_ParseReturnToMark(&token, &level.currentScriptIOLineMark[filenum]);

					if ( eof )
					{
						Scr_AddInt(-1);
					}
					else
					{
						argcount = Com_GetArgCountOnLine(&token);
						Scr_AddInt(argcount);
					}
				}
				else
				{
					Com_ParseSetMark(&token, &level.currentScriptIOLineMark[filenum]);
					argcount = Com_GetArgCountOnLine(&token);
					Scr_AddInt(argcount);
				}
			}
			else
			{
				Com_Printf("freadln failed, file number %i was not open for reading\n", filenum);
				Scr_AddInt(-1);
			}
		}
		else
		{
			Com_Printf("freadln failed, invalid file number %i\n", filenum);
			Scr_AddInt(-1);
		}
	}
	else
	{
		Com_Printf("freadln requires a parameter - the file to read from\n");
		Scr_AddInt(-1);
	}
}

void GScr_FGetArg()
{
	const char *token;
	const char *buf;
	int i;
	int arg;
	unsigned int filenum;

	if ( Scr_GetNumParam() > 1 )
	{
		filenum = Scr_GetInt(0);
		arg = Scr_GetInt(1u);

		if ( filenum < 2 )
		{
			if ( arg >= 0 )
			{
				if ( level.openScriptIOFileBuffers[filenum] )
				{
					buf = 0;
					token = level.openScriptIOFileBuffers[filenum];
					Com_ParseReturnToMark(&token, &level.currentScriptIOLineMark[filenum]);

					for ( i = 0; i <= arg; ++i )
					{
						buf = Com_ParseOnLine(&token);

						if ( !*buf )
						{
							Com_Printf("freadline failed, there aren't %i arguments on this line, there are only %i arguments\n", arg + 1, i);
							Scr_AddString("");
							return;
						}
					}

					Scr_AddString(buf);
				}
				else
				{
					Com_Printf("freadline failed, file number %i was not open for reading\n", filenum);
					Scr_AddString("");
				}
			}
			else
			{
				Com_Printf("freadline failed, invalid argument number %i\n", arg);
				Scr_AddString("");
			}
		}
		else
		{
			Com_Printf("freadline failed, invalid file number %i\n", filenum);
			Scr_AddString("");
		}
	}
	else
	{
		Com_Printf("freadline requires at least 2 parameters (file, string)\n");
		Scr_AddString("");
	}
}

void GScr_KickPlayer()
{
	int playernum;

	if ( Scr_GetNumParam() )
	{
		playernum = Scr_GetInt(0);
		Cbuf_ExecuteText(2, va("tempBanClient %i\n", playernum));
	}
}

void GScr_BanPlayer()
{
	int playernum;

	if ( Scr_GetNumParam() )
	{
		playernum = Scr_GetInt(0);
		Cbuf_ExecuteText(2, va("banClient %i\n", playernum));
	}
}

void GScr_LoadMap()
{
	const char *mapname;

	if ( Scr_GetNumParam() )
	{
		mapname = Scr_GetString(0);

		if ( SV_MapExists(mapname) )
		{
			if ( level.finished )
			{
				if ( level.finished == 2 )
					Scr_Error("map already called");
				else
					Scr_Error("exitlevel already called");
			}

			level.finished = 2;
			level.savePersist = 0;

			if ( Scr_GetNumParam() > 1 )
				level.savePersist = Scr_GetInt(1u);

			Cbuf_ExecuteText(2, va("map %s\n", mapname));
		}
	}
}

void Scr_PlayRumbleOnPos()
{
	;
}

void Scr_PlayLoopRumbleOnPos()
{
	;
}

void Scr_StopAllRumbles()
{
	;
}

void ScrCmd_SoundExists()
{
	Scr_Error("ScrCmd_SoundExists: This function is currently not supported.");
}

void Scr_IsSplitscreen()
{
	Scr_AddInt(0);
}

void GScr_EndXboxLiveLobby()
{
	;
}

void ScrCmd_attach(scr_entref_t entref)
{
	int ignoreCollision;
	unsigned int stringValue;
	const char *modelName;
	gentity_s *ent;

	ent = GetEntity(entref);
	modelName = Scr_GetString(0);

	if ( Scr_GetNumParam() <= 1 )
		stringValue = scr_const.emptystring;
	else
		stringValue = Scr_GetConstLowercaseString(1u);

	if ( Scr_GetNumParam() <= 2 )
		ignoreCollision = 0;
	else
		ignoreCollision = Scr_GetInt(2u);

	if ( G_EntDetach(ent, modelName, stringValue) )
	{
		Scr_Error(va("model '%s' already attached to tag '%s'", modelName, SL_ConvertToString(stringValue)));
	}

	if ( !G_EntAttach(ent, modelName, stringValue, ignoreCollision) )
		Scr_Error("maximum attached models exceeded");
}

void ScrCmd_detach(scr_entref_t entref)
{
	unsigned int stringValue;
	int i;
	const char *modelName;
	gentity_s *ent;

	ent = GetEntity(entref);
	modelName = Scr_GetString(0);

	if ( Scr_GetNumParam() <= 1 )
		stringValue = scr_const.emptystring;
	else
		stringValue = Scr_GetConstLowercaseString(1u);

	if ( !G_EntDetach(ent, modelName, stringValue) )
	{
		Com_Printf("Current attachments:\n");

		for ( i = 0; i <= 6; ++i )
		{
			if ( ent->attachModelNames[i] )
			{
				if ( ent->attachTagNames[i] )
				{
					Com_Printf("model: '%s', tag: '%s'\n", G_ModelName(ent->attachModelNames[i]), SL_ConvertToString(ent->attachTagNames[i]));
				}
			}
		}

		Scr_Error(va("failed to detach model '%s' from tag '%s'", modelName, SL_ConvertToString(stringValue)));
	}
}

void ScrCmd_detachAll(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);
	G_EntDetachAll(ent);
}

void ScrCmd_GetAttachSize(scr_entref_t entref)
{
	gentity_s *ent;
	int i;

	ent = GetEntity(entref);

	for ( i = 0; i <= 6 && ent->attachModelNames[i]; ++i )
		;

	Scr_AddInt(i);
}

void ScrCmd_GetAttachModelName(scr_entref_t entref)
{
	gentity_s *ent;
	unsigned int i;

	ent = GetEntity(entref);
	i = Scr_GetInt(0);

	if ( i > 6 || !ent->attachModelNames[i] )
		Scr_ParamError(0, "bad index");

	Scr_AddString(G_ModelName(ent->attachModelNames[i]));
}

void ScrCmd_GetAttachTagName(scr_entref_t entref)
{
	gentity_s *ent;
	unsigned int i;

	ent = GetEntity(entref);
	i = Scr_GetInt(0);

	if ( i > 6 || !ent->attachModelNames[i] )
		Scr_ParamError(0, "bad index");

	Scr_AddConstString(ent->attachTagNames[i]);
}

void ScrCmd_GetAttachIgnoreCollision(scr_entref_t entref)
{
	gentity_s *ent;
	unsigned int i;

	ent = GetEntity(entref);
	i = Scr_GetInt(0);

	if ( i > 6 || !ent->attachModelNames[i] )
		Scr_ParamError(0, "bad index");

	Scr_AddBool(((int)ent->attachIgnoreCollision >> i) & 1);
}

void GScr_GetAmmoCount(scr_entref_t entref)
{
	int ammocount;
	gentity_s *ent;
	int weaponIndex;
	const char *weaponName;

	ent = GetPlayerEntity(entref);
	weaponName = Scr_GetString(0);
	weaponIndex = G_GetWeaponIndexForName(weaponName);

	if ( weaponIndex )
	{
		ammocount = BG_WeaponAmmo(&ent->client->ps, weaponIndex);
		Scr_AddInt(ammocount);
	}
	else
	{
		Scr_AddInt(0);
	}
}

void ScrCmd_GetClanId(scr_entref_t entref)
{
	Scr_AddString("0");
}

void ScrCmd_GetClanName(scr_entref_t entref)
{
	Scr_AddString("");
}

void ScrCmd_GetClanDescription(scr_entref_t entref)
{
	Scr_AddString("");
}

void ScrCmd_GetClanMotto(scr_entref_t entref)
{
	Scr_AddString("");
}

void ScrCmd_GetClanURL(scr_entref_t entref)
{
	Scr_AddString("");
}

void ScrCmd_LinkTo(scr_entref_t entref)
{
	vec3_t anglesOffset;
	vec3_t originOffset;
	int paramNum;
	unsigned int stringValue;
	gentity_s *parent;
	gentity_s *ent;

	printf("test\n");

	ent = GetEntity(entref);

	if ( Scr_GetType(0) != VAR_OBJECT || Scr_GetPointerType(0) != VAR_ENTITY )
		Scr_ParamError(0, "not an entity");

	if ( (ent->flags & 0x1000) == 0 )
	{
		Scr_ObjectError(va("entity (classname: '%s') does not currently support linkTo", SL_ConvertToString(ent->classname)));
	}

	parent = Scr_GetEntity(0);
	paramNum = Scr_GetNumParam();
	stringValue = 0;

	if ( paramNum > 1 )
	{
		stringValue = Scr_GetConstLowercaseString(1u);

		if ( !*SL_ConvertToString(stringValue) )
			stringValue = 0;
	}

	if ( paramNum > 2 )
	{
		Scr_GetVector(2u, originOffset);
		Scr_GetVector(3u, anglesOffset);

		if ( G_EntLinkToWithOffset(ent, parent, stringValue, originOffset, anglesOffset) )
			return;
	}
	else if ( G_EntLinkTo(ent, parent, stringValue) )
	{
		return;
	}

	if ( !SV_DObjExists(parent) )
	{
		if ( !parent->model )
			Scr_Error("failed to link entity since parent has no model");

		Scr_Error(va("failed to link entity since parent model '%s' is invalid", G_ModelName(parent->model)));
	}

	if ( stringValue )
	{
		if ( SV_DObjGetBoneIndex(parent, stringValue) < 0 )
		{
			SV_DObjDumpInfo(parent);
			Scr_Error(va("failed to link entity since tag '%s' does not exist in parent model '%s'", SL_ConvertToString(stringValue), G_ModelName(parent->model)));
		}
	}

	Scr_Error("failed to link entity due to link cycle");
}

void ScrCmd_Unlink(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);
	G_EntUnlink(ent);
}

void ScrCmd_EnableLinkTo(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);

	if ( (ent->flags & 0x1000) != 0 )
		Scr_ObjectError("entity already has linkTo enabled");

	if ( ent->s.eType || ent->physicsObject )
	{
		Scr_ObjectError(va("entity (classname: '%s') does not currently support enableLinkTo", SL_ConvertToString(ent->classname)));
	}

	ent->flags |= 0x1000u;
}

void ScrCmd_GetOrigin(scr_entref_t entref)
{
	vec3_t origin;
	gentity_s *ent;

	ent = GetEntity(entref);
	VectorCopy(ent->r.currentOrigin, origin);
	Scr_AddVector(origin);
}

void ScrCmd_GetEye(scr_entref_t entref)
{
	vec3_t origin;
	gentity_s *ent;

	ent = GetEntity(entref);
	VectorCopy(ent->r.currentOrigin, origin);
	origin[2] = origin[2] + 40.0;
	Scr_AddVector(origin);
}

void ScrCmd_UseBy(scr_entref_t entref)
{
	void (*use)(struct gentity_s *, struct gentity_s *, struct gentity_s *);
	gentity_s *pOther;
	gentity_s *pEnt;

	pEnt = GetEntity(entref);
	pOther = Scr_GetEntity(0);
	Scr_AddEntity(pOther);
	Scr_Notify(pEnt, scr_const.trigger, 1u);
	use = entityHandlers[pEnt->handler].use;

	if ( use )
		use(pEnt, pOther, pOther);
}

void Scr_SetStableMissile(scr_entref_t entref)
{
	unsigned int flags;
	int stableMissile;
	gentity_s *ent;

	ent = GetEntity(entref);
	stableMissile = Scr_GetInt(0);

	if ( ent->s.eType != ET_PLAYER )
		Scr_Error("Type should be a player");

	if ( stableMissile )
		flags = ent->flags | 0x20000;
	else
		flags = ent->flags & 0xFFFDFFFF;

	ent->flags = flags;
}

void ScrCmd_IsTouching(scr_entref_t entref)
{
	int bTouching;
	vec3_t vMaxs;
	vec3_t vMins;
	gentity_s *pTemp;
	gentity_s *pEnt;
	gentity_s *ent;

	ent = GetEntity(entref);

	if ( ent->r.bmodel || (ent->r.svFlags & 0x60) != 0 )
	{
		pTemp = ent;
		ent = Scr_GetEntity(0);

		if ( ent->r.bmodel || (ent->r.svFlags & 0x60) != 0 )
			Scr_Error("istouching cannot be called on 2 brush/cylinder entities");

		pEnt = pTemp;
	}
	else
	{
		pEnt = Scr_GetEntity(0);
	}

	VectorAdd(ent->r.currentOrigin, ent->r.mins, vMins);
	VectorAdd(ent->r.currentOrigin, ent->r.maxs, vMaxs);
	ExpandBoundsToWidth(vMins, vMaxs);
	bTouching = SV_EntityContact(vMins, vMaxs, pEnt);

	Scr_AddInt(bTouching);
}

void ScrCmd_PlaySound(scr_entref_t entref)
{
	gentity_s *tempEnt;
	unsigned char soundIndex;
	const char *name;
	gentity_s *ent;

	ent = GetEntity(entref);
	name = Scr_GetString(0);
	soundIndex = G_SoundAliasIndex(name);
	tempEnt = G_TempEntity(ent->r.currentOrigin, EV_SOUND_ALIAS);
	tempEnt->r.svFlags |= 8u;
	tempEnt->s.eventParm = soundIndex;
}

void ScrCmd_PlaySoundAsMaster(scr_entref_t entref)
{
	gentity_s *tempEnt;
	unsigned char soundIndex;
	const char *name;
	gentity_s *ent;

	ent = GetEntity(entref);
	name = Scr_GetString(0);
	soundIndex = G_SoundAliasIndex(name);
	tempEnt = G_TempEntity(ent->r.currentOrigin, EV_SOUND_ALIAS_AS_MASTER);
	tempEnt->r.svFlags |= 8u;
	tempEnt->s.eventParm = soundIndex;
}

void ScrCmd_PlayLoopSound(scr_entref_t entref)
{
	unsigned char soundIndex;
	const char *name;
	gentity_s *ent;

	ent = GetEntity(entref);
	name = Scr_GetString(0);
	soundIndex = G_SoundAliasIndex(name);
	ent->r.broadcastTime = -1;
	ent->s.loopSound = soundIndex;
}

void ScrCmd_StopLoopSound(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);
	ent->r.broadcastTime = level.time + 300;
	ent->s.loopSound = 0;
}

void ScrCmd_PlayRumble(scr_entref_t entref)
{
	;
}

void ScrCmd_PlayLoopRumble(scr_entref_t entref)
{
	;
}

void ScrCmd_StopRumble(scr_entref_t entref)
{
	;
}

void ScrCmd_Delete(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);

	if ( ent->client )
		Scr_Error("Cannot delete a client entity");

	if ( level.currentEntityThink == ent->s.number )
		Scr_Error("Cannot delete entity during its think");

	Scr_Notify(ent, scr_const.death, 0);
	G_FreeEntity(ent);
}

void ScrCmd_SetModel(scr_entref_t entref)
{
	const char *modelName;
	gentity_s *ent;

	ent = GetEntity(entref);
	modelName = Scr_GetString(0);
	G_SetModel(ent, modelName);
	G_DObjUpdate(ent);
	SV_LinkEntity(ent);
}

void ScrCmd_GetNormalHealth(scr_entref_t entref)
{
	float playerHealth;
	float entityHealth;
	gentity_s *ent;

	ent = GetEntity(entref);

	if ( ent->client )
	{
		if ( ent->health )
		{
			playerHealth = (long double)ent->health / (long double)ent->client->sess.maxHealth;
			Scr_AddFloat(playerHealth);
		}
		else
		{
			Scr_AddFloat(0.0);
		}
	}
	else
	{
		entityHealth = (float)ent->health;
		Scr_AddFloat(entityHealth);
	}
}

void ScrCmd_SetNormalHealth(scr_entref_t entref)
{
	float temp;
	int newHealth;
	float normalHealth;
	gentity_s *ent;

	ent = GetEntity(entref);
	normalHealth = Scr_GetFloat(0);

	if ( normalHealth > 1.0 )
		normalHealth = 1.0;

	if ( ent->client )
	{
		temp = (long double)ent->client->sess.maxHealth * normalHealth;
		newHealth = floorf(temp);
		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"%i\"", 73, 0));
	}
	else if ( ent->maxHealth )
	{
		newHealth = (int)((long double)ent->maxHealth * normalHealth);
	}
	else
	{
		newHealth = (int)normalHealth;
	}

	if ( newHealth > 0 )
		ent->health = newHealth;
	else
		Com_Printf("ERROR: Cannot setnormalhealth to 0 or below.\n");
}

void ScrCmd_Show(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);
	ent->flags &= ~0x800u;
	ent->r.clientMask[0] = 0;
	ent->r.clientMask[1] = 0;
}

void ScrCmd_Hide(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);
	ent->flags |= 0x800u;
	ent->r.clientMask[0] = -1;
	ent->r.clientMask[1] = -1;
}

void ScrCmd_ShowToPlayer(scr_entref_t entref)
{
	gentity_s *clientEnt;
	gentity_s *pEnt;

	pEnt = GetEntity(entref);
	clientEnt = Scr_GetEntity(0);

	if ( clientEnt->s.number < MAX_CLIENTS )
	{
		pEnt->flags &= ~0x800u;
		pEnt->r.clientMask[clientEnt->s.number >> 5] &= ~(1 << (clientEnt->s.number & 0x1F));
	}
	else
	{
		Scr_Error("showToClient error: param must be a client entity\n");
	}
}

void ScrCmd_SetContents(scr_entref_t entRef)
{
	int contents;
	gentity_s *ent;

	ent = GetEntity(entRef);
	contents = Scr_GetInt(0);
	Scr_AddInt(ent->r.contents);
	ent->r.contents = contents;
	SV_LinkEntity(ent);
}

void GScr_ShellShock(scr_entref_t entref)
{
	float time;
	char s[1024];
	const char *shock;
	unsigned int duration;
	int i;
	gentity_s *ent;

	ent = GetPlayerEntity(entref);

	if ( Scr_GetNumParam() != 2 )
		Scr_Error("USAGE: <player> shellshock(<shellshockname>, <duration>)\n");

	shock = Scr_GetString(0);

	for ( i = 1; ; ++i )
	{
		if ( i > 15 )
		{
			Scr_Error(va("shellshock '%s' was not precached\n", shock));
			return;
		}

		SV_GetConfigstring(i + 1166, s, 1024);

		if ( !strcasecmp(s, shock) )
			break;
	}

	time = Scr_GetFloat(1u) * 1000.0;
	duration = floorf(time);

	if ( duration >= 0xEA61 )
	{
		Scr_ParamError(1, va("duration %g should be >= 0 and <= 60", (double)((long double)(int)duration * 0.001)));
	}

	ent->client->ps.shellshockIndex = i;
	ent->client->ps.shellshockTime = level.time;
	ent->client->ps.shellshockDuration = duration;

	if ( ent->health > 0 )
	{
		bgs = &level_bgs;
		BG_AnimScriptEvent(&ent->client->ps, ANIM_ET_SHELLSHOCK, 0, 1);
	}
}

void GScr_StopShellShock(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetPlayerEntity(entref);

	if ( Scr_GetNumParam() )
		Scr_Error("USAGE: <player> stopshellshock()\n");

	ent->client->ps.shellshockIndex = 0;
	ent->client->ps.shellshockTime = 0;
	ent->client->ps.shellshockDuration = 0;
}

void GScr_ViewKick(scr_entref_t entref)
{
	gclient_s *client;
	long double damage;
	vec3_t origin;
	gentity_s *ent;

	ent = GetPlayerEntity(entref);

	if ( Scr_GetNumParam() != 2 )
		Scr_Error("USAGE: <player> viewkick <force 0-127> <source position>\n");

	client = ent->client;
	client->damage_blood = (ent->maxHealth * Scr_GetInt(0) + 50) / 100;

	if ( ent->client->damage_blood < 0 )
	{
		damage = Scr_GetFloat(0);
		Scr_Error(va("viewkick: damage %g < 0\n", damage));
	}

	Scr_GetVector(1u, origin);
	VectorSubtract(ent->client->ps.origin, origin, ent->client->damage_from);
}

void GScr_LocalToWorldCoords(scr_entref_t entref)
{
	float vLocal[3];
	float vWorld[3];
	gentity_s *ent;
	float axis[3][3];

	ent = GetEntity(entref);
	Scr_GetVector(0, vLocal);
	AnglesToAxis(ent->r.currentAngles, axis);
	MatrixTransformVector(vLocal, axis, vWorld);

	vWorld[0] = vWorld[0] + ent->r.currentOrigin[0];
	vWorld[1] = vWorld[1] + ent->r.currentOrigin[1];
	vWorld[2] = vWorld[2] + ent->r.currentOrigin[2];

	Scr_AddVector(vWorld);
}

void GScr_SetRightArc(scr_entref_t entref)
{
	float value;
	turretInfo_s *pTurretInfo;

	pTurretInfo = GetEntity(entref)->pTurretInfo;

	if ( !pTurretInfo )
		Scr_Error("entity is not a turret");

	value = Scr_GetFloat(0);
	pTurretInfo->arcmin[1] = -value;

	if ( pTurretInfo->arcmin[1] > 0.0 )
		pTurretInfo->arcmin[1] = 0.0;
}

void GScr_SetLeftArc(scr_entref_t entref)
{
	turretInfo_s *pTurretInfo;

	pTurretInfo = GetEntity(entref)->pTurretInfo;

	if ( !pTurretInfo )
		Scr_Error("entity is not a turret");

	pTurretInfo->arcmax[1] = Scr_GetFloat(0);

	if ( pTurretInfo->arcmax[1] < 0.0 )
		pTurretInfo->arcmax[1] = 0.0;
}

void GScr_SetTopArc(scr_entref_t entref)
{
	float value;
	turretInfo_s *pTurretInfo;

	pTurretInfo = GetEntity(entref)->pTurretInfo;

	if ( !pTurretInfo )
		Scr_Error("entity is not a turret");

	value = Scr_GetFloat(0);
	pTurretInfo->arcmin[0] = -value;

	if ( pTurretInfo->arcmin[0] > 0.0 )
		pTurretInfo->arcmin[0] = 0.0;
}

void GScr_SetBottomArc(scr_entref_t entref)
{
	turretInfo_s *pTurretInfo;

	pTurretInfo = GetEntity(entref)->pTurretInfo;

	if ( !pTurretInfo )
		Scr_Error("entity is not a turret");

	pTurretInfo->arcmax[0] = Scr_GetFloat(0);

	if ( pTurretInfo->arcmax[0] < 0.0 )
		pTurretInfo->arcmax[0] = 0.0;
}

void GScr_GetEntityNumber(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);
	Scr_AddInt(ent->s.number);
}

void GScr_EnableGrenadeTouchDamage(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);

	if ( ent->classname != scr_const.trigger_damage )
		Scr_Error("Currently on supported on damage triggers");

	ent->flags |= 0x4000u;
}

void GScr_DisableGrenadeTouchDamage(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);

	if ( ent->classname != scr_const.trigger_damage )
		Scr_Error("Currently on supported on damage triggers");

	ent->flags &= ~0x4000u;
}

void GScr_EnableGrenadeBounce(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);
	ent->flags &= ~0x8000u;
}

void GScr_DisableGrenadeBounce(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);
	ent->flags |= 0x8000u;
}

void GScr_EnableAimAssist(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);

	if ( !ent->r.bmodel )
		Scr_Error("Currently only supported on entities with brush models");

	ent->s.eFlags |= 0x800u;
}

void GScr_DisableAimAssist(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);

	if ( !ent->r.bmodel )
		Scr_Error("Currently only supported on entities with brush models");

	ent->s.eFlags &= ~0x800u;
}

void GScr_PlaceSpawnPoint(scr_entref_t entref)
{
	vec3_t vEnd;
	vec3_t vStart;
	trace_t trace;
	gentity_s *ent;

	ent = GetEntity(entref);

	VectorCopy(ent->r.currentOrigin, vStart);
	VectorCopy(ent->r.currentOrigin, vEnd);
	vEnd[2] = vEnd[2] + 128.0;

	G_TraceCapsule(&trace, vStart, playerMins, playerMaxs, vEnd, ent->s.number, 42008593);

	Vec3Lerp(vStart, vEnd, trace.fraction, vStart);
	VectorCopy(vStart, vEnd);
	vEnd[2] = vEnd[2] - 262144.0;

	G_TraceCapsule(&trace, vStart, playerMins, playerMaxs, vEnd, ent->s.number, 42008593);

	ent->s.groundEntityNum = trace.hitId;
	Vec3Lerp(vStart, vEnd, trace.fraction, vStart);

	G_TraceCapsule(&trace, vStart, playerMins, playerMaxs, vStart, ent->s.number, 42008593);

	if ( trace.allsolid )
		Com_Printf("WARNING: Spawn point entity %i is in solid at (%i, %i, %i)\n", ent->s.number, (int)ent->r.currentOrigin[0], (int)ent->r.currentOrigin[1], (int)ent->r.currentOrigin[2]);

	G_SetOrigin(ent, vStart);
}

void GScr_UpdateScores(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);

	SV_GameSendServerCommand(ent - g_entities, 0, va("%c %i", 72, level.teamScores[2]));
	SV_GameSendServerCommand(ent - g_entities, 0, va("%c %i", 71, level.teamScores[1]));
}

void GScr_SetTeamForTrigger(scr_entref_t entref)
{
	unsigned short team;
	gentity_s *ent;

	ent = GetEntity(entref);

	if ( ent->classname != scr_const.trigger_use && ent->classname != scr_const.trigger_use_touch )
	{
		Scr_Error(va("setteamfortrigger: trigger entity must be of type %s or %s", SL_ConvertToString(scr_const.trigger_use), SL_ConvertToString(scr_const.trigger_use_touch)));
	}

	team = Scr_GetConstString(0);

	if ( team == scr_const.allies )
	{
		ent->team = 2;
	}
	else if ( team == scr_const.axis )
	{
		ent->team = 1;
	}
	else if ( team == scr_const.none )
	{
		ent->team = 0;
	}
	else
	{
		Scr_Error(va("setteamfortrigger: invalid team used must be %s, %s or %s", SL_ConvertToString(scr_const.allies), SL_ConvertToString(scr_const.axis), SL_ConvertToString(scr_const.none)));
	}
}

void GScr_ClientClaimTrigger(scr_entref_t entref)
{
	gentity_s *triggerEnt;
	gentity_s *clientEnt;

	clientEnt = GetEntity(entref);

	if ( !clientEnt->client )
		Scr_Error("clientclaimtrigger: claimer must be a client.");

	triggerEnt = Scr_GetEntity(0);

	if ( triggerEnt->classname != scr_const.trigger_use && triggerEnt->classname != scr_const.trigger_use_touch )
	{
		Scr_Error(va("clientclaimtrigger: trigger entity must be of type %s or %s", SL_ConvertToString(scr_const.trigger_use), SL_ConvertToString(scr_const.trigger_use_touch)));
	}

	if ( triggerEnt->trigger.singleUserEntIndex == 1023
	        || triggerEnt->trigger.singleUserEntIndex == clientEnt->client->ps.clientNum )
	{
		triggerEnt->trigger.singleUserEntIndex = clientEnt->client->ps.clientNum;
	}
}

void GScr_ClientReleaseTrigger(scr_entref_t entref)
{
	gentity_s *triggerEnt;
	gentity_s *clientEnt;

	clientEnt = GetEntity(entref);

	if ( !clientEnt->client )
		Scr_Error("clientreleasetrigger: releaser must be a client.");

	triggerEnt = Scr_GetEntity(0);

	if ( triggerEnt->classname != scr_const.trigger_use && triggerEnt->classname != scr_const.trigger_use_touch )
	{
		Scr_Error(va("clientreleasetrigger: trigger entity must be of type %s or %s", SL_ConvertToString(scr_const.trigger_use), SL_ConvertToString(scr_const.trigger_use_touch)));
	}

	if ( triggerEnt->trigger.singleUserEntIndex == clientEnt->client->ps.clientNum )
		triggerEnt->trigger.singleUserEntIndex = 1023;
}

void GScr_ReleaseClaimedTrigger(scr_entref_t entref)
{
	gentity_s *ent;

	ent = GetEntity(entref);

	if ( ent->classname != scr_const.trigger_use && ent->classname != scr_const.trigger_use_touch )
	{
		Scr_Error(va("releaseclaimedtrigger: trigger entity must be of type %s or %s", SL_ConvertToString(scr_const.trigger_use), SL_ConvertToString(scr_const.trigger_use_touch)));
	}

	ent->trigger.singleUserEntIndex = 1023;
}

void (*BuiltIn_GetMethod(const char **pName, int *type))(scr_entref_t)
{
	const char *name;
	unsigned int i;

	name = *pName;

	for ( i = 0; i < COUNT_OF(methods); ++i )
	{
		if ( !strcmp(name, methods[i].name) )
		{
			*pName = methods[i].name;
			*type = methods[i].developer;
			return methods[i].call;
		}
	}

	return NULL;
}

void (*Scr_GetFunction(const char **pName, int *type))()
{
	const char *name;
	unsigned int i;

	name = *pName;

	for ( i = 0; i < COUNT_OF(functions); ++i )
	{
		if ( !strcmp(name, functions[i].name) )
		{
			*pName = functions[i].name;
			*type = functions[i].developer;
			return functions[i].call;
		}
	}

	return NULL;
}

void (*Scr_GetMethod(const char **pName, int *type))(scr_entref_t)
{
	void (*scriptent_meth)(scr_entref_t);
	void (*scr_meth)(scr_entref_t);
	void (*hud_meth)(scr_entref_t);
	void (*meth)(scr_entref_t);

	*type = 0;

	meth = Player_GetMethod(pName);
	scriptent_meth = ScriptEnt_GetMethod(pName);

	if ( !meth )
		meth = scriptent_meth;

	scr_meth = BuiltIn_GetMethod(pName, type);

	if ( !meth )
		meth = scr_meth;

	hud_meth = HudElem_GetMethod(pName);

	if ( !meth )
		return hud_meth;

	return meth;
}

void Scr_ParseGameTypeList()
{
	size_t fileLength;
	fileHandle_t f;
	char listbuf[4096];
	char buffer[1024];
	char *path;
	gameTypeScript_t *script;
	char *token;
	const char *data;
	int list;
	int count;
	int len;
	int i;
	char *filename;

	memset(buffer, 0, sizeof(buffer));
	count = 0;
	memset(g_scr_data.gametype.list, 0, sizeof(g_scr_data.gametype.list));
	list = FS_GetFileList("maps/mp/gametypes", "gsc", FS_LIST_PURE_ONLY, listbuf, sizeof(listbuf));
	filename = listbuf;

	for ( i = 0; i < list; ++i )
	{
		script = &g_scr_data.gametype.list[count];
		fileLength = strlen(filename);

		if ( *filename == 95 )
		{
			filename += fileLength + 1;
		}
		else
		{
			if ( !I_stricmp(&filename[fileLength - 4], ".gsc") )
				filename[fileLength - 4] = 0;

			if ( count == 32 )
			{
				Com_Printf("Too many game type scripts found! Only loading the first %i\n", 31);
				break;
			}

			I_strncpyz(script->pszScript, filename, 64);
			I_strlwr(script->pszScript);
			path = va("maps/mp/gametypes/%s.txt", filename);
			len = FS_FOpenFileByMode(path, &f, FS_READ);

			if ( len > 0 && len < 1024 )
			{
				FS_Read(buffer, len, f);
				data = buffer;
				token = Com_Parse(&data);
				I_strncpyz(script->pszName, token, 64);
				token = Com_Parse(&data);
				script->bTeamBased = token && !I_stricmp(token, "team");
			}
			else
			{
				if ( len > 0 )
				{
					Com_Printf("WARNING: GameType description file %s is too big to load.\n", va("maps/mp/gametypes/%s.txt", filename));
				}
				else
				{
					Com_Printf("WARNING: Could not load GameType description file %s for gametype %s\n", va("maps/mp/gametypes/%s.txt", filename), filename);
				}

				I_strncpyz(script->pszName, script->pszScript, 64);
				script->bTeamBased = 0;
			}

			++count;

			if ( len > 0 )
				FS_FCloseFile(f);

			filename += fileLength + 1;
		}
	}

	g_scr_data.gametype.iNumGameTypes = count;
}

extern dvar_t *g_gametype;
void GScr_LoadGameTypeScript()
{
	char s[64];

	Com_sprintf(s, sizeof(s), "maps/mp/gametypes/%s", g_gametype->current.string);

	g_scr_data.gametype.main = Scr_GetFunctionHandle(s, "main", 1);
	g_scr_data.gametype.startupgametype = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_StartGameType", 1);
	g_scr_data.gametype.playerconnect = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerConnect", 1);
	g_scr_data.gametype.playerdisconnect = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerDisconnect", 1);
	g_scr_data.gametype.playerdamage = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerDamage", 1);
	g_scr_data.gametype.playerkilled = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerKilled", 1);
}

void GScr_LoadLevelScript()
{
	dvar_t *mapname;
	char s[64];

	mapname = Dvar_RegisterString("mapname", "", 0x1044u);
	Com_sprintf(s, sizeof(s), "maps/mp/%s", mapname->current.string);
	g_scr_data.levelscript = Scr_GetFunctionHandle(s, "main", 0);
}

void GScr_LoadFields()
{
	int classnum;

	for ( classnum = 0; classnum < CLASS_NUM_COUNT; classnum++ )
		Scr_SetClassMap(classnum);

	GScr_AddFieldsForEntity();
	GScr_AddFieldsForHudElems();
	GScr_AddFieldsForRadiant();
}

void GScr_LoadScripts()
{
	Scr_BeginLoadScripts();
	g_scr_data.deletestruct = Scr_GetFunctionHandle("codescripts/delete", "main", 1);
	g_scr_data.initstructs = Scr_GetFunctionHandle("codescripts/struct", "initstructs", 1);
	g_scr_data.createstruct = Scr_GetFunctionHandle("codescripts/struct", "createstruct", 1);
	GScr_LoadGameTypeScript();
	GScr_LoadLevelScript();
	Scr_PostCompileScripts();
	GScr_LoadFields();
	Scr_EndLoadScripts();
}