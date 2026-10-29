#include "gsc_bots.hpp"

#if LIBCOD_COMPILE_BOTS == 1

void gsc_bots_set_walkdir(scr_entref_t id)
{
	const char *dir;

	if ( ! stackGetParams("s", &dir))
	{
		stackError("gsc_bots_set_walkdir() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];
	gentity_t *entity = &g_entities[id.entnum];

	if (entity->client == NULL)
	{
		stackError("gsc_bots_set_walkdir() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_set_walkdir() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	if (strcmp(dir, "none") == 0)
	{
		entity->client->libcod.bot_forwardmove = BUTTON_NONE;
		entity->client->libcod.bot_rightmove = BUTTON_NONE;
	}
	else if (strcmp(dir, "forward") == 0)
		entity->client->libcod.bot_forwardmove = BUTTON_FORWARD;
	else if (strcmp(dir, "back") == 0)
		entity->client->libcod.bot_forwardmove = BUTTON_BACK;
	else if (strcmp(dir, "right") == 0)
		entity->client->libcod.bot_rightmove = BUTTON_MOVERIGHT;
	else if (strcmp(dir, "left") == 0)
		entity->client->libcod.bot_rightmove = BUTTON_MOVELEFT;
	else
	{
		stackError("gsc_bots_set_walkdir() invalid argument '%s'. Valid arguments are: 'none' 'forward' 'back' 'right' 'left'", dir);
		stackPushUndefined();
		return;
	}

	stackPushBool(qtrue);
}

void gsc_bots_set_lean(scr_entref_t id)
{
	const char *lean;

	if ( ! stackGetParams("s", &lean))
	{
		stackError("gsc_bots_set_lean() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];
	gentity_t *entity = &g_entities[id.entnum];

	if (entity->client == NULL)
	{
		stackError("gsc_bots_set_lean() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_set_lean() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	if (strcmp(lean, "none") == 0)
		entity->client->libcod.bot_buttons &= ~(BUTTON_LEANLEFT | BUTTON_LEANRIGHT);
	else if (strcmp(lean, "left") == 0)
		entity->client->libcod.bot_buttons |= BUTTON_LEANLEFT;
	else if (strcmp(lean, "right") == 0)
		entity->client->libcod.bot_buttons |= BUTTON_LEANRIGHT;
	else
	{
		stackError("gsc_bots_set_lean() invalid argument '%s'. Valid arguments are: 'right' 'left'", lean);
		stackPushUndefined();
		return;
	}

	stackPushBool(qtrue);
}

void gsc_bots_set_stance(scr_entref_t id)
{
	const char *stance;

	if ( ! stackGetParams("s", &stance))
	{
		stackError("gsc_bots_set_stance() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];
	gentity_t *entity = &g_entities[id.entnum];

	if (entity->client == NULL)
	{
		stackError("gsc_bots_set_stance() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_set_stance() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	if (strcmp(stance, "stand") == 0)
		entity->client->libcod.bot_buttons &= ~(BUTTON_CROUCH | BUTTON_PRONE | BUTTON_JUMP);
	else if (strcmp(stance, "crouch") == 0)
		entity->client->libcod.bot_buttons |= BUTTON_CROUCH;
	else if (strcmp(stance, "prone") == 0)
		entity->client->libcod.bot_buttons |= BUTTON_PRONE;
	else if (strcmp(stance, "jump") == 0)
		entity->client->libcod.bot_buttons |= BUTTON_JUMP;
	else
	{
		stackError("gsc_bots_set_stance() invalid argument '%s'. Valid arguments are: 'stand' 'crouch' 'prone' 'jump'", stance);
		stackPushUndefined();
		return;
	}

	stackPushBool(qtrue);
}

void gsc_bots_thrownade(scr_entref_t id)
{
	int grenade;

	if ( ! stackGetParams("i", &grenade))
	{
		stackError("gsc_bots_thrownade() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];
	gentity_t *entity = &g_entities[id.entnum];

	if (entity->client == NULL)
	{
		stackError("gsc_bots_thrownade() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_thrownade() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	if (!grenade)
		entity->client->libcod.bot_buttons &= ~BUTTON_FRAG;
	else
		entity->client->libcod.bot_buttons |= BUTTON_FRAG;

	stackPushBool(qtrue);
}

void gsc_bots_fireweapon(scr_entref_t id)
{
	int shoot;

	if ( ! stackGetParams("i", &shoot))
	{
		stackError("gsc_bots_fireweapon() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];
	gentity_t *entity = &g_entities[id.entnum];

	if (entity->client == NULL)
	{
		stackError("gsc_bots_fireweapon() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_fireweapon() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	if (!shoot)
		entity->client->libcod.bot_buttons &= ~BUTTON_ATTACK;
	else
		entity->client->libcod.bot_buttons |= BUTTON_ATTACK;

	stackPushBool(qtrue);
}

void gsc_bots_meleeweapon(scr_entref_t id)
{
	int melee;

	if ( ! stackGetParams("i", &melee))
	{
		stackError("gsc_bots_meleeweapon() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];
	gentity_t *entity = &g_entities[id.entnum];

	if (entity->client == NULL)
	{
		stackError("gsc_bots_meleeweapon() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_meleeweapon() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	if (!melee)
		entity->client->libcod.bot_buttons &= ~BUTTON_MELEE;
	else
		entity->client->libcod.bot_buttons |= BUTTON_MELEE;

	stackPushBool(qtrue);
}

void gsc_bots_reloadweapon(scr_entref_t id)
{
	int reload;

	if ( ! stackGetParams("i", &reload))
	{
		stackError("gsc_bots_reloadweapon() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];
	gentity_t *entity = &g_entities[id.entnum];

	if (entity->client == NULL)
	{
		stackError("gsc_bots_reloadweapon() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_reloadweapon() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	if (!reload)
		entity->client->libcod.bot_buttons &= ~BUTTON_RELOAD;
	else
		entity->client->libcod.bot_buttons |= BUTTON_RELOAD;

	stackPushBool(qtrue);
}

void gsc_bots_adsaim(scr_entref_t id)
{
	int ads;

	if ( ! stackGetParams("i", &ads))
	{
		stackError("gsc_bots_adsaim() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];
	gentity_t *entity = &g_entities[id.entnum];

	if (entity->client == NULL)
	{
		stackError("gsc_bots_adsaim() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_adsaim() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	if (!ads)
		entity->client->libcod.bot_buttons &= ~BUTTON_ADS;
	else
		entity->client->libcod.bot_buttons |= BUTTON_ADS;

	stackPushBool(qtrue);
}

void gsc_bots_switchtoweaponid(scr_entref_t id)
{
	int weaponid;

	if ( ! stackGetParams("i", &weaponid))
	{
		stackError("gsc_bots_switchtoweaponid() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];
	gentity_t *entity = &g_entities[id.entnum];

	if (entity->client == NULL)
	{
		stackError("gsc_bots_switchtoweaponid() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_switchtoweaponid() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	entity->client->libcod.bot_weapon = weaponid;
	stackPushBool(qtrue);
}

#endif
