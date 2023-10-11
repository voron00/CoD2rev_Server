#include "../qcommon/qcommon.h"
#include "../qcommon/cmd.h"
#include "dvar.h"

void Dvar_ForEach( void(*callback)(const char *s) )
{
	dvar_t *var;

	for ( var = sortedDvars ; var ; var = var->next )
	{
		callback( var->name );
	}
}

void Dvar_WriteVariables(fileHandle_t f)
{
	dvar_t	*var;

	for (var = sortedDvars; var; var = var->next)
	{
		if (I_stricmp(var->name, "cl_cdkey"))
		{
			if (var->flags & DVAR_ARCHIVE)
				FS_Printf(f, "seta %s \"%s\"\n", var->name, Dvar_DisplayableLatchedValue(var));
		}
	}
}

void Dvar_WriteDefaults(fileHandle_t f)
{
	dvar_t	*var;

	for (var = sortedDvars; var; var = var->next)
	{
		if (I_stricmp(var->name, "cl_cdkey"))
		{
			if ( ((var->flags & 0x40C0) == 0 ))
				FS_Printf(f, "set %s \"%s\"\n", var->name, Dvar_DisplayableResetValue(var));
		}
	}
}

void Dvar_RegisterBool_f(void)
{
	dvar_t *dvar;
	const char *name, *value;
	qboolean var_value;

	if (Cmd_Argc() == 3)
	{
		name = Cmd_Argv(1);
		value = Cmd_Argv(2);
		var_value = atoi(value) != 0;

		dvar = Dvar_FindVar(name);

		if (dvar)
		{
			if (dvar->type != DVAR_TYPE_BOOL)
			{
				Com_Printf("dvar '%s' is not a boolean dvar\n", dvar->name);
				return;
			}
		}
		else
			Dvar_RegisterBool(name, var_value, DVAR_EXTERNAL);
	}
	else
		Com_Printf("USAGE: %s <name> <default>\n", Cmd_Argv(0));
}

void Dvar_RegisterInt_f(void)
{
	dvar_t *dvar;
	const char *name, *value, *min, *max;
	int var_min, var_max, var_value;

	if (Cmd_Argc() == 5)
	{
		name = Cmd_Argv(1);
		value = Cmd_Argv(2);
		min = Cmd_Argv(3);
		max = Cmd_Argv(4);

		var_value = atoi(value);
		var_min = atoi(min);
		var_max = atoi(max);

		dvar = Dvar_FindVar(name);

		if (var_max >= var_min)
		{
			if (dvar)
			{
				if (dvar->type != DVAR_TYPE_INT)
				{
					Com_Printf("dvar '%s' is not a integer dvar\n", dvar->name);
					return;
				}
			}
			else
				Dvar_RegisterFloat(name, var_value, var_min, var_max, DVAR_EXTERNAL);
		}
		else
			Com_Printf("dvar %s: min %i should not be greater than max %i\n", Cmd_Argv(0), var_min, var_max);
	}
	else
		Com_Printf("USAGE: %s <name> <default> <min> <max>\n", Cmd_Argv(0));
}

void Dvar_RegisterFloat_f(void)
{
	dvar_t *dvar;
	const char *name, *value, *min, *max;
	float var_min, var_max, var_value;

	if (Cmd_Argc() == 5)
	{
		name = Cmd_Argv(1);
		value = Cmd_Argv(2);
		min = Cmd_Argv(3);
		max = Cmd_Argv(4);

		var_value = atof(value);
		var_min = atof(min);
		var_max = atof(max);

		dvar = Dvar_FindVar(name);

		if (var_max >= var_min)
		{
			if (dvar)
			{
				if (dvar->type != DVAR_TYPE_FLOAT)
				{
					Com_Printf("dvar '%s' is not a float dvar\n", dvar->name);
					return;
				}
			}
			else
				Dvar_RegisterFloat(name, var_value, var_min, var_max, DVAR_EXTERNAL);
		}
		else
			Com_Printf("dvar %s: min %g should not be greater than max %g\n", Cmd_Argv(0), var_min, var_max);
	}
	else
		Com_Printf("USAGE: %s <name> <default> <min> <max>\n", Cmd_Argv(0));
}

