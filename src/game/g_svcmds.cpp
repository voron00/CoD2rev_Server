#include "../qcommon/qcommon.h"
#include "g_shared.h"

#define MAX_CVAR_VALUE_STRING   256

typedef struct ipFilter_s
{
	unsigned mask;
	unsigned compare;
} ipFilter_t;

#define MAX_IPFILTERS   1024

typedef struct ipFilterList_s
{
	ipFilter_t ipFilters[MAX_IPFILTERS];
	int numIPFilters;
} ipFilterList_t;

static ipFilterList_t ipFilters;

extern dvar_t *g_banIPs;

/*
=================
StringToFilter
=================
*/
qboolean StringToFilter( const char *s, ipFilter_t *f )
{
	char num[128];
	int i, j;
	byte b[4];
	byte m[4];

	for ( i = 0 ; i < 4 ; i++ )
	{
		b[i] = 0;
		m[i] = 0;
	}

	for ( i = 0 ; i < 4 ; i++ )
	{
		if ( *s < '0' || *s > '9' )
		{
			if ( *s == '*' )   // 'match any'
			{
				// b[i] and m[i] to 0
				s++;
				if ( !*s )
				{
					break;
				}
				s++;
				continue;
			}
			Com_Printf( "Bad filter address: %s\n", s );
			return qfalse;
		}

		j = 0;
		while ( *s >= '0' && *s <= '9' )
		{
			num[j++] = *s++;
		}
		num[j] = 0;
		b[i] = atoi( num );
		m[i] = 255;

		if ( !*s )
		{
			break;
		}
		s++;
	}

	f->mask = *(unsigned *)m;
	f->compare = *(unsigned *)b;

	return qtrue;
}

/*
=================
UpdateIPBans
=================
*/
static void UpdateIPBans( ipFilterList_t *ipFilterList )
{
	byte b[4];
	byte m[4];
	int i,j;
	char iplist_final[MAX_CVAR_VALUE_STRING];
	char ip[64];

	*iplist_final = 0;
	for ( i = 0 ; i < ipFilterList->numIPFilters ; i++ )
	{
		if ( ipFilterList->ipFilters[i].compare == 0xffffffff )
		{
			continue;
		}

		*(unsigned *)b = ipFilterList->ipFilters[i].compare;
		*(unsigned *)m = ipFilterList->ipFilters[i].mask;
		*ip = 0;
		for ( j = 0; j < 4 ; j++ )
		{
			if ( m[j] != 255 )
			{
				Q_strcat( ip, sizeof( ip ), "*" );
			}
			else
			{
				Q_strcat( ip, sizeof( ip ), va( "%i", b[j] ) );
			}
			Q_strcat( ip, sizeof( ip ), ( j < 3 ) ? "." : " " );
		}
		if ( strlen( iplist_final ) + strlen( ip ) < MAX_CVAR_VALUE_STRING )
		{
			Q_strcat( iplist_final, sizeof( iplist_final ), ip );
		}
	}

	Dvar_SetString(g_banIPs, iplist_final);
}

/*
=================
AddIP
=================
*/
void AddIP( ipFilterList_t *ipFilterList, const char *str )
{
	int i;

	for ( i = 0; i < ipFilterList->numIPFilters; i++ )
	{
		if (  ipFilterList->ipFilters[i].compare == 0xffffffff )
		{
			break;      // free spot
		}
	}

	if ( i == ipFilterList->numIPFilters )
	{
		if ( ipFilterList->numIPFilters == MAX_IPFILTERS )
		{
			Com_Printf( "IP filter list is full\n" );
			return;
		}
		ipFilterList->numIPFilters++;
	}

	if ( !StringToFilter( str, &ipFilterList->ipFilters[i] ) )
	{
		ipFilterList->ipFilters[i].compare = 0xffffffffu;
	}

	UpdateIPBans( ipFilterList );
}

/*
=================
Svcmd_AddIP_f
=================
*/
void Svcmd_AddIP_f( void )
{
	char str[MAX_TOKEN_CHARS];

	if ( SV_Cmd_Argc() < 2 )
	{
		Com_Printf( "Usage:  addip <ip-mask>\n" );
		return;
	}

	SV_Cmd_ArgvBuffer( 1, str, sizeof( str ) );

	AddIP( &ipFilters, str );

}

