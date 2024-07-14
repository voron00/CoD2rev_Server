#include "qcommon.h"

#define IBSP_VERSION 4

struct BspLump
{
	unsigned int length;
	unsigned int offset;
};

struct BspHeader
{
	unsigned int ident;
	unsigned int version;
	BspLump lumps[LUMP_COUNT];
};

typedef struct comBspGlob_s
{
	BspHeader *header;
	unsigned int fileSize;
	unsigned int checksum;
} comBspGlob_t;
#if defined(__i386__)
static_assert((sizeof(comBspGlob_t) == 0xC), "ERROR: comBspGlob_t size is invalid!");
#endif

comBspGlob_t comBspGlob;

qboolean Com_IsBspLoaded()
{
	return comBspGlob.header != NULL;
}

const char *GetBspExtension()
{
	const char *string;

	string = Dvar_GetString("gfx_driver");

	if ( *string )
		return va("%sbsp", string);
	else
		return va("d3dbsp");
}

void Com_UnloadBsp()
{
	Z_FreeInternal(comBspGlob.header);
	comBspGlob.header = NULL;
}

void Com_CleanupBsp()
{
	if ( Com_IsBspLoaded() )
		Com_UnloadBsp();
}

BspHeader* Com_GetBspHeader(unsigned int *size, unsigned int *checksum)
{
	if ( size )
		*size = comBspGlob.fileSize;

	if ( checksum )
		*checksum = comBspGlob.checksum;

	return comBspGlob.header;
}

byte* Com_ValidateBspLumpData(int type, unsigned int offset, unsigned int length, unsigned int elemSize, unsigned int *count)
{
	if ( length )
	{
		if ( type != LUMP_ENTITIES && length + offset > comBspGlob.fileSize )
		{
			Com_Error(ERR_DROP, "CM_LoadMap: lump %i extends past end of file", type);
		}

		*count = length / elemSize;

		if ( elemSize * *count != length )
		{
			Com_Error(ERR_DROP, "CM_LoadMap: lump %i has funny size", type);
		}

		return (byte*)comBspGlob.header + offset;
	}

	*count = 0;
	return NULL;
}

byte* Com_GetBspLump(int type, unsigned int elemSize, unsigned int *count)
{
	return Com_ValidateBspLumpData(type, comBspGlob.header->lumps[type].offset, comBspGlob.header->lumps[type].length, elemSize, count);
}

bool Com_BspHasLump(int type)
{
	unsigned int count;

	Com_GetBspLump(type, 1u, &count);

	return count != 0;
}

void Com_LoadBsp(const char *filename)
{
	fileHandle_t h;
	unsigned int bytesRead;
	int i;

	comBspGlob.fileSize = FS_FOpenFileRead(filename, &h, 0);

	if ( !h )
	{
		Com_Error(ERR_DROP, "EXE_ERR_COULDNT_LOAD\x15%s", filename);
	}

	comBspGlob.header = (BspHeader *)Z_MallocGarbage(comBspGlob.fileSize);
	bytesRead = FS_Read(comBspGlob.header, comBspGlob.fileSize, h);
	FS_FCloseFile(h);

	if ( bytesRead != comBspGlob.fileSize || comBspGlob.fileSize < 320 )
	{
		Z_Free(comBspGlob.header);
		Com_Error(ERR_DROP, "EXE_ERR_COULDNT_LOAD\x15%s", filename);
	}

	comBspGlob.checksum = Com_BlockChecksum(comBspGlob.header, comBspGlob.fileSize);

	comBspGlob.header->ident = LittleLong(comBspGlob.header->ident);
	comBspGlob.header->version = LittleLong(comBspGlob.header->version);

	if ( comBspGlob.header->ident != (uint32_t)'PSBI' || comBspGlob.header->version != IBSP_VERSION )
	{
		Z_Free(comBspGlob.header);
		Com_Error(ERR_DROP, "EXE_ERR_WRONG_MAP_VERSION_NUM\x15%s", filename);
	}

	for ( i = 0; i != LUMP_COUNT; ++i )
	{
		comBspGlob.header->lumps[i].length = LittleLong(comBspGlob.header->lumps[i].length);
		comBspGlob.header->lumps[i].offset = LittleLong(comBspGlob.header->lumps[i].offset);
	}
}