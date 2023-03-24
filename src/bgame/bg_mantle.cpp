#include "../qcommon/qcommon.h"
#include "bg_public.h"

dvar_t *mantle_enable;
dvar_t *mantle_debug;
dvar_t *mantle_check_range;
dvar_t *mantle_check_radius;
dvar_t *mantle_check_angle;
dvar_t *mantle_view_yawcap;

void Mantle_RegisterDvars()
{
	mantle_enable = Dvar_RegisterBool("mantle_enable", 1, 0x1180u);
	mantle_debug = Dvar_RegisterBool("mantle_debug", 0, 0x1180u);
	mantle_check_range = Dvar_RegisterFloat("mantle_check_range", 20.0, 0.0, 128.0, 0x1180u);
	mantle_check_radius = Dvar_RegisterFloat("mantle_check_radius", 0.1, 0.0, 15.0, 0x1180u);
	mantle_check_angle = Dvar_RegisterFloat("mantle_check_angle", 60.0, 0.0, 180.0, 0x1180u);
	mantle_view_yawcap = Dvar_RegisterFloat("mantle_view_yawcap", 60.0, 0.0, 180.0, 0x1180u);
}