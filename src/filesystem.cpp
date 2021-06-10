/*
===========================================================================
    Copyright (C) 1999-2005 Id Software, Inc.
    Copyright (C) 2010-2013  Ninja and TheKelm
    This file is part of CoD4X18-Server source code.
    CoD4X18-Server source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.
    CoD4X18-Server source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.
    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/

#include "i_common.h"
#include "i_shared.h"
#include "sys_main.h"
#include "dvar.h"
#include "filesystem.h"
#include "cmd.h"

#define BASEGAME "main"
#define fs_gamedirvar fs_gameDirVar

dvar_t* fs_debug;
dvar_t* fs_copyfiles;
dvar_t* fs_cdpath;
dvar_t* fs_basepath;
dvar_t* fs_basegame;
dvar_t* fs_useOldAssets;
dvar_t* fs_homepath;
dvar_t* fs_gameDirVar;
dvar_t* fs_restrict;
dvar_t* fs_ignoreLocalized;

dvar_t* loc_language;
dvar_t* loc_forceEnglish;
dvar_t* loc_translate;
dvar_t* loc_warnings;
dvar_t* loc_warningsAsErrors;

qboolean g_currentAsian;

static searchpath_t *fs_searchpaths;
static fileHandleData_t fsh[MAX_FILE_HANDLES +1];
static char fs_gamedir[MAX_OSPATH];

int fs_packFiles;
int fs_checksumFeed;
static int fs_loadStack;

char lastValidBase[MAX_OSPATH];
char lastValidGame[MAX_OSPATH];

/*
================
FS_fplength
================
*/
long FS_fplength(FILE *h)
{
	long		pos;
	long		end;

	pos = ftell(h);
	fseek(h, 0, SEEK_END);
	end = ftell(h);
	fseek(h, pos, SEEK_SET);

	return end;
}

/*
==============
FS_Initialized
==============
*/
qboolean FS_Initialized()
{
	return fs_searchpaths != NULL;
}

/*
====================
FS_ReplaceSeparators
Fix things up differently for win/unix/mac
====================
*/
void FS_ReplaceSeparators( char *path )
{
	char	*s;

	for ( s = path ; *s ; s++ )
	{
		if ( *s == '/' || *s == '\\' )
		{
			*s = PATH_SEP;
		}
	}
}

/*
===========
FS_PathCmp
Ignore case and seprator char distinctions
===========
*/
int FS_PathCmp( const char *s1, const char *s2 )
{
	int		c1, c2;

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if (c1 >= 'a' && c1 <= 'z')
		{
			c1 -= ('a' - 'A');
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 -= ('a' - 'A');
		}

		if ( c1 == '\\' || c1 == ':' )
		{
			c1 = '/';
		}
		if ( c2 == '\\' || c2 == ':' )
		{
			c2 = '/';
		}

		if (c1 < c2)
		{
			return -1;		// strings not equal
		}
		if (c1 > c2)
		{
			return 1;
		}
	}
	while (c1);

	return 0;		// strings are equal
}

static FILE *FS_FileForHandle( fileHandle_t f )
{
	return fsh[f].handleFiles.file.o;
}

/*
================
FS_filelength
If this is called on a non-unique FILE (from a pak file),
it will return the size of the pak file, not the expected
size of the file.
================
*/
int FS_filelength( fileHandle_t f )
{
	int		pos;
	int		end;
	FILE*	h;

	h = FS_FileForHandle(f);
	pos = ftell (h);
	fseek (h, 0, SEEK_END);
	end = ftell (h);
	fseek (h, pos, SEEK_SET);

	return end;
}

/*
================
return a hash value for the filename
================
*/
long FS_HashFileName( const char *fname, int hashSize )
{
	int i;
	long hash;
	char letter;

	hash = 0;
	i = 0;
	while ( fname[i] != '\0' )
	{
		letter = tolower( fname[i] );
		if ( letter == '.' )
		{
			break;                          // don't include extension
		}
		if ( letter == '\\' )
		{
			letter = '/';                   // damn path names
		}
		if ( letter == PATH_SEP )
		{
			letter = '/';                           // damn path names
		}
		hash += (long)( letter ) * ( i + 119 );
		i++;
	}
	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	hash &= ( hashSize - 1 );
	return hash;
}

/*
===========
FS_FilenameCompare
Ignore case and seprator char distinctions
===========
*/
qboolean FS_FilenameCompare( const char *s1, const char *s2 )
{
	int		c1, c2;

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if (c1 >= 'a' && c1 <= 'z')
		{
			c1 -= ('a' - 'A');
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 -= ('a' - 'A');
		}

		if ( c1 == '\\' || c1 == ':' )
		{
			c1 = '/';
		}
		if ( c2 == '\\' || c2 == ':' )
		{
			c2 = '/';
		}

		if (c1 != c2)
		{
			return -1;		// strings not equal
		}
	}
	while (c1);

	return 0;		// strings are equal
}

qboolean FS_FilesAreLoadedGlobally(const char *filename)
{
	const char *extensions[10];
	int filenameLen;
	int extensionNum;

	extensions[0] = ".hlsl";
	extensions[1] = ".txt";
	extensions[2] = ".cfg";
	extensions[3] = ".levelshots";
	extensions[4] = ".menu";
	extensions[5] = ".arena";
	extensions[6] = ".str";
	extensions[7] = ".so";
	extensions[8] = ".dll";
	extensions[8] = ".sum";
	extensions[9] = "";
	filenameLen = strlen(filename);
	for ( extensionNum = 0; *extensions[extensionNum]; ++extensionNum )
	{
		if ( !I_stricmp(&filename[filenameLen - strlen(extensions[extensionNum])], extensions[extensionNum]) )
		{
			return qtrue;
		}
	}
	return qfalse;
}

typedef struct fsPureSums_s
{

	struct fsPureSums_s *next;
	int checksum;
	char baseName[MAX_OSPATH];
	char gameName[MAX_OSPATH];

} fsPureSums_t;

