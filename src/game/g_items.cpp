#include "../qcommon/qcommon.h"
#include "g_shared.h"

/*
 * name:		g_items.c
 *
 * desc:		Items are any object that a player can touch to gain some effect.
 *				Pickup will return the number of seconds until they should respawn.
 *				all items should pop when dropped in lava or slime.
 *				Respawnable items don't actually go away when picked up, they are
 *				just made invisible and untouchable.  This allows them to ride
 *				movers and respawn apropriately.
 *
*/

static int itemRegistered[MAX_GENTITIES];

/*
==================
IsItemRegistered
==================
*/
int IsItemRegistered( int iItemIndex )
{
	assert((iItemIndex >= 0) && (iItemIndex < MAX_WEAPONS));
	return itemRegistered[iItemIndex];
}

/*
==================
ClearRegisteredItems
==================
*/
void ClearRegisteredItems()
{
	memset(itemRegistered, 0, sizeof(itemRegistered));
	itemRegistered[0] = qtrue;
}

/*
==================
DroppedItemClearOwner
==================
*/
void DroppedItemClearOwner( gentity_t *pSelf )
{
	pSelf->s.clientNum = ENTITYNUM_WORLD;
}

/*
==============
Fill_Clip
	push reserve ammo into available space in the clip
==============
*/
void Fill_Clip( playerState_t *ps, int weapon )
{
	int iClipIndex;
	int iAmmoIndex;
	int ammomove;
	int inclip;

	iAmmoIndex = BG_AmmoForWeapon(weapon);
	iClipIndex = BG_ClipForWeapon(weapon);

	if ( weapon <= 0 || weapon > BG_GetNumWeapons() )
	{
		return;
	}

	inclip = ps->ammoclip[iClipIndex];
	ammomove = BG_GetAmmoClipSize(iClipIndex) - inclip;

	if ( ammomove > ps->ammo[iAmmoIndex] )
	{
		ammomove = ps->ammo[iAmmoIndex];
	}

	if ( ammomove )
	{
		ps->ammo[iAmmoIndex] -= ammomove;
		ps->ammoclip[iClipIndex] += ammomove;
	}
}

/*
==============
Add_Ammo
	Try to always add ammo here unless you have specific needs
	(like the AI "infinite ammo" where they get below 900 and force back up to 999)

	fillClip will push the ammo straight through into the clip and leave the rest in reserve
==============
*/
//----(SA)	modified
// xkan, 10/25/2002 - modified to return whether any ammo was added.
int Add_Ammo( gentity_t *ent, int weaponIndex, int count, qboolean fillClip )
{
	int oldClip;
	int oldAmmo;
	qboolean clipOnly = qfalse;
	int clipweap;
	int ammoweap;

	assert(ent);
	assert(ent->client);

	ammoweap = BG_AmmoForWeapon(weaponIndex);
	clipweap = BG_ClipForWeapon(weaponIndex);

	oldAmmo = ent->client->ps.ammo[ammoweap];
	oldClip = ent->client->ps.ammoclip[clipweap];

	ent->client->ps.ammo[ammoweap] = oldAmmo + count;

	if ( BG_WeaponIsClipOnly(weaponIndex) )
	{
		G_GivePlayerWeapon(&ent->client->ps, weaponIndex);
		clipOnly = qtrue;
	}

	if ( fillClip || clipOnly )
	{
		Fill_Clip(&ent->client->ps, weaponIndex);
	}

	if ( clipOnly )
	{
		ent->client->ps.ammo[ammoweap] = 0;
	}
	else
	{
		if ( ent->client->ps.ammo[ammoweap] > BG_GetAmmoTypeMax(ammoweap) )
		{
			ent->client->ps.ammo[ammoweap] = BG_GetAmmoTypeMax(ammoweap);
		}
	}

	if ( ent->client->ps.ammoclip[clipweap] > BG_GetAmmoClipSize(clipweap) )
	{
		ent->client->ps.ammoclip[clipweap] = BG_GetAmmoClipSize(clipweap);
	}

	if ( BG_GetWeaponDef(weaponIndex)->sharedAmmoCapIndex < 0 )
	{
		return ent->client->ps.ammoclip[clipweap] - oldClip + ent->client->ps.ammo[ammoweap] - oldAmmo;
	}

	count = BG_GetMaxPickupableAmmo(&ent->client->ps, weaponIndex);

	if ( count >= 0 )
	{
		return ent->client->ps.ammoclip[clipweap] - oldClip + ent->client->ps.ammo[ammoweap] - oldAmmo;
	}

	if ( !BG_WeaponIsClipOnly(weaponIndex) )
	{
		ent->client->ps.ammo[ammoweap] += count;

		if ( ent->client->ps.ammo[ammoweap] < 0 )
		{
			ent->client->ps.ammo[ammoweap] = 0;
		}

		return ent->client->ps.ammoclip[clipweap] - oldClip + ent->client->ps.ammo[ammoweap] - oldAmmo;
	}

	ent->client->ps.ammoclip[clipweap] += count;

	if ( ent->client->ps.ammoclip[clipweap] > 0 )
	{
		return ent->client->ps.ammoclip[clipweap] - oldClip + ent->client->ps.ammo[ammoweap] - oldAmmo;
	}

	ent->client->ps.ammoclip[clipweap] = 0;
	BG_TakePlayerWeapon(&ent->client->ps, weaponIndex);

	return 0;
}

/*
==================
G_GetItemClassname
==================
*/
void G_GetItemClassname( const gitem_t *item, unsigned short *out )
{
	char classname[MAX_OSPATH];
	WeaponDef *weapDef;
	int index;

	index = item - bg_itemlist;

	if ( index > BG_GetNumWeapons() )
	{
		G_SetConstString(out, item->classname);
		return;
	}

	weapDef = BG_GetWeaponDef(index);
	Com_sprintf(classname, sizeof(classname), "weapon_%s", weapDef->szInternalName);
	G_SetConstString(out, classname);
}

/*
==================
SaveRegisteredItems
==================
*/
void SaveRegisteredItems()
{
	char string[MAX_OSPATH];
	int bits = 0;
	int n = 0;
	int digit = 0;
	int last_non_zero_char = 0;

	level.bRegisterItems = 0;

	for ( int itemIdx = 0; itemIdx < bg_numItems; itemIdx++ )
	{
		if ( itemRegistered[itemIdx] )
		{
			digit += 1 << bits;
		}

		bits++;

		if ( bits == 4 )
		{
			string[n] = digit + (digit < '\n' ? '0' : 'W');
			n++;
			last_non_zero_char = n;

			digit = 0;
			bits = 0;
		}
	}

	if ( bits )
	{
		string[n] = digit + (digit < '\n' ? '0' : 'W');
		n++;
		last_non_zero_char = n;
	}

	string[last_non_zero_char] = 0;
	SV_SetConfigstring(CS_ITEMS, string);
}

