#include "../qcommon/qcommon.h"
#include "../qcommon/cmd.h"
#include "com_files.h"
#include "dvar.h"
#include "../stringed/stringed_public.h"

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

#ifdef LIBCOD
dvar_t* fs_library;
#endif

static searchpath_t *fs_searchpaths;
static fileHandleData_t fsh[MAX_FILE_HANDLES];
static char fs_gamedir[MAX_OSPATH];

int fs_checksumFeed;
static int fs_loadStack;

int fs_serverIwds[MAX_IWDFILES];
const char* fs_serverIwdNames[MAX_IWDFILES];
const char* fs_serverReferencedIwdNames[MAX_IWDFILES];
int fs_numServerReferencedIwds = 0;
int fs_numServerIwds = 0;
int fs_packFiles = 0;
int fs_fakeChkSum = 0;

char lastValidBase[MAX_OSPATH];
char lastValidGame[MAX_OSPATH];

qboolean FS_Initialized()
{
	return (fs_searchpaths != NULL);
}

void FS_CheckFileSystemStarted()
{
	assert(fs_searchpaths);
}

size_t FS_FileRead(void *ptr, size_t size, size_t n, FILE *stream)
{
	return fread(ptr, size, n, stream);
}

size_t FS_FileWrite(void *ptr, size_t size, size_t n, FILE *s)
{
	return fwrite(ptr, size, n, s);
}

FILE* FS_FileOpen(const char *filename, const char *modes)
{
	return fopen(filename, modes);
}

int FS_FileClose(FILE *stream)
{
	return fclose(stream);
}

int FS_FileSeek(FILE *stream, int off, int whence)
{
	return fseek(stream, off, whence);
}

long int FS_FileGetFileSize(FILE *stream)
{
	return ftell(stream);
}

FILE* FS_FileOpenWriteBinary(const char *filename)
{
	return FS_FileOpen(filename, "wb");
}

FILE* FS_FileOpenReadBinary(const char *filename)
{
	return FS_FileOpen(filename, "rb");
}

FILE* FS_FileOpenWriteText(const char *filename)
{
	return FS_FileOpen(filename, "wt");
}

qboolean FS_UseSearchPath(const searchpath_t *pSearch)
{
	return !pSearch->localized || !fs_ignoreLocalized->current.boolean;
}

char FS_IsBackupSubStr(const char* filenameSubStr)
{
	if (*filenameSubStr == 46 && filenameSubStr[1] == 46)
	{
		return 1;
	}
	if (*filenameSubStr != 58 || filenameSubStr[1] != 58)
	{
		return 0;
	}
	return 1;
}

bool FS_SanitizeFilename(const char *filename, char *sanitizedName)
{
	int srcIndex;
	int dstIndex;

	for (srcIndex = 0; ; ++srcIndex)
	{
		if (!(filename[srcIndex] == '/' || filename[srcIndex] == '\\'))
		{
			break;
		}
	}

	dstIndex = 0;
	while (filename[srcIndex])
	{
		if (FS_IsBackupSubStr(&filename[srcIndex]))
		{
			return 0;
		}

		if (filename[srcIndex] != '.' || (filename[srcIndex + 1] != 0 &&
		                                  filename[srcIndex + 1] != '/' && filename[srcIndex + 1] != '\\'))
		{
			if (filename[srcIndex] == '/' || filename[srcIndex] == '\\')
			{
				sanitizedName[dstIndex] = '/';
				while (1)
				{
					if (!(filename[srcIndex + 1] == '/' || filename[srcIndex + 1] == '\\'))
					{
						break;
					}
					++srcIndex;
				}
			}
			else
			{
				sanitizedName[dstIndex] = filename[srcIndex];
			}
			++dstIndex;
		}
		++srcIndex;
	}

	sanitizedName[dstIndex] = 0;
	return 1;
}

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

static fileHandle_t FS_HandleForFile(FsThread thread)
{
	int i;
	int count;
	int first;

	switch (thread)
	{
	case FS_THREAD_MAIN:
		first = 1;
		count = 49;
		break;
	case FS_THREAD_STREAM:
		first = 50;
		count = 11;
		break;
	default:
		break;
	}

	for (i = 0; i < count; ++i)
	{
		if (fsh[i + first].handleFiles.file.o == NULL)
		{
			return i + first;
		}
	}

	for (i = 1; i < MAX_FILE_HANDLES; ++i)
	{
		Com_Printf("FILE %2i: '%s' 0x%x\n", i, fsh[i].name, fsh[i].handleFiles.file.o);
	}

	Com_Error(ERR_DROP, "FS_HandleForFile: none free");
	return 0;
}

static FILE *FS_FileForHandle( fileHandle_t f )
{
	return fsh[f].handleFiles.file.o;
}

int FS_filelength( fileHandle_t f )
{
	int pos;
	int end;
	FILE*   h;

	if (fsh[f].zipFile)
	{
		return ((unz_s*)fsh[f].handleFiles.file.z)->cur_file_info.uncompressed_size;
	}

	h = FS_FileForHandle( f );

	pos = ftell( h );
	FS_FileSeek( h, 0, SEEK_END );
	end = ftell( h );
	FS_FileSeek( h, pos, SEEK_SET );

	return end;
}

int FS_FilenameCompare(const char *s1, const char *s2)
{
	int c1, c2;

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if (I_islower(c1))
		{
			c1 -= ('a' - 'A');
		}
		if (I_islower(c2))
		{
			c2 -= ('a' - 'A');
		}

		if (c1 == '\\' || c1 == ':')
		{
			c1 = '/';
		}
		if (c2 == '\\' || c2 == ':')
		{
			c2 = '/';
		}

		if (c1 != c2)
		{
			return -1;      // strings not equal
		}
	}
	while (c1);

	return 0;       // strings are equal
}

int FS_PathCmp(const char* s1, const char* s2)
{
	int c2;
	int c1;

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if (I_islower(c1))
		{
			c1 -= ('a' - 'A');
		}
		if (I_islower(c2))
		{
			c2 -= ('a' - 'A');
		}
		if (c1 == '\\' || c1 == ':')
		{
			c1 = '/';
		}
		if (c2 == '\\' || c2 == ':')
		{
			c2 = '/';
		}

		if (c1 < c2)
		{
			return -1;      // strings not equal
		}
		if (c1 > c2)
		{
			return 1;
		}
	}
	while (c1);

	return 0;       // strings are equal
}

int FS_LanguageHasAssets(int iLanguage)
{
	searchpath_t* pSearch;

	for (pSearch = fs_searchpaths; pSearch; pSearch = pSearch->next)
	{
		if (pSearch->localized && pSearch->language == iLanguage)
		{
			return 1;
		}
	}
	return 0;
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

void FS_ReplaceSeparators( char *path )
{
	char    *s;

	for ( s = path ; *s ; s++ )
	{
		if ( *s == '/' || *s == '\\' )
		{
			*s = PATH_SEP;
		}
	}
}

void FS_AddSearchPath(searchpath_t* search)
{
	searchpath_t** pSearch;

	pSearch = &fs_searchpaths;
	if (search->localized)
	{
		while (*pSearch && !(*pSearch)->localized)
		{
			pSearch = (searchpath_t**)*pSearch;
		}
	}
	search->next = *pSearch;
	*pSearch = search;
}

void FS_FCloseFile( fileHandle_t f )
{
	FS_CheckFileSystemStarted();

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
		FS_FileClose( fsh[f].handleFiles.file.o );
	}

	Com_Memset( &fsh[f], 0, sizeof( fsh[f] ) );
}