static fsPureSums_t *fs_iwdPureChecks;
void FS_AddIwdPureCheckReference(searchpath_t *search)
{

	fsPureSums_t *checks;
	fsPureSums_t *newCheck;

	if( search->localized )
	{
		return;
	}

	for(checks = fs_iwdPureChecks; checks != NULL ; checks = checks->next)
	{
		if ( checks->checksum == search->pack->checksum )
		{
			if ( !I_stricmp(checks->baseName, search->pack->pakBasename) )
			{
				return;
			}
		}
	}
	newCheck = (fsPureSums_t *)Z_Malloc(sizeof(fsPureSums_t));
	newCheck->next = NULL;
	newCheck->checksum = search->pack->checksum;
	I_strncpyz(newCheck->baseName, search->pack->pakBasename, sizeof(newCheck->baseName));
	I_strncpyz(newCheck->gameName, search->pack->pakGamename, sizeof(newCheck->gameName));

	if(fs_iwdPureChecks == NULL)
	{
		fs_iwdPureChecks = newCheck;
		return;
	}

	for( checks = fs_iwdPureChecks; checks->next != NULL; checks = checks->next );

	checks->next = newCheck;
}

void FS_ShutdownIwdPureCheckReferences()
{
	fsPureSums_t *cur;
	fsPureSums_t *next;

	cur = fs_iwdPureChecks;

	while( cur )
	{
		next = cur->next;
		Z_Free( cur );
		cur = next;
	}
	fs_iwdPureChecks = 0;
}

unsigned Com_BlockChecksumKey32(void* buffer, int length, int key)
{
	int i, j;
	unsigned int q = ~key;
	byte* val = (byte*)buffer;

	for(i = 0; i < length; i++)
	{
		q = val[i] ^ q;

		for(j = 0; j < 8; j++)
		{
			if(q & 1)
				q = (q >> 1) ^ 0xEDB88320;
			else
				q = (q >> 1) ^ 0;
		}
	}
	return ~q;
}

static fileHandle_t FS_HandleForFile( void )
{
	int i;

	for ( i = 1 ; i < MAX_FILE_HANDLES ; i++ )
	{
		if ( fsh[i].handleFiles.file.o == NULL )
		{
			return i;
		}
	}
	Com_Error( ERR_DROP, "FS_HandleForFile: none free" );
	return 0;
}

static void FS_SetFilenameForHandle( fileHandle_t f, const char* filename )
{
	if ( f < 0 || f > MAX_FILE_HANDLES )
	{
		Com_Error( ERR_DROP, "FS_SetFilenameForHandle: out of range %i\n", f);
	}
	I_strncpyz(fsh[f].name, filename, sizeof(fsh[f].name));
}

void FS_BuildOSPath_Internal(const char *base, const char *game, const char *qpath, char *fs_path, int fs_thread)
{
	char basename[MAX_OSPATH];
	char gamename[MAX_OSPATH];

	int len;

	if ( !game || !*game )
		game = fs_gamedir;

	I_strncpyz(basename, base, sizeof(basename));
	I_strncpyz(gamename, game, sizeof(gamename));

	len = strlen(basename);

	if(len > 0 && (basename[len -1] == '/' || basename[len -1] == '\\'))
	{
		basename[len -1] = '\0';
	}

	len = strlen(gamename);

	if(len > 0 && (gamename[len -1] == '/' || gamename[len -1] == '\\'))
	{
		gamename[len -1] = '\0';
	}

	if ( Com_sprintf(fs_path, MAX_OSPATH, "%s/%s/%s", basename, gamename, qpath) >= MAX_OSPATH )
	{
		if ( fs_thread )
		{
			fs_path[0] = 0;
			return;
		}

		Com_Error(ERR_FATAL, "FS_BuildOSPath: os path length exceeded");
	}

	FS_ReplaceSeparators(fs_path);
}

void FS_BuildOSPath(const char *base, const char *game, const char *qpath, char *fs_path)
{
	FS_BuildOSPath_Internal(base, game, qpath, fs_path, 0);
}

static const char* g_languages[] = {"english", "french", "german", "italian",
                                    "spanish", "british", "russian", "polish",
                                    "korean", "taiwanese", "japanese", "chinese",
                                    "thai", "leet", "czech"
                                   };

int SEH_GetCurrentLanguage(void)
{
	return loc_language->current.integer;
}

const char* SEH_GetLanguageName(unsigned int langindex)
{
	if(langindex > 14)
		return g_languages[0];

	return g_languages[langindex];
}

qboolean SEH_GetLanguageIndexForName(const char* language, int *langindex)
{
	int i;

	for(i = 0; i < 15; i++)
	{
		if(!I_stricmp(language, g_languages[i]))
		{
			*langindex = i;
			return qtrue;
		}
	}
	return qfalse;
}

void FS_ShutdownSearchpath(searchpath_t *clear)
{
	searchpath_t    **back, *p;

	back = &fs_searchpaths;
	while ( 1 )
	{
		p = *back;
		if( p == NULL )
		{
			return;
		}
		if(p == clear)
		{
			*back = p->next;
			if ( p->pack )
			{
				unzClose( p->pack->handle );
				Z_Free( p->pack->buildBuffer );
				Z_Free( p->pack );
			}
			if ( p->dir )
			{
				Z_Free( p->dir );
			}
			Z_Free( p );
			return;
		}
		back = &p->next;
	}
}