/*
==================
SaveRegisteredWeapons
==================
*/
void SaveRegisteredWeapons()
{
	WeaponDef *weapDef;
	int weapIndex;
	char string[BIG_INFO_STRING];

	level.registerWeapons = 0;

	string[0] = 0;
	weapDef = NULL;

	for ( weapIndex = 1; weapIndex <= BG_GetNumWeapons(); weapIndex++ )
	{
		if ( weapDef )
		{
			I_strncat(string, sizeof(string), " ");
		}

		weapDef = BG_GetWeaponDef(weapIndex);
		I_strncat(string, sizeof(string), weapDef->szInternalName);
	}

	SV_SetConfigstring(CS_WEAPONS, string);
}

/*
==================
RegisterItem
==================
*/
void RegisterItem( int iItemIndex, qboolean bUpdateCS )
{
	const char *name;

	if ( itemRegistered[iItemIndex] )
	{
		return;
	}

	if ( !level.initializing )
	{
		name = bg_itemlist[iItemIndex].pickup_name;

		if ( !name || !name[0] )
		{
			name = "<<unknown>>";
		}

		Scr_Error(va("game tried to register the item '%s' after initialization finished\n", name));
	}

	itemRegistered[iItemIndex] = qtrue;

	if ( bg_itemlist[iItemIndex].world_model[0] )
	{
		G_ModelIndex(bg_itemlist[iItemIndex].world_model[0]);
	}

	if ( bg_itemlist[iItemIndex].world_model[1] )
	{
		G_ModelIndex(bg_itemlist[iItemIndex].world_model[1]);
	}

	if ( bUpdateCS )
	{
		level.bRegisterItems = qtrue;
	}
}

/*
==================
G_RegisterWeapon
==================
*/
int G_RegisterWeapon( int weapIndex )
{
	WeaponDef *weapDef;
	int modelIndex;

	assert(!itemRegistered[weapIndex]);
	itemRegistered[weapIndex] = qtrue;

	level.bRegisterItems = qtrue;
	level.registerWeapons = qtrue;

	weapDef = BG_GetWeaponDef(weapIndex);

	if ( weapDef->useHintString[0] && !G_GetHintStringIndex(&weapDef->useHintStringIndex, weapDef->useHintString) )
	{
		Com_Error(ERR_DROP, "Too many different hintstring values on weapons. Max allowed is %i different strings", MAX_HINTSTRINGS);
	}

	if ( weapDef->dropHintString[0] && !G_GetHintStringIndex(&weapDef->dropHintStringIndex, weapDef->dropHintString) )
	{
		Com_Error(ERR_DROP, "Too many different hintstring values on weapons. Max allowed is %i different strings", MAX_HINTSTRINGS);
	}

	modelIndex = G_ModelIndex(weapDef->worldModel);

	if ( modelIndex && G_XModelBad(modelIndex) )
	{
		G_OverrideModel(modelIndex, "xmodel/defaultweapon");
	}

	return G_ModelIndex(weapDef->projectileModel);
}

/*
============
G_SpawnItem

Sets the clipping size and plants the object on the floor.

Items can't be immediately dropped to floor, because they might
be on an entity that hasn't spawned yet.
============
*/
void G_SpawnItem( gentity_t *ent, const gitem_t *item )
{
	RegisterItem(item - bg_itemlist, qfalse);

	ent->item.index = item - bg_itemlist;
	G_SetModel(ent, item->world_model[0]);

	if ( item->giType == IT_WEAPON )
	{
		VectorSet(ent->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, -ITEM_RADIUS);
		VectorSet(ent->r.maxs, ITEM_RADIUS, ITEM_RADIUS, ITEM_RADIUS);
	}
	else
	{
		VectorSet(ent->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, 0);
		VectorSet(ent->r.maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS);
	}

	ent->r.contents = CONTENTS_LAVA | CONTENTS_UNKNOWNCLIP | CONTENTS_TELEPORTER | CONTENTS_JUMPPAD | CONTENTS_CLUSTERPORTAL | CONTENTS_DONOTENTER_LARGE | CONTENTS_TRIGGER;

	if ( item->giType != IT_AMMO )
	{
		ent->r.contents |= CONTENTS_DONOTENTER;
	}

	ent->s.eType = ET_ITEM;
	ent->s.index = ent->item.index;

	G_DObjUpdate(ent);

	ent->s.clientNum = ENTITYNUM_WORLD;
	ent->flags |= FL_SUPPORTS_LINKTO;

	if ( level.spawnVars.spawnVarsValid )
	{
		G_SetAngle(ent, ent->r.currentAngles);
		// some movers spawn on the second frame, so delay item
		// spawns until the third frame so they can ride trains
		ent->nextthink = level.time + FRAMETIME;
		ent->handler = ENT_HANDLER_ITEM_INIT;
		return;
	}

	ent->handler = ENT_HANDLER_ITEM;

	if ( !(ent->spawnflags & 1) )
	{
		ent->s.groundEntityNum = ENTITYNUM_NONE;

		if ( item->giType == IT_WEAPON )
		{
			ent->r.currentAngles[2] = ent->r.currentAngles[2] + 90;
		}
	}

	G_SetAngle(ent, ent->r.currentAngles);
	G_SetOrigin(ent, ent->r.currentOrigin);

	SV_LinkEntity(ent);
}

/*
================
Drop_Item

Spawns an item and tosses it forward
================
*/
gentity_t* Drop_Item( gentity_t *ent, const gitem_t *item, float angle, qboolean novelocity )
{
	vec3_t vPos;
	vec3_t angles;
	vec3_t velocity;

	VectorCopy(ent->r.currentAngles, angles);
	angles[YAW] += angle;
	angles[PITCH] = 0;  // always forward
	angles[ROLL] = 0;

	if ( novelocity )
	{
		VectorClear( velocity );
	}
	else
	{
		AngleVectors( angles, velocity, NULL, NULL );
		VectorScale( velocity, g_dropForwardSpeed->current.decimal, velocity );
		velocity[2] += G_crandom() * g_dropUpSpeedRand->current.decimal + g_dropUpSpeedBase->current.decimal;
	}

	VectorCopy( ent->r.currentOrigin, vPos );
	vPos[2] += ( ent->r.maxs[2] - ent->r.mins[2] ) * 0.5;

	return LaunchItem( item, vPos, velocity, ent->s.number );
}

