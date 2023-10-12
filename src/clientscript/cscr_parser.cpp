#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

typedef struct scrParserGlob_s
{
	OpcodeLookup *opcodeLookup;
	unsigned int opcodeLookupMaxLen;
	unsigned int opcodeLookupLen;
	SourceLookup *sourcePosLookup;
	unsigned int sourcePosLookupMaxLen;
	unsigned int sourcePosLookupLen;
	unsigned int sourceBufferLookupMaxLen;
	const char *currentCodePos;
	unsigned int currentSourcePosCount;
	SaveSourceBufferInfo *saveSourceBufferLookup;
	unsigned int saveSourceBufferLookupLen;
	int delayedSourceIndex;
	int threadStartSourceIndex;
} scrParserGlob_t;
// static_assert((sizeof(scrParserGlob_t) == 0x34), "ERROR: scrParserGlob_t size is invalid!");

scrParserGlob_t scrParserGlob;
scrParserPub_t scrParserPub;

char g_EndPos;

unsigned int Scr_GetLineNumInternal(const char *buf, unsigned int sourcePos, const char **startLine, int *col)
{
	unsigned int lineNum;

	*startLine = buf;
	lineNum = 0;

	while ( sourcePos )
	{
		if ( !*buf )
		{
			*startLine = buf + 1;
			++lineNum;
		}

		++buf;
		--sourcePos;
	}

	*col = buf - *startLine;
	return lineNum;
}

unsigned int Scr_GetLineInfo(const char *buf, unsigned int sourcePos, int *col, char *line)
{
	char b, c;
	const char *startLine;
	unsigned int linenum;
	int i;
	int length;

	linenum = Scr_GetLineNumInternal(buf, sourcePos, &startLine, col);
	length = strlen(startLine);

	if ( length > MAX_STRING_CHARS - 1 )
		length = MAX_STRING_CHARS - 1;

	for ( i = 0; i <= length; ++i )
	{
		b = startLine[i];

		if ( b == 9 )
			c = 32;
		else
			c = b;

		line[i] = c;
	}

	if ( line[length - 1] == 13 )
		line[length - 1] = 0;

	return linenum;
}

void Scr_PrintSourcePos(conChannel_t channel, const char *filename, const char *buf, unsigned int sourcePos)
{
	const char *save;
	int i;
	int col;
	unsigned int line;
	char string[MAX_STRING_CHARS];

	line = Scr_GetLineInfo(buf, sourcePos, &col, string);

	if ( scrParserGlob.saveSourceBufferLookup )
		save = " (savegame)";
	else
		save = "";

	Com_PrintMessage(channel, va("(file '%s'%s, line %d)\n", filename, save, line + 1));
	Com_PrintMessage(channel, va("%s\n", string));

	for ( i = 0; i < col; ++i )
		Com_PrintMessage(channel, " ");

	Com_PrintMessage(channel, "*\n");
}

OpcodeLookup* Scr_GetPrevSourcePosOpcodeLookup(const char *codePos)
{
	int middle;
	signed int high;
	signed int low;

	low = 0;
	high = scrParserGlob.opcodeLookupLen - 1;

	while ( low <= high )
	{
		middle = (high + low) / 2;

		if ( codePos < scrParserGlob.opcodeLookup[middle].codePos )
		{
			high = middle - 1;
		}
		else
		{
			low = middle + 1;

			if ( middle + 1 == scrParserGlob.opcodeLookupLen || codePos < scrParserGlob.opcodeLookup[low].codePos )
				return &scrParserGlob.opcodeLookup[middle];
		}
	}

	return 0;
}

unsigned int Scr_GetPrevSourcePos(const char *codePos, unsigned int index)
{
	return scrParserGlob.sourcePosLookup[Scr_GetPrevSourcePosOpcodeLookup(codePos)->sourcePosIndex + index].sourcePos;
}

