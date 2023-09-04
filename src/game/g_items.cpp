#include "../qcommon/qcommon.h"
#include "g_shared.h"

int itemRegistered[1024];

int IsItemRegistered(unsigned int iItemIndex)
{
	return itemRegistered[iItemIndex];
}

void RegisterItem(unsigned int index, int global)
{
	const char *name;

	if ( !itemRegistered[index] )
	{
		if ( !level.initializing )
		{
			name = bg_itemlist[index].pickup_name;

			if ( !name || !*name )
				name = "<<unknown>>";

			Scr_Error(va("game tried to register the item '%s' after initialization finished\n", name));
		}

		itemRegistered[index] = 1;

		if ( bg_itemlist[index].world_model )
			G_ModelIndex(bg_itemlist[index].world_model);

		if ( bg_itemlist[index].view_model )
			G_ModelIndex(bg_itemlist[index].view_model);

		if ( global )
			level.bRegisterItems = 1;
	}
}

void SaveRegisteredItems()
{
	char last_non_zero_char;
	char *ptr;
	int bits;
	int digit;
	int n;
	int weapIdx;
	char string[256];

	level.bRegisterItems = 0;

	n = 0;
	digit = 0;
	bits = 0;

	for ( weapIdx = 0; weapIdx < 131; ++weapIdx )
	{
		if ( itemRegistered[weapIdx] )
			digit += 1 << bits;

		if ( ++bits == 4 )
		{
			ptr = &string[n++];

			if ( digit > 9 )
				last_non_zero_char = digit + 87;
			else
				last_non_zero_char = digit + 48;

			*ptr = last_non_zero_char;

			digit = 0;
			bits = 0;
		}
	}

	if ( bits )
	{
		ptr = &string[n++];

		if ( digit > 9 )
			last_non_zero_char = digit + 87;
		else
			last_non_zero_char = digit + 48;

		*ptr = last_non_zero_char;
	}

	string[n] = 0;
	SV_SetConfigstring(8u, string);
}

void SaveRegisteredWeapons()
{
	WeaponDef *weaponDef;
	int weapon;
	char string[8192];

	level.registerWeapons = 0;
	string[0] = 0;
	weaponDef = 0;

	for ( weapon = 1; weapon <= BG_GetNumWeapons(); ++weapon )
	{
		if ( weaponDef )
			I_strncat(string, 8192, " ");

		weaponDef = BG_GetWeaponDef(weapon);
		I_strncat(string, 8192, weaponDef->szInternalName);
	}

	SV_SetConfigstring(7u, string);
}

extern dvar_t *g_maxDroppedWeapons;
int GetFreeDropCueIdx()
{
	int maxDroppedWeapon;
	gentity_s *ent;
	float fBestDistSqrd;
	float fClientDistSqrd;
	float fDistSqrd;
	int iBest;
	int j;
	int i;

	iBest = 0;
	fBestDistSqrd = -1.0;
	maxDroppedWeapon = g_maxDroppedWeapons->current.integer;

	for ( i = 0; i < maxDroppedWeapon; ++i )
	{
		ent = level.droppedWeaponCue[i];

		if ( !ent )
			return i;

		fDistSqrd = 9.9999803e11;

		for ( j = 0; j < level.maxclients; ++j )
		{
			if ( level.clients[j].sess.connected == CON_CONNECTED && level.clients[j].sess.sessionState == SESS_STATE_PLAYING )
			{
				fClientDistSqrd = Vec3DistanceSq(g_entities[j].r.currentOrigin, ent->r.currentOrigin);

				if ( fDistSqrd > fClientDistSqrd )
					fDistSqrd = fClientDistSqrd;
			}
		}

		if ( fDistSqrd > fBestDistSqrd )
		{
			fBestDistSqrd = fDistSqrd;
			iBest = i;
		}
	}

	G_FreeEntity(level.droppedWeaponCue[iBest]);
	level.droppedWeaponCue[iBest] = 0;

	return iBest;
}

void G_GetItemClassname(const gitem_s *item, unsigned short *out)
{
	char classname[256];
	WeaponDef *weaponDef;
	int index;

	index = item - bg_itemlist;

	if ( index > BG_GetNumWeapons() )
	{
		G_SetConstString(out, item->classname);
	}
	else
	{
		weaponDef = BG_GetWeaponDef(index);
		Com_sprintf(classname, 0x100u, "weapon_%s", weaponDef->szInternalName);
		G_SetConstString(out, classname);
	}
}

gentity_s* LaunchItem(const gitem_s *item, float *origin, float *angles, int ownerNum)
{
	gentity_s *ent;
	int itemIndex;

	itemIndex = item - bg_itemlist;
	RegisterItem(itemIndex, 1);
	ent = G_Spawn();
	level.droppedWeaponCue[GetFreeDropCueIdx()] = ent;
	ent->s.eType = ET_ITEM;
	ent->s.index = itemIndex;
	G_GetItemClassname(item, &ent->classname);
	ent->item.index = itemIndex;

	if ( item->giType == IT_WEAPON )
	{
		VectorSet(ent->r.mins, -1.0, -1.0, -1.0);
		VectorSet(ent->r.maxs, 1.0, 1.0, 1.0);
	}
	else
	{
		VectorSet(ent->r.mins, -1.0, -1.0, 0.0);
		VectorSet(ent->r.maxs, 1.0, 1.0, 2.0);
	}

	ent->r.contents = 1079771400;

	if ( item->giType != IT_AMMO )
		ent->r.contents |= 0x200000u;

	ent->s.clientNum = ownerNum;
	G_SetModel(ent, item->world_model);
	G_DObjUpdate(ent);
	ent->handler = 15;
	G_SetOrigin(ent, origin);
	ent->s.pos.trType = TR_GRAVITY;
	ent->s.pos.trTime = level.time;
	VectorCopy(angles, ent->s.pos.trDelta);
	ent->nextthink = level.time + 1000;
	ent->flags = 16;
	SV_LinkEntity(ent);

	return ent;
}