/*
================
G_RunItem
================
*/
void G_RunItem( gentity_t *ent )
{
	vec3_t delta;
	vec3_t endpos;
	int mask;
	int contents;
	trace_t trace;
	vec3_t origin;

	assert(ent->s.eType != ET_PLAYER_CORPSE);

	// if groundentity has been set to -1, it may have been pushed off an edge
	if ( ent->s.groundEntityNum == ENTITYNUM_NONE || level.gentities[ent->s.groundEntityNum].s.pos.trType != TR_STATIONARY )
	{
		if ( ent->s.pos.trType != TR_GRAVITY && !(ent->spawnflags & 1) )
		{
			ent->s.pos.trType = TR_GRAVITY;
			ent->s.pos.trTime = level.time;

			VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
			VectorClear(ent->s.pos.trDelta);
		}
	}

	if ( ent->s.pos.trType == TR_STATIONARY || ent->s.pos.trType == TR_GRAVITY_PAUSED || ent->tagInfo ) //----(SA)
	{
		// check think function
		G_RunThink( ent );
		return;
	}

	assert(!IS_NAN((ent->s.pos.trBase)[0]) && !IS_NAN((ent->s.pos.trBase)[1]) && !IS_NAN((ent->s.pos.trBase)[2]));
	assert(!IS_NAN((ent->s.pos.trDelta)[0]) && !IS_NAN((ent->s.pos.trDelta)[1]) && !IS_NAN((ent->s.pos.trDelta)[2]));

	// get current position
	BG_EvaluateTrajectory(&ent->s.pos, level.time + 50, origin);
	assert(!IS_NAN((origin)[0]) && !IS_NAN((origin)[1]) && !IS_NAN((origin)[2]));

	// trace a line from the previous position to the current position
	mask = G_ItemClipMask(ent);
	assert(!( ent->r.contents & mask ));

	if ( Vec3DistanceSq(ent->r.currentOrigin, origin) < 0.1 )
	{
		origin[2] -= 1.0;
	}

	G_TraceCapsule(&trace, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, ent->r.ownerNum, mask);

	if ( trace.fraction < 1.0 )
	{
		Vec3Lerp(ent->r.currentOrigin, origin, trace.fraction, endpos);

		if ( !trace.startsolid && trace.fraction < 0.0099999998 && trace.normal[2] < 0.5 )
		{
			VectorSubtract(origin, ent->r.currentOrigin, delta);
			VectorMA(origin, 1.0 - DotProduct(delta, trace.normal), trace.normal, origin);

			G_TraceCapsule(&trace, endpos, ent->r.mins, ent->r.maxs, origin, ent->r.ownerNum, mask);

			Vec3Lerp(endpos, origin, trace.fraction, endpos);
		}

		ent->s.pos.trType = TR_LINEAR_STOP;
		ent->s.pos.trTime = level.time;
		ent->s.pos.trDuration = 50;

		VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
		VectorSubtract(endpos, ent->r.currentOrigin, ent->s.pos.trDelta);

		VectorScale(ent->s.pos.trDelta, 20, ent->s.pos.trDelta);
		VectorCopy(endpos, ent->r.currentOrigin);
	}
	else
	{
		VectorCopy(origin, ent->r.currentOrigin);
	}

	SV_LinkEntity(ent); // FIXME: avoid this for stationary?

	// check think function
	G_RunThink(ent);

	if ( !ent->r.inuse )
	{
		return;
	}

	if ( trace.fraction >= 0.0099999998 )
	{
		return;
	}

	if ( trace.normal[2] <= 0 )
	{
		G_FreeEntity(ent);
		return;
	}

	// if it is in a nodrop volume, remove it
	contents = SV_PointContents(ent->r.currentOrigin, -1, CONTENTS_NODROP);

	if ( contents )
	{
		G_FreeEntity(ent);
		return;
	}

	G_OrientItemToGround(ent, &trace);
	G_SetOrigin(ent, endpos);

	ent->s.groundEntityNum = trace.entityNum;

	SV_LinkEntity(ent);
}

