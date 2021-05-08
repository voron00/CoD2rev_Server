#include "i_common.h"
#include "i_shared.h"

static huffman_t msgHuff;
static qboolean msgInit = qfalse;

int msg_hData[256] =
{
	250315,
	41193,
	6292,
	7106,
	3730,
	3750,
	6110,
	23283,
	33317,
	6950,
	7838,
	9714,
	9257,
	17259,
	3949,
	1778,
	8288,
	1604,
	1590,
	1663,
	1100,
	1213,
	1238,
	1134,
	1749,
	1059,
	1246,
	1149,
	1273,
	4486,
	2805,
	3472,
	21819,
	1159,
	1670,
	1066,
	1043,
	1012,
	1053,
	1070,
	1726,
	888,
	1180,
	850,
	960,
	780,
	1752,
	3296,
	10630,
	4514,
	5881,
	2685,
	4650,
	3837,
	2093,
	1867,
	2584,
	1949,
	1972,
	940,
	1134,
	1788,
	1670,
	1206,
	5719,
	6128,
	7222,
	6654,
	3710,
	3795,
	1492,
	1524,
	2215,
	1140,
	1355,
	971,
	2180,
	1248,
	1328,
	1195,
	1770,
	1078,
	1264,
	1266,
	1168,
	965,
	1155,
	1186,
	1347,
	1228,
	1529,
	1600,
	2617,
	2048,
	2546,
	3275,
	2410,
	3585,
	2504,
	2800,
	2675,
	6146,
	3663,
	2840,
	14253,
	3164,
	2221,
	1687,
	3208,
	2739,
	3512,
	4796,
	4091,
	3515,
	5288,
	4016,
	7937,
	6031,
	5360,
	3924,
	4892,
	3743,
	4566,
	4807,
	5852,
	6400,
	6225,
	8291,
	23243,
	7838,
	7073,
	8935,
	5437,
	4483,
	3641,
	5256,
	5312,
	5328,
	5370,
	3492,
	2458,
	1694,
	1821,
	2121,
	1916,
	1149,
	1516,
	1367,
	1236,
	1029,
	1258,
	1104,
	1245,
	1006,
	1149,
	1025,
	1241,
	952,
	1287,
	997,
	1713,
	1009,
	1187,
	879,
	1099,
	929,
	1078,
	951,
	1656,
	930,
	1153,
	1030,
	1262,
	1062,
	1214,
	1060,
	1621,
	930,
	1106,
	912,
	1034,
	892,
	1158,
	990,
	1175,
	850,
	1121,
	903,
	1087,
	920,
	1144,
	1056,
	3462,
	2240,
	4397,
	12136,
	7758,
	1345,
	1307,
	3278,
	1950,
	886,
	1023,
	1112,
	1077,
	1042,
	1061,
	1071,
	1484,
	1001,
	1096,
	915,
	1052,
	995,
	1070,
	876,
	1111,
	851,
	1059,
	805,
	1112,
	923,
	1103,
	817,
	1899,
	1872,
	976,
	841,
	1127,
	956,
	1159,
	950,
	7791,
	954,
	1289,
	933,
	1127,
	3207,
	1020,
	927,
	1355,
	768,
	1040,
	745,
	952,
	805,
	1073,
	740,
	1013,
	805,
	1008,
	796,
	996,
	1057,
	11457,
	13504
};

void MSG_initHuffman()
{
	int i,j;

	msgInit = qtrue;
	Huff_Init(&msgHuff);

	for(i=0; i<256; i++)
	{
		for (j=0; j<msg_hData[i]; j++)
		{
			Huff_addRef(&msgHuff.compressor,	(byte)i);			// Do update
			Huff_addRef(&msgHuff.decompressor,	(byte)i);			// Do update
		}
	}
}

void MSG_Init( msg_t *buf, byte *data, int length )
{
	if (!msgInit)
	{
		MSG_initHuffman();
	}

	buf->data = data;
	buf->maxsize = length;
	buf->overflowed = qfalse;
	buf->cursize = 0;
	buf->readcount = 0;
	buf->bit = 0;
}

void MSG_BeginReading( msg_t *msg )
{
	msg->overflowed = qfalse;
	msg->readcount = 0;
	msg->bit = 0;
}

int MSG_ReadBits(msg_t *msg, int bits)
{
	int i;
	signed int var;
	int retval;

	retval = 0;

	if ( bits > 0 )
	{
		for(i = 0 ; bits != i; i++)
		{
			if ( !(msg->bit & 7) )
			{
				if ( msg->readcount >= msg->cursize )
				{
					msg->overflowed = 1;
					return -1;
				}
				msg->bit = 8 * msg->readcount;
				msg->readcount++;
			}
			var = msg->data[msg->bit / 8];
			retval |= ((var >> (msg->bit & 7)) & 1) << i;
			msg->bit++;
		}
	}

	return retval;
}

int MSG_ReadBitsCompress(const byte* input, int readsize, byte* outputBuf, int outputBufSize)
{
	readsize = readsize * 8;
	byte *outptr = outputBuf;

	int get;
	int offset;
	int i;

	if(readsize <= 0)
	{
		return 0;
	}

	for(offset = 0, i = 0; offset < readsize && i < outputBufSize; i++)
	{
		Huff_offsetReceive(msgHuff.decompressor.tree, &get, (byte*)input, &offset);
		*outptr = (byte)get;
		outptr++;
	}

	return i;
}

int MSG_WriteBitsCompress( const byte *datasrc, byte *buffdest, int bytecount)
{
	int offset;
	int i;

	if(bytecount <= 0)
	{
		return 0;
	}

	for(offset = 0, i = 0; i < bytecount; i++)
	{
		Huff_offsetTransmit(&msgHuff.compressor, (int)datasrc[i], buffdest, &offset);
	}

	return (offset + 7) / 8;
}

