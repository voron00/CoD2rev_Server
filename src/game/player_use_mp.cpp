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
#define bg_itemlist ((gitem_t*)( 0x08164C20 ))
#else
extern gitem_t bg_itemlist[];
#endif

bool Player_ActivateCmd(gentity_s *ent)
{
	if ( !Scr_IsSystemActive() )
		return 0;

	ent->client->useHoldEntity = 1023;

	if ( ent->active )
	{
		if ( (ent->client->ps.eFlags & 0x300) != 0 )
			ent->active = 2;
		else
			ent->active = 0;

		return 1;
	}
	else if ( (ent->client->ps.pm_flags & 4) != 0 )
	{
		return 1;
	}
	else if ( ent->client->ps.cursorHintEntIndex == 1023 )
	{
		return 0;
	}
	else
	{
		ent->client->useHoldEntity = ent->client->ps.cursorHintEntIndex;
		ent->client->useHoldTime = level.time;

		return 1;
	}
}

void Player_UseEntity(gentity_s *playerEnt, gentity_s *useEnt)
{
	void (*touch)(struct gentity_s *, struct gentity_s *, int);
	void (*use)(struct gentity_s *, struct gentity_s *, struct gentity_s *);

	use = entityHandlers[useEnt->handler].use;
	touch = entityHandlers[useEnt->handler].touch;

	if ( useEnt->s.eType == ET_ITEM )
	{
		Scr_AddEntity(playerEnt);
		Scr_Notify(useEnt, scr_const.touch, 1);
		useEnt->active = 1;

		if ( touch )
			touch(useEnt, playerEnt, 0);
	}
	else if ( useEnt->s.eType != ET_TURRET || G_IsTurretUsable(useEnt, playerEnt) )
	{
		Scr_AddEntity(playerEnt);
		Scr_Notify(useEnt, scr_const.trigger, 1);

		if ( use )
			use(useEnt, playerEnt, playerEnt);
	}

	playerEnt->client->useHoldEntity = 1023;
}

extern dvar_t *g_useholdspawndelay;
extern dvar_t *g_useholdtime;
void Player_ActivateHoldCmd(gentity_s *ent)
{
	if ( Scr_IsSystemActive()
	        && ent->client->useHoldEntity != 1023
	        && level.time - ent->client->lastSpawnTime >= g_useholdspawndelay->current.integer
	        && level.time - ent->client->useHoldTime >= g_useholdtime->current.integer )
	{
		Player_UseEntity(ent, &g_entities[ent->client->useHoldEntity]);
	}
}

void Player_UpdateActivate(gentity_s *ent)
{
	bool useSucceeded;

	ent->client->ps.pm_flags &= ~8u;

	if ( ent->client->ps.weaponstate < WEAPON_BINOCULARS_INIT || ent->client->ps.weaponstate > WEAPON_BINOCULARS_END )
	{
		useSucceeded = 0;

		if ( ent->client->useHoldEntity == 1023
		        || (ent->client->oldbuttons & 0x20) == 0
		        || (ent->client->buttons & 0x20) != 0 )
		{
			if ( (ent->client->latched_buttons & 0x28) != 0 )
				useSucceeded = Player_ActivateCmd(ent);

			if ( ent->client->useHoldEntity != 1023 || useSucceeded )
			{
				if ( (ent->client->buttons & 0x28) != 0 )
					Player_ActivateHoldCmd(ent);
			}
			else if ( (ent->client->latched_buttons & 0x20) != 0 )
			{
				ent->client->ps.pm_flags |= 8u;
			}
		}
		else
		{
			ent->client->ps.pm_flags |= 8u;
		}
	}
}

vec3_t useRadius = { 192.0, 192.0, 96.0 };

static signed int compare_use(const void *num1, const void *num2)
{
	useList_t *a;
	useList_t *b;

	a = (useList_t *)num1;
	b = (useList_t *)num2;

	return (int)(a->score - b->score);
}

