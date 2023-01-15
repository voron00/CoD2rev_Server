#include "qcommon.h"

static huffman_t msgHuff;
static qboolean msgInit = qfalse;

typedef struct NetField
{
	const char *name;
	int offset;
	int bits;
} netField_t;

NetField objectiveFields[] =
{
	{ "origin[0]", 4, 0 },
	{ "origin[1]", 8, 0 },
	{ "origin[2]", 12, 0 },
	{ "entNum", 16, 10 },
	{ "teamNum", 20, 4 },
	{ "icon", 24, 12 }
};

NetField hudElemFields[] =
{
	{ "type", 0, 4 },
	{ "x", 4, -99 },
	{ "y", 8, -99 },
	{ "z", 12, -99 },
	{ "fontScale", 16, 0 },
	{ "font", 20, 4 },
	{ "alignOrg", 24, 4 },
	{ "alignScreen", 28, 6 },
	{ "color.rgba", 32, 32 },
	{ "fromColor.rgba", 36, 32 },
	{ "fadeStartTime", 40, 32 },
	{ "fadeTime", 44, 16 },
	{ "label", 48, 8 },
	{ "width", 52, 10 },
	{ "height", 56, 10 },
	{ "materialIndex", 60, 8 },
	{ "fromWidth", 64, 10 },
	{ "fromHeight", 68, 10 },
	{ "scaleStartTime", 72, 32 },
	{ "scaleTime", 76, 16 },
	{ "fromX", 80, -99 },
	{ "fromY", 84, -99 },
	{ "fromAlignOrg", 88, 4 },
	{ "fromAlignScreen", 92, 6 },
	{ "moveStartTime", 96, 32 },
	{ "moveTime", 100, 16 },
	{ "time", 104, 32 },
	{ "duration", 108, 32 },
	{ "value", 112, 0 },
	{ "text", 116, 8 },
	{ "sort", 120, 0 },
	{ "foreground", 124, 1 }
};

NetField playerStateFields[] =
{
	{ "commandTime", 0, 32 },
	{ "pm_type", 4, 8 },
	{ "bobCycle", 8, 8 },
	{ "pm_flags", 12, 27 },
	{ "pm_time", 16, -16 },
	{ "origin[0]", 20, 0 },
	{ "origin[1]", 24, 0 },
	{ "origin[2]", 28, 0 },
	{ "velocity[0]", 32, 0 },
	{ "velocity[1]", 36, 0 },
	{ "velocity[2]", 40, 0 },
	{ "weaponTime", 52, -16 },
	{ "weaponDelay", 56, -16 },
	{ "grenadeTimeLeft", 60, -16 },
	{ "weaponRestrictKickTime", 64, -16 },
	{ "foliageSoundTime", 68, 32 },
	{ "gravity", 72, 16 },
	{ "leanf", 76, 0 },
	{ "speed", 80, 16 },
	{ "delta_angles[0]", 84, 16 },
	{ "delta_angles[1]", 88, 16 },
	{ "delta_angles[2]", 92, 16 },
	{ "groundEntityNum", 96, 10 },
	{ "vLadderVec[0]", 100, 0 },
	{ "vLadderVec[1]", 104, 0 },
	{ "vLadderVec[2]", 108, 0 },
	{ "jumpTime", 112, 32 },
	{ "jumpOriginZ", 116, 0 },
	{ "legsTimer", 120, 16 },
	{ "legsAnim", 124, 10 },
	{ "torsoTimer", 128, 16 },
	{ "torsoAnim", 132, 10 },
	{ "damageTimer", 144, 16 },
	{ "damageDuration", 148, 16 },
	{ "flinchYaw", 152, 16 },
	{ "movementDir", 156, -8 },
	{ "eFlags", 160, 24 },
	{ "eventSequence", 164, 8 },
	{ "events[0]", 168, 8 },
	{ "events[1]", 172, 8 },
	{ "events[2]", 176, 8 },
	{ "events[3]", 180, 8 },
	{ "eventParms[0]", 184, 8 },
	{ "eventParms[1]", 188, 8 },
	{ "eventParms[2]", 192, 8 },
	{ "eventParms[3]", 196, 8 },
	{ "clientNum", 204, 8 },
	{ "offHandIndex", 208, 7 },
	{ "weapon", 212, 7 },
	{ "weaponstate", 216, 5 },
	{ "fWeaponPosFrac", 220, 0 },
	{ "adsDelayTime", 224, 32 },
	{ "viewmodelIndex", 228, 8 },
	{ "viewangles[0]", 232, -100 },
	{ "viewangles[1]", 236, -100 },
	{ "viewangles[2]", 240, -100 },
	{ "viewHeightTarget", 244, -8 },
	{ "viewHeightCurrent", 248, 0 },
	{ "viewHeightLerpTime", 252, 32 },
	{ "viewHeightLerpTarget", 256, -8 },
	{ "viewHeightLerpDown", 260, 1 },
	{ "viewHeightLerpPosAdj", 264, 0 },
	{ "viewAngleClampBase[0]", 268, 0 },
	{ "viewAngleClampBase[1]", 272, 0 },
	{ "viewAngleClampRange[0]", 276, 0 },
	{ "viewAngleClampRange[1]", 280, 0 },
	{ "damageEvent", 284, 8 },
	{ "damageYaw", 288, 8 },
	{ "damagePitch", 292, 8 },
	{ "damageCount", 296, 7 },
	{ "weapons[0]", 1348, 32 },
	{ "weapons[1]", 1352, 32 },
	{ "weaponslots[0]", 1364, 32 },
	{ "weaponslots[4]", 1368, 32 },
	{ "weaponrechamber[0]", 1372, 32 },
	{ "weaponrechamber[1]", 1376, 32 },
	{ "mins[0]", 1388, 0 },
	{ "mins[1]", 1392, 0 },
	{ "mins[2]", 1396, 0 },
	{ "maxs[0]", 1400, 0 },
	{ "maxs[1]", 1404, 0 },
	{ "maxs[2]", 1408, 0 },
	{ "proneDirection", 1412, 0 },
	{ "proneDirectionPitch", 1416, 0 },
	{ "proneTorsoPitch", 1420, 0 },
	{ "viewlocked", 1424, 8 },
	{ "viewlocked_entNum", 1428, 16 },
	{ "cursorHint", 1432, 8 },
	{ "cursorHintString", 1436, -8 },
	{ "iCompassFriendInfo", 1444, 32 },
	{ "fTorsoHeight", 1448, 0 },
	{ "fTorsoPitch", 1452, 0 },
	{ "fWaistPitch", 1456, 0 },
	{ "holdBreathScale", 1460, 0 },
	{ "holdBreathTimer", 1464, 16 },
	{ "mantleState.yaw", 1468, 0 },
	{ "mantleState.timer", 1472, 32 },
	{ "mantleState.transIndex", 1476, 4 },
	{ "weapAnim", 1488, 10 },
	{ "aimSpreadScale", 1492, 0 },
	{ "shellshockIndex", 1496, 4 },
	{ "shellshockTime", 1500, 32 },
	{ "shellshockDuration", 1504, 16 },
	{ "deltaTime", 1956, 32 }
};