extern dvar_t *g_dropUpSpeedBase;
extern dvar_t *g_dropForwardSpeed;
extern dvar_t *g_dropUpSpeedRand;
gentity_s* Drop_Item(gentity_s *ent, const gitem_s *item, float angle, int novelocity)
{
	vec3_t vPos;
	vec3_t angles;
	vec3_t velocity;

	VectorCopy(ent->r.currentAngles, angles);

	angles[1] = angles[1] + angle;
	angles[0] = 0.0;
	angles[2] = 0.0;

	if ( novelocity )
	{
		VectorClear(velocity);
	}
	else
	{
		AngleVectors(angles, velocity, 0, 0);
		VectorScale(velocity, g_dropForwardSpeed->current.decimal, velocity);
		velocity[2] = G_crandom() * g_dropUpSpeedRand->current.decimal + g_dropUpSpeedBase->current.decimal + velocity[2];
	}

	VectorCopy(ent->r.currentOrigin, vPos);
	vPos[2] = (ent->r.maxs[2] - ent->r.mins[2]) * 0.5 + vPos[2];

	return LaunchItem(item, vPos, velocity, ent->s.number);
}

int G_ItemClipMask(gentity_s *ent)
{
	int clipmask;

	clipmask = ent->clipmask;

	if ( !clipmask )
		return 1169;

	return clipmask;
}

gentity_s* Drop_Weapon(gentity_s *entity, int weapon, unsigned int tag)
{
	int clipmask;
	float randomAmmo;
	float randomClip;
	float randomx;
	float randomy;
	float randomz;
	int randomCount;
	int ammoMin;
	float randomTemp;
	trace_t trace;
	vec3_t vCenter;
	vec3_t angles;
	float tagMat[4][3];
	vec3_t delta;
	int dropAmmoMin;
	int dropAmmoMax;
	int clipSize;
	gitem_s *item;
	gentity_s *ent;
	int weaponIndex;
	int ammoIndex;
	int clipIndex;
	int clientAmmoCount;

	item = &bg_itemlist[weapon];

	if ( entity->client && !COM_BitTest(entity->client->ps.weapons, weapon)
	        || (ammoIndex = BG_AmmoForWeapon(weapon), weaponIndex = BG_ClipForWeapon(weapon), BG_GetWeaponDef(weapon)->clipOnly)
	        && !entity->client->ps.ammoclip[weaponIndex] )
	{
		BG_TakePlayerWeapon(&entity->client->ps, weapon);
		return 0;
	}
	else
	{
		ent = Drop_Item(entity, item, 0.0, 0);

		if ( entity->client )
		{
			clientAmmoCount = entity->client->ps.ammo[ammoIndex];
			entity->client->ps.ammo[ammoIndex] = 0;
			clipIndex = entity->client->ps.ammoclip[weaponIndex];
			entity->client->ps.ammoclip[weaponIndex] = 0;
			BG_TakePlayerWeapon(&entity->client->ps, weapon);
		}
		else
		{
			dropAmmoMax = BG_GetWeaponDef(weapon)->dropAmmoMax;
			dropAmmoMin = BG_GetWeaponDef(weapon)->dropAmmoMin;

			if ( dropAmmoMax < dropAmmoMin )
			{
				dropAmmoMax = dropAmmoMin;
				dropAmmoMin = BG_GetWeaponDef(weapon)->dropAmmoMax;
			}

			if ( dropAmmoMax || dropAmmoMin )
			{
				if ( dropAmmoMax >= 0 )
				{
					if ( dropAmmoMax == dropAmmoMin )
						ammoMin = dropAmmoMin;
					else
						ammoMin = rand() % (dropAmmoMax - dropAmmoMin) + dropAmmoMin;

					clientAmmoCount = ammoMin;

					if ( ammoMin > 0 )
					{
						clipSize = BG_GetClipSize(weaponIndex);

						if ( clipSize )
							randomCount = rand() % clipSize;
						else
							randomCount = 0;

						clipIndex = randomCount;

						if ( randomCount < clientAmmoCount )
						{
							clientAmmoCount -= clipIndex;
						}
						else
						{
							clipIndex = clientAmmoCount;
							clientAmmoCount = 0;
						}
					}
					else
					{
						clientAmmoCount = 0;
						clipIndex = 0;
					}
				}
				else
				{
					clientAmmoCount = 0;
					clipIndex = 0;
				}
			}
			else
			{
				randomTemp = (G_random() + 1.0) * 0.5;
				randomAmmo = (float)(BG_GetClipSize(weaponIndex) - 1) * randomTemp;
				clientAmmoCount = Q_rint(randomAmmo) + 1;
				randomClip = (G_random() * 0.5 + 0.25) * (float)clientAmmoCount;
				clipIndex = Q_rint(randomClip);
				clientAmmoCount -= clipIndex;
			}
		}

		ent->count = clientAmmoCount;
		ent->item.clipAmmoCount = clipIndex;

		if ( !ent->count )
			ent->count = -1;

		if ( !ent->item.clipAmmoCount )
			ent->item.clipAmmoCount = -1;

		if ( !clientAmmoCount && !clipIndex )
			ent->r.contents &= ~0x200000u;

		if ( tag )
		{
			if ( G_DObjGetWorldTagMatrix(entity, tag, tagMat) )
			{
				vCenter[0] = (float)(ent->r.mins[0] + ent->r.maxs[0]) * 0.5;
				vCenter[1] = (float)(ent->r.mins[1] + ent->r.maxs[1]) * 0.5;
				vCenter[2] = (float)(ent->r.mins[2] + ent->r.maxs[2]) * 0.5;
				VectorAdd(vCenter, entity->r.currentOrigin, vCenter);
				clipmask = G_ItemClipMask(ent);
				G_TraceCapsule(&trace, vCenter, ent->r.mins, ent->r.maxs, tagMat[3], entity->s.number, clipmask);
				Vec3Lerp(vCenter, tagMat[3], trace.fraction, ent->s.pos.trBase);
				VectorCopy(ent->s.pos.trBase, ent->r.currentOrigin);
				ent->s.pos.trTime = level.time;
				AxisToAngles(tagMat, angles);
			}
			else
			{
				VectorCopy(entity->r.currentAngles, angles);
			}

			angles[2] = angles[2] + 90.0;
			G_SetAngle(ent, entity->r.currentAngles);
			randomz = G_crandom() * 60.0;
			randomy = G_crandom() * 40.0;
			randomx = G_crandom() * 50.0;
			VectorSet(delta, randomx, randomy, randomz);
			ent->s.apos.trType = TR_LINEAR;
			ent->s.apos.trTime = level.time;
			VectorCopy(delta, ent->s.apos.trDelta);
		}

		return ent;
	}
}

