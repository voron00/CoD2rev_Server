#include "../qcommon/qcommon.h"
#include "bg_public.h"

#ifdef TESTING_LIBRARY
#define level_bgs (*((bgs_t*)( 0x0859EA40 )))
#else
bgs_t level_bgs;
#endif

#ifdef TESTING_LIBRARY
#define bgs (*((bgs_t**)( 0x0855A4E0 )))
#else
bgs_t *bgs;
#endif

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

void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps )
{
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

	if ( ps->pm_type <= 5 )
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

void BG_RegisterDvars()
{
	player_view_pitch_up = Dvar_RegisterFloat("player_view_pitch_up", 85.0, 0.0, 90.0, 0x1180u);
	player_view_pitch_down = Dvar_RegisterFloat("player_view_pitch_down", 85.0, 0.0, 90.0, 0x1180u);
	bg_ladder_yawcap = Dvar_RegisterFloat("bg_ladder_yawcap", 100.0, 0.0, 360.0, 0x1180u);
	bg_prone_yawcap = Dvar_RegisterFloat("bg_prone_yawcap", 85.0, 0.0, 360.0, 0x1180u);
	bg_foliagesnd_minspeed = Dvar_RegisterFloat("bg_foliagesnd_minspeed", 40.0, 0.0, 3.4028235e38, 0x1180u);
	bg_foliagesnd_maxspeed = Dvar_RegisterFloat("bg_foliagesnd_maxspeed", 180.0, 0.0, 3.4028235e38, 0x1180u);
	bg_foliagesnd_slowinterval = Dvar_RegisterInt("bg_foliagesnd_slowinterval", 1500, 0, 0x7FFFFFFF, 0x1180u);
	bg_foliagesnd_fastinterval = Dvar_RegisterInt("bg_foliagesnd_fastinterval", 500, 0, 0x7FFFFFFF, 0x1180u);
	bg_foliagesnd_resetinterval = Dvar_RegisterInt("bg_foliagesnd_resetinterval", 500, 0, 0x7FFFFFFF, 0x1180u);
	bg_fallDamageMinHeight = Dvar_RegisterFloat("bg_fallDamageMinHeight", 256.0, 1.0, 3.4028235e38, 0x1188u);
	bg_fallDamageMaxHeight = Dvar_RegisterFloat("bg_fallDamageMaxHeight", 480.0, 1.0, 3.4028235e38, 0x1188u);
	inertiaMax = Dvar_RegisterFloat("inertiaMax", 50.0, 0.0, 1000.0, 0x1180u);
	inertiaDebug = Dvar_RegisterBool("inertiaDebug", 0, 0x1180u);
	inertiaAngle = Dvar_RegisterFloat("inertiaAngle", 0.0, -1.0, 1.0, 0x1180u);
	friction = Dvar_RegisterFloat("friction", 5.5, 0.0, 100.0, 0x1180u);
	stopspeed = Dvar_RegisterFloat("stopspeed", 100.0, 0.0, 1000.0, 0x1180u);
	bg_swingSpeed = Dvar_RegisterFloat("bg_swingSpeed", 0.2, 0.0, 1.0, 0x1080u);
	bg_bobAmplitudeStanding = Dvar_RegisterFloat("bg_bobAmplitudeStanding", 0.0070000002, 0.0, 1.0, 0x1180u);
	bg_bobAmplitudeDucked = Dvar_RegisterFloat("bg_bobAmplitudeDucked", 0.0074999998, 0.0, 1.0, 0x1180u);
	bg_bobAmplitudeProne = Dvar_RegisterFloat("bg_bobAmplitudeProne", 0.029999999, 0.0, 1.0, 0x1180u);
	bg_bobMax = Dvar_RegisterFloat("bg_bobMax", 8.0, 0.0, 36.0, 0x1180u);
	bg_aimSpreadMoveSpeedThreshold = Dvar_RegisterFloat("bg_aimSpreadMoveSpeedThreshold", 11.0, 0.0, 300.0, 0x1180u);
	player_breath_hold_time = Dvar_RegisterFloat("player_breath_hold_time", 4.5, 0.0, 30.0, 0x1180u);
	player_breath_gasp_time = Dvar_RegisterFloat("player_breath_gasp_time", 1.0, 0.0, 30.0, 0x1180u);
	player_breath_fire_delay = Dvar_RegisterFloat("player_breath_fire_delay", 0.0, 0.0, 30.0, 0x1180u);
	player_breath_gasp_scale = Dvar_RegisterFloat("player_breath_gasp_scale", 4.5, 0.0, 50.0, 0x1180u);
	player_breath_hold_lerp = Dvar_RegisterFloat("player_breath_hold_lerp", 4.0, 0.0, 50.0, 0x1180u);
	player_breath_gasp_lerp = Dvar_RegisterFloat("player_breath_gasp_lerp", 6.0, 0.0, 50.0, 0x1180u);
	player_breath_snd_lerp = Dvar_RegisterFloat("player_breath_snd_lerp", 2.0, 0.0, 100.0, 0x1180u);
	player_breath_snd_delay = Dvar_RegisterFloat("player_breath_snd_delay", 1.0, 0.0, 2.0, 0x1180u);
	player_toggleBinoculars = Dvar_RegisterBool("player_toggleBinoculars", 1, 0x1100u);
	player_scopeExitOnDamage = Dvar_RegisterBool("player_scopeExitOnDamage", 0, 0x1180u);
	player_adsExitDelay = Dvar_RegisterInt("player_adsExitDelay", 0, 0, 1000, 0x1180u);
	player_moveThreshhold = Dvar_RegisterFloat("player_moveThreshhold", 10.0, 0.0000000099999999, 20.0, 0x11C0u);
	player_footstepsThreshhold = Dvar_RegisterFloat("player_footstepsThreshhold", 0.0, 0.0, 50000.0, 0x11C0u);
	player_strafeSpeedScale = Dvar_RegisterFloat("player_strafeSpeedScale", 0.80000001, 0.0, 20.0, 0x1180u);
	player_backSpeedScale = Dvar_RegisterFloat("player_backSpeedScale", 0.69999999, 0.0, 20.0, 0x1180u);
	player_spectateSpeedScale = Dvar_RegisterFloat("player_spectateSpeedScale", 2.0, 0.0, 20.0, 0x1180u);
	player_turnAnims = Dvar_RegisterBool("player_turnAnims", 0, 0x1180u);
	player_dmgtimer_timePerPoint = Dvar_RegisterFloat("player_dmgtimer_timePerPoint", 100.0, 0.0, 3.4028235e38, 0x1180u);
	player_dmgtimer_maxTime = Dvar_RegisterFloat("player_dmgtimer_maxTime", 750.0, 0.0, 3.4028235e38, 0x1180u);
	player_dmgtimer_minScale = Dvar_RegisterFloat("player_dmgtimer_minScale", 0.0, 0.0, 1.0, 0x1180u);
	player_dmgtimer_stumbleTime = Dvar_RegisterInt("player_dmgtimer_stumbleTime", 500, 0, 2000, 0x1180u);
	player_dmgtimer_flinchTime = Dvar_RegisterInt("player_dmgtimer_flinchTime", 500, 0, 2000, 0x1180u);
	Jump_RegisterDvars();
	Mantle_RegisterDvars();
}