/*
=================
Svcmd_RemoveIP_f
=================
*/
void Svcmd_RemoveIP_f( void )
{
	ipFilter_t f;
	int i;
	char str[MAX_TOKEN_CHARS];

	if ( SV_Cmd_Argc() < 2 )
	{
		Com_Printf( "Usage:  removeip <ip-mask>\n" );
		return;
	}

	SV_Cmd_ArgvBuffer( 1, str, sizeof( str ) );

	if ( !StringToFilter( str, &f ) )
	{
		return;
	}

	for ( i = 0 ; i < ipFilters.numIPFilters ; i++ )
	{
		if ( ipFilters.ipFilters[i].mask == f.mask   &&
		        ipFilters.ipFilters[i].compare == f.compare )
		{
			ipFilters.ipFilters[i].compare = 0xffffffffu;
			Com_Printf( "Removed.\n" );

			UpdateIPBans( &ipFilters );
			return;
		}
	}

	Com_Printf( "Didn't find %s.\n", str );
}

/*
=================
G_ProcessIPBans
=================
*/
void G_ProcessIPBans( void )
{
	char *s, *t;
	char str[MAX_CVAR_VALUE_STRING];

	ipFilters.numIPFilters = 0;

	Q_strncpyz( str, g_banIPs->current.string, sizeof( str ) );

	for ( t = s = (char *)g_banIPs->current.string; *t; /* */ )
	{
		s = strchr( s, ' ' );
		if ( !s )
		{
			break;
		}
		while ( *s == ' ' )
			*s++ = 0;
		if ( *t )
		{
			AddIP( &ipFilters, t );
		}
		t = s;
	}
}

void Svcmd_EntityList_f()
{
	gentity_s *entity;
	int i;

	entity = &g_entities[1];

	for ( i = 1; i < level.num_entities; ++i, ++entity )
	{
		if ( entity->r.inuse )
		{
			Com_Printf("%3i:", i);

			switch ( entity->s.eType )
			{
			case ET_GENERAL:
				Com_Printf("ET_GENERAL          ");
				break;
			case ET_PLAYER:
				Com_Printf("ET_PLAYER           ");
				break;
			case ET_PLAYER_CORPSE:
				Com_Printf("ET_PLAYER_CORPSE    ");
				break;
			case ET_ITEM:
				Com_Printf("ET_ITEM             ");
				break;
			case ET_MISSILE:
				Com_Printf("ET_MISSILE          ");
				break;
			case ET_INVISIBLE:
				Com_Printf("ET_INVISIBLE        ");
				break;
			case ET_SCRIPTMOVER:
				Com_Printf("ET_SCRIPTMOVER      ");
				break;
			case ET_FX:
				Com_Printf("ET_FX               ");
				break;
			case ET_LOOP_FX:
				Com_Printf("ET_LOOP_FX          ");
				break;
			case ET_TURRET:
				Com_Printf("ET_TURRET           ");
				break;
			default:
				Com_Printf("%3i                 ", entity->s.eType);
				break;
			}

			if ( entity->classname )
			{
				Com_Printf("%s", SL_ConvertToString(entity->classname));
			}

			Com_Printf("\n");
		}
	}
}

qboolean ConsoleCommand()
{
	char cmd[MAX_TOKEN_CHARS];

	SV_Cmd_ArgvBuffer(0, cmd, 1024);

	if ( Q_stricmp( cmd, "entitylist" ) == 0 )
	{
		Svcmd_EntityList_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "addip" ) == 0 )
	{
		Svcmd_AddIP_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "removeip" ) == 0 )
	{
		Svcmd_RemoveIP_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "listip" ) == 0 )
	{
		Cbuf_ExecuteText(1, "g_banIPs\n");
		return qtrue;
	}

	if ( Q_stricmp( cmd, "say" ) == 0 )
	{
		SV_GameSendServerCommand(-1, 0, va("%c \"GAME_SERVER\x15: %s\"", 101, ConcatArgs(1)));
		return qtrue;
	}

	Com_Printf("Unknown command \"%s\"\n", cmd);
	return qfalse;
}