qboolean Dvar_ToggleSimple(dvar_t *dvar)
{
	switch (dvar->type)
	{
	case DVAR_TYPE_BOOL:
		Dvar_SetBoolFromSource(dvar, !dvar->current.boolean, DVAR_SOURCE_EXTERNAL);
		return qtrue;

	case DVAR_TYPE_FLOAT:
		if (dvar->domain.decimal.min > 0.0 || dvar->domain.decimal.max < 1.0)
		{
			if (dvar->current.decimal == dvar->domain.decimal.min)
				Dvar_SetFloatFromSource(dvar, dvar->domain.decimal.max, DVAR_SOURCE_EXTERNAL);
			else
				Dvar_SetFloatFromSource(dvar, dvar->domain.decimal.min, DVAR_SOURCE_EXTERNAL);
		}
		else if (dvar->current.decimal == 0.0)
			Dvar_SetFloatFromSource(dvar, 1, DVAR_SOURCE_EXTERNAL);
		else
			Dvar_SetFloatFromSource(dvar, 0, DVAR_SOURCE_EXTERNAL);
		return qtrue;

	case DVAR_TYPE_VEC2:
	case DVAR_TYPE_VEC3:
	case DVAR_TYPE_VEC4:
	case DVAR_TYPE_STRING:
	case DVAR_TYPE_COLOR:
		Com_Printf("'toggle' with no arguments makes no sense for dvar '%s'\n", dvar->name);
		return qfalse;

	case DVAR_TYPE_INT:
		if (dvar->domain.integer.min > 0 || dvar->domain.integer.max <= 0)
		{
			if (dvar->current.integer == dvar->domain.integer.min)
				Dvar_SetIntFromSource(dvar, dvar->domain.integer.max, DVAR_SOURCE_EXTERNAL);
			else
				Dvar_SetIntFromSource(dvar, dvar->domain.integer.min, DVAR_SOURCE_EXTERNAL);
		}
		else if (dvar->current.integer)
			Dvar_SetIntFromSource(dvar, 0, DVAR_SOURCE_EXTERNAL);
		else
			Dvar_SetIntFromSource(dvar, 1, DVAR_SOURCE_EXTERNAL);
		return qtrue;

	case DVAR_TYPE_ENUM:
		if (dvar->current.integer >= dvar->domain.enumeration.stringCount - 1)
			Dvar_SetIntFromSource(dvar, 0, DVAR_SOURCE_EXTERNAL);
		else
			Dvar_SetIntFromSource(dvar, dvar->current.integer + 1, DVAR_SOURCE_EXTERNAL);
		return qtrue;

	default:
		return qfalse;
	}
}

qboolean Dvar_ToggleInternal()
{
	const char* string;
	int argIndex;
	const char* argString;
	const char* dvarName;
	dvar_t* dvar;
	const char* enumString;

	if (Cmd_Argc() < 2)
	{
		Com_Printf("USAGE: %s <variable> <optional value sequence>\n", Cmd_Argv(0));
		return qfalse;
	}

	dvarName = Cmd_Argv(1);
	dvar = Dvar_FindVar(dvarName);
	if (!dvar)
	{
		Com_Printf("toggle failed: dvar '%s' not found.\n", dvarName);
		return qfalse;
	}

	if (Cmd_Argc() != 2)
	{
		string = Dvar_DisplayableValue(dvar);
		for (argIndex = 2; argIndex + 1 < Cmd_Argc(); ++argIndex)
		{
			argString = Cmd_Argv(argIndex);
			if (dvar->type == DVAR_TYPE_ENUM)
			{
				enumString = Dvar_IndexStringToEnumString(dvar, argString);
				if (strlen(enumString))
				{
					argString = (char*)enumString;
				}
			}
			if (!I_stricmp(string, argString))
			{
				Dvar_SetCommand(dvarName, Cmd_Argv(argIndex + 1));
				return qtrue;
			}
		}

		argString = Cmd_Argv(2);
		if (dvar->type == DVAR_TYPE_ENUM)
		{
			enumString = Dvar_IndexStringToEnumString(dvar, argString);
			if (strlen(enumString))
			{
				argString = (char*)enumString;
			}
		}
		Dvar_SetCommand(dvarName, argString);
		return qtrue;
	}

	return Dvar_ToggleSimple(dvar);
}

void Dvar_Toggle_f(void)
{
	Dvar_ToggleInternal();
}

void Dvar_Set_f(void)
{
	char dvar_value[MAXPRINTMSG];

	if (Cmd_Argc() > 2)
	{
		const char *name = Cmd_Argv(1);

		if (Dvar_IsValidName(name))
		{
			Dvar_GetCombinedString(dvar_value, 2);
			Dvar_SetCommand(name, dvar_value);
		}
		else
			Com_Printf("invalid variable name: %s\n", name);
	}
	else
		Com_Printf("USAGE: set <variable> <value>\n");
}