static iwd_t *FS_LoadZipFile( char *zipfile, const char *basename )
{
	fileInIwd_t    *buildBuffer;
	iwd_t          *iwd;
	unzFile uf;
	int err;
	unz_global_info gi;
	char filename_inzip[MAX_ZPATH];
	unz_file_info file_info;
	int i, len;
	long hash;
	int fs_numHeaderLongs;
	intptr_t        *fs_headerLongs;
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

	buildBuffer = (fileInIwd_t *)Z_Malloc( ( gi.number_entry * sizeof( fileInIwd_t ) ) + len );
	namePtr = ( (char *) buildBuffer ) + gi.number_entry * sizeof( fileInIwd_t );
	fs_headerLongs = (intptr_t *)Z_Malloc( gi.number_entry * sizeof( intptr_t ) );

	// get the hash table size from the number of files in the zip
	// because lots of custom iwd files have less than 32 or 64 files
	for ( i = 1; i <= MAX_FILEHASH_SIZE; i <<= 1 )
	{
		if ( i > gi.number_entry )
		{
			break;
		}
	}

	iwd = (iwd_t *)Z_Malloc( sizeof( iwd_t ) + i * sizeof( fileInIwd_t * ) );
	iwd->hashSize = i;
	iwd->hashTable = ( fileInIwd_t ** )( ( (char *) iwd ) + sizeof( iwd_t ) );
	for ( i = 0; i < iwd->hashSize; i++ )
	{
		iwd->hashTable[i] = NULL;
	}

	Q_strncpyz( iwd->iwdFilename, zipfile, sizeof( iwd->iwdFilename ) );
	Q_strncpyz( iwd->iwdBasename, basename, sizeof( iwd->iwdBasename ) );

	// strip .iwd if needed
	if ( strlen( iwd->iwdBasename ) > 4 && !Q_stricmp( iwd->iwdBasename + strlen( iwd->iwdBasename ) - 4, ".iwd" ) )
	{
		iwd->iwdBasename[strlen( iwd->iwdBasename ) - 4] = 0;
	}

	iwd->handle = uf;
	iwd->numFiles = gi.number_entry;
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
		Q_strlwr( filename_inzip );
		hash = FS_HashFileName( filename_inzip, iwd->hashSize );
		buildBuffer[i].name = namePtr;
		strcpy( buildBuffer[i].name, filename_inzip );
		namePtr += strlen( filename_inzip ) + 1;
		// store the file position in the zip
		unzGetCurrentFileInfoPosition( uf, &buildBuffer[i].pos );
		//
		buildBuffer[i].next = iwd->hashTable[hash];
		iwd->hashTable[hash] = &buildBuffer[i];
		unzGoToNextFile( uf );
	}

	iwd->checksum = Com_BlockChecksum( fs_headerLongs, 4 * fs_numHeaderLongs );
	iwd->pure_checksum = Com_BlockChecksumKey( fs_headerLongs, 4 * fs_numHeaderLongs, LittleLong( fs_checksumFeed ) );
	iwd->checksum = LittleLong( iwd->checksum );
	iwd->pure_checksum = LittleLong( iwd->pure_checksum );

	Z_Free( fs_headerLongs );

	iwd->buildBuffer = buildBuffer;
	return iwd;
}

int FS_CreatePath (char *OSPath)
{
	char	*ofs;

	// make absolutely sure that it can't back up the path
	// FIXME: is c: allowed???
	if ( strstr( OSPath, ".." ) || strstr( OSPath, "::" ) )
	{
		Com_Printf( "WARNING: refusing to create relative path \"%s\"\n", OSPath );
		return qtrue;
	}

	for (ofs = OSPath+1 ; *ofs ; ofs++)
	{
		if (*ofs == PATH_SEP)
		{
			// create the directory
			*ofs = 0;
			Sys_Mkdir (OSPath);
			*ofs = PATH_SEP;
		}
	}

	return qfalse;
}

void FS_CopyFile(char *fromOSPath, char *toOSPath)
{
	char* buf;
	size_t len;
	FILE* f;

	f = FS_FileOpenReadBinary(fromOSPath);

	if (!f)
	{
		return;
	}

	len = FS_FileGetFileSize(f);
	buf = (char *)Z_Malloc(len);

	if (FS_FileRead(buf, 1u, len, f) != len)
	{
		Com_Error(ERR_FATAL, "Short read in FS_CopyFile()\n");
	}

	FS_FileClose(f);

	if (FS_CreatePath(toOSPath))
	{
		Z_Free(buf);
		return;
	}

	f = FS_FileOpenWriteBinary(toOSPath);

	if (!f)
	{
		Z_Free(buf);
		return;
	}

	if (FS_FileWrite(buf, 1u, len, f) != len)
	{
		Com_Error(ERR_FATAL, "Short write in FS_CopyFile()\n");
	}

	FS_FileClose(f);
	Z_Free(buf);
}

qboolean FS_PureServerSetLoadedIwds(const char *paksums, const char *paknames)
{
	int i, k, l, rt;
	int numPakSums;
	int numPakNames;
	char *lpakNames[MAX_IWDFILES];
	int lpakSums[MAX_IWDFILES];

	rt = 0;

	Cmd_TokenizeString(paksums);

	numPakSums = Cmd_Argc();

	if ( numPakSums > sizeof(lpakSums)/sizeof(lpakSums[0]))
	{
		numPakSums = sizeof(lpakSums)/sizeof(lpakSums[0]);
	}

	for ( i = 0 ; i < numPakSums ; i++ )
	{
		lpakSums[i] = atoi( Cmd_Argv( i ) );
	}

	Cmd_TokenizeString(paknames);
	numPakNames = Cmd_Argc();

	if ( numPakNames > sizeof(lpakNames)/sizeof(lpakNames[0]) )
	{
		numPakNames = sizeof(lpakNames)/sizeof(lpakNames[0]);
	}

	for ( i = 0 ; i < numPakNames ; i++ )
	{
		lpakNames[i] = CopyString( Cmd_Argv( i ) );
	}

	if ( numPakSums != numPakNames )
	{
		Com_Error(ERR_FATAL, "iwd sum/name mismatch");
		return rt;
	}

	if ( numPakSums == fs_numServerIwds )
	{
		for ( i = 0, k = 0; i < fs_numServerIwds; )
		{
			if ( lpakSums[k] == fs_serverIwds[i] && !Q_stricmp(lpakNames[k], fs_serverIwdNames[i]) )
			{
				++k;
				if ( k < numPakSums )
				{
					i = 0;
					continue;
				}

				for ( l = 0; l < numPakNames; ++l )
				{
					FreeString(lpakNames[l]);
					lpakNames[l] = NULL;
				}
				return 0;

			}
			++i;
		}

		if ( numPakSums == 0 )
		{
			return rt;
		}
	}

	FS_ShutdownServerIwdNames();
	fs_numServerIwds = numPakSums;

	if ( numPakSums )
	{
		Com_DPrintf("Connected to a pure server.\n");
		Com_Memcpy(fs_serverIwds, lpakSums, sizeof(char*) * fs_numServerIwds);
		Com_Memcpy(fs_serverIwdNames, lpakNames, sizeof(char*) * fs_numServerIwds);
		fs_fakeChkSum = 0;
	}

	return rt;
}

void FS_ShutdownServerFileReferences(int *numFiles, const char **fileNames)
{
	int i;

	for (i = 0; i < *numFiles; ++i)
	{
		if (fileNames[i])
		{
			FreeString((char *)fileNames[i]);
			fileNames[i] = NULL;
		}
	}

	*numFiles = 0;
}

void FS_ShutdownServerIwdNames()
{
	FS_ShutdownServerFileReferences(&fs_numServerIwds, fs_serverIwdNames);
}

void FS_ShutdownServerReferencedIwds()
{
	FS_ShutdownServerFileReferences(&fs_numServerReferencedIwds, fs_serverReferencedIwdNames);
}

void FS_AddIwdFilesForGameDirectory(const char *path, const char *dir)
{
	searchpath_t *search;
	int langindex;
	int numfiles;
	const char* language;
	qboolean islocalized;
	int i, j;
	iwd_t* iwd;
	char** iwdfiles;
	char iwdfile[MAX_OSPATH];
	char* sorted[MAX_IWDFILES];
	qboolean languagesListed;

	FS_BuildOSPath(path, dir, "", iwdfile);
	iwdfile[strlen(iwdfile) - 1] = 0;
	iwdfiles = Sys_ListFiles(iwdfile, "iwd", 0, &numfiles, 0);

	if ( numfiles > MAX_IWDFILES )
	{
		Com_Printf("WARNING: Exceeded max number of iwd files in %s %s (%d %d)\n", path, dir, numfiles, MAX_IWDFILES);
		numfiles = MAX_IWDFILES;
	}

	for(i = 0; i < numfiles; i++)
	{
		sorted[i] = iwdfiles[i];

		if ( !I_strncmp(sorted[i], "localized_", 10) )
		{
			Com_Memcpy(sorted[i],  "          ", 10);
		}
	}

	qsort(sorted, numfiles, sizeof(intptr_t), iwdsort);

	languagesListed = 0;

	for(i = 0; i < numfiles; i++)
	{
		islocalized = qfalse;
		langindex = 0;
		if(!I_strncmp(sorted[i], "          ", 10))
		{
			if (fs_ignoreLocalized->current.boolean)
				continue;

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
					for(j = 0; j < MAX_LANGUAGES; j++)
					{
						Com_Printf("    %s\n", SEH_GetLanguageName(j));
					}
					languagesListed = 1;
				}
				continue;
			}
			islocalized = qtrue;
			SEH_UpdateCurrentLanguage(langindex);
		}

		FS_BuildOSPath(path, dir, sorted[i], iwdfile);
		iwd = FS_LoadZipFile( iwdfile, sorted[i]);

		if (!iwd)
		{
			continue;
		}

		I_strncpyz(iwd->iwdGamename, dir, sizeof(iwd->iwdGamename));

		search = (searchpath_t *)S_Malloc(sizeof(searchpath_t));
		search->iwd = iwd;
		search->localized = islocalized;
		search->language = langindex;
		search->next = fs_searchpaths;
		fs_searchpaths = search;
	}

	Sys_FreeFileList(iwdfiles);
}

