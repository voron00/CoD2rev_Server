#include "../qcommon/qcommon.h"
#include "bg_public.h"

bgs_t level_bgs;

dvar_t *player_view_pitch_up;
dvar_t *player_view_pitch_down;
dvar_t *bg_ladder_yawcap;
dvar_t *bg_prone_yawcap;
dvar_t *bg_foliagesnd_minspeed;
dvar_t *bg_foliagesnd_maxspeed;
dvar_t *bg_foliagesnd_slowinterval;
dvar_t *bg_foliagesnd_fastinterval;
dvar_t *bg_foliagesnd_resetinterval;
dvar_t *bg_fallDamageMinHeight;
dvar_t *bg_fallDamageMaxHeight;
dvar_t *inertiaMax;
dvar_t *inertiaDebug;
dvar_t *inertiaAngle;
dvar_t *friction;
dvar_t *stopspeed;
dvar_t *bg_swingSpeed;
dvar_t *bg_bobAmplitudeStanding;
dvar_t *bg_bobAmplitudeDucked;
dvar_t *bg_bobAmplitudeProne;
dvar_t *bg_bobMax;
dvar_t *bg_aimSpreadMoveSpeedThreshold;
dvar_t *player_breath_hold_time;
dvar_t *player_breath_gasp_time;
dvar_t *player_breath_fire_delay;
dvar_t *player_breath_gasp_scale;
dvar_t *player_breath_hold_lerp;
dvar_t *player_breath_gasp_lerp;
dvar_t *player_breath_snd_lerp;
dvar_t *player_breath_snd_delay;
dvar_t *player_toggleBinoculars;
dvar_t *player_scopeExitOnDamage;
dvar_t *player_adsExitDelay;
dvar_t *player_moveThreshhold;
dvar_t *player_footstepsThreshhold;
dvar_t *player_strafeSpeedScale;
dvar_t *player_backSpeedScale;
dvar_t *player_spectateSpeedScale;
dvar_t *player_turnAnims;
dvar_t *player_dmgtimer_timePerPoint;
dvar_t *player_dmgtimer_maxTime;
dvar_t *player_dmgtimer_minScale;
dvar_t *player_dmgtimer_stumbleTime;
dvar_t *player_dmgtimer_flinchTime;

gitem_t bg_itemlist[] =
{
	{ NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ NULL, "", "", "", "", "", 0, 0, 0, 0, 0 },
	{ "item_health_small", "health_pickup_small", "xmodel/health_small", NULL, "icons/iconh_small", "Small Health", 10, 3, 0, 0, 0 },
	{ "item_health_large", "health_pickup_large", "xmodel/health_large", NULL, "icons/iconh_large", "Large Health", 50, 3, 0, 0, 0 },
	{ NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0 },
};

gitem_s* BG_FindItemForWeapon(int weapon)
{
	return &bg_itemlist[weapon];
}

void BG_StringCopy(unsigned char *member, const char *keyValue)
{
	strcpy((char *)member, keyValue);
}

qboolean BG_CanItemBeGrabbed(entityState_s *ent, playerState_s *ps, int touched)
{
	int giType;
	gitem_s *item;

	if ( ent->index <= 0 || ent->index >= 131 )
	{
		Com_Error(ERR_DROP, va("BG_CanItemBeGrabbed: index out of range (index is %i, eType is %i)", ent->index, ent->eType));
	}

	item = &bg_itemlist[ent->index];

	if ( ent->clientNum != ps->clientNum )
	{
		giType = item->giType;

		if ( giType != IT_WEAPON )
		{
			if ( giType > IT_WEAPON )
			{
				if ( giType == IT_AMMO )
				{
					if ( Com_BitCheck(ps->weapons, item->giTag) )
					{
						if ( BG_GetMaxPickupableAmmo(ps, item->giTag) <= 0 )
							return 0;
					}
					else if ( !BG_WeaponIsClipOnly(item->giTag) || BG_GetMaxPickupableAmmo(ps, item->giTag) <= 0 )
					{
						return 0;
					}
					return 1;
				}

				if ( giType == IT_HEALTH )
					return ps->stats[0] < ps->stats[2];
			}
			else if ( giType == IT_BAD )
			{
				Com_Error(ERR_DROP, "BG_CanItemBeGrabbed: IT_BAD");
			}
			return 0;
		}

		if ( BG_DoesWeaponNeedSlot(item->giTag) && !Com_BitCheck(ps->weapons, item->giTag) )
		{
			if ( touched || BG_GetMaxPickupableAmmo(ps, item->giTag) <= 0 )
				return 0;
		}
		else if ( BG_GetMaxPickupableAmmo(ps, item->giTag) <= 0 )
		{
			return 0;
		}

		return 1;
	}

	return 0;
}

