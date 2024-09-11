#include "../qcommon/qcommon.h"
#include "../qcommon/sys_thread.h"

static ParseThreadInfo q_parse[NUMTHREADS];

// multiple character punctuation tokens
static const char *punctuation[] =
{
	"+=", "-=",  "*=",  "/=", "&=", "|=", "++", "--",
	"&&", "||",  "<=",  ">=", "==", "!=",
	NULL
};

/*
==============
Com_Compress
==============
*/
int Com_Compress( char *data_p )
{
	char *datai, *datao;
	int size;
	char c, pc;
	qboolean ws = qfalse;

	size = 0;
	pc = 0;
	datai = datao = data_p;
	if ( datai )
	{
		while ( ( c = *datai ) != 0 )
		{
			if ( c == 13 || c == 10 )
			{
				*datao = c;
				datao++;
				ws = qfalse;
				pc = c;
				size++;
				datai++;
				// skip double slash comments
			}
			else if ( c == '/' && datai[1] == '/' )
			{
				while ( *datai && *datai != '\n' )
				{
					datai++;
				}
				ws = qfalse;
				// skip /* */ comments
			}
			else if ( c == '/' && datai[1] == '*' )
			{
				//datai += 2; // Arnout: skip over '/*'
				while ( *datai && ( *datai != '*' || datai[1] != '/' ) )
				{
					if ( *datai == 10 )
					{
						*datao = 10;
						datao++;
						++size;
					}
					datai++;
				}
				if ( *datai )
				{
					datai += 2;
				}
				ws = qfalse;
			}
			else
			{
				if ( ws )
				{
					*datao = ' ';
					datao++;
				}
				*datao = c;
				datao++;
				size++;
				datai++;
				ws = qfalse;
				pc = c;
			}
		}
	}
	*datao = 0;
	return size;
}

/*
==============
Com_GetParseThreadInfo
==============
*/
ParseThreadInfo *Com_GetParseThreadInfo()
{
	return q_parse;
}

/*
==============
Com_GetLastTokenPos
==============
*/
const char *Com_GetLastTokenPos()
{
	return Com_GetParseThreadInfo()->tokenPos;
}

/*
==============
Com_ParseReturnToMark
==============
*/
void Com_ParseReturnToMark( const char *( *text ), com_parse_mark_t *mark )
{
	parseInfo_t *pi;
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	assert(text);
	assert(mark);

	pi->lines = mark->lines;
	*text = mark->text;
	pi->ungetToken = mark->ungetToken != qfalse;
	pi->backup_lines = mark->backup_lines;
	pi->backup_text = mark->backup_text;
}

/*
==============
Com_ParseSetMark
==============
*/
void Com_ParseSetMark( const char *( *text ), com_parse_mark_t *mark )
{
	parseInfo_t *pi;
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	assert(text);
	assert(mark);

	mark->lines = pi->lines;
	mark->text = *text;
	mark->ungetToken = pi->ungetToken;
	mark->backup_lines = pi->backup_lines;
	mark->backup_text = pi->backup_text;
}

/*
==============
Com_SetScriptWarningPrefix
==============
*/
void Com_SetScriptWarningPrefix( const char *prefix )
{
	parseInfo_t *pi;
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	assert(prefix);
	assert(parse->parseInfoNum > 0);

	pi->warningPrefix = prefix;
}

/*
==============
Com_SetScriptErrorPrefix
==============
*/
void Com_SetScriptErrorPrefix( const char *prefix )
{
	parseInfo_t *pi;
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	assert(prefix);
	assert(parse->parseInfoNum > 0);

	pi->errorPrefix = prefix;
}

/*
====================
Com_ParseRestOfLine
====================
*/
const char *Com_ParseRestOfLine( const char *( *data_p ) )
{
	char *line;
	const char *token;

	line = Com_GetParseThreadInfo()->line;
	line[0] = 0;
	while ( 1 )
	{
		token = Com_ParseOnLine( data_p );
		if ( !token[0] )
		{
			break;
		}
		if ( line[0] )
		{
			Q_strncat( line, MAX_TOKEN_CHARS, " " );
		}
		Q_strncat( line, MAX_TOKEN_CHARS, token );
	}

	return line;
}

/*
=================
Com_SkipRestOfLine
=================
*/
void Com_SkipRestOfLine( const char *( *data ) )
{
	ParseThreadInfo *parse;
	const char  *p;
	int c;

	parse = Com_GetParseThreadInfo();
	p = *data;

	if ( !p )
	{
		return;
	}

	while ( 1 )
	{
		c = *p;
		if ( !*p )
			break;
		p++;
		if ( c == '\n' )
		{
			parse->parseInfo[parse->parseInfoNum].lines++;
			break;
		}
	}

	*data = p;
}