void FS_BuildOSPath_Internal(const char *base, const char *game, const char *qpath, char *ospath, FsThread thread)
{
	unsigned int lenQpath;
	unsigned int lenGame;
	unsigned int lenBase;

	if (game)
	{
		if (!*game)
		{
			game = fs_gamedir;
		}
	}
	else
	{
		game = "";
	}

	lenBase = strlen(base);
	lenGame = strlen(game);
	lenQpath = strlen(qpath);

	if ((int)(lenBase + lenGame + 1 + lenQpath + 1) >= MAX_OSPATH)
	{
		if (thread)
		{
			*ospath = 0;
			return;
		}

		Com_Error(ERR_FATAL, "FS_BuildOSPath: os path length exceeded\n");
	}

	memcpy(ospath, base, lenBase);
	ospath[lenBase] = '/';

	memcpy(&ospath[lenBase + 1], game, lenGame);
	ospath[lenBase + 1 + lenGame] = '/';

	memcpy(&ospath[lenBase + 2 + lenGame], qpath, lenQpath + 1);
	FS_ReplaceSeparators(ospath);
}

void FS_BuildOSPath(const char *base, const char *game, const char *qpath, char* ospath)
{
	FS_BuildOSPath_Internal(base, game, qpath, ospath, FS_THREAD_MAIN);
}

void FS_AddGameDirectoryInternal(const char *path, const char *dir, int bLanguageDirectory, int iLanguage)
{
	char ospath[MAX_OSPATH];
	const char* pszLanguage;
	char szGameFolder[MAX_QPATH];
	searchpath_t* search;
	searchpath_t* i;

	if (bLanguageDirectory)
	{
		pszLanguage = SEH_GetLanguageName(iLanguage);
		Com_sprintf(szGameFolder, sizeof(szGameFolder), "%s/%s", dir, pszLanguage);
	}
	else
	{
		I_strncpyz(szGameFolder, dir, sizeof(szGameFolder));
	}

	for (i = fs_searchpaths; i; i = i->next)
	{
		if (i->dir && !I_stricmp(i->dir->path, path) && !I_stricmp(i->dir->gamedir, szGameFolder))
		{
			if (i->localized != bLanguageDirectory)
			{
				Com_Printf(
				    "WARNING: game folder %s/%s added as both localized & non-localized. Using folder as %s\n",
				    path, szGameFolder, i->localized ? "localized" : "non-localized");
			}

			if (i->localized)
			{
				if (i->language != iLanguage)
				{
					Com_Printf(
					    "WARNING: game folder %s/%s re-added as localized folder with different language\n",
					    path, szGameFolder);
				}
			}
			return;
		}
	}

	if (bLanguageDirectory)
	{
		// find all iwd files in this directory
		FS_BuildOSPath(path, szGameFolder, "", ospath);
		ospath[strlen(ospath) - 1] = 0; // strip the trailing slash
		if (!Sys_DirectoryHasContents(ospath))
		{
			return;
		}
	}
	else
	{
		I_strncpyz(fs_gamedir, szGameFolder, sizeof(fs_gamedir));
	}

	//
	// add the directory to the search path
	//
	search = (searchpath_t*)Z_Malloc(sizeof(searchpath_t));
	search->dir = (directory_t*)Z_Malloc(sizeof(*search->dir));

	I_strncpyz(search->dir->path, path, sizeof(search->dir->path));
	I_strncpyz(search->dir->gamedir, szGameFolder, sizeof(search->dir->gamedir));

	search->localized = bLanguageDirectory;
	search->language = iLanguage;

	FS_AddSearchPath(search);
	FS_AddIwdFilesForGameDirectory(path, szGameFolder);
}

void FS_AddGameDirectory(const char *path, const char *dir)
{
	FS_AddGameDirectoryInternal(path, dir, qtrue, 0);
	FS_AddGameDirectoryInternal(path, dir, qfalse, 0);
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
		if ( !Q_stricmp(&filename[filenameLen - strlen(extensions[extensionNum])], extensions[extensionNum]) )
		{
			return qtrue;
		}
	}

	return qfalse;
}

int FS_IwdIsPure(const iwd_t *iwd)
{
	int i;

	if (!fs_numServerIwds)
	{
		return 1;
	}
	for (i = 0; i < fs_numServerIwds; ++i)
	{
		if (iwd->checksum == fs_serverIwds[i])
		{
			return 1;
		}
	}
	return 0;
}

qboolean FS_PureIgnoreFiles(const char *extension)
{
	if (*extension == '.')
		++extension;

	if (!I_stricmp(extension, "cfg"))
		return 1;

	if (I_stricmp(extension, "menu"))
		return I_stricmp(extension, ".dm_NETWORK_PROTOCOL_VERSION") == 0;

	return qtrue;
}

int FS_FOpenFileRead_Internal(const char *filename, int *file, qboolean uniqueFILE, FsThread thread)
{
	char copypath[MAX_OSPATH];
	fileInIwd_t* iwdFile;
	int hash;
	iwd_t* iwd;
	char sanitizedName[MAX_OSPATH];
	const char *extension;
	directory_t* dir;
	unz_s* zfi;
	file_in_zip_read_info_s* ziptemp;
	char netpath[MAX_OSPATH];
	bool wasSkipped = 0;
	searchpath_t* search;
	FILE* filetemp;
	iwd_t* impureIwd = NULL;

	FS_CheckFileSystemStarted();

	if (!FS_SanitizeFilename(filename, sanitizedName))
	{
		if (file)
		{
			*file = 0;
		}

		return -1;
	}

	if (file == NULL)
	{
		// just wants to see if file is there
		for (search = fs_searchpaths; search; search = search->next)
		{
			if (!FS_UseSearchPath(search))
			{
				continue;
			}

			// look through all the iwd file elements
			iwd = search->iwd;
			if (iwd && iwd->numFiles)
			{
				hash = FS_HashFileName(sanitizedName, iwd->hashSize);
				for (iwdFile = iwd->hashTable[hash]; iwdFile; iwdFile = iwdFile->next)
				{
					// case and separator insensitive comparisons
					if (!FS_FilenameCompare(iwdFile->name, sanitizedName))
					{
						// found it!
						return 1;
					}
				}
			}
			else if (search->dir)
			{
				dir = search->dir;

				FS_BuildOSPath_Internal(dir->path, dir->gamedir, sanitizedName, netpath, thread);
				filetemp = FS_FileOpenReadBinary(netpath);
				if (!filetemp)
				{
					continue;
				}
				FS_FileClose(filetemp);
				return 1;
			}
		}
		return -1;
	}

	//
	// search through the path, one element at a time
	//

	*file = FS_HandleForFile(thread);
	fsh[*file].handleFiles.unique = uniqueFILE;

	for (search = fs_searchpaths; search; search = search->next)
	{
		if (!FS_UseSearchPath(search))
		{
			continue;
		}

		// look through all the iwd file elements
		iwd = search->iwd;
		if (iwd && iwd->numFiles)
		{
			hash = FS_HashFileName(sanitizedName, iwd->hashSize);
			for (iwdFile = iwd->hashTable[hash]; iwdFile; iwdFile = iwdFile->next)
			{
				// case and separator insensitive comparisons
				if (!FS_FilenameCompare(iwdFile->name, sanitizedName))
				{
					// found it!
					if (!search->localized && !FS_IwdIsPure(iwd))
					{
						impureIwd = iwd;
						break;
					}

					if (!iwd->referenced && !FS_FilesAreLoadedGlobally(sanitizedName))
					{
						iwd->referenced = 1;
					}
					if ( uniqueFILE )
					{
						// open a new file on the pakfile
						fsh[*file].handleFiles.file.z = unzReOpen( iwd->iwdFilename, iwd->handle );
						if ( fsh[*file].handleFiles.file.z == NULL )
						{
							Com_Error( ERR_FATAL, "Couldn't reopen %s", iwd->iwdFilename );
						}
					}
					else
					{
						fsh[*file].handleFiles.file.z = iwd->handle;
					}

					I_strncpyz(fsh[*file].name, sanitizedName, sizeof(fsh[*file].name));
					fsh[*file].zipFile = qtrue;
					zfi = (unz_s*)fsh[*file].handleFiles.file.z;
					// in case the file was new
					filetemp = (FILE *)zfi->filestream;
					ziptemp = zfi->pfile_in_zip_read;
					// set the file position in the zip file (also sets the current file info)
					unzSetCurrentFileInfoPosition(iwd->handle, iwdFile->pos);
					// copy the file info into the unzip structure
					Com_Memcpy(zfi, iwd->handle, sizeof(unz_s));
					// we copy this back into the structure
					zfi->filestream = filetemp;
					zfi->pfile_in_zip_read = ziptemp;
					// open the file in the zip
					unzOpenCurrentFile(fsh[*file].handleFiles.file.z);
					fsh[*file].zipFilePos = iwdFile->pos;

					if (fs_debug->current.integer)
					{
						Com_Printf(
						    "FS_FOpenFileRead: %s (found in '%s')\n",
						    sanitizedName,
						    iwd->iwdFilename);
					}

					return zfi->cur_file_info.uncompressed_size;
				}
			}
		}
		else if (search->dir)
		{
			extension = Com_GetExtensionSubString(sanitizedName);
			// check a file in the directory tree
			if (!fs_restrict->current.boolean && (!fs_numServerIwds || search->localized || FS_PureIgnoreFiles(extension)))
			{
				dir = search->dir;

				FS_BuildOSPath_Internal(dir->path, dir->gamedir, sanitizedName, netpath, thread);
				fsh[*file].handleFiles.file.o = FS_FileOpenReadBinary(netpath);
				if (!fsh[*file].handleFiles.file.o)
				{
					continue;
				}

				if (!search->localized && !FS_PureIgnoreFiles(extension))
				{
					fs_fakeChkSum = rand() + 1;
				}

				I_strncpyz(fsh[*file].name, sanitizedName, sizeof(fsh[*file].name));
				fsh[*file].zipFile = qfalse;

				// if we are getting it from the cdpath, optionally copy it
				//  to the basepath
				if (fs_copyfiles->current.boolean &&
				        !I_stricmp(dir->path, fs_cdpath->current.string))
				{
					FS_BuildOSPath_Internal(fs_basepath->current.string, dir->gamedir,
					                        sanitizedName, copypath, thread);
					FS_CopyFile(netpath, copypath);
				}

				return FS_filelength(*file);
			}
			else if (!wasSkipped)
			{
				dir = search->dir;

				FS_BuildOSPath_Internal(dir->path, dir->gamedir, sanitizedName, netpath, thread);
				filetemp = FS_FileOpenReadBinary(netpath);
				if (filetemp)
				{
					wasSkipped = 1;
					FS_FileClose(filetemp);
				}
			}
		}
	}

	if (fs_debug->current.integer)
		Com_Printf( "Can't find %s\n", filename );

	FS_FCloseFile(*file);
	*file = 0;

	if (impureIwd)
	{
		Com_Error(ERR_DROP, va("EXE_UNPURECLIENTDETECTED\x15\n%s",
		                       impureIwd->iwdFilename));
	}

	if (wasSkipped)
	{
		if (fs_numServerIwds || fs_restrict->current.boolean)
		{
			Com_Printf("Error: %s must be in an IWD\n", filename);
		}
		else
		{
			Com_Printf("Error: %s must be in an IWD or not in the main directory\n",
			           filename);
		}
		return -2;
	}
	else
	{
		return -1;
	}
}

