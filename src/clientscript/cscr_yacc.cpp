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

extern const short yy_base[];
extern const short yy_chk[];
extern const short yy_def[];
extern const short yy_nxt[];
extern const short yy_accept[];
extern const int yy_meta[];
extern const int yy_ec[];
extern const short yytable[];
extern const short yycheck[];
extern const short yypact[];

extern int yy_last_accepting_state;
extern char *yy_last_accepting_cpos;
extern int yy_did_buffer_switch_on_eof;
extern stype_t yylval;
extern unsigned int yyleng;
extern int yychar;

extern FILE *yyout;

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

sval_u* QDECL node0(unsigned char type)
{
	sval_u *result;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(4);
	result->type = type;

	return result;
}

sval_u* QDECL node1(unsigned char type, sval_u val1)
{
	sval_u *result;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(8);
	result->type = type;

	result[1].node = val1.node;

	return result;
}

sval_u* QDECL node1_(sval_u val1)
{
	return val1.node;
}

sval_u* QDECL node2(unsigned char type, sval_u val1, sval_u val2)
{
	sval_u *result;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(12);
	result->type = type;

	result[1].node = val1.node;
	result[2].node = val2.node;

	return result;
}

sval_u* QDECL node2_(sval_u val1, sval_u val2)
{
	sval_u *result;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(8);

	result[0].node = val1.node;
	result[1].node = val2.node;

	return result;
}

sval_u* QDECL node3(unsigned char type, sval_u val1, sval_u val2, sval_u val3)
{
	sval_u *result;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(16);
	result->type = type;

	result[1].node = val1.node;
	result[2].node = val2.node;
	result[3].node = val3.node;

	return result;
}

sval_u* QDECL node4(unsigned char type, sval_u val1, sval_u val2, sval_u val3, sval_u val4)
{
	sval_u *result;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(20);
	result->type = type;

	result[1].node = val1.node;
	result[2].node = val2.node;
	result[3].node = val3.node;
	result[4].node = val4.node;

	return result;
}

sval_u* QDECL node5(unsigned char type, sval_u val1, sval_u val2, sval_u val3, sval_u val4, sval_u val5)
{
	sval_u *result;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(24);
	result->type = type;

	result[1].node = val1.node;
	result[2].node = val2.node;
	result[3].node = val3.node;
	result[4].node = val4.node;
	result[5].node = val5.node;

	return result;
}

sval_u* QDECL node6(unsigned char type, sval_u val1, sval_u val2, sval_u val3, sval_u val4, sval_u val5, sval_u val6)
{
	sval_u *result;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(28);
	result->type = type;

	result[1].node = val1.node;
	result[2].node = val2.node;
	result[3].node = val3.node;
	result[4].node = val4.node;
	result[5].node = val5.node;
	result[6].node = val6.node;

	return result;
}

sval_u* QDECL node7(unsigned char type, sval_u val1, sval_u val2, sval_u val3, sval_u val4, sval_u val5, sval_u val6, sval_u val7)
{
	sval_u *result;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(32);
	result->type = type;

	result[1].node = val1.node;
	result[2].node = val2.node;
	result[3].node = val3.node;
	result[4].node = val4.node;
	result[5].node = val5.node;
	result[6].node = val6.node;
	result[7].node = val7.node;

	return result;
}

sval_u* QDECL node8(unsigned char type, sval_u val1, sval_u val2, sval_u val3, sval_u val4, sval_u val5, sval_u val6, sval_u val7, sval_u val8)
{
	sval_u *result;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(36);
	result->type = type;

	result[1].node = val1.node;
	result[2].node = val2.node;
	result[3].node = val3.node;
	result[4].node = val4.node;
	result[5].node = val5.node;
	result[6].node = val6.node;
	result[7].node = val7.node;
	result[8].node = val8.node;

	return result;
}

sval_u* QDECL node_pos(sval_u val)
{
	return val.node;
}

sval_u* QDECL append_node(sval_u val1, sval_u val2)
{
	sval_u *result;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(8);

	result[0] = val2;
	result[1].node = 0;

	val1.node[1].node[1].node = result;
	val1.node[1].node = result;

	return val1.node;
}

sval_u* QDECL prepend_node(sval_u val1, sval_u val2)
{
	sval_u *result;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(8);

	result[0] = val1;
	result[1] = *val2.node;

	val2.node->node = result;

	return val2.node;
}