unsigned int Scr_GetSourceBuffer(const char *codePos)
{
	signed int bufferIndex;

	for ( bufferIndex = scrParserPub.sourceBufferLookupLen - 1;
	        bufferIndex > 0
	        && (!scrParserPub.sourceBufferLookup[bufferIndex].codePos
	            || scrParserPub.sourceBufferLookup[bufferIndex].codePos > codePos);
	        --bufferIndex )
	{
		;
	}

	return bufferIndex;
}

void Scr_PrintPrevCodePos(conChannel_t channel, const char *codePos, unsigned int index)
{
	unsigned int bufferIndex;

	if ( !codePos )
	{
		Com_PrintMessage(channel, "<frozen thread>\n");
		return;
	}

	if ( codePos == &g_EndPos )
	{
		Com_PrintMessage(channel, "<removed thread>\n");
	}
	else
	{
		if ( scrVarPub.developer )
		{
			if ( scrVarPub.programBuffer && Scr_IsInOpcodeMemory(codePos) )
			{
				bufferIndex = Scr_GetSourceBuffer(codePos - 1);
				Scr_PrintSourcePos(
				    channel,
				    scrParserPub.sourceBufferLookup[bufferIndex].buf,
				    scrParserPub.sourceBufferLookup[bufferIndex].sourceBuf,
				    Scr_GetPrevSourcePos(codePos - 1, index));
				return;
			}
		}
		else if ( Scr_IsInOpcodeMemory(codePos - 1) )
		{
			Com_PrintMessage(channel, va("@ %d\n", codePos - scrVarPub.programBuffer));
			return;
		}

		Com_PrintMessage(channel, va("%s\n\n", codePos));
	}
}

void AddOpcodePos(unsigned int sourcePos, int type)
{
	int sourcePosLookupIndex;
	SourceLookup *opcodeLookup;
	OpcodeLookup *sourcePosLookup;
	SourceLookup *newSourcePosLookup;
	OpcodeLookup *newOpcodeLookup;

	if ( scrVarPub.developer && scrCompilePub.developer_statement != 2 )
	{
		if ( !scrCompilePub.allowedBreakpoint )
			type &= ~SOURCE_TYPE_BREAKPOINT;

		if ( scrParserGlob.opcodeLookupLen >= scrParserGlob.opcodeLookupMaxLen )
		{
			scrParserGlob.opcodeLookupMaxLen *= 2;
			newOpcodeLookup = (OpcodeLookup *)Z_MallocInternal(sizeof(OpcodeLookup) * scrParserGlob.opcodeLookupMaxLen);
			Com_Memcpy(newOpcodeLookup, scrParserGlob.opcodeLookup, sizeof(OpcodeLookup) * scrParserGlob.opcodeLookupLen);
			Z_FreeInternal(scrParserGlob.opcodeLookup);
			scrParserGlob.opcodeLookup = newOpcodeLookup;
		}

		if ( scrParserGlob.sourcePosLookupLen >= scrParserGlob.sourcePosLookupMaxLen )
		{
			scrParserGlob.sourcePosLookupMaxLen *= 2;
			newSourcePosLookup = (SourceLookup *)Z_MallocInternal(sizeof(SourceLookup) * scrParserGlob.sourcePosLookupMaxLen);
			Com_Memcpy(newSourcePosLookup, scrParserGlob.sourcePosLookup, sizeof(SourceLookup) * scrParserGlob.sourcePosLookupLen);
			Z_FreeInternal(scrParserGlob.sourcePosLookup);
			scrParserGlob.sourcePosLookup = newSourcePosLookup;
		}

		if ( scrParserGlob.currentCodePos == scrCompilePub.opcodePos )
		{
			sourcePosLookup = &scrParserGlob.opcodeLookup[--scrParserGlob.opcodeLookupLen];
		}
		else
		{
			scrParserGlob.currentSourcePosCount = 0;
			scrParserGlob.currentCodePos = scrCompilePub.opcodePos;
			sourcePosLookup = &scrParserGlob.opcodeLookup[scrParserGlob.opcodeLookupLen];
			sourcePosLookup->sourcePosIndex = scrParserGlob.sourcePosLookupLen;
			sourcePosLookup->codePos = scrParserGlob.currentCodePos;
		}

		sourcePosLookupIndex = sourcePosLookup->sourcePosIndex + scrParserGlob.currentSourcePosCount;
		opcodeLookup = &scrParserGlob.sourcePosLookup[sourcePosLookupIndex];
		opcodeLookup->sourcePos = sourcePos;

		if ( sourcePos == -1 )
		{
			scrParserGlob.delayedSourceIndex = sourcePosLookupIndex;
		}
		else if ( sourcePos == -2 )
		{
			scrParserGlob.threadStartSourceIndex = sourcePosLookupIndex;
		}
		else if ( scrParserGlob.delayedSourceIndex >= 0 && (type & SOURCE_TYPE_BREAKPOINT) != 0 )
		{
			scrParserGlob.sourcePosLookup[scrParserGlob.delayedSourceIndex].sourcePos = sourcePos;
			scrParserGlob.delayedSourceIndex = -1;
		}

		opcodeLookup->type |= type;
		sourcePosLookup->sourcePosCount = ++scrParserGlob.currentSourcePosCount;
		++scrParserGlob.opcodeLookupLen;
		++scrParserGlob.sourcePosLookupLen;
	}
}