static pack_t *FS_LoadZipFile( char *zipfile, const char *basename )
{
	fileInPack_t    *buildBuffer;
	pack_t          *pack;
	unzFile uf;
	int err;
	unz_global_info gi;
	char filename_inzip[MAX_ZPATH];
	unz_file_info file_info;
	unsigned int i, len;
	int x;
	long hash;
	int fs_numHeaderLongs;
	int             *fs_headerLongs;
	char            *namePtr;

	fs_numHeaderLongs = 0;

	uf = unzOpen( zipfile );
	err = unzGetGlobalInfo( uf,&gi );

	if ( err != UNZ_OK )
	{
		return NULL;
	}

	fs_packFiles += gi.number_entry;

	len = 0;
	unzGoToFirstFile( uf );
	for ( i = 0; i < gi.number_entry; i++ )
	{
		err = unzGetCurrentFileInfo( uf, &file_info, filename_inzip, sizeof( filename_inzip ), NULL, 0, NULL, 0 );
		if ( err != UNZ_OK )
		{
			break;
		}
		len += strlen( filename_inzip ) + 1;
		unzGoToNextFile( uf );
	}

	buildBuffer = (fileInPack_t *)Z_Malloc( ( gi.number_entry * sizeof( fileInPack_t ) ) + len );
	namePtr = ( (char *) buildBuffer ) + gi.number_entry * sizeof( fileInPack_t );
	fs_headerLongs = (int *)Z_Malloc( gi.number_entry * sizeof( int ) );

	// get the hash table size from the number of files in the zip
	// because lots of custom pk3 files have less than 32 or 64 files
	for ( i = 1; i <= MAX_FILEHASH_SIZE; i <<= 1 )
	{
		if ( i > gi.number_entry )
		{
			break;
		}
	}

	pack = (pack_t *)Z_Malloc( sizeof( pack_t ) + i * sizeof( fileInPack_t * ) );
	pack->hashSize = i;
	pack->hashTable = ( fileInPack_t ** )( ( (char *) pack ) + sizeof( pack_t ) );

	for ( x = 0; x < pack->hashSize; x++ )
	{
		pack->hashTable[x] = NULL;
	}

	I_strncpyz( pack->pakFilename, zipfile, sizeof( pack->pakFilename ) );
	I_strncpyz( pack->pakBasename, basename, sizeof( pack->pakBasename ) );

	// strip .pk3 if needed
	if ( strlen( pack->pakBasename ) > 4 && !I_stricmp( pack->pakBasename + strlen( pack->pakBasename ) - 4, ".iwd" ) )
	{
		pack->pakBasename[strlen( pack->pakBasename ) - 4] = 0;
	}

	pack->handle = uf;
	pack->numfiles = gi.number_entry;
	pack->hasOpenFile = 0;
	unzGoToFirstFile( uf );

	for ( i = 0; i < gi.number_entry; i++ )
	{
		err = unzGetCurrentFileInfo( uf, &file_info, filename_inzip, sizeof( filename_inzip ), NULL, 0, NULL, 0 );
		if ( err != UNZ_OK )
		{
			break;
		}
		if ( file_info.uncompressed_size > 0 )
		{
			fs_headerLongs[fs_numHeaderLongs++] = LittleLong( file_info.crc );
		}
		I_strlwr( filename_inzip );
		hash = FS_HashFileName( filename_inzip, pack->hashSize );
		buildBuffer[i].name = namePtr;
		strcpy( buildBuffer[i].name, filename_inzip );
		namePtr += strlen( filename_inzip ) + 1;
		// store the file position in the zip
		buildBuffer[i].pos = unzGetOffset( uf );
		buildBuffer[i].next = pack->hashTable[hash];
		pack->hashTable[hash] = &buildBuffer[i];
		unzGoToNextFile( uf );
	}

	pack->checksum = Com_BlockChecksumKey32( fs_headerLongs, 4 * fs_numHeaderLongs, LittleLong( 0 ) );
	if(fs_checksumFeed)
		pack->pure_checksum = Com_BlockChecksumKey32( fs_headerLongs, 4 * fs_numHeaderLongs, LittleLong( fs_checksumFeed ) );
	else
		pack->pure_checksum = pack->checksum;

	pack->checksum = LittleLong( pack->checksum );
	pack->pure_checksum = LittleLong( pack->pure_checksum );

	Z_Free( fs_headerLongs );

	pack->buildBuffer = buildBuffer;
	return pack;
}

static const char* IwdFileLanguage(const char *instr)
{
	signed int i;
	static qboolean flip;
	static char Array64[128];

	flip ^= 1u;
	if ( strlen(instr) >= 10 )
	{
		Com_Memset(&Array64[64 * flip], 0, 64);
		for(i = 10; i < 64 && instr[i] != '\0' && isalpha(instr[i]) != '\0'; i++)
		{
			Array64[(64 * flip) + i - 10] = instr[i];
		}
		return &Array64[64 * flip];
	}

	Array64[64 * flip] = 0;
	return &Array64[64 * flip];
}

static signed int iwdsort(const void *cmp1_arg, const void *cmp2_arg)
{
	const char* cmp1;
	const char* cmp2;

	cmp1 = *(const char**)cmp1_arg;
	cmp2 = *(const char**)cmp2_arg;

	if(I_stricmpn(cmp1, "          ", 10) || I_stricmpn(cmp2, "          ", 10) )
		return FS_PathCmp(cmp1, cmp2);

	if ( I_stricmp( IwdFileLanguage(cmp1), "english") )
	{
		if ( !I_stricmp( IwdFileLanguage(cmp2), "english") )
			return 1;
	}
	else
	{
		if ( I_stricmp( IwdFileLanguage(cmp2), "english") )
			return -1;
	}
	return FS_PathCmp(cmp1, cmp2);
}

