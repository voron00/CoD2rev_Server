#include "qcommon.h"

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

void MSG_WriteBits(msg_t *msg, int bits, int bitcount)
{
	int i;

	if ( msg->maxsize - msg->cursize < 4 )
	{
		msg->overflowed = qtrue;
		return;
	}

	if ( bitcount )
	{
		for (i = 0 ; bitcount != i; i++)
		{

			if ( !(msg->bit & 7) )
			{
				msg->bit = 8 * msg->cursize;
				msg->data[msg->cursize] = 0;
				msg->cursize++;
			}

			if ( bits & 1 )
				msg->data[msg->bit >> 3] |= 1 << (msg->bit & 7);

			msg->bit++;
			bits >>= 1;
		}
	}
}

void MSG_WriteBit0( msg_t* msg )
{
	if(!(msg->bit & 7))
	{
		if(msg->maxsize <= msg->cursize)
		{
			msg->overflowed = qtrue;
			return;
		}
		msg->bit = msg->cursize*8;
		msg->data[msg->cursize] = 0;
		msg->cursize ++;
	}
	msg->bit++;
}

void MSG_WriteBit1(msg_t *msg)
{
	if ( !(msg->bit & 7) )
	{
		if ( msg->cursize >= msg->maxsize )
		{
			msg->overflowed = qtrue;
			return;
		}
		msg->bit = 8 * msg->cursize;
		msg->data[msg->cursize] = 0;
		msg->cursize++;
	}
	msg->data[msg->bit >> 3] |= 1 << (msg->bit & 7);
	msg->bit++;
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
					msg->overflowed = qtrue;
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

int MSG_ReadBit(msg_t *msg)
{
	int oldbit;

	oldbit = msg->bit & 7;

	if ( oldbit )
		return (msg->data[msg->bit++ >> 3] >> oldbit) & 1;

	if ( msg->readcount < msg->cursize )
	{
		msg->bit = 8 * msg->readcount++;
		return (msg->data[msg->bit++ >> 3] >> oldbit) & 1;
	}

	msg->overflowed = qtrue;
	return -1;
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

int MSG_ReadBitsCompress(const byte* input, byte* outputBuf, int readsize)
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

	for(offset = 0, i = 0; offset < readsize; i++)
	{
		Huff_offsetReceive(msgHuff.decompressor.tree, &get, (byte*)input, &offset);
		*outptr = (byte)get;
		outptr++;
	}

	return i;
}

void MSG_WriteData(msg_t *buf, const void *data, int length)
{
	int size;

	size = buf->cursize + length;

	if ( size > buf->maxsize )
	{
		buf->overflowed = qtrue;
	}
	else
	{
		Com_Memcpy(&buf->data[buf->cursize], data, length);
		buf->cursize = size;
	}
}

void MSG_WriteByte( msg_t *msg, int c )
{
	int8_t* dst;

	if ( msg->maxsize - msg->cursize < sizeof(int8_t) )
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
	int16_t* dst;

	if ( msg->maxsize - msg->cursize < sizeof(int16_t) )
	{
		msg->overflowed = qtrue;
		return;
	}

	dst = (int16_t*)&msg->data[msg->cursize];
	*dst = c;
	msg->cursize += sizeof(int16_t);
}

void MSG_WriteLong( msg_t *msg, int c )
{
	int32_t *dst;

	if ( msg->maxsize - msg->cursize < sizeof(int32_t) )
	{
		msg->overflowed = qtrue;
		return;
	}

	dst = (int32_t*)&msg->data[msg->cursize];
	*dst = c;
	msg->cursize += sizeof(int32_t);
}

void MSG_WriteLong64( msg_t *msg, int c )
{
	int64_t *dst;

	if ( msg->maxsize - msg->cursize < sizeof(int64_t) )
	{
		msg->overflowed = qtrue;
		return;
	}

	dst = (int64_t*)&msg->data[msg->cursize];
	*dst = c;
	msg->cursize += sizeof(int64_t);
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
			Com_Printf("MSG_WriteString: MAX_STRING_CHARS" );
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
			Com_Printf("MSG_WriteString: BIG_INFO_STRING" );
			MSG_WriteData( sb, "", 1 );
			return;
		}

		I_strncpyz( string, s, sizeof( string ) );
		MSG_WriteData( sb, string, l + 1 );
	}
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

int MSG_GetLong64(msg_t *msg, int where)
{
	return *(int64_t*)&msg->data[where];
}

int MSG_ReadByte( msg_t *msg )
{
	byte	c;

	if ( msg->readcount+(int)sizeof(byte) > msg->cursize )
	{
		msg->overflowed = qtrue;
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
		msg->overflowed = qtrue;
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
		msg->overflowed = qtrue;
		return -1;
	}

	c = MSG_GetLong(msg, msg->readcount);
	msg->readcount += sizeof(int32_t);

	return c;
}

int MSG_ReadLong64( msg_t *msg )
{
	int64_t	c;

	if ( msg->readcount+(int)sizeof(int64_t) > msg->cursize )
	{
		msg->overflowed = qtrue;
		return -1;
	}

	c = MSG_GetLong(msg, msg->readcount);
	msg->readcount += sizeof(int64_t);

	return c;
}

void MSG_ReadData(msg_t *msg, void *data, int len)
{
	int size;

	size = msg->readcount + len;

	if ( size > msg->cursize )
	{
		msg->overflowed = qtrue;
		Com_Memset(data, -1, len);
	}
	else
	{
		Com_Memcpy(data, &msg->data[msg->readcount], len);
		msg->readcount = size;
	}
}

char *MSG_ReadCommandString( msg_t *msg )
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

		string[l] = I_CleanChar(c);
		l++;
	}
	while ( l < sizeof( string ) - 1 );

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

		string[l] = I_CleanChar(c);
		l++;
	}
	while ( l < sizeof( string ) - 1 );

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
		if ( c == 10 || c == -1 || c == 0 )
		{
			break;
		}
		// translate all fmt spec to avoid crash bugs
		if ( c == '%' )
		{
			c = '.';
		}

		string[l] = I_CleanChar(c);
		l++;
	}
	while ( l < sizeof( string ) - 1 );

	string[l] = 0;

	return string;
}