void ClearRegisteredItems()
{
	memset(itemRegistered, 0, sizeof(itemRegistered));
	itemRegistered[0] = 1;
}

int G_RegisterWeapon(int weapIndex)
{
	WeaponDef *weaponDef;
	unsigned int modelIndex;

	itemRegistered[weapIndex] = 1;
	level.bRegisterItems = 1;
	level.registerWeapons = 1;
	weaponDef = BG_GetWeaponDef(weapIndex);

	if ( *weaponDef->useHintString && !G_GetHintStringIndex(&weaponDef->useHintStringIndex, weaponDef->useHintString) )
		Com_Error(ERR_DROP, "Too many different hintstring values on weapons. Max allowed is %i different strings", 32);

	if ( *weaponDef->dropHintString && !G_GetHintStringIndex(&weaponDef->dropHintStringIndex, weaponDef->dropHintString) )
		Com_Error(ERR_DROP, "Too many different hintstring values on weapons. Max allowed is %i different strings", 32);

	modelIndex = G_ModelIndex(weaponDef->worldModel);

	if ( modelIndex && G_XModelBad(modelIndex) )
		G_OverrideModel(modelIndex, "xmodel/defaultweapon");

	return G_ModelIndex(weaponDef->projectileModel);
}

gitem_s* G_GetItemForClassname(const char *name)
{
	int i;
	int weapon;

	if ( !strncmp(name, "weapon_", 7) && (weapon = G_GetWeaponIndexForName(name + 7)) != 0 )
	{
		BG_GetWeaponDef(weapon);
		return BG_FindItemForWeapon(weapon);
	}
	else
	{
		for ( i = 129; i < 131; ++i )
		{
			if ( !strcmp(bg_itemlist[i].classname, name) )
				return &bg_itemlist[i];
		}

		return 0;
	}
}

gitem_s* G_FindItem(const char *pickupName)
{
	int i;
	int iIndex;

	for ( i = 129; i < 131; ++i )
	{
		if ( !I_stricmp(bg_itemlist[i].pickup_name, pickupName)
		        || !I_stricmp(bg_itemlist[i].classname, pickupName) )
		{
			return &bg_itemlist[i];
		}
	}

	iIndex = G_GetWeaponIndexForName(pickupName);

	if ( iIndex )
		return &bg_itemlist[iIndex];
	else
		return 0;
}

void G_SpawnItem(gentity_s *ent, const gitem_s *item)
{
	RegisterItem(item - bg_itemlist, 0);
	ent->item.index = item - bg_itemlist;
	G_SetModel(ent, item->world_model);

	if ( item->giType == IT_WEAPON )
	{
		VectorSet(ent->r.mins, -1.0, -1.0, -1.0);
		VectorSet(ent->r.maxs, 1.0, 1.0, 1.0);
	}
	else
	{
		VectorSet(ent->r.mins, -1.0, -1.0, 0.0);
		VectorSet(ent->r.maxs, 1.0, 1.0, 2.0);
	}

	ent->r.contents = 1079771400;

	if ( item->giType != IT_AMMO )
		ent->r.contents |= 0x200000u;

	ent->s.eType = ET_ITEM;
	ent->s.index = ent->item.index;
	G_DObjUpdate(ent);
	ent->s.clientNum = 1022;
	ent->flags |= 0x1000u;

	if ( level.spawnVars.spawnVarsValid )
	{
		G_SetAngle(ent, ent->r.currentAngles);
		ent->nextthink = level.time + 100;
		ent->handler = 16;
	}
	else
	{
		ent->handler = 17;

		if ( ((LOBYTE(ent->spawnflags) ^ 1) & 1) != 0 )
		{
			ent->s.groundEntityNum = 1023;

			if ( item->giType == IT_WEAPON )
				ent->r.currentAngles[2] = ent->r.currentAngles[2] + 90.0;
		}

		G_SetAngle(ent, ent->r.currentAngles);
		G_SetOrigin(ent, ent->r.currentOrigin);
		SV_LinkEntity(ent);
	}
}

