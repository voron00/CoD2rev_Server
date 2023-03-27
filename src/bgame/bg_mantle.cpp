#include "../qcommon/qcommon.h"
#include "bg_public.h"

dvar_t *mantle_enable;
dvar_t *mantle_debug;
dvar_t *mantle_check_range;
dvar_t *mantle_check_radius;
dvar_t *mantle_check_angle;
dvar_t *mantle_view_yawcap;

MantleAnimTransition s_mantleTrans[] =
{
	{ 1, 8, 57.000000 },
	{ 2, 8, 51.000000 },
	{ 3, 9, 45.000000 },
	{ 4, 9, 39.000000 },
	{ 5, 9, 33.000000 },
	{ 6, 10, 27.000000 },
	{ 7, 10, 21.000000 },
	{ 0, 0, 0.000000 },
};

bool Mantle_IsWeaponInactive(const playerState_s *ps)
{
	return mantle_enable->current.boolean
	       && (ps->pm_flags & 4) != 0
	       && s_mantleTrans[ps->mantleState.transIndex].overAnimIndex != 10;
}

void Mantle_RegisterDvars()
{
	mantle_enable = Dvar_RegisterBool("mantle_enable", 1, 0x1180u);
	mantle_debug = Dvar_RegisterBool("mantle_debug", 0, 0x1180u);
	mantle_check_range = Dvar_RegisterFloat("mantle_check_range", 20.0, 0.0, 128.0, 0x1180u);
	mantle_check_radius = Dvar_RegisterFloat("mantle_check_radius", 0.1, 0.0, 15.0, 0x1180u);
	mantle_check_angle = Dvar_RegisterFloat("mantle_check_angle", 60.0, 0.0, 180.0, 0x1180u);
	mantle_view_yawcap = Dvar_RegisterFloat("mantle_view_yawcap", 60.0, 0.0, 180.0, 0x1180u);
}