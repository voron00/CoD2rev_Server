#include "qcommon.h"
#include "sys_thread.h"

static huffman_t msgHuff;
static qboolean msgInit = qfalse;

/*
==============
MSG_SetDefaultUserCmd
==============
*/
void MSG_SetDefaultUserCmd( playerState_t *ps, usercmd_t *ucmd )
{
	int i;

	Com_Memset(ucmd, 0, sizeof(usercmd_t));

	ucmd->weapon = ps->weapon;
	ucmd->offHandIndex = ps->offHandIndex;

	for( i = 0; i < 2; i++ )
	{
		ucmd->angles[i] = ANGLE2SHORT( ps->viewangles[i] ) - ps->delta_angles[i];
	}

	if ( !(ps->pm_flags & PMF_PLAYER) )
	{
		return;
	}

	if ( ps->eFlags & EF_PRONE )
	{
		ucmd->buttons |= BUTTON_PRONE;
	}
	else if ( ps->eFlags & EF_CROUCH )
	{
		ucmd->buttons |= BUTTON_CROUCH;
	}

	if ( ps->leanf > 0 )
	{
		ucmd->buttons |= BUTTON_LEANRIGHT;
	}
	else if ( ps->leanf < 0 )
	{
		ucmd->buttons |= BUTTON_LEANLEFT;
	}

	if ( ps->fWeaponPosFrac != 0 )
	{
		ucmd->buttons |= BUTTON_ADS;
	}
}

/*
==============
MSG_WriteReliableCommandToBuffer
==============
*/
void MSG_WriteReliableCommandToBuffer( const char *pszCommand, char *pszBuffer, int iBufferSize )
{
	int iCommandLength;
	int i;

	iCommandLength = strlen(pszCommand);

	if ( iCommandLength >= iBufferSize )
	{
		Com_Printf("WARNING: Reliable command is too long (%i/%i) and will be truncated: '%s'\n", iCommandLength, iBufferSize, pszCommand);
	}

	if ( !iCommandLength )
	{
		Com_Printf("WARNING: Empty reliable command\n");
	}

	for ( i = 0; i < iBufferSize && pszCommand[i]; i++ )
	{
		pszBuffer[i] = I_CleanChar(pszCommand[i]);

		if ( pszBuffer[i] == '%' )
		{
			pszBuffer[i] = '.';
		}
	}

	if ( i < iBufferSize )
		pszBuffer[i] = 0;
	else
		pszBuffer[iBufferSize - 1] = 0;
}

/*
==============
MSG_ReadData
==============
*/
void MSG_ReadData( msg_t *msg, void *data, int len )
{
	int newcount = msg->readcount + len;

	if ( newcount <= msg->cursize )
	{
		memcpy(data, &msg->data[msg->readcount], len);
		msg->readcount = newcount;
		return;
	}

	msg->overflowed = qtrue;
	memset(data, -1, len);
}

/*
==============
MSG_ReadInt64
==============
*/
int64_t MSG_ReadInt64( msg_t *msg )
{
	int newcount = msg->readcount + sizeof(uint64_t);

	if ( newcount <= msg->cursize )
	{
		uint64_t c = LittleLong64(*(uint64_t *)&msg->data[msg->readcount]);
		msg->readcount = newcount;
		return c;
	}

	msg->overflowed = qtrue;
	return 0;
}

/*
==============
MSG_ReadByte
==============
*/
int MSG_ReadByte( msg_t *msg )
{
	int newcount = msg->readcount + sizeof(uint8_t);

	if ( newcount <= msg->cursize )
	{
		uint8_t c = *(uint8_t *)&msg->data[msg->readcount];
		msg->readcount = newcount;
		return c;
	}

	msg->overflowed = qtrue;
	return -1;
}

/*
==============
MSG_WriteInt64
==============
*/
void MSG_WriteInt64( msg_t *msg, int64_t c )
{
	int newsize = msg->cursize + sizeof(uint64_t);

	if ( newsize <= msg->maxsize )
	{
		*(uint64_t *)&msg->data[msg->cursize] = LittleLong64(c);
		msg->cursize = newsize;
		return;
	}

	msg->overflowed = qtrue;
}

/*
==============
MSG_WriteData
==============
*/
void MSG_WriteData( msg_t *msg, const void *data, int length )
{
	int newsize = msg->cursize + length;

	if ( newsize <= msg->maxsize )
	{
		memcpy(&msg->data[msg->cursize], data, length);
		msg->cursize = newsize;
		return;
	}

	msg->overflowed = qtrue;
}

/*
==============
MSG_WriteByte
==============
*/
void MSG_WriteByte( msg_t *msg, int c )
{
	int newsize = msg->cursize + sizeof(uint8_t);

	if ( newsize <= msg->maxsize )
	{
		*(uint8_t *)&msg->data[msg->cursize] = c;
		msg->cursize = newsize;
		return;
	}

	msg->overflowed = qtrue;
}

/*
==============
MSG_ReadBit
==============
*/
int MSG_ReadBit( msg_t *msg )
{
	int bit = msg->bit & 7;

	if ( !bit )
	{
		if ( msg->readcount >= msg->cursize )
		{
			msg->overflowed = qtrue;
			return -1;
		}

		msg->bit = msg->readcount * 8;
		msg->readcount++;
	}

	return msg->data[msg->bit++ >> 3] >> bit & 1;
}

/*
==============
MSG_ReadBits
==============
*/
int MSG_ReadBits( msg_t *msg, int bits )
{
	assert((unsigned)bits <= 32);
	int value = 0;

	for ( int i = 0; i < bits; i++ )
	{
		value |= MSG_ReadBit( msg ) << i;
	}

	return value;
}

/*
==============
MSG_WriteBit1
==============
*/
void MSG_WriteBit1( msg_t *msg )
{
	if ( msg->maxsize <= msg->cursize )
	{
		msg->overflowed = qtrue;
		return;
	}

	int bit = msg->bit & 7;

	if ( !bit )
	{
		msg->bit = msg->cursize * 8;
		msg->data[msg->cursize] = 0;
		msg->cursize++;
	}

	msg->data[msg->bit >> 3] |= 1 << bit;
	msg->bit++;
}

/*
==============
MSG_WriteBit0
==============
*/
void MSG_WriteBit0( msg_t *msg )
{
	if ( msg->maxsize <= msg->cursize )
	{
		msg->overflowed = qtrue;
		return;
	}

	int bit = msg->bit & 7;

	if ( !bit )
	{
		msg->bit = msg->cursize * 8;
		msg->data[msg->cursize] = 0;
		msg->cursize++;
	}

	msg->bit++;
}

/*
==============
MSG_WriteBit
==============
*/
void MSG_WriteBit( msg_t *msg, int value )
{
	int bit = msg->bit & 7;

	if ( msg->maxsize <= msg->cursize )
	{
		msg->overflowed = qtrue;
		return;
	}

	if ( !bit )
	{
		msg->bit = msg->cursize * 8;
		msg->data[msg->cursize] = 0;
		msg->cursize++;
	}

	if ( value & 1 )
	{
		msg->data[msg->bit >> 3] |= 1 << bit;
	}

	msg->bit++;
}

/*
==============
MSG_WriteBits
==============
*/
void MSG_WriteBits( msg_t *msg, int value, int bits )
{
	assert((unsigned)bits <= 32);

	for ( int i = 0; i < bits; i++ )
	{
		MSG_WriteBit( msg, value );
		value = ( value >> 1 );
	}
}

