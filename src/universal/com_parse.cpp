#include "../qcommon/qcommon.h"
#include "../qcommon/sys_thread.h"

static ParseThreadInfo g_parse[NUMTHREADS];

const char* punctuation[17] =
{
	"+=",
	"-=",
	"*=",
	"/=",
	"&=",
	"|=",
	"++",
	"--",
	"&",
	"||",
	"<=",
	">=",
	"==",
	"!=",
	"<<",
	">>",
	NULL
};

/*
==============
Com_GetParseThreadInfo
==============
*/
ParseThreadInfo *Com_GetParseThreadInfo()
{
	if (Sys_IsMainThread())
		return &g_parse[0];

	return NULL;
}

/*
==============
Com_InitParse
==============
*/
void Com_InitParse()
{
	for (int i = 0; i < NUMTHREADS; ++i)
	{
		Com_InitParseInfo(g_parse[i].parseInfo);
	}
}

/*
==============
Com_InitParse
==============
*/
void Com_InitParseInfo(parseInfo_t* pi)
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
	pi->backup_text = NULL;
}

/*
==============
Com_BeginParseSession
==============
*/
void Com_BeginParseSession(const char *filename)
{
	parseInfo_t* pi;
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	if (parse->parseInfoNum == 15)
	{
		Com_Printf("Already parsing:\n");
		for (int i = 0; i < parse->parseInfoNum; ++i)
		{
			Com_Printf("%i. %s\n", i, parse->parseInfo[i].parseFile);
		}
	}

	pi = &parse->parseInfo[++parse->parseInfoNum];
	Com_InitParseInfo(pi);
	pi->parseFile = filename;
}

/*
==============
Com_EndParseSession
==============
*/
void Com_EndParseSession()
{
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	if (!parse->parseInfoNum)
	{
		Com_Error(ERR_FATAL, "Com_EndParseSession: session underflow");
	}
	--parse->parseInfoNum;
}

/*
==============
Com_ResetParseSessions
==============
*/
void Com_ResetParseSessions()
{
	Com_GetParseThreadInfo()->parseInfoNum = 0;
}

/*
==============
Com_SetSpaceDelimited
==============
*/
void Com_SetSpaceDelimited(int spaceDelimited)
{
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	parse->parseInfo[parse->parseInfoNum].spaceDelimited = spaceDelimited != 0;
}

/*
==============
Com_SetKeepStringQuotes
==============
*/
void Com_SetKeepStringQuotes(int keepStringQuotes)
{
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	parse->parseInfo[parse->parseInfoNum].keepStringQuotes = keepStringQuotes != 0;
}

/*
==============
Com_SetCSV
==============
*/
void Com_SetCSV(int csv)
{
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	parse->parseInfo[parse->parseInfoNum].csv = csv != 0;
}

/*
==============
Com_SetParseNegativeNumbers
==============
*/
void Com_SetParseNegativeNumbers(int negativeNumbers)
{
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	parse->parseInfo[parse->parseInfoNum].negativeNumbers = negativeNumbers != 0;
}

/*
==============
Com_GetCurrentParseLine
==============
*/
int Com_GetCurrentParseLine()
{
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	return parse->parseInfo[parse->parseInfoNum].lines;
}

/*
==============
Com_SetScriptErrorPrefix
==============
*/
void Com_SetScriptErrorPrefix(const char *prefix)
{
	parseInfo_t* pi;
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	pi->errorPrefix = prefix;
}

/*
==============
Com_GetScriptErrorPrefix
==============
*/
const char *Com_GetScriptErrorPrefix()
{
	parseInfo_t* pi;
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	return pi->errorPrefix;
}

/*
==============
Com_SetScriptWarningPrefix
==============
*/
void Com_SetScriptWarningPrefix(const char *prefix)
{
	parseInfo_t* pi;
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	pi->warningPrefix = prefix;
}

/*
==============
Com_GetScriptWarningPrefix
==============
*/
const char *Com_GetScriptWarningPrefix()
{
	parseInfo_t* pi;
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	return pi->warningPrefix;
}