#define MAX_PAKFILES 1024
void FS_AddIwdFilesForGameDirectory(const char *path, const char *dir)
{
	searchpath_t *search, *prev, *sp;
	int langindex;
	int numfiles;
	const char* language;
	qboolean islocalized;
	int i, j;
	pack_t* pak;
	char** pakfiles;
	char pakfile[MAX_OSPATH];
	char* sorted[MAX_PAKFILES];
	qboolean languagesListed;

	FS_BuildOSPath(path, dir, "", pakfile);
	pakfile[strlen(pakfile) - 1] = 0;
	pakfiles = Sys_ListFiles(pakfile, "iwd", 0, &numfiles, 0);

	if(!pakfiles)
		return;

	if ( numfiles > MAX_PAKFILES )
	{
		Com_Printf("WARNING: Exceeded max number of iwd files in %s %s (%d %d)\n", path, dir, numfiles, MAX_PAKFILES);
		numfiles = MAX_PAKFILES;
	}
	if ( !numfiles && !I_stricmp(dir, BASEGAME) && !I_stricmp(path, fs_basepath->current.string) )
		Com_Error(ERR_FATAL, "No IWD files found in /main");

	for(i = 0; i < numfiles; i++)
	{
		sorted[i] = pakfiles[i];

		if ( !I_strncmp(sorted[i], "localized_", 10) )
		{
			Com_Memcpy(sorted[i],  "          ", 10);
		}
	}

	qsort(sorted, numfiles, 4, iwdsort);

	langindex = 0;
	languagesListed = 0;

	for(i = 0; i < numfiles; i++)
	{
		if(!I_strncmp(sorted[i], "          ", 10))
		{
			Com_Memcpy(sorted[i],  "localized_", 10);
			language = IwdFileLanguage(sorted[i]);
			if ( !language[0] )
			{
				Com_Printf("WARNING: Localized assets iwd file %s/%s/%s has invalid name (no language specified). Proper naming convention is: localized_[language]_iwd#.iwd\n", path, dir, sorted[i]);
				continue;
			}
			if ( !SEH_GetLanguageIndexForName(language, &langindex))
			{
				Com_Printf("WARNING: Localized assets iwd file %s/%s/%s has invalid name (bad language name specified). Proper naming convention is: localized_[language]_iwd#.iwd\n", path, dir, sorted[i]);
				if ( !languagesListed )
				{
					Com_Printf("Supported languages are:\n");
					for(j = 0; j < 15; j++)
					{
						Com_Printf("    %s\n", SEH_GetLanguageName(j));
					}
					languagesListed = 1;
				}
				continue;
			}
			islocalized = qtrue;
		}
		else
		{
			if ( !I_stricmp(dir, BASEGAME) && !I_stricmp(path, fs_basepath->current.string) && I_stricmpn(sorted[i], "iw_", 3) && I_stricmpn(sorted[i], "jcod4x_", 7) && I_stricmpn(sorted[i], "xbase_", 6))
			{
				Com_Printf("WARNING: Invalid IWD %s in \\main.\n", sorted[i]);
				continue;
			}
			islocalized = qfalse;
		}

		FS_BuildOSPath(path, dir, sorted[i], pakfile);
		pak = FS_LoadZipFile( pakfile, sorted[i]);
		if(pak == NULL)
		{
			continue;
		}
		/* Shutdown already loaded pak files with same name to circumvent conflicts */
		for(sp = fs_searchpaths; sp != NULL; sp = sp->next)
		{
			if(sp->pack != NULL && !I_stricmp(sp->pack->pakBasename, pak->pakBasename))
			{
				FS_ShutdownSearchpath(sp);
				break; //Break out - sp is now invalid
			}
		}

		I_strncpyz(pak->pakGamename, dir, sizeof(pak->pakGamename));

		search = (searchpath_t *)S_Malloc(sizeof(searchpath_t));
		search->pack = pak;
		search->localized = islocalized;
		search->langIndex = langindex;

		prev = (searchpath_t*)&fs_searchpaths;
		sp = fs_searchpaths;

		if (search->localized)
		{
			for (sp = fs_searchpaths; sp != NULL && !sp->localized; sp = sp->next)
			{
				prev = sp;
			}
			Dvar_SetInt(loc_language, search->langIndex);
		}
		search->next = sp;
		prev->next = search;
	}

	Sys_FreeFileList(pakfiles);
}

void FS_AddGameDirectory_Single(const char *path, const char *dir_nolocal, qboolean localized, int index)
{
	const char* localization;
	searchpath_t *search;
	searchpath_t *sp;
	searchpath_t *prev;
	const char *language_name;
	char ospath[MAX_OSPATH];
	char dir[MAX_QPATH];

	if ( localized )
	{
		language_name = SEH_GetLanguageName(index);
		Com_sprintf(dir, sizeof(dir), "%s/%s", dir_nolocal, language_name);
	}
	else
	{
		I_strncpyz(dir, dir_nolocal, sizeof(dir));
	}

	for (sp = fs_searchpaths ; sp ; sp = sp->next)
	{
		if ( sp->dir && !I_stricmp(sp->dir->path, path) && !I_stricmp(sp->dir->gamedir, dir) )
		{
			if ( localized != sp->localized )
			{
				localization = "localized";
				if ( !sp->localized )
					localization = "non-localized";
				Com_Printf("WARNING: game folder %s/%s added as both localized & non-localized. Using folder as %s\n", path, dir, localization);
			}
			if ( sp->localized && index != sp->localized )
				Com_Printf("WARNING: game folder %s/%s re-added as localized folder with different language\n", path, dir);

			return;
		}
	}

	if ( localized )
	{
		FS_BuildOSPath(path, dir, "", ospath);
		if(ospath[0])
			ospath[strlen(ospath) -1] = 0;
		if ( !Sys_DirectoryHasContents(ospath) )
		{
			return;
		}
	}
	else
	{
		I_strncpyz(fs_gamedir, dir, MAX_OSPATH);
	}

	search = (searchpath_t *)S_Malloc(sizeof(searchpath_t));
	search->dir = (directory_t *)Z_Malloc(sizeof(directory_t));
	I_strncpyz(search->dir->path, path, sizeof(search->dir->path));
	I_strncpyz(search->dir->gamedir, dir, sizeof(search->dir->gamedir));
	search->localized = localized;
	search->langIndex = index;

	prev = (searchpath_t*)&fs_searchpaths;
	sp = fs_searchpaths;

	if (search->localized)
	{
		for (sp = fs_searchpaths; sp != NULL && !sp->localized; sp = sp->next)
		{
			prev = sp;
		}
	}

	search->next = sp;
	prev->next = search;

	FS_AddIwdFilesForGameDirectory(path, dir);
}

