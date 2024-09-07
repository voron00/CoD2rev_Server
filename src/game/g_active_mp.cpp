#include "../qcommon/qcommon.h"
#include "g_shared.h"

/*
===============
GetFollowPlayerState
===============
*/
qboolean GetFollowPlayerState( int clientNum, playerState_t *ps )
{
	gclient_t *client;

	assert( clientNum >= 0 && clientNum < level.maxclients );
	client = g_entities[clientNum].client;

	assert(client);
	assert(client == g_entities[clientNum].client);

	if ( !(client->ps.pm_flags & PMF_PLAYER) )
	{
		return qfalse;
	}

	*ps = client->ps;
	memset(&ps->hud.current, 0, sizeof(ps->hud.current));

	return qtrue;
}

/*
===============
G_ClientCanSpectateTeam
===============
*/
qboolean G_ClientCanSpectateTeam( gclient_t *client, int team )
{
	return (client->sess.noSpectate & (1 << team)) == qfalse;
}

/*
===============
G_SetClientContents
===============
*/
void G_SetClientContents( gentity_t *pEnt )
{
#ifdef LIBCOD
	if ( !g_playerCollision->current.boolean )
	{
		return;
	}
#endif

	assert(pEnt->client);

	if ( pEnt->client->noclip )
	{
		pEnt->r.contents = CONTENTS_NONE;
		return;
	}

	if ( pEnt->client->ufo )
	{
		pEnt->r.contents = CONTENTS_NONE;
		return;
	}

	if ( pEnt->client->sess.sessionState == SESS_STATE_DEAD )
	{
		pEnt->r.contents = CONTENTS_NONE;
		return;
	}

	pEnt->r.contents = CONTENTS_BODY;
}

/*
===============
G_SetLastServerTime
===============
*/
void G_SetLastServerTime( int clientNum, int lastServerTime )
{
	gentity_t *ent;

	ent = &g_entities[clientNum];
	assert(ent->client);

	if ( level.time - lastServerTime > 1000 )
		lastServerTime = level.time - 1000;

	if ( lastServerTime >= ent->client->lastServerTime || level.time <= lastServerTime )
		ent->client->lastServerTime = lastServerTime;
}

/*
====================
ClientIntermissionThink
====================
*/
void ClientIntermissionThink( gentity_t *ent )
{
	gclient_t *client;

	client = ent->client;
	assert(client);

	// the level will exit when everyone wants to or after timeouts

	// swap and latch button actions
	client->oldbuttons = client->buttons;
	client->buttons = client->sess.cmd.buttons;
//----(SA)	added
	client->buttonsSinceLastFrame |= client->buttons & ~client->oldbuttons;
}

/*
===============
G_SetClientSound
===============
*/
void G_SetClientSound( gentity_t *ent )
{
	assert(ent);
	ent->s.loopSound = 0;
}

/*
===============
StuckInClient
===============
*/
#define PLAYER_PUSH_TIME 300
qboolean StuckInClient( gentity_t *self )
{
	vec2_t vDelta;
	gentity_t *hit;
	float hitSpeed;
	float selfSpeed;
	int num;

#ifdef LIBCOD
	if ( !g_playerEject->current.boolean )
	{
		return qfalse;
	}
#endif

	assert(self);

	if ( !(self->client->ps.pm_flags & PMF_PLAYER) )
	{
		return qfalse;
	}

	if ( self->client->sess.sessionState != SESS_STATE_PLAYING )
	{
		return qfalse;
	}

	if ( self->r.contents != CONTENTS_BODY && self->r.contents != CONTENTS_CORPSE )
	{
		return qfalse;
	}

	for ( num = 0, hit = g_entities; num < MAX_GENTITIES; num++, hit++ )
	{
		if ( num >= level.maxclients )
		{
			return qfalse;
		}

		if ( !hit->r.inuse )
		{
			continue;
		}

		if ( hit->client == NULL )
		{
			continue;
		}

		if ( !( hit->client->ps.pm_flags & PMF_PLAYER ) )
		{
			continue;
		}

		if ( hit->client->sess.sessionState != SESS_STATE_PLAYING )
		{
			continue;
		}

		if ( hit == self )
		{
			continue;
		}

		if ( hit->health <= 0 )
		{
			continue;
		}

		if ( hit->r.contents != CONTENTS_BODY && hit->r.contents != CONTENTS_CORPSE )
		{
			continue;
		}

		if ( hit->r.absmin[0] > self->r.absmax[0] )
		{
			continue;
		}

		if ( self->r.absmin[0] > hit->r.absmax[0] )
		{
			continue;
		}

		if ( hit->r.absmin[1] > self->r.absmax[1] )
		{
			continue;
		}

		if ( self->r.absmin[1] > hit->r.absmax[1] )
		{
			continue;
		}

		if ( hit->r.absmin[2] > self->r.absmax[2] )
		{
			continue;
		}

		if ( self->r.absmin[2] > hit->r.absmax[2] )
		{
			continue;
		}

		Vector2Subtract(hit->r.currentOrigin, self->r.currentOrigin, vDelta);

		if ( Vec2LengthSq(vDelta) <= Square(self->r.maxs[0] + hit->r.maxs[0]) )
		{
			break;
		}
	}

	assert(hit->client);

	Vector2Subtract(hit->r.currentOrigin, self->r.currentOrigin, vDelta);

	vDelta[0] = G_crandom() + vDelta[0];
	vDelta[1] = G_crandom() + vDelta[1];

	Vec2Length(hit->client->ps.velocity);

	if ( Vec2Normalize(vDelta) > 0 )
		hitSpeed = (float)g_playerCollisionEjectSpeed->current.integer;
	else
		hitSpeed = 0;

	Vec2Length(self->client->ps.velocity);

	if ( hitSpeed > 0 )
		selfSpeed = (float)g_playerCollisionEjectSpeed->current.integer;
	else
		selfSpeed = 0;

	if ( hitSpeed < 0.000099999997 && selfSpeed < 0.000099999997 )
	{
		hitSpeed = (float)hit->client->ps.speed;
		selfSpeed = (float)self->client->ps.speed;
	}

	Vec2Scale(vDelta, hitSpeed, hit->client->ps.velocity);

	hit->client->ps.pm_time = PLAYER_PUSH_TIME;
	hit->client->ps.pm_flags |= PMF_TIME_SLIDE;

	Vec2Scale(vDelta, -selfSpeed, self->client->ps.velocity);

	self->client->ps.pm_time = PLAYER_PUSH_TIME;
	self->client->ps.pm_flags |= PMF_TIME_SLIDE;

	return qtrue;
}