NetField archivedEntityFields[] =
{
	{ "eType", 4, 8 },
	{ "eFlags", 8, 24 },
	{ "pos.trType", 12, 8 },
	{ "pos.trTime", 16, 32 },
	{ "pos.trDuration", 20, 32 },
	{ "pos.trBase[0]", 24, 0 },
	{ "pos.trBase[1]", 28, 0 },
	{ "pos.trBase[2]", 32, 0 },
	{ "pos.trDelta[0]", 36, 0 },
	{ "pos.trDelta[1]", 40, 0 },
	{ "pos.trDelta[2]", 44, 0 },
	{ "apos.trType", 48, 8 },
	{ "apos.trTime", 52, 32 },
	{ "apos.trDuration", 56, 32 },
	{ "apos.trBase[0]", 60, -100 },
	{ "apos.trBase[1]", 64, -100 },
	{ "apos.trBase[2]", 68, -100 },
	{ "apos.trDelta[0]", 72, 0 },
	{ "apos.trDelta[1]", 76, 0 },
	{ "apos.trDelta[2]", 80, 0 },
	{ "time", 84, 32 },
	{ "time2", 88, 32 },
	{ "origin2[0]", 92, 0 },
	{ "origin2[1]", 96, 0 },
	{ "origin2[2]", 100, 0 },
	{ "angles2[0]", 104, 0 },
	{ "angles2[1]", 108, 0 },
	{ "angles2[2]", 112, 0 },
	{ "otherEntityNum", 116, 10 },
	{ "attackerEntityNum", 120, 10 },
	{ "groundEntityNum", 124, 10 },
	{ "constantLight", 128, 32 },
	{ "loopSound", 132, 8 },
	{ "surfType", 136, 8 },
	{ "index", 140, 10 },
	{ "clientNum", 144, 8 },
	{ "iHeadIcon", 148, 4 },
	{ "iHeadIconTeam", 152, 2 },
	{ "solid", 156, 24 },
	{ "eventParm", 160, 8 },
	{ "eventSequence", 164, 8 },
	{ "events[0]", 168, 8 },
	{ "events[1]", 172, 8 },
	{ "events[2]", 176, 8 },
	{ "events[3]", 180, 8 },
	{ "eventParms[0]", 184, 8 },
	{ "eventParms[1]", 188, 8 },
	{ "eventParms[2]", 192, 8 },
	{ "eventParms[3]", 196, 8 },
	{ "weapon", 200, 7 },
	{ "legsAnim", 204, 10 },
	{ "torsoAnim", 208, 10 },
	{ "leanf", 212, 0 },
	{ "scale", 216, 8 },
	{ "dmgFlags", 220, 32 },
	{ "animMovetype", 224, 4 },
	{ "fTorsoHeight", 228, 0 },
	{ "fTorsoPitch", 232, 0 },
	{ "fWaistPitch", 236, 0 },
	{ "svFlags", 240, 32 },
	{ "clientMask[0]", 244, 32 },
	{ "clientMask[1]", 248, 32 },
	{ "absmin[0]", 252, 0 },
	{ "absmin[1]", 256, 0 },
	{ "absmin[2]", 260, 0 },
	{ "absmax[0]", 264, 0 },
	{ "absmax[1]", 268, 0 },
	{ "absmax[2]", 272, 0 }
};