/*
====================
Com_GetArgCountOnLine
====================
*/
int Com_GetArgCountOnLine( const char *( *data_p ) )
{
	com_parse_mark_t mark;
	const char *token;
	int count;

	Com_ParseSetMark(data_p, &mark);
	for ( count = 0; ; count++ )
	{
		token = Com_ParseOnLine(data_p);
		if ( !token[0] )
			break;
	}
	Com_ParseReturnToMark(data_p, &mark);

	return count;
}

/*
=================
Com_SkipBracedSection

The next token should be an open brace.
Skips until a matching close brace is found.
Internal brace depths are properly skipped.
=================
*/
qboolean Com_SkipBracedSection( const char *( *program ), int iMaxNesting )
{
	const char *token;

	qboolean bNestingExceeded = qfalse;
	int depth = 0;
	do
	{
		token = Com_Parse( program );
		if ( token[1] == 0 )
		{
			if ( token[0] == '{' )
			{
				if ( depth == iMaxNesting )
					bNestingExceeded = qtrue;
				else
					depth++;
			}
			else if ( token[0] == '}' )
			{
				depth--;
			}
		}
	}
	while ( depth && *program );
	return bNestingExceeded;
}

/*
===================
Com_ScriptError

Prints the script name and line number in the message
===================
*/
void Com_ScriptError( const char *msg, ... )
{
	char string[MAXPRINTMSG];
	parseInfo_t *pi;
	ParseThreadInfo *parse;
	va_list va;

	va_start(va, msg);

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	Q_vsnprintf(string, sizeof(string), msg, va);

	if ( parse->parseInfoNum )
		Com_Error(ERR_DROP, "File %s, line %i: %s", pi->errorPrefix, pi->parseFile, pi->lines, string);
	else
		Com_Error(ERR_DROP, "%s", string);
}

/*
===================
Com_InitParseInfo
===================
*/
void Com_InitParseInfo( parseInfo_t *pi )
{
	pi->lines = 1;
	pi->ungetToken = 0;
	pi->spaceDelimited = 1;
	pi->keepStringQuotes = 0;
	pi->csv = 0;
	pi->negativeNumbers = 0;
	pi->errorPrefix = "";
	pi->warningPrefix = "";
	pi->backup_lines = 0;
	pi->backup_text = 0;
}

/*
===================
Com_ResetParseSessions
===================
*/
void Com_ResetParseSessions()
{
	Com_GetParseThreadInfo()->parseInfoNum = 0;
}

/*
===================
SkipWhitespace
===================
*/
static const char *SkipWhitespace( const char(*data), qboolean *hasNewLines )
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;
	int c;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	while ( ( c = *data ) <= ' ' )
	{
		if ( !c )
		{
			return NULL;
		}
		if ( c == '\n' )
		{
			pi->lines++;
			*hasNewLines = qtrue;
		}
		data++;
	}

	return data;
}

/*
===================
Com_BeginParseSession
===================
*/
void Com_BeginParseSession( const char *filename )
{
	parseInfo_t *pi;
	ParseThreadInfo *parse;
	int i;

	parse = Com_GetParseThreadInfo();

	if ( parse->parseInfoNum == MAX_PARSE_INFO - 1 )
	{
		Com_Printf("Already parsing:\n");

		for ( i = 0; i < parse->parseInfoNum; i++ )
			Com_Printf("%i. %s\n", i, parse->parseInfo[i].parseFile);

		Com_Error( ERR_FATAL, "Com_BeginParseSession: session overflow trying to parse %s", filename );
	}

	pi = &parse->parseInfo[++parse->parseInfoNum];

	Com_InitParseInfo(pi);
	Q_strncpyz( pi->parseFile, filename, sizeof( pi->parseFile ) );
}

/*
===================
Com_EndParseSession
===================
*/
void Com_EndParseSession( void )
{
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();
	if ( parse->parseInfoNum == 0 )
	{
		Com_Error( ERR_FATAL, "Com_EndParseSession: session underflow" );
	}
	--parse->parseInfoNum;
}

/*
===================
Com_UngetToken

Calling this will make the next Com_Parse return
the current token instead of advancing the pointer
===================
*/
void Com_UngetToken()
{
	parseInfo_t *pi;
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	if ( pi->ungetToken )
		Com_ScriptError("UngetToken called twice");

	pi->ungetToken = qtrue;
	parse->tokenPos = parse->prevTokenPos;
}