void Dvar_SetA_f(void)
{
	dvar_t *dvar;

	if (Cmd_Argc() > 2)
	{
		Dvar_Set_f();
		dvar = Dvar_FindVar(Cmd_Argv(1));

		if (dvar)
			Dvar_AddFlags(dvar, DVAR_ARCHIVE);
	}
	else
		Com_Printf("USAGE: seta <variable> <value>\n");
}

void Dvar_SetS_f(void)
{
	dvar_t *dvar;

	if (Cmd_Argc() > 2)
	{
		Dvar_Set_f();
		dvar = Dvar_FindVar(Cmd_Argv(1));

		if (dvar)
			Dvar_AddFlags(dvar, DVAR_SERVERINFO);
	}
	else
		Com_Printf("USAGE: sets <variable> <value>\n");
}

void Dvar_SetU_f(void)
{
	dvar_t *dvar;

	if (Cmd_Argc() > 2)
	{
		Dvar_Set_f();
		dvar = Dvar_FindVar(Cmd_Argv(1));

		if (dvar)
			Dvar_AddFlags(dvar, DVAR_USERINFO);
	}
	else
		Com_Printf("USAGE: setu <variable> <value>\n");
}

void Dvar_SetFromDvar_f(void)
{
	dvar_t *dvar;

	if (Cmd_Argc() == 3)
	{
		dvar = Dvar_FindVar(Cmd_Argv(2));

		if (dvar)
			Dvar_SetCommand(Cmd_Argv(1), Dvar_DisplayableValue(dvar));
		else
			Com_Printf("dvar '%s' doesn't exist\n", Cmd_Argv(2));
	}
	else
		Com_Printf("USAGE: setfromdvar <dest_dvar> <source_dvar>\n");
}

void Dvar_Reset_f(void)
{
	dvar_t *dvar;

	if (Cmd_Argc() == 2)
	{
		dvar = Dvar_FindVar(Cmd_Argv(1));

		if (dvar)
			Dvar_Reset(dvar, DVAR_SOURCE_EXTERNAL);
	}
	else
		Com_Printf("USAGE: reset <variable>\n");
}

void Dvar_TogglePrint_f(void)
{
	const char *cmd;
	dvar_t *dvar;

	if (Dvar_ToggleInternal())
	{
		cmd = Cmd_Argv(1);
		dvar = Dvar_FindVar(cmd);
		Com_Printf("%s toggled to %s\n", cmd, Dvar_DisplayableValue(dvar));
	}
}

void Com_DvarDump(conChannel_t channel)
{
	dvar_t *dvar;
	const char *match;
	int	i = 0;
	int count = 0;
	char message[8196];
	char summary[128];

	if ( Cmd_Argc() <= 1 )
		match = NULL;
	else
		match = Cmd_Argv(1);

	if (channel != CON_CHANNEL_LOGFILEONLY || (com_logfile && com_logfile->current.integer))
	{
		Com_PrintMessage(channel, "=============================== DVAR DUMP ========================================\n");

		for (dvar = sortedDvars; dvar; dvar = dvar->next, i++)
		{
			count++;

			if (!match || Com_Filter(match, dvar->name, 0))
			{
				if (Dvar_HasLatchedValue(dvar))
					Com_sprintf(message, sizeof(message), "      %s \"%s\" -- latched \"%s\"\n", dvar->name,
					            Dvar_DisplayableValue(dvar), Dvar_DisplayableLatchedValue(dvar));
				else
					Com_sprintf(message, sizeof(message), "      %s \"%s\"\n", dvar->name, Dvar_DisplayableValue(dvar));

				Com_PrintMessage(channel, message);
			}
		}

		Com_sprintf(summary, sizeof(summary), "\n%i total dvars\n%i dvar indexes\n", count, dvarCount);
		Com_PrintMessage(channel, summary);
		Com_PrintMessage(channel, "=============================== END DVAR DUMP =====================================\n");
	}
}

void Dvar_Dump_f(void)
{
	Com_DvarDump(CON_CHANNEL_DONT_FILTER);
}

