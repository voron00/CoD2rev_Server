#include "qcommon.h"

static huffman_t msgHuff;
static qboolean msgInit = qfalse;

// Q3 TA freq. table.
static const int msg_hData[256] =
{
	250315,			// 0
	41193,			// 1
	6292,			// 2
	7106,			// 3
	3730,			// 4
	3750,			// 5
	6110,			// 6
	23283,			// 7
	33317,			// 8
	6950,			// 9
	7838,			// 10
	9714,			// 11
	9257,			// 12
	17259,			// 13
	3949,			// 14
	1778,			// 15
	8288,			// 16
	1604,			// 17
	1590,			// 18
	1663,			// 19
	1100,			// 20
	1213,			// 21
	1238,			// 22
	1134,			// 23
	1749,			// 24
	1059,			// 25
	1246,			// 26
	1149,			// 27
	1273,			// 28
	4486,			// 29
	2805,			// 30
	3472,			// 31
	21819,			// 32
	1159,			// 33
	1670,			// 34
	1066,			// 35
	1043,			// 36
	1012,			// 37
	1053,			// 38
	1070,			// 39
	1726,			// 40
	888,			// 41
	1180,			// 42
	850,			// 43
	960,			// 44
	780,			// 45
	1752,			// 46
	3296,			// 47
	10630,			// 48
	4514,			// 49
	5881,			// 50
	2685,			// 51
	4650,			// 52
	3837,			// 53
	2093,			// 54
	1867,			// 55
	2584,			// 56
	1949,			// 57
	1972,			// 58
	940,			// 59
	1134,			// 60
	1788,			// 61
	1670,			// 62
	1206,			// 63
	5719,			// 64
	6128,			// 65
	7222,			// 66
	6654,			// 67
	3710,			// 68
	3795,			// 69
	1492,			// 70
	1524,			// 71
	2215,			// 72
	1140,			// 73
	1355,			// 74
	971,			// 75
	2180,			// 76
	1248,			// 77
	1328,			// 78
	1195,			// 79
	1770,			// 80
	1078,			// 81
	1264,			// 82
	1266,			// 83
	1168,			// 84
	965,			// 85
	1155,			// 86
	1186,			// 87
	1347,			// 88
	1228,			// 89
	1529,			// 90
	1600,			// 91
	2617,			// 92
	2048,			// 93
	2546,			// 94
	3275,			// 95
	2410,			// 96
	3585,			// 97
	2504,			// 98
	2800,			// 99
	2675,			// 100
	6146,			// 101
	3663,			// 102
	2840,			// 103
	14253,			// 104
	3164,			// 105
	2221,			// 106
	1687,			// 107
	3208,			// 108
	2739,			// 109
	3512,			// 110
	4796,			// 111
	4091,			// 112
	3515,			// 113
	5288,			// 114
	4016,			// 115
	7937,			// 116
	6031,			// 117
	5360,			// 118
	3924,			// 119
	4892,			// 120
	3743,			// 121
	4566,			// 122
	4807,			// 123
	5852,			// 124
	6400,			// 125
	6225,			// 126
	8291,			// 127
	23243,			// 128
	7838,			// 129
	7073,			// 130
	8935,			// 131
	5437,			// 132
	4483,			// 133
	3641,			// 134
	5256,			// 135
	5312,			// 136
	5328,			// 137
	5370,			// 138
	3492,			// 139
	2458,			// 140
	1694,			// 141
	1821,			// 142
	2121,			// 143
	1916,			// 144
	1149,			// 145
	1516,			// 146
	1367,			// 147
	1236,			// 148
	1029,			// 149
	1258,			// 150
	1104,			// 151
	1245,			// 152
	1006,			// 153
	1149,			// 154
	1025,			// 155
	1241,			// 156
	952,			// 157
	1287,			// 158
	997,			// 159
	1713,			// 160
	1009,			// 161
	1187,			// 162
	879,			// 163
	1099,			// 164
	929,			// 165
	1078,			// 166
	951,			// 167
	1656,			// 168
	930,			// 169
	1153,			// 170
	1030,			// 171
	1262,			// 172
	1062,			// 173
	1214,			// 174
	1060,			// 175
	1621,			// 176
	930,			// 177
	1106,			// 178
	912,			// 179
	1034,			// 180
	892,			// 181
	1158,			// 182
	990,			// 183
	1175,			// 184
	850,			// 185
	1121,			// 186
	903,			// 187
	1087,			// 188
	920,			// 189
	1144,			// 190
	1056,			// 191
	3462,			// 192
	2240,			// 193
	4397,			// 194
	12136,			// 195
	7758,			// 196
	1345,			// 197
	1307,			// 198
	3278,			// 199
	1950,			// 200
	886,			// 201
	1023,			// 202
	1112,			// 203
	1077,			// 204
	1042,			// 205
	1061,			// 206
	1071,			// 207
	1484,			// 208
	1001,			// 209
	1096,			// 210
	915,			// 211
	1052,			// 212
	995,			// 213
	1070,			// 214
	876,			// 215
	1111,			// 216
	851,			// 217
	1059,			// 218
	805,			// 219
	1112,			// 220
	923,			// 221
	1103,			// 222
	817,			// 223
	1899,			// 224
	1872,			// 225
	976,			// 226
	841,			// 227
	1127,			// 228
	956,			// 229
	1159,			// 230
	950,			// 231
	7791,			// 232
	954,			// 233
	1289,			// 234
	933,			// 235
	1127,			// 236
	3207,			// 237
	1020,			// 238
	927,			// 239
	1355,			// 240
	768,			// 241
	1040,			// 242
	745,			// 243
	952,			// 244
	805,			// 245
	1073,			// 246
	740,			// 247
	1013,			// 248
	805,			// 249
	1008,			// 250
	796,			// 251
	996,			// 252
	1057,			// 253
	11457,			// 254
	13504,			// 255
};

