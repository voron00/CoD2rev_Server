#include "../qcommon/qcommon.h"
#include "bg_public.h"

dvar_t *jump_height;
dvar_t *jump_stepSize;
dvar_t *jump_slowdownEnable;
dvar_t *jump_ladderPushVel;
dvar_t *jump_spreadAdd;

void Jump_RegisterDvars()
{
	jump_height = Dvar_RegisterFloat("jump_height", 39.0, 0.0, 128.0, 0x1180u);
	jump_stepSize = Dvar_RegisterFloat("jump_stepSize", 18.0, 0.0, 64.0, 0x1180u);
	jump_slowdownEnable = Dvar_RegisterBool("jump_slowdownEnable", 1, 0x1180u);
	jump_ladderPushVel = Dvar_RegisterFloat("jump_ladderPushVel", 128.0, 0.0, 1024.0, 0x1180u);
	jump_spreadAdd = Dvar_RegisterFloat("jump_spreadAdd", 64.0, 0.0, 512.0, 0x1180u);
}