void FS_DisplayPath( void )
{
	searchpath_t    *s;
	int i;

	Com_Printf("Current language: %s\n", SEH_GetLanguageName(SEH_GetCurrentLanguage()));
	Com_Printf("Current fs_basepath: %s\n", fs_basepath->current.string);
	Com_Printf("Current fs_homepath: %s\n", fs_homepath->current.string);
	if ( fs_ignoreLocalized->current.integer)
		Com_Printf("    localized assets are being ignored\n");

	Com_Printf( "Current search path:\n" );
	for ( s = fs_searchpaths; s; s = s->next )
	{
		if ( s->pack )
		{
			Com_Printf( "%s (%i files)\n", s->pack->pakFilename, s->pack->numfiles );
			if ( s->localized )
			{
				Com_Printf("    localized assets iwd file for %s\n", SEH_GetLanguageName(s->langIndex));
			}
		}
		else
		{
			Com_Printf( "%s/%s\n", s->dir->path, s->dir->gamedir );
			if ( s->localized )
			{
				Com_Printf("    localized assets game folder for %s\n", SEH_GetLanguageName(s->langIndex));
			}
		}
	}
	Com_Printf("\nFile Handles:\n");
	for ( i = 1 ; i < MAX_FILE_HANDLES ; i++ )
	{
		if ( fsh[i].handleFiles.file.o )
		{
			Com_Printf( "handle %i: %s\n", i, fsh[i].name );
		}
	}
}

void FS_AddGameDirectory(const char *path, const char *dir)
{
	FS_AddGameDirectory_Single(path, dir, qtrue, 0);
	FS_AddGameDirectory_Single(path, dir, qfalse, 0);
}

void FS_RegisterDvars()
{
	char *homePath;

	fs_debug = Dvar_RegisterInt("fs_debug", 0, 0, 2, 0x1000u);
	fs_copyfiles = Dvar_RegisterBool("fs_copyfiles", 0, 0x1010u);
	fs_cdpath = Dvar_RegisterString("fs_cdpath", Sys_DefaultCDPath(), 0x1010u);
	fs_basepath = Dvar_RegisterString("fs_basepath", Sys_DefaultInstallPath(), 0x1010u);
	fs_basegame = Dvar_RegisterString("fs_basegame", "", 0x1010u);
	fs_useOldAssets = Dvar_RegisterBool("fs_useOldAssets", 0, 0x1000u);
	homePath = (char *)Sys_DefaultHomePath();
	if (!homePath || !homePath[0])
		homePath = Sys_Cwd();
	fs_homepath = Dvar_RegisterString("fs_homepath", homePath, 0x1010u);
	fs_gameDirVar = Dvar_RegisterString("fs_game", "", 0x101Cu);
	fs_restrict = Dvar_RegisterBool("fs_restrict", 0, 0x1010u);
	fs_ignoreLocalized = Dvar_RegisterBool("fs_ignoreLocalized", 0, 0x10A0u);
}

const char **FS_ListFiles(const char *path, const char *extension, int behavior, int *numfiles)
{
	return (const char**)Sys_ListFiles(path, extension, 0, numfiles, qfalse);
}

void FS_FreeFileList(const char** list)
{
	Sys_FreeFileList((char**)list);
}

void FS_Dir_f( void )
{
	const char	*path;
	const char	*extension;
	const char	**dirnames;
	int		ndirs;
	int		i;

	if ( Cmd_Argc() < 2 || Cmd_Argc() > 3 )
	{
		Com_Printf( "usage: dir <directory> [extension]\n" );
		return;
	}

	if ( Cmd_Argc() == 2 )
	{
		path = Cmd_Argv( 1 );
		extension = "";
	}
	else
	{
		path = Cmd_Argv( 1 );
		extension = Cmd_Argv( 2 );
	}

	Com_Printf( "Directory of %s %s\n", path, extension );
	Com_Printf( "---------------\n" );

	dirnames = FS_ListFiles( path, extension, 0, &ndirs );

	for ( i = 0; i < ndirs; i++ )
	{
		Com_Printf( "%s\n", dirnames[i] );
	}
	FS_FreeFileList( dirnames );
}

void FS_Path_f(void)
{
	FS_DisplayPath();
}

void FS_FullPath_f(void)
{
	FS_DisplayPath();
}

void FS_AddCommands()
{
	Cmd_AddCommand("path", FS_Path_f);
	Cmd_AddCommand("fullpath", FS_FullPath_f);
	Cmd_AddCommand("dir", FS_Dir_f);
	//Cmd_AddCommand("fdir", FS_NewDir_f);
	//Cmd_AddCommand("touchFile", FS_TouchFile_f);
}

int FS_LoadStack()
{
	int val;

	val = fs_loadStack;

	return val;
}

void FS_LoadStackInc()
{
	fs_loadStack++;
}

void FS_LoadStackDec()
{
	fs_loadStack--;
}

void FS_FCloseFile( fileHandle_t f )
{
	if ( !fs_searchpaths )
	{
		Com_Error( ERR_FATAL, "Filesystem call made without initialization\n" );
	}

	if ( fsh[f].streamed )
	{
		Sys_EndStreamedFile( f );
	}
	if ( fsh[f].zipFile == qtrue )
	{
		unzCloseCurrentFile( fsh[f].handleFiles.file.z );
		if ( fsh[f].handleFiles.unique )
		{
			unzClose( fsh[f].handleFiles.file.z );
		}
		Com_Memset( &fsh[f], 0, sizeof( fsh[f] ) );
		return;
	}

	// we didn't find it as a pak, so close it as a unique file
	if ( fsh[f].handleFiles.file.o )
	{
		fclose( fsh[f].handleFiles.file.o );
	}
	Com_Memset( &fsh[f], 0, sizeof( fsh[f] ) );
}

/*
=============
FS_FreeFile
=============
*/
void FS_FreeFile( void *buffer )
{

	if ( !buffer )
	{
		Com_Error( ERR_FATAL, "FS_FreeFile( NULL )" );
	}
	FS_LoadStackDec();

	free( buffer );
}

/*
 =============
 FS_FreeFileKeepBuf
 =============
 */
void FS_FreeFileKeepBuf( )
{
	FS_LoadStackDec();
}

