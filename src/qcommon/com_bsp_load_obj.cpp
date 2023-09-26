#include "qcommon.h"

#define MAX_BSP_CHUNCKS 39

struct BspChunk
{
	int type;
	unsigned int length;
};

struct BspHeader
{
	unsigned int ident;
	unsigned int version;
	BspChunk chunks[MAX_BSP_CHUNCKS];
};

typedef struct comBspGlob_s
{
	BspHeader *header;
	unsigned int fileSize;
	unsigned int checksum;
} comBspGlob_t;
// static_assert((sizeof(comBspGlob_t) == 0xC), "ERROR: comBspGlob_t size is invalid!");

comBspGlob_t comBspGlob;

qboolean Com_IsBspLoaded()
{
	return comBspGlob.header != 0;
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

int Com_GetBspHeaderData(int data)
{
	return data;
}

void Com_UnloadBsp()
{
	Z_FreeInternal(comBspGlob.header);
	comBspGlob.header = 0;
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
	return Com_ValidateBspLumpData(type, comBspGlob.header->chunks[type].length, comBspGlob.header->chunks[type].type, elemSize, count);
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
	bytesRead  = FS_Read(comBspGlob.header, comBspGlob.fileSize, h);
	FS_FCloseFile(h);

	if ( bytesRead  != comBspGlob.fileSize || comBspGlob.fileSize <= 0x13F )
	{
		Z_Free(comBspGlob.header);
		Com_Error(ERR_DROP, "EXE_ERR_COULDNT_LOAD\x15%s", filename);
	}

	comBspGlob.checksum = Com_BlockChecksum(comBspGlob.header, comBspGlob.fileSize);
	comBspGlob.header->ident = Com_GetBspHeaderData(comBspGlob.header->ident);
	comBspGlob.header->version = Com_GetBspHeaderData(comBspGlob.header->version);

	if ( comBspGlob.header->ident != (uint32_t)'PSBI' || comBspGlob.header->version != 4 )
	{
		Z_Free(comBspGlob.header);
		Com_Error(ERR_DROP, "EXE_ERR_WRONG_MAP_VERSION_NUM\x15%s", filename);
	}

	for ( i = 0; i < MAX_BSP_CHUNCKS; ++i )
	{
		comBspGlob.header->chunks[i].type = Com_GetBspHeaderData(comBspGlob.header->chunks[i].type);
		comBspGlob.header->chunks[i].length = Com_GetBspHeaderData(comBspGlob.header->chunks[i].length);
	}
}