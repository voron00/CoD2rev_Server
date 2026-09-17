#include "../qcommon/qcommon.h"
#include "g_shared.h"

/*
===============
Player_UpdateActivate
===============
*/
void Player_UpdateActivate( gentity_t *ent )
{
	assert(ent);
	assert(ent->client);

	ent->client->ps.pm_flags &= ~PMF_RELOAD;

	// Using binoculars
	if ( ent->client->ps.weaponstate >= WEAPON_BINOCULARS_INIT && ent->client->ps.weaponstate <= WEAPON_BINOCULARS_END )
	{
		return;
	}

	bool useSucceeded = false;

	if ( ent->client->useHoldEntity != ENTITYNUM_NONE && ent->client->oldbuttons & BUTTON_USERELOAD && ent->client->buttons & BUTTON_USERELOAD )
	{
		ent->client->ps.pm_flags |= PMF_RELOAD;
		return;
	}

	if ( ent->client->latched_buttons & ( BUTTON_USE | BUTTON_USERELOAD ) )
	{
		useSucceeded = Player_ActivateCmd(ent);
	}

	if ( ent->client->useHoldEntity != ENTITYNUM_NONE || useSucceeded )
	{
		if ( ent->client->buttons & ( BUTTON_USE | BUTTON_USERELOAD ) )
		{
			Player_ActivateHoldCmd(ent);
		}
	}
	else if ( ent->client->latched_buttons & BUTTON_USERELOAD )
	{
		ent->client->ps.pm_flags |= PMF_RELOAD;
	}
}

/*
===============
Player_UpdateLookAtEntity
===============
*/
void Player_UpdateLookAtEntity( gentity_t *ent )
{
	vec3_t vDelta, vForward, vEnd, vEyePosition;
	trace_t trace;
	unsigned char *priorityMap;
	gentity_t *lookAtEnt;
	WeaponDef *weapDef;
	playerState_t *ps;

	assert(ent);
	assert(ent->client);
	ps = &ent->client->ps;
	assert(ps);

	ps->pm_flags &= ~( PMF_LOOKAT_FRIEND | PMF_LOOKAT_ENEMY );
	ent->client->pLookatEnt = NULL;

	G_GetPlayerViewOrigin(ent, vEyePosition);
	G_GetPlayerViewDirection(ent, vForward, NULL, NULL);

	if ( ps->eFlags & EF_TURRET_ACTIVE )
		weapDef = BG_GetWeaponDef(g_entities[ps->viewlocked_entNum].s.weapon);
	else
		weapDef = BG_GetWeaponDef(ent->client->ps.weapon);

	assert(weapDef);

	if ( ent->client->ps.weapon && weapDef->rifleBullet )
		priorityMap = riflePriorityMap;
	else
		priorityMap = bulletPriorityMap;

	VectorMA(vEyePosition, 15000, vForward, vEnd);

	lookAtEnt = Player_UpdateLookAtEntityTrace( &trace, vEyePosition, vEnd, ent->s.number,
	            CONTENTS_SOLID | CONTENTS_SKY | CONTENTS_CLIPSHOT | CONTENTS_UNKNOWN | CONTENTS_BODY | CONTENTS_TRANSLUCENT,
	            priorityMap, vForward );

	if ( !lookAtEnt )
	{
		return;
	}

	if ( lookAtEnt->classname == scr_const.trigger_lookat )
	{
		ent->client->pLookatEnt = lookAtEnt;
		G_Trigger(lookAtEnt, ent);

		lookAtEnt = Player_UpdateLookAtEntityTrace( &trace, vEyePosition, vEnd, ent->s.number,
		            CONTENTS_SOLID | CONTENTS_SKY | CONTENTS_CLIPSHOT | CONTENTS_UNKNOWN | CONTENTS_BODY,
		            priorityMap, vForward );

		if ( !lookAtEnt )
		{
			return;
		}
	}

	if ( lookAtEnt->s.eType != ET_PLAYER )
	{
		return;
	}

	if ( trace.surfaceFlags & SURF_NOIMPACT )
	{
		return;
	}

	VectorSubtract(lookAtEnt->r.currentOrigin, vEyePosition, vDelta);

	if ( OnSameTeam(lookAtEnt, ent) )
	{
		if ( Square(g_friendlyNameDist->current.decimal) > VectorLengthSquared(vDelta) )
		{
			if ( !ent->client->pLookatEnt )
			{
				ent->client->pLookatEnt = lookAtEnt;
			}
		}

		if ( Square(g_friendlyfireDist->current.decimal) > VectorLengthSquared(vDelta) )
		{
			// looking at friend
			ps->pm_flags |= PMF_LOOKAT_FRIEND;
		}
	}
	else
	{
		if ( Square(weapDef->enemyCrosshairRange) > VectorLengthSquared(vDelta) )
		{
			if ( !ent->client->pLookatEnt )
			{
				ent->client->pLookatEnt = lookAtEnt;
			}

			// looking at enemy
			ps->pm_flags |= PMF_LOOKAT_ENEMY;
		}
	}
}

