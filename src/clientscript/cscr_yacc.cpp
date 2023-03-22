#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

typedef struct
{
	FILE *yy_input_file;
	char* yy_ch_buf;
	char* yy_buf_pos;
	int yy_buf_size;
	int yy_n_chars;
	int yy_is_our_buffer;
	int yy_is_interactive;
	int yy_at_bol;
	int yy_fill_buffer;
	int yy_buffer_status;
} yy_buffer_state;

extern unsigned int g_out_pos;
extern unsigned int g_sourcePos;
extern unsigned char g_parse_user;
extern sval_u g_dummyVal;
extern int yy_init;
extern yy_buffer_state* yy_current_buffer;
extern int yy_start;
extern int yy_n_chars;
extern char* yy_c_buf_p;
extern char *yytext;
extern FILE* yyin;
extern char yy_hold_char;
extern char ch_buf[];
extern sval_u yaccResult;

#ifdef __cplusplus
extern "C" {
#endif

FILE* QDECL yy_stdin()
{
	return stdin;
}

FILE* QDECL yy_stdout()
{
	return stdout;
}

yy_buffer_state* QDECL yy_create_buffer(FILE *file, int bufferSize)
{
	yy_buffer_state *yybufState;

	yybufState = (yy_buffer_state *)malloc(sizeof(yy_buffer_state));

	if ( yybufState == NULL)
	{
		Com_Error(ERR_FATAL,"out of dynamic memory in yy_create_buffer()");
		return NULL;
	}

	yybufState->yy_buf_size = bufferSize;
	yybufState->yy_ch_buf = (char *)malloc(bufferSize + 2);

	if (yybufState->yy_ch_buf == NULL)
	{
		Com_Error(ERR_FATAL,"out of dynamic memory in yy_create_buffer()");
		return NULL;
	}

	yybufState->yy_is_our_buffer = 1;
	yybufState->yy_n_chars = 0;
	yybufState->yy_ch_buf[0] = 0;
	yybufState->yy_ch_buf[1] = 0;
	yybufState->yy_buf_pos = yybufState->yy_ch_buf;
	yybufState->yy_at_bol = 1;
	yybufState->yy_buffer_status = 0;
	yybufState->yy_input_file = file;
	yybufState->yy_fill_buffer = 1;
	yybufState->yy_is_interactive = 0;

	return yybufState;
}

void QDECL yy_load_buffer_state()
{
	yy_n_chars = yy_current_buffer->yy_n_chars;
	yy_c_buf_p = yy_current_buffer->yy_buf_pos;
	yytext = yy_c_buf_p;
	yyin = yy_current_buffer->yy_input_file;
	yy_hold_char = *yy_c_buf_p;
}

void QDECL yy_flush_buffer(yy_buffer_state *b)
{
	if ( b )
	{
		b->yy_n_chars = 0;
		*b->yy_ch_buf = 0;
		b->yy_ch_buf[1] = 0;
		b->yy_buf_pos = b->yy_ch_buf;
		b->yy_at_bol = 1;
		b->yy_buffer_status = 0;

		if ( b == yy_current_buffer )
			yy_load_buffer_state();
	}
}

void QDECL yy_init_buffer(yy_buffer_state *b, FILE *file)
{
	yy_flush_buffer(b);
	b->yy_input_file = file;
	b->yy_fill_buffer = 1;
	b->yy_is_interactive = 0;
}

void QDECL ScriptParse(sval_u *parseData, unsigned char user)
{
	yy_buffer_state buffer_state;

	g_out_pos = -1;
	g_sourcePos = 0;
	g_parse_user = user;
	g_dummyVal.stringValue = 0;
	yy_init = 1;
	buffer_state.yy_buf_size = 0x4000;
	buffer_state.yy_ch_buf = ch_buf;
	buffer_state.yy_is_our_buffer = 0;
	yy_init_buffer(&buffer_state, 0);
	yy_current_buffer = &buffer_state;
	yy_start = 3;
	yyparse();
	*parseData = yaccResult;
}

#ifdef __cplusplus
}
#endif