/*
===============
G_PlayerEvent
===============
*/
void G_PlayerEvent( int clientNum, int event )
{
	gclient_t *client;
	vec3_t kickAVel;

	client = g_entities[clientNum].client;
	assert(client);

	if ( event < EV_FIRE_WEAPON )
	{
		return;
	}

	if ( event > EV_FIRE_WEAPON_LASTSHOT && event != EV_FIRE_WEAPON_MG42 )
	{
		return;
	}

	BG_WeaponFireRecoil(&client->ps, client->vGunSpeed, kickAVel);
}

/*
===============
G_DamageFeedback

Called just before a snapshot is sent to the given player.
Totals up all damage and generates both the player_state_t
damage values to that client for pain blends and kicks, and
global pain sound events for all clients.
===============
*/
#define DAMAGE_COUNT_DURATION 20
void P_DamageFeedback( gentity_t *player )
{
	vec3_t viewaxis[3];
	float kick;
	vec3_t angles;
	int count;
	gclient_t *client;

	assert(player);
	client = player->client;
	assert(client);

	if ( client->ps.pm_type >= PM_DEAD )
	{
		return;
	}

	// total points of damage shot at the player this frame
	client->ps.damageCount = 0;
	count = client->damage_blood;

	if ( count <= 0 )
	{
		return; // didn't take any damage
	}

	if ( client->sess.maxHealth <= 0 )
	{
		return;
	}

	count = 100 * count / client->sess.maxHealth;

	if ( count > 127 )
	{
		count = 127;
	}

	// send the information to the client
	client->ps.aimSpreadScale = count + client->ps.aimSpreadScale;

	if ( client->ps.aimSpreadScale > 255 )
	{
		client->ps.aimSpreadScale = 255;
	}

	kick = count * 0.2;

	if ( kick < 5.0 )
	{
		kick = 5.0;
	}
	else if ( kick > 90.0 )
	{
		kick = 90.0;
	}

	// world damage (falling, slime, etc) uses a special code
	// to make the blend blob centered instead of positional
	if ( client->damage_fromWorld )
	{
		client->v_dmg_roll = 0;
		client->v_dmg_pitch = -kick;
		client->ps.damagePitch = 255;
		client->ps.damageYaw = 255;
		client->damage_fromWorld = 0;
	}
	else
	{
		vectoangles(client->damage_from, angles);
		AnglesToAxis(client->ps.viewangles, viewaxis);

		client->v_dmg_roll  = DotProduct(client->damage_from, viewaxis[1]) * -kick;
		client->v_dmg_pitch = DotProduct(client->damage_from, viewaxis[0]) * kick;

		client->ps.damagePitch = angles[PITCH] / 360.0 * 256;
		client->ps.damageYaw = angles[YAW] / 360.0 * 256;
	}

	client->ps.damageEvent++; // Ridah, always increment this since we do multiple view damage anims
	client->damageTime = level.time - DAMAGE_COUNT_DURATION;
	client->ps.damageCount = count;

	//
	// clear totals
	//
	client->damage_blood = 0;
}

/*
===============
G_PlayerController
===============
*/
void G_PlayerController( gentity_t *self, int *partBits )
{
	DObj *obj;
	clientInfo_t *ci;

	assert(self->s.clientNum >= 0 && self->s.clientNum <= MAX_CLIENTS);

	ci = &level_bgs.clientinfo[self->s.clientNum];
	assert(ci->infoValid);
	obj = Com_GetServerDObj(self->s.number);

	BG_Player_DoControllers(obj, self, partBits, ci, level.frameTime);
}

/*
===============
IntermissionClientEndFrame
===============
*/
void IntermissionClientEndFrame( gentity_t *ent )
{
	gclient_t *client;

	assert(ent);
	client = ent->client;
	assert(client);

	ent->r.svFlags &= ~SVF_BODY;
	ent->r.svFlags |= SVF_NOCLIENT;

	ent->takedamage = 0;
	ent->r.contents = 0;

	client->ps.pm_flags &= ~( CONTENTS_UNKNOWN | CONTENTS_MANTLE | CONTENTS_BODY );
	client->ps.pm_type = PM_INTERMISSION;

	client->ps.eFlags &= ~EF_TALK;
	client->ps.eFlags &= ~EF_FIRING;

	client->ps.viewmodelIndex = 0;
	ent->s.eType = ET_INVISIBLE;

	SV_SetConfigstring(CS_SCORES1, va("%i", level.teamScores[TEAM_AXIS]));
	SV_SetConfigstring(CS_SCORES2, va("%i", level.teamScores[TEAM_ALLIES]));
}

/*
============
G_TouchTriggers

Find all trigger entities that ent's current position touches.
Spectators will only interact with teleporters.
============
*/
void G_TouchTriggers( gentity_t *ent )
{
	int i, num;
	int touch[MAX_GENTITIES];
	gentity_t   *hit;
	trace_t trace;
	vec3_t mins, maxs;
	static vec3_t range = { 40, 40, 52 };
	void (*hitTouch)(gentity_t *, gentity_t *, int);
	void (*entTouch)(gentity_t *, gentity_t *, int);

	assert(ent->client);

	// dead clients don't activate triggers!
	if ( ent->client->ps.pm_type >= PM_DEAD )
	{
		return;
	}

	VectorSubtract( ent->client->ps.origin, range, mins );
	VectorAdd( ent->client->ps.origin, range, maxs );

	num = CM_AreaEntities( mins, maxs, touch, MAX_GENTITIES, CONTENTS_LAVA | CONTENTS_TELEPORTER | CONTENTS_JUMPPAD | CONTENTS_CLUSTERPORTAL | CONTENTS_DONOTENTER_LARGE | CONTENTS_TRIGGER );

	// can't use ent->absmin, because that has a one unit pad
	VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
	VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );

	ShrinkBoundsToHeight( mins, maxs );
	entTouch = entityHandlers[ent->handler].touch;

	for ( i = 0 ; i < num ; i++ )
	{
		hit = &g_entities[touch[i]];
		hitTouch = entityHandlers[hit->handler].touch;

		if ( !hitTouch && !entTouch )
		{
			continue;
		}

		// use seperate code for determining if an item is picked up
		// so you don't have to actually contact its bounding box
		if ( hit->s.eType == ET_ITEM )
		{
			if ( !BG_PlayerTouchesItem( &ent->client->ps, &hit->s, level.time ) )
			{
				continue;
			}
		}
		else
		{
			// MrE: always use capsule for player
			if ( !SV_EntityContact( mins, maxs, hit ) )
			{
				//if ( !SV_EntityContact( mins, maxs, hit ) ) {
				continue;
			}
		}

		if ( Scr_IsSystemActive() )
		{
			Scr_AddEntity(ent);
			Scr_Notify(hit, scr_const.touch, 1);

			Scr_AddEntity(hit);
			Scr_Notify(ent, scr_const.touch, 1);
		}

		if ( hitTouch )
		{
			hitTouch(hit, ent, qtrue);
		}
	}
}