/*
==============
Com_ScriptErrorDrop
==============
*/
void Com_ScriptErrorDrop(const char *msg, ...)
{
	char string[4096];
	char* argptr;
	parseInfo_t* pi;
	ParseThreadInfo* parse;
	va_list va;

	va_start(va, msg);
	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	va_copy(argptr, va);
	Q_vsnprintf(string, 0x1000, msg, va);
	argptr = NULL;

	if (parse->parseInfoNum)
	{
		Com_Error(ERR_DROP, "%sFile %s, line %i: %s", pi->errorPrefix, pi->parseFile, pi->lines, string);
	}
	else
	{
		Com_Error(ERR_DROP, "%s", string);
	}
}

/*
==============
Com_ScriptError
==============
*/
void Com_ScriptError(const char *msg, ...)
{
	char string[4096];
	char* argptr;
	parseInfo_t* pi;
	ParseThreadInfo* parse;
	va_list va;

	va_start(va, msg);
	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	va_copy(argptr, va);
	Q_vsnprintf(string, 0x1000u, msg, va);
	argptr = NULL;
	if (parse->parseInfoNum)
	{
		Com_Printf("%sFile %s, line %i: %s", pi->warningPrefix, pi->parseFile, pi->lines, string);
	}
	else
	{
		Com_Printf("%s", string);
	}
}

/*
==============
Com_ScriptWarning
==============
*/
void Com_ScriptWarning(const char *msg, ...)
{
	char string[4096];
	char* argptr;
	parseInfo_t* pi;
	ParseThreadInfo* parse;
	va_list va;

	va_start(va, msg);
	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	va_copy(argptr, va);
	Q_vsnprintf(string, 0x1000u, msg, va);
	argptr = NULL;

	if (parse->parseInfoNum)
	{
		Com_Printf("%sFile %s, line %i: %s", pi->warningPrefix, pi->parseFile, pi->lines, string);
	}
	else
	{
		Com_Printf("%s", string);
	}
}

/*
==============
Com_UngetToken
==============
*/
void Com_UngetToken()
{
	parseInfo_t* pi;
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	if (pi->ungetToken)
	{
		Com_ScriptErrorDrop("UngetToken called twice");
	}
	pi->ungetToken = 1;
	parse->tokenPos = parse->prevTokenPos;
}

/*
==============
Com_ParseSetMark
==============
*/
void Com_ParseSetMark(const char **text, com_parse_mark_t *mark)
{
	ParseThreadInfo* parse;
	parseInfo_t* pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	mark->lines = pi->lines;
	mark->text = *text;
	mark->ungetToken = pi->ungetToken;
	mark->backup_lines = pi->backup_lines;
	mark->backup_text = pi->backup_text;
}

/*
==============
Com_ParseReturnToMark
==============
*/
void Com_ParseReturnToMark(const char **text, com_parse_mark_t *mark)
{
	ParseThreadInfo* parse;
	parseInfo_t* pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	pi->lines = mark->lines;
	*text = mark->text;
	pi->ungetToken = mark->ungetToken != 0;
	pi->backup_lines = mark->backup_lines;
	pi->backup_text = mark->backup_text;
}