void Fill_Clip(playerState_s *ps, int weapon)
{
	int weaponIndex;
	int iAmmoIndex;
	int ammomove;
	int inclip;

	iAmmoIndex = BG_AmmoForWeapon(weapon);
	weaponIndex = BG_ClipForWeapon(weapon);

	if ( weapon > 0 && weapon <= BG_GetNumWeapons() )
	{
		inclip = ps->ammoclip[weaponIndex];
		ammomove = BG_GetClipSize(weaponIndex) - inclip;

		if ( ammomove > ps->ammo[iAmmoIndex] )
			ammomove = ps->ammo[iAmmoIndex];

		if ( ammomove )
		{
			ps->ammo[iAmmoIndex] -= ammomove;
			ps->ammoclip[weaponIndex] += ammomove;
		}
	}
}

int Add_Ammo(gentity_s *pSelf, int weaponIndex, int count, int fillClip)
{
	int clip;
	int ammo;
	int clipOnly;
	int oldClip;
	int oldAmmo;
	int ammoCount;

	oldAmmo = BG_AmmoForWeapon(weaponIndex);
	oldClip = BG_ClipForWeapon(weaponIndex);
	clipOnly = 0;
	ammo = pSelf->client->ps.ammo[oldAmmo];
	clip = pSelf->client->ps.ammoclip[oldClip];
	pSelf->client->ps.ammo[oldAmmo] = ammo + count;

	if ( BG_WeaponIsClipOnly(weaponIndex) )
	{
		G_GivePlayerWeapon(&pSelf->client->ps, weaponIndex);
		clipOnly = 1;
	}

	if ( fillClip || clipOnly )
		Fill_Clip(&pSelf->client->ps, weaponIndex);

	if ( clipOnly )
	{
		pSelf->client->ps.ammo[oldAmmo] = 0;
	}
	else
	{
		if ( pSelf->client->ps.ammo[oldAmmo] > BG_GetMaxAmmo(oldAmmo) )
		{
			pSelf->client->ps.ammo[oldAmmo] = BG_GetMaxAmmo(oldAmmo);
		}
	}

	if ( pSelf->client->ps.ammoclip[oldClip] > BG_GetClipSize(oldClip) )
	{
		pSelf->client->ps.ammoclip[oldClip] = BG_GetClipSize(oldClip);
	}

	if ( BG_GetWeaponDef(weaponIndex)->sharedAmmoCapIndex < 0 )
		return pSelf->client->ps.ammoclip[oldClip] - clip + pSelf->client->ps.ammo[oldAmmo] - ammo;

	ammoCount = BG_GetMaxPickupableAmmo(&pSelf->client->ps, weaponIndex);

	if ( ammoCount >= 0 )
		return pSelf->client->ps.ammoclip[oldClip] - clip + pSelf->client->ps.ammo[oldAmmo] - ammo;

	if ( !BG_WeaponIsClipOnly(weaponIndex) )
	{
		pSelf->client->ps.ammo[oldAmmo] += ammoCount;

		if ( pSelf->client->ps.ammo[oldAmmo] < 0 )
			pSelf->client->ps.ammo[oldAmmo] = 0;

		return pSelf->client->ps.ammoclip[oldClip] - clip + pSelf->client->ps.ammo[oldAmmo] - ammo;
	}

	pSelf->client->ps.ammoclip[oldClip] += ammoCount;

	if ( pSelf->client->ps.ammoclip[oldClip] > 0 )
		return pSelf->client->ps.ammoclip[oldClip] - clip + pSelf->client->ps.ammo[oldAmmo] - ammo;

	pSelf->client->ps.ammoclip[oldClip] = 0;
	BG_TakePlayerWeapon(&pSelf->client->ps, weaponIndex);

	return 0;
}

qboolean Pickup_Ammo(gentity_s *ent, gentity_s *other)
{
	char *pickupMsg;
	WeaponDef *weaponDef;
	char *pickupCmd;
	gitem_s *item;
	int count;

	item = &bg_itemlist[ent->item.index];

	if ( ent->count )
		count = ent->count;
	else
		count = item->quantity;

	if ( !Add_Ammo(other, item->giTag, count, 0) )
		return 0;

	if ( BG_WeaponIsClipOnly(item->giTag) )
	{
		weaponDef = BG_GetWeaponDef(item->giTag);
		pickupMsg = va("%c \"GAME_PICKUP_CLIPONLY_AMMO\x14%s\"", 102, weaponDef->szDisplayName);
	}
	else
	{
		weaponDef = BG_GetWeaponDef(item->giTag);
		pickupMsg = va("%c \"GAME_PICKUP_AMMO\x14%s\"", 102, weaponDef->szDisplayName);
	}

	SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, pickupMsg);
	Scr_AddEntity(other);
	Scr_Notify(ent, scr_const.trigger, 1u);

	if ( BG_GetWeaponDef(item->giTag)->offhandClass )
		pickupCmd = va("%c \"%i\"", 73, 4);
	else
		pickupCmd = va("%c \"%i\"", 73, 1);

	SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, pickupCmd);
	return 1;
}