sval_u* QDECL linked_list_end(sval_u val)
{
	sval_u *result;
	sval_u *node;

	node = (sval_u *)Hunk_AllocateTempMemoryHighInternal(8);

	node[0] = val;
	node[1].node = 0;

	result = (sval_u *)Hunk_AllocateTempMemoryHighInternal(8);

	result->node = node;
	result[1].node = node;

	return result;
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

int StringValue(char *str, int len)
{
	char c;
	char string[8192];
	char *pC1;
	char *pC2;
	int n;

	if ( len < 0x2000 )
	{
		pC1 = string;
		while ( len )
		{
			if ( *str == 92 )
			{
				n = len - 1;
				if ( !n )
					break;
				pC2 = str + 1;
				c = *pC2;
				if ( *pC2 == 110 )
				{
					*pC1++ = 10;
				}
				else if ( c == 114 )
				{
					*pC1++ = 13;
				}
				else
				{
					if ( c == 116 )
						*pC1 = 9;
					else
						*pC1 = *pC2;
					++pC1;
				}
				len = n - 1;
				str = pC2 + 1;
			}
			else
			{
				--len;
				*pC1++ = *str++;
			}
		}
		*pC1 = 0;
		yylval.val.stringValue = SL_GetString_(string, g_parse_user);
		return 1;
	}
	else
	{
		CompileError(g_sourcePos, "max string length exceeded: \"%s\"", str);
		return 0;
	}
}

void TextValue(char *str, int len)
{
	yylval.val.stringValue = SL_GetStringOfLen(str, 0, len + 1);
}

void IntegerValue(char *str)
{
	sscanf(str, "%d", &yylval.val.intValue);
}

void FloatValue(char *str)
{
	sscanf(str, "%f", &yylval.val.floatValue);
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

int yyerror(const char *msg)
{
	if ( yychar )
	{
		if ( yychar != 257 )
			CompileError(g_sourcePos, "bad syntax");
	}
	else
	{
		CompileError(g_sourcePos, "unexpected end of file found");
	}

	return 0;
}

void yy_fatal_error(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(2);
}

void *yy_flex_realloc(void *ptr, unsigned int size)
{
	return realloc(ptr, size);
}

void QDECL yy_init_buffer(yy_buffer_state *b, FILE *file)
{
	yy_flush_buffer(b);
	b->yy_input_file = file;
	b->yy_fill_buffer = 1;
	b->yy_is_interactive = 0;
}

void yyrestart(FILE *input_file)
{
	if ( !yy_current_buffer )
		yy_current_buffer = yy_create_buffer(yyin, 0x4000);

	yy_init_buffer(yy_current_buffer, input_file);
	yy_load_buffer_state();
}

int yy_try_NUL_trans(int yy_current_state)
{
	unsigned char yy_c;
	int current_state;

	yy_c = 1;

	if ( yy_accept[yy_current_state] )
	{
		yy_last_accepting_state = yy_current_state;
		yy_last_accepting_cpos = yy_c_buf_p;
	}

	while ( yy_chk[yy_c + yy_base[yy_current_state]] != yy_current_state )
	{
		yy_current_state = yy_def[yy_current_state];

		if ( yy_current_state > 255 )
			yy_c = yy_meta[yy_c];
	}

	current_state = yy_nxt[yy_c + yy_base[yy_current_state]];

	if ( current_state == 255 )
		return 0;
	else
		return current_state;
}

int yy_get_previous_state()
{
	unsigned char yy_c;
	char *yy_cp;
	int yy_current_state;

	yy_current_state = yy_start;

	for ( yy_cp = yytext; yy_cp < yy_c_buf_p; ++yy_cp )
	{
		if ( *yy_cp )
			yy_c = yy_ec[(unsigned char)*yy_cp];
		else
			yy_c = 1;

		if ( yy_accept[yy_current_state] )
		{
			yy_last_accepting_state = yy_current_state;
			yy_last_accepting_cpos = yy_cp;
		}

		while ( yy_chk[yy_c + yy_base[yy_current_state]] != yy_current_state )
		{
			yy_current_state = yy_def[yy_current_state];

			if ( yy_current_state > 255 )
				yy_c = yy_meta[yy_c];
		}

		yy_current_state = yy_nxt[yy_c + yy_base[yy_current_state]];
	}

	return yy_current_state;
}

int yy_get_next_buffer()
{
	int yy_c_buf_p_offset;
	yy_buffer_state *b;
	signed int num_to_read;
	char *source;
	int ret_val;
	char *dest;
	int number_to_move;
	int i;

	dest = yy_current_buffer->yy_ch_buf;
	source = yytext;
	if ( yy_c_buf_p > &dest[yy_n_chars + 1] )
		yy_fatal_error("fatal flex scanner internal error--end of buffer missed");
	if ( yy_current_buffer->yy_fill_buffer )
	{
		number_to_move = yy_c_buf_p - yytext - 1;
		for ( i = 0; i < number_to_move; ++i )
			*dest++ = *source++;
		if ( yy_current_buffer->yy_buffer_status == 2 )
		{
			yy_n_chars = 0;
			yy_current_buffer->yy_n_chars = 0;
		}
		else
		{
			for ( num_to_read = yy_current_buffer->yy_buf_size - number_to_move - 1;
			        num_to_read <= 0;
			        num_to_read = yy_current_buffer->yy_buf_size - number_to_move - 1 )
			{
				b = yy_current_buffer;
				yy_c_buf_p_offset = yy_c_buf_p - yy_current_buffer->yy_ch_buf;
				if ( yy_current_buffer->yy_is_our_buffer )
				{
					if ( (signed int)(2 * yy_current_buffer->yy_buf_size) > 0 )
						yy_current_buffer->yy_buf_size *= 2;
					else
						yy_current_buffer->yy_buf_size += yy_current_buffer->yy_buf_size >> 3;
					b->yy_ch_buf = (char *)yy_flex_realloc(b->yy_ch_buf, b->yy_buf_size + 2);
				}
				else
				{
					yy_current_buffer->yy_ch_buf = 0;
				}
				if ( !b->yy_ch_buf )
					yy_fatal_error("fatal error - scanner input buffer overflow");
				yy_c_buf_p = &b->yy_ch_buf[yy_c_buf_p_offset];
			}
			if ( num_to_read > 0x2000 )
				num_to_read = 0x2000;
			yy_n_chars = Scr_ScanFile(&yy_current_buffer->yy_ch_buf[number_to_move], num_to_read);
			yy_current_buffer->yy_n_chars = yy_n_chars;
		}
		if ( yy_n_chars )
		{
			ret_val = 0;
		}
		else if ( number_to_move )
		{
			ret_val = 2;
			yy_current_buffer->yy_buffer_status = 2;
		}
		else
		{
			ret_val = 1;
			yyrestart(yyin);
		}
		yy_n_chars += number_to_move;
		yy_current_buffer->yy_ch_buf[yy_n_chars] = 0;
		yy_current_buffer->yy_ch_buf[yy_n_chars + 1] = 0;
		yytext = yy_current_buffer->yy_ch_buf;
		return ret_val;
	}
	else if ( yy_c_buf_p - yytext == 1 )
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

int yylex()
{
	int next_buffer;
	unsigned char yy_c;
	int yy_act;
	char *c;
	char *yy_cp;
	int yy_current_state;
	int yy_next_state;
	int yy_amount_of_matched_text;

	if ( yy_init )
	{
		yy_init = 0;
		if ( !yy_start )
			yy_start = 1;
		if ( !yyin )
			yyin = (FILE *)stdin;
		if ( !yyout )
			yyout = stdout;
		if ( !yy_current_buffer )
			yy_current_buffer = yy_create_buffer(yyin, 0x4000);
		yy_load_buffer_state();
	}
yy_begin:
	yy_cp = yy_c_buf_p;
	*yy_c_buf_p = yy_hold_char;
	c = yy_cp;
	yy_current_state = yy_start;
	do
	{
yy_match:
		yy_c = yy_ec[(unsigned char)*yy_cp];
		if ( yy_accept[yy_current_state] )
		{
			yy_last_accepting_state = yy_current_state;
			yy_last_accepting_cpos = yy_cp;
		}
		while ( yy_chk[yy_c + yy_base[yy_current_state]] != yy_current_state )
		{
			yy_current_state = yy_def[yy_current_state];
			if ( yy_current_state > 255 )
				yy_c = yy_meta[yy_c];
		}
		yy_current_state = yy_nxt[yy_c + yy_base[yy_current_state]];
		++yy_cp;
	}
	while ( yy_base[yy_current_state] != 431 );
	while ( 1 )
	{
		yy_act = yy_accept[yy_current_state];
		if ( !yy_accept[yy_current_state] )
		{
			yy_cp = yy_last_accepting_cpos;
			yy_act = yy_accept[yy_last_accepting_state];
		}
		yytext = c;
		yyleng = yy_cp - c;
		yy_hold_char = *yy_cp;
		*yy_cp = 0;
		yy_c_buf_p = yy_cp;
do_action:
		switch ( yy_act )
		{
		case 0:
			*yy_cp = yy_hold_char;
			yy_cp = yy_last_accepting_cpos;
			yy_current_state = yy_last_accepting_state;
			continue;
		case 1:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			goto yy_begin;
		case 2:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			yy_start = 3;
			goto yy_begin;
		case 3:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			goto yy_begin;
		case 4:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			goto yy_begin;
		case 5:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			goto yy_begin;
		case 6:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			yy_start = 5;
			goto yy_begin;
		case 7:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			if ( StringValue(yytext + 1, yyleng - 2) )
				return 259;
			else
				return 257;
		case 8:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			if ( StringValue(yytext + 2, yyleng - 3) )
				return 260;
			else
				return 257;
		case 9:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 261;
		case 10:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 262;
		case 11:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 263;
		case 12:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 264;
		case 13:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 265;
		case 14:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 266;
		case 15:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 267;
		case 16:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 268;
		case 17:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 269;
		case 18:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 270;
		case 19:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 271;
		case 20:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 272;
		case 21:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 273;
		case 22:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 274;
		case 23:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 275;
		case 24:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 276;
		case 25:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 277;
		case 26:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 278;
		case 27:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 279;
		case 28:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 280;
		case 29:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 281;
		case 30:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 282;
		case 31:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 283;
		case 32:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 284;
		case 33:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 285;
		case 34:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 286;
		case 35:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			IntegerValue(yytext);
			return 287;
		case 36:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			FloatValue(yytext);
			return 288;
		case 37:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 290;
		case 38:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 289;
		case 39:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 294;
		case 40:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 291;
		case 41:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 293;
		case 42:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 292;
		case 43:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 295;
		case 44:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 296;
		case 45:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 297;
		case 46:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 298;
		case 47:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 299;
		case 48:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 300;
		case 49:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 301;
		case 50:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 302;
		case 51:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 303;
		case 52:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 304;
		case 53:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 305;
		case 54:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 306;
		case 55:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 307;
		case 56:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 308;
		case 57:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 309;
		case 58:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 310;
		case 59:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 311;
		case 60:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 312;
		case 61:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 313;
		case 62:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 314;
		case 63:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 315;
		case 64:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 316;
		case 65:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 317;
		case 66:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 318;
		case 67:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 319;
		case 68:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 320;
		case 69:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 321;
		case 70:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 322;
		case 71:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 323;
		case 72:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 325;
		case 73:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 326;
		case 74:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 327;
		case 75:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 328;
		case 76:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 329;
		case 77:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 330;
		case 78:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 331;
		case 79:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 332;
		case 80:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 333;
		case 81:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 334;
		case 82:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 335;
		case 83:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 336;
		case 84:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 337;
		case 85:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 338;
		case 86:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 339;
		case 87:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 340;
		case 88:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 341;
		case 89:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			return 342;
		case 90:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			TextValue(yytext, yyleng);
			return 258;
		case 91:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			TextValue(yytext, yyleng);
			return 324;
		case 92:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			CompileError(g_sourcePos, "bad token '%s'", yytext);
			return 257;
		case 93:
			yylval.pos = g_out_pos;
			g_sourcePos = g_out_pos;
			g_out_pos += yyleng;
			fwrite(yytext, yyleng, 1u, yyout);
			goto yy_begin;
		case 94:
			yy_amount_of_matched_text = yy_cp - yytext - 1;
			*yy_cp = yy_hold_char;
			if ( !yy_current_buffer->yy_buffer_status )
			{
				yy_n_chars = yy_current_buffer->yy_n_chars;
				yy_current_buffer->yy_input_file = yyin;
				yy_current_buffer->yy_buffer_status = 1;
			}
			if ( yy_c_buf_p <= &yy_current_buffer->yy_ch_buf[yy_n_chars] )
			{
				yy_c_buf_p = &yytext[yy_amount_of_matched_text];
				yy_current_state = yy_get_previous_state();
				yy_next_state = yy_try_NUL_trans(yy_current_state);
				c = yytext;
				if ( yy_next_state )
				{
					yy_cp = ++yy_c_buf_p;
					yy_current_state = yy_next_state;
					goto yy_match;
				}
				yy_cp = yy_c_buf_p;
				continue;
			}
			next_buffer = yy_get_next_buffer();
			if ( next_buffer == 1 )
			{
				yy_did_buffer_switch_on_eof = 0;
				if ( CL_LocalClient_GetActiveCount() )
				{
					yy_c_buf_p = yytext;
					yy_act = (yy_start - 1) / 2 + 95;
					goto do_action;
				}
				if ( !yy_did_buffer_switch_on_eof )
					yyrestart(yyin);
				goto yy_begin;
			}
			if ( next_buffer <= 1 )
			{
				if ( next_buffer )
					goto yy_begin;
				yy_c_buf_p = &yytext[yy_amount_of_matched_text];
				yy_current_state = yy_get_previous_state();
				yy_cp = yy_c_buf_p;
				c = yytext;
				goto yy_match;
			}
			if ( next_buffer != 2 )
				goto yy_begin;
			yy_c_buf_p = &yy_current_buffer->yy_ch_buf[yy_n_chars];
			yy_current_state = yy_get_previous_state();
			yy_cp = yy_c_buf_p;
			c = yytext;
			break;
		case 95:
		case 96:
		case 97:
			return 0;
		default:
			yy_fatal_error("fatal flex scanner internal error--no action found");
		}
	}
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