/*
================
G_RunCorpseMove
================
*/
void G_RunCorpseMove( gentity_t *ent )
{
#define DROP_CHECK_TRACE_START_HEIGHT 64
	bool haveDelta;
	vec3_t left;
	vec3_t right;
	vec3_t forward;
	vec3_t deltaChange;
	corpseInfo_t *corpseInfo;
	int corpseIndex;
	vec3_t start;
	vec3_t endpos;
	int mask;
	int contents;
	trace_t trace;
	vec3_t origin;

	assert(ent->s.eType == ET_PLAYER_CORPSE);

	corpseIndex = G_GetPlayerCorpseIndex(ent);
	corpseInfo = &g_scr_data.playerCorpseInfo[corpseIndex];

	G_GetAnimDeltaForCorpse(ent, corpseInfo, deltaChange);
	haveDelta = 0;

	if ( !corpseInfo->falling )
	{
		haveDelta = VectorLengthSquared(deltaChange) > 1.0;
	}

	if ( !corpseInfo->falling && !haveDelta )
	{
		return;
	}

	// get current position
	BG_EvaluateTrajectory(&ent->s.pos, level.time, origin);

	if ( haveDelta )
	{
		AngleVectors(ent->r.currentAngles, forward, right, NULL);
		VectorScale(right, -1.0, left);

		Vec3Normalize(forward);
		Vec3Normalize(left);

		VectorMA(origin, deltaChange[0], forward, origin);
		VectorMA(origin, deltaChange[1], left, origin);
	}

	// trace a line from the previous position to the current position
	assert(ent->clipmask);
	mask = ent->clipmask;
	assert(!( ent->r.contents & mask ));
	G_TraceCapsule(&trace, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, ent->r.ownerNum, mask);

	Vec3Lerp(ent->r.currentOrigin, origin, trace.fraction, endpos);
	VectorCopy(endpos, ent->r.currentOrigin);

	if ( trace.startsolid )
	{
		trace.fraction = 0;
	}

	SV_LinkEntity(ent); // FIXME: avoid this for stationary?

	// check think function
	G_RunThink(ent);

	if ( !ent->r.inuse )
	{
		return;
	}

	if ( trace.fraction == 1 )
	{
		if ( corpseInfo->falling || !haveDelta )
		{
			return;
		}

		ent->s.pos.trType = TR_INTERPOLATE;
		VectorCopy(endpos, ent->s.pos.trBase);

		ent->s.pos.trTime = 0;
		ent->s.pos.trDuration = 0;

		VectorClear(ent->s.pos.trDelta);
		origin[2] -= 1.0;

		if ( !G_TraceCapsuleComplete(ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, ent->r.ownerNum, mask) )
		{
			return;
		}

		corpseInfo->falling = qtrue;
		ent->s.pos.trType = TR_GRAVITY;

		VectorCopy(endpos, ent->s.pos.trBase);
		VectorClear(ent->s.pos.trDelta);

		VectorMA(ent->s.pos.trDelta, deltaChange[0], forward, ent->s.pos.trDelta);
		VectorMA(ent->s.pos.trDelta, deltaChange[1], left, ent->s.pos.trDelta);

		ent->s.pos.trTime = level.time;
		ent->s.pos.trDuration = 0;

		return;
	}

	// if it is in a nodrop volume, remove it
	contents = SV_PointContents(ent->r.currentOrigin, -1, CONTENTS_NODROP);

	if ( contents )
	{
		G_FreeEntity(ent);
		return;
	}

	if ( !corpseInfo->falling )
	{
		return;
	}

	if ( trace.allsolid )
	{
		VectorCopy(ent->r.currentOrigin, start);
		start[2] += DROP_CHECK_TRACE_START_HEIGHT;

		G_TraceCapsule(&trace, start, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, ent->r.ownerNum, mask & ~( CONTENTS_PLAYERCLIP ));

		if ( !trace.allsolid )
		{
			Vec3Lerp(start, ent->r.currentOrigin, trace.fraction, endpos);
			VectorCopy(endpos, ent->r.currentOrigin);
		}
	}

	G_BounceCorpse(ent, corpseInfo, &trace, endpos);
}

/*
================
FinishSpawningItem

Traces down to find where an item should rest, instead of letting them
free fall from their spawn points
================
*/
void FinishSpawningItem( gentity_t *ent )
{
	vec3_t vAxis[3];
	vec3_t vAngles;
	int mask;
	vec3_t endpos;
	vec3_t start;
	gitem_t *item;
	vec3_t maxs;
	vec3_t mins;
	vec3_t end;
	trace_t tr;

	ent->handler = ENT_HANDLER_ITEM;

	if ( ent->spawnflags & 1 )  // suspended
	{
		G_SetOrigin(ent, ent->r.currentOrigin);
	}
	else
	{
		item = &bg_itemlist[ent->item.index];

		if ( item->giType == IT_WEAPON )
		{
			VectorSet(mins, -ITEM_RADIUS, -ITEM_RADIUS, -ITEM_RADIUS);
			VectorSet(maxs, ITEM_RADIUS, ITEM_RADIUS, ITEM_RADIUS);
		}
		else
		{
			// Rafael
			// had to modify this so that items would spawn in shelves
			VectorSet(mins, -ITEM_RADIUS, -ITEM_RADIUS, 0);
			VectorSet(maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS);
		}

		mask = G_ItemClipMask(ent);

		VectorCopy(ent->r.currentOrigin, start);
		VectorSet(end, ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2] - 4096);

		G_TraceCapsule(&tr, start, mins, maxs, end, ent->s.number, mask);

		if ( tr.startsolid )
		{
			VectorCopy(ent->r.currentOrigin, start);
			start[2] -= 15;
			VectorSet(end, ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2] - 4096);

			G_TraceCapsule(&tr, start, mins, maxs, end, ent->s.number, mask);
		}

		if ( tr.startsolid )
		{
			Com_Printf("FinishSpawningItem: %s startsolid at %s\n", SL_ConvertToString(ent->classname), vtos(ent->r.currentOrigin));
			G_FreeEntity(ent);
			return;
		}

		ent->s.groundEntityNum = tr.entityNum;
		Vec3Lerp(start, end, tr.fraction, endpos);

		G_SetOrigin(ent, endpos);

		if ( tr.fraction < 1.0 )
		{
			VectorCopy(tr.normal, vAxis[2]);
			AngleVectors(ent->r.currentAngles, vAxis[0], NULL, NULL);

			Vec3Cross(vAxis[2], vAxis[0], vAxis[1]);
			Vec3Cross(vAxis[1], vAxis[2], vAxis[0]);

			AxisToAngles(vAxis, vAngles);

			if ( bg_itemlist[ent->s.index].giType == IT_WEAPON )
			{
				vAngles[2] += 90;
			}

			//----(SA) moved this up so it happens for suspended items too (and made it a function)
			G_SetAngle(ent, vAngles);
		}
	}

	SV_LinkEntity( ent );
}