/*
==============
MSG_GetUsedBitCount
==============
*/
int MSG_GetUsedBitCount( const msg_t *msg )
{
	return 8 * (msg->cursize) - ((8 - msg->bit) & 7);
}

/*
==============
MSG_BeginReading
==============
*/
void MSG_BeginReading( msg_t *msg )
{
	msg->overflowed = qfalse;
	msg->readcount = 0;
	msg->bit = 0;
}

/*
=============================================================================

delta functions with keys

=============================================================================
*/

/*
==============
MSG_ReadDeltaKeyByte
==============
*/
int MSG_ReadDeltaKeyByte( msg_t *msg, int key, int oldV )
{
	if ( MSG_ReadBit(msg) )
	{
		return (byte)MSG_ReadByte( msg ) ^ (byte)( key );
	}

	return oldV;
}

/*
==============
MSG_WriteDeltaKeyByte
==============
*/
void MSG_WriteDeltaKeyByte( msg_t *msg, int key, int oldV, int newV )
{
	if ( (byte)oldV == (byte)newV )
	{
		MSG_WriteBit0(msg);
		return;
	}

	MSG_WriteBit1(msg);
	MSG_WriteByte(msg, newV ^ key);
}

unsigned int kbitmask[] =
{
	0x00000000, 0x00000001, 0x00000003,
	0x00000007, 0x0000000F, 0x0000001F,
	0x0000003F, 0x0000007F, 0x000000FF,
	0x000001FF, 0x000003FF, 0x000007FF,
	0x00000FFF, 0x00001FFF, 0x00003FFF,
	0x00007FFF, 0x0000FFFF, 0x0001FFFF,
	0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
	0x001FFFFf, 0x003FFFFF, 0x007FFFFF,
	0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF,
	0x07FFFFFF, 0x0FFFFFFF, 0x1FFFFFFF,
	0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF,
};

/*
==============
MSG_ReadKey
==============
*/
int MSG_ReadKey( msg_t *msg, int key, int bits )
{
	return MSG_ReadBits( msg, bits ) ^ ( key & kbitmask[bits] );
}

/*
==============
MSG_WriteKey
==============
*/
void MSG_WriteKey( msg_t *msg, int key, int newV, int bits )
{
	MSG_WriteBits(msg, newV ^ key, bits);
}

/*
==============
MSG_ReadDeltaKey
==============
*/
int MSG_ReadDeltaKey( msg_t *msg, int key, int oldV, int bits )
{
	if ( MSG_ReadBit(msg) )
	{
		return MSG_ReadBits( msg, bits ) ^ ( key & kbitmask[bits] );
	}

	return oldV;
}

/*
==============
MSG_WriteDeltaKey
==============
*/
void MSG_WriteDeltaKey( msg_t *msg, int key, int oldV, int newV, int bits )
{
	if ( oldV == newV )
	{
		MSG_WriteBit0(msg);
		return;
	}

	MSG_WriteBit1(msg);
	MSG_WriteBits(msg, newV ^ key, bits);
}

/*
==============
MSG_ReadStringLine
==============
*/
char *MSG_ReadStringLine( msg_t *msg, char *string, unsigned int maxChars )
{
	int c;
	unsigned int l;

	for ( l = 0; ; l++ )
	{
		c = MSG_ReadByte(msg); // use ReadByte so -1 is out of bounds

		// translate all fmt spec to avoid crash bugs
		if ( c == '%' )
		{
			c = '.';
		}
		else if ( c == '\n' || c == -1 )
		{
			c = 0;
		}

		if ( l < maxChars )
		{
			string[l] = I_CleanChar(c);
		}

		if ( c == 0 )
		{
			break;
		}
	}

	string[maxChars - 1] = 0;

	return string;
}

/*
==============
MSG_ReadBigString
==============
*/
char *MSG_ReadBigString( msg_t *msg )
{
	static char string[BIG_INFO_STRING];
	int c;
	unsigned int l;

	assert(Sys_IsMainThread());

	for ( l = 0; ; l++ )
	{
		c = MSG_ReadByte(msg); // use ReadByte so -1 is out of bounds

		// translate all fmt spec to avoid crash bugs
		if ( c == '%' )
		{
			c = '.';
		}
		else if ( c == -1 )
		{
			c = 0;
		}

		if ( l < sizeof( string ) )
		{
			string[l] = I_CleanChar(c);
		}

		if ( c == 0 )
		{
			break;
		}
	}

	string[l] = 0;

	return string;
}

/*
==============
MSG_ReadString
==============
*/
char *MSG_ReadString( msg_t *msg, char *string, unsigned int maxChars )
{
	int c;
	unsigned int l;

	assert(string);

	for ( l = 0; ; l++ )
	{
		c = MSG_ReadByte(msg);

		if ( c == -1 )
		{
			c = 0;
		}

		if ( l < maxChars )
		{
			string[l] = I_CleanChar(c);
		}

		if ( c == 0 )
		{
			break;
		}
	}

	string[maxChars - 1] = 0;

	return string;
}

/*
==============
MSG_ReadLong
==============
*/
int MSG_ReadLong( msg_t *msg )
{
	int newcount = msg->readcount + sizeof(uint32_t);

	if ( newcount <= msg->cursize )
	{
		uint32_t c = LittleLong(*(uint32_t *)&msg->data[msg->readcount]);
		msg->readcount = newcount;
		return c;
	}

	msg->overflowed = qtrue;
	return 0;
}

/*
==============
MSG_ReadShort
==============
*/
int MSG_ReadShort( msg_t *msg )
{
	int newcount = msg->readcount + sizeof(uint16_t);

	if ( newcount <= msg->cursize )
	{
		uint16_t c = LittleShort(*(uint16_t *)&msg->data[msg->readcount]);
		msg->readcount = newcount;
		return c;
	}

	msg->overflowed = qtrue;
	return 0;
}

/*
==============
MSG_WriteAngle
==============
*/
void MSG_WriteAngle( msg_t *sb, float f )
{
	MSG_WriteByte( sb, (int)( f * 256 / 360 ) & 255 );
}

/*
==============
MSG_WriteString
==============
*/
void MSG_WriteString( msg_t *sb, const char *s )
{
	int l;
	char string[MAX_STRING_CHARS];
	int i;

	assert(s);
	l = strlen(s);

	if ( l >= MAX_STRING_CHARS )
	{
		Com_Printf("MSG_WriteString: MAX_STRING_CHARS");
		MSG_WriteData(sb, "", 1);
		return;
	}

	for ( i = 0; i < l; i++ )
	{
		string[i] = I_CleanChar(s[i]);
	}

	string[i] = 0;
	MSG_WriteData(sb, string, l + 1);
}

/*
==============
MSG_WriteLong
==============
*/
void MSG_WriteLong( msg_t *msg, int c )
{
	int newsize = msg->cursize + sizeof(uint32_t);

	if ( newsize <= msg->maxsize )
	{
		*(uint32_t *)&msg->data[msg->cursize] = LittleLong(c);
		msg->cursize = newsize;
		return;
	}

	msg->overflowed = qtrue;
}