void MSG_WriteReliableCommandToBuffer(const char *source, char *destination, int length)
{
	int i;
	int requiredlen;

	if ( *source == '\0' )
		Com_Printf("WARNING: Empty reliable command\n");

	for (i = 0 ; i < (length -1) && source[i] != '\0'; i++)
	{
		destination[i] = I_CleanChar(source[i]);

		if ( destination[i] == '%' )
		{
			destination[i] = '.';
		}
	}

	destination[i] = '\0';

	if ( i == length -1)
	{
		requiredlen = strlen(source) + 1;

		if (requiredlen > length)
		{
			Com_Printf("WARNING: Reliable command is too long (%i/%i) and will be truncated: '%s'\n", requiredlen, length, source);
		}
	}
}

void MSG_SetDefaultUserCmd(playerState_t *ps, usercmd_t *ucmd)
{
	int i;

	Com_Memset(ucmd, 0, sizeof(usercmd_t));

	ucmd->weapon = ps->weapon;
	ucmd->offHandIndex = ps->offHandIndex;

	for(i = 0; i < 2; i++)
	{
		ucmd->angles[i] = ANGLE2SHORT(ps->viewangles[i]) - LOWORD(ps->delta_angles[i]);
	}

	if ( (ps->pm_flags & 0x800000) != 0 )
	{
		if ( (ps->eFlags & 8) != 0 )
		{
			ucmd->buttons |= 0x100u;
		}
		else if ( (ps->eFlags & 4) != 0 )
		{
			ucmd->buttons |= 0x200u;
		}

		if ( ps->leanf <= 0.0 )
		{
			if ( ps->leanf < 0.0 )
				ucmd->buttons |= 0x40u;
		}
		else
		{
			ucmd->buttons |= 0x80u;
		}

		if ( ps->fWeaponPosFrac != 0.0 )
			ucmd->buttons |= 0x1000u;
	}
}

uint32_t kbitmask[33] =
{
	0,
	1,
	3,
	7,
	15,
	31,
	63,
	127,
	255,
	511,
	1023,
	2047,
	4095,
	8191,
	16383,
	32767,
	65535,
	131071,
	262143,
	524287,
	1048575,
	2097151,
	4194303,
	8388607,
	16777215,
	33554431,
	67108863,
	134217727,
	268435455,
	536870911,
	1073741823,
	2147483647,
	4294967294,
};

unsigned int MSG_ReadKey(msg_t *msg, int key, int bits)
{
	return kbitmask[bits] & (key ^ MSG_ReadBits(msg, bits));
}

int MSG_ReadDeltaKey(msg_t *msg, int key, int oldV, int bits)
{
	if ( MSG_ReadBit(msg) )
	{
		return kbitmask[bits] & (key ^ MSG_ReadBits(msg, bits));
	}

	return oldV;
}

int MSG_ReadDeltaKeyShort(msg_t *msg, int key, int oldV)
{
	if ( MSG_ReadBit(msg) )
	{
		return (int16_t)(key ^ MSG_ReadShort(msg));
	}

	return oldV;
}

int MSG_ReadForwardRightMoveMask(char forwardmove, char rightmove)
{
	int mask = 0;

	if ( forwardmove <= 10 )
	{
		if ( forwardmove < -10 )
			mask = 2;
	}
	else
	{
		mask = 1;
	}

	if ( rightmove > 10 )
		return mask | 4;

	if ( rightmove < -10 )
		return mask | 8;

	return mask;
}

void MSG_ClampForwardRightmove(char key, char *forwardmove, char *rightmove)
{
	if ( (key & 1) != 0 )
	{
		*forwardmove = 127;
	}
	else if ( (key & 2) != 0 )
	{
		*forwardmove = -127;
	}
	else
	{
		*forwardmove = 0;
	}

	if ( (key & 4) != 0 )
	{
		*rightmove = 127;
	}
	else if ( (key & 8) != 0 )
	{
		*rightmove = -127;
	}
	else
	{
		*rightmove = 0;
	}
}

void MSG_ReadDeltaUsercmdKey(msg_t *msg, int key, usercmd_t *from, usercmd_t *to)
{
	int mask;
	int newkey;
	int deltakey;

	Com_Memcpy(to, from, sizeof(usercmd_t));

	if ( MSG_ReadBit(msg) )
		to->serverTime = from->serverTime + MSG_ReadByte(msg);
	else
		to->serverTime = MSG_ReadLong(msg);

	if ( MSG_ReadKey(msg, key, 1) )
	{
		to->buttons &= ~1u;

		if ( MSG_ReadKey(msg, key, 1) )
		{
			to->buttons |= MSG_ReadKey(msg, key, 1);
			to->angles[0] = MSG_ReadDeltaKeyShort(msg, key, from->angles[0]);
			to->angles[1] = MSG_ReadDeltaKeyShort(msg, key, from->angles[1]);
			mask = MSG_ReadForwardRightMoveMask(from->forwardmove, from->rightmove);
			deltakey = MSG_ReadDeltaKey(msg, key, mask, 4);
			MSG_ClampForwardRightmove(deltakey, &to->forwardmove, &to->rightmove);
			newkey = to->serverTime ^ key;
			to->angles[2] = MSG_ReadDeltaKeyShort(msg, newkey, from->angles[2]);
			to->buttons &= 1u;
			to->buttons |= 2 * MSG_ReadDeltaKey(msg, newkey, from->buttons >> 1, 18);
			to->weapon = MSG_ReadDeltaKey(msg, newkey, from->weapon, 7);
			to->offHandIndex = MSG_ReadDeltaKey(msg, newkey, from->offHandIndex, 7);
		}
		else
		{
			newkey = to->serverTime ^ key;
			to->buttons |= MSG_ReadKey(msg, newkey, 1);
			to->angles[0] = MSG_ReadDeltaKeyShort(msg, newkey, from->angles[0]);
			to->angles[1] = MSG_ReadDeltaKeyShort(msg, newkey, from->angles[1]);
			mask = MSG_ReadForwardRightMoveMask(from->forwardmove, from->rightmove);
			deltakey = MSG_ReadDeltaKey(msg, newkey, mask, 4);
			MSG_ClampForwardRightmove(deltakey, &to->forwardmove, &to->rightmove);
		}
	}
}

void MSG_WriteAngle16( msg_t *sb, float f )
{
	MSG_WriteShort( sb, ANGLE2SHORT( f ) );
}