int Pickup_Health(gentity_s *ent, gentity_s *other)
{
	int healthScale;
	int healthToRestore;
	int maxHealthScale;
	gitem_s *item;
	int oldHealth;
	int max;
	int newHealth;

	item = &bg_itemlist[ent->item.index];

	if ( item->quantity == 5 || item->quantity == 100 )
		newHealth = 2 * other->client->ps.stats[2];
	else
		newHealth = other->client->ps.stats[2];

	if ( ent->count )
		max = ent->count;
	else
		max = item->quantity;

	oldHealth = other->health;
	other->health = oldHealth + (int)((float)max * (float)other->client->ps.stats[2] * 0.0099999998);

	if ( other->health <= newHealth )
	{
		maxHealthScale = (int)((float)(100 * other->health) / (float)other->client->ps.stats[2]);

		if ( maxHealthScale > 0 )
		{
			if ( maxHealthScale > 100 )
				maxHealthScale = 100;
		}
		else
		{
			maxHealthScale = 1;
		}

		healthScale = (int)((float)(100 * oldHealth) / (float)other->client->ps.stats[2]);

		if ( healthScale <= 0 )
			healthScale = 1;

		healthToRestore = max + healthScale;

		if ( healthToRestore > 100 )
			healthToRestore = 100;

		if ( maxHealthScale != healthToRestore )
			other->health = other->client->ps.stats[2] * healthToRestore / 100;
	}
	else
	{
		other->health = newHealth;
	}

	other->client->ps.stats[0] = other->health;

	SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_PICKUP_HEALTH\x15%i\"", 102, max));
	SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"%i\"", 73, 0));

	Scr_AddEntity(other);
	Scr_Notify(ent, scr_const.trigger, 1u);

	return 1;
}

void G_SelectWeaponIndex(int clientnum, int iWeaponIndex)
{
	SV_GameSendServerCommand(clientnum, SV_CMD_RELIABLE, va("%c %i", 97, iWeaponIndex));
}

