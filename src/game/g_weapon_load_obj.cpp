#include "../qcommon/qcommon.h"
#include "g_shared.h"

const char *accuracyDirName[] =
{
	"aivsai",
	"aivsplayer"
};

bool G_ParseAIWeaponAccurayGraphFile(const char *buffer, const char *fileName, float (*knots)[2], int *knotCount)
{
	int count;
	char *token;

	Com_BeginParseSession(fileName);
	token = Com_Parse(&buffer);
	*knotCount = atoi(token);
	count = 0;

	while ( 1 )
	{
		token = Com_Parse(&buffer);

		if ( !*token )
			break;

		if ( *token == 125 )
			break;

		knots[count][0] = atof(token);
		token = Com_Parse(&buffer);

		if ( !*token || *token == 125 )
			break;

		(*knots)[2 * count + 1] = atof(token);

		if ( ++count > 15 )
		{
			Com_Printf("^3WARNING: \"%s\" has too many graph knots\n", fileName);
			Com_EndParseSession();
			return 0;
		}
	}

	Com_EndParseSession();

	if ( count == *knotCount )
		return 1;

	Com_Printf("^3WARNING: \"%s\" Error in parsing an ai weapon accuracy file\n", fileName);
	return 0;
}

bool G_ParseWeaponAccurayGraphInternal(WeaponDef *weaponDef, const char *dirName, const char *graphName, float (*knots)[2], int *knotCount)
{
	int count;
	size_t n;
	const char *s;
	char buf[8192];
	char src[64];
	fileHandle_t f;
	int len;

	s = "WEAPONACCUFILE";
	n = strlen("WEAPONACCUFILE");

	if ( weaponDef->weaponType && weaponDef->weaponType != WEAPTYPE_PROJECTILE )
		return 1;

	if ( !graphName[0] )
		return 1;

	sprintf(src, "accuracy/%s/%s", dirName, graphName);
	len = FS_FOpenFileByMode(src, &f, FS_READ);

	if ( len >= 0 )
	{
		FS_Read(buf, n, f);
		buf[n] = 0;

		if ( !strncmp(buf, s, n) )
		{
			if ( (int)(len - n) <= 0x1FFF )
			{
				memset(buf, 0, sizeof(buf));
				FS_Read(buf, len - n, f);
				buf[len - n] = 0;
				FS_FCloseFile(f);
				count = 0;

				if ( G_ParseAIWeaponAccurayGraphFile(buf, src, knots, &count) )
				{
					*knotCount = count;
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				Com_Printf("^3WARNING: \"%s\" Is too long of an ai weapon accuracy file to parse\n", src);
				FS_FCloseFile(f);
				return 0;
			}
		}
		else
		{
			Com_Printf("^3WARNING: \"%s\" does not appear to be an ai weapon accuracy file\n", src);
			FS_FCloseFile(f);
			return 0;
		}
	}
	else
	{
		Com_Printf("^3WARNING: Could not load ai weapon accuracy file '%s'\n", src);
		return 0;
	}

	return 0;
}

bool G_ParseWeaponAccurayGraphs(WeaponDef *weaponDef)
{
	int weaponType;
	int size;
	int accuracyGraphKnotCount;
	float accuracyGraphKnots[16][2];

	for ( weaponType = 0; weaponType < 2; ++weaponType )
	{
		memset(accuracyGraphKnots, 0, sizeof(accuracyGraphKnots));
		accuracyGraphKnotCount = 0;

		if ( !G_ParseWeaponAccurayGraphInternal(
		            weaponDef,
		            accuracyDirName[weaponType],
		            weaponDef->accuracyGraphName[weaponType],
		            accuracyGraphKnots,
		            &accuracyGraphKnotCount) )
			return 0;
		if ( accuracyGraphKnotCount > 0 )
		{
			size = COUNT_OF(weaponDef->accuracyGraphKnotCount) * accuracyGraphKnotCount;
			weaponDef->accuracyGraphKnotCount[weaponType] = (intptr_t)Hunk_AllocLowAlignInternal(COUNT_OF(weaponDef->accuracyGraphKnotCount) * accuracyGraphKnotCount, 4);
			memcpy((void *)weaponDef->accuracyGraphKnotCount[weaponType], accuracyGraphKnots, size);
			weaponDef->originalAccuracyGraphKnotCount[weaponType] = accuracyGraphKnotCount;
		}
	}

	return 1;
}