int FS_FOpenFileRead(const char *filename, fileHandle_t *file, qboolean uniqueFILE)
{
	com_fileAccessed = 1;
	return FS_FOpenFileRead_Internal(filename, file, uniqueFILE, FS_THREAD_MAIN);
}

int FS_FOpenFileReadStream(const char *filename, fileHandle_t *file, qboolean uniqueFILE)
{
	return FS_FOpenFileRead_Internal(filename, file, uniqueFILE, FS_THREAD_STREAM);
}

int FS_Seek(int f, int offset, int origin)
{
	int iZipPos;
	unsigned int iZipOffset;

	FS_CheckFileSystemStarted();

	if (!fsh[f].zipFile)
	{
		return FS_FileSeek(FS_FileForHandle(f), offset, origin);
	}

	if (offset == 0 && origin == 2)
	{
		// set the file position in the zip file (also sets the current file info)
		unzSetCurrentFileInfoPosition(fsh[f].handleFiles.file.z, fsh[f].zipFilePos);
		return unzOpenCurrentFile(fsh[f].handleFiles.file.z);
	}

	if (!offset && !origin)
	{
		return 0;
	}

	iZipPos = unztell(fsh[f].handleFiles.file.z);
	switch (origin)
	{
	case 0:
		if (offset >= 0)
		{
			iZipOffset = offset;
		}
		else
		{
			unzSetCurrentFileInfoPosition(fsh[f].handleFiles.file.z, fsh[f].zipFilePos);
			unzOpenCurrentFile(fsh[f].handleFiles.file.z);
			iZipOffset = offset + iZipPos;
		}

		if (unzReadCurrentFile(fsh[f].handleFiles.file.z, NULL, iZipOffset))
		{
			return 0;
		}
		return -1;
	case 1:
		if (offset + FS_filelength(f) >= iZipPos)
		{
			iZipOffset = offset + FS_filelength(f) - iZipPos;
		}
		else
		{
			unzSetCurrentFileInfoPosition(fsh[f].handleFiles.file.z, fsh[f].zipFilePos);
			unzOpenCurrentFile(fsh[f].handleFiles.file.z);
			iZipOffset = offset + FS_filelength(f);
		}
		if (unzReadCurrentFile(fsh[f].handleFiles.file.z, NULL, iZipOffset))
		{
			return 0;
		}
		return -1;
	case 2:
		if (offset >= iZipPos)
		{
			iZipOffset = offset - iZipPos;
		}
		else
		{
			unzSetCurrentFileInfoPosition(fsh[f].handleFiles.file.z, fsh[f].zipFilePos);
			unzOpenCurrentFile(fsh[f].handleFiles.file.z);
			iZipOffset = offset;
		}
		if (unzReadCurrentFile(fsh[f].handleFiles.file.z, NULL, iZipOffset))
		{
			return 0;
		}
		return -1;
	}

	return -1;
}

int FS_Read(void *buffer, int len, int h)
{
	int tries;
	unsigned int remaining;
	char* buf;
	FILE* f;
	int read;

	FS_CheckFileSystemStarted();

	if (!h)
	{
		return 0;
	}

	if (fsh[h].zipFile)
	{
		return unzReadCurrentFile(fsh[h].handleFiles.file.z, buffer, len);
	}

	f = FS_FileForHandle(h);
	buf = (char*)buffer;
	remaining = len;
	tries = 0;

	while (remaining)
	{
		read = FS_FileRead(buf, 1u, remaining, f);
		if (!read)
		{
			if (tries)
			{
				return len - remaining;
			}
			tries = 1;
		}

		if (read == -1)
		{
			if (h >= 50 && h < 61)
			{
				return -1;
			}
			Com_Error(ERR_FATAL, "FS_Read: -1 bytes read");
		}

		remaining -= read;
		buf += read;
	}

	return len;
}

void FS_Flush(int f)
{
	FILE* file;

	file = FS_FileForHandle(f);
	fflush(file);
}

int FS_Write(const void *buffer, int len, int h)
{
	int tries;
	unsigned int remaining;
	char* buf;
	int written;
	FILE* f;

	FS_CheckFileSystemStarted();

	if (!h)
	{
		return 0;
	}

	f = FS_FileForHandle(h);
	buf = (char*)buffer;
	remaining = len;
	tries = 0;
	while (remaining)
	{
		written = FS_FileWrite(buf, 1u, remaining, f);
		if (!written)
		{
			if (tries)
			{
				return 0;
			}
			tries = 1;
		}

		if (written == -1)
		{
			return 0;
		}

		remaining -= written;
		buf += written;
	}

	if (fsh[h].handleSync)
	{
		fflush(f);
	}
	return len;
}

void FS_Printf( fileHandle_t h, const char *fmt, ... )
{
	va_list argptr;
	char msg[MAXPRINTMSG];

	va_start( argptr,fmt );
	Q_vsnprintf( msg, sizeof( msg ), fmt, argptr );
	va_end( argptr );

	FS_Write( msg, strlen( msg ), h );
}

int FS_GetHandleAndOpenFile(const char *filename, const char *ospath, const char *modes, FsThread thread)
{
	fileHandle_t f;
	FILE* fp;

	fp = FS_FileOpen(ospath, modes);

	if (!fp)
	{
		return 0;
	}

	f = FS_HandleForFile(thread);

	fsh[f].zipFile = qfalse;
	fsh[f].handleFiles.file.o = fp;
	I_strncpyz(fsh[f].name, filename, sizeof(fsh[f].name));
	fsh[f].handleSync = 0;

	return f;
}

int FS_FOpenFileWrite(const char *filename)
{
	char ospath[MAX_OSPATH];

	FS_CheckFileSystemStarted();
	FS_BuildOSPath(fs_homepath->current.string, fs_gamedir, filename, ospath);

	if (fs_debug->current.integer)
	{
		Com_Printf("FS_FOpenFileWrite: %s\n", ospath);
	}

	if (FS_CreatePath(ospath))
	{
		return 0;
	}

	return FS_GetHandleAndOpenFile(filename, ospath, "wb", FS_THREAD_MAIN);
}