int Player_GetUseList(gentity_s *ent, useList_t *useList)
{
	float vLenSq;
	int useCount;
	vec3_t absmax;
	vec3_t absmin;
	int j;
	int i;
	int entityCount;
	int entities;
	int entityList[1024];
	float vDist;
	float vNorm;
	vec3_t vAng;
	vec3_t pos;
	vec3_t vOrigin;
	vec3_t maxs;
	vec3_t mins;
	vec3_t vForward;
	gentity_s *areaEnt;
	gclient_s *client;
	gentity_s *other;

	useCount = 0;
	client = ent->client;
	G_GetPlayerViewOrigin(ent, vOrigin);
	G_GetPlayerViewDirection(ent, vForward, 0, 0);
	VectorAdd(client->ps.origin, client->ps.mins, absmin);
	VectorAdd(client->ps.origin, client->ps.maxs, absmax);
	VectorSubtract(vOrigin, useRadius, mins);
	VectorAdd(vOrigin, useRadius, maxs);
	entities = CM_AreaEntities(mins, maxs, entityList, 1024, 0x200000);
	entityCount = 0;

	for ( i = 0; i < entities; ++i )
	{
		areaEnt = &g_entities[entityList[i]];

		if ( ent != areaEnt && (areaEnt->s.eType == ET_ITEM || (areaEnt->r.contents & 0x200000) != 0) )
		{
			if ( areaEnt->classname == scr_const.trigger_use_touch )
			{
				if ( areaEnt->r.absmin[0] <= absmax[0]
				        && absmin[0] <= areaEnt->r.absmax[0]
				        && areaEnt->r.absmin[1] <= absmax[1]
				        && absmin[1] <= areaEnt->r.absmax[1]
				        && areaEnt->r.absmin[2] <= absmax[2]
				        && absmin[2] <= areaEnt->r.absmax[2] )
				{
					if ( SV_EntityContact(absmin, absmax, areaEnt) )
					{
						useList[entityCount].score = -256.0;
						useList[entityCount].ent = areaEnt;
						entityCount++;
					}
				}
			}
			else
			{
				VectorAdd(areaEnt->r.absmin, areaEnt->r.absmax, pos);
				VectorScale(pos, 0.5, pos);
				VectorSubtract(pos, vOrigin, vAng);
				vNorm = Vec3Normalize(vAng);

				if ( vNorm <= 128.0 )
				{
					vLenSq = DotProduct(vAng, vForward);
					vDist = 1.0 - (vLenSq + 1.0) * 0.5;
					useList[entityCount].score = vDist * 256.0;

					if ( areaEnt->classname == scr_const.trigger_use )
						useList[entityCount].score = useList[entityCount].score - 256.0;

					if ( areaEnt->s.eType == ET_ITEM && !BG_CanItemBeGrabbed(&areaEnt->s, &ent->client->ps, 0) )
					{
						useList[entityCount].score = useList[entityCount].score + 10000.0;
						++useCount;
					}

					useList[entityCount].ent = areaEnt;
					useList[entityCount].score = useList[entityCount].score + vNorm;
					++entityCount;
				}
			}
		}
	}

	qsort(useList, entityCount, 8u, compare_use);
	entityCount -= useCount;
	j = 0;

	for ( i = 0; i < entityCount; ++i )
	{
		other = useList[i].ent;

		if ( other->classname != scr_const.trigger_use_touch )
		{
			VectorAdd(other->r.absmin, other->r.absmax, pos);
			VectorScale(pos, 0.5, pos);

			if ( other->s.eType == ET_TURRET )
				G_DObjGetWorldTagPos(other, scr_const.tag_aim, pos);

			if ( !G_TraceCapsuleComplete(vOrigin, vec3_origin, vec3_origin, pos, client->ps.clientNum, 17) )
			{
				useList[i].score = useList[i].score + 10000.0;
				++j;
			}
		}
	}

	qsort(useList, entityCount, 8u, compare_use);
	return entityCount - j;
}

void Player_SetTurretDropHint(gentity_s *ent)
{
	gclient_s *client;
	gentity_s *other;

	client = ent->client;
	other = &level.gentities[client->ps.viewlocked_entNum];

	if ( *BG_GetWeaponDef(other->s.weapon)->dropHintString )
	{
		client->ps.cursorHintEntIndex = 1023;
		client->ps.cursorHint = other->s.weapon + 4;
		client->ps.cursorHintString = BG_GetWeaponDef(other->s.weapon)->dropHintStringIndex;
	}
}

int Player_GetItemCursorHint(gclient_s *client, gentity_s *traceEnt)
{
	int index;
	gitem_s *item;

	item = &bg_itemlist[traceEnt->item.index];
	index = 0;

	if ( item->giType == IT_WEAPON
	        && BG_GetWeaponDef(item->giTag)->weaponType != WEAPTYPE_GRENADE
	        && !COM_BitTest(client->ps.weapons, item->giTag) )
	{
		return item->giTag + 4;
	}

	return index;
}

void Player_UpdateCursorHints(gentity_s *ent)
{
	int type;
	int i;
	int entiytCount;
	int useHint;
	int hint;
	int item;
	gentity_s *useEnt;
	gclient_s *client;
	useList_t useList[1024];

	client = ent->client;
	client->ps.cursorHint = 0;
	client->ps.cursorHintString = -1;
	client->ps.cursorHintEntIndex = 1023;

	if ( ent->health > 0
	        && (ent->client->ps.weaponstate < WEAPON_BINOCULARS_INIT || ent->client->ps.weaponstate > WEAPON_BINOCULARS_END) )
	{
		if ( ent->active )
		{
			if ( (client->ps.eFlags & 0x300) != 0 )
				Player_SetTurretDropHint(ent);
		}
		else if ( (ent->client->ps.pm_flags & 4) == 0 )
		{
			entiytCount = Player_GetUseList(ent, useList);

			if ( entiytCount )
			{
				item = 0;
				useHint = -1;

				for ( i = 0; ; ++i )
				{
					if ( i >= entiytCount )
						return;

					useEnt = useList[i].ent;
					type = useEnt->s.eType;

					if ( type == ET_ITEM )
					{
						hint = Player_GetItemCursorHint(ent->client, useEnt);

						if ( hint )
						{
							item = hint;
setstring:
							client->ps.cursorHintEntIndex = useEnt->s.number;
							client->ps.cursorHint = item;
							client->ps.cursorHintString = useHint;

							if ( !client->ps.cursorHint )
								client->ps.cursorHintEntIndex = 1023;

							return;
						}
					}
					else if ( type > ET_ITEM )
					{
						if ( type == ET_TURRET && G_IsTurretUsable(useEnt, ent) )
						{
							item = useEnt->s.weapon + 4;

							if ( *BG_GetWeaponDef(useEnt->s.weapon)->useHintString )
								useHint = BG_GetWeaponDef(useEnt->s.weapon)->useHintStringIndex;

							goto setstring;
						}
					}
					else if ( type == ET_GENERAL )
					{
						if ( useEnt->classname != scr_const.trigger_use && useEnt->classname != scr_const.trigger_use_touch )
							goto setstring;

						if ( (!useEnt->team || useEnt->team == ent->client->sess.state.team)
						        && (useEnt->trigger.singleUserEntIndex == 1023
						            || useEnt->trigger.singleUserEntIndex == ent->client->ps.clientNum) )
						{
							item = useEnt->s.dmgFlags;

							if ( item && useEnt->s.scale != 255 )
								useHint = useEnt->s.scale;

							goto setstring;
						}
					}
				}
			}
		}
	}
}