void MSG_WriteDeltaField(msg_t *msg, const byte *from, const byte *to, const netField_t *field)
{
	int32_t absbits;
	uint8_t abs3bits;
	int32_t bitmask;
	float floatbits;
	int32_t signedbits;
	uint32_t unsignedbits;

	if ( *(uint32_t *)&from[field->offset] == *(uint32_t *)(float *)&to[field->offset] )
	{
		MSG_WriteBit0(msg);
		return;
	}

	MSG_WriteBit1(msg);

	floatbits = *(float *)&to[field->offset];
	signedbits = *(int32_t *)&to[field->offset];
	unsignedbits = *(uint32_t *)&to[field->offset];

	switch (field->bits)
	{
	case 0:
		if ( floatbits == 0.0 )
		{
			MSG_WriteBit0(msg);
			return;
		}
		MSG_WriteBit1(msg);
		if ( (float)signedbits == floatbits && (unsigned int)(signedbits + 4096) <= 0x1FFF )
		{
			MSG_WriteBit0(msg);
			MSG_WriteBits(msg, signedbits + 4096, 5);
			MSG_WriteByte(msg, (signedbits + 4096) >> 5);
			return;
		}
		MSG_WriteBit1(msg);
		MSG_WriteLong(msg, unsignedbits);
		return;

	case -99:
		if ( floatbits == 0.0 )
		{
			MSG_WriteBit0(msg);
			return;
		}
		MSG_WriteBit1(msg);
		if ( (float)signedbits == floatbits && (unsigned int)(signedbits + 512) <= 0x3FF )
		{
			MSG_WriteBit0(msg);
			MSG_WriteBits(msg, signedbits + 512, 4);
			MSG_WriteByte(msg, (signedbits + 512) >> 4);
			return;
		}
		MSG_WriteBit1(msg);
		MSG_WriteLong(msg, unsignedbits);
		return;

	case -100:
		if ( unsignedbits )
		{
			MSG_WriteBit1(msg);
			MSG_WriteAngle16(msg, floatbits);
			return;
		}
		MSG_WriteBit0(msg);
		return;

	default:
		if ( unsignedbits )
		{
			MSG_WriteBit1(msg);
			absbits = abs(field->bits);
			bitmask = unsignedbits;
			abs3bits = absbits & 7;
			if ( abs3bits )
			{
				MSG_WriteBits(msg, bitmask, absbits & 7);
				absbits -= abs3bits;
				bitmask >>= abs3bits;
			}
			while( absbits )
			{
				MSG_WriteByte(msg, bitmask);
				bitmask >>= 8;
				absbits -= 8;
			}
			return;
		}
		MSG_WriteBit0(msg);
		return;
	}
}

void MSG_WriteDeltaStruct(msg_t *msg, const byte *from, const byte *to, qboolean force, int numFields, int indexBits, netField_t *stateFields, qboolean bChangeBit)
{
	netField_t* field;
	int lc;
	int i;

	if (to)
	{
		lc = 0;

		for (i = 0, field = stateFields; i < numFields; i++, field++)
		{
			if (*(uint32_t *)&from[field->offset] != *(uint32_t *)&to[field->offset])
			{
				lc = i + 1;
			}
		}

		if ( lc )
		{
			if ( bChangeBit )
				MSG_WriteBit1(msg);

			MSG_WriteBits(msg, *(uint32_t *)to, indexBits);
			MSG_WriteBit0(msg);
			MSG_WriteBit1(msg);
			MSG_WriteByte(msg, lc);

			for (i = 0, field = stateFields; i < lc; i++, field++)
			{
				MSG_WriteDeltaField(msg, from, to, field);
			}
		}
		else if ( force )
		{
			if ( bChangeBit )
				MSG_WriteBit1(msg);

			MSG_WriteBits(msg, *(uint32_t *)to, indexBits);
			MSG_WriteBit0(msg);
			MSG_WriteBit0(msg);
		}
	}
	else
	{
		if ( bChangeBit )
			MSG_WriteBit1(msg);

		MSG_WriteBits(msg, *(uint32_t *)from, indexBits);
		MSG_WriteBit1(msg);
	}
}

#define OBJF( x ) # x,(intptr_t)&( (objective_t*)0 )->x
netField_t objectiveFields[] =
{
	{ OBJF( origin[0] ), 0},
	{ OBJF( origin[1] ), 0},
	{ OBJF( origin[2] ), 0},
	{ OBJF( icon ), 12},
	{ OBJF( entNum ), 10},
	{ OBJF( teamNum ), 4},
};

void MSG_WriteDeltaObjective(msg_t *msg,objective_t *from,objective_t *to, int lc, int numFields, netField_t *objFields)
{
	int i;

	if (lc)
	{
WRITE:
		MSG_WriteBit1(msg);

		for (i = 0; i < numFields; ++i)
		{
			MSG_WriteDeltaField(msg, (byte *)from, (byte *)to, &objFields[i]);
		}
	}
	else
	{
		for (i = 0; i < numFields; ++i)
		{
			if ( *(int32_t *)((byte *)from + objFields[i].offset) != *(int32_t *)((byte *)to + objFields[i].offset) )
			{
				goto WRITE;
			}
		}

		MSG_WriteBit0(msg);
	}
}

#define NETF( x ) # x,(intptr_t)&( (entityState_t*)0 )->x
netField_t entityStateFields[] =
{
	{ NETF( pos.trTime ), 32},
	{ NETF( pos.trBase[1] ), 0},
	{ NETF( pos.trBase[0] ), 0},
	{ NETF( pos.trDelta[0] ), 0},
	{ NETF( pos.trDelta[1] ), 0},
	{ NETF( angles2[1] ), 0},
	{ NETF( apos.trBase[1] ), -100},
	{ NETF( pos.trDelta[2] ), 0},
	{ NETF( pos.trBase[2] ), 0},
	{ NETF( apos.trBase[0] ), -100},
	{ NETF( eventSequence ), 8},
	{ NETF( legsAnim ), 10},
	{ NETF( eType ), 8},
	{ NETF( eFlags ), 24},
	{ NETF( otherEntityNum ), 10},
	{ NETF( surfType ), 8},
	{ NETF( eventParm ), 8},
	{ NETF( scale ), 8},
	{ NETF( clientNum ), 8},
	{ NETF( torsoAnim ), 10},
	{ NETF( groundEntityNum ), 10},
	{ NETF( events[0] ), 8},
	{ NETF( events[1] ), 8},
	{ NETF( events[2] ), 8},
	{ NETF( angles2[0] ), 0},
	{ NETF( events[3] ), 8},
	{ NETF( apos.trBase[2] ), -100},
	{ NETF( pos.trType ), 8},
	{ NETF( fWaistPitch ), 0},
	{ NETF( fTorsoPitch ), 0},
	{ NETF( apos.trTime ), 32},
	{ NETF( solid ), 24},
	{ NETF( apos.trDelta[0] ), 0},
	{ NETF( apos.trType ), 8},
	{ NETF( animMovetype ), 4},
	{ NETF( fTorsoHeight ), 0},
	{ NETF( apos.trDelta[2] ), 0},
	{ NETF( weapon ), 7},
	{ NETF( index ), 10},
	{ NETF( apos.trDelta[1] ), 0},
	{ NETF( eventParms[0] ), 8},
	{ NETF( eventParms[1] ), 8},
	{ NETF( eventParms[2] ), 8},
	{ NETF( eventParms[3] ), 8},
	{ NETF( iHeadIcon ), 4},
	{ NETF( pos.trDuration ), 32},
	{ NETF( iHeadIconTeam ), 2},
	{ NETF( time ), 32},
	{ NETF( leanf ), 0},
	{ NETF( attackerEntityNum ), 10},
	{ NETF( time2 ), 32},
	{ NETF( loopSound ), 8},
	{ NETF( origin2[2] ), 0},
	{ NETF( origin2[0] ), 0},
	{ NETF( origin2[1] ), 0},
	{ NETF( angles2[2] ), 0},
	{ NETF( constantLight ), 32},
	{ NETF( apos.trDuration ), 32},
	{ NETF( dmgFlags ), 32},
};