int FS_FOpenTextFileWrite(const char* filename)
{
	char ospath[MAX_OSPATH];
	const char* basepath;
	FILE* f;
	int h;

	h = 0;
	FS_CheckFileSystemStarted();

	basepath = fs_homepath->current.string;;
	FS_BuildOSPath(basepath, fs_gamedir, filename, ospath);

	if (fs_debug->current.integer)
	{
		Com_Printf("FS_FOpenTextFileWrite: %s\n", ospath);
	}

	if (FS_CreatePath(ospath))
	{
		return 0;
	}

	f = FS_FileOpenWriteText(ospath);

	if (!f)
	{
		return 0;
	}

	h = FS_HandleForFile(FS_THREAD_MAIN);

	fsh[h].zipFile = qfalse;
	fsh[h].handleFiles.file.o = f;
	I_strncpyz(fsh[h].name, filename, sizeof(fsh[h].name));
	fsh[h].handleSync = qfalse;

	if (!fsh[h].handleFiles.file.o)
	{
		FS_FCloseFile(h);
		h = 0;
	}

	return h;
}

fileHandle_t FS_FOpenFileAppend( const char *filename )
{
	char ospath[MAX_OSPATH];
	fileHandle_t f;

	FS_CheckFileSystemStarted();

	f = FS_HandleForFile(FS_THREAD_MAIN);
	fsh[f].zipFile = qfalse;

	Q_strncpyz( fsh[f].name, filename, sizeof( fsh[f].name ) );
	FS_BuildOSPath( fs_homepath->current.string, fs_gamedir, filename, ospath );

	if ( fs_debug->current.integer )
	{
		Com_Printf( "FS_FOpenFileAppend: %s\n", ospath );
	}

	if ( FS_CreatePath( ospath ) )
	{
		return 0;
	}

	fsh[f].handleFiles.file.o = FS_FileOpen( ospath, "ab" );
	fsh[f].handleSync = qfalse;

	if ( !fsh[f].handleFiles.file.o )
	{
		f = 0;
	}

	return f;
}

int FS_FOpenFileByMode( const char *qpath, fileHandle_t *f, fsMode_t mode )
{
	int r;
	qboolean sync;

	sync = qfalse;

	switch ( mode )
	{
	case FS_READ:
		r = FS_FOpenFileRead( qpath, f, qtrue );
		break;
	case FS_WRITE:
		*f = FS_FOpenFileWrite( qpath );
		r = 0;
		if ( *f == 0 )
		{
			r = -1;
		}
		break;
	case FS_APPEND_SYNC:
		sync = qtrue;
	case FS_APPEND:
		*f = FS_FOpenFileAppend( qpath );
		r = 0;
		if ( *f == 0 )
		{
			r = -1;
		}
		break;
	default:
		Com_Error( ERR_FATAL, "FSH_FOpenFile: bad mode" );
		return -1;
	}

	if ( !f )
	{
		return r;
	}

	if ( *f )
	{
		fsh[*f].fileSize = r;
		fsh[*f].streamed = qfalse;
	}

	fsh[*f].handleSync = sync;

	return r;
}

int FS_LoadStack()
{
	return fs_loadStack;
}

void QDECL FS_FreeFile(void* buffer)
{
	FS_CheckFileSystemStarted();
	--fs_loadStack;
	Hunk_FreeTempMemory(buffer);
}

void FS_ResetFiles()
{
	fs_loadStack = 0;
}

bool FS_Delete(const char *filename)
{
	char ospath[MAX_OSPATH];
	const char* basepath;

	FS_CheckFileSystemStarted();

	if (!*filename)
	{
		return 0;
	}

	basepath = Dvar_GetString("fs_homepath");
	FS_BuildOSPath(basepath, fs_gamedir, filename, ospath);

	return remove(ospath) != -1;
}

int FS_WriteFile(const char* filename, const void* buffer, int size)
{
	int actualSize;
	fileHandle_t f;

	FS_CheckFileSystemStarted();

	f = FS_FOpenFileWrite(filename);

	if (!f)
	{
		Com_Printf("Failed to open %s\n", filename);
		return 0;
	}

	actualSize = FS_Write(buffer, size, f);
	FS_FCloseFile(f);

	if (actualSize != size)
	{
		FS_Delete(filename);
		return 0;
	}

	return 1;
}

int QDECL FS_ReadFile(const char* qpath, void** buffer)
{
	char* buf;
	int len;
	int h;

	FS_CheckFileSystemStarted();

	if (!qpath || !qpath[0])
	{
		Com_Error(ERR_FATAL, "FS_ReadFile with empty name\n");
	}

	// look for it in the filesystem or iwd files
	len = FS_FOpenFileRead(qpath, &h, 0);

	if (h == 0)
	{
		if (buffer)
		{
			*buffer = NULL;
		}
		return -1;
	}

	if (buffer)
	{
		++fs_loadStack;
		buf = (char *)Hunk_AllocateTempMemory(len + 1);
		*buffer = buf;

		FS_Read(buf, len, h);

		// guarantee that it will have a trailing 0 for string operations
		buf[len] = 0;
	}

	FS_FCloseFile(h);
	return len;
}

void FS_ClearIwdReferences()
{
	searchpath_t *search;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		// is the element a iwd file and has it been referenced?
		if ( search->iwd )
		{
			search->iwd->referenced = 0;
		}
	}
}

void FS_DisplayPath(int bLanguageCull)
{
	int iLanguage;
	const char* pszLanguageName;
	searchpath_t* s;
	int i;

	iLanguage = SEH_GetCurrentLanguage();
	pszLanguageName = SEH_GetLanguageName(iLanguage);
	Com_Printf("Current language: %s\n", pszLanguageName);
	if (fs_ignoreLocalized->current.boolean)
	{
		Com_Printf("    localized assets are being ignored\n");
	}

	Com_Printf("Current search path:\n");
	for (s = fs_searchpaths; s; s = s->next)
	{
		if (!bLanguageCull || FS_UseSearchPath(s))
		{
			if (s->iwd)
			{
				Com_Printf("%s (%i files)\n", s->iwd, s->iwd->numFiles);
				if (s->language)
				{
					Com_Printf("    localized assets iwd file for %s\n", SEH_GetLanguageName(s->language));
				}

				if (fs_numServerIwds)
				{
					if (FS_IwdIsPure(s->iwd))
					{
						Com_Printf("    on the pure list\n");
					}
					else
					{
						Com_Printf("    not on the pure list\n");
					}
				}
			}
			else
			{
				Com_Printf("%s/%s\n", s->dir, s->dir->gamedir);
				if (s->language)
				{
					Com_Printf("    localized assets game folder for %s\n", SEH_GetLanguageName(s->language));
				}
			}
		}
	}

	Com_Printf("\nFile Handles:\n");
	for (i = 1; i < MAX_FILE_HANDLES; ++i)
	{
		if (fsh[i].handleFiles.file.o)
		{
			Com_Printf("handle %i: %s\n", i, fsh[i].name);
		}
	}
}

void FS_ConvertPath(char *s)
{
	while (*s)
	{
		if (*s == '\\' || *s == ':')
		{
			*s = '/';
		}
		s++;
	}
}

#define MAX_FOUND_FILES 0x1000
static int FS_AddFileToList( char *name, char *list[MAX_FOUND_FILES], int nfiles )
{
	int i;

	if ( nfiles == MAX_FOUND_FILES - 1 )
	{
		return nfiles;
	}
	for ( i = 0 ; i < nfiles ; i++ )
	{
		if ( !Q_stricmp( name, list[i] ) )
		{
			return nfiles;      // allready in list
		}
	}
	list[nfiles] = CopyString( name );
	nfiles++;

	return nfiles;
}

void FS_SortFileList( char **filelist, int numfiles )
{
	int i, j, k, numsortedfiles;
	char **sortedlist;

	sortedlist = (char **)Z_Malloc( ( numfiles + sizeof(intptr_t) ) * sizeof( *sortedlist ) );
	sortedlist[0] = NULL;
	numsortedfiles = 0;
	for ( i = 0; i < numfiles; i++ )
	{
		for ( j = 0; j < numsortedfiles; j++ )
		{
			if ( FS_PathCmp( filelist[i], sortedlist[j] ) < 0 )
			{
				break;
			}
		}
		for ( k = numsortedfiles; k > j; k-- )
		{
			sortedlist[k] = sortedlist[k - 1];
		}
		sortedlist[j] = filelist[i];
		numsortedfiles++;
	}
	Com_Memcpy( filelist, sortedlist, numfiles * sizeof( *filelist ) );
	Z_Free( sortedlist );
}

void FS_FreeFileList( char **list )
{
	int i;

	FS_CheckFileSystemStarted();

	if ( !list )
	{
		return;
	}

	for ( i = 0 ; list[i] ; i++ )
	{
		Z_Free( list[i] );
	}

	Z_Free( list );
}

static int FS_ReturnPath( const char *zname, char *zpath, int *depth )
{
	int len, at, newdep;

	newdep = 0;
	zpath[0] = 0;
	len = 0;
	at = 0;

	while ( zname[at] != 0 )
	{
		if ( zname[at] == '/' || zname[at] == '\\' )
		{
			len = at;
			newdep++;
		}
		at++;
	}
	strcpy( zpath, zname );
	zpath[len] = 0;
	*depth = newdep;

	return len;
}