/*
==============
Drop_Weapon
==============
*/
gentity_t *Drop_Weapon( gentity_t *ent, int iWeaponIndex, unsigned int tag )
{
	int mask;
	trace_t trace;
	vec3_t vCenter;
	vec3_t vAngles;
	float tagMat[4][3];
	vec3_t velocity;
	int iMin;
	int iMax;
	gitem_t *weapItem;
	gentity_t *itemEnt;
	int clipIndex;
	int ammoIndex;
	int iDropClip;
	int iDropAmmo;

	weapItem = &bg_itemlist[iWeaponIndex];

	if ( ent->client && !Com_BitCheck(ent->client->ps.weapons, iWeaponIndex) )
	{
		BG_TakePlayerWeapon(&ent->client->ps, iWeaponIndex);
		return NULL;
	}

	ammoIndex = BG_AmmoForWeapon(iWeaponIndex);
	clipIndex = BG_ClipForWeapon(iWeaponIndex);

	if ( BG_GetWeaponDef(iWeaponIndex)->clipOnly && !ent->client->ps.ammoclip[clipIndex] )
	{
		BG_TakePlayerWeapon(&ent->client->ps, iWeaponIndex);
		return NULL;
	}

	itemEnt = Drop_Item(ent, weapItem, 0, qfalse);

	if ( ent->client )
	{
		iDropAmmo = ent->client->ps.ammo[ammoIndex];
		ent->client->ps.ammo[ammoIndex] = 0;

		iDropClip = ent->client->ps.ammoclip[clipIndex];
		ent->client->ps.ammoclip[clipIndex] = 0;

		BG_TakePlayerWeapon(&ent->client->ps, iWeaponIndex);
	}
	else
	{
		iMax = BG_GetWeaponDef(iWeaponIndex)->dropAmmoMax;
		iMin = BG_GetWeaponDef(iWeaponIndex)->dropAmmoMin;

		if ( iMax < iMin )
		{
			iMax = iMin;
			iMin = iMax;
		}

		if ( iMax || iMin )
		{
			if ( iMax < 0 )
			{
				iDropAmmo = 0;
				iDropClip = 0;
			}
			else
			{
				if ( iMax == iMin )
					iDropAmmo = iMin;
				else
					iDropAmmo = rand() % (iMax - iMin) + iMin;

				if ( iDropAmmo <= 0 )
				{
					iDropAmmo = 0;
					iDropClip = 0;
				}
				else
				{
					if ( BG_GetAmmoClipSize(clipIndex) )
						iDropClip = rand() % BG_GetAmmoClipSize(clipIndex);
					else
						iDropClip = 0;

					if ( iDropClip < iDropAmmo )
					{
						iDropAmmo -= iDropClip;
					}
					else
					{
						iDropClip = iDropAmmo;
						iDropAmmo = 0;
					}
				}
			}
		}
		else
		{
			iDropAmmo = Q_rint(BG_GetAmmoClipSize(clipIndex) - 1 * ((G_random() + 1.0) * 0.5)) + 1;
			iDropClip = Q_rint((G_random() * 0.5 + 0.25) * iDropAmmo);
			iDropAmmo -= iDropClip;
		}
	}

	itemEnt->count = iDropAmmo;
	itemEnt->item.clipAmmoCount = iDropClip;

	if ( !itemEnt->count )
	{
		itemEnt->count = -1;
	}

	if ( !itemEnt->item.clipAmmoCount )
	{
		itemEnt->item.clipAmmoCount = -1;
	}

	if ( !iDropAmmo && !iDropClip )
	{
		itemEnt->r.contents &= ~CONTENTS_DONOTENTER;
	}

	if ( tag )
	{
		if ( G_DObjGetWorldTagMatrix(ent, tag, tagMat) )
		{
			Vec3Avg(ent->r.mins, ent->r.maxs, vCenter);
			VectorAdd(vCenter, ent->r.currentOrigin, vCenter);

			mask = G_ItemClipMask(itemEnt);

			G_TraceCapsule(&trace, vCenter, itemEnt->r.mins, itemEnt->r.maxs, tagMat[3], ent->s.number, mask);

			Vec3Lerp(vCenter, tagMat[3], trace.fraction, itemEnt->s.pos.trBase);
			VectorCopy(itemEnt->s.pos.trBase, itemEnt->r.currentOrigin);

			itemEnt->s.pos.trTime = level.time;

			AxisToAngles(tagMat, vAngles);
		}
		else
		{
			VectorCopy(ent->r.currentAngles, vAngles);
		}

		vAngles[2] += 90;
		G_SetAngle(itemEnt, ent->r.currentAngles);

		VectorSet(velocity, G_crandom() * 50, G_crandom() * 40, G_crandom() * 60);

		itemEnt->s.apos.trType = TR_LINEAR;
		itemEnt->s.apos.trTime = level.time;

		VectorCopy(velocity, itemEnt->s.apos.trDelta);
	}

	return itemEnt;
}

/*
==============
G_RunCorpse
==============
*/
void G_RunCorpse( gentity_t *ent )
{
	G_RunCorpseMove(ent);
	G_RunCorpseAnimate(ent);
	G_RunThink(ent);
}

/*
===============
Touch_Item
===============
*/
void Touch_Item( gentity_t *ent, gentity_t *other, qboolean touched )
{
	WeaponDef *weapDef;
	gitem_t *item;
	char cleanname[64];
	int respawn;
	int makenoise = EV_ITEM_PICKUP;

	// only activated items can be picked up
	if ( !ent->active )
	{
		return;
	}
	else
	{
		// need to set active to false if player is maxed out
		ent->active = qfalse;
	}

	if ( !other->client )
	{
		return;
	}

	if ( other->health <= 0 )
	{
		return;     // dead people can't pickup
	}

	if ( level.clientIsSpawning )
	{
		return;
	}

	item = &bg_itemlist[ent->item.index];

	// the same pickup rules are used for client side and server side
	if ( !BG_CanItemBeGrabbed(&ent->s, &other->client->ps, touched) )
	{
		if ( !touched && ent->s.clientNum != other->s.number && item->giType == IT_WEAPON )
		{
			if ( Com_BitCheck(other->client->ps.weapons, item->giTag) )
			{
				weapDef = BG_GetWeaponDef(item->giTag);
				SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_PICKUP_CANTCARRYMOREAMMO\x14%s\"", 102, weapDef->displayName));
			}
			else if ( BG_GetWeaponDef(item->giTag)->weaponSlot - 1 <= 1 )
			{
				SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_CANT_GET_PRIMARY_WEAP_MESSAGE\"", 102));
			}
		}

		return;
	}

	I_strncpyz(cleanname, other->client->sess.state.name, sizeof(cleanname));
	I_CleanStr(cleanname);

	if ( item->giType == IT_WEAPON )
	{
		weapDef = BG_GetWeaponDef(item->giTag);
		G_LogPrintf("Weapon;%d;%d;%s;%s\n", SV_GetGuid(other->s.number), other->s.number, cleanname, weapDef->szInternalName);
	}
	else
	{
		G_LogPrintf("Item;%d;%d;%s;%s\n", SV_GetGuid(other->s.number), other->s.number, cleanname, item->classname);
	}

	// call the item-specific pickup function
	switch ( item->giType )
	{
	case IT_WEAPON:
		respawn = Pickup_Weapon( ent, other, &makenoise, touched );
		break;
	case IT_HEALTH:
		respawn = Pickup_Health( ent, other );
		break;
	case IT_AMMO:
		respawn = Pickup_Ammo( ent, other );
		break;
	default:
		return;
	}

	if ( !respawn )
	{
		return;
	}

	// play sounds
	if ( other->client->sess.predictItemPickup )
	{
		G_AddPredictableEvent(other, makenoise, ent->s.index);
	}
	else
	{
		G_AddEvent(other, makenoise, ent->s.index);
	}

	G_FreeEntity(ent);
}