/*
===============
Player_UpdateCursorHints
===============
*/
void Player_UpdateCursorHints( gentity_t *ent )
{
	useList_t useList[MAX_GENTITIES];
	gentity_t *traceEnt;
	playerState_t *ps;
	WeaponDef *weapDef;

	assert(ent);
	assert(ent->client);
	ps = &ent->client->ps;
	assert(ps);

	ps->cursorHint = 0;
	ps->cursorHintString = -1;
	ps->cursorHintEntIndex = ENTITYNUM_NONE;

	// dead
	if ( ent->health <= 0 )
	{
		return;
	}

	// Using binoculars
	if ( ent->client->ps.weaponstate >= WEAPON_BINOCULARS_INIT && ent->client->ps.weaponstate <= WEAPON_BINOCULARS_END )
	{
		return;
	}

	// using turret
	if ( ent->active && ps->eFlags & EF_TURRET_ACTIVE )
	{
		Player_SetTurretDropHint(ent);
		return;
	}

	// mantling
	if ( ent->client->ps.pm_flags & PMF_MANTLE )
	{
		return;
	}

	int num = Player_GetUseList(ent, useList);

	if ( !num )
	{
		return;
	}

	int hintString = -1;

	for ( int i = 0; i < num; i++ )
	{
		traceEnt = useList[i].ent;
		assert(traceEnt);
		assert(traceEnt->r.inuse);

		if ( traceEnt->s.eType == ET_GENERAL )
		{
			if ( traceEnt->classname == scr_const.trigger_use || traceEnt->classname == scr_const.trigger_use_touch )
			{
				// Team for trigger do not match
				if ( traceEnt->team != TEAM_NONE && traceEnt->team != ent->client->sess.cs.team )
				{
					continue;
				}

				// user for trigger do not match
				if ( traceEnt->trigger.singleUserEntIndex != ENTITYNUM_NONE && traceEnt->trigger.singleUserEntIndex != ent->client->ps.clientNum )
				{
					continue;
				}

				if ( traceEnt->s.hintType != HINT_NONE && traceEnt->s.hintString != 255 )
				{
					hintString = traceEnt->s.hintString;
				}
			}

			ps->cursorHintEntIndex = traceEnt->s.number;
			ps->cursorHint = traceEnt->s.hintType;
			ps->cursorHintString = hintString;

			if ( !ps->cursorHint )
			{
				ps->cursorHintEntIndex = ENTITYNUM_NONE;
			}

			return;
		}

		if ( traceEnt->s.eType == ET_ITEM )
		{
			int itemHint = Player_GetItemCursorHint(ent->client, traceEnt);

			if ( !itemHint )
			{
				continue;
			}

			ps->cursorHintEntIndex = traceEnt->s.number;
			ps->cursorHint = itemHint;
			ps->cursorHintString = hintString;

			if ( !ps->cursorHint )
			{
				ps->cursorHintEntIndex = ENTITYNUM_NONE;
			}

			return;
		}

		if ( traceEnt->s.eType == ET_TURRET )
		{
			if ( !G_IsTurretUsable(traceEnt, ent) )
			{
				continue;
			}

			weapDef = BG_GetWeaponDef(traceEnt->s.weapon);
			assert(weapDef);

			if ( weapDef->useHintString[0] )
			{
				hintString = weapDef->useHintStringIndex;
			}

			ps->cursorHintEntIndex = traceEnt->s.number;
			ps->cursorHint = traceEnt->s.weapon + 4;
			ps->cursorHintString = hintString;

			if ( !ps->cursorHint )
			{
				ps->cursorHintEntIndex = ENTITYNUM_NONE;
			}

			return;
		}
	}
}

/*
===============
compare_use
===============
*/
static signed int compare_use( const void *num1, const void *num2 )
{
	useList_t *a = (useList_t *)num1;
	useList_t *b = (useList_t *)num2;

	return a->score - b->score;
}