int FS_Read( void *buffer, int len, fileHandle_t f )
{
	int		block, remaining;
	int		read;
	byte	*buf;
	int		tries;

	if ( !FS_Initialized() )
	{
		Com_Error( ERR_FATAL, "Filesystem call made without initialization" );
	}

	if ( !f )
	{
		return 0;
	}

	buf = (byte *)buffer;

	if (fsh[f].zipFile == qfalse)
	{
		remaining = len;
		tries = 0;
		while (remaining)
		{
			block = remaining;
			read = fread (buf, 1, block, fsh[f].handleFiles.file.o);
			if (read == 0)
			{
				// we might have been trying to read from a CD, which
				// sometimes returns a 0 read on windows
				if (!tries)
				{
					tries = 1;
				}
				else
				{
					return len-remaining;	//Com_Error (ERR_FATAL, "FS_Read: 0 bytes read");
				}
			}

			if (read == -1)
			{
				Com_Error (ERR_FATAL, "FS_Read: -1 bytes read");
			}

			remaining -= read;
			buf += read;
		}
		return len;
	}
	else
	{
		return unzReadCurrentFile(fsh[f].handleFiles.file.z, buffer, len);
	}
}

long FS_FOpenFileReadDir(const char *filename, searchpath_t *search, fileHandle_t *file, qboolean uniqueFILE, qboolean unpure, int FsThread)
{
	long			hash;
	pack_t		*pak;
	fileInPack_t	*pakFile;
	directory_t	*dir;
	char		netpath[MAX_OSPATH];
	FILE		*filep;
	int		len;
	int		err;
	unz_file_info	file_info;

	if(filename == NULL)
		Com_Error(ERR_FATAL, "FS_FOpenFileRead: NULL 'filename' parameter passed");

	// qpaths are not supposed to have a leading slash
	if(filename[0] == '/' || filename[0] == '\\')
		filename++;

	// make absolutely sure that it can't back up the path.
	// The searchpaths do guarantee that something will always
	// be prepended, so we don't need to worry about "c:" or "//limbo"
	if(strstr(filename, ".." ) || strstr(filename, "::"))
	{
		if(file == NULL)
			return qfalse;

		*file = 0;
		return -1;
	}
	if(file == NULL)
	{
		// just wants to see if file is there

		/* Cod4 adds localization check */
		if(search->localized && !fs_ignoreLocalized->current.boolean && search->langIndex != loc_language->current.integer)
			return 0;

		// is the element a pak file?
		if(search->pack)
		{
			hash = FS_HashFileName(filename, search->pack->hashSize);

			if(search->pack->hashTable[hash])
			{
				// look through all the pak file elements
				pak = search->pack;
				pakFile = pak->hashTable[hash];

				do
				{
					// case and separator insensitive comparisons
					if(!FS_FilenameCompare(pakFile->name, filename))
					{
						err = unzLocateFile( pak->handle, pakFile->name, 2);
						if(err != UNZ_OK)
						{
							Com_Printf("Read error in Zip-file: %s\n", pak->pakFilename);
							return 1;
						}
						err = unzGetCurrentFileInfo( pak->handle, &file_info, netpath, sizeof( netpath ), NULL, 0, NULL, 0 );
						if(err != UNZ_OK)
						{
							Com_Printf("Read error in Zip-file: %s\n", pak->pakFilename);
							return 1;
						}

						// found it!
						if(file_info.uncompressed_size)
							return file_info.uncompressed_size;
						else
						{
							// It's not nice, but legacy code depends
							// on positive value if file exists no matter
							// what size
							return 1;
						}
					}

					pakFile = pakFile->next;
				}
				while(pakFile != NULL);
			}
		}
		else if(search->dir)
		{
			dir = search->dir;

			FS_BuildOSPath_Internal(dir->path, dir->gamedir, filename, netpath, FsThread);
			filep = fopen (netpath, "rb");

			if(filep)
			{
				len = FS_fplength(filep);
				fclose(filep);

				if(len)
					return len;
				else
					return 1;
			}
		}

		return -1;
	}
	*file = FS_HandleForFile();

	if(*file == 0)
		return -1;

	FS_SetFilenameForHandle(*file, filename);

	fsh[*file].handleFiles.unique = uniqueFILE;

	// is the element a pak file?
	if(search->pack)
	{

		hash = FS_HashFileName(filename, search->pack->hashSize);

		if(search->pack->hashTable[hash])
		{
			// look through all the pak file elements
			pak = search->pack;
			pakFile = pak->hashTable[hash];

			do
			{
				// case and separator insensitive comparisons
				if(!FS_FilenameCompare(pakFile->name, filename))
				{
					// found it!

					// mark the pak as having been referenced and mark specifics on cgame and ui
					// shaders, txt, arena files  by themselves do not count as a reference as
					// these are loaded from all pk3s
					// from every pk3 file..
					len = strlen(filename);

					if (!(pak->referenced & FS_GENERAL_REF) && !FS_FilesAreLoadedGlobally(filename))
					{
						pak->referenced |= FS_GENERAL_REF;
						FS_AddIwdPureCheckReference(search);
					}

					if(uniqueFILE)
					{
						// open a new file on the pakfile
						fsh[*file].handleFiles.file.z = unzOpen(pak->pakFilename);

						if(fsh[*file].handleFiles.file.z == NULL)
						{
							Com_Error(ERR_FATAL, "Couldn't open %s", pak->pakFilename);
						}
					}
					else
					{
						fsh[*file].handleFiles.file.z = pak->handle;
					}
					I_strncpyz(fsh[*file].name, filename, sizeof(fsh[*file].name));
					fsh[*file].zipFile = qtrue;

					// set the file position in the zip file (also sets the current file info)
					unzSetOffset(fsh[*file].handleFiles.file.z, pakFile->pos);

					// open the file in the zip
					if(unzOpenCurrentFile(fsh[*file].handleFiles.file.z) != UNZ_OK)
					{
						Com_Printf("FS_FOpenFileReadDir: Failed to open Zip-File\n");
					}
					fsh[*file].zipFilePos = pakFile->pos;

					if(fs_debug->current.integer)
					{
						Sys_Print(va("FS_FOpenFileRead: %s (found in '%s')\n", filename, pak->pakFilename));
					}

					err = unzGetCurrentFileInfo( fsh[*file].handleFiles.file.z, &file_info, netpath, sizeof( netpath ), NULL, 0, NULL, 0 );
					if(err != UNZ_OK)
					{
						Com_Printf("Read error in Zip-file: %s\n", pak->pakFilename);
						return 1;
					}
					if(file_info.uncompressed_size)
					{
						return file_info.uncompressed_size;
					}
					return 1;
				}

				pakFile = pakFile->next;
			}
			while(pakFile != NULL);
		}
	}
	else if(search->dir)
	{
		// check a file in the directory tree
		dir = search->dir;

		FS_BuildOSPath_Internal(dir->path, dir->gamedir, filename, netpath, FsThread);
		filep = fopen(netpath, "rb");

		if (filep == NULL)
		{
			*file = 0;
			return -1;
		}

		I_strncpyz(fsh[*file].name, filename, sizeof(fsh[*file].name));
		fsh[*file].zipFile = qfalse;

		if(fs_debug->current.integer)
		{
			Sys_Print(va("FS_FOpenFileRead: %s (found in '%s/%s')\n", filename, dir->path, dir->gamedir));
		}

		fsh[*file].handleFiles.file.o = filep;

		return FS_fplength(filep);
	}

	return -1;
}