/*
==============
Touch_Item_Auto
	if other->client->pers.autoActivate == PICKUP_ACTIVATE	(0), he will pick up items only when using +activate
	if other->client->pers.autoActivate == PICKUP_TOUCH		(1), he will pickup items when touched
	if other->client->pers.autoActivate == PICKUP_FORCE		(2), he will pickup the next item when touched (and reset to PICKUP_ACTIVATE when done)
==============
*/
void Touch_Item_Auto( gentity_t *ent, gentity_t *other, qboolean bTouched )
{
#if LIBCOD_COMPILE_PLAYER == 1
	extern int player_disableitempickup[MAX_CLIENTS];
	if (player_disableitempickup[other->s.number])
		return;
#endif
	ent->active = qtrue;
	Touch_Item(ent, other, bTouched);
}

/*
===============
G_ItemClipMask
===============
*/
int G_ItemClipMask( gentity_t *ent )
{
	assert(ent);

	if ( ent->clipmask )
	{
		return ent->clipmask;
	}

	return CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_MISSILECLIP | CONTENTS_ITEMCLIP;
}

/*
===============
G_OrientItemToGround
===============
*/
void G_OrientItemToGround( gentity_t *ent, trace_t *trace )
{
	vec3_t vAngles;
	vec3_t vAxis[3];

	VectorCopy(trace->normal, vAxis[2]);
	AngleVectors(ent->r.currentAngles, vAxis[0], NULL, NULL);

	Vec3Cross(vAxis[2], vAxis[0], vAxis[1]);
	Vec3Cross(vAxis[1], vAxis[2], vAxis[0]);

	AxisToAngles(vAxis, vAngles);

	if ( bg_itemlist[ent->s.index].giType == IT_WEAPON )
	{
		vAngles[2] += 90;
	}

	G_SetAngle(ent, vAngles);
}

/*
===============
G_GetAnimDeltaForCorpse
===============
*/
void G_GetAnimDeltaForCorpse( gentity_t *ent, corpseInfo_t *ci, vec3_t originChange )
{
	vec3_t rot;

	XAnimCalcDelta(ci->tree, 0, rot, originChange, true);
}

/*
===============
G_BounceCorpse
===============
*/
void G_BounceCorpse( gentity_t *ent, corpseInfo_t *corpseInfo, trace_t *trace, vec3_t endpos )
{
	vec3_t vAngles;
	vec3_t vAxis[3];

	VectorClear(ent->s.pos.trDelta);

	if ( !trace->allsolid && trace->normal[2] <= 0 )
	{
		assert(ent->s.pos.trType == TR_GRAVITY);
		VectorAdd(ent->r.currentOrigin, trace->normal, ent->r.currentOrigin);
		VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
		ent->s.pos.trTime = level.time;
		return;
	}

	corpseInfo->falling = qfalse;

	ent->s.pos.trType = TR_INTERPOLATE;
	VectorCopy(endpos, ent->s.pos.trBase);

	ent->s.pos.trTime = 0;
	ent->s.pos.trDuration = 0;

	VectorClear(ent->s.pos.trDelta);
	ent->s.groundEntityNum = trace->entityNum;

	if ( trace->allsolid )
	{
		G_SetAngle(ent, ent->r.currentAngles);
	}
	else
	{
		VectorCopy(trace->normal, vAxis[2]);
		AngleVectors(ent->r.currentAngles, vAxis[0], NULL, NULL);
		Vec3Cross(vAxis[2], vAxis[0], vAxis[1]);
		Vec3Cross(vAxis[1], vAxis[2], vAxis[0]);
		AxisToAngles(vAxis, vAngles);
		G_SetAngle(ent, vAngles);
	}

	SV_LinkEntity(ent);
}

/*
===============
RegisterWeaponRumbles
===============
*/
void RegisterWeaponRumbles( WeaponDef *weapDef )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
===============
GetFreeCueSpot
===============
*/
int GetFreeCueSpot()
{
	int maxDroppedWeapon;
	gentity_t *ent;
	float fBestDistSqrd = -1.0;
	float fClientDistSqrd;
	float fDistSqrd;
	int iBest = 0;
	int i, j;

	maxDroppedWeapon = g_maxDroppedWeapons->current.integer;
	assert(maxDroppedWeapon >= 1 && maxDroppedWeapon <= 32);

	for ( i = 0; i < maxDroppedWeapon; i++ )
	{
		ent = level.droppedWeaponCue[i];

		if ( !ent )
		{
			return i;
		}

		fDistSqrd = 9.9999803e11;

		for ( j = 0; j < level.maxclients; j++ )
		{
			if ( level.clients[j].sess.connected != CON_CONNECTED )
			{
				continue;
			}

			if ( level.clients[j].sess.sessionState != SESS_STATE_PLAYING )
			{
				continue;
			}

			fClientDistSqrd = Vec3DistanceSq(g_entities[j].r.currentOrigin, ent->r.currentOrigin);

			if ( fDistSqrd > fClientDistSqrd )
			{
				fDistSqrd = fClientDistSqrd;
			}
		}

		if ( fDistSqrd > fBestDistSqrd )
		{
			fBestDistSqrd = fDistSqrd;
			iBest = i;
		}
	}

	ent = level.droppedWeaponCue[iBest];
	assert(ent);

	G_FreeEntity(ent);
	level.droppedWeaponCue[iBest] = NULL;

	return iBest;
}

/*
===============
G_RunCorpseAnimate
===============
*/
void G_RunCorpseAnimate( gentity_t *ent )
{
	int corpseIndex;
	corpseInfo_t *corpseInfo;
	DObj *obj;

	corpseIndex = G_GetPlayerCorpseIndex(ent);
	assert(corpseIndex >= 0);
	assert(corpseIndex < MAX_CLIENT_CORPSES);

	corpseInfo = &g_scr_data.playerCorpseInfo[corpseIndex];
	obj = Com_GetServerDObj(ent->s.number);

	BG_UpdatePlayerDObj(obj, &ent->s, &corpseInfo->ci, qfalse);
	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
	{
		BG_PlayerAnimation(obj, &ent->s, &corpseInfo->ci);
	}
}