/*
==============
ClientImpacts
==============
*/
void ClientImpacts( gentity_t *ent, pmove_t *pm )
{
	int i, j;
	gentity_t   *other;
	void (*otherTouch)(gentity_t *, gentity_t *, int);
	void (*entTouch)(gentity_t *, gentity_t *, int);

	entTouch = entityHandlers[ent->handler].touch;
	for ( i = 0 ; i < pm->numtouch ; i++ )
	{
		for ( j = 0 ; j < i ; j++ )
		{
			if ( pm->touchents[j] == pm->touchents[i] )
			{
				break;
			}
		}
		if ( j != i )
		{
			continue;   // duplicated
		}
		other = &g_entities[ pm->touchents[i] ];

		if ( Scr_IsSystemActive() )
		{
			Scr_AddEntity(other);
			Scr_Notify(ent, scr_const.touch, 1);

			Scr_AddEntity(ent);
			Scr_Notify(other, scr_const.touch, 1);
		}
		otherTouch = entityHandlers[other->handler].touch;

		if ( otherTouch )
		{
			otherTouch(other, ent, qtrue);
		}

		if ( entTouch )
		{
			entTouch(ent, other, qtrue);
		}
	}
}

/*
==================
SpectatorClientEndFrame
==================
*/
void SpectatorClientEndFrame( gentity_t *ent )
{
	int pArchiveTime;
	gclient_t *client;
	clientState_t cs;
	playerState_t ps;
	int clientNum;

	client = ent->client;
	assert(client);

	ent->r.svFlags &= ~SVF_BODY;
	ent->r.svFlags |= SVF_NOCLIENT;

	ent->takedamage = qfalse;
	ent->r.contents = 0;

	client->ps.pm_flags &= ~PMF_PLAYER;
	ent->s.eType = ET_INVISIBLE;

	client->ps.viewmodelIndex = 0;

	client->fGunPitch = 0;
	client->fGunYaw = 0;

	if ( client->sess.forceSpectatorClient >= 0 )
	{
		clientNum = client->sess.forceSpectatorClient;
		client->spectatorClient = clientNum;

		while ( 1 )
		{
			if ( client->sess.archiveTime < 0 )
			{
				client->sess.archiveTime = 0;
			}

			pArchiveTime = client->sess.archiveTime - client->sess.psOffsetTime;

			if ( SV_GetArchivedClientInfo(client->sess.forceSpectatorClient, &pArchiveTime, &ps, &cs) )
			{
				if ( G_ClientCanSpectateTeam(client, cs.team) )
				{
					client->ps = ps;
					HudElem_UpdateClient(client, ent->s.number, HUDELEM_UPDATE_CURRENT);

					//client->ps.eFlags = client->ps.eFlags & 0x1000000 | ps.eFlags & 0xFEFFFFFF; // legacy ?

					client->ps.pm_flags &= ~PMF_PLAYER;
					client->ps.pm_flags |= PMF_FOLLOW;

					if ( client->sess.forceSpectatorClient >= 0 )
					{
						client->ps.pm_flags &= ~PMF_SPECTATOR_FREE;
						return;
					}

					client->ps.pm_flags |= PMF_SPECTATOR_FREE;

					if ( G_ClientCanSpectateTeam(client, TEAM_NUM_TEAMS) )
						client->ps.pm_flags |= PMF_SPECTATOR_FOLLOW;
					else
						client->ps.pm_flags &= ~PMF_SPECTATOR_FOLLOW;

					return;
				}
			}

			if ( !client->sess.archiveTime )
			{
				client->sess.forceSpectatorClient = -1;
				client->spectatorClient = -1;
				break;
			}

			client->sess.archiveTime -= 50;
		}
	}

	if ( client->spectatorClient < 0 && !G_ClientCanSpectateTeam(client, TEAM_NUM_TEAMS) )
	{
		Cmd_FollowCycle_f(ent, 1);
	}

	clientNum = client->spectatorClient;

	if ( clientNum >= 0 )
	{
		pArchiveTime = client->sess.archiveTime + client->sess.psOffsetTime;

		if ( SV_GetArchivedClientInfo(clientNum, &pArchiveTime, &ps, &cs) )
		{
			if ( G_ClientCanSpectateTeam(client, cs.team) )
			{
				client->ps = ps;
				HudElem_UpdateClient(client, ent->s.number, HUDELEM_UPDATE_CURRENT);

				//client->ps.eFlags = client->ps.eFlags & 0x1000000 | ps.eFlags & 0xFEFFFFFF; // legacy ?

				client->ps.pm_flags &= ~PMF_PLAYER;
				client->ps.pm_flags |= PMF_FOLLOW;

				if ( client->sess.forceSpectatorClient >= 0 )
				{
					client->ps.pm_flags &= ~PMF_SPECTATOR_FREE;
					return;
				}

				client->ps.pm_flags |= PMF_SPECTATOR_FREE;

				if ( G_ClientCanSpectateTeam(client, TEAM_NUM_TEAMS) )
					client->ps.pm_flags |= PMF_SPECTATOR_FOLLOW;
				else
					client->ps.pm_flags &= ~PMF_SPECTATOR_FOLLOW;

				return;
			}
		}
	}

	StopFollowing(ent);
	client->ps.pm_flags &= ~PMF_SPECTATOR_FOLLOW;

	if ( G_ClientCanSpectateTeam(client, TEAM_ALLIES) || G_ClientCanSpectateTeam(client, TEAM_AXIS) || G_ClientCanSpectateTeam(client, TEAM_FREE) )
		client->ps.pm_flags |= PMF_SPECTATOR_FREE;
	else
		client->ps.pm_flags &= ~PMF_SPECTATOR_FREE;
}