void RemoveOpcodePos()
{
	if ( scrVarPub.developer && scrCompilePub.developer_statement != 2 )
	{
		--scrParserGlob.sourcePosLookupLen;
		--scrParserGlob.opcodeLookupLen;

		if ( !--scrParserGlob.currentSourcePosCount )
			scrParserGlob.currentCodePos = 0;

		scrParserGlob.opcodeLookup[scrParserGlob.opcodeLookupLen].sourcePosCount = scrParserGlob.currentSourcePosCount;
	}
}

void AddThreadStartOpcodePos(unsigned int sourcePos)
{
	SourceLookup *lookup;

	if ( scrVarPub.developer )
	{
		if ( scrCompilePub.developer_statement != 2 )
		{
			lookup = &scrParserGlob.sourcePosLookup[scrParserGlob.threadStartSourceIndex];
			lookup->sourcePos = sourcePos;
			lookup->type = 4;
			scrParserGlob.threadStartSourceIndex = -1;
		}
	}
}

void Scr_InitOpcodeLookup()
{
	if ( scrVarPub.developer )
	{
		scrParserGlob.delayedSourceIndex = -1;
		scrParserGlob.opcodeLookupMaxLen = 0x10000;
		scrParserGlob.opcodeLookupLen = 0;
		scrParserGlob.opcodeLookup = (OpcodeLookup *)Z_MallocInternal(sizeof(OpcodeLookup) * 65536);
		Com_Memset(scrParserGlob.opcodeLookup, 0, sizeof(OpcodeLookup) * scrParserGlob.opcodeLookupMaxLen);
		scrParserGlob.sourcePosLookupMaxLen = 0x10000;
		scrParserGlob.sourcePosLookupLen = 0;
		scrParserGlob.sourcePosLookup = (SourceLookup *)Z_MallocInternal(sizeof(SourceLookup) * 65536);
		scrParserGlob.currentCodePos = 0;
		scrParserGlob.currentSourcePosCount = 0;
		scrParserGlob.sourceBufferLookupMaxLen = 16;
		scrParserPub.sourceBufferLookupLen = 0;
		scrParserPub.sourceBufferLookup = (SourceBufferInfo *)Z_MallocInternal(sizeof(SourceBufferInfo) * 16);
	}
}

