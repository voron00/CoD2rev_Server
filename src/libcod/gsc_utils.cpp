#include "gsc_utils.hpp"

#if COMPILE_UTILS == 1

//thanks to riicchhaarrd/php
void gsc_utils_getarraykeys()
{
	unsigned int arrIndex;

	if (!stackGetParamObject(0, &arrIndex))
	{
		stackError("gsc_utils_getarraykeys() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int arraysize = GetArraySize(arrIndex);

	if (!arraysize)
	{
		stackError("gsc_utils_getarraykeys() got an empty or invalid array");
		stackPushUndefined();
		return;
	}

	unsigned int index = arrIndex;
	unsigned int name;

	stackPushArray();

	for (int i = 0; i < arraysize; i++)
	{
		index = FindNextSibling(index);
		name = GetVariableName(index);

		if (name < 0x10000)
		{
			stackPushString(SL_ConvertToString(name));
			stackPushArrayLast();
		}
	}
}

void gsc_utils_printf()
{
	char result[COD2_MAX_STRINGLENGTH];
	const char *str;

	if (!stackGetParams("s", &str))
	{
		stackError("gsc_utils_printf() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int param = 1; // maps to first %
	int len = strlen(str);
	int num = 0;

	for (int i = 0; i < len; i++)
	{
		if (str[i] == '%')
		{
			if (str[i + 1] == '%')
			{
				result[num++] = '%';
				i++;
			}
			else
			{
				if(param >= Scr_GetNumParam())
					continue;

				switch (stackGetParamType(param))
				{
				case STACK_STRING:
					const char *tmp_str;
					stackGetParamString(param, &tmp_str); // no error checking, since we know it's a string
					num += sprintf(&(result[num]), "%s", tmp_str);
					break;

				case STACK_VECTOR:
					float vec[3];
					stackGetParamVector(param, vec);
					num += sprintf(&(result[num]), "(%.2f, %.2f, %.2f)", vec[0], vec[1], vec[2]);
					break;

				case STACK_FLOAT:
					float tmp_float;
					stackGetParamFloat(param, &tmp_float);
					num += sprintf(&(result[num]), "%.3f", tmp_float); // need a way to define precision
					break;

				case STACK_INT:
					int tmp_int;
					stackGetParamInt(param, &tmp_int);
					num += sprintf(&(result[num]), "%d", tmp_int);
					break;
				}

				param++;
			}
		}
		else
			result[num++] = str[i];
	}

	result[num] = '\0';
	Com_Printf(result);
	stackPushBool(qtrue);
}

void gsc_utils_sprintf()
{
	char result[COD2_MAX_STRINGLENGTH];
	const char *str;

	if (!stackGetParams("s", &str))
	{
		stackError("gsc_utils_sprintf() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int param = 1; // maps to first %
	int len = strlen(str);
	int num = 0;

	for (int i = 0; i < len; i++)
	{
		if (str[i] == '%')
		{
			if (str[i + 1] == '%')
			{
				result[num++] = '%';
				i++;
			}
			else
			{
				if(param >= Scr_GetNumParam())
					continue;

				switch (stackGetParamType(param))
				{
				case STACK_STRING:
					const char *tmp_str;
					stackGetParamString(param, &tmp_str); // no error checking, since we know it's a string
					num += sprintf(&(result[num]), "%s", tmp_str);
					break;

				case STACK_VECTOR:
					float vec[3];
					stackGetParamVector(param, vec);
					num += sprintf(&(result[num]), "(%.2f, %.2f, %.2f)", vec[0], vec[1], vec[2]);
					break;

				case STACK_FLOAT:
					float tmp_float;
					stackGetParamFloat(param, &tmp_float);
					num += sprintf(&(result[num]), "%.3f", tmp_float); // need a way to define precision
					break;

				case STACK_INT:
					int tmp_int;
					stackGetParamInt(param, &tmp_int);
					num += sprintf(&(result[num]), "%d", tmp_int);
					break;
				}

				param++;
			}
		}
		else
			result[num++] = str[i];
	}

	result[num] = '\0';
	stackPushString(result);
}

void gsc_utils_outofbandprint()
{
	const char *address;
	const char *msg;

	if (!stackGetParams("ss", &address, &msg))
	{
		stackError("gsc_utils_outofbandprint() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	netadr_t from;
	NET_StringToAdr(address, &from);
	NET_OutOfBandPrint(NS_SERVER, from, msg);
}

void gsc_utils_getAscii()
{
	const char *str;

	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_utils_getAscii() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!strlen(str))
	{
		stackError("gsc_utils_getAscii() string length is 0");
		stackPushUndefined();
		return;
	}

	stackPushInt(str[0]);
}

void gsc_utils_putchar()
{
	int val;

	if ( ! stackGetParams("i",  &val))
	{
		stackError("gsc_utils_putchar() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (val < -127 || val > 127)
	{
		stackError("gsc_utils_putchar() character index is out of range");
		stackPushUndefined();
		return;
	}

	char s[2];

	s[0] = val;
	s[1] = '\0';

	stackPushString( s );
}

void gsc_utils_toupper()
{
	const char *string;
	int i;
	char c;
	char tempString[1024];

	if ( ! stackGetParams("s", &string))
	{
		stackError("gsc_utils_toupper() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!strlen(string))
	{
		stackError("gsc_utils_toupper() string length is 0");
		stackPushUndefined();
		return;
	}

	for ( i = 0; i < 1024; ++i )
	{
		c = toupper(*string);
		tempString[i] = c;

		if ( !c )
		{
			stackPushString(tempString);
			return;
		}

		++string;
	}

	stackPushString( tempString );
}

void gsc_utils_system()
{
	const char *cmd;

	if ( ! stackGetParams("s",  &cmd))
	{
		stackError("gsc_utils_system() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushInt( system(cmd) );
}

static int starttime = time(NULL);
void gsc_utils_getserverstarttime()
{
	stackPushInt( starttime );
}

void gsc_utils_getsystemtime()
{
	time_t timer;
	stackPushInt( time(&timer) );
}

void gsc_utils_getlocaltime()
{
	time_t timer;
	struct tm *timeinfo;

	time(&timer);
	timeinfo = localtime(&timer);

	const char *timestring = asctime(timeinfo);
	char stripped_time[128];

	strncpy(stripped_time, timestring, sizeof(stripped_time));
	stripped_time[strlen(timestring) - 1] = '\0';

	stackPushString( stripped_time );
}

void gsc_utils_exponent()
{
	float basis;
	float exponent;

	if ( ! stackGetParams("ff", &basis, &exponent))
	{
		stackError("gsc_utils_exponent() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushFloat( pow(basis, exponent) );
}

void gsc_utils_round()
{
	float val;

	if ( ! stackGetParams("f",  &val))
	{
		stackError("gsc_utils_round() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushFloat( roundf(val * 100) / 100 );
}

void gsc_utils_file_link()
{
	const char *source, *dest;

	if ( ! stackGetParams("ss",  &source, &dest))
	{
		stackError("gsc_utils_file_link() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

#ifdef _WIN32
	char linkcmd[COD2_MAX_STRINGLENGTH];
	Com_sprintf(linkcmd, sizeof(linkcmd), "mklink /h %s %s", dest, source);
	stackPushInt( system(linkcmd) == 0 );
#else
	stackPushInt( symlink(source, dest) == 0 );
#endif
}

void gsc_utils_file_unlink()
{
	const char *file;

	if ( ! stackGetParams("s",  &file))
	{
		stackError("gsc_utils_file_unlink() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int unlink_success = unlink(file) == 0;
	stackPushInt( unlink_success );
}

void gsc_utils_file_exists()
{
	const char *filename;

	if ( ! stackGetParams("s", &filename))
	{
		stackError("gsc_utils_file_exists() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int file_exists = access(filename, F_OK) != -1;
	stackPushInt( file_exists );
}

void gsc_utils_FS_LoadDir()
{
	const char *path, *dir;

	if ( ! stackGetParams("ss", &path, &dir))
	{
		stackError("gsc_utils_FS_LoadDir() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	FS_AddIwdFilesForGameDirectory(path, dir);
	stackPushBool(qtrue);
}

void gsc_utils_getType()
{
	if (Scr_GetNumParam() == 0)
	{
		stackError("gsc_utils_getType() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushString( stackGetParamTypeAsString(0) );
}

void gsc_utils_float()
{
	if (Scr_GetNumParam() == 0)
	{
		stackError("gsc_utils_float() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	switch (stackGetParamType(0))
	{
	case STACK_STRING:
		const char *asstring;
		stackGetParamString(0, &asstring);
		stackPushFloat( atof(asstring) );
		return;

	case STACK_FLOAT:
		float asfloat;
		stackGetParamFloat(0, &asfloat);
		stackPushFloat( asfloat );
		return;

	case STACK_INT:
		int asinteger;
		stackGetParamInt(0, &asinteger);
		stackPushFloat( float(asinteger) );
		return;

	default:
		stackError("gsc_utils_float() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
}

void gsc_utils_ExecuteString()
{
	const char *str;

	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_utils_ExecuteString() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	Cmd_ExecuteString(str);
	stackPushBool(qtrue);
}

void gsc_utils_sendgameservercommand()
{
	int clientNum;
	const char *message;

	if ( ! stackGetParams("is", &clientNum, &message))
	{
		stackError("gsc_utils_sendgameservercommand() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	SV_GameSendServerCommand(clientNum, SV_CMD_CAN_IGNORE, message);
	stackPushBool(qtrue);
}

void gsc_utils_scandir()
{
	const char *dirname;
	char** files;
	int numfiles;
	int i;

	if ( ! stackGetParams("s", &dirname))
	{
		stackError("gsc_utils_scandir() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	files = Sys_ListFiles(dirname, "", 0, &numfiles, 0);

	if (!numfiles)
	{
		stackPushUndefined();
		return;
	}

	stackPushArray();

	for(i = 0; i < numfiles; i++)
	{
		stackPushString(files[i]);
		stackPushArrayLast();
	}

	Sys_FreeFileList(files);
}

void gsc_utils_fopen()
{
	FILE *file;
	const char *filename, *mode;

	if ( ! stackGetParams("ss", &filename, &mode))
	{
		stackError("gsc_utils_fopen() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	file = fopen(filename, mode);

	if (!file)
	{
		stackError("gsc_utils_fopen() returned a error");
		stackPushUndefined();
		return;
	}

	stackPushInt((int)file);
}

void gsc_utils_fread()
{
	FILE *file;

	if ( ! stackGetParams("i", &file))
	{
		stackError("gsc_utils_fread() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!file)
	{
		stackError("gsc_utils_fread() returned a error");
		stackPushUndefined();
		return;
	}

	char buffer[256];
	int ret = fread(buffer, 1, 255, file);

	if ( ! ret)
	{
		stackPushUndefined();
		return;
	}

	buffer[ret] = '\0';
	stackPushString(buffer);
}

void gsc_utils_fwrite()
{
	FILE *file;
	const char *buffer;

	if ( ! stackGetParams("is", &file, &buffer))
	{
		stackError("gsc_utils_fwrite() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!file)
	{
		stackError("gsc_utils_fwrite() returned a error");
		stackPushUndefined();
		return;
	}

	stackPushInt(fwrite(buffer, 1, strlen(buffer), file));
}

void gsc_utils_fclose()
{
	FILE *file;

	if ( ! stackGetParams("i", &file))
	{
		stackError("gsc_utils_fclose() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!file)
	{
		stackError("gsc_utils_fclose() returned a error");
		stackPushUndefined();
		return;
	}

	stackPushInt( fclose(file) );
}

void gsc_utils_fsize()
{
	FILE *file;

	if ( ! stackGetParams("i", &file))
	{
		stackError("gsc_utils_fsize() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!file)
	{
		stackError("gsc_utils_fsize() returned a error");
		stackPushUndefined();
		return;
	}

	struct stat buf;
	fstat(fileno(file), &buf);

	stackPushInt( buf.st_size );
}

void gsc_utils_ftime()
{
	FILE *file;

	if ( ! stackGetParams("i", &file))
	{
		stackError("gsc_utils_ftime() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!file)
	{
		stackError("gsc_utils_ftime() returned a error");
		stackPushUndefined();
		return;
	}

	struct stat buf;
	fstat(fileno(file), &buf);

	const char *timestring = ctime(&buf.st_mtime);
	char stripped_time[128];

	if (!timestring)
	{
		stackPushUndefined();
		return;
	}

	strncpy(stripped_time, timestring, sizeof(stripped_time));
	stripped_time[strlen(timestring) - 1] = '\0';

	stackPushString( stripped_time );
}

// http://code.metager.de/source/xref/RavenSoftware/jediacademy/code/game/g_utils.cpp#36
void gsc_G_FindConfigstringIndexOriginal()
{
	const char *name;
	int min, max, create;

	if ( ! stackGetParams("siii", &name, &min, &max, &create))
	{
		stackError("gsc_G_FindConfigstringIndexOriginal() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (min < 0 || max >= MAX_CONFIGSTRINGS)
	{
		stackError("gsc_G_FindConfigstringIndexOriginal() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	stackPushInt( G_FindConfigstringIndex(name, min, max, create, "G_FindConfigstringIndex() from GSC") );
}

// simple version, without crash
void gsc_G_FindConfigstringIndex()
{
	const char *name;
	int min, max;

	if ( ! stackGetParams("sii", &name, &min, &max))
	{
		stackError("gsc_G_FindConfigstringIndex() one or more arguments is undefined or has a wrong type");
		return;
	}

	if (min < 0 || max >= MAX_CONFIGSTRINGS)
	{
		stackError("gsc_G_FindConfigstringIndex() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	for (int i = 1; i < max; i++)
	{
		const char *curitem = SV_GetConfigstringConst(min + i);

		if ( ! *curitem)
			break;

		if ( ! strcasecmp(name, curitem))
		{
			stackPushInt(i + min);
			return;
		}
	}

	stackPushBool(qtrue);
}

void gsc_get_configstring()
{
	int index;

	if ( ! stackGetParams("i", &index))
	{
		stackError("gsc_get_configstring() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (index < 0 || index >= MAX_CONFIGSTRINGS)
	{
		stackError("gsc_get_configstring() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	const char *string = SV_GetConfigstringConst(index);

	if ( ! *string )
		stackPushUndefined();
	else
		stackPushString(string);
}

void gsc_set_configstring()
{
	int index;
	const char *string;

	if ( ! stackGetParams("is", &index, &string))
	{
		stackError("gsc_set_configstring() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (index < 0 || index >= MAX_CONFIGSTRINGS)
	{
		stackError("gsc_set_configstring() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	SV_SetConfigstring(index, string);
	stackPushBool(qtrue);
}

void gsc_utils_sqrt()
{
	float x;

	if ( ! stackGetParams("f", &x))
	{
		stackError("gsc_utils_sqrt() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushFloat(sqrt(x));
}

void gsc_utils_sqrtInv()
{
	float x;

	if ( ! stackGetParams("f", &x))
	{
		stackError("gsc_utils_sqrtInv() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	// http://www.beyond3d.com/content/articles/8/
	float xhalf = 0.5f*x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i>>1);
	x = *(float*)&i;
	x = x*(1.5f - xhalf*x*x);

	stackPushFloat(x);
}

void gsc_make_localized_string()
{
	const char *str;

	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_make_localized_string() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushString(str);

	VariableValue *var;
	int param = 0;

	var = &scrVmPub.top[-param];
	var->type = STACK_LOCALIZED_STRING;
}

void gsc_utils_getlasttestclientnumber()
{
	extern int botport;
	stackPushInt(botport);
}

void gsc_utils_bullethiteffect()
{
	vec3_t origin;
	vec3_t normal;

	if ( ! stackGetParams("vv", &origin, &normal))
	{
		stackError("gsc_utils_bullethiteffect() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = G_TempEntity(origin, EV_SHOTGUN_HIT);
	entity->s.eventParm = DirToByte(normal);

	trace_t trace;

	vec3_t end_origin = { origin[0] - (normal[0] * 10), origin[1] - (normal[1] * 10), origin[2] - (normal[2] * 10) };
	G_TraceCapsule(&trace, origin, vec3_origin, vec3_origin, end_origin, 1023, 2065);

	entity->s.surfType = (trace.surfaceFlags >> 20) & 0x1F;

	stackPushBool(qtrue);
}

void gsc_utils_vectorscale()
{
	vec3_t vector;
	float scale;

	if ( ! stackGetParams("vf", &vector, &scale))
	{
		stackError("gsc_utils_vectorscale() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	vec3_t out;

	VectorScale(vector, scale, out);
	stackPushVector(out);
}

void gsc_utils_remove_file()
{
	const char *filename;

	if (!stackGetParams("s", &filename))
	{
		stackError("gsc_utils_remove_file() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushInt(remove( filename ));
}

void gsc_utils_remotecommand()
{
	const char *sFrom;
	int pointerMsg;

	if (!stackGetParams("si", &sFrom, &pointerMsg))
	{
		stackError("gsc_utils_remotecommand() one or more arguments is undefined or has a wrong type");
		return;
	}

	netadr_t from;

	msg_t * msg = (msg_t *)pointerMsg;
	NET_StringToAdr(sFrom, &from);
	SVC_RemoteCommand(from, msg);
}

#endif