/*
=================
SpectatorThink
=================
*/
void SpectatorThink( gentity_t *ent, usercmd_t *ucmd )
{
	pmove_t pm;
	gclient_t *client;

	client = ent->client;
	assert(client);

	client->oldbuttons = client->buttons;
//----(SA)	added
	client->buttons = client->sess.cmd.buttons;
	client->buttonsSinceLastFrame |= client->buttons & ~client->oldbuttons;

	if ( client->sess.forceSpectatorClient < 0 && G_ClientCanSpectateTeam(client, TEAM_NUM_TEAMS) && client->spectatorClient >= 0 )
	{
		// melee button to go freespec
		if ( (client->buttons & BUTTON_MELEE) != (client->oldbuttons & BUTTON_MELEE) )
		{
			// code moved to StopFollowing
			StopFollowing(ent);
		}
	}

	// attack and ads button cycles through spectators
	if ( ( client->buttons & BUTTON_ATTACK ) && !( client->oldbuttons & BUTTON_ATTACK ) )
	{
		Cmd_FollowCycle_f(ent, 1);
	}
	else if ( ( client->buttons & BUTTON_ADS ) && !( client->oldbuttons & BUTTON_ADS ) )
	{
		Cmd_FollowCycle_f(ent, -1);
	}

	if ( client->ps.pm_flags & PMF_FOLLOW )
	{
		return;
	}

	client->ps.pm_type = PM_SPECTATOR;

	if ( G_ClientCanSpectateTeam(client, TEAM_NUM_TEAMS) )
		client->ps.speed = 400;
	else
		client->ps.speed = 0;

	// set up for pmove
	memset( &pm, 0, sizeof( pm ) );
	pm.ps = &client->ps;
	pm.cmd = *ucmd;
	pm.tracemask = ( CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_SKY | CONTENTS_UNKNOWN );   // spectators can fly through bodies
	pm.handler = ENT_HANDLER_ACTOR_INIT;

	Pmove( &pm ); // JPW NERVE

	// save results of pmove
	VectorCopy(client->ps.origin, ent->r.currentOrigin);

	SV_UnlinkEntity(ent);
}

/*
=================
ClientInactivityTimer

Returns qfalse if the client is dropped
=================
*/
qboolean ClientInactivityTimer( gclient_t *client )
{
	assert(client - level.clients >= 0 && client - level.clients < level.maxclients);

	// OSP - modified
	if ( !g_inactivity->current.integer )
	{
		// give everyone some time, so if the operator sets g_inactivity during
		// gameplay, everyone isn't kicked
		client->inactivityTime = level.time + 60000;
		client->inactivityWarning = qfalse;
		return qtrue;
	}

	if ( client->sess.cmd.forwardmove || client->sess.cmd.rightmove || client->sess.cmd.buttons & ( BUTTON_ATTACK | BUTTON_JUMP ) )
	{
		client->inactivityTime = level.time + 1000 * g_inactivity->current.integer;
		client->inactivityWarning = qfalse;
		return qtrue;
	}

	if ( client->sess.localClient )
	{
		return qtrue;
	}

	if ( level.time > client->inactivityTime )
	{
		SV_GameDropClient(client - level.clients, "GAME_DROPPEDFORINACTIVITY");
		return qfalse;
	}

	if ( level.time > client->inactivityTime - 10000 && !client->inactivityWarning )
	{
		client->inactivityWarning = qtrue;
		SV_GameSendServerCommand(client - level.clients, SV_CMD_CAN_IGNORE, va("%c \"GAME_INACTIVEDROPWARNING\"", 99));
	}

	return qtrue;
}

/*
================
ClientEvents

Events will be passed on to the clients for presentation,
but any server game effects are handled here
================
*/
void ClientEvents( gentity_t *ent, int oldEventSequence )
{
	vec3_t dir;
	float damage;
	gclient_t *client;
	int event;
	int eventParm;
	int i;

	client = ent->client;
	assert(client);

	if ( oldEventSequence < client->ps.eventSequence - MAX_EVENTS )
	{
		oldEventSequence = client->ps.eventSequence - MAX_EVENTS;
	}

	for ( i = oldEventSequence ; i < client->ps.eventSequence ; i++ )
	{
		event = client->ps.events[ i & ( MAX_EVENTS - 1 ) ];
		eventParm = client->ps.eventParms[ i & ( MAX_EVENTS - 1 ) ];

		if ( event >= EV_LANDING_PAIN_DEFAULT && event <= EV_LANDING_PAIN_ASPHALT )
		{
			if ( ent->s.eType != ET_PLAYER )
			{
				return; // not in the player model
			}

			if ( eventParm >= 100 )
				damage = 1.1;
			else
				damage = eventParm * 0.0099999998;

			if ( damage )
			{
				damage *= client->ps.stats[STAT_MAX_HEALTH];
				VectorSet(dir, 0, 0, 1);
				G_Damage(ent, NULL, NULL, NULL, NULL, damage, DFLAGS_NONE, MOD_FALLING, HITLOC_NONE, 0);
			}

			continue;
		}

		switch ( event )
		{
		case EV_FIRE_WEAPON:
		case EV_FIRE_WEAPONB:
		case EV_FIRE_WEAPON_LASTSHOT:
		case EV_FIRE_WEAPON_MG42:
			if ( g_antilag->current.boolean )
				FireWeapon(ent, client->lastServerTime);
			else
				FireWeapon(ent, level.time);
			break;
		case EV_FIRE_MELEE:
			FireWeaponMelee(ent);
			break;
		case EV_USE_OFFHAND:
			G_UseOffHand(ent);
			break;
		case EV_BINOCULAR_ENTER:
			Scr_Notify(ent, scr_const.binocular_enter, 0);
			break;
		case EV_BINOCULAR_EXIT:
			Scr_Notify(ent, scr_const.binocular_exit, 0);
			break;
		case EV_BINOCULAR_FIRE:
			Scr_Notify(ent, scr_const.binocular_fire, 0);
			break;
		case EV_BINOCULAR_RELEASE:
			Scr_Notify(ent, scr_const.binocular_release, 0);
			break;
		case EV_BINOCULAR_DROP:
			Scr_Notify(ent, scr_const.binocular_drop, 0);
			break;
		case EV_GRENADE_SUICIDE:
			if ( ent->client && !(ent->flags & ( FL_GODMODE | FL_DEMIGOD ) ) )
			{
				ent->health = 0;
				ent->client->ps.stats[STAT_HEALTH] = 0;
				player_die(ent, ent, ent, 100000, MOD_SUICIDE, WP_NONE, NULL, HITLOC_NONE, 0);
			}
			break;
		default:
			break;
		}
	}
}

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame on fast clients.