/*
==============
MSG_WriteShort
==============
*/
void MSG_WriteShort( msg_t *msg, int c )
{
	int newsize = msg->cursize + sizeof(uint16_t);

	if ( newsize <= msg->maxsize )
	{
		*(uint16_t *)&msg->data[msg->cursize] = LittleShort(c);
		msg->cursize = newsize;
		return;
	}

	msg->overflowed = qtrue;
}

/*
==============
MSG_ReadDeltaKeyShort
==============
*/
int MSG_ReadDeltaKeyShort( msg_t *msg, int key, int oldV )
{
	if ( MSG_ReadBit(msg) )
	{
		return (uint16_t)MSG_ReadShort( msg ) ^ (uint16_t)( key );
	}

	return oldV;
}

/*
==============
MSG_WriteDeltaKeyShort
==============
*/
void MSG_WriteDeltaKeyShort( msg_t *msg, int key, int oldV, int newV )
{
	if ( (uint16_t)oldV == (uint16_t)newV )
	{
		MSG_WriteBit0(msg);
		return;
	}

	MSG_WriteBit1(msg);
	MSG_WriteShort(msg, newV ^ key);
}

/*
==============
MSG_ReadAngle16
==============
*/
float MSG_ReadAngle16( msg_t *msg )
{
	return SHORT2ANGLE( MSG_ReadShort( msg ) );
}

/*
==============
MSG_WriteAngle16
==============
*/
void MSG_WriteAngle16( msg_t *sb, float f )
{
	MSG_WriteShort( sb, ANGLE2SHORT( f ) );
}

/*
==============
MSG_ReadBitsCompress
==============
*/
int MSG_ReadBitsCompress( byte *from, byte *to, int toSizeBytes )
{
	byte *data;
	int bit;
	int bits;
	int get;

	bits = toSizeBytes * 8;
	data = to;
	bit = 0;

	while ( bits > bit )
	{
		Huff_offsetReceive(msgHuff.decompressor.tree, &get, from, &bit);
		*data = get;
		*data++;
	}

	return data - to;
}

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

/*
==============
MSG_InitHuffmanInternal
==============
*/
static void MSG_InitHuffmanInternal()
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

/*
==============
MSG_InitHuffman
==============
*/
void MSG_InitHuffman()
{
	msgInit = qtrue;
	MSG_InitHuffmanInternal();
}

#define KEY_MASK_MOVE_FORWARD 1
#define KEY_MASK_MOVE_BACK    2
#define KEY_MASK_MOVE_RIGHT   4
#define KEY_MASK_MOVE_LEFT    8

/*
==============
MSG_MaskKey
==============
*/
int MSG_GetMoveKeyMask( char forwardmove, char rightmove )
{
	int value = 0;

	if ( forwardmove > 10 )
	{
		value = KEY_MASK_MOVE_FORWARD;
	}
	else if ( forwardmove < -10 )
	{
		value = KEY_MASK_MOVE_BACK;
	}

	if ( rightmove > 10 )
	{
		return value | KEY_MASK_MOVE_RIGHT;
	}
	else if ( rightmove < -10 )
	{
		return value | KEY_MASK_MOVE_LEFT;
	}

	return value;
}

/*
==============
MSG_UnmaskKey
==============
*/
void MSG_UnmaskMoveKey( char key, char *forwardmove, char *rightmove )
{
	if ( key & KEY_MASK_MOVE_FORWARD )
	{
		*forwardmove = BUTTON_FORWARD;
	}
	else if ( key & KEY_MASK_MOVE_BACK )
	{
		*forwardmove = BUTTON_BACK;
	}
	else
	{
		*forwardmove = BUTTON_NONE;
	}

	if ( key & KEY_MASK_MOVE_RIGHT )
	{
		*rightmove = BUTTON_MOVERIGHT;
	}
	else if ( key & KEY_MASK_MOVE_LEFT )
	{
		*rightmove = BUTTON_MOVELEFT;
	}
	else
	{
		*rightmove = BUTTON_NONE;
	}
}

/*
==============
MSG_ReadDeltaUsercmdKey
==============
*/
void MSG_ReadDeltaUsercmdKey( msg_t *msg, int key, usercmd_t *from, usercmd_t *to )
{
	*to = *from;

	if ( MSG_ReadBit(msg) )
	{
		to->serverTime = from->serverTime + MSG_ReadByte(msg);
	}
	else
	{
		to->serverTime = MSG_ReadLong(msg);
	}

	if ( !MSG_ReadKey(msg, key, 1) )
	{
		return;
	}

	to->buttons &= ~BUTTON_ATTACK;

	if ( MSG_ReadKey(msg, key, 1) )
	{
		to->buttons |= MSG_ReadKey(msg, key, 1);
		to->angles[0] = (uint16_t)MSG_ReadDeltaKeyShort(msg, key, from->angles[0]);
		to->angles[1] = (uint16_t)MSG_ReadDeltaKeyShort(msg, key, from->angles[1]);
		MSG_UnmaskMoveKey(MSG_ReadDeltaKey(msg, key, MSG_GetMoveKeyMask(from->forwardmove, from->rightmove), 4), &to->forwardmove, &to->rightmove);
		key ^= to->serverTime;
		to->angles[2] = (uint16_t)MSG_ReadDeltaKeyShort(msg, key, from->angles[2]);
		to->buttons &= BUTTON_ATTACK;
		to->buttons |= 2 * MSG_ReadDeltaKey(msg, key, from->buttons >> 1, 18);
		to->weapon = MSG_ReadDeltaKey(msg, key, from->weapon, 7);
		to->offHandIndex = MSG_ReadDeltaKey(msg, key, from->offHandIndex, 7);
	}
	else
	{
		key ^= to->serverTime;
		to->buttons |= MSG_ReadKey(msg, key, 1);
		to->angles[0] = (uint16_t)MSG_ReadDeltaKeyShort(msg, key, from->angles[0]);
		to->angles[1] = (uint16_t)MSG_ReadDeltaKeyShort(msg, key, from->angles[1]);
		MSG_UnmaskMoveKey(MSG_ReadDeltaKey(msg, key, MSG_GetMoveKeyMask(from->forwardmove, from->rightmove), 4), &to->forwardmove, &to->rightmove);
	}
}