/*
===============
Player_ActivateCmd
===============
*/
bool Player_ActivateCmd( gentity_t *ent )
{
	if ( !Scr_IsSystemActive() )
	{
		return false;
	}

	assert(ent);
	assert(ent->r.inuse);
	assert(ent->client);

	ent->client->useHoldEntity = ENTITYNUM_NONE;

	if ( ent->active )
	{
		if ( ent->client->ps.eFlags & EF_TURRET_ACTIVE )
			ent->active = ACTIVE_TURRET;
		else
			ent->active = qfalse;

		return true;
	}

	if ( ent->client->ps.pm_flags & PMF_MANTLE )
	{
		return true;
	}

	if ( ent->client->ps.cursorHintEntIndex != ENTITYNUM_NONE )
	{
		ent->client->useHoldEntity = ent->client->ps.cursorHintEntIndex;
		ent->client->useHoldTime = level.time;

		return true;
	}

	return false;
}

/*
===============
Player_SetTurretDropHint
===============
*/
void Player_SetTurretDropHint( gentity_t *ent )
{
	playerState_t *ps;
	gentity_t *turret;
	WeaponDef *weapDef;

	assert(ent);
	assert(ent->client);
	assert(ent->active);

	ps = &ent->client->ps;
	assert(ps);
	assert(ps->eFlags & EF_TURRET_ACTIVE);
	assert(ps->viewlocked_entNum != ENTITYNUM_NONE);

	turret = &level.gentities[ps->viewlocked_entNum];
	assert(turret->s.eType == ET_TURRET);

	weapDef = BG_GetWeaponDef(turret->s.weapon);
	assert(weapDef);

	if ( !weapDef->dropHintString[0] )
	{
		return;
	}

	ps->cursorHintEntIndex = ENTITYNUM_NONE;
	ps->cursorHint = turret->s.weapon + 4;
	ps->cursorHintString = weapDef->dropHintStringIndex;
}

/*
===============
Player_GetItemCursorHint
===============
*/
int Player_GetItemCursorHint( gclient_t *client, gentity_t *traceEnt )
{
	assert(traceEnt);
	assert(client);

	int index = traceEnt->item.index;
	assert(((0 <= index) && (index < (( MAX_WEAPONS ) ))));

	gitem_t *item = &bg_itemlist[index];
	assert(item->giType == IT_WEAPON);

	if ( item->giType != IT_WEAPON )
	{
		return 0;
	}

	WeaponDef *weapDef = BG_GetWeaponDef(item->giTag);

	if ( weapDef->weaponType == WEAPTYPE_GRENADE )
	{
		return 0;
	}

	if ( Com_BitCheck(client->ps.weapons, item->giTag))
	{
		return 0;
	}

	return item->giTag + 4;
}

/*
===============
Player_UseEntity
===============
*/
void Player_UseEntity( gentity_t *playerEnt, gentity_t *useEnt )
{
	void (*touch)(gentity_t *, gentity_t *, int);
	void (*use)(gentity_t *, gentity_t *, gentity_t *);

	assert(playerEnt);
	assert(playerEnt->client);
	assert(useEnt);
	assert(useEnt->r.inuse);

	use = entityHandlers[useEnt->handler].use;
	touch = entityHandlers[useEnt->handler].touch;

	if ( useEnt->s.eType == ET_ITEM )
	{
		Scr_AddEntity(playerEnt);
		Scr_Notify(useEnt, scr_const.touch, 1);

		useEnt->active = qtrue;

		if ( touch )
		{
			touch(useEnt, playerEnt, 0);
		}
	}
	else if ( useEnt->s.eType != ET_TURRET || G_IsTurretUsable(useEnt, playerEnt) )
	{
		Scr_AddEntity(playerEnt);
		Scr_Notify(useEnt, scr_const.trigger, 1);

		if ( use )
		{
			use(useEnt, playerEnt, playerEnt);
		}
	}

	playerEnt->client->useHoldEntity = ENTITYNUM_NONE;
}

/*
===============
Player_ActivateHoldCmd
===============
*/
void Player_ActivateHoldCmd( gentity_t *ent )
{
	if ( !Scr_IsSystemActive() )
	{
		return;
	}

	assert(ent);
	assert(ent->client);

	if ( ent->client->useHoldEntity == ENTITYNUM_NONE )
	{
		return;
	}

	if ( level.time - ent->client->lastSpawnTime < g_useholdspawndelay->current.integer )
	{
		return;
	}

	if ( level.time - ent->client->useHoldTime < g_useholdtime->current.integer )
	{
		return;
	}

	Player_UseEntity(ent, &g_entities[ent->client->useHoldEntity]);
}