/*
================
BG_AddPredictableEventToPlayerstate
================
*/
void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps )
{
	if ( !newEvent )
		return;

	ps->events[ps->eventSequence & ( MAX_EVENTS - 1 )] = newEvent;
	ps->eventParms[ps->eventSequence & ( MAX_EVENTS - 1 )] = eventParm;
	ps->eventSequence++;
}

/*
================
BG_EvaluateTrajectory
================
*/
void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result )
{
	float deltaTime;
	float phase;
	vec3_t v;

	switch ( tr->trType )
	{
	case TR_STATIONARY:
	case TR_INTERPOLATE:
	case TR_GRAVITY_PAUSED: //----(SA)
		VectorCopy( tr->trBase, result );
		break;
	case TR_LINEAR:
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration )
		{
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		if ( deltaTime < 0 )
		{
			deltaTime = 0;
		}
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = sin( deltaTime * M_PI * 2 );
		VectorMA( tr->trBase, phase, tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;     // FIXME: local gravity...
		break;
	// RF, acceleration
	case TR_ACCELERATE:     // trDelta is the ultimate speed
		if ( atTime > tr->trTime + tr->trDuration )
		{
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		// phase is the acceleration constant
		phase = VectorLength( tr->trDelta ) / ( tr->trDuration * 0.001 );
		// trDelta at least gives us the acceleration direction
		Vec3NormalizeTo( tr->trDelta, result );
		// get distance travelled at current time
		VectorMA( tr->trBase, phase * 0.5 * deltaTime * deltaTime, result, result );
		break;
	case TR_DECCELERATE:    // trDelta is the starting speed
		if ( atTime > tr->trTime + tr->trDuration )
		{
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		// phase is the breaking constant
		phase = VectorLength( tr->trDelta ) / ( tr->trDuration * 0.001 );
		// trDelta at least gives us the acceleration direction
		Vec3NormalizeTo( tr->trDelta, result );
		// get distance travelled at current time (without breaking)
		VectorMA( tr->trBase, deltaTime, tr->trDelta, v );
		// subtract breaking force
		VectorMA( v, -phase * 0.5 * deltaTime * deltaTime, result, result );
		break;
	default:
		Com_Error( ERR_DROP, "BG_EvaluateTrajectory: unknown trType: %i", tr->trTime );
		break;
	}
}

/*
================
BG_EvaluateTrajectoryDelta
For determining velocity at a given time
================
*/
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result)
{
	float deltaTime;
	float phase;

	switch ( tr->trType )
	{
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorClear( result );
		break;
	case TR_LINEAR:
		VectorCopy( tr->trDelta, result );
		break;
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration )
		{
			VectorClear( result );
			return;
		}
		VectorCopy( tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = cos( deltaTime * M_PI * 2 );    // derivative of sin = cos
		phase *= 0.5;
		VectorScale( tr->trDelta, phase, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		VectorCopy( tr->trDelta, result );
		result[2] -= DEFAULT_GRAVITY * deltaTime;       // FIXME: local gravity...
		break;
	// RF, acceleration
	case TR_ACCELERATE: // trDelta is eventual speed
		if ( atTime > tr->trTime + tr->trDuration )
		{
			VectorClear( result );
			return;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		phase = deltaTime / (float)tr->trDuration;
		VectorScale( tr->trDelta, deltaTime * deltaTime, result );
		break;
	case TR_DECCELERATE:    // trDelta is breaking force
		if ( atTime > tr->trTime + tr->trDuration )
		{
			VectorClear( result );
			return;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		VectorScale( tr->trDelta, deltaTime, result );
		break;
	default:
		Com_Error( ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trTime );
		break;
	}
}

/*
================
BG_CreateRotationMatrix
================
*/
void BG_CreateRotationMatrix( const vec3_t angles, vec3_t matrix[3] )
{
	AngleVectors( angles, matrix[0], matrix[1], matrix[2] );
	VectorInverse( matrix[1] );
}

/*
================
BG_TransposeMatrix
================
*/
void BG_TransposeMatrix( const vec3_t matrix[3], vec3_t transpose[3] )
{
	int i, j;
	for ( i = 0; i < 3; i++ )
	{
		for ( j = 0; j < 3; j++ )
		{
			transpose[i][j] = matrix[j][i];
		}
	}
}

/*
================
BG_RotatePoint
================
*/
void BG_RotatePoint( vec3_t point, const vec3_t matrix[3] )
{
	vec3_t tvec;

	VectorCopy( point, tvec );
	point[0] = DotProduct( matrix[0], tvec );
	point[1] = DotProduct( matrix[1], tvec );
	point[2] = DotProduct( matrix[2], tvec );
}

void BG_PlayerStateToEntityState(playerState_s *ps, entityState_s *s, int snap, byte handler)
{
	int flags;
	byte event;
	int j;
	float lerpTime;
	void (*playerEvent)(int, int);
	int i;

	if ( (ps->pm_flags & 0xC00000) != 0 )
		s->eType = ET_PLAYER;
	else
		s->eType = ET_INVISIBLE;

	s->pos.trType = TR_INTERPOLATE;
	VectorCopy(ps->origin, s->pos.trBase);

	if ( snap )
	{
		s->pos.trBase[0] = (float)(int)s->pos.trBase[0];
		s->pos.trBase[1] = (float)(int)s->pos.trBase[1];
		s->pos.trBase[2] = (float)(int)s->pos.trBase[2];
	}

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy(ps->viewangles, s->apos.trBase);

	if ( snap )
	{
		s->apos.trBase[0] = (float)(int)s->apos.trBase[0];
		s->apos.trBase[1] = (float)(int)s->apos.trBase[1];
		s->apos.trBase[2] = (float)(int)s->apos.trBase[2];
	}

	s->angles2[1] = (float)ps->movementDir;
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->clientNum = ps->clientNum;
	s->eFlags = ps->eFlags;

	if ( (ps->eFlags & 0x300) != 0 )
		s->otherEntityNum = ps->viewlocked_entNum;

	if ( ps->pm_type <= PM_INTERMISSION )
		flags = s->eFlags & 0xFFFDFFFF;
	else
		flags = s->eFlags | 0x20000;

	s->eFlags = flags;

	if ( (ps->pm_flags & 0x40) != 0 )
		flags = s->eFlags | 0x40000;
	else
		flags = s->eFlags & 0xFFFBFFFF;

	s->eFlags = flags;
	s->leanf = ps->leanf;

	if ( PM_GetEffectiveStance(ps) == 1 )
	{
		if ( ps->viewHeightLerpTime )
		{
			lerpTime = (float)(ps->commandTime - ps->viewHeightLerpTime)
			           / (float)PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);

			if ( lerpTime >= 0.0 )
			{
				if ( lerpTime > 1.0 )
					lerpTime = 1.0;
			}
			else
			{
				lerpTime = 0.0;
			}

			if ( !ps->viewHeightLerpDown )
				lerpTime = 1.0 - lerpTime;
		}
		else
		{
			lerpTime = 1.0;
		}

		s->fTorsoHeight = ps->fTorsoHeight * lerpTime;
		s->fTorsoPitch = AngleNormalize180(ps->fTorsoPitch) * lerpTime;
		s->fWaistPitch = AngleNormalize180(ps->fWaistPitch) * lerpTime;
	}
	else
	{
		s->fTorsoHeight = 0.0;
		s->fTorsoPitch = 0.0;
		s->fWaistPitch = 0.0;
	}

	if ( ps->entityEventSequence - ps->eventSequence >= 0 )
	{
		s->eventParm = 0;
	}
	else
	{
		if ( ps->eventSequence - ps->entityEventSequence > 4 )
			ps->entityEventSequence = ps->eventSequence - 4;

		s->eventParm = LOBYTE(ps->eventParms[ps->entityEventSequence++ & 3]);
	}

	for ( i = ps->oldEventSequence; i != ps->eventSequence; ++i )
	{
		event = ps->events[i & 3];
		playerEvent = pmoveHandlers[handler].playerEvent;

		if ( playerEvent )
			playerEvent(s->number, event);

		for ( j = 0; singleClientEvents[j] > 0 && singleClientEvents[j] != event; ++j )
			;

		if ( singleClientEvents[j] < 0 )
		{
			s->events[s->eventSequence & 3] = event;
			s->eventParms[s->eventSequence++ & 3] = LOBYTE(ps->eventParms[i & 3]);
		}
	}

	ps->oldEventSequence = ps->eventSequence;
	s->weapon = LOBYTE(ps->weapon);
	s->groundEntityNum = LOWORD(ps->groundEntityNum);
}

qboolean BG_PlayerTouchesItem(const playerState_s *ps, const entityState_s *item, int atTime)
{
	vec3_t origin;

	BG_EvaluateTrajectory(&item->pos, atTime, origin);
	return ps->origin[0] - origin[0] <= 36.0
	       && ps->origin[0] - origin[0] >= -36.0
	       && ps->origin[1] - origin[1] <= 36.0
	       && ps->origin[1] - origin[1] >= -36.0
	       && ps->origin[2] - origin[2] <= 18.0
	       && ps->origin[2] - origin[2] >= -88.0;
}

int BG_CheckProneValid(int passEntityNum, const float *const vPos, float fSize, float fHeight, float fYaw, float *pfTorsoHeight, float *pfTorsoPitch, float *pfWaistPitch, int bAlreadyProne, int bOnGround, float *const vGroundNormal, unsigned char handler, int proneCheckType, float prone_feet_dist)
{
	float feetPitch;
	float feetPosPitch;
	float proneFeedScale;
	float feetDist;
	float yawDist;
	char bLanded;
	void (*traceFunc)(trace_t *, const float *, const float *, const float *, const float *, int, int);
	float frac;
	int bFirstTraceHit;
	float feetFrac;
	float feetHeight;
	float normalPos;
	float dist;
	float angle1;
	float angle2;
	vec3_t vEnd;
	vec3_t vWaistPos;
	vec3_t vEndPos;
	vec3_t vUp;
	vec3_t vRight;
	vec3_t vForward;
	vec3_t vFeetPos;
	vec3_t maxs;
	vec3_t mins;
	vec3_t angles;
	vec3_t start;
	trace_t trace;
	int contentMask;

	bFirstTraceHit = 0;
	bLanded = 1;

	traceFunc = pmoveHandlers[handler].trace;

	VectorSet(mins, -fSize, -fSize, 0.0);
	VectorAdd(mins, vPos, mins);
	VectorSet(maxs, fSize, fSize, fHeight);
	VectorAdd(maxs, vPos, maxs);

	if ( proneCheckType )
		contentMask = 8519697;
	else
		contentMask = 8454161;

	if ( !bAlreadyProne )
	{
		VectorSet(mins, -fSize, -fSize, 0.0);
		VectorSet(maxs, fSize, fSize, fHeight);
		VectorCopy(vPos, start);
		VectorCopy(vPos, angles);
		angles[2] = angles[2] + 10.0;

		traceFunc(&trace, start, mins, maxs, angles, passEntityNum, contentMask);

		if ( trace.allsolid )
			return 0;
	}

	if ( bOnGround && vGroundNormal && vGroundNormal[2] < 0.69999999 )
		return 0;

	VectorSet(mins, -6.0, -6.0, -6.0);
	VectorSet(maxs, 6.0, 6.0, 6.0);
	yawDist = fYaw - 180.0;
	VectorSet(angles, 0.0, yawDist, 0.0);
	AngleVectors(angles, vForward, vRight, vUp);
	feetHeight = fHeight - 6.0;
	VectorCopy(vPos, start);
	start[2] = start[2] + feetHeight;
	proneFeedScale = prone_feet_dist - 6.0;
	VectorMA(start, proneFeedScale, vForward, angles);
	traceFunc(&trace, start, mins, maxs, angles, passEntityNum, contentMask);

	if ( trace.fraction >= 1.0 )
	{
		feetFrac = prone_feet_dist;
	}
	else
	{
		if ( !bOnGround )
			return 0;

		bFirstTraceHit = 1;
		feetFrac = (prone_feet_dist - 6.0) * trace.fraction + 6.0;

		if ( fSize + 2.0 > feetFrac )
			return 0;

		if ( feetHeight * 0.69999999 + 48.0 > feetFrac )
		{
			bFirstTraceHit = 0;
			angles[2] = angles[2] + 22.0;
			VectorSubtract(angles, start, vFeetPos);
			normalPos = Vec3NormalizeTo(vFeetPos, vForward);

			traceFunc(&trace, start, mins, maxs, angles, passEntityNum, contentMask);

			if ( trace.fraction >= 1.0 )
			{
				feetFrac = prone_feet_dist;
			}
			else
			{
				bFirstTraceHit = 1;
				feetFrac = trace.fraction * normalPos + 6.0;

				if ( feetHeight * 0.69999999 + 48.0 > feetFrac )
					return 0;
			}
		}
	}

	Vec3Lerp(start, angles, trace.fraction, vEnd);
	VectorMA(vPos, 48.0, vForward, start);
	start[2] = start[2] + feetHeight;
	VectorCopy(start, angles);
	angles[2] = angles[2] - (fSize * 2.5 + feetHeight - 6.0);

	traceFunc(&trace, start, mins, maxs, angles, passEntityNum, contentMask);

	if ( trace.fraction != 1.0 )
	{
		if ( trace.normal[2] < 0.69999999 )
			return 0;

		frac = (fSize * 2.5 + feetHeight - 6.0) * trace.fraction + 6.0;
		Vec3Lerp(start, angles, trace.fraction, vWaistPos);
		vWaistPos[2] = vWaistPos[2] - 6.0;

		if ( !bFirstTraceHit )
			goto LABEL_30;

		if ( frac * -0.75 <= feetFrac - frac )
		{
			VectorSubtract(vEnd, vWaistPos, vFeetPos);
			VectorMA(vFeetPos, 6.0, vForward, vFeetPos);
			vFeetPos[2] = vFeetPos[2] + 6.0;
			Vec3Normalize(vFeetPos);
			feetDist = prone_feet_dist - 6.0 - 48.0;
			VectorMA(start, feetDist, vFeetPos, angles);
			angles[0] = ((prone_feet_dist - 6.0) * vForward[0] + vPos[0] + angles[0]) * 0.5;
			angles[1] = ((prone_feet_dist - 6.0) * vForward[1] + vPos[1] + angles[1]) * 0.5;

			traceFunc(&trace, start, mins, maxs, angles, passEntityNum, contentMask);

			if ( trace.fraction >= 1.0
			        || (Vec3Lerp(start, angles, trace.fraction, start),
			            start[2] = start[2] + 18.0,
			            angles[2] = angles[2] + 18.0,
			            traceFunc(&trace, start, mins, maxs, angles, passEntityNum, contentMask),
			            trace.fraction >= 1.0) )
			{
				Vec3Lerp(start, angles, trace.fraction, vEnd);
LABEL_30:
				VectorCopy(vEnd, start);
				VectorCopy(start, angles);
				angles[2] = angles[2] - (start[2] - vWaistPos[2] + start[2] - vWaistPos[2] + fSize);
				traceFunc(&trace, start, mins, maxs, angles, passEntityNum, contentMask);

				if ( trace.fraction != 1.0 )
				{
					if ( trace.normal[2] < 0.69999999 )
						return 0;

					Vec3Lerp(start, angles, trace.fraction, vEnd);
					vEnd[2] = vEnd[2] - 6.0;
					VectorCopy(vPos, vEndPos);
					VectorSubtract(vWaistPos, vEndPos, vFeetPos);
					angle2 = vectopitch(vFeetPos);
					VectorSubtract(vEnd, vWaistPos, vFeetPos);
					angle1 = vectopitch(vFeetPos);
					dist = AngleSubtract(angle1, angle2);

					if ( dist < -50.0 || dist > 70.0 )
						bLanded = 0;

					VectorSet(mins, -0.0, -0.0, -0.0);
					VectorSet(maxs, 0.0, 0.0, 0.0);
					VectorCopy(vEndPos, start);
					start[2] = start[2] + 5.0;
					VectorCopy(vWaistPos, angles);
					angles[2] = angles[2] + 5.0;
					traceFunc(&trace, start, mins, maxs, angles, passEntityNum, contentMask);

					if ( trace.fraction < 1.0 )
						bLanded = 0;

					VectorCopy(angles, start);
					VectorCopy(vEnd, angles);
					angles[2] = angles[2] + 5.0;

					traceFunc(&trace, start, mins, maxs, angles, passEntityNum, contentMask);

					if ( trace.fraction < 1.0 )
						bLanded = 0;

					if ( pfTorsoHeight )
						*pfTorsoHeight = 0.0;

					if ( pfTorsoPitch )
					{
						VectorSubtract(vEndPos, vWaistPos, vFeetPos);
						feetPitch = vectopitch(vFeetPos);
						*pfTorsoPitch = AngleNormalize180(feetPitch);
					}

					if ( pfWaistPitch )
					{
						VectorSubtract(vWaistPos, vEnd, vFeetPos);
						feetPosPitch = vectopitch(vFeetPos);
						*pfWaistPitch = AngleNormalize180(feetPosPitch);
					}

					if ( bLanded )
						return 1;
				}
			}
		}
	}

	if ( bOnGround )
		return 0;

	if ( pfTorsoHeight )
		*pfTorsoHeight = 0.0;

	if ( pfTorsoPitch )
		*pfTorsoPitch = 0.0;

	if ( pfWaistPitch )
		*pfWaistPitch = 0.0;

	return 1;
}

int BG_CheckProne(int passEntityNum, const float *const vPos, float fSize, float fHeight, float fYaw, float *pfTorsoHeight, float *pfTorsoPitch, float *pfWaistPitch, int bAlreadyProne, int bOnGround, float *const vGroundNormal, unsigned char handler, int proneCheckType, float prone_feet_dist)
{
	return BG_CheckProneValid(passEntityNum, vPos, fSize, fHeight, fYaw, pfTorsoHeight, pfTorsoPitch, pfWaistPitch, bAlreadyProne, bOnGround, vGroundNormal, handler, proneCheckType, prone_feet_dist);
}

int BG_CheckProneTurned(playerState_s *ps, float newProneYaw, unsigned char handler)
{
	float fraction;
	float dist;
	float abs;
	float testYaw;
	float delta;

	delta = AngleDelta(newProneYaw, ps->viewangles[1]);
	abs = fabs(delta) / 240.0;
	fraction = newProneYaw - (1.0 - abs) * delta;
	testYaw = AngleNormalize360Accurate(fraction);
	dist = abs * 45.0 + (1.0 - abs) * 66.0;

	return BG_CheckProne(
	           ps->clientNum,
	           ps->origin,
	           ps->maxs[0],
	           30.0,
	           testYaw,
	           &ps->fTorsoHeight,
	           &ps->fTorsoPitch,
	           &ps->fWaistPitch,
	           1,
	           ps->groundEntityNum != 1023,
	           0,
	           handler,
	           0,
	           dist);
}

void BG_RegisterDvars()
{
	player_view_pitch_up = Dvar_RegisterFloat("player_view_pitch_up", 85.0, 0.0, 90.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_view_pitch_down = Dvar_RegisterFloat("player_view_pitch_down", 85.0, 0.0, 90.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_ladder_yawcap = Dvar_RegisterFloat("bg_ladder_yawcap", 100.0, 0.0, 360.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_prone_yawcap = Dvar_RegisterFloat("bg_prone_yawcap", 85.0, 0.0, 360.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_foliagesnd_minspeed = Dvar_RegisterFloat("bg_foliagesnd_minspeed", 40.0, 0.0, 3.4028235e38, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_foliagesnd_maxspeed = Dvar_RegisterFloat("bg_foliagesnd_maxspeed", 180.0, 0.0, 3.4028235e38, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_foliagesnd_slowinterval = Dvar_RegisterInt("bg_foliagesnd_slowinterval", 1500, 0, 0x7FFFFFFF, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_foliagesnd_fastinterval = Dvar_RegisterInt("bg_foliagesnd_fastinterval", 500, 0, 0x7FFFFFFF, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_foliagesnd_resetinterval = Dvar_RegisterInt("bg_foliagesnd_resetinterval", 500, 0, 0x7FFFFFFF, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_fallDamageMinHeight = Dvar_RegisterFloat("bg_fallDamageMinHeight", 256.0, 1.0, 3.4028235e38, DVAR_SYSTEMINFO | DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_fallDamageMaxHeight = Dvar_RegisterFloat("bg_fallDamageMaxHeight", 480.0, 1.0, 3.4028235e38, DVAR_SYSTEMINFO | DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	inertiaMax = Dvar_RegisterFloat("inertiaMax", 50.0, 0.0, 1000.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	inertiaDebug = Dvar_RegisterBool("inertiaDebug", 0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	inertiaAngle = Dvar_RegisterFloat("inertiaAngle", 0.0, -1.0, 1.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	friction = Dvar_RegisterFloat("friction", 5.5, 0.0, 100.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	stopspeed = Dvar_RegisterFloat("stopspeed", 100.0, 0.0, 1000.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_swingSpeed = Dvar_RegisterFloat("bg_swingSpeed", 0.2, 0.0, 1.0, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_bobAmplitudeStanding = Dvar_RegisterFloat("bg_bobAmplitudeStanding", 0.0070000002, 0.0, 1.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_bobAmplitudeDucked = Dvar_RegisterFloat("bg_bobAmplitudeDucked", 0.0074999998, 0.0, 1.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_bobAmplitudeProne = Dvar_RegisterFloat("bg_bobAmplitudeProne", 0.029999999, 0.0, 1.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_bobMax = Dvar_RegisterFloat("bg_bobMax", 8.0, 0.0, 36.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	bg_aimSpreadMoveSpeedThreshold = Dvar_RegisterFloat("bg_aimSpreadMoveSpeedThreshold", 11.0, 0.0, 300.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_breath_hold_time = Dvar_RegisterFloat("player_breath_hold_time", 4.5, 0.0, 30.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_breath_gasp_time = Dvar_RegisterFloat("player_breath_gasp_time", 1.0, 0.0, 30.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_breath_fire_delay = Dvar_RegisterFloat("player_breath_fire_delay", 0.0, 0.0, 30.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_breath_gasp_scale = Dvar_RegisterFloat("player_breath_gasp_scale", 4.5, 0.0, 50.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_breath_hold_lerp = Dvar_RegisterFloat("player_breath_hold_lerp", 4.0, 0.0, 50.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_breath_gasp_lerp = Dvar_RegisterFloat("player_breath_gasp_lerp", 6.0, 0.0, 50.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_breath_snd_lerp = Dvar_RegisterFloat("player_breath_snd_lerp", 2.0, 0.0, 100.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_breath_snd_delay = Dvar_RegisterFloat("player_breath_snd_delay", 1.0, 0.0, 2.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_toggleBinoculars = Dvar_RegisterBool("player_toggleBinoculars", 1, DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_scopeExitOnDamage = Dvar_RegisterBool("player_scopeExitOnDamage", 0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_adsExitDelay = Dvar_RegisterInt("player_adsExitDelay", 0, 0, 1000, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_moveThreshhold = Dvar_RegisterFloat("player_moveThreshhold", 10.0, 0.0000000099999999, 20.0, DVAR_ROM | DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_footstepsThreshhold = Dvar_RegisterFloat("player_footstepsThreshhold", 0.0, 0.0, 50000.0, DVAR_ROM | DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_strafeSpeedScale = Dvar_RegisterFloat("player_strafeSpeedScale", 0.80000001, 0.0, 20.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_backSpeedScale = Dvar_RegisterFloat("player_backSpeedScale", 0.69999999, 0.0, 20.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_spectateSpeedScale = Dvar_RegisterFloat("player_spectateSpeedScale", 2.0, 0.0, 20.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_turnAnims = Dvar_RegisterBool("player_turnAnims", 0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_dmgtimer_timePerPoint = Dvar_RegisterFloat("player_dmgtimer_timePerPoint", 100.0, 0.0, 3.4028235e38, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_dmgtimer_maxTime = Dvar_RegisterFloat("player_dmgtimer_maxTime", 750.0, 0.0, 3.4028235e38, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_dmgtimer_minScale = Dvar_RegisterFloat("player_dmgtimer_minScale", 0.0, 0.0, 1.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_dmgtimer_stumbleTime = Dvar_RegisterInt("player_dmgtimer_stumbleTime", 500, 0, 2000, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	player_dmgtimer_flinchTime = Dvar_RegisterInt("player_dmgtimer_flinchTime", 500, 0, 2000, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	Jump_RegisterDvars();
	Mantle_RegisterDvars();
}