char **FS_ListFilteredFiles(searchpath_t *searchPath, const char *path, const char *extension, const char *filter, FsListBehavior behavior, int *numfiles)
{
	int nameLen;
	char netpath[MAX_OSPATH];
	char szTrimmedName[MAX_QPATH];
	int numSysFiles;
	char **sysFiles;
	char *name;
	bool isDirSearch;
	char sanitizedPath[MAX_OSPATH];
	char zpath[MAX_ZPATH];
	int zpathLen;
	fileInIwd_t *buildBuffer;
	iwd_t *iwd;
	int temp;
	int depth;
	int extLen;
	int pathLength;
	int i;
	searchpath_t *pSearch;
	char *list[MAX_FOUND_FILES];
	char **listCopy;
	int nfiles;

	FS_CheckFileSystemStarted();

	if ( !path )
	{
		*numfiles = 0;
		return 0;
	}

	if ( !extension )
		extension = "";

	if ( !FS_SanitizeFilename(path, sanitizedPath) )
	{
		*numfiles = 0;
		return 0;
	}

	isDirSearch = I_stricmp(extension, "/") == 0;
	pathLength = I_strlen(sanitizedPath);

	if ( pathLength && (sanitizedPath[pathLength - 1] == 92 || sanitizedPath[pathLength - 1] == 47) )
		--pathLength;

	extLen = I_strlen(extension);
	nfiles = 0;
	FS_ReturnPath(sanitizedPath, zpath, &depth);

	if ( sanitizedPath[0] )
		++depth;

	for ( pSearch = searchPath; pSearch; pSearch = pSearch->next )
	{
		if ( FS_UseSearchPath(pSearch) )
		{
			if ( pSearch->iwd )
			{
				if ( pSearch->localized || FS_IwdIsPure(pSearch->iwd) )
				{
					iwd = pSearch->iwd;
					buildBuffer = iwd->buildBuffer;

					for ( i = 0; i < iwd->numFiles; ++i )
					{
						name = buildBuffer[i].name;

						if ( filter )
						{
							if ( Com_FilterPath(filter, name, 0) )
								nfiles = FS_AddFileToList(name, list, nfiles);
						}
						else
						{
							zpathLen = FS_ReturnPath(name, zpath, &numSysFiles);
							if ( numSysFiles == depth
							        && pathLength <= zpathLen
							        && (pathLength <= 0 || name[pathLength] == 47)
							        && !I_strnicmp(name, sanitizedPath, pathLength) )
							{
								if ( isDirSearch )
								{
									nameLen = strlen(name);
									if ( name[nameLen - 1] != 47 )
										continue;
								}
								else if ( extLen )
								{
									nameLen = I_strlen(name);
									if ( nameLen <= extLen
									        || name[nameLen - extLen - 1] != 46
									        || I_stricmp(&name[nameLen - extLen], extension) )
									{
										continue;
									}
								}

								temp = pathLength;
								if ( pathLength )
									++temp;
								if ( isDirSearch )
								{
									strcpy(szTrimmedName, name + temp);
									szTrimmedName[strlen(szTrimmedName) - 1] = 0;
									nfiles = FS_AddFileToList(szTrimmedName, list, nfiles);
								}
								else
								{
									nfiles = FS_AddFileToList(&name[temp], list, nfiles);
								}
							}
						}
					}
				}
			}
			else if ( pSearch->dir && (!fs_restrict->current.boolean && !fs_numServerIwds || behavior) )
			{
				FS_BuildOSPath(pSearch->dir->path, pSearch->dir->gamedir, sanitizedPath, netpath);
				sysFiles = Sys_ListFiles(netpath, extension, filter, &numSysFiles, isDirSearch);

				for ( i = 0; i < numSysFiles; ++i )
				{
					name = sysFiles[i];
					nfiles = FS_AddFileToList(name, list, nfiles);
				}

				Sys_FreeFileList(sysFiles);
			}
		}
	}

	*numfiles = nfiles;

	if ( !nfiles )
		return 0;

	listCopy = (char **)Z_Malloc( ( nfiles + sizeof(intptr_t) ) * sizeof( *listCopy ) );

	for ( i = 0; i < nfiles; ++i )
		listCopy[i] = list[i];

	listCopy[i] = 0;

	return listCopy;
}

/*
=======================
Sys_ConcatenateFileLists

mkv: Naive implementation. Concatenates three lists into a
	 new list, and frees the old lists from the heap.
bk001129 - from cvs1.17 (mkv)

FIXME TTimo those two should move to common.c next to Sys_ListFiles
=======================
 */
static unsigned int Sys_CountFileList( char **list )
{
	int i = 0;

	if ( list )
	{
		while ( *list )
		{
			list++;
			i++;
		}
	}
	return i;
}

static char** Sys_ConcatenateFileLists( char **list0, char **list1, char **list2 )
{
	int totalLength = 0;
	char** cat = NULL, **dst, **src;

	totalLength += Sys_CountFileList( list0 );
	totalLength += Sys_CountFileList( list1 );
	totalLength += Sys_CountFileList( list2 );

	/* Create new list. */
	dst = cat = (char **)Z_Malloc( ( totalLength + sizeof(intptr_t) ) * sizeof( char* ) );

	/* Copy over lists. */
	if ( list0 )
	{
		for ( src = list0; *src; src++, dst++ )
			*dst = *src;
	}
	if ( list1 )
	{
		for ( src = list1; *src; src++, dst++ )
			*dst = *src;
	}
	if ( list2 )
	{
		for ( src = list2; *src; src++, dst++ )
			*dst = *src;
	}

	// Terminate the list
	*dst = NULL;

	// Free our old lists.
	// NOTE: not freeing their content, it's been merged in dst and still being used
	if ( list0 )
	{
		Z_Free( list0 );
	}
	if ( list1 )
	{
		Z_Free( list1 );
	}
	if ( list2 )
	{
		Z_Free( list2 );
	}

	return cat;
}

/*
================
FS_GetModList

Returns a list of mod directory names
A mod directory is a peer to baseq3 with a pk3 in it
The directories are searched in base path, cd path and home path
================
*/
int FS_GetModList( char *listbuf, int bufsize )
{
	int nMods, i, j, nTotal, nLen, nPaks, nPotential, nDescLen;
	char **pFiles = NULL;
	char **pPaks = NULL;
	char *name;
	char path[MAX_OSPATH];
	char descPath[MAX_OSPATH];
	fileHandle_t descHandle;

	int dummy;
	char **pFiles0 = NULL;
	char **pFiles1 = NULL;
	char **pFiles2 = NULL;
	qboolean bDrop = qfalse;

	*listbuf = 0;
	nMods = nPotential = nTotal = 0;

	pFiles0 = Sys_ListFiles( fs_homepath->current.string, NULL, NULL, &dummy, qtrue );
	pFiles1 = Sys_ListFiles( fs_basepath->current.string, NULL, NULL, &dummy, qtrue );

	// DHM - Nerve :: Don't add blank paths (root)
	if ( fs_cdpath->current.string[0] )
	{
		pFiles2 = Sys_ListFiles( fs_cdpath->current.string, NULL, NULL, &dummy, qtrue );
	}

	// we searched for mods in the three paths
	// it is likely that we have duplicate names now, which we will cleanup below
	pFiles = Sys_ConcatenateFileLists( pFiles0, pFiles1, pFiles2 );
	nPotential = Sys_CountFileList( pFiles );

	for ( i = 0 ; i < nPotential ; i++ )
	{
		name = pFiles[i];
		// NOTE: cleaner would involve more changes
		// ignore duplicate mod directories
		if ( i != 0 )
		{
			bDrop = qfalse;
			for ( j = 0; j < i; j++ )
			{
				if ( Q_stricmp( pFiles[j],name ) == 0 )
				{
					// this one can be dropped
					bDrop = qtrue;
					break;
				}
			}
		}

		// we drop "baseq3" "." and ".."
		if ( !bDrop && Q_stricmpn( name, ".", 1 ) )
		{
			// now we need to find some .pk3 files to validate the mod
			// NOTE TTimo: (actually I'm not sure why .. what if it's a mod under developement with no .pk3?)
			// we didn't keep the information when we merged the directory names, as to what OS Path it was found under
			//   so it could be in base path, cd path or home path
			//   we will try each three of them here (yes, it's a bit messy)
			// NOTE Arnout: what about dropping the current loaded mod as well?
			FS_BuildOSPath( fs_basepath->current.string, name, "", path );
			nPaks = 0;
			pPaks = Sys_ListFiles( path, "iwd", NULL, &nPaks, qfalse );
			Sys_FreeFileList( pPaks ); // we only use Sys_ListFiles to check wether .pk3 files are present

			/* Try on cd path */
			if ( nPaks <= 0 )
			{
				FS_BuildOSPath( fs_cdpath->current.string, name, "", path );
				nPaks = 0;
				pPaks = Sys_ListFiles( path, "iwd", NULL, &nPaks, qfalse );
				Sys_FreeFileList( pPaks );
			}

			/* try on home path */
			if ( nPaks <= 0 )
			{
				FS_BuildOSPath( fs_homepath->current.string, name, "", path );
				nPaks = 0;
				pPaks = Sys_ListFiles( path, "iwd", NULL, &nPaks, qfalse );
				Sys_FreeFileList( pPaks );
			}

			if ( nPaks > 0 )
			{
				nLen = strlen( name ) + 1;
				// nLen is the length of the mod path
				// we need to see if there is a description available
				descPath[0] = '\0';
				strcpy( descPath, name );
				strcat( descPath, "/description.txt" );
				nDescLen = FS_SV_FOpenFileRead( descPath, &descHandle );
				if ( nDescLen > 0 && descHandle )
				{
					FILE *file;
					file = FS_FileForHandle( descHandle );
					Com_Memset( descPath, 0, sizeof( descPath ) );
					nDescLen = fread( descPath, 1, 48, file );
					if ( nDescLen >= 0 )
					{
						descPath[nDescLen] = '\0';
					}
					FS_FCloseFile( descHandle );
				}
				else if ( I_stricmp(name, "main") )
				{
					strcpy(descPath, name);
				}
				else
				{
					strcpy(descPath, "Call of Duty 2 Multiplayer");
				}
				nDescLen = strlen( descPath ) + 1;

				if ( nTotal + nLen + 1 + nDescLen + 1 < bufsize )
				{
					strcpy( listbuf, name );
					listbuf += nLen;
					strcpy( listbuf, descPath );
					listbuf += nDescLen;
					nTotal += nLen + nDescLen;
					nMods++;
				}
				else
				{
					break;
				}
			}
		}
	}
	Sys_FreeFileList( pFiles );

	return nMods;
}