/*
==============
MSG_WriteDeltaUsercmdKey
==============
*/
void MSG_WriteDeltaUsercmdKey( msg_t *msg, int key, usercmd_t *from, usercmd_t *to )
{
	int oldV;
	int newV;

	if ( to->serverTime - from->serverTime < 256 )
	{
		MSG_WriteBit1(msg);
		MSG_WriteByte(msg, to->serverTime - from->serverTime);
	}
	else
	{
		MSG_WriteBit0(msg);
		MSG_WriteLong(msg, to->serverTime);
	}

	newV = MSG_GetMoveKeyMask(to->forwardmove, to->rightmove);
	oldV = MSG_GetMoveKeyMask(from->forwardmove, from->rightmove);

	if ( from->buttons >> 1 == to->buttons >> 1 && from->weapon == to->weapon && from->offHandIndex == to->offHandIndex && from->angles[2] == to->angles[2] )
	{
		if ( from->angles[0] == to->angles[0] && from->angles[1] == to->angles[1] && (from->buttons & 1) == (to->buttons & 1) && oldV == newV )
		{
			MSG_WriteKey(msg, key, 0, 1); // NERVE - SMF
			return;						  // no change
		}

		MSG_WriteKey(msg, key, 1, 1);
		MSG_WriteKey(msg, key, 0, 1);
		key ^= to->serverTime;
		MSG_WriteKey(msg, key, to->buttons, 1);
		MSG_WriteDeltaKeyShort(msg, key, from->angles[0], to->angles[0]);
		MSG_WriteDeltaKeyShort(msg, key, from->angles[1], to->angles[1]);
		MSG_WriteDeltaKey(msg, key, oldV, newV, 4);
	}
	else
	{
		MSG_WriteKey(msg, key, 1, 1);
		MSG_WriteKey(msg, key, 1, 1);
		MSG_WriteKey(msg, key, to->buttons, 1);
		MSG_WriteDeltaKeyShort(msg, key, from->angles[0], to->angles[0]);
		MSG_WriteDeltaKeyShort(msg, key, from->angles[1], to->angles[1]);
		MSG_WriteDeltaKey(msg, key, oldV, newV, 4);
		key ^= to->serverTime;
		MSG_WriteDeltaKeyShort(msg, key, from->angles[2], to->angles[2]);
		MSG_WriteDeltaKey(msg, key, from->buttons >> 1, to->buttons >> 1, 18);
		MSG_WriteDeltaKey(msg, key, from->weapon, to->weapon, 7);
		MSG_WriteDeltaKey(msg, key, from->offHandIndex, to->offHandIndex, 7);
	}
}

/*
==============
MSG_WriteBitsCompress
==============
*/
int MSG_WriteBitsCompress( byte *from, byte *to, int fromSizeBytes )
{
	int bit;
	int data;

	bit = 0;
	data = fromSizeBytes;

	while ( data )
	{
		Huff_offsetTransmit(&msgHuff.compressor, *from, to, &bit);
		--data;
		++from;
	}

	return (bit + 7) >> 3;
}

/*
==============
MSG_Init
==============
*/
void MSG_Init( msg_t *buf, byte *data, int length )
{
	if ( !msgInit )
	{
		MSG_InitHuffman();
	}

	memset(buf, 0, sizeof(*buf));
//bani - optimization
//	memset (data, 0, length);
	buf->data = data;
	buf->maxsize = length;
}

// using the stringizing operator to save typing...
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

// using the stringizing operator to save typing...
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

// if (int)f == f and (int)f + ( 1<<(FLOAT_INT_BITS-1) ) < ( 1 << FLOAT_INT_BITS )
// the float will be sent with FLOAT_INT_BITS, otherwise all 32 bits will be sent
#define FLOAT_INT_BITS  13
#define FLOAT_INT_BIAS  ( 1 << ( FLOAT_INT_BITS - 1 ) )

#define HUDELEM_COORD_BITS  10
#define HUDELEM_COORD_BIAS  ( 1 << ( HUDELEM_COORD_BITS - 1 ) )

/*
==============
MSG_WriteValueNoXor
==============
*/
void MSG_WriteValueNoXor( msg_t *msg, int value, int bits )
{
	int absbits;
	unsigned int partialBits;

	absbits = abs(bits);
	partialBits = absbits & 7;

	if ( partialBits )
	{
		MSG_WriteBits(msg, value, partialBits);
		absbits -= partialBits;
		value >>= partialBits;
	}

	while ( absbits )
	{
		MSG_WriteByte(msg, value);
		value >>= 8;
		absbits -= 8;
	}
}

/*
==============
MSG_WriteDeltaPlayerstate
==============
*/
void MSG_WriteDeltaPlayerstate( msg_t *msg, playerState_t *from, playerState_t *to, int number )
{
	int *fromF, *toF;
	netField_t *field;
	playerState_t dummy;
	int i, j, lc;

	if ( !from )
	{
		from = &dummy;
		memset( &dummy, 0, sizeof( dummy ) );
	}

	lc = 0;
	for ( i = 0, field = playerStateFields ; i < ARRAY_COUNT( playerStateFields ) ; i++, field++ )
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );
		if ( *fromF != *toF )
		{
			lc = i + 1;
		}
	}

	MSG_WriteByte( msg, lc ); // # of changes

	for ( i = 0, field = playerStateFields ; i < lc ; i++, field++ )
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );

		if ( *fromF == *toF )
		{
			MSG_WriteBit0( msg ); // no change
			continue;
		}

		MSG_WriteBit1( msg );  // changed

		if ( field->bits == 0 )
		{
			// float
			float fullFloat = *(float *)toF;
			int trunc = (int)fullFloat;

			if ( trunc == fullFloat && (unsigned)( trunc + FLOAT_INT_BIAS ) < ( 1 << FLOAT_INT_BITS ) )
			{
				// send as small integer
				MSG_WriteBit0( msg );
				MSG_WriteBits( msg, (trunc + FLOAT_INT_BIAS), 5 );
				MSG_WriteByte( msg, (trunc + FLOAT_INT_BIAS) >> 5 );
			}
			else
			{
				// send as full floating point value
				MSG_WriteBit1( msg );
				MSG_WriteLong( msg, *toF );
			}
		}
		else if ( field->bits == -100 )
		{
			// viewangle bits
			if ( *toF )
			{
				MSG_WriteBit1( msg );
				MSG_WriteAngle16( msg, *(float *)toF );
			}
			else
			{
				MSG_WriteBit0( msg );
			}
		}
		else
		{
			// integer
			MSG_WriteValueNoXor( msg, *toF, field->bits );
		}
	}

	//
	// send the arrays
	//

	// stats
	int statsbits = 0;
	int numstatsbits = MAX_STATS;

#if PROTOCOL_VERSION == 115 and defined LIBCOD
	client_t *client = &svs.clients[number];
	assert(client);
	int protocol = client->netchan.protocol;
	if ( protocol == 119 )
		numstatsbits -= 1;
#endif

	for ( i = 0; i < numstatsbits; i++ )
	{
		if ( to->stats[i] != from->stats[i] )
		{
			statsbits |= 1 << i;
		}
	}

	if ( statsbits )
	{
		MSG_WriteBit1( msg ); // changed
		MSG_WriteBits( msg, statsbits, numstatsbits );

		if ( statsbits & ( 1 << STAT_HEALTH ) )
			MSG_WriteShort( msg, to->stats[STAT_HEALTH] );

		if ( statsbits & ( 1 << STAT_DEAD_YAW ) )
			MSG_WriteShort( msg, to->stats[STAT_DEAD_YAW] );

		if ( statsbits & ( 1 << STAT_MAX_HEALTH ) )
			MSG_WriteShort( msg, to->stats[STAT_MAX_HEALTH] );

		if ( statsbits & ( 1 << STAT_IDENT_CLIENT_NUM ) )
			MSG_WriteBits( msg, to->stats[STAT_IDENT_CLIENT_NUM], CLIENTNUM_BITS );

#if PROTOCOL_VERSION != 119
#if PROTOCOL_VERSION == 115 and defined LIBCOD
		if ( protocol != 119 )
		{
			if ( statsbits & ( 1 << STAT_IDENT_CLIENT_HEALTH ) )
				MSG_WriteShort( msg, to->stats[STAT_IDENT_CLIENT_HEALTH] );
		}
#else
		if ( statsbits & ( 1 << STAT_IDENT_CLIENT_HEALTH ) )
			MSG_WriteShort( msg, to->stats[STAT_IDENT_CLIENT_HEALTH] );
#endif
#endif

		if ( statsbits & ( 1 << STAT_SPAWN_COUNT ) )
			MSG_WriteByte( msg, to->stats[STAT_SPAWN_COUNT] );
	}
	else
	{
		MSG_WriteBit0( msg ); // no change to stats
	}