void MSG_WriteDeltaEntity(msg_t *msg, entityState_t *from, entityState_t *to, qboolean force)
{
	MSG_WriteDeltaStruct(msg, (byte *)from, (byte *)to, force, 59, 10, entityStateFields, 0);
}

#define AEF( x ) # x,(intptr_t)&( (archivedEntity_t*)0 )->x
netField_t archivedEntityFields[] =
{
	{ AEF( r.absmin[1] ), 0},
	{ AEF( r.absmax[1] ), 0},
	{ AEF( r.absmin[0] ), 0},
	{ AEF( r.absmax[0] ), 0},
	{ AEF( r.absmin[2] ), 0},
	{ AEF( r.absmax[2] ), 0},
	{ AEF( s.pos.trBase[1] ), 0},
	{ AEF( s.pos.trBase[0] ), 0},
	{ AEF( s.eType ), 8},
	{ AEF( s.eFlags ), 24},
	{ AEF( s.pos.trBase[2] ), 0},
	{ AEF( r.svFlags ), 32},
	{ AEF( s.groundEntityNum ), 10},
	{ AEF( s.apos.trBase[1] ), 0},
	{ AEF( s.clientNum ), 8},
	{ AEF( s.apos.trBase[0] ), 0},
	{ AEF( s.index ), 10},
	{ AEF( s.apos.trBase[2] ), 0},
	{ AEF( s.eventSequence ), 8},
	{ AEF( s.events[0] ), 8},
	{ AEF( s.legsAnim ), 10},
	{ AEF( s.events[1] ), 8},
	{ AEF( s.events[2] ), 8},
	{ AEF( s.events[3] ), 8},
	{ AEF( s.weapon ), 7},
	{ AEF( s.pos.trType ), 8},
	{ AEF( s.pos.trTime ), 32},
	{ AEF( s.apos.trType ), 8},
	{ AEF( s.solid ), 24},
	{ AEF( s.pos.trDuration ), 32},
	{ AEF( s.eventParms[0] ), 8},
	{ AEF( s.torsoAnim ), 10},
	{ AEF( s.pos.trDelta[0] ), 0},
	{ AEF( s.pos.trDelta[1] ), 0},
	{ AEF( s.angles2[1] ), 0},
	{ AEF( s.angles2[0] ), 0},
	{ AEF( s.animMovetype ), 4},
	{ AEF( s.pos.trDelta[2] ), 0},
	{ AEF( s.otherEntityNum ), 10},
	{ AEF( s.eventParms[1] ), 8},
	{ AEF( s.surfType ), 8},
	{ AEF( s.eventParm ), 8},
	{ AEF( s.eventParms[2] ), 8},
	{ AEF( s.scale ), 8},
	{ AEF( s.eventParms[3] ), 8},
	{ AEF( s.fTorsoHeight ), 0},
	{ AEF( s.fWaistPitch ), 0},
	{ AEF( s.fTorsoPitch ), 0},
	{ AEF( s.apos.trTime ), 32},
	{ AEF( s.apos.trDelta[0] ), 0},
	{ AEF( s.apos.trDelta[2] ), 0},
	{ AEF( r.clientMask[0] ), 32},
	{ AEF( r.clientMask[1] ), 32},
	{ AEF( s.leanf ), 0},
	{ AEF( s.apos.trDelta[1] ), 0},
	{ AEF( s.loopSound ), 8},
	{ AEF( s.attackerEntityNum ), 10},
	{ AEF( s.iHeadIcon ), 4},
	{ AEF( s.iHeadIconTeam ), 2},
	{ AEF( s.apos.trDuration ), 32},
	{ AEF( s.time ), 32},
	{ AEF( s.time2 ), 32},
	{ AEF( s.origin2[0] ), 0},
	{ AEF( s.origin2[1] ), 0},
	{ AEF( s.origin2[2] ), 0},
	{ AEF( s.angles2[2] ), 0},
	{ AEF( s.constantLight ), 32},
	{ AEF( s.dmgFlags ), 32},
};

void MSG_WriteDeltaArchivedEntity(msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, int flags)
{
	MSG_WriteDeltaStruct(msg, (byte *)from, (byte *)to, flags, 68, 10, archivedEntityFields, 0);
}

#define CSF( x ) # x,(intptr_t)&( (clientState_t*)0 )->x
netField_t clientStateFields[] =
{
	{ CSF( team ), 2},
	{ CSF( name[0] ), 32},
	{ CSF( name[4] ), 32},
	{ CSF( modelindex ), 8},
	{ CSF( attachModelIndex[1] ), 8},
	{ CSF( attachModelIndex[0] ), 8},
	{ CSF( name[8] ), 32},
	{ CSF( name[12] ), 32},
	{ CSF( name[16] ), 32},
	{ CSF( name[20] ), 32},
	{ CSF( name[24] ), 32},
	{ CSF( name[28] ), 32},
	{ CSF( attachTagIndex[5] ), 5},
	{ CSF( attachTagIndex[0] ), 5},
	{ CSF( attachTagIndex[1] ), 5},
	{ CSF( attachTagIndex[2] ), 5},
	{ CSF( attachTagIndex[3] ), 5},
	{ CSF( attachTagIndex[4] ), 5},
	{ CSF( attachModelIndex[2] ), 8},
	{ CSF( attachModelIndex[3] ), 8},
	{ CSF( attachModelIndex[4] ), 8},
	{ CSF( attachModelIndex[5] ), 8},
};

