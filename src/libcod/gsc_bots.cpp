#include "gsc_bots.hpp"

#if COMPILE_BOTS == 1

void gsc_bots_set_walkdir(scr_entref_t id)
{
	char *dir;

	if ( ! stackGetParams("s", &dir))
	{
		stackError("gsc_bots_set_walkdir() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (id.entnum >= MAX_CLIENTS)
	{
		stackError("gsc_bots_set_walkdir() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_set_walkdir() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	extern char bot_forwardmove[MAX_CLIENTS];
	extern char bot_rightmove[MAX_CLIENTS];

	if (strcmp(dir, "none") == 0)
	{
		bot_forwardmove[id.entnum] = KEY_MASK_NONE;
		bot_rightmove[id.entnum] = KEY_MASK_NONE;
	}
	else if (strcmp(dir, "forward") == 0)
		bot_forwardmove[id.entnum] = KEY_MASK_FORWARD;
	else if (strcmp(dir, "back") == 0)
		bot_forwardmove[id.entnum] = KEY_MASK_BACK;
	else if (strcmp(dir, "right") == 0)
		bot_rightmove[id.entnum] = KEY_MASK_MOVERIGHT;
	else if (strcmp(dir, "left") == 0)
		bot_rightmove[id.entnum] = KEY_MASK_MOVELEFT;
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
	char *lean;

	if ( ! stackGetParams("s", &lean))
	{
		stackError("gsc_bots_set_lean() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (id.entnum >= MAX_CLIENTS)
	{
		stackError("gsc_bots_set_lean() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_set_lean() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	extern int bot_buttons[MAX_CLIENTS];

	if (strcmp(lean, "none") == 0)
		bot_buttons[id.entnum] &= ~(KEY_MASK_LEANLEFT | KEY_MASK_LEANRIGHT);
	else if (strcmp(lean, "left") == 0)
		bot_buttons[id.entnum] |= KEY_MASK_LEANLEFT;
	else if (strcmp(lean, "right") == 0)
		bot_buttons[id.entnum] |= KEY_MASK_LEANRIGHT;
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
	char *stance;

	if ( ! stackGetParams("s", &stance))
	{
		stackError("gsc_bots_set_stance() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (id.entnum >= MAX_CLIENTS)
	{
		stackError("gsc_bots_set_stance() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_set_stance() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	extern int bot_buttons[MAX_CLIENTS];

	if (strcmp(stance, "stand") == 0)
		bot_buttons[id.entnum] &= ~(KEY_MASK_CROUCH | KEY_MASK_PRONE | KEY_MASK_JUMP);
	else if (strcmp(stance, "crouch") == 0)
		bot_buttons[id.entnum] |= KEY_MASK_CROUCH;
	else if (strcmp(stance, "prone") == 0)
		bot_buttons[id.entnum] |= KEY_MASK_PRONE;
	else if (strcmp(stance, "jump") == 0)
		bot_buttons[id.entnum] |= KEY_MASK_JUMP;
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

	if (id.entnum >= MAX_CLIENTS)
	{
		stackError("gsc_bots_thrownade() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_thrownade() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	extern int bot_buttons[MAX_CLIENTS];

	if (!grenade)
		bot_buttons[id.entnum] &= ~KEY_MASK_FRAG;
	else
		bot_buttons[id.entnum] |= KEY_MASK_FRAG;

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

	if (id.entnum >= MAX_CLIENTS)
	{
		stackError("gsc_bots_fireweapon() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_fireweapon() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	extern int bot_buttons[MAX_CLIENTS];

	if (!shoot)
		bot_buttons[id.entnum] &= ~KEY_MASK_FIRE;
	else
		bot_buttons[id.entnum] |= KEY_MASK_FIRE;

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

	if (id.entnum >= MAX_CLIENTS)
	{
		stackError("gsc_bots_meleeweapon() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_meleeweapon() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	extern int bot_buttons[MAX_CLIENTS];

	if (!melee)
		bot_buttons[id.entnum] &= ~KEY_MASK_MELEE;
	else
		bot_buttons[id.entnum] |= KEY_MASK_MELEE;

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

	if (id.entnum >= MAX_CLIENTS)
	{
		stackError("gsc_bots_reloadweapon() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_reloadweapon() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	extern int bot_buttons[MAX_CLIENTS];

	if (!reload)
		bot_buttons[id.entnum] &= ~KEY_MASK_RELOAD;
	else
		bot_buttons[id.entnum] |= KEY_MASK_RELOAD;

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

	if (id.entnum >= MAX_CLIENTS)
	{
		stackError("gsc_bots_adsaim() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_adsaim() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	extern int bot_buttons[MAX_CLIENTS];

	if (!ads)
		bot_buttons[id.entnum] &= ~KEY_MASK_ADS_MODE;
	else
		bot_buttons[id.entnum] |= KEY_MASK_ADS_MODE;

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

	if (id.entnum >= MAX_CLIENTS)
	{
		stackError("gsc_bots_switchtoweaponid() entity %i is not a player", id.entnum);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id.entnum];

	if (client->netchan.remoteAddress.type != NA_BOT)
	{
		stackError("gsc_bots_switchtoweaponid() player %i is not a bot", id.entnum);
		stackPushUndefined();
		return;
	}

	extern int bot_weapon[MAX_CLIENTS];

	bot_weapon[id.entnum] = weaponid;

	stackPushBool(qtrue);
}

#endif