//----(SA)	I split this into two groups using shorts so it wouldn't have
//			to use a long every time ammo changed for any weap.
//			this seemed like a much friendlier option than making it
//			read/write a long for any ammo change.

	// j == 0 : weaps 0-15
	// j == 1 : weaps 16-31
	// j == 2 : weaps 32-47	//----(SA)	now up to 64 (but still pretty net-friendly)
	// j == 3 : weaps 48-63

	// ammo stored
	int ammobits[8]; //----(SA)	modified

	for ( j = 0; j < 4; j++ )    //----(SA)	modified for 64 weaps
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

//----(SA)	also encapsulated ammo changes into one check.  clip values will change frequently,
	// but ammo will not.  (only when you get ammo/reload rather than each shot)
	if ( ammobits[0] || ammobits[1] || ammobits[2] || ammobits[3] )    // if any were set...
	{
		MSG_WriteBit1( msg ); // changed
		for ( j = 0; j < 4; j++ )
		{
			if ( ammobits[j] )
			{
				MSG_WriteBit1( msg ); // changed
				MSG_WriteShort( msg, ammobits[j] );
				for ( i = 0 ; i < 16 ; i++ )
					if ( ammobits[j] & ( 1 << i ) )
					{
						MSG_WriteShort( msg, to->ammo[i + ( j * 16 )] );
					}
			}
			else
			{
				MSG_WriteBit0( msg ); // no change
			}
		}
	}
	else
	{
		MSG_WriteBit0( msg ); // no change
	}

	// ammo in clip
	int clipbits;

	for ( j = 0; j < 4; j++ )    //----(SA)	modified for 64 weaps
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
			MSG_WriteBit1( msg ); // changed
			MSG_WriteShort( msg, clipbits );
			for ( i = 0 ; i < 16 ; i++ )
				if ( clipbits & ( 1 << i ) )
				{
					MSG_WriteShort( msg, to->ammoclip[i + ( j * 16 )] );
				}
		}
		else
		{
			MSG_WriteBit0( msg ); // no change
		}
	}

	// Objectives
	if ( memcmp(from->objective, to->objective, sizeof(from->objective)) )
	{
		MSG_WriteBit1( msg ); // changed

		for ( i = 0; i < MAX_OBJECTIVES; i++ )
		{
			MSG_WriteBits(msg, to->objective[i].state, OBJST_DONE);
			MSG_WriteDeltaFields(msg, (byte *)&from->objective[i], (byte *)&to->objective[i], 0, ARRAY_COUNT( objectiveFields ), objectiveFields);
		}
	}
	else
	{
		MSG_WriteBit0( msg ); // no change
	}

	// Hud-elements
	if ( memcmp(&from->hud, &to->hud, sizeof(from->hud)) )
	{
		MSG_WriteBit1( msg ); // changed

		MSG_WriteDeltaHudElems(msg, from->hud.archival, to->hud.archival, MAX_HUDELEMS_ARCHIVAL);
		MSG_WriteDeltaHudElems(msg, from->hud.current,  to->hud.current,  MAX_HUDELEMS_CURRENT);
	}
	else
	{
		MSG_WriteBit0( msg ); // no change
	}
}

/*
==============
MSG_ReadValueNoXor
==============
*/
int MSG_ReadValueNoXor( msg_t *msg, int bits )
{
	int j;
	int value;

	assert(bits >= 0);

	if ( bits & 7 )
		value = MSG_ReadBits(msg, bits & 7);
	else
		value = 0;

	for ( j = bits & 7; j < bits; j += 8 )
		value |= MSG_ReadByte(msg) << j;

	return value;
}

/*
==============
MSG_ReadValue
==============
*/
int MSG_ReadValue( msg_t *msg, int bits )
{
	int absbits;
	int mask;
	int value;

	absbits = abs(bits);
	value = MSG_ReadValueNoXor(msg, absbits);

	assert(absbits <= 32);

	if ( (unsigned)bits >> 31 && (value >> (absbits - 1)) & 1 )
		value |= ~((1 << absbits) - 1);

	return value;
}

/*
==============
MSG_ReadDeltaField
==============
*/
void MSG_ReadDeltaField( msg_t *msg, byte *from, byte *to, const netField_t *field, qboolean print )
{
	int *fromF, *toF;

	fromF = ( int * )( (byte *)from + field->offset );
	toF = ( int * )( (byte *)to + field->offset );

	if ( !MSG_ReadBit( msg ) )
	{
		// no change
		*toF = *fromF;
		return;
	}

	if ( field->bits == 0 )
	{
		// float
		if ( !MSG_ReadBit( msg ) )
		{
			*toF = 0;
			return;
		}

		if ( MSG_ReadBit( msg ) == 0 )
		{
			// integral float
			int trunc = MSG_ReadBits( msg, 5 );
			// bias to allow equal parts positive and negative
			trunc = 32 * MSG_ReadByte( msg ) + trunc - FLOAT_INT_BIAS;
			*(float *)toF = trunc;
			if ( print )
				Com_Printf("%s:%i ", field->name, trunc);
		}
		else
		{
			// full floating point value
			*toF = MSG_ReadLong( msg );
			if ( print )
				Com_Printf("%s:%f ", field->name, *(float *)toF);
		}
	}
	else if ( field->bits == -99 )
	{
		// hudelem coord bits
		if ( !MSG_ReadBit( msg ) )
		{
			*toF = 0;
			return;
		}

		if ( MSG_ReadBit( msg ) == 0 )
		{
			// integral float
			int trunc = MSG_ReadBits( msg, 2 );
			// bias to allow equal parts positive and negative
			trunc = 4 * MSG_ReadByte(msg) + trunc - HUDELEM_COORD_BIAS;
			*(float *)toF = trunc;
			if ( print )
				Com_Printf("%s:%i ", field->name, trunc);
		}
		else
		{
			// full floating point value
			*toF = MSG_ReadLong(msg);
			if ( print )
				Com_Printf("%s:%f ", field->name, *(float *)toF);
		}
	}
	else if ( field->bits == -100 )
	{
		// angle bits
		if ( !MSG_ReadBit( msg ) )
		{
			*toF = 0;
			return;
		}

		*(float *)toF = MSG_ReadAngle16( msg );
	}
	else
	{
		// integer
		if ( !MSG_ReadBit( msg ) )
		{
			*toF = 0;
			return;
		}

		*toF = MSG_ReadValue( msg, field->bits );
		if ( print )
			Com_Printf("%s:%i ", field->name, *toF);
	}
}

/*
==============
MSG_WriteBigString
==============
*/
void MSG_WriteBigString( msg_t *sb, const char *s )
{
	int l, i;
	char string[BIG_INFO_STRING];

	assert(s);
	l = strlen( s );

	if ( l >= BIG_INFO_STRING )
	{
		Com_Printf("MSG_WriteString: BIG_INFO_STRING");
		MSG_WriteData( sb, "", 1 );
		return;
	}

	I_strncpyz( string, s, sizeof( string ) );

	for ( i = 0; i < l; ++i )
	{
		string[i] = I_CleanChar(s[i]);
	}

	MSG_WriteData( sb, string, l + 1 );
}