void MSG_WriteDeltaClient(msg_t *msg, clientState_t *from, clientState_t *to, qboolean force)
{
	clientState_t nullstate;

	if ( !from )
	{
		from = &nullstate;
		Com_Memset(&nullstate, 0, sizeof(nullstate));
	}

	MSG_WriteDeltaStruct(msg, (byte *)from, (byte *)to, force, 22, 6, clientStateFields, 1);
}

float MSG_ReadAngle16(msg_t *msg)
{
	return SHORT2ANGLE((float)MSG_ReadShort(msg));
}

void MSG_ReadDeltaField(msg_t *msg, const void *from, const void *to, netField_t *field, qboolean print)
{
	int *fromF, *toF;
	int readbits;
	int readbyte;
	int bitstemp;
	uint32_t unsignedbits;
	int bitcount;

	fromF = (int *)((byte *)from + field->offset);
	toF = (int *)((byte *)to + field->offset);

	if ( !MSG_ReadBit(msg) )
	{
		*toF = *fromF;
		return;
	}

	switch (field->bits)
	{
	case 0:
		if ( MSG_ReadBit(msg) )
		{
			if ( MSG_ReadBit(msg) )
			{
				*(uint32_t *)toF = MSG_ReadLong(msg);
				if ( print )
					Com_Printf("%s:%f ", field->name, *(float *)toF);
			}
			else
			{
				readbits = MSG_ReadBits(msg, 5);
				readbyte = 32 * MSG_ReadByte(msg) + readbits - 4096;
				*toF = readbyte;
				if ( print )
					Com_Printf("%s:%i ", field->name, readbyte);
			}
		}
		else
			*toF = 0;
		return;

	case -99:
		if ( MSG_ReadBit(msg) )
		{
			if ( MSG_ReadBit(msg) )
			{
				*(uint32_t *)toF = MSG_ReadLong(msg);
				if ( print )
					Com_Printf("%s:%f ", field->name, *(float *)toF);
			}
			else
			{
				readbits = MSG_ReadBits(msg, 2);
				readbyte = 4 * MSG_ReadByte(msg) + readbits - 512;
				*toF = readbyte;
				if ( print )
					Com_Printf("%s:%i ", field->name, readbyte);
			}
		}
		else
			*toF = 0;
		return;

	case -100:
		if ( MSG_ReadBit(msg) )
			*(float *)toF = MSG_ReadAngle16(msg);
		else
			*toF = 0;
		return;

	default:
		if ( MSG_ReadBit(msg) )
		{
			unsignedbits = (unsigned int)field->bits >> 31;
			readbits = abs(field->bits);
			if ( (readbits & 7) != 0 )
				bitstemp = MSG_ReadBits(msg, readbits & 7);
			else
				bitstemp = 0;
			readbyte = bitstemp;
			for ( bitcount = readbits & 7; bitcount < readbits; bitcount += 8 )
				readbyte |= MSG_ReadByte(msg) << bitcount;
			if ( unsignedbits && ((readbyte >> (readbits - 1)) & 1) != 0 )
				readbyte |= ~((1 << readbits) - 1);
			*(uint32_t *)toF = readbyte;
			if ( print )
				Com_Printf("%s:%i ", field->name, *(uint32_t *)toF);
		}
		else
			*toF = 0;
		return;
	}
}

void MSG_ReadDeltaObjective(msg_t *msg, objective_t *from, objective_t *to, int numFields, netField_t *objFields)
{
	int i = 0;

	if ( MSG_ReadBit(msg) )
	{
		for ( i = 0; i < numFields; i++ )
			MSG_ReadDeltaField(msg, from, to, &objFields[i], 0);
	}
	else
	{
		for ( i = 0; i < numFields; i++ )
		{
			*(int32_t *)((byte *)to + objFields[i].offset) = *(int32_t *)((byte *)from + objFields[i].offset);
		}
	}
}

void MSG_ReadDeltaStruct(msg_t *msg, const void *from, void *to, unsigned int number, int numFields, int indexBits, netField_t *stateFields)
{
	NetField *field;
	int lc;
	int i;

	if ( MSG_ReadBit(msg) != 1 )
	{
		if ( MSG_ReadBit(msg) )
		{
			lc = MSG_ReadByte(msg);

			if ( lc <= numFields )
			{
				*(uint32_t *)to = number;
				i = 0;
				field = stateFields;
				while ( i < lc )
				{
					MSG_ReadDeltaField(msg, from, to, field, 0);
					++i;
					++field;
				}
				i = lc;
				field = &stateFields[lc];
				while ( i < numFields )
				{
					*(uint32_t *)((byte *)to + field->offset) = *(uint32_t *)((byte *)from + field->offset);
					++i;
					++field;
				}
			}
			else
			{
				msg->overflowed = qtrue;
			}
		}
		else
		{
			Com_Memcpy(to, from, 4 * numFields + 4);
		}
	}
}

void MSG_ReadDeltaEntity(msg_t *msg, entityState_t *from, entityState_t *to, int number)
{
	int numFields = sizeof( entityStateFields ) / sizeof( entityStateFields[0] );
	MSG_ReadDeltaStruct(msg, from, to, number, numFields, 10, entityStateFields);
}

void MSG_ReadDeltaArchivedEntity(msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, int number)
{
	int numFields = sizeof( archivedEntityFields ) / sizeof( archivedEntityFields[0] );
	MSG_ReadDeltaStruct(msg, from, to, number, numFields, 10, archivedEntityFields);
}

void MSG_ReadDeltaClient(msg_t *msg, clientState_t *from, clientState_t *to, int number)
{
	clientState_t nullstate;

	if ( !from )
	{
		from = &nullstate;
		Com_Memset(&nullstate, 0, sizeof(nullstate));
	}

	int numFields = sizeof( clientStateFields ) / sizeof( clientStateFields[0] );
	MSG_ReadDeltaStruct(msg, from, to, number, numFields, 6, clientStateFields);
}