NetField entityStateFields[] =
{
	{ "eType", 4, 8 },
	{ "eFlags", 8, 24 },
	{ "pos.trType", 12, 8 },
	{ "pos.trTime", 16, 32 },
	{ "pos.trDuration", 20, 32 },
	{ "pos.trBase[0]", 24, 0 },
	{ "pos.trBase[1]", 28, 0 },
	{ "pos.trBase[2]", 32, 0 },
	{ "pos.trDelta[0]", 36, 0 },
	{ "pos.trDelta[1]", 40, 0 },
	{ "pos.trDelta[2]", 44, 0 },
	{ "apos.trType", 48, 8 },
	{ "apos.trTime", 52, 32 },
	{ "apos.trDuration", 56, 32 },
	{ "apos.trBase[0]", 60, -100 },
	{ "apos.trBase[1]", 64, -100 },
	{ "apos.trBase[2]", 68, -100 },
	{ "apos.trDelta[0]", 72, 0 },
	{ "apos.trDelta[1]", 76, 0 },
	{ "apos.trDelta[2]", 80, 0 },
	{ "time", 84, 32 },
	{ "time2", 88, 32 },
	{ "origin2[0]", 92, 0 },
	{ "origin2[1]", 96, 0 },
	{ "origin2[2]", 100, 0 },
	{ "angles2[0]", 104, 0 },
	{ "angles2[1]", 108, 0 },
	{ "angles2[2]", 112, 0 },
	{ "otherEntityNum", 116, 10 },
	{ "attackerEntityNum", 120, 10 },
	{ "groundEntityNum", 124, 10 },
	{ "constantLight", 128, 32 },
	{ "loopSound", 132, 8 },
	{ "surfType", 136, 8 },
	{ "index", 140, 10 },
	{ "clientNum", 144, 8 },
	{ "iHeadIcon", 148, 4 },
	{ "iHeadIconTeam", 152, 2 },
	{ "solid", 156, 24 },
	{ "eventParm", 160, 8 },
	{ "eventSequence", 164, 8 },
	{ "events[0]", 168, 8 },
	{ "events[1]", 172, 8 },
	{ "events[2]", 176, 8 },
	{ "events[3]", 180, 8 },
	{ "eventParms[0]", 184, 8 },
	{ "eventParms[1]", 188, 8 },
	{ "eventParms[2]", 192, 8 },
	{ "eventParms[3]", 196, 8 },
	{ "weapon", 200, 7 },
	{ "legsAnim", 204, 10 },
	{ "torsoAnim", 208, 10 },
	{ "leanf", 212, 0 },
	{ "scale", 216, 8 },
	{ "dmgFlags", 220, 32 },
	{ "animMovetype", 224, 4 },
	{ "fTorsoHeight", 228, 0 },
	{ "fTorsoPitch", 232, 0 },
	{ "fWaistPitch", 236, 0 }
};

NetField clientStateFields[] =
{
	{ "team", 4, 2 },
	{ "modelindex", 8, 8 },
	{ "attachModelIndex[0]", 12, 8 },
	{ "attachModelIndex[1]", 16, 8 },
	{ "attachModelIndex[2]", 20, 8 },
	{ "attachModelIndex[3]", 24, 8 },
	{ "attachModelIndex[4]", 28, 8 },
	{ "attachModelIndex[5]", 32, 8 },
	{ "attachTagIndex[0]", 36, 5 },
	{ "attachTagIndex[1]", 40, 5 },
	{ "attachTagIndex[2]", 44, 5 },
	{ "attachTagIndex[3]", 48, 5 },
	{ "attachTagIndex[4]", 52, 5 },
	{ "attachTagIndex[5]", 56, 5 },
	{ "name[0]", 60, -98 },
	{ "name[4]", 64, -98 },
	{ "name[8]", 68, -98 },
	{ "name[12]", 72, -98 },
	{ "name[16]", 76, -98 },
	{ "name[20]", 80, -98 },
	{ "name[24]", 84, -98 },
	{ "name[28]", 88, -98 }
};

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