// using the stringizing operator to save typing...
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

/*
==============
MSG_WriteDeltaClient
==============
*/
void MSG_WriteDeltaClient( msg_t *msg, clientState_t *from, clientState_t *to, qboolean force )
{
	clientState_t dummy;

	if ( !from )
	{
		from = &dummy;
		Com_Memset(&dummy, 0, sizeof(dummy));
	}

	MSG_WriteDeltaStruct(msg, (byte *)from, (byte *)to, force, ARRAY_COUNT(clientStateFields), CLIENTNUM_BITS, clientStateFields, qtrue);
}

// using the stringizing operator to save typing...
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

/*
==============
MSG_WriteDeltaArchivedEntity
==============
*/
void MSG_WriteDeltaArchivedEntity( msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, int flags )
{
	MSG_WriteDeltaStruct(msg, (byte *)from, (byte *)to, flags, ARRAY_COUNT(archivedEntityFields), GENTITYNUM_BITS, archivedEntityFields, qfalse);
}

// using the stringizing operator to save typing...
#define ESF( x ) # x,(intptr_t)&( (entityState_t*)0 )->x

netField_t entityStateFields[] =
{
	{ ESF( pos.trTime ), 32},
	{ ESF( pos.trBase[1] ), 0},
	{ ESF( pos.trBase[0] ), 0},
	{ ESF( pos.trDelta[0] ), 0},
	{ ESF( pos.trDelta[1] ), 0},
	{ ESF( angles2[1] ), 0},
	{ ESF( apos.trBase[1] ), -100},
	{ ESF( pos.trDelta[2] ), 0},
	{ ESF( pos.trBase[2] ), 0},
	{ ESF( apos.trBase[0] ), -100},
	{ ESF( eventSequence ), 8},
	{ ESF( legsAnim ), 10},
	{ ESF( eType ), 8},
	{ ESF( eFlags ), 24},
	{ ESF( otherEntityNum ), 10},
	{ ESF( surfType ), 8},
	{ ESF( eventParm ), 8},
	{ ESF( scale ), 8},
	{ ESF( clientNum ), 8},
	{ ESF( torsoAnim ), 10},
	{ ESF( groundEntityNum ), 10},
	{ ESF( events[0] ), 8},
	{ ESF( events[1] ), 8},
	{ ESF( events[2] ), 8},
	{ ESF( angles2[0] ), 0},
	{ ESF( events[3] ), 8},
	{ ESF( apos.trBase[2] ), -100},
	{ ESF( pos.trType ), 8},
	{ ESF( fWaistPitch ), 0},
	{ ESF( fTorsoPitch ), 0},
	{ ESF( apos.trTime ), 32},
	{ ESF( solid ), 24},
	{ ESF( apos.trDelta[0] ), 0},
	{ ESF( apos.trType ), 8},
	{ ESF( animMovetype ), 4},
	{ ESF( fTorsoHeight ), 0},
	{ ESF( apos.trDelta[2] ), 0},
	{ ESF( weapon ), 7},
	{ ESF( index ), 10},
	{ ESF( apos.trDelta[1] ), 0},
	{ ESF( eventParms[0] ), 8},
	{ ESF( eventParms[1] ), 8},
	{ ESF( eventParms[2] ), 8},
	{ ESF( eventParms[3] ), 8},
	{ ESF( iHeadIcon ), 4},
	{ ESF( pos.trDuration ), 32},
	{ ESF( iHeadIconTeam ), 2},
	{ ESF( time ), 32},
	{ ESF( leanf ), 0},
	{ ESF( attackerEntityNum ), 10},
	{ ESF( time2 ), 32},
	{ ESF( loopSound ), 8},
	{ ESF( origin2[2] ), 0},
	{ ESF( origin2[0] ), 0},
	{ ESF( origin2[1] ), 0},
	{ ESF( angles2[2] ), 0},
	{ ESF( constantLight ), 32},
	{ ESF( apos.trDuration ), 32},
	{ ESF( dmgFlags ), 32},
};

/*
==============
MSG_WriteDeltaEntity
==============
*/
void MSG_WriteDeltaEntity( msg_t *msg, entityState_t *from, entityState_t *to, qboolean force )
{
	MSG_WriteDeltaStruct(msg, (byte *)from, (byte *)to, force, ARRAY_COUNT(entityStateFields), GENTITYNUM_BITS, entityStateFields, qfalse);
}

/*
==============
MSG_ReadDeltaPlayerstate
==============
*/
void MSG_ReadDeltaPlayerstate( msg_t *msg, playerState_t *from, playerState_t *to, int number )
{
	int *fromF, *toF;
	int i, j, lc;
	netField_t  *field;
	int print;
	int bits;
	playerState_t dummy;

	if ( !from )
	{
		from = &dummy;
		memset( &dummy, 0, sizeof( dummy ) );
	}
	*to = *from;

	// shownet 2/3 will interleave with other printed info, -2 will
	// just print the delta records
	//if ( cl_shownet && ( cl_shownet->integer >= 2 || cl_shownet->integer == -2 ) )
	//{
	// print = 1;
	//	Com_Printf( "%3i: playerstate ", msg->readcount );
	//}
	//else
	//{
	print = 0;
	//}

	lc = MSG_ReadByte( msg );

	for ( i = 0, field = playerStateFields ; i < lc ; i++, field++ )
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );

		if ( !MSG_ReadBit( msg ) )
		{
			// no change
			*toF = *fromF;
			continue;
		}

		if ( field->bits == 0 )
		{
			// float
			if ( MSG_ReadBit( msg ) == 0 )
			{
				// integral float
				int trunc = MSG_ReadBits( msg, 5 );
				// bias to allow equal parts positive and negative
				trunc = 32 * MSG_ReadByte( msg ) + trunc - FLOAT_INT_BIAS;
				*(float *)toF = trunc;
				if ( print )
					Com_Printf("%s:%i ", field->name, trunc);
			}
			else
			{
				// full floating point value
				*toF = MSG_ReadLong( msg );
				if ( print )
					Com_Printf("%s:%f ", field->name, *(float *)toF);
			}
		}
		else if ( field->bits == -100 )
		{
			// angle bits
			if ( !MSG_ReadBit( msg ) )
			{
				*toF = 0;
				continue;
			}

			*(float *)toF = MSG_ReadAngle16( msg );
		}
		else
		{
			// integer
			*toF = MSG_ReadValue( msg, field->bits );
			if ( print )
				Com_Printf("%s:%i ", field->name, *toF);
		}
	}

	for ( i = lc,field = &playerStateFields[lc]; i < ARRAY_COUNT( playerStateFields ); i++, field++ )
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );
		// no change
		*toF = *fromF;
	}

	// read the arrays
	int numstatsbits = MAX_STATS;

#if PROTOCOL_VERSION == 115 and defined LIBCOD
	client_t *client = &svs.clients[number];
	assert(client);
	int protocol = client->netchan.protocol;
	if ( protocol == 119 )
		numstatsbits -= 1;