#define HEF( x ) # x,(intptr_t)&( (hudelem_t*)0 )->x
netField_t hudElemFields[] =
{
	{ HEF( y ), -99},
	{ HEF( type ), 4},
	{ HEF( color.rgba ), 32},
	{ HEF( x ), -99},
	{ HEF( alignScreen ), 6},
	{ HEF( fontScale ), 0},
	{ HEF( materialIndex ), 8},
	{ HEF( width ), 10},
	{ HEF( height ), 10},
	{ HEF( fadeStartTime ), 32},
	{ HEF( fromColor.rgba ), 32},
	{ HEF( fadeTime ), 16},
	{ HEF( value ), 0},
	{ HEF( time ), 32},
	{ HEF( z ), -99},
	{ HEF( alignOrg ), 4},
	{ HEF( sort ), 0},
	{ HEF( text ), 8},
	{ HEF( font ), 4},
	{ HEF( scaleStartTime ), 32},
	{ HEF( scaleTime ), 16},
	{ HEF( fromHeight ), 10},
	{ HEF( label ), 8},
	{ HEF( fromWidth ), 10},
	{ HEF( moveStartTime ), 32},
	{ HEF( moveTime ), 16},
	{ HEF( fromX ), -99},
	{ HEF( fromY ), -99},
	{ HEF( fromAlignScreen ), 6},
	{ HEF( fromAlignOrg ), 4},
	{ HEF( duration ), 32},
	{ HEF( foreground ), 1},
};

void MSG_WriteDeltaHudElems(msg_t *msg, hudelem_t *from, hudelem_t *to, int count)
{
	int i, j, lc, k, l;
	netField_t* field;

	for (i = 0; i < MAX_HUDELEMENTS; ++i);

	MSG_WriteBits(msg, i, 5);

	int numFields = sizeof( hudElemFields ) / sizeof( hudElemFields[0] );

	for (j = 0; j < i; ++j)
	{
		lc = 0;

		for (k = 0, field = hudElemFields; k < numFields; k++, field++)
		{
			if (*(int32_t *)((byte *)(from + j) + field->offset) != *(int32_t *)((byte *)(to + j) + field->offset))
			{
				lc = k;
			}
		}

		MSG_WriteBits(msg, lc, 5);

		for (l = 0, field = hudElemFields; l <= lc; l++, field++)
		{
			MSG_WriteDeltaField(msg, (byte *)(from + j), (byte *)(to + j), field);
		}
	}
}

void MSG_ReadDeltaHudElems(msg_t *msg, hudelem_t *from, hudelem_t *to, int count)
{
	int inuse, lc;
	int i, j, k;

	inuse = MSG_ReadBits(msg, 5);

	int numFields = sizeof( hudElemFields ) / sizeof( hudElemFields[0] );

	for ( i = 0; i < inuse; ++i )
	{
		lc = MSG_ReadBits(msg, 5);

		for ( j = 0; j <= lc; ++j )
		{
			MSG_ReadDeltaField(msg, &from[i], &to[i], &hudElemFields[j], 0);
		}

		for ( k = j; j < numFields; ++j, ++k )
		{
			*(int32_t *)((byte *)(from + i) + hudElemFields[k].offset) = *(int32_t *)((byte *)(to + i) + hudElemFields[k].offset);
		}
	}

	Com_Memset(&to[inuse], 0, (count - inuse) << 7);
}

#define PSF( x ) # x,(intptr_t)&( (playerState_t*)0 )->x
netField_t playerStateFields[] =
{
	{ PSF( commandTime ), 32},
	{ PSF( origin[1] ), 0},
	{ PSF( origin[0] ), 0},
	{ PSF( bobCycle ), 8},
	{ PSF( viewangles[1] ), -100},
	{ PSF( origin[2] ), 0},
	{ PSF( velocity[1] ), 0},
	{ PSF( velocity[0] ), 0},
	{ PSF( viewangles[0] ), -100},
	{ PSF( movementDir ), -8},
	{ PSF( velocity[2] ), 0},
	{ PSF( eventSequence ), 8},
	{ PSF( legsAnim ), 10},
	{ PSF( aimSpreadScale ), 0},
	{ PSF( weaponTime ), -16},
	{ PSF( pm_flags ), 27},
	{ PSF( events[0] ), 8},
	{ PSF( events[1] ), 8},
	{ PSF( events[2] ), 8},
	{ PSF( events[3] ), 8},
	{ PSF( weapAnim ), 10},
	{ PSF( viewHeightCurrent ), 0},
	{ PSF( torsoTimer ), 16},
	{ PSF( torsoAnim ), 10},
	{ PSF( eFlags ), 24},
	{ PSF( fWeaponPosFrac ), 0},
	{ PSF( holdBreathScale ), 0},
	{ PSF( weaponstate ), 5},
	{ PSF( viewHeightTarget ), -8},
	{ PSF( weaponDelay ), -16},
	{ PSF( legsTimer ), 16},
	{ PSF( viewHeightLerpTarget ), -8},
	{ PSF( groundEntityNum ), 10},
	{ PSF( pm_time ), -16},
	{ PSF( eventParms[3] ), 8},
	{ PSF( eventParms[1] ), 8},
	{ PSF( eventParms[0] ), 8},
	{ PSF( eventParms[2] ), 8},
	{ PSF( weapon ), 7},
	{ PSF( weapons[0] ), 32},
	{ PSF( viewHeightLerpDown ), 1},
	{ PSF( weaponslots[0] ), 32},
	{ PSF( delta_angles[0] ), 16},
	{ PSF( delta_angles[1] ), 16},
	{ PSF( cursorHintString ), -8},
	{ PSF( offHandIndex ), 7},
	{ PSF( clientNum ), 8},
	{ PSF( viewlocked_entNum ), 16},
	{ PSF( viewmodelIndex ), 8},
	{ PSF( viewHeightLerpTime ), 32},
	{ PSF( speed ), 16},
	{ PSF( mins[1] ), 0},
	{ PSF( mins[0] ), 0},
	{ PSF( maxs[2] ), 0},
	{ PSF( maxs[1] ), 0},
	{ PSF( maxs[0] ), 0},
	{ PSF( gravity ), 16},
	{ PSF( damageTimer ), 16},
	{ PSF( cursorHint ), 8},
	{ PSF( mantleState.flags ), 4},
	{ PSF( flinchYaw ), 16},
	{ PSF( fWaistPitch ), 0},
	{ PSF( mantleState.timer ), 32},
	{ PSF( fTorsoPitch ), 0},
	{ PSF( proneTorsoPitch ), 0},
	{ PSF( holdBreathTimer ), 16},
	{ PSF( jumpTime ), 32},
	{ PSF( viewangles[2] ), -100},
	{ PSF( foliageSoundTime ), 32},
	{ PSF( weapons[1] ), 32},
	{ PSF( damageEvent ), 8},
	{ PSF( damageDuration ), 16},
	{ PSF( damageYaw ), 8},
	{ PSF( proneDirection ), 0},
	{ PSF( proneDirectionPitch ), 0},
	{ PSF( mantleState.yaw ), 0},
	{ PSF( mantleState.transIndex ), 4},
	{ PSF( fTorsoHeight ), 0},
	{ PSF( damagePitch ), 8},
	{ PSF( jumpOriginZ ), 0},
	{ PSF( pm_type ), 8},
	{ PSF( viewlocked ), 8},
	{ PSF( weaponrechamber[0] ), 32},
	{ PSF( vLadderVec[0] ), 0},
	{ PSF( weaponslots[4] ), 32},
	{ PSF( weaponRestrictKickTime ), -16},
	{ PSF( vLadderVec[1] ), 0},
	{ PSF( viewAngleClampRange[1] ), 0},
	{ PSF( viewAngleClampRange[0] ), 0},
	{ PSF( viewAngleClampBase[1] ), 0},
	{ PSF( weaponrechamber[1] ), 32},
	{ PSF( leanf ), 0},
	{ PSF( damageCount ), 7},
	{ PSF( grenadeTimeLeft ), -16},
	{ PSF( deltaTime ), 32},
	{ PSF( shellshockTime ), 32},
	{ PSF( shellshockIndex ), 4},
	{ PSF( shellshockDuration ), 16},
	{ PSF( vLadderVec[2] ), 0},
	{ PSF( delta_angles[2] ), 16},
	{ PSF( viewHeightLerpPosAdj ), 0},
	{ PSF( mins[2] ), 0},
	{ PSF( viewAngleClampBase[0] ), 0},
	{ PSF( adsDelayTime ), 32},
	{ PSF( iCompassFriendInfo ), 32},
};