/*
===============
Player_UpdateLookAtEntityTrace
===============
*/
gentity_t* Player_UpdateLookAtEntityTrace( trace_t *trace, const vec3_t start, const vec3_t end,
        int passentitynum, int contentmask, unsigned char *priorityMap, vec3_t vForward )
{
	G_LocationalTrace(trace, start, end, passentitynum, contentmask, priorityMap);

	if ( trace->entityNum >= ENTITYNUM_WORLD )
	{
		return NULL;
	}

	// Visibility code not supported
	return &g_entities[trace->entityNum];
}

/*
===============
Player_GetUseList
===============
*/
static vec3_t useRadius = { 192.0, 192.0, 96.0 };
int Player_GetUseList( gentity_t *ent, useList_t *useList )
{
	vec3_t mins, maxs, absmin, absmax, origin, forward, midpoint, dest;
	int itemEntCount = 0, otherEntCount = 0;
	int i, useCount, num;
	float dist;
	int entityList[MAX_GENTITIES];
	playerState_t *ps;
	gentity_t *gEnt, *ent2;

	assert(ent);
	assert(ent->client);
	ps = &ent->client->ps;
	assert(ps);

	G_GetPlayerViewOrigin(ent, origin);
	G_GetPlayerViewDirection(ent, forward, NULL, NULL);

	VectorAdd(ps->origin, ps->mins, absmin);
	VectorAdd(ps->origin, ps->maxs, absmax);

	VectorSubtract(origin, useRadius, mins);
	VectorAdd(origin, useRadius, maxs);

	num = CM_AreaEntities(mins, maxs, entityList, MAX_GENTITIES, CONTENTS_DONOTENTER);
	useCount = 0;

	for ( i = 0; i < num; i++ )
	{
		gEnt = &g_entities[entityList[i]];

		if ( ent == gEnt )
		{
			continue;
		}

		if ( gEnt->s.eType != ET_ITEM && !( gEnt->r.contents & CONTENTS_DONOTENTER ) )
		{
			continue;
		}

		if ( gEnt->classname == scr_const.trigger_use_touch )
		{
			if ( gEnt->r.absmin[0] > absmax[0] )
			{
				continue;
			}

			if ( absmin[0] > gEnt->r.absmax[0] )
			{
				continue;
			}

			if ( gEnt->r.absmin[1] > absmax[1] )
			{
				continue;
			}

			if ( absmin[1] > gEnt->r.absmax[1] )
			{
				continue;
			}

			if ( gEnt->r.absmin[2] > absmax[2] )
			{
				continue;
			}

			if ( absmin[2] > gEnt->r.absmax[2] )
			{
				continue;
			}

			if ( !SV_EntityContact(absmin, absmax, gEnt) )
			{
				continue;
			}

			useList[useCount].score = -256;
			useList[useCount].ent = gEnt;
			useCount++;
		}
		else
		{
			VectorAdd(gEnt->r.absmin, gEnt->r.absmax, midpoint);
			VectorScale(midpoint, 0.5, midpoint);
			VectorSubtract(midpoint, origin, dest);

			dist = Vec3Normalize(dest);

			if ( dist > 128 )
			{
				continue;
			}

			useList[useCount].score = 1.0 - (DotProduct(dest, forward) + 1.0) * 0.5 * 256;

			if ( gEnt->classname == scr_const.trigger_use )
			{
				useList[useCount].score -= 256;
			}

			if ( gEnt->s.eType == ET_ITEM && !BG_CanItemBeGrabbed(&gEnt->s, &ent->client->ps, qfalse) )
			{
				useList[useCount].score += 10000;
				itemEntCount++;
			}

			useList[useCount].ent = gEnt;
			useList[useCount].score += dist;
			useCount++;
		}
	}

	qsort(useList, useCount, sizeof(useList_t), compare_use);
	useCount -= itemEntCount;

	for ( i = 0; i < useCount; i++ )
	{
		ent2 = useList[i].ent;

		if ( ent2->classname == scr_const.trigger_use_touch )
		{
			continue;
		}

		VectorAdd(ent2->r.absmin, ent2->r.absmax, midpoint);
		VectorScale(midpoint, 0.5, midpoint);

		if ( ent2->s.eType == ET_TURRET )
		{
			G_DObjGetWorldTagPos(ent2, scr_const.tag_aim, midpoint);
		}

		if ( G_TraceCapsuleComplete(origin, vec3_origin, vec3_origin, midpoint, ps->clientNum, CONTENTS_SOLID | CONTENTS_GLASS) )
		{
			continue;
		}

		useList[i].score += 10000;
		otherEntCount++;
	}

	qsort(useList, useCount, sizeof(useList_t), compare_use);
	useCount -= otherEntCount;

	return useCount;
}