SourceBufferInfo *Scr_GetNewSourceBuffer()
{
	SourceBufferInfo *newBuffer;

	if ( scrParserPub.sourceBufferLookupLen >= scrParserGlob.sourceBufferLookupMaxLen )
	{
		scrParserGlob.sourceBufferLookupMaxLen *= 2;
		newBuffer = (SourceBufferInfo *)Z_MallocInternal(sizeof(SourceBufferInfo) * scrParserGlob.sourceBufferLookupMaxLen);
		Com_Memcpy(newBuffer, scrParserPub.sourceBufferLookup, sizeof(SourceBufferInfo) * scrParserPub.sourceBufferLookupLen);
		Z_FreeInternal(scrParserPub.sourceBufferLookup);
		scrParserPub.sourceBufferLookup = newBuffer;
	}

	return &scrParserPub.sourceBufferLookup[scrParserPub.sourceBufferLookupLen++];
}

void Scr_AddSourceBufferInternal(const char *extFilename, const char *codePos, char *sourceBuf, int len, bool doEolFixup, bool archive)
{
	SourceBufferInfo *newBuffer;
	const char *source;
	char *buf;
	char c;
	char count;
	int i;
	int j;
	char *tmp;
	size_t size;
	char *dest;

	if ( scrParserPub.sourceBufferLookup )
	{
		size = strlen(extFilename) + 1;
		dest = (char *)Z_MallocInternal(size + len + 2);
		strcpy(dest, extFilename);

		if ( sourceBuf )
			source = &dest[size];
		else
			source = 0;

		buf = sourceBuf;
		tmp = (char *)source;

		if ( doEolFixup )
		{
			for ( i = 0; i <= len; ++i )
			{
				c = *buf++;

				if ( c == 10 || c == 13 && *buf != 10 )
					*tmp = 0;
				else
					*tmp = c;

				++tmp;
			}
		}
		else
		{
			for ( j = 0; j <= len; ++j )
			{
				count = *buf++;
				*tmp++ = count;
			}
		}

		newBuffer = Scr_GetNewSourceBuffer();
		newBuffer->codePos = codePos;
		newBuffer->buf = dest;
		newBuffer->sourceBuf = source;
		newBuffer->len = len;
		newBuffer->sortedIndex = -1;
		newBuffer->archive = archive;

		if ( source )
			scrParserPub.sourceBuf = source;
	}
	else
	{
		scrParserPub.sourceBuf = 0;
	}
}

char* Scr_ReadFile(const char *filename, const char *extFilename, const char *codePos, bool archive)
{
	fileHandle_t f;
	int len;
	char *buffer;

	len = FS_FOpenFileByMode(extFilename, &f, FS_READ);

	if ( len >= 0 )
	{
		buffer = (char *)Hunk_AllocateTempMemoryHighInternal(len + 1);
		FS_Read(buffer, len, f);
		buffer[len] = 0;
		FS_FCloseFile(f);
		Scr_AddSourceBufferInternal(extFilename, codePos, buffer, len, 1, archive);
		return buffer;
	}
	else
	{
		Scr_AddSourceBufferInternal(extFilename, codePos, 0, -1, 1, archive);
		return 0;
	}
}

char* Scr_AddSourceBuffer(const char *filename, const char *extFilename, const char *codePos, bool archive)
{
	char eol;
	SaveSourceBufferInfo *infoLookup;
	char *source;
	int len;
	char *sourceBuf;
	char c;
	int i;
	char *dest;

	if ( !archive || !scrParserGlob.saveSourceBufferLookup )
		return Scr_ReadFile(filename, extFilename, codePos, archive);

	infoLookup = &scrParserGlob.saveSourceBufferLookup[--scrParserGlob.saveSourceBufferLookupLen];
	len = infoLookup->len;

	if ( len >= 0 )
	{
		sourceBuf = (char *)Hunk_AllocateTempMemoryHighInternal(len + 1);
		source = infoLookup->sourceBuf;
		dest = sourceBuf;

		for ( i = 0; i < len; ++i )
		{
			c = *source++;
			eol = c;
			if ( !c )
				eol = 10;
			*dest++ = eol;
		}

		*dest = 0;

		if ( infoLookup->sourceBuf )
			Z_FreeInternal(scrParserGlob.saveSourceBufferLookup[scrParserGlob.saveSourceBufferLookupLen].sourceBuf);
	}
	else
	{
		sourceBuf = 0;
	}

	Scr_AddSourceBufferInternal(extFilename, codePos, sourceBuf, len, 1, archive);
	return sourceBuf;
}