int FS_GetFileList(const char *path, const char *extension, FsListBehavior behavior, char *listbuf, int bufsize)
{
	char** fileNames;
	int nLen;
	int nTotal;
	int i;
	int fileCount;

	*listbuf = 0;
	fileCount = 0;
	nTotal = 0;
	if (!I_stricmp(path, "$modlist"))
	{
		return FS_GetModList(listbuf, bufsize);
	}

	fileNames = FS_ListFiles(path, extension, behavior, &fileCount);
	for (i = 0; i < fileCount; ++i)
	{
		nLen = strlen(fileNames[i]) + 1;
		if (nTotal + nLen + 1 >= bufsize)
		{
			fileCount = i;
			break;
		}

		strcpy(listbuf, fileNames[i]);
		listbuf += nLen;
		nTotal += nLen;
	}

	FS_FreeFileList(fileNames);
	return fileCount;
}

int FS_SV_FOpenFileRead( const char *filename, fileHandle_t *fp )
{
	char ospath[MAX_OSPATH];
	fileHandle_t f = 0;

	FS_CheckFileSystemStarted();

	f = FS_HandleForFile(FS_THREAD_MAIN);
	fsh[f].zipFile = qfalse;

	Q_strncpyz( fsh[f].name, filename, sizeof( fsh[f].name ) );

	// search homepath
	FS_BuildOSPath( fs_homepath->current.string, filename, "", ospath );
	// remove trailing slash
	ospath[strlen( ospath ) - 1] = '\0';

	if ( fs_debug->current.integer )
	{
		Com_Printf( "FS_SV_FOpenFileRead (fs_homepath): %s\n", ospath );
	}

	fsh[f].handleFiles.file.o = fopen( ospath, "rb" );
	fsh[f].handleSync = qfalse;

	if ( !fsh[f].handleFiles.file.o )
	{
		// NOTE TTimo on non *nix systems, fs_homepath == fs_basepath, might want to avoid
		if ( Q_stricmp( fs_homepath->current.string,fs_basepath->current.string ) )
		{
			// search basepath
			FS_BuildOSPath( fs_basepath->current.string, filename, "", ospath );
			ospath[strlen( ospath ) - 1] = '\0';

			if ( fs_debug->current.integer )
			{
				Com_Printf( "FS_SV_FOpenFileRead (fs_basepath): %s\n", ospath );
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

	if ( f )
	{
		return FS_filelength( f );
	}

	return 0;
}

fileHandle_t FS_SV_FOpenFileWrite( const char *filename )
{
	char ospath[MAX_OSPATH];
	fileHandle_t f;

	if ( !fs_searchpaths )
	{
		Com_Error( ERR_FATAL, "Filesystem call made without initialization\n" );
	}

	FS_BuildOSPath( fs_homepath->current.string, filename, "", ospath );
	ospath[strlen( ospath ) - 1] = '\0';

	f = FS_HandleForFile(FS_THREAD_MAIN);
	fsh[f].zipFile = qfalse;

	if ( fs_debug->current.integer )
	{
		Com_Printf( "FS_SV_FOpenFileWrite: %s\n", ospath );
	}

	if ( FS_CreatePath( ospath ) )
	{
		return 0;
	}

	Com_DPrintf( "writing to: %s\n", ospath );
	fsh[f].handleFiles.file.o = fopen( ospath, "wb" );

	Q_strncpyz( fsh[f].name, filename, sizeof( fsh[f].name ) );

	fsh[f].handleSync = qfalse;

	if ( !fsh[f].handleFiles.file.o )
	{
		f = 0;
	}

	return f;
}

qboolean FS_iwIwd(char *iwd, const char *base)
{
	char *localized;
	char dest[76];
	int i;

	for ( i = 0; i < NUM_IW_IWDS; ++i )
	{
		if ( !FS_FilenameCompare(iwd, va("%s/iw_%02d", base, i)) )
			return qtrue;
	}

	localized = strstr(iwd, "localized_");

	if ( localized )
	{
		strcpy(dest, iwd);
		dest[localized - iwd + 10] = 0;

		if ( !FS_FilenameCompare(dest, va("%s/localized_", base)) )
		{
			strcpy(dest, localized + 10);
			I_strlwr(dest);

			for ( i = 0; i < NUM_IW_IWDS; ++i )
			{
				if ( strstr(dest, va("_iw%02d", i)) )
					return qtrue;
			}
		}
	}

	return qfalse;
}

const char *FS_LoadedIwdNames()
{
	static char info[BIG_INFO_STRING];
	searchpath_t *search;

	info[0] = 0;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		// is the element a iwd file?
		if ( !search->iwd )
			continue;

		if ( !search->localized )
		{
			if ( *info )
				I_strncat( info, sizeof( info ), " " );
			I_strncat( info, sizeof( info ), search->iwd->iwdBasename );
		}
	}

	return info;
}

const char *FS_ReferencedIwdNames()
{
	static char info[BIG_INFO_STRING];
	searchpath_t *search;

	info[0] = 0;

	// we want to return ALL iwd's from the fs_game path
	// and referenced one's from base
	for ( search = fs_searchpaths ; search ; search = search->next )
	{
		// is the element a iwd file?
		if ( !search->iwd )
			continue;

		if ( search->iwd->referenced || I_strnicmp( search->iwd->iwdGamename, BASEGAME, strlen( BASEGAME ) ) )
		{
			if ( *info )
				I_strncat( info, sizeof( info ), " " );
			I_strncat( info, sizeof( info ), search->iwd->iwdGamename );
			I_strncat( info, sizeof( info ), "/" );
			I_strncat( info, sizeof( info ), search->iwd->iwdBasename );
		}
	}

	return info;
}

const char *FS_ReferencedIwdChecksums()
{
	static char info[BIG_INFO_STRING];
	searchpath_t *search;

	info[0] = 0;

	for ( search = fs_searchpaths ; search ; search = search->next )
	{
		// is the element a iwd file?
		if ( !search->iwd )
			continue;

		// is the element a iwd file and has it been referenced based on flag?
		if ( search->iwd->referenced || I_strnicmp( search->iwd->iwdGamename, BASEGAME, strlen( BASEGAME ) ) )
			I_strncat( info, sizeof( info ), va( "%i ", search->iwd->checksum ) );
	}

	return info;
}

const char *FS_LoadedIwdChecksums()
{
	static char info[BIG_INFO_STRING];
	searchpath_t *search;

	info[0] = 0;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		// is the element a iwd file?
		if ( !search->iwd )
			continue;

		if ( !search->localized )
			I_strncat( info, sizeof( info ), va( "%i ", search->iwd->checksum ) );
	}

	return info;
}

const char *FS_LoadedIwdPureChecksums()
{
	static char info[BIG_INFO_STRING];
	searchpath_t *search;

	info[0] = 0;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		// is the element a iwd file?
		if ( !search->iwd )
			continue;

		if ( !search->localized )
			I_strncat( info, sizeof( info ), va( "%i ", search->iwd->pure_checksum ) );
	}

	return info;
}

char *FS_GetMapBaseName(const char *mapname)
{
	int c;
	int len;
	static char basename[MAX_QPATH];

	if ( !Q_stricmpn(mapname, "maps/mp/", 8) )
	{
		mapname += 8;
	}

	len = strlen(mapname);

	if (len >= sizeof(basename))
	{
		if ( !Q_stricmp(&mapname[len - 3], "bsp") )
		{
			len = len - 7;
		}
	}

	Com_Memcpy(basename, (char *)mapname, len);
	basename[len] = 0;

	for ( c = 0; c < len; ++c )
	{
		if ( basename[c] == '%' )
		{
			basename[c] = '_';
		}
	}

	return basename;
}