extern dvar_t *g_weaponAmmoPools;
int Pickup_Weapon(gentity_s *ent, gentity_s *other, int *pickupEvent, int touched)
{
	int clipForRandom;
	int otherAmmoIndex;
	int ammoIndex;
	int clipIndex;
	int clipForWeapon;
	int otherClipForWeapon;
	bool hasWeapon;
	WeaponDef *debugWeaponDef;
	WeaponDef *weaponStackDef;
	char *pickupMsg;
	int newClipForWeapon;
	int newClipForWeapon2;
	int newClipForWeapon3;
	gclient_s *client;
	char *pickupCmd;
	float randomClipSize;
	int dropAmmoRandom;
	float tempRandom;
	int currentClip;
	int currentAmmo;
	int dropAmmoMax;
	int i;
	int dropAmmoMin;
	int currentSlot;
	WeaponDef *weaponDef;
	gentity_s *weaponEnt;
	int newAmmoCount;
	int weaponIndex;
	int bHasWeapon;
	int clipAmmoCount;
	int weaponModel;
	int oldAmmoCount;

	weaponEnt = 0;
	weaponIndex = bg_itemlist[ent->item.index].giTag;
	weaponDef = BG_GetWeaponDef(weaponIndex);

	if ( ent->count >= 0 )
	{
		if ( !ent->count )
		{
			dropAmmoMax = weaponDef->dropAmmoMax;
			dropAmmoMin = weaponDef->dropAmmoMin;
			if ( dropAmmoMax < dropAmmoMin )
			{
				dropAmmoMax = weaponDef->dropAmmoMin;
				dropAmmoMin = weaponDef->dropAmmoMax;
			}
			if ( dropAmmoMax || dropAmmoMin )
			{
				if ( dropAmmoMax >= 0 )
				{
					if ( dropAmmoMax == dropAmmoMin )
						dropAmmoRandom = dropAmmoMin;
					else
						dropAmmoRandom = rand() % (dropAmmoMax - dropAmmoMin) + dropAmmoMin;
					ent->count = dropAmmoRandom;
					if ( ent->count <= 0 )
						ent->count = 0;
				}
				else
				{
					ent->count = 0;
				}
			}
			else
			{
				tempRandom = (G_random() + 1.0) * 0.5;
				clipForRandom = BG_ClipForWeapon(weaponIndex);
				randomClipSize = (float)(BG_GetClipSize(clipForRandom) - 1) * tempRandom;
				ent->count = Q_rint(randomClipSize) + 1;
			}
		}
		otherAmmoIndex = BG_AmmoForWeapon(weaponIndex);
		if ( ent->count > BG_GetMaxAmmo(otherAmmoIndex) )
		{
			ammoIndex = BG_AmmoForWeapon(weaponIndex);
			ent->count = BG_GetMaxAmmo(ammoIndex);
		}
		weaponModel = ent->count;
	}
	else
	{
		weaponModel = 0;
	}
	if ( ent->item.clipAmmoCount >= 0 )
	{
		if ( !ent->item.clipAmmoCount )
		{
			if ( ent->count >= 0 )
			{
				clipIndex = BG_ClipForWeapon(weaponIndex);
				ent->item.clipAmmoCount = BG_GetClipSize(clipIndex);
				if ( ent->item.clipAmmoCount > ent->count )
					ent->item.clipAmmoCount = ent->count;
				ent->count -= ent->item.clipAmmoCount;
				weaponModel = ent->count;
			}
			else
			{
				ent->item.clipAmmoCount = 0;
			}
		}
		clipForWeapon = BG_ClipForWeapon(weaponIndex);
		if ( ent->item.clipAmmoCount > BG_GetClipSize(clipForWeapon) )
		{
			otherClipForWeapon = BG_ClipForWeapon(weaponIndex);
			ent->item.clipAmmoCount = BG_GetClipSize(otherClipForWeapon);
		}
		clipAmmoCount = ent->item.clipAmmoCount;
	}
	else
	{
		clipAmmoCount = 0;
	}
	hasWeapon = COM_BitTest(other->client->ps.weapons, weaponIndex);
	bHasWeapon = hasWeapon;
	if ( !hasWeapon )
	{
		if ( BG_DoesWeaponNeedSlot(weaponIndex) )
		{
			if ( other->client->ps.weapon )
			{
				if ( !COM_BitTest(other->client->ps.weapons, other->client->ps.weapon) )
					return 0;
				if ( !BG_PlayerHasWeapon(&other->client->ps, other->client->ps.weapon, 1) )
				{
					debugWeaponDef = BG_GetWeaponDef(other->client->ps.weapon);
					if ( !BG_GetStackableSlot(other->client, other->client->ps.weapon, debugWeaponDef->weaponSlot)
					        && !BG_GetEmptySlotForWeapon(&other->client->ps, weaponIndex) )
					{
						Com_Printf("WARNING: cannot swap out a debug weapon (can result from too many weapons given to the player)\n");
						return 0;
					}
				}
			}
			if ( !BG_GetEmptySlotForWeapon(&other->client->ps, weaponIndex) )
			{
				weaponStackDef = BG_GetWeaponDef(other->client->ps.weapon);
				if ( !BG_GetStackableSlot(other->client, weaponIndex, weaponStackDef->weaponSlot) )
				{
					if ( weaponDef->weaponSlot == BG_GetWeaponDef(other->client->ps.weapon)->weaponSlot )
					{
						weaponEnt = Drop_Weapon(other, other->client->ps.weapon, 0);
					}
					else
					{
						for ( i = 1; i <= 2; ++i )
						{
							currentSlot = other->client->ps.weaponslots[i];
							currentAmmo = BG_AmmoForWeapon(weaponIndex);
							currentClip = BG_ClipForWeapon(weaponIndex);
							if ( !other->client->ps.ammo[currentAmmo] && !other->client->ps.ammoclip[currentClip] )
							{
								weaponEnt = Drop_Weapon(other, currentSlot, 0);
								break;
							}
						}
						if ( i > 2 )
						{
							SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_CANT_GET_PRIMARY_WEAP_MESSAGE\"", 102));
							return 0;
						}
					}
					if ( !weaponEnt )
						return 0;
					weaponEnt->spawnflags = ent->spawnflags;
					weaponEnt->s.groundEntityNum = ent->s.groundEntityNum;
					G_SetOrigin(weaponEnt, ent->r.currentOrigin);
					G_SetAngle(weaponEnt, ent->r.currentAngles);
					SV_LinkEntity(weaponEnt);
				}
			}
			G_GivePlayerWeapon(&other->client->ps, weaponIndex);
			if ( !touched )
				G_SelectWeaponIndex(other - g_entities, weaponIndex);
		}
		else
		{
			G_GivePlayerWeapon(&other->client->ps, weaponIndex);
		}
	}
	if ( bHasWeapon )
	{
		*pickupEvent = EV_AMMO_PICKUP;
		oldAmmoCount = clipAmmoCount + weaponModel;
		newAmmoCount = Add_Ammo(other, weaponIndex, oldAmmoCount, 0);
		if ( newAmmoCount )
		{
			if ( BG_WeaponIsClipOnly(weaponIndex) )
				pickupMsg = va("%c \"GAME_PICKUP_CLIPONLY_AMMO\x14%s\"", 102, weaponDef->szDisplayName);
			else
				pickupMsg = va("%c \"GAME_PICKUP_AMMO\x14%s\"", 102, weaponDef->szDisplayName);
			SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, pickupMsg);
		}
		if ( newAmmoCount != oldAmmoCount )
		{
			ent->count -= newAmmoCount;
			if ( ent->count <= 0 )
			{
				ent->item.clipAmmoCount += ent->count;
				ent->count = -1;
				if ( ent->item.clipAmmoCount <= 0 )
					ent->item.clipAmmoCount = -1;
			}
			if ( (ent->count > 0 || ent->item.clipAmmoCount > 0) && g_weaponAmmoPools->current.boolean )
				return 0;
		}
	}
	else
	{
		if ( clipAmmoCount >= 0 )
		{
			newClipForWeapon = BG_ClipForWeapon(weaponIndex);
			if ( clipAmmoCount > BG_GetClipSize(newClipForWeapon) )
			{
				newClipForWeapon2 = BG_ClipForWeapon(weaponIndex);
				weaponModel += clipAmmoCount - BG_GetClipSize(newClipForWeapon2);
				newClipForWeapon3 = BG_ClipForWeapon(weaponIndex);
				clipAmmoCount = BG_GetClipSize(newClipForWeapon3);
			}
			client = other->client;
			client->ps.ammoclip[BG_ClipForWeapon(weaponIndex)] = clipAmmoCount;
		}
		Add_Ammo(other, weaponIndex, weaponModel, clipAmmoCount == -1);
	}
	if ( weaponEnt )
		Scr_AddEntity(weaponEnt);
	else
		Scr_AddUndefined();
	Scr_AddEntity(other);
	Scr_Notify(ent, scr_const.trigger, 2u);
	if ( weaponDef->offhandClass )
		pickupCmd = va("%c \"%i\"", 73, 4);
	else
		pickupCmd = va("%c \"%i\"", 73, 1);
	SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, pickupCmd);
	return 1;
}