void MSG_InitHuffmanInternal()
{
	int i,j;

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

void MSG_InitHuffman()
{
	msgInit = qtrue;
	MSG_InitHuffmanInternal();
}

void MSG_Init(msg_t *buf, byte *data, int length)
{
	if ( !msgInit )
	{
		MSG_InitHuffman();
	}

	Com_Memset(buf, 0, sizeof(msg_t));
	buf->data = data;
	buf->maxsize = length;
}

void MSG_BeginReading(msg_t *msg)
{
	msg->overflowed = qfalse;
	msg->readcount = 0;
	msg->bit = 0;
}

void MSG_WriteBits(msg_t *msg, int value, unsigned int bits)
{
	int bit;

	if ( msg->maxsize - msg->cursize < 4 )
	{
		msg->overflowed = qtrue;
		return;
	}

	while ( bits )
	{
		--bits;
		bit = msg->bit & 7;

		if ( !bit )
		{
			msg->bit = 8 * msg->cursize;
			msg->data[msg->cursize++] = 0;
		}

		if ( (value & 1) != 0 )
			msg->data[msg->bit >> 3] |= 1 << bit;

		++msg->bit;
		value >>= 1;
	}
}

void MSG_WriteBit0(msg_t* msg)
{
	if (msg->maxsize <= msg->cursize)
	{
		msg->overflowed = qtrue;
		return;
	}

	if (!(msg->bit & 7))
	{
		msg->bit = msg->cursize * 8;
		msg->data[msg->cursize] = 0;
		msg->cursize ++;
	}

	msg->bit++;
}

void MSG_WriteBit1(msg_t *msg)
{
	int bit;

	if (msg->maxsize <= msg->cursize)
	{
		msg->overflowed = qtrue;
		return;
	}

	bit = msg->bit & 7;

	if ( !bit )
	{
		msg->bit = 8 * msg->cursize;
		msg->data[msg->cursize++] = 0;
	}

	msg->data[msg->bit++ >> 3] |= 1 << bit;
}

int MSG_ReadBits(msg_t *msg, int bits)
{
	int i;
	int bit;
	int value;

	value = 0;

	for ( i = 0; i < bits; ++i )
	{
		bit = msg->bit & 7;

		if ( !bit )
		{
			if ( msg->readcount >= msg->cursize )
			{
				msg->overflowed = qtrue;
				return -1;
			}

			msg->bit = 8 * msg->readcount++;
		}

		value |= ((msg->data[msg->bit++ >> 3] >> bit) & 1) << i;
	}

	return value;
}

int MSG_ReadBit(msg_t *msg)
{
	int bit;

	bit = msg->bit & 7;

	if ( !bit )
	{
		if ( msg->readcount >= msg->cursize )
		{
			msg->overflowed = qtrue;
			return -1;
		}

		msg->bit = 8 * msg->readcount++;
	}

	return (msg->data[msg->bit++ >> 3] >> bit) & 1;
}

int MSG_WriteBitsCompress(const byte *datasrc, byte *buffdest, int bytecount)
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
	int newsize = buf->cursize + length;

	if ( newsize > buf->maxsize )
	{
		buf->overflowed = qtrue;
	}
	else
	{
		Com_Memcpy(&buf->data[buf->cursize], data, length);
		buf->cursize = newsize;
	}
}