If "g_synchronousClients 1" is set, this will be called exactly
once for each server frame, which makes for smooth demo recording.
==============
*/
void ClientThink_real( gentity_t *ent, usercmd_t *ucmd )
{
	// VoroN: removed legacy shellshock code
	vec3_t vAxis[3];
	vec3_t vAxis2[3];
	vec3_t vAxis3[3];
	WeaponDef *weapDef;
	vec3_t angles;
	vec3_t viewangles;
	weaponState_t ws;
	viewState_t vs;
	int msec;
	int oldEventSequence;
	pmove_t pm;
	gclient_t *client;

	assert(ent);
	client = ent->client;
	assert(client);

	// don't think if the client is not yet connected (and thus not yet spawned in)
	if ( client->sess.connected != CON_CONNECTED )
	{
		return;
	}

	// sanity check the command time to prevent speedup cheating
	if ( ucmd->serverTime > level.time + 200 )
	{
		ucmd->serverTime = level.time + 200;
//		G_Printf("serverTime <<<<<\n" );
	}
	if ( ucmd->serverTime < level.time - 1000 )
	{
		ucmd->serverTime = level.time - 1000;
//		G_Printf("serverTime >>>>>\n" );
	}

	msec = ucmd->serverTime - client->ps.commandTime;
	// following others may result in bad times, but we still want
	// to check for follow toggles
	if ( msec < 1 && client->ps.clientNum == ent - g_entities )
	{
		return;
	}
	if ( msec > 200 )
	{
		msec = 200;
	}

	// freezeControls func
	if ( client->bFrozen )
		client->ps.pm_flags |= PMF_FROZEN;
	else
		client->ps.pm_flags &= ~PMF_FROZEN;

	//
	// check for exiting intermission
	//
	if ( client->sess.sessionState == SESS_STATE_INTERMISSION )
	{
		ClientIntermissionThink(ent);
		return;
	}

	if ( client->sess.sessionState == SESS_STATE_SPECTATOR )
	{
		SpectatorThink(ent, ucmd);
		return;
	}

	// check for inactivity timer, but never drop the local client of a non-dedicated server
	// OSP - moved here to allow for spec inactivity checks as well
	if ( !ClientInactivityTimer( client ) )
	{
		return;
	}

	// set up for pmove
	oldEventSequence = client->ps.eventSequence;

	memset( &pm, 0, sizeof( pm ) );

	pm.ps = &client->ps;
	pm.cmd = *ucmd;
	pm.oldcmd = client->sess.oldcmd;

	if ( client->ps.pm_type >= PM_DEAD )
		pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;
	else
		pm.tracemask = MASK_PLAYERSOLID;

	pm.handler = PMOVE_HANDLER_SERVER;

	VectorCopy(client->ps.origin, client->oldOrigin);

	// swap and latch button actions
	client->oldbuttons = client->buttons;
	client->buttons = client->sess.cmd.buttons;
	client->latched_buttons = client->buttons & ~client->oldbuttons;
	//----(SA)	added
	client->buttonsSinceLastFrame |= client->latched_buttons;

	// save view state
	vs.ps = &client->ps;
	vs.damageTime = client->damageTime;
	vs.time = level.time;
	vs.v_dmg_pitch = client->v_dmg_pitch;
	vs.v_dmg_roll = client->v_dmg_roll;
	vs.xyspeed = BG_GetSpeed(&client->ps, level.time);
	vs.frametime = msec * 0.001;
	vs.fLastIdleFactor = client->fLastIdleFactor;
	vs.weapIdleTime = &client->weapIdleTime;

	BG_CalculateViewAngles(&vs, angles);
	VectorAdd(client->ps.viewangles, angles, viewangles);

	weapDef = BG_GetWeaponDef(client->ps.weapon);
	BG_CalculateWeaponPosition_Sway(&client->ps, client->swayViewAngles, client->swayOffset, client->swayAngles, 1.0, msec);

	// save weapon state
	ws.ps = vs.ps;
	ws.xyspeed = vs.xyspeed;
	ws.frametime = msec * 0.001;
	VectorCopy(client->vLastMoveAng, ws.vLastMoveAng);
	ws.fLastIdleFactor = client->fLastIdleFactor;
	ws.time = vs.time;
	ws.damageTime = client->damageTime;
	ws.v_dmg_pitch = client->v_dmg_pitch;
	ws.v_dmg_roll = client->v_dmg_roll;

	VectorCopy(client->vGunOffset, ws.vGunOffset);
	VectorCopy(client->vGunSpeed, ws.vGunSpeed);
	VectorCopy(client->swayAngles, ws.swayAngles);

	ws.weapIdleTime = &client->weapIdleTime;
	BG_CalculateWeaponAngles(&ws, angles);

	if ( BG_IsAimDownSightWeapon(ws.ps->weapon) && ws.ps->fWeaponPosFrac && !weapDef->adsOverlayReticle )
	{
		AnglesToAxis(angles, vAxis);
		AnglesToAxis(viewangles, vAxis2);
		MatrixMultiply(vAxis, vAxis2, vAxis3);
		AxisToAngles(vAxis3, viewangles);
	}
	assert(!IS_NAN((viewangles)[0]) && !IS_NAN((viewangles)[1]) && !IS_NAN((viewangles)[2]));

	VectorCopy(ws.vLastMoveAng, client->vLastMoveAng);
	client->fLastIdleFactor = ws.fLastIdleFactor;

	VectorCopy(ws.vGunOffset, client->vGunOffset);
	VectorCopy(ws.vGunSpeed, client->vGunSpeed);

	client->fGunPitch = viewangles[PITCH];
	client->fGunYaw = viewangles[YAW];

	assert(!pm.mantleStarted);
	Pmove(&pm);

	if ( pm.mantleStarted )
	{
		assert(client->ps.pm_flags & PMF_MANTLE);
		G_AddPlayerMantleBlockage(pm.mantleEndPos, pm.mantleDuration, &pm);
	}

	// DHM - Nerve :: Set animMovetype to 1 if ducking
	if ( ent->client->ps.pm_flags & PMF_DUCKED )
	{
		ent->s.animMovetype = 1;
	}
	else
	{
		ent->s.animMovetype = 0;
	}

	// save results of pmove
	if ( ent->client->ps.eventSequence != oldEventSequence )
	{
		ent->eventTime = level.time;
		ent->r.eventTime = level.time;
	}

	// Ridah, fixes jittery zombie movement
	if ( g_smoothClients->current.boolean )
		G_PlayerStateToEntityStateExtrapolate(&ent->client->ps, &ent->s, ent->client->ps.commandTime, qtrue);
	else
		BG_PlayerStateToEntityState(&ent->client->ps, &ent->s, qtrue, PMOVE_HANDLER_SERVER);

//
//	// use the precise origin for linking
//	VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
//
//	// use the snapped origin for linking so it matches client predicted versions
	VectorCopy(ent->s.pos.trBase, ent->r.currentOrigin);

	VectorCopy(pm.mins, ent->r.mins);
	VectorCopy(pm.maxs, ent->r.maxs);

	// execute client events
	ClientEvents(ent, oldEventSequence);

	// link entity now, after any personal teleporters have been used
	SV_LinkEntity(ent);
	if ( !ent->client->noclip )
	{
		G_TouchTriggers(ent);
	}

// NOTE: now copy the exact origin over otherwise clients can be snapped into solid
	VectorCopy(ent->client->ps.origin, ent->r.currentOrigin);

	VectorClear(ent->r.currentAngles);
	ent->r.currentAngles[YAW] = ent->client->ps.viewangles[YAW];

	// touch other objects
	ClientImpacts(ent, &pm);

	// save results of triggers and client events
	if ( ent->client->ps.eventSequence != oldEventSequence )
	{
		ent->eventTime = level.time;
	}

	// perform once-a-second actions
	Player_UpdateActivate(ent);
}