int MSG_GetByte(msg_t *msg, int where)
{
	return msg->data[where];
}

int MSG_GetShort(msg_t *msg, int where)
{
	return *(int16_t*)&msg->data[where];
}

int MSG_GetLong(msg_t *msg, int where)
{
	return *(int32_t*)&msg->data[where];
}

int MSG_ReadByte( msg_t *msg )
{
	byte	c;

	if ( msg->readcount+(int)sizeof(byte) > msg->cursize )
	{
		msg->overflowed = 1;
		return -1;
	}

	c = MSG_GetByte(msg, msg->readcount);
	msg->readcount += sizeof(byte);

	return c;
}

int MSG_ReadShort( msg_t *msg )
{
	int16_t	c;

	if ( msg->readcount+(int)sizeof(short) > msg->cursize )
	{
		msg->overflowed = 1;
		return -1;
	}

	c = MSG_GetShort(msg, msg->readcount);
	msg->readcount += sizeof(short);

	return c;
}

int MSG_ReadLong( msg_t *msg )
{
	int32_t	c;

	if ( msg->readcount+(int)sizeof(int32_t) > msg->cursize )
	{
		msg->overflowed = 1;
		return -1;
	}

	c = MSG_GetLong(msg, msg->readcount);
	msg->readcount += sizeof(int32_t);

	return c;
}

char *MSG_ReadString( msg_t *msg )
{
	static char string[MAX_STRING_CHARS];
	int l,c;

	l = 0;
	do
	{
		c = MSG_ReadByte( msg );      // use ReadByte so -1 is out of bounds
		if ( c == -1 || c == 0 )
		{
			break;
		}
		// translate all fmt spec to avoid crash bugs
		if ( c == '%' )
		{
			c = '.';
		}
		// don't allow higher ascii values
		if ( c > 127 )
		{
			c = '.';
		}

		string[l] = c;
		l++;
	}
	while ( l < (int)sizeof( string ) - 1 );

	string[l] = 0;

	return string;
}

char *MSG_ReadBigString( msg_t *msg )
{
	static char string[BIG_INFO_STRING];
	int l,c;

	l = 0;
	do
	{
		c = MSG_ReadByte( msg );      // use ReadByte so -1 is out of bounds
		if ( c == -1 || c == 0 )
		{
			break;
		}
		// translate all fmt spec to avoid crash bugs
		if ( c == '%' )
		{
			c = '.';
		}

		string[l] = c;
		l++;
	}
	while ( l < (int)sizeof( string ) - 1 );

	string[l] = 0;

	return string;
}

char *MSG_ReadStringLine( msg_t *msg )
{
	static char string[MAX_STRING_CHARS];
	int l,c;

	l = 0;
	do
	{
		c = MSG_ReadByte( msg );      // use ReadByte so -1 is out of bounds
		if ( c == -1 || c == 0 || c == '\n' )
		{
			break;
		}
		// translate all fmt spec to avoid crash bugs
		if ( c == '%' )
		{
			c = '.';
		}
		string[l] = c;
		l++;
	}
	while ( l < (int)sizeof( string ) - 1 );

	string[l] = 0;

	return string;
}

void MSG_WriteByte( msg_t *msg, int c )
{
	int8_t* dst;

	if ( msg->maxsize - msg->cursize < 1 )
	{
		msg->overflowed = qtrue;
		return;
	}

	dst = (int8_t*)&msg->data[msg->cursize];
	*dst = c;
	msg->cursize += sizeof(int8_t);
}

void MSG_WriteShort( msg_t *msg, int c )
{
	signed short* dst;

	if ( msg->maxsize - msg->cursize < 2 )
	{
		msg->overflowed = qtrue;
		return;
	}

	dst = (int16_t*)&msg->data[msg->cursize];
	*dst = c;
	msg->cursize += sizeof(short);
}

void MSG_WriteLong( msg_t *msg, int c )
{
	int32_t *dst;

	if ( msg->maxsize - msg->cursize < 4 )
	{
		msg->overflowed = qtrue;
		return;
	}

	dst = (int32_t*)&msg->data[msg->cursize];
	*dst = c;
	msg->cursize += sizeof(int32_t);
}

void MSG_WriteData( msg_t *buf, const void *data, int length )
{
	int i;
	for(i=0; i < length; i++)
	{
		MSG_WriteByte(buf, ((byte*)data)[i]);
	}
}

void MSG_WriteString( msg_t *sb, const char *s )
{
	if ( !s )
	{
		MSG_WriteData( sb, "", 1 );
	}
	else
	{
		int l;
		char string[MAX_STRING_CHARS];

		l = strlen( s );
		if ( l >= MAX_STRING_CHARS )
		{
			Com_Printf("MSG_WriteString: MAX_STRING_CHARS");
			MSG_WriteData( sb, "", 1 );
			return;
		}

		I_strncpyz( string, s, sizeof( string ) );
		MSG_WriteData( sb, string, l + 1 );
	}
}

void MSG_WriteBigString( msg_t *sb, const char *s )
{
	if ( !s )
	{
		MSG_WriteData( sb, "", 1 );
	}
	else
	{
		int l;
		char string[BIG_INFO_STRING];

		l = strlen( s );
		if ( l >= BIG_INFO_STRING )
		{
			Com_Printf("MSG_WriteString: BIG_INFO_STRING");
			MSG_WriteData( sb, "", 1 );
			return;
		}

		I_strncpyz( string, s, sizeof( string ) );
		MSG_WriteData( sb, string, l + 1 );
	}
}