void MSG_WriteByte(msg_t *msg, int c)
{
	int newsize = msg->cursize + sizeof(uint8_t);

	if ( newsize > msg->maxsize )
	{
		msg->overflowed = qtrue;
	}
	else
	{
		*(uint8_t *)&msg->data[msg->cursize] = c;
		msg->cursize = newsize;
	}
}

void MSG_WriteShort(msg_t *msg, int c)
{
	int newsize = msg->cursize + sizeof(int16_t);

	if ( newsize > msg->maxsize )
	{
		msg->overflowed = qtrue;
	}
	else
	{
		*(int16_t *)&msg->data[msg->cursize] = c;
		msg->cursize = newsize;
	}
}

void MSG_WriteLong(msg_t *msg, int c)
{
	int newsize = msg->cursize + sizeof(int32_t);

	if ( newsize > msg->maxsize )
	{
		msg->overflowed = qtrue;
	}
	else
	{
		*(int32_t *)&msg->data[msg->cursize] = c;
		msg->cursize = newsize;
	}
}

void MSG_WriteString(msg_t *sb, const char *s)
{
	int i;
	int l;
	char string[MAX_STRING_CHARS];

	l = strlen( s );

	if ( l >= MAX_STRING_CHARS )
	{
		Com_Printf("MSG_WriteString: MAX_STRING_CHARS" );
		MSG_WriteData( sb, "", 1 );
		return;
	}

	for ( i = 0; i < l; ++i )
	{
		string[i] = I_CleanChar(s[i]);
	}

	string[i] = 0;
	MSG_WriteData(sb, string, l + 1);
}

void MSG_WriteBigString(msg_t *sb, const char *s)
{
	int i;
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

	for ( i = 0; i < l; ++i )
	{
		string[i] = I_CleanChar(s[i]);
	}

	MSG_WriteData(sb, string, l + 1);
}

int MSG_ReadByte(msg_t *msg)
{
	int newcount = msg->readcount + sizeof(uint8_t);

	if ( newcount > msg->cursize )
	{
		msg->overflowed = qtrue;
		return -1;
	}

	uint8_t c = *(uint8_t *)&msg->data[msg->readcount];
	msg->readcount = newcount;

	return c;
}

int MSG_ReadShort(msg_t *msg)
{
	int newcount = msg->readcount + sizeof(int16_t);

	if ( newcount > msg->cursize )
	{
		msg->overflowed = qtrue;
		return -1;
	}

	int16_t c = *(int16_t *)&msg->data[msg->readcount];
	msg->readcount = newcount;

	return c;
}

