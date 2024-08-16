#include <dirent.h>
#include "qcommon.h"

int CL_LocalClient_GetActiveCount()
{
	return 1;
}

void Sys_SnapVector(vec3_t v)
{
	v[0] = rint(v[0]);
	v[1] = rint(v[1]);
	v[2] = rint(v[2]);
}

qboolean Sys_DirectoryHasContents(const char *dir)
{
	DIR *hdir;
	struct dirent *hfiles;

	hdir = opendir(dir);

	if ( hdir )
	{
		hfiles = readdir(hdir);
		while ( hfiles )
		{
			if ( hfiles->d_reclen != 4 || hfiles->d_name[0] != '.' )
			{
				closedir(hdir);
				return qtrue;
			}
			hfiles = readdir(hdir);
		}
		closedir(hdir);
	}

	return qfalse;
}

void Sys_OutOfMemErrorInternal(const char *filename, int line)
{
	Com_Printf("OUT OF MEMORY! ABORTING!!! (%s:%d)\n", filename, line);
	exit(-1);
}

void Sys_UnimplementedFunctionInternal(const char *function)
{
	Com_Printf("FUNCTION NOT IMPLEMENTED!!! (%s)\n", function);
}