void Scr_ShutdownOpcodeLookup()
{
	unsigned int i;
	unsigned int j;

	if ( scrParserGlob.opcodeLookup )
	{
		Z_FreeInternal(scrParserGlob.opcodeLookup);
		scrParserGlob.opcodeLookup = 0;
	}

	if ( scrParserGlob.sourcePosLookup )
	{
		Z_FreeInternal(scrParserGlob.sourcePosLookup);
		scrParserGlob.sourcePosLookup = 0;
	}

	if ( scrParserPub.sourceBufferLookup )
	{
		for ( i = 0; i < scrParserPub.sourceBufferLookupLen; ++i )
			Z_FreeInternal(scrParserPub.sourceBufferLookup[i].buf);

		Z_FreeInternal(scrParserPub.sourceBufferLookup);
		scrParserPub.sourceBufferLookup = 0;
	}

	if ( scrParserGlob.saveSourceBufferLookup )
	{
		for ( j = 0; j < scrParserGlob.saveSourceBufferLookupLen; ++j )
		{
			if ( scrParserGlob.saveSourceBufferLookup[j].sourceBuf )
				Z_FreeInternal(scrParserGlob.saveSourceBufferLookup[j].sourceBuf);
		}

		Z_FreeInternal(scrParserGlob.saveSourceBufferLookup);
		scrParserGlob.saveSourceBufferLookup = 0;
	}
}

void CompileError(unsigned int sourcePos, const char *format, ...)
{
	char buf[MAX_STRING_CHARS];
	va_list argptr;

	va_start( argptr, format );
	Q_vsnprintf( buf, sizeof( buf ), format, argptr );
	va_end( argptr );

	if ( !scrVarPub.evaluate )
	{
		Com_Printf("\n");
		Com_Printf("******* script compile error *******\n");

		if ( scrVarPub.developer )
		{
			Com_Printf("%s: ", buf);
			Scr_PrintSourcePos(CON_CHANNEL_DONT_FILTER, scrParserPub.scriptfilename, scrParserPub.sourceBuf, sourcePos);
		}
		else
		{
			Com_Printf("%s\n", buf);
		}

		Com_Printf("************************************\n");
		Com_Error(ERR_SCRIPT_DROP, "script compile error\n(see console for details)\n");
	}

	if ( !scrVarPub.error_message )
		scrVarPub.error_message = va("%s", buf);
}

void CompileError2(const char *codePos, const char *format, ...)
{
	char buf[MAX_STRING_CHARS];
	va_list argptr;

	Com_Printf("\n");
	Com_Printf("******* script compile error *******\n");
	va_start( argptr, format );
	Q_vsnprintf( buf, sizeof( buf ), format, argptr );
	va_end( argptr );
	Com_Printf("%s: ", buf);
	Scr_PrintPrevCodePos(CON_CHANNEL_DONT_FILTER, codePos, 0);
	Com_Printf("************************************\n");
	Com_Error(ERR_SCRIPT_DROP, "script compile error\n(see console for details)\n");
}

int QDECL Scr_ScanFile(char *buf, int max_size)
{
	char c;
	int i;
	char ptr;

	ptr = 42;

	for ( i = 0; i < max_size; ++i )
	{
		c = *scrCompilePub.in_ptr++;
		ptr = c;

		if ( !c || c == 10 )
			break;

		buf[i] = c;
	}

	if ( ptr == 10 )
	{
		buf[i++] = 10;
	}
	else if ( !ptr )
	{
		if ( scrCompilePub.parseBuf )
		{
			scrCompilePub.in_ptr = scrCompilePub.parseBuf;
			scrCompilePub.parseBuf = 0;
		}
		else
		{
			--scrCompilePub.in_ptr;
		}
	}

	return i;
}