int MSG_ReadLong(msg_t *msg)
{
	int newcount = msg->readcount + sizeof(int32_t);

	if ( newcount > msg->cursize )
	{
		msg->overflowed = qtrue;
		return -1;
	}

	int32_t c = *(int32_t *)&msg->data[msg->readcount];
	msg->readcount = newcount;

	return c;
}

void MSG_ReadData(msg_t *msg, void *data, int len)
{
	int size = msg->readcount + len;

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

char *MSG_ReadString(msg_t *msg, char *string, unsigned int maxChars)
{
	int c;
	unsigned int l;

	for ( l = 0; ; ++l )
	{
		c = MSG_ReadByte(msg);

		if ( c == -1 )
			c = 0;

		if ( l < maxChars )
			string[l] = I_CleanChar(c);

		if ( !c )
			break;
	}

	string[maxChars - 1] = 0;
	return string;
}

char *MSG_ReadStringLine(msg_t *msg, char *string, unsigned int maxChars)
{
	int c;
	unsigned int l;

	for ( l = 0; ; ++l )
	{
		c = MSG_ReadByte(msg);

		if ( c == 37 )
		{
			c = 46;
		}
		else if ( c == 10 || c == -1 )
		{
			c = 0;
		}

		if ( l < maxChars )
			string[l] = I_CleanChar(c);

		if ( !c )
			break;
	}

	string[maxChars - 1] = 0;
	return string;
}

void MSG_WriteReliableCommandToBuffer(const char *pszCommand, char *pszBuffer, int iBufferSize)
{
	int iCommandLength;
	int i;

	iCommandLength = strlen(pszCommand);

	if ( iCommandLength >= iBufferSize )
		Com_Printf(
		    "WARNING: Reliable command is too long (%i/%i) and will be truncated: '%s'\n",
		    iCommandLength,
		    iBufferSize,
		    pszCommand);

	if ( !iCommandLength )
		Com_Printf("WARNING: Empty reliable command\n");

	for ( i = 0; i < iBufferSize && pszCommand[i]; ++i )
	{
		pszBuffer[i] = I_CleanChar(pszCommand[i]);

		if ( pszBuffer[i] == 37 )
			pszBuffer[i] = 46;
	}

	if ( i >= iBufferSize )
		pszBuffer[iBufferSize - 1] = 0;
	else
		pszBuffer[i] = 0;
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

uint32_t kbitmask[] =
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
	4294967295
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

int MSG_GetForwardRightMoveMask(char forwardmove, char rightmove)
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

void MSG_ReadForwardRightMove(char key, char *forwardmove, char *rightmove)
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
			mask = MSG_GetForwardRightMoveMask(from->forwardmove, from->rightmove);
			deltakey = MSG_ReadDeltaKey(msg, key, mask, 4);
			MSG_ReadForwardRightMove(deltakey, &to->forwardmove, &to->rightmove);
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
			mask = MSG_GetForwardRightMoveMask(from->forwardmove, from->rightmove);
			deltakey = MSG_ReadDeltaKey(msg, newkey, mask, 4);
			MSG_ReadForwardRightMove(deltakey, &to->forwardmove, &to->rightmove);
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
	int32_t abs3bits;
	int32_t bitmask;
	float floatbits;
	int32_t signedbits;
	uint32_t unsignedbits;

	if ( *(uint32_t *)&from[field->offset] == *(uint32_t *)&to[field->offset] )
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
			MSG_WriteBits(msg, (signedbits + 4096), 5);
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
			MSG_WriteBits(msg, (signedbits + 512), 2);
			MSG_WriteByte(msg, (signedbits + 512) >> 2);
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
				MSG_WriteBits(msg, bitmask, abs3bits);
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
	MSG_WriteDeltaStruct(msg, (byte *)from, (byte *)to, force, COUNT_OF(entityStateFields), 10, entityStateFields, 0);
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
	MSG_WriteDeltaStruct(msg, (byte *)from, (byte *)to, flags, COUNT_OF(archivedEntityFields), 10, archivedEntityFields, 0);
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

	MSG_WriteDeltaStruct(msg, (byte *)from, (byte *)to, force, COUNT_OF(clientStateFields), 6, clientStateFields, 1);
}

float MSG_ReadAngle16(msg_t *msg)
{
	return SHORT2ANGLE((float)MSG_ReadShort(msg));
}

void MSG_ReadDeltaField(msg_t *msg, const void *from, const void *to, netField_t *field, qboolean print)
{
	int *fromF, *toF;
	int32_t readbits;
	int32_t readbyte;
	uint32_t unsignedbits;
	int i;

	fromF = (int32_t *)((byte *)from + field->offset);
	toF = (int32_t *)((byte *)to + field->offset);

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
				*toF = MSG_ReadLong(msg);
				if ( print )
					Com_Printf("%s:%f ", field->name, *(float *)toF);
			}
			else
			{
				readbits = MSG_ReadBits(msg, 5);
				readbyte = 32 * MSG_ReadByte(msg) + readbits - 4096;
				*(float *)toF = (float)readbyte;
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
				*toF = MSG_ReadLong(msg);
				if ( print )
					Com_Printf("%s:%f ", field->name, *(float *)toF);
			}
			else
			{
				readbits = MSG_ReadBits(msg, 2);
				readbyte = 4 * MSG_ReadByte(msg) + readbits - 512;
				*(float *)toF = (float)readbyte;
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
				readbyte = MSG_ReadBits(msg, readbits & 7);
			else
				readbyte = 0;
			for ( i = readbits & 7; i < readbits; i += 8 )
				readbyte |= MSG_ReadByte(msg) << i;
			if ( unsignedbits && ((readbyte >> (readbits - 1)) & 1) != 0 )
				readbyte |= ~((1 << readbits) - 1);
			*toF = readbyte;
			if ( print )
				Com_Printf("%s:%i ", field->name, *toF);
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

	if ( MSG_ReadBit(msg) == 1 )
		return;

	if ( !MSG_ReadBit(msg) )
	{
		//Com_Memcpy(to, from, 4 * numFields + 4);
		Com_Memcpy(to, from, ( numFields + sizeof( intptr_t ) ) * sizeof( from ));
		return;
	}

	lc = MSG_ReadByte(msg);

	if ( lc > numFields )
	{
		msg->overflowed = qtrue;
		return;
	}

	*(uint32_t *)to = number;

	for (i = 0, field = stateFields; i < lc; i++, field++)
	{
		MSG_ReadDeltaField(msg, from, to, field, 0);
	}

	for (i = lc, field = &stateFields[lc]; i < numFields; i++, field++)
	{
		*(uint32_t *)((byte *)to + field->offset) = *(uint32_t *)((byte *)from + field->offset);
	}
}

void MSG_ReadDeltaEntity(msg_t *msg, entityState_t *from, entityState_t *to, int number)
{
	MSG_ReadDeltaStruct(msg, from, to, number, COUNT_OF(entityStateFields), 10, entityStateFields);
}

void MSG_ReadDeltaArchivedEntity(msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, int number)
{
	MSG_ReadDeltaStruct(msg, from, to, number, COUNT_OF(archivedEntityFields), 10, archivedEntityFields);
}

void MSG_ReadDeltaClient(msg_t *msg, clientState_t *from, clientState_t *to, int number)
{
	clientState_t nullstate;

	if ( !from )
	{
		from = &nullstate;
		Com_Memset(&nullstate, 0, sizeof(nullstate));
	}

	MSG_ReadDeltaStruct(msg, from, to, number, COUNT_OF(clientStateFields), 6, clientStateFields);
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

void MSG_WriteDeltaHudElems(msg_t *msg, hudelem_s *from, hudelem_s *to, int count)
{
	int i, inuse, lc, j;
	netField_t* field;

	for(inuse = 0; inuse < MAX_HUDELEMENTS; ++inuse)
	{
		if(to[inuse].type == HE_TYPE_FREE)
		{
			break;
		}
	}

	MSG_WriteBits(msg, inuse, 5); //31 HE MAX for 5 Bits

	for ( i = 0; i < inuse; ++i )
	{
		lc = 0;

		for (j = 0, field = hudElemFields; j < COUNT_OF(hudElemFields); j++, field++)
		{
			if (*(int32_t *)((byte *)(from + i) + field->offset) != *(int32_t *)((byte *)(to + i) + field->offset))
			{
				lc = j;
			}
		}

		MSG_WriteBits(msg, lc, 5);

		for (j = 0, field = hudElemFields; j <= lc; j++, field++)
		{
			MSG_WriteDeltaField(msg, (byte *)(from + i), (byte *)(to + i), field);
		}
	}
}

void MSG_ReadDeltaHudElems(msg_t *msg, hudelem_t *from, hudelem_t *to, int count)
{
	int inuse, lc, i, j, k;

	inuse = MSG_ReadBits(msg, 5);

	for ( i = 0; i < inuse; ++i )
	{
		lc = MSG_ReadBits(msg, 5);

		for ( j = 0; j <= lc; ++j )
		{
			MSG_ReadDeltaField(msg, &from[i], &to[i], &hudElemFields[j], 0);
		}

		for ( k = j; j < COUNT_OF(hudElemFields); ++j, ++k )
		{
			*(int32_t *)((byte *)(from + i) + hudElemFields[k].offset) = *(int32_t *)((byte *)(to + i) + hudElemFields[k].offset);
		}
	}

	while ( inuse < count && to[inuse].type )
	{
		memset(&to[inuse], 0, sizeof(hudelem_t));
		assert(to[inuse].type == HE_TYPE_FREE);
		++inuse;
	}
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
	int i, j, lc;
	int32_t absbits;
	int32_t abs3bits;
	int32_t bitmask;
	float floatbits;
	int32_t signedbits;
	uint32_t unsignedbits;

	if ( !from )
	{
		from = &nullstate;
		Com_Memset(&nullstate, 0, sizeof(nullstate));
	}

	lc = 0;

	for ( i = 0, field = playerStateFields; i < COUNT_OF(playerStateFields); i++, field++ )
	{
		fromF = ( int32_t * )( (byte *)from + field->offset );
		toF = ( int32_t * )( (byte *)to + field->offset );

		if ( *fromF != *toF )
		{
			lc = i + 1;
		}
	}

	MSG_WriteByte( msg, lc );

	for ( i = 0, field = playerStateFields; i < lc; i++, field++ )
	{
		fromF = ( int32_t * )( (byte *)from + field->offset );
		toF = ( int32_t * )( (byte *)to + field->offset );

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
				MSG_WriteBits(msg, (signedbits + 4096), 5);
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
				MSG_WriteBits(msg, bitmask, abs3bits);
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
			MSG_WriteShort(msg, to->stats[STAT_HEALTH]);

		if ( (statsbits & 2) != 0 )
			MSG_WriteShort(msg, to->stats[STAT_DEAD_YAW]);

		if ( (statsbits & 4) != 0 )
			MSG_WriteShort(msg, to->stats[STAT_MAX_HEALTH]);

		if ( (statsbits & 8) != 0 )
			MSG_WriteBits(msg, to->stats[STAT_IDENT_CLIENT_NUM], 6);

		if ( (statsbits & 0x10) != 0 )
			MSG_WriteShort(msg, to->stats[STAT_IDENT_CLIENT_HEALTH]);

		if ( (statsbits & 0x20) != 0 )
			MSG_WriteByte(msg, to->stats[STAT_SPAWN_COUNT]);
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
			MSG_WriteDeltaObjective(msg, &from->objective[i], &to->objective[i], 0, COUNT_OF(objectiveFields), objectiveFields);
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
	int i, j, k, lc;

	if ( !from )
	{
		from = &nullstate;
		Com_Memset(&nullstate, 0, sizeof(nullstate));
	}

	Com_Memcpy(to, from, sizeof(playerState_t));

	lc = 0;

	qboolean print = qfalse; // only present in client aka cl_shownet

	lc = MSG_ReadByte( msg );

	for ( i = 0, field = playerStateFields ; i < lc ; i++, field++ )
	{
		fromF = ( int32_t * )( (byte *)from + field->offset );
		toF = ( int32_t * )( (byte *)to + field->offset );

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
				*toF = MSG_ReadLong(msg);
				if ( print )
					Com_Printf("%s:%f ", field->name, *(float *)toF);
			}
			else
			{
				readbits = MSG_ReadBits(msg, 5);
				readbyte = 32 * MSG_ReadByte(msg) + readbits - 4096;
				*(float *)toF = (float)readbyte;
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
				readbyte = MSG_ReadBits(msg, readbits & 7);
			else
				readbyte = 0;
			for ( k = readbits & 7; k < readbits; k += 8 )
				readbyte |= MSG_ReadByte(msg) << k;
			if ( unsignedbits && ((readbyte >> (readbits - 1)) & 1) != 0 )
				readbyte |= ~((1 << readbits) - 1);
			*toF = readbyte;
			if ( print )
				Com_Printf("%s:%i ", field->name, *toF);
			break;
		}
	}

	for ( i = lc, field = &playerStateFields[lc]; i < COUNT_OF(playerStateFields); i++, field++ )
	{
		fromF = ( int32_t * )( (byte *)from + field->offset );
		toF = ( int32_t * )( (byte *)to + field->offset );

		*toF = *fromF;
	}

	// stats
	int statsbits = 0;

	if ( MSG_ReadBit(msg) )
	{
		statsbits = MSG_ReadBits(msg, 6);

		if ( (statsbits & 1) != 0 )
			to->stats[STAT_HEALTH] = MSG_ReadShort(msg);

		if ( (statsbits & 2) != 0 )
			to->stats[STAT_DEAD_YAW] = MSG_ReadShort(msg);

		if ( (statsbits & 4) != 0 )
			to->stats[STAT_MAX_HEALTH] = MSG_ReadShort(msg);

		if ( (statsbits & 8) != 0 )
			to->stats[STAT_IDENT_CLIENT_NUM] = MSG_ReadBits(msg, 6);

		if ( (statsbits & 0x10) != 0 )
			to->stats[STAT_IDENT_CLIENT_HEALTH] = MSG_ReadShort(msg);

		if ( (statsbits & 0x20) != 0 )
			to->stats[STAT_SPAWN_COUNT] = MSG_ReadByte(msg);
	}

	//ammo
	int ammobits = 0;

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
						to->ammo[j + 16 * i] = MSG_ReadShort(msg);
					}
				}
			}
		}
	}

	// ammo in clip
	int clipbits = 0;

	for ( i = 0; i < 4; ++i )
	{
		if ( MSG_ReadBit(msg) )
		{
			clipbits = MSG_ReadShort(msg);

			for ( j = 0; j < 16; ++j )
			{
				if ( ((clipbits >> j) & 1) != 0 )
				{
					to->ammoclip[j + 16 * i] = MSG_ReadShort(msg);
				}
			}
		}
	}

	// Objectives
	if ( MSG_ReadBit(msg) )
	{
		for ( i = 0; i < MAX_OBJECTIVES; ++i )
		{
			to->objective[i].state = MSG_ReadBits(msg, 3);
			MSG_ReadDeltaObjective(msg, &from->objective[i], &to->objective[i], COUNT_OF(objectiveFields), objectiveFields);
		}
	}

	// Hud-elements
	if ( MSG_ReadBit(msg) )
	{
		MSG_ReadDeltaHudElems(msg, from->hud.archival, to->hud.archival, MAX_HUDELEMS_ARCHIVAL);
		MSG_ReadDeltaHudElems(msg, from->hud.current, to->hud.current, MAX_HUDELEMS_CURRENT);
	}
}