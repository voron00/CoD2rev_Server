#pragma once

enum ParseTokenType
{
	PARSE_TOKEN_UNKNOWN = 0x0,
	PARSE_TOKEN_NUMBER = 0x1,
	PARSE_TOKEN_STRING = 0x2,
	PARSE_TOKEN_NAME = 0x3,
	PARSE_TOKEN_HASH = 0x4,
	PARSE_TOKEN_PUNCTUATION = 0x5,
};

struct com_parse_mark_t
{
	int lines;
	const char *text;
	int ungetToken;
	int backup_lines;
	const char *backup_text;
};

struct parseInfo_t
{
	char token[1024];
	ParseTokenType tokenType;
	int lines;
	bool ungetToken;
	bool spaceDelimited;
	bool keepStringQuotes;
	bool csv;
	bool negativeNumbers;
	bool numbers;
	const char *extraWordCharacters;
	const char *errorPrefix;
	const char *warningPrefix;
	int backup_lines;
	const char *backup_text;
	const char *parseFile;
};

struct ParseThreadInfo
{
	parseInfo_t parseInfo[16];
	int parseInfoNum;
	const char *tokenPos;
	const char *prevTokenPos;
	char line[1024];
};

ParseThreadInfo *Com_GetParseThreadInfo();
void Com_InitParse();
void Com_InitParseInfo(parseInfo_t* pi);
void Com_BeginParseSession(const char *filename);
void Com_EndParseSession();
void Com_ResetParseSessions();
void Com_SetSpaceDelimited(int spaceDelimited);
void Com_SetKeepStringQuotes(int keepStringQuotes);
void Com_SetCSV(int csv);
void Com_SetParseNegativeNumbers(int negativeNumbers);
int Com_GetCurrentParseLine();
void Com_SetScriptErrorPrefix(const char *prefix);
const char *Com_GetScriptErrorPrefix();
void Com_SetScriptWarningPrefix(const char *prefix);
const char *Com_GetScriptWarningPrefix();
void Com_ScriptErrorDrop(const char *msg, ...);
void Com_ScriptError(const char *msg, ...);
void Com_ScriptWarning(const char *msg, ...);
void Com_UngetToken();
void Com_ParseSetMark(const char **text, com_parse_mark_t *mark);
void Com_ParseReturnToMark(const char **text, com_parse_mark_t *mark);
int Com_Compress(char *data_p);
const char *Com_GetLastTokenPos();
const char* SkipWhitespace(const char* data, int* newLines);
char *Com_ParseCSV(const char **data_p, int allowLineBreaks);
char *Com_ParseExt(const char **data_p, int allowLineBreaks);
char *Com_Parse(const char **data_p);
char *Com_ParseOnLine(const char **data_p);
ParseTokenType Com_GetTokenType();
int Com_MatchToken(const char **buf_p, const char *match, int warning);
int Com_SkipBracedSection(const char **program, unsigned int startDepth, const int iMaxNesting);
void Com_SkipRestOfLine(const char **data);
int Com_GetArgCountOnLine(const char **data_p);
char *Com_ParseRestOfLine(const char **data_p);
float Com_ParseFloat(const char **buf_p);
float Com_ParseFloatOnLine(const char **buf_p);
int Com_ParseInt(const char **buf_p);
int Com_ParseIntOnLine(const char **buf_p);