/*
===================
Com_GetCurrentParseLine
===================
*/
int Com_GetCurrentParseLine( void )
{
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();
	return parse->parseInfo[parse->parseInfoNum].lines;
}

/*
===================
Com_ParseCSV
===================
*/
static char *Com_ParseCSV( const char *( *data_p ), qboolean allowLineBreaks )
{
	parseInfo_t *pi;
	ParseThreadInfo *parse;
	const char *data;
	unsigned int len;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	data = *data_p;
	len = 0;
	pi->token[0] = 0;

	if ( allowLineBreaks )
	{
		while ( *data == '\r' || *data == '\n' )
		{
			data++;
		}
	}
	else
	{
		if ( *data == '\r' || *data == '\n' )
		{
			return pi->token;
		}
	}

	parse->prevTokenPos = parse->tokenPos;
	parse->tokenPos = data;

	while ( *data && *data != ',' && *data != '\n' )
	{
		if ( *data == '\r' )
		{
			data++;
		}
		else if ( *data == '\"' )
		{
			for ( data = data + 1; ; data += 2 )
			{
				while ( *data != '\"' )
				{
					if ( len < MAX_TOKEN_CHARS - 1 )
					{
						pi->token[len] = *data;
						len++;
					}

					data++;
				}

				if ( data[1] != '\"' )
				{
					break;
				}

				if ( len < MAX_TOKEN_CHARS - 1 )
				{
					pi->token[len] = '\"';
					len++;
				}
			}

			data++;
		}
		else
		{
			if ( len < MAX_TOKEN_CHARS - 1 )
			{
				pi->token[len] = *data;
				len++;
			}

			data++;
		}
	}

	if ( *data )
	{
		if ( *data != '\n' )
		{
			data++;
		}

		*data_p = data;
	}
	else
	{
		*data_p = 0;
	}

	pi->token[len] = 0;
	return pi->token;
}