void MSG_WriteDeltaPlayerstate(msg_t *msg, playerState_t *from, playerState_t *to)
{
	int *fromF, *toF;
	netField_t *field;
	playerState_t nullstate;
	int i, j;
	int32_t absbits;
	uint8_t abs3bits;
	int32_t bitmask;
	float floatbits;
	int32_t signedbits;
	uint32_t unsignedbits;

	if ( !from )
	{
		from = &nullstate;
		Com_Memset(&nullstate, 0, sizeof(nullstate));
	}

	int numFields = sizeof( playerStateFields ) / sizeof( playerStateFields[0] );
	int lc = 0;

	for ( i = 0, field = playerStateFields ; i < numFields ; i++, field++ )
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );

		if ( *fromF != *toF )
		{
			lc = i + 1;
		}
	}

	MSG_WriteByte( msg, lc );

	for ( i = 0, field = playerStateFields ; i < lc ; i++, field++ )
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );

		floatbits = *(float *)toF;
		signedbits = *(int32_t *)toF;
		unsignedbits = *(uint32_t *)toF;

		if ( *fromF == *toF )
		{
			MSG_WriteBit0(msg);
			continue;
		}

		MSG_WriteBit1(msg);

		switch (field->bits)
		{
		case 0:
			if ( (float)signedbits == floatbits && (unsigned int)(signedbits + 4096) <= 0x1FFF )
			{
				MSG_WriteBit0(msg);
				MSG_WriteBits(msg, signedbits + 4096, 5);
				MSG_WriteByte(msg, (signedbits + 4096) >> 5);
			}
			else
			{
				MSG_WriteBit1(msg);
				MSG_WriteLong(msg, unsignedbits);
			}
			break;

		case -100:
			if ( unsignedbits )
			{
				MSG_WriteBit1(msg);
				MSG_WriteAngle16(msg, floatbits);
			}
			else
			{
				MSG_WriteBit0(msg);
			}
			break;

		default:
			absbits = abs(field->bits);
			bitmask = unsignedbits;
			abs3bits = absbits & 7;
			if ( abs3bits )
			{
				MSG_WriteBits(msg, bitmask, absbits & 7);
				absbits -= abs3bits;
				bitmask >>= abs3bits;
			}
			while( absbits )
			{
				MSG_WriteByte(msg, bitmask);
				bitmask >>= 8;
				absbits -= 8;
			}
			break;
		}
	}

	// stats
	int statsbits = 0;

	for ( i = 0; i < MAX_STATS; ++i )
	{
		if ( to->stats[i] != from->stats[i] )
			statsbits |= 1 << i;
	}

	if ( statsbits )
	{
		MSG_WriteBit1(msg);
		MSG_WriteBits(msg, statsbits, 6);

		if ( (statsbits & 1) != 0 )
			MSG_WriteShort(msg, to->stats[0]);
		if ( (statsbits & 2) != 0 )
			MSG_WriteShort(msg, to->stats[1]);
		if ( (statsbits & 4) != 0 )
			MSG_WriteShort(msg, to->stats[2]);
		if ( (statsbits & 8) != 0 )
			MSG_WriteBits(msg, to->stats[3], 6);
		if ( (statsbits & 0x10) != 0 )
			MSG_WriteShort(msg, to->stats[4]);
		if ( (statsbits & 0x20) != 0 )
			MSG_WriteByte(msg, to->stats[5]);
	}
	else
	{
		MSG_WriteBit0(msg);
	}

	// ammo stored
	int ammobits[8];

	for ( j = 0; j < 4; j++ )
	{
		ammobits[j] = 0;

		for ( i = 0 ; i < 16 ; i++ )
		{
			if ( to->ammo[i + ( j * 16 )] != from->ammo[i + ( j * 16 )] )
			{
				ammobits[j] |= 1 << i;
			}
		}
	}

	if ( ammobits[0] || ammobits[1] || ammobits[2] || ammobits[3] )
	{
		MSG_WriteBit1( msg );

		for ( j = 0; j < 4; j++ )
		{
			if ( ammobits[j] )
			{
				MSG_WriteBit1( msg );
				MSG_WriteShort( msg, ammobits[j] );
				for ( i = 0 ; i < 16 ; i++ )
				{
					if ( ammobits[j] & ( 1 << i ) )
					{
						MSG_WriteShort( msg, to->ammo[i + ( j * 16 )] );
					}
				}
			}
			else
			{
				MSG_WriteBit0( msg );
			}
		}
	}
	else
	{
		MSG_WriteBit0( msg );
	}

	// ammo in clip
	int clipbits;

	for ( j = 0; j < 4; j++ )
	{
		clipbits = 0;

		for ( i = 0 ; i < 16 ; i++ )
		{
			if ( to->ammoclip[i + ( j * 16 )] != from->ammoclip[i + ( j * 16 )] )
			{
				clipbits |= 1 << i;
			}
		}

		if ( clipbits )
		{
			MSG_WriteBit1( msg );
			MSG_WriteShort( msg, clipbits );

			for ( i = 0 ; i < 16 ; i++ )
			{
				if ( clipbits & ( 1 << i ) )
				{
					MSG_WriteShort( msg, to->ammoclip[i + ( j * 16 )] );
				}
			}
		}
		else
		{
			MSG_WriteBit0( msg );
		}
	}

	// Objectives
	int numObjectiveFields = sizeof( objectiveFields ) / sizeof( objectiveFields[0] );

	if ( !Com_Memcmp(from->objective, to->objective, sizeof(from->objective)) )
	{
		MSG_WriteBit0(msg);
	}
	else
	{
		MSG_WriteBit1(msg);

		for ( i = 0; i < MAX_OBJECTIVES; ++i )
		{
			MSG_WriteBits(msg, to->objective[i].state, 3);
			MSG_WriteDeltaObjective(msg, &from->objective[i], &to->objective[i], 0, numObjectiveFields, objectiveFields);
		}
	}

	// Hud-elements
	if ( !Com_Memcmp(&from->hud, &to->hud, sizeof(from->hud)) )
	{
		MSG_WriteBit0(msg);
	}
	else
	{
		MSG_WriteBit1(msg);
		MSG_WriteDeltaHudElems(msg, from->hud.archival, to->hud.archival, MAX_HUDELEMS_ARCHIVAL);
		MSG_WriteDeltaHudElems(msg, from->hud.current, to->hud.current, MAX_HUDELEMS_CURRENT);
	}
}