#endif

	if ( MSG_ReadBit(msg) )  // one general bit tells if any of this infrequently changing stuff has changed
	{
		// parse stats
		bits = MSG_ReadBits(msg, numstatsbits);

		if ( bits & ( 1 << STAT_HEALTH ) )
			to->stats[STAT_HEALTH] = MSG_ReadShort(msg);

		if ( bits & ( 1 << STAT_DEAD_YAW ) )
			to->stats[STAT_DEAD_YAW] = MSG_ReadShort(msg);

		if ( bits & ( 1 << STAT_MAX_HEALTH ) )
			to->stats[STAT_MAX_HEALTH] = MSG_ReadShort(msg);

		if ( bits & ( 1 << STAT_IDENT_CLIENT_NUM ) )
			to->stats[STAT_IDENT_CLIENT_NUM] = MSG_ReadBits(msg, CLIENTNUM_BITS);

#if PROTOCOL_VERSION != 119
#if PROTOCOL_VERSION == 115 and defined LIBCOD
		if ( protocol != 119 )
		{
			if ( bits & ( 1 << STAT_IDENT_CLIENT_HEALTH ) )
				to->stats[STAT_IDENT_CLIENT_HEALTH] = MSG_ReadShort(msg);
		}
#else
		if ( bits & ( 1 << STAT_IDENT_CLIENT_HEALTH ) )
			to->stats[STAT_IDENT_CLIENT_HEALTH] = MSG_ReadShort(msg);
#endif
#endif

		if ( bits & ( 1 << STAT_SPAWN_COUNT ) )
			to->stats[STAT_SPAWN_COUNT] = MSG_ReadByte(msg);
	}

//----(SA)	I split this into two groups using shorts so it wouldn't have
//			to use a long every time ammo changed for any weap.
//			this seemed like a much friendlier option than making it
//			read/write a long for any ammo change.

	// parse ammo

	// j == 0 : weaps 0-15
	// j == 1 : weaps 16-31
	// j == 2 : weaps 32-47	//----(SA)	now up to 64 (but still pretty net-friendly)
	// j == 3 : weaps 48-63

	// ammo stored
	if ( MSG_ReadBit(msg) )       // check for any ammo change (0-63)
	{
		for ( j = 0; j < 4; j++ )
		{
			if ( MSG_ReadBit(msg) )
			{
				//LOG( "PS_AMMO" );
				bits = MSG_ReadShort( msg );
				for ( i = 0 ; i < 16 ; i++ )
				{
					if ( bits & ( 1 << i ) )
					{
						to->ammo[i + ( j * 16 )] = MSG_ReadShort( msg );
					}
				}
			}
		}
	}

	// ammo in clip
	for ( j = 0; j < 4; j++ )
	{
		if ( MSG_ReadBit(msg) )
		{
			//LOG( "PS_AMMOCLIP" );
			bits = MSG_ReadShort( msg );
			for ( i = 0 ; i < 16 ; i++ )
			{
				if ( bits & ( 1 << i ) )
				{
					to->ammoclip[i + ( j * 16 )] = MSG_ReadShort( msg );
				}
			}
		}
	}

	// Objectives
	if ( MSG_ReadBit(msg) )
	{
		for ( i = 0; i < MAX_OBJECTIVES; i++ )
		{
			to->objective[i].state = MSG_ReadBits(msg, OBJST_DONE);
			MSG_ReadDeltaFields(msg, (byte *)&from->objective[i], (byte *)&to->objective[i], ARRAY_COUNT( objectiveFields ), objectiveFields);
		}
	}

	// Hud-elements
	if ( MSG_ReadBit(msg) )
	{
		MSG_ReadDeltaHudElems(msg, from->hud.archival, to->hud.archival, MAX_HUDELEMS_ARCHIVAL);
		MSG_ReadDeltaHudElems(msg, from->hud.current, to->hud.current, MAX_HUDELEMS_CURRENT);
	}
}

/*
==============
MSG_ReadDeltaClient
==============
*/
void MSG_ReadDeltaClient( msg_t *msg, clientState_t *from, clientState_t *to, int number )
{
	clientState_t dummy;

	if ( !from )
	{
		from = &dummy;
		Com_Memset(&dummy, 0, sizeof(dummy));
	}

	MSG_ReadDeltaStruct(msg, (byte *)from, (byte *)to, number, ARRAY_COUNT(clientStateFields), CLIENTNUM_BITS, clientStateFields);
}

/*
==============
MSG_ReadDeltaArchivedEntity
==============
*/
void MSG_ReadDeltaArchivedEntity( msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, int number )
{
	MSG_ReadDeltaStruct(msg, (byte *)from, (byte *)to, number, ARRAY_COUNT(archivedEntityFields), GENTITYNUM_BITS, archivedEntityFields);
}

/*
==============
MSG_ReadDeltaEntity
==============
*/
void MSG_ReadDeltaEntity( msg_t *msg, entityState_t *from, entityState_t *to, int number )
{
	MSG_ReadDeltaStruct(msg, (byte *)from, (byte *)to, number, ARRAY_COUNT(entityStateFields), GENTITYNUM_BITS, entityStateFields);
}

/*
==============
MSG_WriteDeltaField
==============
*/
void MSG_WriteDeltaField( msg_t *msg, byte *from, byte *to, const netField_t *field )
{
	int *fromF, *toF;

	fromF = ( int * )( (byte *)from + field->offset );
	toF = ( int * )( (byte *)to + field->offset );

	if ( fromF == toF )
	{
		MSG_WriteBit0( msg );
		return; // no change
	}

	MSG_WriteBit1( msg );  // changed

	if ( field->bits == 0 )
	{
		// float
		float fullFloat = *(float *)toF;
		int trunc = (int)fullFloat;

		if ( fullFloat == 0.0f )
		{
			MSG_WriteBit0( msg );
			return;
		}

		MSG_WriteBit1( msg );
		if ( trunc == fullFloat && (unsigned)( trunc + FLOAT_INT_BIAS ) < ( 1 << FLOAT_INT_BITS ) )
		{
			// send as small integer
			MSG_WriteBit0( msg );
			MSG_WriteBits( msg, trunc + FLOAT_INT_BIAS, 5 );
			MSG_WriteByte( msg, trunc + FLOAT_INT_BIAS >> 5 );
//					if ( print ) {
//						Com_Printf( "%s:%i ", field->name, trunc );
//					}
		}
		else
		{
			// send as full floating point value
			MSG_WriteBit1( msg );
			MSG_WriteLong( msg, *toF );
//					if ( print ) {
//						Com_Printf( "%s:%f ", field->name, *(float *)toF );
//					}
		}
	}
	else if ( field->bits == -99 )
	{
		// hudelem coord bits
		float fullFloat = *(float *)toF;
		int trunc = (int)fullFloat;

		if ( fullFloat == 0.0f )
		{
			MSG_WriteBit0( msg );
			return;
		}

		MSG_WriteBit1( msg );
		if ( trunc == fullFloat && (unsigned)( trunc + HUDELEM_COORD_BIAS ) < ( 1 << HUDELEM_COORD_BITS ) )
		{
			// send as small integer
			MSG_WriteBit0( msg );
			MSG_WriteBits( msg, trunc + HUDELEM_COORD_BIAS, 2 );
			MSG_WriteByte( msg, trunc + HUDELEM_COORD_BIAS >> 2 );
//					if ( print ) {
//						Com_Printf( "%s:%i ", field->name, trunc );
//					}
		}
		else
		{
			// send as full floating point value
			MSG_WriteBit1( msg );
			MSG_WriteLong( msg, *toF );
//					if ( print ) {
//						Com_Printf( "%s:%f ", field->name, *(float *)toF );
//					}
		}
	}
	else if ( field->bits == -100 )
	{
		// viewangle bits
		if ( *toF )
		{
			MSG_WriteBit1( msg );
			MSG_WriteAngle16( msg, *(float *)toF );
		}
		else
		{
			MSG_WriteBit0( msg );
		}
	}
	else
	{
		// integer
		if ( *toF )
		{
			MSG_WriteBit1( msg );
			MSG_WriteValueNoXor( msg, *toF, field->bits );
		}
		else
		{
			MSG_WriteBit0( msg );
		}
	}
}