/*
==============
ClientEndFrame

Called at the end of each server frame for each connected client
A fast client will have multiple ClientThink for each ClientEndFrame,
while a slow client may have multiple ClientEndFrame between ClientThink.
==============
*/
void ClientEndFrame( gentity_t *ent )
{
	DObj *obj;
	vec3_t spawn_angles;
	vec3_t spawn_origin;
	gclient_t *client;
	qboolean bChanged;
	int clientNum;
	clientInfo_t *ci;
	vec3_t vViewPos;

	assert(ent);
	client = ent->client;
	assert(client);
	assert(client->sess.connected != CON_DISCONNECTED);

	ent->handler = ENT_HANDLER_CLIENT_SPECTATOR;
	client->ps.deltaTime = 0;

	bChanged = G_UpdateClientInfo(ent);

	if ( client->sess.connected != CON_CONNECTED )
	{
		ent->client->buttonsSinceLastFrame = 0;
		return;
	}

	if ( client->sess.sessionState == SESS_STATE_INTERMISSION )
	{
		IntermissionClientEndFrame(ent);
		ent->client->buttonsSinceLastFrame = 0;
		return;
	}

	if ( client->sess.sessionState == SESS_STATE_SPECTATOR )
	{
		SpectatorClientEndFrame(ent);
		ent->client->buttonsSinceLastFrame = 0;
		return;
	}

	if ( client->ps.clientNum != ent->s.number )
	{
		VectorCopy(client->ps.origin, spawn_origin);
		VectorSet(spawn_angles, 0, client->ps.viewangles[YAW], 0);

		ClientSpawn(ent, spawn_origin, spawn_angles);
		ent->client->buttonsSinceLastFrame = 0;
		return;
	}

	ent->r.svFlags |= SVF_BODY;
	ent->r.svFlags &= ~SVF_NOCLIENT;

	ent->takedamage = qtrue;

	client->ps.pm_flags |= PMF_PLAYER;
	client->ps.pm_flags &= ~(PMF_SPECTATOR_FREE | PMF_SPECTATOR_FOLLOW);

	client->ps.viewmodelIndex = client->sess.viewmodelIndex;

	G_SetClientContents(ent);

	client->dropWeaponTime = 0;

	if ( client->compassPingTime <= level.time )
	{
		client->ps.eFlags &= ~EF_TAUNT;
	}

	if ( client->noclip )
	{
		client->ps.pm_type = PM_NOCLIP;
	}
	else if ( client->ufo )
	{
		client->ps.pm_type = PM_UFO;
	}
	else if ( client->sess.sessionState == SESS_STATE_DEAD )
	{
		if ( ent->tagInfo )
		{
			client->ps.pm_type = PM_DEAD_LINKED;
		}
		else
		{
			client->ps.pm_type = PM_DEAD;
		}

		ent->r.svFlags |= SVF_NOCLIENT;
		ent->r.svFlags &= ~SVF_BODY;

		ent->takedamage = 0;
	}
	else if ( ent->tagInfo )
	{
		client->ps.pm_type = PM_NORMAL_LINKED;
	}
	else
	{
		client->ps.pm_type = PM_NORMAL;
	}

	client->ps.gravity = (int)g_gravity->current.decimal;
	client->ps.speed = g_speed->current.integer;

#if LIBCOD_COMPILE_PLAYER == 1
	int num = client->ps.clientNum;
	extern int player_g_gravity[];
	if (player_g_gravity[num] > 0)
		client->ps.gravity = player_g_gravity[num];
	extern int player_g_speed[];
	if (player_g_speed[num] > 0)
		client->ps.speed = player_g_speed[num];
#endif

	client->currentAimSpreadScale = client->ps.aimSpreadScale / 255;

	Player_UpdateLookAtEntity(ent);
	Player_UpdateCursorHints(ent);

	// apply all the damage taken this frame
	P_DamageFeedback(ent);

	// add the EF_CONNECTION flag if we haven't gotten commands recently
	if ( level.time - ent->client->lastCmdTime > 1000 )
	{
		ent->s.eFlags |= EF_CONNECTION;
	}
	else
	{
		ent->s.eFlags &= ~EF_CONNECTION;
	}

	ent->client->ps.stats[STAT_HEALTH] = ent->health;   // FIXME: get rid of ent->health...
	// Gordon: WHY? other ents use it.

	G_SetClientSound( ent );

	// set the latest infor

	// Ridah, fixes jittery zombie movement
	if ( g_smoothClients->current.boolean )
		G_PlayerStateToEntityStateExtrapolate(&client->ps, &ent->s, client->ps.commandTime, qtrue);
	else
		BG_PlayerStateToEntityState(&client->ps, &ent->s, qtrue, PMOVE_HANDLER_SERVER);

	if ( ent->health > 0 && StuckInClient( ent ) )
	{
		//G_DPrintf( "%s is stuck in a client.\n", ent->client->pers.netname );
		ent->r.contents = CONTENTS_CORPSE;
	}

	// store the client's current position for antilag traces
	G_GetPlayerViewOrigin(ent, vViewPos);

	client->ps.iCompassFriendInfo = G_GetNonPVSFriendlyInfo(ent, vViewPos, client->iLastCompassFriendlyInfoEnt);
	assert(COMPFR_GET_INDEX( client->ps.iCompassFriendInfo ) >= 0 && COMPFR_GET_INDEX( client->ps.iCompassFriendInfo ) < MAX_CLIENTS);

	if ( client->ps.iCompassFriendInfo )
	{
		client->iLastCompassFriendlyInfoEnt = COMPFR_GET_INDEX( client->ps.iCompassFriendInfo );

		if ( g_entities[client->iLastCompassFriendlyInfoEnt].s.eFlags & EF_TAUNT )
		{
			client->ps.eFlags |= EF_PING;
		}
		else
		{
			client->ps.eFlags &= ~EF_PING;
		}
	}
	else
	{
		client->iLastCompassFriendlyInfoEnt = ENTITYNUM_NONE;
	}

	if ( ent->s.eType != ET_PLAYER )
	{
		ent->client->buttonsSinceLastFrame = 0;
		return;
	}

	if ( ent->health > 0 )
		ent->handler = ENT_HANDLER_CLIENT;
	else
		ent->handler = ENT_HANDLER_CLIENT_DEAD;

	clientNum = ent->s.clientNum;
	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);

	ci = &level_bgs.clientinfo[clientNum];
	assert(ci->infoValid);

	ci->lerpMoveDir = ent->s.angles2[YAW];
	ci->lerpLean = ent->s.leanf;

	VectorCopy(client->ps.viewangles, ci->playerAngles);

	if ( bChanged )
	{
		G_SafeDObjFree(ent);
	}

	obj = Com_GetServerDObj(ent->s.number);

	BG_UpdatePlayerDObj(obj, &ent->s, ci, ent->attachIgnoreCollision);
	BG_PlayerAnimation(obj, &ent->s, ci);

	if ( client->ps.pm_flags & PMF_PLAYER && client->ps.eFlags & EF_TURRET_ACTIVE )
	{
		assert(client->ps.clientNum == ent->s.number);
		assert(client->ps.viewlocked_entNum != ENTITYNUM_NONE);
		turret_think_client(&level.gentities[client->ps.viewlocked_entNum]);
	}

	if ( g_debugLocDamage->current.boolean )
	{
		if ( SV_DObjExists(ent) )
		{
			G_DObjCalcPose(ent);
			SV_XModelDebugBoxes();
		}
	}

	ent->client->buttonsSinceLastFrame = 0;
}