void Touch_Item(gentity_s *ent, gentity_s *other, int touched)
{
	WeaponDef *weaponDef;
	int guid;
	int giType;
	gitem_s *item;
	char cleanname[64];
	int pickupEvent;
	qboolean pickedUp;

	pickupEvent = EV_ITEM_PICKUP;

	if ( ent->active )
	{
		ent->active = 0;

		if ( other->client )
		{
			if ( other->health > 0 && !level.clientIsSpawning )
			{
				item = &bg_itemlist[ent->item.index];

				if ( BG_CanItemBeGrabbed(&ent->s, &other->client->ps, touched) )
				{
					I_strncpyz(cleanname, other->client->sess.state.name, sizeof(cleanname));
					I_CleanStr(cleanname);

					if ( item->giType == IT_WEAPON )
					{
						weaponDef = BG_GetWeaponDef(item->giTag);
						guid = SV_GetGuid(other->s.number);
						G_LogPrintf("Weapon;%d;%d;%s;%s\n", guid, other->s.number, cleanname, weaponDef->szInternalName);
					}
					else
					{
						guid = SV_GetGuid(other->s.number);
						G_LogPrintf("Item;%d;%d;%s;%s\n", guid, other->s.number, cleanname, item->classname);
					}

					giType = item->giType;

					if ( giType == IT_AMMO )
					{
						pickedUp = Pickup_Ammo(ent, other);
					}
					else if ( giType > IT_AMMO )
					{
						if ( giType != IT_HEALTH )
							return;

						pickedUp = Pickup_Health(ent, other);
					}
					else
					{
						if ( giType != IT_WEAPON )
							return;

						pickedUp = Pickup_Weapon(ent, other, &pickupEvent, touched);
					}

					if ( pickedUp )
					{
						if ( other->client->sess.predictItemPickup )
							G_AddPredictableEvent(other, pickupEvent, ent->s.index);
						else
							G_AddEvent(other, pickupEvent, ent->s.index);

						G_FreeEntity(ent);
					}
				}
				else if ( !touched && ent->s.clientNum != other->s.number && item->giType == IT_WEAPON )
				{
					if ( COM_BitTest(other->client->ps.weapons, item->giTag) )
					{
						weaponDef = BG_GetWeaponDef(item->giTag);
						SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_PICKUP_CANTCARRYMOREAMMO\x14%s\"", 102, weaponDef->szDisplayName));
					}
					else if ( (unsigned int)(BG_GetWeaponDef(item->giTag)->weaponSlot - 1) <= 1 )
					{
						SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_CANT_GET_PRIMARY_WEAP_MESSAGE\"", 102));
					}
				}
			}
		}
	}
}

void FinishSpawningItem(gentity_s *ent)
{
	float positionZ;
	float vAxis[3][3];
	vec3_t angles;
	int contentmask;
	vec3_t endpos;
	vec3_t start;
	gitem_s *item;
	vec3_t maxs;
	vec3_t mins;
	vec3_t end;
	trace_t trace;

	ent->handler = 17;

	if ( (ent->spawnflags & 1) != 0 )
	{
		G_SetOrigin(ent, ent->r.currentOrigin);
	}
	else
	{
		item = &bg_itemlist[ent->item.index];

		if ( item->giType == IT_WEAPON )
		{
			VectorSet(mins, -1.0, -1.0, -1.0);
			VectorSet(maxs, 1.0, 1.0, 1.0);
		}
		else
		{
			VectorSet(mins, -1.0, -1.0, 0.0);
			VectorSet(maxs, 1.0, 1.0, 2.0);
		}

		contentmask = G_ItemClipMask(ent);
		VectorCopy(ent->r.currentOrigin, start);
		positionZ = ent->r.currentOrigin[2] - 4096.0;
		VectorSet(end, ent->r.currentOrigin[0], ent->r.currentOrigin[1], positionZ);
		G_TraceCapsule(&trace, start, mins, maxs, end, ent->s.number, contentmask);

		if ( trace.startsolid )
		{
			VectorCopy(ent->r.currentOrigin, start);
			start[2] = start[2] - 15.0;
			positionZ = ent->r.currentOrigin[2] - 4096.0;
			VectorSet(end, ent->r.currentOrigin[0], ent->r.currentOrigin[1], positionZ);
			G_TraceCapsule(&trace, start, mins, maxs, end, ent->s.number, contentmask);
		}

		if ( trace.startsolid )
		{
			Com_Printf("FinishSpawningItem: %s startsolid at %s\n", SL_ConvertToString(ent->classname), vtos(ent->r.currentOrigin));
			G_FreeEntity(ent);
			return;
		}

		ent->s.groundEntityNum = trace.entityNum;
		Vec3Lerp(start, end, trace.fraction, endpos);
		G_SetOrigin(ent, endpos);

		if ( trace.fraction < 1.0 )
		{
			VectorCopy(trace.normal, vAxis[2]);
			AngleVectors(ent->r.currentAngles, vAxis[0], 0, 0);
			Vec3Cross(vAxis[2], vAxis[0], vAxis[1]);
			Vec3Cross(vAxis[1], vAxis[2], vAxis[0]);
			AxisToAngles(vAxis, angles);

			if ( bg_itemlist[ent->s.index].giType == IT_WEAPON )
				angles[2] = angles[2] + 90.0;

			G_SetAngle(ent, angles);
		}
	}

	SV_LinkEntity(ent);
}