/*
================
LaunchItem

Spawns an item and tosses it forward
================
*/
gentity_t* LaunchItem( const gitem_t *item, vec3_t origin, vec3_t angles, int ownerNum )
{
	gentity_t *dropped;
	int itemIndex;
	int dropIdx;

	assert(item);
	assert(!IS_NAN((origin)[0]) && !IS_NAN((origin)[1]) && !IS_NAN((origin)[2]));

	itemIndex = item - bg_itemlist;
	RegisterItem(itemIndex, qtrue);

	dropped = G_Spawn();

	dropIdx = GetFreeCueSpot();
	level.droppedWeaponCue[dropIdx] = dropped;

	dropped->s.eType = ET_ITEM;
	dropped->s.index = itemIndex;

	G_GetItemClassname(item, &dropped->classname);
	dropped->item.index = itemIndex;

	if ( item->giType == IT_WEAPON )
	{
		VectorSet( dropped->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, -ITEM_RADIUS );            //----(SA)	so items sit on the ground
		VectorSet( dropped->r.maxs, ITEM_RADIUS, ITEM_RADIUS, ITEM_RADIUS );  //----(SA)	so items sit on the ground
	}
	else
	{
		VectorSet( dropped->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );            //----(SA)	so items sit on the ground
		VectorSet( dropped->r.maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );  //----(SA)	so items sit on the ground
	}

	dropped->r.contents = CONTENTS_LAVA | CONTENTS_UNKNOWNCLIP | CONTENTS_TELEPORTER |
	                      CONTENTS_JUMPPAD | CONTENTS_CLUSTERPORTAL | CONTENTS_DONOTENTER_LARGE |
	                      CONTENTS_TRIGGER; // NERVE - SMF - fix for items falling through grates

	if ( item->giType != IT_AMMO )
	{
		dropped->r.contents |= CONTENTS_DONOTENTER;
	}

	dropped->s.clientNum = ownerNum;

	G_SetModel(dropped, item->world_model[0]);
	G_DObjUpdate(dropped);

	dropped->handler = ENT_HANDLER_DROPPED_ITEM;

	G_SetOrigin(dropped, origin);

	dropped->s.pos.trType = TR_GRAVITY;
	dropped->s.pos.trTime = level.time;

	VectorCopy(angles, dropped->s.pos.trDelta);

	dropped->nextthink = level.time + 1000;
	dropped->flags = FL_DROPPED_ITEM;

	SV_LinkEntity(dropped);

	return dropped;
}

/*
================
Pickup_Health
================
*/
qboolean Pickup_Health( gentity_t *ent, gentity_t *other )
{
	gitem_t *item;
	int max;
	int quantity;
	int oldHealth;

	item = &bg_itemlist[ent->item.index];
	assert(item);

	// small and mega healths will go over the max
	if ( item->quantity != 5 && item->quantity != 100 )
	{
		max = other->client->ps.stats[STAT_MAX_HEALTH];
	}
	else
	{
		max = other->client->ps.stats[STAT_MAX_HEALTH] * 2;
	}

	if ( ent->count )
	{
		quantity = ent->count;
	}
	else
	{
		quantity = item->quantity;
	}

	oldHealth = other->health;
	other->health += quantity * other->client->ps.stats[STAT_MAX_HEALTH] * 0.0099999998;

	if ( other->health > max )
	{
		other->health = max;
	}
	else
	{
		int current;
		int old;
		int max;

		current = 100 * other->health / other->client->ps.stats[STAT_MAX_HEALTH];

		if ( current <= 0 )
		{
			current = 1;
		}
		else if ( current > 100 )
		{
			current = 100;
		}

		old = 100 * oldHealth / other->client->ps.stats[STAT_MAX_HEALTH];

		if ( old <= 0 )
		{
			old = 1;
		}

		max = quantity + old;

		if ( max > 100 )
		{
			max = 100;
		}

		if ( current != max )
		{
			other->health = other->client->ps.stats[STAT_MAX_HEALTH] * max / 100;
		}
	}

	other->client->ps.stats[STAT_HEALTH] = other->health;

	SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_PICKUP_HEALTH\x15%i\"", 102, quantity));
	SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"%i\"", 73, 0));

	Scr_AddEntity(other);
	Scr_Notify(ent, scr_const.trigger, 1);

	return qtrue;
}

/*
==============
Pickup_Ammo
==============
*/
qboolean Pickup_Ammo( gentity_t *ent, gentity_t *other )
{
	gitem_t *item;
	int quantity;
	WeaponDef *weapDef;

	item = &bg_itemlist[ent->item.index];
	assert(item);

	// added some ammo pickups, so I'll use ent->item->quantity if no ent->count
	if ( ent->count )
	{
		quantity = ent->count;
	}
	else
	{
		quantity = item->quantity;
	}

	if ( !Add_Ammo(other, item->giTag, quantity, qfalse) )
	{
		return qfalse;
	}

	if ( BG_WeaponIsClipOnly(item->giTag) )
	{
		weapDef = BG_GetWeaponDef(item->giTag);
		SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_PICKUP_CLIPONLY_AMMO\x14%s\"", 102, weapDef->displayName));
	}
	else
	{
		weapDef = BG_GetWeaponDef(item->giTag);
		SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_PICKUP_AMMO\x14%s\"", 102, weapDef->displayName));
	}

	Scr_AddEntity(other);
	Scr_Notify(ent, scr_const.trigger, 1);

	if ( BG_GetWeaponDef(item->giTag)->offhandClass )
	{
		SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"%i\"", 73, 4));
	}
	else
	{
		SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"%i\"", 73, 1));
	}

	return qtrue;
}