void Dvar_List_f(void)
{
	dvar_t *var;
	const char *match;
	int	i = 0;

	if (Cmd_Argc() > 1)
	{
		match = Cmd_Argv(1);
		Com_Printf("Displaying all dvars starting with: %s\n", match);
	}
	else
		match = NULL;

	Com_Printf("====================================== Dvar List ======================================\n");

	for (var = sortedDvars; var; var = var->next, i++)
	{
		if (match && !Com_Filter(match, var->name, qfalse))
			continue;

		if (var->flags & (DVAR_SERVERINFO | DVAR_SCRIPTINFO))
			Com_Printf("S");
		else
			Com_Printf(" ");

		if (var->flags & DVAR_USERINFO)
			Com_Printf("U");
		else
			Com_Printf(" ");

		if (var->flags & DVAR_ROM)
			Com_Printf("R");
		else
			Com_Printf(" ");

		if (var->flags & DVAR_INIT)
			Com_Printf("I");
		else
			Com_Printf(" ");

		if (var->flags & DVAR_ARCHIVE)
			Com_Printf("A");
		else
			Com_Printf(" ");

		if (var->flags & DVAR_LATCH)
			Com_Printf("L");
		else
			Com_Printf(" ");

		if (var->flags & DVAR_CHEAT)
			Com_Printf("C");
		else
			Com_Printf(" ");

		Com_Printf(" %s \"%s\"\n", var->name, Dvar_DisplayableValue(var));
	}

	Com_Printf("\n%i total dvars\n", i);
	Com_Printf("%i dvar indexes\n", dvarCount);
	Com_Printf("==================================== End Dvar List ====================================\n");
}

char *Dvar_InfoString(unsigned short bit)
{
	static char info[MAX_INFO_STRING];
	dvar_t  *var;

	info[0] = 0;

	for ( var = sortedDvars ; var ; var = var->next )
	{
		if ( var->flags & bit )
		{
			Info_SetValueForKey( info, var->name, Dvar_DisplayableValue(var) );
		}
	}

	return info;
}

char *Dvar_InfoString_Big(unsigned short bit)
{
	static char info[BIG_INFO_STRING];
	dvar_t  *var;

	info[0] = 0;

	for ( var = sortedDvars ; var ; var = var->next )
	{
		if ( var->flags & bit )
		{
			Info_SetValueForKey_Big( info, var->name, Dvar_DisplayableValue(var) );
		}
	}

	return info;
}

void SV_SetConfig(int start, int max, unsigned short bit)
{
	dvar_t *var;

	for ( var = sortedDvars ; var ; var = var->next )
	{
		if ( var->flags & bit )
		{
			SV_SetConfigValueForKey(start, max, var->name, Dvar_DisplayableValue(var));
		}
	}
}

qboolean Dvar_Command()
{
	const char *cmd;
	dvar_t *var;
	char dvar_value[MAXPRINTMSG];

	cmd = Cmd_Argv(0);
	var = Dvar_FindVar(cmd);

	if ( !var )
		return qfalse;

	if ( Cmd_Argc() == 1 )
	{
		Com_Printf("\"%s\" is: \"%s^7\" default: \"%s^7\"\n", var->name, Dvar_DisplayableValue(var), Dvar_DisplayableResetValue(var));
		if ( Dvar_HasLatchedValue(var) )
		{
			Com_Printf("latched: \"%s\"\n", Dvar_DisplayableLatchedValue(var));
		}
		Dvar_PrintDomain(var->type, var->domain);
	}
	else
	{
		Dvar_GetCombinedString(dvar_value, 1);
		Dvar_SetCommand(cmd, dvar_value);
	}

	return qtrue;
}

void Dvar_AddCommands()
{
	Cmd_AddCommand("toggle", Dvar_Toggle_f);
	Cmd_AddCommand("togglep", Dvar_TogglePrint_f);
	Cmd_AddCommand("set", Dvar_Set_f);
	Cmd_AddCommand("sets", Dvar_SetS_f);
	Cmd_AddCommand("seta", Dvar_SetA_f);
	Cmd_AddCommand("setfromdvar", Dvar_SetFromDvar_f);
	Cmd_AddCommand("reset", Dvar_Reset_f);
	Cmd_AddCommand("dvarlist", Dvar_List_f);
	Cmd_AddCommand("dvardump", Dvar_Dump_f);
	Cmd_AddCommand("dvar_bool", Dvar_RegisterBool_f);
	Cmd_AddCommand("dvar_int", Dvar_RegisterInt_f);
	Cmd_AddCommand("dvar_float", Dvar_RegisterFloat_f);
	Cmd_AddCommand("setu", Dvar_SetU_f);
}