/*
==================
G_RunClient
==================
*/
void G_RunClient( gentity_t *ent )
{
	assert(ent->client);
	assert(ent->client->sess.connected != CON_DISCONNECTED);

	if ( g_synchronousClients->current.boolean )
	{
		ent->client->sess.cmd.serverTime = level.time;
		ClientThink_real(ent, &ent->client->sess.cmd);
	}

	if ( ent->client->noclip )
	{
		return;
	}

	// link/unlink func
	if ( ent->tagInfo )
	{
		if ( ent->client->sess.sessionState == SESS_STATE_DEAD )
			ent->client->ps.pm_type = PM_DEAD_LINKED;
		else
			ent->client->ps.pm_type = PM_NORMAL_LINKED;

		G_SetFixedLink(ent, FIXED_LINK_ANGLES_NONE);

		G_SetOrigin(ent, ent->r.currentOrigin);
		G_SetAngle(ent, ent->r.currentAngles);

		ent->s.pos.trType = TR_INTERPOLATE;
		ent->s.apos.trType = TR_INTERPOLATE;

		SV_LinkEntity(ent);

		VectorCopy(ent->r.currentOrigin, ent->client->ps.origin);
	}
	else if ( ent->client->ps.pm_type == PM_NORMAL_LINKED )
	{
		ent->client->ps.pm_type == PM_NORMAL;
	}
	else if ( ent->client->ps.pm_type == PM_DEAD_LINKED )
	{
		ent->client->ps.pm_type == PM_DEAD;
	}
}

/*
==================
ClientThink

A new command has arrived from the client
==================
*/
void ClientThink( int clientNum )
{
	gentity_t *ent;

	ent = &g_entities[clientNum];
	assert(ent->client);
	ent->client->sess.oldcmd = ent->client->sess.cmd;
	SV_GetUsercmd(clientNum, &ent->client->sess.cmd);

	// mark the time we got info, so we can display the
	// phone jack if they don't get any for a while
	ent->client->lastCmdTime = level.time;

	if ( g_synchronousClients->current.boolean )
	{
		return;
	}

	ClientThink_real(ent, &ent->client->sess.cmd);
}