/*
==============
Pickup_Weapon
==============
*/
qboolean Pickup_Weapon( gentity_t *ent, gentity_t *other, int *pickupEvent, qboolean touched )
{
	int quantity;
	int clipAmmo;
	int ammoAvailable;
	int ammoTaken;
	bool alreadyHave;

	gentity_t *droppedEnt = NULL;
	int weapIdx = bg_itemlist[ent->item.index].giTag;
	WeaponDef *weapDef = BG_GetWeaponDef(weapIdx);

	if ( ent->count < 0 )
	{
		quantity = 0;
	}
	else
	{
		if ( !ent->count )
		{
			int max = weapDef->dropAmmoMax;
			int min = weapDef->dropAmmoMin;

			if ( max < min )
			{
				max = weapDef->dropAmmoMin;
				min = weapDef->dropAmmoMax;
			}

			if ( max || min )
			{
				if ( max < 0 )
				{
					ent->count = 0;
				}
				else
				{
					if ( max == min )
						ent->count = min;
					else
						ent->count = rand() % (max - min) + min;

					if ( ent->count <= 0 )
					{
						ent->count = 0;
					}
				}
			}
			else
			{
				ent->count = Q_rint(BG_GetAmmoClipSize(BG_ClipForWeapon(weapIdx)) - 1 * (G_random() + 1.0) * 0.5) + 1;
			}
		}

		if ( ent->count > BG_GetAmmoTypeMax(BG_AmmoForWeapon(weapIdx)) )
		{
			ent->count = BG_GetAmmoTypeMax(BG_AmmoForWeapon(weapIdx));
		}

		quantity = ent->count;
	}

	if ( ent->item.clipAmmoCount < 0 )
	{
		clipAmmo = 0;
	}
	else
	{
		if ( !ent->item.clipAmmoCount )
		{
			if ( ent->count < 0 )
			{
				ent->item.clipAmmoCount = 0;
			}
			else
			{
				ent->item.clipAmmoCount = BG_GetAmmoClipSize(BG_ClipForWeapon(weapIdx));

				if ( ent->item.clipAmmoCount > ent->count )
				{
					ent->item.clipAmmoCount = ent->count;
				}

				ent->count -= ent->item.clipAmmoCount;
				quantity = ent->count;
			}
		}

		if ( ent->item.clipAmmoCount > BG_GetAmmoClipSize(BG_ClipForWeapon(weapIdx)) )
		{
			ent->item.clipAmmoCount = BG_GetAmmoClipSize(BG_ClipForWeapon(weapIdx));
		}

		clipAmmo = ent->item.clipAmmoCount;
	}

	// check if player already had the weapon
	alreadyHave = Com_BitCheck(other->client->ps.weapons, weapIdx);

	if ( !alreadyHave )
	{
		if ( BG_DoesWeaponNeedSlot(weapIdx) )
		{
			if ( other->client->ps.weapon )
			{
				if ( !Com_BitCheck(other->client->ps.weapons, other->client->ps.weapon) )
				{
					return qfalse;
				}

				if ( !BG_IsPlayerWeaponInSlot(&other->client->ps, other->client->ps.weapon, qtrue) )
				{
					if ( BG_GetStackSlotForWeapon(&other->client->ps, other->client->ps.weapon, BG_GetWeaponDef(other->client->ps.weapon)->weaponSlot) == SLOT_NONE && !BG_GetEmptySlotForWeapon(&other->client->ps, weapIdx) )
					{
						Com_Printf("WARNING: cannot swap out a debug weapon (can result from too many weapons given to the player)\n");
						return qfalse;
					}
				}
			}

			if ( !BG_GetEmptySlotForWeapon(&other->client->ps, weapIdx) )
			{
				if ( BG_GetStackSlotForWeapon(&other->client->ps, weapIdx, BG_GetWeaponDef(other->client->ps.weapon)->weaponSlot) == SLOT_NONE )
				{
					if ( weapDef->weaponSlot == BG_GetWeaponDef(other->client->ps.weapon)->weaponSlot )
					{
						droppedEnt = Drop_Weapon(other, other->client->ps.weapon, 0);
					}
					else
					{
						int slot;

						for ( slot = SLOT_PRIMARY; slot < SLOT_COUNT; slot++ )
						{
							if ( !other->client->ps.ammo[BG_AmmoForWeapon(weapIdx)] && !other->client->ps.ammoclip[BG_ClipForWeapon(weapIdx)] )
							{
								droppedEnt = Drop_Weapon(other, other->client->ps.weaponslots[slot], 0);
								break;
							}
						}

						if ( slot > SLOT_PRIMARYB )
						{
							SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_CANT_GET_PRIMARY_WEAP_MESSAGE\"", 102));
							return qfalse;
						}
					}

					if ( !droppedEnt )
					{
						return qfalse;
					}

					droppedEnt->spawnflags = ent->spawnflags;
					droppedEnt->s.groundEntityNum = ent->s.groundEntityNum;

					G_SetOrigin(droppedEnt, ent->r.currentOrigin);
					G_SetAngle(droppedEnt, ent->r.currentAngles);

					SV_LinkEntity(droppedEnt);
				}
			}

			G_GivePlayerWeapon(&other->client->ps, weapIdx);

			if ( !touched )
			{
				G_SelectWeaponIndex(other - g_entities, weapIdx);
			}
		}
		else
		{
			G_GivePlayerWeapon(&other->client->ps, weapIdx);
		}
	}

	if ( alreadyHave )
	{
		*pickupEvent = EV_AMMO_PICKUP;

		ammoAvailable = clipAmmo + quantity;
		ammoTaken = Add_Ammo(other, weapIdx, ammoAvailable, qfalse);

		if ( ammoTaken )
		{
			if ( BG_WeaponIsClipOnly(weapIdx) )
			{
				SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_PICKUP_CLIPONLY_AMMO\x14%s\"", 102, weapDef->displayName));
			}
			else
			{
				SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_PICKUP_AMMO\x14%s\"", 102, weapDef->displayName));
			}
		}

		if ( ammoTaken != ammoAvailable )
		{
			ent->count -= ammoTaken;

			if ( ent->count <= 0 )
			{
				ent->item.clipAmmoCount += ent->count;
				ent->count = -1;

				if ( ent->item.clipAmmoCount <= 0 )
				{
					ent->item.clipAmmoCount = -1;
				}
			}

			if ( (ent->count > 0 || ent->item.clipAmmoCount > 0) && g_weaponAmmoPools->current.boolean )
			{
				return qfalse;
			}
		}
	}
	else
	{
		if ( clipAmmo >= 0 )
		{
			if ( clipAmmo > BG_GetAmmoClipSize(BG_ClipForWeapon(weapIdx)) )
			{
				quantity += clipAmmo - BG_GetAmmoClipSize(BG_ClipForWeapon(weapIdx));
				clipAmmo = BG_GetAmmoClipSize(BG_ClipForWeapon(weapIdx));
			}

			other->client->ps.ammoclip[BG_ClipForWeapon(weapIdx)] = clipAmmo;
		}

		Add_Ammo(other, weapIdx, quantity, clipAmmo == -1);
	}

	if ( droppedEnt )
		Scr_AddEntity(droppedEnt);
	else
		Scr_AddUndefined();

	Scr_AddEntity(other);
	Scr_Notify(ent, scr_const.trigger, 2);

	if ( weapDef->offhandClass )
	{
		SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"%i\"", 73, 4));
	}
	else
	{
		SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"%i\"", 73, 1));
	}

	return qtrue;
}