/*
==============
Com_ParseExt

Parse a token out of a string
Will never return NULL, just empty strings.
An empty string will only be returned at end of file.

If "allowLineBreaks" is qtrue then an empty
string will be returned if the next token is
a newline.
==============
*/
static char *Com_ParseExt( const char *( *data_p ), qboolean allowLineBreaks )
{
	parseInfo_t *pi;
	ParseThreadInfo *parse;
	char c = 0;
	int len;
	qboolean hasNewLines = qfalse;
	const char *data;
	const char **punc;
	int l, j;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	data = *data_p;
	len = 0;
	pi->token[0] = 0;

	// make sure incoming data is valid
	if ( !data )
	{
		*data_p = NULL;
		return pi->token;
	}

	pi->backup_lines = pi->lines;
	pi->backup_text = *data_p;

	if ( pi->csv )
	{
		return Com_ParseCSV( data_p, allowLineBreaks );
	}

	// skip any leading whitespace
	while ( 1 )
	{
		// skip whitespace
		data = SkipWhitespace( data, &hasNewLines );
		if ( !data )
		{
			*data_p = NULL;
			return pi->token;
		}
		if ( hasNewLines && !allowLineBreaks )
		{
			//*data_p = data;
			return pi->token;
		}

		c = *data;

		// skip double slash comments
		if ( c == '/' && data[1] == '/' )
		{
			while ( *data && *data != '\n' )
			{
				data++;
			}
			continue;
		}

		// skip /* */ comments
		if ( c == '/' && data[1] == '*' )
		{
			while ( *data && ( *data != '*' || data[1] != '/' ) )
			{
				if ( *data == '\n' )
				{
					pi->lines++;
				}
				data++;
			}
			if ( *data )
			{
				data += 2;
			}
			continue;
		}

		// a real token to parse
		break;
	}

	parse->prevTokenPos = parse->tokenPos;
	parse->tokenPos = data;

	// handle quoted strings
	if ( c == '\"' )
	{
		if ( pi->keepStringQuotes )
		{
			pi->token[len] = '\"';
			len++;
		}

		data = data + 1;
		while ( 1 )
		{
			c = *data++;
			if ( ( c == '\\' ) && ( *data == '\"' || *data == '\\' ) )
			{
				// allow quoted strings to use \" to indicate the " character
				c = *data++;
			}
			else if ( c == '\"' || !c )
			{
				if ( pi->keepStringQuotes )
				{
					pi->token[len] = '\"';
					len++;
				}
				pi->token[len] = 0;
				*data_p = ( char * ) data;
				return pi->token;
			}
			else if ( *data == '\n' )
			{
				pi->lines++;
			}
			if ( len < MAX_TOKEN_CHARS - 1 )
			{
				pi->token[len] = c;
				len++;
			}
		}
	}

	if ( pi->spaceDelimited )
	{
		do
		{
			if ( len < MAX_TOKEN_CHARS - 1 )
			{
				pi->token[len] = c;
				len++;
			}

			c = *++data;
		}
		while ( c > ' ' );

		if ( len == MAX_TOKEN_CHARS )
		{
			len = 0;
		}

		pi->token[len] = 0;
		*data_p = ( char * ) data;
		return pi->token;
	}

	// check for a number
	// is this parsing of negative numbers going to cause expression problems
	if ( ( c >= '0' && c <= '9' ) || pi->negativeNumbers && ( c == '-' && data[ 1 ] >= '0' && data[ 1 ] <= '9' ) ||
	        ( c == '.' && data[ 1 ] >= '0' && data[ 1 ] <= '9' ) )
	{
		do
		{

			if ( len < MAX_TOKEN_CHARS - 1 )
			{
				pi->token[len] = c;
				len++;
			}
			data++;

			c = *data;
		}
		while ( ( c >= '0' && c <= '9' ) || c == '.' );

		// parse the exponent
		if ( c == 'e' || c == 'E' )
		{
			if ( len < MAX_TOKEN_CHARS - 1 )
			{
				pi->token[len] = c;
				len++;
			}
			data++;
			c = *data;

			if ( c == '-' || c == '+' )
			{
				if ( len < MAX_TOKEN_CHARS - 1 )
				{
					pi->token[len] = c;
					len++;
				}
				data++;
				c = *data;
			}

			do
			{
				if ( len < MAX_TOKEN_CHARS - 1 )
				{
					pi->token[len] = c;
					len++;
				}
				data++;

				c = *data;
			}
			while ( c >= '0' && c <= '9' );
		}

		if ( len == MAX_TOKEN_CHARS )
		{
			len = 0;
		}
		pi->token[len] = 0;

		*data_p = ( char * ) data;
		return pi->token;
	}

	// check for a regular word
	// we still allow forward and back slashes in name tokens for pathnames
	// and also colons for drive letters
	if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || c == '_' || c == '/' || c == '\\' )
	{
		do
		{
			if ( len < MAX_TOKEN_CHARS - 1 )
			{
				pi->token[len] = c;
				len++;
			}
			data++;

			c = *data;
		}
		while ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || c == '_' || ( c >= '0' && c <= '9' ) );

		if ( len == MAX_TOKEN_CHARS )
		{
			len = 0;
		}
		pi->token[len] = 0;

		*data_p = ( char * ) data;
		return pi->token;
	}

	// check for multi-character punctuation token
	for ( punc = punctuation ; ; punc++ )
	{
		if ( !*punc )
		{
			// single character punctuation
			pi->token[0] = *data;
			pi->token[1] = 0;
			*data_p = data + 1;

			return pi->token;
		}

		l = I_strlen( *punc );
		for ( j = 0 ; j < l ; j++ )
		{
			if ( data[j] != ( *punc )[j] )
			{
				break;
			}
		}
		if ( j == l )
		{
			break;
		}
	}

	// a valid multi-character punctuation
	memcpy(pi, *punc, l);
	pi->token[l] = 0;
	*data_p = &data[l];

	return pi->token;
}

/*
===================
Com_Parse
===================
*/
char *Com_Parse( const char *( *data_p ) )
{
	parseInfo_t *pi;
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	if ( pi->ungetToken )
	{
		pi->ungetToken = qfalse;
		*data_p = pi->backup_text;
		pi->lines = pi->backup_lines;
	}

	return Com_ParseExt(data_p, qtrue);
}

/*
===================
Com_ParseOnLine
===================
*/
char *Com_ParseOnLine( const char *( *data_p ) )
{
	parseInfo_t *pi;
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	if ( !pi->ungetToken )
	{
		return Com_ParseExt(data_p, qfalse);
	}

	pi->ungetToken = qfalse;

	if ( !pi->spaceDelimited )
	{
		return pi->token;
	}

	*data_p = pi->backup_text;
	pi->lines = pi->backup_lines;

	return Com_ParseExt(data_p, qfalse);
}

/*
===================
Com_SetCSV
===================
*/
void Com_SetCSV( qboolean csv )
{
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();
	parse->parseInfo[parse->parseInfoNum].csv = csv != qfalse;
}