/*
========================
G_PlayerStateToEntityStateExtrapolate

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void G_PlayerStateToEntityStateExtrapolate( playerState_t *ps, entityState_t *s, int time, qboolean snap )
{
	s->pos.trType = TR_LINEAR_STOP;
	VectorCopy( ps->origin, s->pos.trBase );
	// set the trDelta for flag direction and linear prediction
	VectorCopy( ps->velocity, s->pos.trDelta );
	// set the time for linear prediction
	s->pos.trTime = time;
	// set maximum extra polation time
	s->pos.trDuration = 50; // 1000 / sv_fps (default = 20)

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );

	s->angles2[YAW] = (float)ps->movementDir;
	s->eFlags = ps->eFlags;
// from MP
	if ( ps->entityEventSequence - ps->eventSequence < 0 )
	{
		int seq;

		if ( ps->eventSequence - ps->entityEventSequence > MAX_EVENTS )
		{
			ps->entityEventSequence = ps->eventSequence - MAX_EVENTS;
		}
		seq = ps->entityEventSequence & ( MAX_EVENTS - 1 );
		s->eventParm = ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}
	else
	{
		s->eventParm = 0;
	}
// end
	if ( ps->oldEventSequence - ps->eventSequence > 0 )
	{
		ps->oldEventSequence = ps->eventSequence;
	}

	// Ridah, now using a circular list of events for all entities
	// add any new events that have been added to the playerState_t
	// (possibly overwriting entityState_t events)
	int i, j;
	for ( i = ps->oldEventSequence; i != ps->eventSequence; i++ )
	{
		int event = ps->events[i & ( MAX_EVENTS - 1 )];

		G_PlayerEvent(s->number, event);

		for ( j = 0; singleClientEvents[j] > 0; j++ )
		{
			if ( singleClientEvents[j] == event )
			{
				break;
			}
		}

		if ( singleClientEvents[j] >= 0 )
		{
			continue;
		}

		s->events[s->eventSequence & ( MAX_EVENTS - 1 )] = event;
		s->eventParms[s->eventSequence & ( MAX_EVENTS - 1 )] = ps->eventParms[i & ( MAX_EVENTS - 1 )];
		s->eventSequence++;
	}
	ps->oldEventSequence = ps->eventSequence;

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	if ( ps->pm_flags & ( PMF_FOLLOW | PMF_PLAYER ) ) // JPW NERVE limbo
		s->eType = ET_PLAYER;
	else
		s->eType = ET_INVISIBLE;

	if ( snap )
	{
		SnapVector( s->pos.trBase );
		SnapVector( s->apos.trBase );
	}

	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->clientNum = ps->clientNum;

	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		s->otherEntityNum = ps->viewlocked_entNum;
	}

	if ( ps->pm_type >= PM_DEAD )
	{
		s->eFlags |= EF_DEAD;
	}
	else
	{
		s->eFlags &= ~EF_DEAD;
	}

	if ( ps->pm_flags & PMF_ADS )
	{
		s->eFlags |= EF_AIMDOWNSIGHT;
	}
	else
	{
		s->eFlags &= ~EF_AIMDOWNSIGHT;
	}

	s->leanf = ps->leanf;

	if ( PM_GetEffectiveStance(ps) == PM_EFF_STANCE_PRONE )
	{
		float fLerpFrac = (ps->commandTime - ps->viewHeightLerpTime) / PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);

		if ( fLerpFrac < 0 )
		{
			fLerpFrac = 0;
		}
		else
		{
			if ( fLerpFrac > 1.0 )
			{
				fLerpFrac = 1.0;
			}
		}

		if ( !ps->viewHeightLerpDown )
		{
			fLerpFrac = 1.0 - fLerpFrac;
		}

		s->fTorsoHeight = ps->fTorsoHeight * fLerpFrac;
		s->fTorsoPitch = AngleNormalize180(ps->fTorsoPitch) * fLerpFrac;
		s->fWaistPitch = AngleNormalize180(ps->fWaistPitch) * fLerpFrac;
	}
	else
	{
		s->fTorsoHeight = 0;
		s->fTorsoPitch = 0;
		s->fWaistPitch = 0;
	}
}

/*
==================
G_UpdateClientInfo
==================
*/
qboolean G_UpdateClientInfo( gentity_t *ent )
{
	gclient_t *client;
	qboolean bChanged;
	const char *tagName;
	const char *modelName;
	clientInfo_t *ci;
	int i;

	client = ent->client;
	assert(client);
	assert(client->sess.connected != CON_DISCONNECTED);

	ci = &level_bgs.clientinfo[ent->s.clientNum];
	//assert(ci->infoValid);
	bChanged = qfalse;

	modelName = G_ModelName(ent->model);
	client->sess.state.modelindex = ent->model;

	if ( strcmp(ci->model, modelName) )
	{
		bChanged = qtrue;
		I_strncpyz(ci->model, modelName, sizeof(ci->model));
	}

	for ( i = 0; i < 6; i++ )
	{
		if ( ent->attachModelNames[i] )
		{
			modelName = G_ModelName(ent->attachModelNames[i]);
			client->sess.state.attachModelIndex[i] = ent->attachModelNames[i];

			if ( strcmp(ci->attachModelNames[i], modelName) )
			{
				bChanged = qtrue;
				I_strncpyz(ci->attachModelNames[i], modelName, sizeof(ci->attachModelNames[i]));
			}

			assert(ent->attachTagNames[i]);
			tagName = SL_ConvertToString(ent->attachTagNames[i]);
			client->sess.state.attachTagIndex[i] = G_TagIndex(tagName);

			if ( strcmp(ci->attachTagNames[i], tagName) )
			{
				bChanged = qtrue;
				I_strncpyz(ci->attachTagNames[i], tagName, sizeof(ci->attachTagNames[i]));
			}
		}
		else
		{
			ci->attachModelNames[i][0] = 0;
			ci->attachTagNames[i][0] = 0;

			client->sess.state.attachModelIndex[i] = 0;
			client->sess.state.attachTagIndex[i] = 0;
		}
	}

	return bChanged;
}

/*
==================
G_AddPlayerMantleBlockage
==================
*/
void G_AddPlayerMantleBlockage( vec3_t endPos, int duration, pmove_t *pm )
{
	gentity_s *owner;
	gentity_s *ent;

#ifdef LIBCOD
	if ( !g_mantleBlockEnable->current.boolean )
	{
		return;
	}
#endif

	owner = &g_entities[pm->ps->clientNum];

	ent = G_Spawn();

	ent->parent = owner;
	ent->r.ownerNum = pm->ps->clientNum;
	ent->r.contents = CONTENTS_PLAYERCLIP;
	ent->clipmask = CONTENTS_PLAYERCLIP;
	ent->r.svFlags = SVF_NOCLIENT | SVF_RADIUS;
	ent->s.eType = ET_INVISIBLE;
	ent->handler = ENT_HANDLER_PLAYER_BLOCK;

	VectorCopy(owner->r.mins, ent->r.mins);
	VectorCopy(owner->r.maxs, ent->r.maxs);

	G_SetOrigin(ent, endPos);

	SV_LinkEntity(ent);

	ent->nextthink = g_mantleBlockTimeBuffer->current.integer + level.time + duration;
}
