#include "../qcommon/qcommon.h"

infoParm_t infoParms[] =
{
	{ NULL, 0, 0, 0, 0 },
	{ "bark", 0, 1048576, 0, 0 },
	{ "brick", 0, 2097152, 0, 0 },
	{ "carpet", 0, 3145728, 0, 0 },
	{ "cloth", 0, 4194304, 0, 0 },
	{ "concrete", 0, 5242880, 0, 0 },
	{ "dirt", 0, 6291456, 0, 0 },
	{ "flesh", 0, 7340032, 0, 0 },
	{ "foliage", 1, 8388608, 2, 0 },
	{ "glass", 1, 9437184, 16, 0 },
	{ "grass", 0, 10485760, 0, 0 },
	{ "gravel", 0, 11534336, 0, 0 },
	{ "ice", 0, 12582912, 0, 0 },
	{ "metal", 0, 13631488, 0, 0 },
	{ "mud", 0, 14680064, 0, 0 },
	{ "paper", 0, 15728640, 0, 0 },
	{ "plaster", 0, 16777216, 0, 0 },
	{ "rock", 0, 17825792, 0, 0 },
	{ "sand", 0, 18874368, 0, 0 },
	{ "snow", 0, 19922944, 0, 0 },
	{ "water", 1, 20971520, 32, 0 },
	{ "wood", 0, 22020096, 0, 0 },
	{ "asphalt", 0, 23068672, 0, 0 },
	{ "opaqueglass", 0, 9437184, 0, 0 },

	{ "clipmissile", 1, 0, 128, 0 },
	{ "ai_nosight", 1, 0, 4096, 0 },
	{ "clipshot", 1, 0, 8192, 0 },
	{ "playerclip", 1, 0, 65536, 0 },
	{ "monsterclip", 1, 0, 131072, 0 },
	{ "vehicleclip", 1, 0, 512, 0 },
	{ "itemclip", 1, 0, 1024, 0 },
	{ "nodrop", 1, 0, -2147483648, 0 },
	{ "nonsolid", 1, 16384, 0, 0 },
	{ "detail", 0, 0, 134217728, 0 },
	{ "structural", 0, 0, 268435456, 0 },
	{ "portal", 1, -2147483648, 0, 0 },
	{ "canshootclip", 0, 0, 64, 0 },
	{ "origin", 1, 0, 0, 4 },
	{ "sky", 0, 4, 2048, 0 },
	{ "alphashadow", 0, 65536, 0, 0 },
	{ "nocastshadow", 0, 262144, 0, 0 },
	{ "slick", 0, 2, 0, 0 },
	{ "noimpact", 0, 16, 0, 0 },
	{ "nomarks", 0, 32, 0, 0 },
	{ "ladder", 0, 8, 0, 0 },
	{ "nodamage", 0, 1, 0, 0 },
	{ "mantleOn", 0, 33554432, 16777216, 0 },
	{ "mantleOver", 0, 67108864, 16777216, 0 },
	{ "nosteps", 0, 8192, 0, 0 },
	{ "nodraw", 0, 128, 0, 0 },
	{ "pointlight", 0, 2048, 0, 0 },
	{ "nolightmap", 0, 1024, 0, 0 },
	{ "nodlight", 0, 131072, 0, 0 },
};

const char* Com_SurfaceTypeToName(int iTypeIndex)
{
	if ( iTypeIndex <= 0 || iTypeIndex > 22 )
		return "default";
	else
		return infoParms[iTypeIndex].name;
}