/*
==============
Com_Compress
==============
*/
int Com_Compress(char *data_p)
{
	char c;
	char* datai;
	int size;
	char* datao;

	size = 0;
	datao = data_p;
	datai = data_p;
	if (data_p)
	{
		while (1)
		{
			c = *datai;
			if (!*datai)
			{
				break;
			}
			if (c != 13 && c != 10)
			{
				if (c != 47 || datai[1] != 47)
				{
					if (c != 47 || datai[1] != 42)
					{
						*datao++ = c;
						++size;
						++datai;
					}
					else
					{
						while (*datai && (*datai != 42 || datai[1] != 47))
						{
							if (*datai == 10)
							{
								*datao++ = 10;
								++size;
							}
							++datai;
						}
						if (*datai)
						{
							datai += 2;
						}
					}
				}
				else
				{
					while (*datai && *datai != 10)
					{
						++datai;
					}
				}
			}
			else
			{
				*datao++ = c;
				++size;
				++datai;
			}
		}
	}

	*datao = 0;
	return size;
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
SkipWhitespace
==============
*/
const char* SkipWhitespace(const char* data, int* newLines)
{
	int c;

	*newLines = 0;
	while (1)
	{
		c = *data;
		if (c > 32)
		{
			break;
		}
		if (!*data)
		{
			return 0;
		}
		if (c == 10)
		{
			++* newLines;
		}
		++data;
	}
	return data;
}

/*
==============
Com_ParseCSV
==============
*/
char *Com_ParseCSV(const char **data_p, int allowLineBreaks)
{
	const char* data;
	unsigned int len;
	parseInfo_t* pi;
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	data = *data_p;
	len = 0;
	pi->token[0] = 0;
	if (allowLineBreaks)
	{
		while (*data == 13 || *data == 10)
		{
			++data;
		}
	}
	else if (*data == 13 || *data == 10)
	{
		return pi->token;
	}

	parse->prevTokenPos = parse->tokenPos;
	parse->tokenPos = data;
	while (*data && *data != 44 && *data != 10)
	{
		if (*data == 13)
		{
			++data;
		}
		else if (*data == 34)
		{
			for (data = data + 1; ; data += 2)
			{
				while (*data != 34)
				{
					if (len < 0x3FF)
					{
						pi->token[len++] = *data;
					}
					++data;
				}
				if (data[1] != 34)
				{
					break;
				}
				if (len < 0x3FF)
				{
					pi->token[len++] = 34;
				}
			}
			data = data + 1;
		}
		else
		{
			if (len < 0x3FF)
			{
				pi->token[len++] = *data;
			}
			++data;
		}
	}

	if (*data && *data != 10)
	{
		++data;
	}

	if (*data || len)
	{
		*data_p = data;
	}
	else
	{
		*data_p = NULL;
	}

	pi->token[len] = 0;

	return pi->token;
}

/*
==============
Com_ParseExt
==============
*/
char *Com_ParseExt(const char **data_p, int allowLineBreaks)
{
	parseInfo_t* result;
	int j;
	int l;
	char c;
	int newLines;
	const char* data;
	const char** punc;
	int len;
	parseInfo_t* pi;
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	data = *data_p;
	len = 0;
	pi->token[0] = 0;
	pi->tokenType = PARSE_TOKEN_UNKNOWN;
	if (!data)
	{
		*data_p = NULL;
		return pi->token;
	}

	pi->backup_lines = pi->lines;
	pi->backup_text = *data_p;
	if (pi->csv)
	{
		return Com_ParseCSV(data_p, allowLineBreaks);
	}

	while (1)
	{
		while (1)
		{
			data = SkipWhitespace(data, &newLines);
			if (!data)
			{
				*data_p = NULL;
				return pi->token;
			}

			if (newLines > 0 && !allowLineBreaks)
			{
				return pi->token;
			}

			pi->lines += newLines;
			c = *data;
			if (*data != 47 || data[1] != 47)
			{
				break;
			}
			while (*data && *data != 10)
			{
				++data;
			}
		}

		if (c != 47 || data[1] != 42)
		{
			break;
		}

		while (*data && (*data != 42 || data[1] != 47))
		{
			if (*data == 10)
			{
				++pi->lines;
			}
			++data;
		}

		if (*data)
		{
			data += 2;
		}
	}

	parse->prevTokenPos = parse->tokenPos;
	parse->tokenPos = data;
	if (c == 34)
	{
		if (pi->keepStringQuotes)
		{
			pi->token[len++] = 34;
		}
		++data;
		while (1)
		{
			c = *data++;
			if (c != 92 || *data != 34 && *data != 92)
			{
				if (c == 34 || !c)
				{
					if (pi->keepStringQuotes)
					{
						pi->token[len++] = 34;
					}
					pi->token[len] = 0;
					*data_p = data;
					pi->tokenType = PARSE_TOKEN_STRING;
					return pi->token;
				}

				if (*data == 10)
				{
					++pi->lines;
				}
			}
			else
			{
				c = *data++;
			}
			if (len < 1023)
			{
				pi->token[len++] = c;
			}
		}
	}

	if (c == -1)
	{
		++data;
		while (1)
		{
			c = *data++;
			if (c == -1 || !c)
			{
				break;
			}
			if (len < 1023)
			{
				pi->token[len++] = c;
			}
		}

		pi->token[len] = 0;
		*data_p = data;
		pi->tokenType = PARSE_TOKEN_HASH;
		result = pi;
	}
	else if (pi->spaceDelimited)
	{
		do
		{
			if (len < 1023)
			{
				pi->token[len++] = c;
			}
			c = *++data;
		}
		while (*data > 32);

		if (len == 1024)
		{
			len = 0;
		}
		pi->token[len] = 0;
		*data_p = data;
		result = pi;
	}
	else if (c >= 48 && c <= 57
	         || pi->negativeNumbers && c == 45 && data[1] >= 48 && data[1] <= 57
	         || c == 46 && data[1] >= 48 && data[1] <= 57)
	{
		do
		{
			if (len < 1023)
			{
				pi->token[len++] = c;
			}
			c = *++data;
		}
		while (*data >= 48 && c <= 57 || c == 46);

		if (c == 101 || c == 69)
		{
			if (len < 1023)
			{
				pi->token[len++] = c;
			}
			c = *++data;
			if (*data == 45 || c == 43)
			{
				if (len < 1023)
				{
					pi->token[len++] = c;
				}
				c = *++data;
			}

			do
			{
				if (len < 1023)
				{
					pi->token[len++] = c;
				}
				c = *++data;
			}
			while (*data >= 48 && c <= 57);
		}
		if (len == 1024)
		{
			len = 0;
		}

		pi->token[len] = 0;
		pi->tokenType = PARSE_TOKEN_NUMBER;
		*data_p = data;
		result = pi;
	}
	else if ((c < 97 || c > 122) && (c < 65 || c > 90) && c != 95 && c != 47 && c != 92)
	{
		pi->tokenType = PARSE_TOKEN_PUNCTUATION;
		for (punc = punctuation; *punc; ++punc)
		{
			*punc;
			l = strlen(*punc);
			for (j = 0; j < l && data[j] == (*punc)[j]; ++j)
			{
				;
			}
			if (j == l)
			{
				memcpy(pi->token, (char*)*punc, l);
				pi->token[l] = 0;
				data += l;
				*data_p = data;
				return pi->token;
			}
		}
		pi->token[0] = *data;
		pi->token[1] = 0;
		*data_p = ++data;
		result = pi;
	}
	else
	{
		do
		{
			if (len < 1023)
			{
				pi->token[len++] = c;
			}
			c = *++data;
		}
		while (*data >= 97 && c <= 122 || c >= 65 && c <= 90 || c == 95 || c >= 48 && c <= 57);

		if (len == 1024)
		{
			len = 0;
		}

		pi->token[len] = 0;
		pi->tokenType = PARSE_TOKEN_NAME;
		*data_p = data;
		result = pi;
	}

	return result->token;
}

/*
==============
Com_Parse
==============
*/
char *Com_Parse(const char **data_p)
{
	ParseThreadInfo* parse;
	parseInfo_t* pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	if (pi->ungetToken)
	{
		pi->ungetToken = 0;
		*data_p = pi->backup_text;
		pi->lines = pi->backup_lines;
	}
	return Com_ParseExt(data_p, 1);
}

/*
==============
Com_ParseOnLine
==============
*/
char *Com_ParseOnLine(const char **data_p)
{
	ParseThreadInfo* parse;
	parseInfo_t* pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	if (pi->ungetToken)
	{
		pi->ungetToken = 0;
		if (!pi->spaceDelimited)
		{
			return pi->token;
		}
		*data_p = pi->backup_text;
		pi->lines = pi->backup_lines;
	}
	return Com_ParseExt(data_p, 0);
}

/*
==============
Com_GetTokenType
==============
*/
ParseTokenType Com_GetTokenType()
{
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();
	return parse->parseInfo[parse->parseInfoNum].tokenType;
}

/*
==============
Com_MatchToken
==============
*/
int Com_MatchToken(const char **buf_p, const char *match, int warning)
{
	const char* token;

	token = Com_Parse(buf_p);
	if (!strcmp(token, match))
	{
		return 1;
	}
	if (warning)
	{
		Com_ScriptError("MatchToken: got '%s', expected '%s'\n", token, match);
	}
	else
	{
		Com_ScriptErrorDrop("MatchToken: got '%s', expected '%s'\n", token, match);
	}
	return 0;
}

/*
==============
Com_SkipBracedSection
==============
*/
int Com_SkipBracedSection(const char **program, unsigned int startDepth, const int iMaxNesting)
{
	int depth;
	int bNestingExceeded;
	const char* token;

	bNestingExceeded = 0;
	depth = startDepth;
	do
	{
		token = Com_Parse(program);
		if (!token[1])
		{
			if (*token == 123)
			{
				if (depth == iMaxNesting)
				{
					bNestingExceeded = 1;
				}
				else
				{
					++depth;
				}
			}
			else if (*token == '}')
			{
				--depth;
			}
		}
	}
	while (depth && *program);
	return bNestingExceeded;
}

/*
==============
Com_SkipRestOfLine
==============
*/
void Com_SkipRestOfLine(const char **data)
{
	const char* p;
	int c;
	ParseThreadInfo* parse;

	parse = Com_GetParseThreadInfo();

	p = *data;
	while ((c = *p++) != 0)
	{
		if (c == '\n')
		{
			++parse->parseInfo[parse->parseInfoNum].lines;
			break;
		}
	}

	*data = p;
}

/*
==============
Com_GetArgCountOnLine
==============
*/
int Com_GetArgCountOnLine(const char **data_p)
{
	com_parse_mark_t mark;
	const char* token;
	int count;

	Com_ParseSetMark(data_p, &mark);
	for (count = 0; ; ++count)
	{
		token = Com_ParseOnLine(data_p);
		if (!*token)
		{
			break;
		}
	}

	Com_ParseReturnToMark(data_p, &mark);
	return count;
}

/*
==============
Com_ParseRestOfLine
==============
*/
char *Com_ParseRestOfLine(const char **data_p)
{
	const char* token;
	char* line;

	line = Com_GetParseThreadInfo()->line;
	*line = 0;
	while (1)
	{
		token = Com_ParseOnLine(data_p);
		if (!token[0])
		{
			break;
		}

		if (*line)
		{
			I_strncat(line, 1024, " ");
		}
		I_strncat(line, 1024, token);
	}

	return line;
}

/*
==============
Com_ParseFloat
==============
*/
float Com_ParseFloat(const char **buf_p)
{
	const char* token;

	token = Com_Parse(buf_p);
	return atof(token);
}

/*
==============
Com_ParseFloatOnLine
==============
*/
float Com_ParseFloatOnLine(const char **buf_p)
{
	const char* token;

	token = Com_ParseOnLine(buf_p);
	return atof(token);
}

/*
==============
Com_ParseInt
==============
*/
int Com_ParseInt(const char **buf_p)
{
	const char* token;

	token = Com_Parse(buf_p);
	return atoi(token);
}

/*
==============
Com_ParseIntOnLine
==============
*/
int Com_ParseIntOnLine(const char **buf_p)
{
	const char* token;

	token = Com_ParseOnLine(buf_p);
	return atoi(token);
}

/*
==============
Com_Parse1DMatrix
==============
*/
void Com_Parse1DMatrix(const char **buf_p, int x, float *m)
{
	const char* token;
	int i;

	Com_MatchToken(buf_p, "(", 0);
	for (i = 0; i < x; ++i)
	{
		token = Com_Parse(buf_p);
		m[i] = atof(token);
	}
	Com_MatchToken(buf_p, ")", 0);
}