long FS_FOpenFileRead_Internal(const char *filename, fileHandle_t *file, int fsThread)
{
	searchpath_t *search;
	long len;

	if (!FS_Initialized())
	{
		Com_Error(ERR_FATAL, "Filesystem call made without initialization");
	}

	for(search = fs_searchpaths; search; search = search->next)
	{
		len = FS_FOpenFileReadDir(filename, search, file, 0, qfalse, fsThread);

		if(file == NULL)
		{
			if(len > 0)
			{
				return len;
			}
		}
		else
		{
			if(len >= 0 && *file)
			{
				return len;
			}
		}
	}

	if(file)
		*file = 0;

	return -1;
}

long FS_FOpenFileRead(const char *filename, fileHandle_t *file)
{
	return FS_FOpenFileRead_Internal(filename, file, 0);
}

long FS_SV_FOpenFileRead( const char *filename, fileHandle_t *fp )
{
	char ospath[MAX_OSPATH];
	fileHandle_t	f = 0;
	*fp = 0;

	if ( !FS_Initialized() )
	{
		Com_Error( ERR_FATAL, "Filesystem call made without initialization" );
	}

	if(filename == NULL || filename[0] == 0)
	{
		return 0;
	}

	f = FS_HandleForFile();
	if(f == 0)
	{
		return 0;
	}
	FS_SetFilenameForHandle(f, filename);
	fsh[f].zipFile = qfalse;

	I_strncpyz( fsh[f].name, filename, sizeof( fsh[f].name ) );

	// search homepath
	FS_BuildOSPath( fs_homepath->current.string, filename, "", ospath );
	// remove trailing slash
	ospath[strlen(ospath)-1] = '\0';

	if ( fs_debug->current.integer )
	{
		Sys_Print(va("FS_SV_FOpenFileRead (fs_homepath): %s\n", ospath ));
	}

	fsh[f].handleFiles.file.o = fopen( ospath, "rb" );
	fsh[f].handleSync = qfalse;

	if (!fsh[f].handleFiles.file.o)
	{
		// NOTE TTimo on non *nix systems, fs_homepath == fs_basepath, might want to avoid
		if (I_stricmp(fs_homepath->current.string,fs_basepath->current.string))
		{
			// search basepath
			FS_BuildOSPath( fs_basepath->current.string, filename, "", ospath );

			ospath[strlen(ospath)-1] = '\0';
			if ( fs_debug->current.integer )
			{
				Sys_Print(va("FS_SV_FOpenFileRead (fs_basepath): %s\n", ospath ));
			}

			fsh[f].handleFiles.file.o = fopen( ospath, "rb" );
			fsh[f].handleSync = qfalse;

			if ( !fsh[f].handleFiles.file.o )
			{
				f = 0;
			}
		}
	}

	if ( !fsh[f].handleFiles.file.o )
	{
		// search cd path
		FS_BuildOSPath( fs_cdpath->current.string, filename, "", ospath );
		ospath[strlen( ospath ) - 1] = '\0';

		if ( fs_debug->current.integer )
		{
			Com_Printf( "FS_SV_FOpenFileRead (fs_cdpath) : %s\n", ospath );
		}

		fsh[f].handleFiles.file.o = fopen( ospath, "rb" );
		fsh[f].handleSync = qfalse;

		if ( !fsh[f].handleFiles.file.o )
		{
			f = 0;
		}
	}

	*fp = f;

	if (f)
	{
		return FS_filelength(f);
	}

	return 0;
}

int FS_ReadFile( const char *qpath, void **buffer )
{
	fileHandle_t	h;
	byte*			buf;
	int			len;

	if ( !qpath || !qpath[0] )
	{
		Com_Error( ERR_FATAL, "FS_ReadFile with empty name\n" );
	}

	buf = NULL;	// quiet compiler warning

	// look for it in the filesystem or pack files
	len = FS_FOpenFileRead( qpath, &h );
	if ( h == 0 )
	{
		if ( buffer )
		{
			*buffer = NULL;
		}
		return -1;
	}

	if ( !buffer )
	{
		FS_FCloseFile( h);
		return len;
	}

	FS_LoadStackInc();

	buf = (byte *)malloc(len+1);
	*buffer = buf;

	FS_Read (buf, len, h);

	// guarantee that it will have a trailing 0 for string operations
	buf[len] = 0;
	FS_FCloseFile( h );
	return len;
}