void DroppedItemClearOwner(gentity_s *pSelf)
{
	pSelf->s.clientNum = 1022;
}

#if COMPILE_PLAYER == 1
int player_disableitempickup[MAX_CLIENTS] = {0};
#endif

void Touch_Item_Auto(gentity_s *ent, gentity_s *other, int touched)
{
#if COMPILE_PLAYER == 1
	if (player_disableitempickup[other->s.number])
		return;
#endif
	ent->active = 1;
	Touch_Item(ent, other, touched);
}

void G_OrientItemToGround(gentity_s *ent, trace_t *trace)
{
	vec3_t vAngles;
	float vAxis[3][3];

	VectorCopy(trace->normal, vAxis[2]);
	AngleVectors(ent->r.currentAngles, vAxis[0], 0, 0);
	Vec3Cross(vAxis[2], vAxis[0], vAxis[1]);
	Vec3Cross(vAxis[1], vAxis[2], vAxis[0]);
	AxisToAngles(vAxis, vAngles);

	if ( bg_itemlist[ent->s.index].giType == IT_WEAPON )
		vAngles[2] = vAngles[2] + 90.0;

	G_SetAngle(ent, vAngles);
}

void G_RunItem(gentity_s *ent)
{
	vec3_t delta;
	float vLenSq;
	vec3_t endpos;
	int contentmask;
	int contents;
	trace_t trace;
	vec3_t origin;

	if ( (ent->s.groundEntityNum == 1023 || level.gentities[ent->s.groundEntityNum].s.pos.trType)
	        && ent->s.pos.trType != TR_GRAVITY
	        && ((LOBYTE(ent->spawnflags) ^ 1) & 1) != 0 )
	{
		ent->s.pos.trType = TR_GRAVITY;
		ent->s.pos.trTime = level.time;
		VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
		VectorClear(ent->s.pos.trDelta);
	}

	if ( ent->s.pos.trType == TR_STATIONARY || ent->s.pos.trType == TR_GRAVITY_PAUSED || ent->tagInfo )
	{
		G_RunThink(ent);
	}
	else
	{
		BG_EvaluateTrajectory(&ent->s.pos, level.time + 50, origin);
		contentmask = G_ItemClipMask(ent);

		if ( Vec3DistanceSq(ent->r.currentOrigin, origin) < 0.1 )
			origin[2] = origin[2] - 1.0;

		G_TraceCapsule(&trace, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, ent->r.ownerNum, contentmask);

		if ( trace.fraction >= 1.0 )
		{
			VectorCopy(origin, ent->r.currentOrigin);
		}
		else
		{
			Vec3Lerp(ent->r.currentOrigin, origin, trace.fraction, endpos);

			if ( !trace.startsolid && trace.fraction < 0.0099999998 && trace.normal[2] < 0.5 )
			{
				VectorSubtract(origin, ent->r.currentOrigin, delta);
				vLenSq = 1.0 - DotProduct(delta, trace.normal);
				VectorMA(origin, vLenSq, trace.normal, origin);
				G_TraceCapsule(&trace, endpos, ent->r.mins, ent->r.maxs, origin, ent->r.ownerNum, contentmask);
				Vec3Lerp(endpos, origin, trace.fraction, endpos);
			}

			ent->s.pos.trType = TR_LINEAR_STOP;
			ent->s.pos.trTime = level.time;
			ent->s.pos.trDuration = 50;
			VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
			VectorSubtract(endpos, ent->r.currentOrigin, ent->s.pos.trDelta);
			VectorScale(ent->s.pos.trDelta, 20.0, ent->s.pos.trDelta);
			VectorCopy(endpos, ent->r.currentOrigin);
		}

		SV_LinkEntity(ent);
		G_RunThink(ent);

		if ( ent->r.inuse && trace.fraction < 0.0099999998 )
		{
			if ( trace.normal[2] <= 0.0 || (contents = SV_PointContents(ent->r.currentOrigin, -1, 0x80000000)) != 0 )
			{
				G_FreeEntity(ent);
			}
			else
			{
				G_OrientItemToGround(ent, &trace);
				G_SetOrigin(ent, endpos);
				ent->s.groundEntityNum = trace.entityNum;
				SV_LinkEntity(ent);
			}
		}
	}
}