// using the stringizing operator to save typing...
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

/*
==============
MSG_WriteDeltaHudElems
==============
*/
void MSG_WriteDeltaHudElems( msg_t *msg, hudelem_s *from, hudelem_s *to, int count )
{
	int *fromF, *toF;
	int i, j, inuse, lc;
	netField_t *field;

	assert(count == MAX_HUDELEMS_ARCHIVAL || count == MAX_HUDELEMS_CURRENT);

	for( inuse = 0; inuse < count; inuse++ )
	{
		if ( to[inuse].type == HE_TYPE_FREE )
		{
			break;
		}
	}

	MSG_WriteBits(msg, inuse, 5);

	for ( i = 0; i < inuse; i++ )
	{
		lc = 0;
		for (j = 0, field = hudElemFields; j < ARRAY_COUNT(hudElemFields); j++, field++)
		{
			fromF = ( int * )( (byte *)(from + i) + field->offset );
			toF = ( int * )( (byte *)(to + i) + field->offset );
			if ( *fromF != *toF )
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

/*
==============
MSG_WriteDeltaFields
==============
*/
void MSG_WriteDeltaFields( msg_t *msg, byte *from, byte *to, int lastChanged, int numFields, const NetField *stateFields )
{
	int *fromF, *toF;
	int i;
	const netField_t *field;

	if ( lastChanged )
	{
		MSG_WriteBit1(msg);

		for ( i = 0; i < numFields; i++ )
		{
			MSG_WriteDeltaField(msg, from, to, stateFields + i);
		}

		return;
	}

	for ( i = 0, field = stateFields; i < numFields; i++, field++ )
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );
		if ( *fromF != *toF )
		{
			MSG_WriteBit1(msg);

			for ( i = 0; i < numFields; i++ )
			{
				MSG_WriteDeltaField(msg, from, to, stateFields + i);
			}

			return;
		}
	}

	MSG_WriteBit0(msg);
}

/*
==============
MSG_WriteDeltaStruct
==============
*/
void MSG_WriteDeltaStruct( msg_t *msg, byte *from, byte *to, qboolean force, int numFields, int indexBits, const netField_t *stateFields, qboolean bChangeBit )
{
	int *fromF, *toF;
	int i, lc;
	const netField_t *field;

	if ( to == NULL )
	{
		if ( bChangeBit )
		{
			MSG_WriteBit1(msg);
		}

		MSG_WriteBits(msg, *(int *)from, indexBits);
		MSG_WriteBit1(msg);
		return;
	}

	lc = 0;
	for ( i = 0, field = stateFields; i < numFields; i++, field++ )
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );
		if ( *fromF != *toF )
		{
			lc = i + 1;
		}
	}

	assert(*reinterpret_cast< unsigned * >( to ) < (1u << indexBits));

	if ( lc )
	{
		if ( bChangeBit )
		{
			MSG_WriteBit1(msg);
		}

		MSG_WriteBits(msg, *(int *)to, indexBits);
		MSG_WriteBit0(msg);
		MSG_WriteBit1(msg);
		MSG_WriteByte(msg, lc);

		for (i = 0, field = stateFields; i < lc; i++, field++)
		{
			MSG_WriteDeltaField(msg, from, to, field);
		}
	}
	else
	{
		if ( !force )
		{
			return;
		}

		if ( bChangeBit )
		{
			MSG_WriteBit1(msg);
		}

		MSG_WriteBits(msg, *(int *)to, indexBits);
		MSG_WriteBit0(msg);
		MSG_WriteBit0(msg);
	}
}

/*
==============
MSG_ReadDeltaHudElems
==============
*/
void MSG_ReadDeltaHudElems( msg_t *msg, hudelem_t *from, hudelem_t *to, int count )
{
	int *fromF, *toF;
	int i, j, inuse, lc;
	netField_t *field;

	assert(count == MAX_HUDELEMS_ARCHIVAL || count == MAX_HUDELEMS_CURRENT);
	inuse = MSG_ReadBits(msg, 5);

	for ( i = 0; i < inuse; i++ )
	{
		lc = MSG_ReadBits(msg, 5);

		for (j = 0, field = hudElemFields; j <= lc; j++, field++)
		{
			MSG_ReadDeltaField(msg, (byte *)(from + i), (byte *)(to + i), field, qfalse);
		}

		for (j, field = hudElemFields; j < ARRAY_COUNT(hudElemFields); j++, field++)
		{
			fromF = ( int * )( (byte *)(from + i) + field->offset );
			toF = ( int * )( (byte *)(to + i) + field->offset );
			*fromF = *toF;
		}
	}

	for ( i = inuse; i < count; i++ )
	{
		if ( to[inuse].type == HE_TYPE_FREE )
		{
			continue;
		}

		memset(&to[inuse], 0, sizeof(*to));
		assert(to[inuse].type == HE_TYPE_FREE);
	}
}

/*
==============
MSG_ReadDeltaStruct
==============
*/
void MSG_ReadDeltaStruct( msg_t *msg, byte *from, byte *to, unsigned int number, int numFields, int indexBits, const netField_t *stateFields )
{
	const NetField *field;
	int *fromF, *toF;
	int lc;
	int i;

	if ( MSG_ReadBit(msg) == 1 )
	{
		return;
	}

	if ( !MSG_ReadBit(msg) )
	{
		Com_Memcpy(to, from, ( numFields + sizeof( from ) ) * sizeof( from ));
		return;
	}

	lc = MSG_ReadByte(msg);

	if ( lc > numFields )
	{
		msg->overflowed = qtrue;
		return;
	}

	*(int *)to = number;

	for (i = 0, field = stateFields; i < lc; i++, field++)
	{
		MSG_ReadDeltaField(msg, from, to, field, qfalse);
	}

	for (i = lc, field = stateFields + lc; i < numFields; i++, field++)
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );
		*toF = *fromF;
	}
}

/*
==============
MSG_ReadDeltaFields
==============
*/
void MSG_ReadDeltaFields( msg_t *msg, byte *from, byte *to, int numFields, const NetField *stateFields )
{
	const NetField *field;
	int *fromF, *toF;
	int i;

	if ( MSG_ReadBit(msg) )
	{
		for ( i = 0; i < numFields; i++ )
		{
			MSG_ReadDeltaField(msg, from, to, stateFields + i, qfalse);
		}
	}
	else
	{
		for ( i = 0, field = stateFields; i < numFields; i++, field++ )
		{
			fromF = ( int * )( (byte *)from + field->offset );
			toF = ( int * )( (byte *)to + field->offset );
			*toF = *fromF;
		}
	}
}