int FS_SV_ReadFile( const char *qpath, void **buffer )
{
	fileHandle_t	h;
	byte*			buf;
	int			len;

	if ( !qpath || !qpath[0] )
	{
		Com_Error( ERR_FATAL, "FS_ReadFile with empty name\n" );
	}

	buf = NULL;	// quiet compiler warning

	// look for it in the filesystem or pack files
	len = FS_SV_FOpenFileRead( qpath, &h );
	if ( h == 0 )
	{
		if ( buffer )
		{
			*buffer = NULL;
		}
		return -1;
	}

	if ( !buffer )
	{
		FS_FCloseFile( h);
		return len;
	}

	FS_LoadStackInc();

	buf = (byte *)malloc(len+1);
	*buffer = buf;

	FS_Read (buf, len, h);

	// guarantee that it will have a trailing 0 for string operations
	buf[len] = 0;
	FS_FCloseFile( h );
	return len;
}

void FS_Startup(const char *gameName)
{
	Com_Printf("----- FS_Startup -----\n");

	fs_packFiles = 0;

	FS_RegisterDvars();

	if (fs_useOldAssets->current.boolean)
	{
		if (fs_basepath->current.string[0])
		{
			FS_AddGameDirectory(fs_basepath->current.string, "tempcod");
		}

		if (fs_homepath->current.string[0])
		{
			FS_AddGameDirectory(fs_homepath->current.string, "tempcod");
		}
	}

	if (fs_basepath->current.string[0])
	{
		FS_AddGameDirectory(fs_basepath->current.string, "devraw_shared");
		FS_AddGameDirectory(fs_basepath->current.string, "devraw");
		FS_AddGameDirectory(fs_basepath->current.string, "raw_shared");
		FS_AddGameDirectory(fs_basepath->current.string, "raw");
	}

	if (fs_homepath->current.string[0])
	{
		FS_AddGameDirectory(fs_homepath->current.string, "devraw_shared");
		FS_AddGameDirectory(fs_homepath->current.string, "devraw");
		FS_AddGameDirectory(fs_homepath->current.string, "raw_shared");
		FS_AddGameDirectory(fs_homepath->current.string, "raw");
	}

	if (fs_cdpath->current.string[0])
	{
		FS_AddGameDirectory(fs_cdpath->current.string, "devraw_shared");
		FS_AddGameDirectory(fs_cdpath->current.string, "devraw");
		FS_AddGameDirectory(fs_cdpath->current.string, "raw_shared");
		FS_AddGameDirectory(fs_cdpath->current.string, "raw");
		FS_AddGameDirectory(fs_cdpath->current.string, gameName);
	}

	if (fs_basepath->current.string[0])
		FS_AddGameDirectory(fs_basepath->current.string, gameName);

	if (fs_basepath->current.string[0] && I_stricmp(fs_homepath->current.string, fs_basepath->current.string))
		FS_AddGameDirectory(fs_homepath->current.string, gameName);

	if (fs_basegame->current.string[0] && !I_stricmp(gameName, BASEGAME) && I_stricmp(fs_basegame->current.string, gameName))
	{
		if (fs_cdpath->current.string[0])
			FS_AddGameDirectory(fs_cdpath->current.string, fs_basegame->current.string);
		if (fs_basepath->current.string[0])
			FS_AddGameDirectory(fs_basepath->current.string, fs_basegame->current.string);
		if (fs_homepath->current.string[0] && I_stricmp(fs_homepath->current.string, fs_basepath->current.string))
			FS_AddGameDirectory(fs_homepath->current.string, fs_basegame->current.string);
	}

	if (fs_gameDirVar->current.string[0] && !I_stricmp(gameName, BASEGAME) && I_stricmp(fs_gameDirVar->current.string, gameName))
	{
		if (fs_cdpath->current.string[0])
			FS_AddGameDirectory(fs_cdpath->current.string, fs_gameDirVar->current.string);
		if (fs_basepath->current.string[0])
			FS_AddGameDirectory(fs_basepath->current.string, fs_gameDirVar->current.string);
		if (fs_homepath->current.string[0] && I_stricmp(fs_homepath->current.string, fs_basepath->current.string))
			FS_AddGameDirectory(fs_homepath->current.string, fs_gameDirVar->current.string);
	}

	FS_AddCommands();
	FS_DisplayPath();
	Dvar_ClearModified(fs_gameDirVar);

	Com_Printf("----------------------\n");
	Com_Printf("%d files in iwd files\n", fs_packFiles);
}

void SEH_InitLanguage()
{
	loc_language = Dvar_RegisterInt("loc_language", 0, 0, 14, 0x1021u);
	loc_forceEnglish = Dvar_RegisterBool("loc_forceEnglish", 0, 0x1021u);
	loc_translate = Dvar_RegisterBool("loc_translate", 1, 0x1020u);
	loc_warnings = Dvar_RegisterBool("loc_warnings", 0, 0x1000u);
	loc_warningsAsErrors = Dvar_RegisterBool("loc_warningsAsErrors", 0, 0x1000u);
	g_currentAsian = (loc_language->current.integer - 8) <= 4;
}

void FS_InitFilesystem()
{
	// allow command line parms to override our defaults
	// we have to specially handle this, because normal command
	// line variable sets don't happen until after the filesystem
	// has already been initialized
	Com_StartupVariable("fs_cdpath");
	Com_StartupVariable("fs_basepath");
	Com_StartupVariable("fs_homepath");
	Com_StartupVariable("fs_game");
	Com_StartupVariable("fs_copyfiles");
	Com_StartupVariable("fs_restrict");
	Com_StartupVariable("loc_language");

	SEH_InitLanguage();

	FS_Startup(BASEGAME);

	// if we can't find default.cfg, assume that the paths are
	// busted and error out now, rather than getting an unreadable
	// graphics screen when the font fails to load
	if ( FS_ReadFile( DEFAULT_CONFIG, NULL ) <= 0 )
	{
		// TTimo - added some verbosity, 'couldn't load default.cfg' confuses the hell out of users
		Com_Error( ERR_FATAL, "Couldn't load %s.  Make sure Call of Duty is run from the correct folder.", DEFAULT_CONFIG );
	}
	I_strncpyz(lastValidBase, fs_basepath->current.string, sizeof(lastValidBase));
	I_strncpyz(lastValidGame, fs_gameDirVar->current.string, sizeof(lastValidGame));
}