void MSG_ReadDeltaPlayerstate(msg_t *msg, playerState_t *from, playerState_t *to)
{
	int *fromF, *toF;
	netField_t *field;
	playerState_t nullstate;
	int readbits;
	int readbyte;
	int bitstemp;
	uint32_t unsignedbits;
	int bitcount;
	int i, j;

	if ( !from )
	{
		from = &nullstate;
		Com_Memset(&nullstate, 0, sizeof(nullstate));
	}

	Com_Memcpy(to, from, sizeof(playerState_t));

	int numFields = sizeof( playerStateFields ) / sizeof( playerStateFields[0] );
	int lc = 0;

	qboolean print = qfalse; // only present in client aka cl_shownet

	lc = MSG_ReadByte( msg );

	for ( i = 0, field = playerStateFields ; i < lc ; i++, field++ )
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );

		if ( !MSG_ReadBit(msg) )
		{
			*toF = *fromF;
			continue;
		}

		switch (field->bits)
		{
		case 0:
			if ( MSG_ReadBit(msg) )
			{
				*(uint32_t *)toF = MSG_ReadLong(msg);
				if ( print )
					Com_Printf("%s:%f ", field->name, *(float *)toF);
			}
			else
			{
				readbits = MSG_ReadBits(msg, 5);
				readbyte = 32 * MSG_ReadByte(msg) + readbits - 4096;
				*toF = readbyte;
				if ( print )
					Com_Printf("%s:%i ", field->name, readbyte);
			}
			break;

		case -100:
			if ( MSG_ReadBit(msg) )
				*(float *)toF = MSG_ReadAngle16(msg);
			else
				*toF = 0;
			break;

		default:
			unsignedbits = (unsigned int)field->bits >> 31;
			readbits = abs(field->bits);
			if ( (readbits & 7) != 0 )
				bitstemp = MSG_ReadBits(msg, readbits & 7);
			else
				bitstemp = 0;
			readbyte = bitstemp;
			for ( bitcount = readbits & 7; bitcount < readbits; bitcount += 8 )
				readbyte |= MSG_ReadByte(msg) << bitcount;
			if ( unsignedbits && ((readbyte >> (readbits - 1)) & 1) != 0 )
				readbyte |= ~((1 << readbits) - 1);
			*(uint32_t *)toF = readbyte;
			if ( print )
				Com_Printf("%s:%i ", field->name, *(uint32_t *)toF);
			break;
		}
	}

	for ( i = lc, field = &playerStateFields[lc]; i < numFields; i++, field++ )
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );
		*toF = *fromF;
	}

	// stats
	int statsbits = 0;

	if ( MSG_ReadBit(msg) )
	{
		statsbits = MSG_ReadBits(msg, 6);
		if ( (statsbits & 1) != 0 )
			to->stats[0] = MSG_ReadShort(msg);
		if ( (statsbits & 2) != 0 )
			to->stats[1] = MSG_ReadShort(msg);
		if ( (statsbits & 4) != 0 )
			to->stats[2] = MSG_ReadShort(msg);
		if ( (statsbits & 8) != 0 )
			to->stats[3] = MSG_ReadBits(msg, 6);
		if ( (statsbits & 0x10) != 0 )
			to->stats[4] = MSG_ReadShort(msg);
		if ( (statsbits & 0x20) != 0 )
			to->stats[5] = MSG_ReadByte(msg);
	}

	//ammo
	int ammobits = 0;
	int ammocount = 0;

	if ( MSG_ReadBit(msg) )
	{
		for ( i = 0; i < 4; ++i )
		{
			if ( MSG_ReadBit(msg) )
			{
				ammobits = MSG_ReadShort(msg);

				for ( j = 0; j < 16; ++j )
				{
					if ( ((ammobits >> j) & 1) != 0 )
					{
						ammocount = j + 16 * i;
						to->ammo[ammocount] = MSG_ReadShort(msg);
					}
				}
			}
		}
	}

	// ammo in clip
	int clipbits = 0;
	int clipcount = 0;

	for ( i = 0; i < 4; ++i )
	{
		if ( MSG_ReadBit(msg) )
		{
			clipbits = MSG_ReadShort(msg);

			for ( j = 0; j < 16; ++j )
			{
				if ( ((clipbits >> j) & 1) != 0 )
				{
					clipcount = j + 16 * i;
					to->ammoclip[clipcount] = MSG_ReadShort(msg);
				}
			}
		}
	}

	// Objectives
	int numObjectiveFields = sizeof( objectiveFields ) / sizeof( objectiveFields[0] );

	if ( MSG_ReadBit(msg) )
	{
		for ( i = 0; i < MAX_OBJECTIVES; ++i )
		{
			to->objective[i].state = MSG_ReadBits(msg, 3);
			MSG_ReadDeltaObjective(msg, &from->objective[i], &to->objective[i], numObjectiveFields, objectiveFields);
		}
	}

	// Hud-elements
	if ( MSG_ReadBit(msg) )
	{
		MSG_ReadDeltaHudElems(msg, from->hud.archival, to->hud.archival, MAX_HUDELEMS_ARCHIVAL);
		MSG_ReadDeltaHudElems(msg, from->hud.current, to->hud.current, MAX_HUDELEMS_CURRENT);
	}
}