char** FS_ListFiles(const char* path, const char* extension, FsListBehavior behavior, int* numfiles)
{
	return FS_ListFilteredFiles(fs_searchpaths, path, extension, 0, behavior, numfiles);
}

void FS_Dir_f( void )
{
	const char    *path;
	const char    *extension;
	char   		  **dirnames;
	int ndirs;
	int i;

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

	dirnames = FS_ListFiles( path, extension, FS_LIST_PURE_ONLY, &ndirs );

	for ( i = 0; i < ndirs; i++ )
	{
		Com_Printf( "%s\n", dirnames[i] );
	}

	FS_FreeFileList( dirnames );
}

void FS_NewDir_f( void )
{
	const char	*filter;
	char	**dirnames;
	int		ndirs;
	int		i;

	if ( Cmd_Argc() < 2 )
	{
		Com_Printf( "usage: fdir <filter>\n" );
		Com_Printf( "example: fdir *q3dm*.bsp\n");
		return;
	}

	filter = Cmd_Argv( 1 );

	Com_Printf( "---------------\n" );

	dirnames = FS_ListFilteredFiles( fs_searchpaths, "", "", filter, FS_LIST_PURE_ONLY, &ndirs );

	FS_SortFileList(dirnames, ndirs);

	for ( i = 0; i < ndirs; i++ )
	{
		FS_ConvertPath(dirnames[i]);
		Com_Printf( "%s\n", dirnames[i] );
	}

	Com_Printf( "%d files listed\n", ndirs );
	FS_FreeFileList( dirnames );
}

int FS_TouchFile( const char *filename )
{
	fileHandle_t file;

	FS_FOpenFileRead(filename, &file, 0);

	if ( !file )
		return 0;

	FS_FCloseFile(file);
	return 1;
}

void FS_TouchFile_f( void )
{
	if ( Cmd_Argc() == 2 )
	{
		FS_TouchFile(Cmd_Argv(1));
	}
	else
	{
		Com_Printf("Usage: touchFile <file>\n");
	}
}

void FS_Path_f(void)
{
	FS_DisplayPath(1);
}

void FS_FullPath_f(void)
{
	FS_DisplayPath(0);
}

void FS_AddCommands()
{
	Cmd_AddCommand("path", FS_Path_f);
	Cmd_AddCommand("fullpath", FS_FullPath_f);
	Cmd_AddCommand("dir", FS_Dir_f);
	Cmd_AddCommand("fdir", FS_NewDir_f);
	Cmd_AddCommand("touchFile", FS_TouchFile_f);
}

void FS_RemoveCommands()
{
	Cmd_RemoveCommand("path");
	Cmd_RemoveCommand("fullpath");
	Cmd_RemoveCommand("dir");
	Cmd_RemoveCommand("fdir");
	Cmd_RemoveCommand("touchFile");
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
	FS_DisplayPath(1);
	Dvar_ClearModified(fs_gameDirVar);
	Com_Printf("----------------------\n");
	Com_Printf("%d files in iwd files\n", fs_packFiles);
}

void FS_ShutdownSearchPaths(searchpath_t *p)
{
	searchpath_t* next;

	while (p)
	{
		next = p->next;
		if (p->iwd)
		{
			unzClose(p->iwd->handle);
			Z_Free(p->iwd->buildBuffer);
			Z_Free(p->iwd);
		}
		if (p->dir)
		{
			Z_Free(p->dir);
		}
		Z_Free(p);
		p = next;
	}
}

void FS_Shutdown()
{
	int i;

	for (i = 1; i < MAX_FILE_HANDLES; ++i)
	{
		if (fsh[i].fileSize)
		{
			FS_FCloseFile(i);
		}
	}

	FS_ShutdownSearchPaths(fs_searchpaths);
	fs_searchpaths = NULL;
	FS_RemoveCommands();
}

void FS_SetRestrictions()
{
	searchpath_t* s;

	if ( fs_restrict->current.boolean )
	{
		Dvar_SetBool(fs_restrict, 1);
		Com_Printf("\nRunning in restricted demo mode.\n\n");
		FS_Shutdown();
		FS_Startup("demomain");

		for (s = fs_searchpaths; s; s = s->next)
		{
			if (FS_UseSearchPath(s) && s->iwd && s->iwd->checksum != -1277981599)
				Com_Error(ERR_FATAL, "Corrupted iw0.iwd: %u", s->iwd->iwdFilename);
		}
	}
}

void FS_Restart(int checksumFeed)
{
	FS_Shutdown();
	fs_checksumFeed = checksumFeed;
	FS_ClearIwdReferences();
	FS_Startup(BASEGAME);
	FS_SetRestrictions();

	// if we can't find default.cfg, assume that the paths are
	// busted and error out now, rather than getting an unreadable
	// graphics screen when the font fails to load
	if ( FS_ReadFile( DEFAULT_CONFIG, NULL ) <= 0 )
	{
		// this might happen when connecting to a pure server not using BASEGAME/pak0.pk3
		// (for instance a TA demo server)
		if ( lastValidBase[0] )
		{
			FS_PureServerSetLoadedIwds( "", "" );
			Dvar_SetString( fs_basepath, lastValidBase );
			Dvar_SetString( fs_gameDirVar, lastValidGame );
			lastValidBase[0] = '\0';
			lastValidGame[0] = '\0';
			Dvar_SetBool( fs_restrict, 0 );
			FS_Restart( checksumFeed );
			Com_Error( ERR_DROP, "Invalid game folder\n" );
			return;
		}
		// TTimo - added some verbosity, 'couldn't load default.cfg' confuses the hell out of users
		Com_Error( ERR_FATAL, "Couldn't load %s.  Make sure Call of Duty is run from the correct folder.", DEFAULT_CONFIG );
	}

	// bk010116 - new check before safeMode
	if ( I_stricmp( fs_gameDirVar->current.string, lastValidGame ) )
	{
		// skip the wolfconfig.cfg if "safe" is on the command line
		if ( !Com_SafeMode() )
		{
			Cbuf_AddText( va( "exec %s\n", CONFIG_NAME ) );
		}
	}

	I_strncpyz(lastValidBase, fs_basepath->current.string, sizeof(lastValidBase));
	I_strncpyz(lastValidGame, fs_gameDirVar->current.string, sizeof(lastValidGame));
}

void FS_RegisterDvars()
{
	char *homePath;

	if ( fs_debug )
		return;

	fs_debug = Dvar_RegisterInt("fs_debug", 0, 0, 2, DVAR_CHANGEABLE_RESET);
	fs_copyfiles = Dvar_RegisterBool("fs_copyfiles", 0, DVAR_INIT | DVAR_CHANGEABLE_RESET);
	fs_cdpath = Dvar_RegisterString("fs_cdpath", Sys_DefaultCDPath(), DVAR_INIT | DVAR_CHANGEABLE_RESET);
	fs_basepath = Dvar_RegisterString("fs_basepath", Sys_DefaultInstallPath(), DVAR_INIT | DVAR_CHANGEABLE_RESET);
	fs_basegame = Dvar_RegisterString("fs_basegame", "", DVAR_INIT | DVAR_CHANGEABLE_RESET);
	fs_useOldAssets = Dvar_RegisterBool("fs_useOldAssets", 0, DVAR_CHANGEABLE_RESET);
	homePath = (char *)Sys_DefaultHomePath();
	if (!homePath || !homePath[0])
		homePath = Sys_Cwd();
	fs_homepath = Dvar_RegisterString("fs_homepath", homePath, DVAR_INIT | DVAR_CHANGEABLE_RESET);
	fs_gameDirVar = Dvar_RegisterString("fs_game", "", DVAR_SERVERINFO | DVAR_SYSTEMINFO | DVAR_INIT | DVAR_CHANGEABLE_RESET);
	fs_restrict = Dvar_RegisterBool("fs_restrict", 0, DVAR_INIT | DVAR_CHANGEABLE_RESET);
	fs_ignoreLocalized = Dvar_RegisterBool("fs_ignoreLocalized", 0, DVAR_INIT | DVAR_CHANGEABLE_RESET);
#ifdef LIBCOD
	fs_library = Dvar_RegisterString("fs_library", "", DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
#endif
}

void FS_InitFilesystem()
{
	Com_StartupVariable("fs_cdpath");
	Com_StartupVariable("fs_basepath");
	Com_StartupVariable("fs_homepath");
	Com_StartupVariable("fs_game");
	Com_StartupVariable("fs_copyfiles");
	Com_StartupVariable("fs_restrict");
	Com_StartupVariable("loc_language");
	SEH_InitLanguage();
	FS_Startup("main");
	FS_SetRestrictions();

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