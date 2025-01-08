#pragma once

#define SURF_START_BIT        20
#define SURF_TYPEINDEX( x ) ( ( x >> ( SURF_START_BIT ) ) & 0x1f )
#define SURF_INVALID          -1

enum
{
	SURF_TYPE_NONE,
	SURF_TYPE_BARK,
	SURF_TYPE_BRICK,
	SURF_TYPE_CARPET,
	SURF_TYPE_CLOTH,
	SURF_TYPE_CONCRETE,
	SURF_TYPE_DIRT,
	SURF_TYPE_FLESH,
	SURF_TYPE_FOLIAGE,
	SURF_TYPE_GLASS,
	SURF_TYPE_GRASS,
	SURF_TYPE_GRAVEL,
	SURF_TYPE_ICE,
	SURF_TYPE_METAL,
	SURF_TYPE_MUD,
	SURF_TYPE_PAPER,
	SURF_TYPE_PLASTER,
	SURF_TYPE_ROCK,
	SURF_TYPE_SAND,
	SURF_TYPE_SNOW,
	SURF_TYPE_WATER,
	SURF_TYPE_WOOD,
	SURF_TYPE_ASPHALT,
	SURF_TYPECOUNT
};

#define SURF_NODAMAGE              0x00000001
#define SURF_SLICK                 0x00000002
#define SURF_SKY                   0x00000004
#define SURF_LADDER                0x00000008
#define SURF_NOIMPACT              0x00000010
#define SURF_NOMARKS               0x00000020
#define SURF_NODRAW                0x00000080
#define SURF_HINT                  0x00000100
#define SURF_SKIP                  0x00000200
#define SURF_NOLIGHTMAP            0x00000400
#define SURF_POINTLIGHT            0x00000800
#define SURF_NOSTEPS               0x00002000
#define SURF_NONSOLID              0x00004000
#define SURF_ALPHASHADOW           0x00010000
#define SURF_NODLIGHT              0x00020000
#define SURF_NOCASTSHADOW          0x00040000
#define SURF_BARK                  0x00100000
#define SURF_BRICK                 0x00200000
#define SURF_CARPET                0x00300000
#define SURF_CLOTH                 0x00400000
#define SURF_CONCRETE              0x00500000
#define SURF_DIRT                  0x00600000
#define SURF_FLESH                 0x00700000
#define SURF_FOLIAGE               0x00800000
#define SURF_GLASS                 0x00900000
#define SURF_GRASS                 0x00A00000
#define SURF_GRAVEL                0x00B00000
#define SURF_ICE                   0x00C00000
#define SURF_METAL                 0x00D00000
#define SURF_MUD                   0x00E00000
#define SURF_PAPER                 0x00F00000
#define SURF_PLASTER               0x01000000
#define SURF_ROCK                  0x01100000
#define SURF_SAND                  0x01200000
#define SURF_SNOW                  0x01300000
#define SURF_WATER                 0x01400000
#define SURF_WOOD                  0x01500000
#define SURF_ASPHALT               0x01600000
#define SURF_MANTLEON              0x02000000
#define SURF_MANTLEOVER            0x04000000
#define SURF_PORTAL                0x80000000

#define SURF_OPAQUEGLASS SURF_GLASS

#define CONTENTS_NONE              0x00000000
#define CONTENTS_SOLID             0x00000001
#define CONTENTS_FOILAGE           0x00000002
#define CONTENTS_NONCOLLIDING      0x00000004
#define CONTENTS_LAVA              0x00000008
#define CONTENTS_GLASS             0x00000010
#define CONTENTS_WATER             0x00000020
#define CONTENTS_CANSHOTCLIP       0x00000040
#define CONTENTS_MISSILECLIP       0x00000080
#define CONTENTS_UNKNOWNCLIP       0x00000100
#define CONTENTS_VEHICLECLIP       0x00000200
#define CONTENTS_ITEMCLIP          0x00000400
#define CONTENTS_SKY               0x00000800
#define CONTENTS_AI_NOSIGHT        0x00001000
#define CONTENTS_CLIPSHOT          0x00002000
#define CONTENTS_MOVER             0x00004000
#define CONTENTS_AREAPORTAL        0x00008000
#define CONTENTS_PLAYERCLIP        0x00010000
#define CONTENTS_MONSTERCLIP       0x00020000
#define CONTENTS_TELEPORTER        0x00040000
#define CONTENTS_JUMPPAD           0x00080000
#define CONTENTS_CLUSTERPORTAL     0x00100000
#define CONTENTS_DONOTENTER        0x00200000
#define CONTENTS_DONOTENTER_LARGE  0x00400000
#define CONTENTS_UNKNOWN           0x00800000
#define CONTENTS_MANTLE            0x01000000
#define CONTENTS_BODY              0x02000000
#define CONTENTS_CORPSE            0x04000000
#define CONTENTS_DETAIL            0x08000000
#define CONTENTS_STRUCTURAL        0x10000000
#define CONTENTS_TRANSLUCENT       0x20000000
#define CONTENTS_TRIGGER           0x40000000
#define CONTENTS_NODROP            0x80000000

#define TOOL_OCCLUDER              0x00000001
#define TOOL_DRAWTOGGLE            0x00000002
#define TOOL_ORIGIN                0x00000004
#define TOOL_RADIALNORMALS         0x00000008

#define MASK_ALL                   ( -1 )
#define MASK_PLAYERSOLID           ( CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_PLAYERCLIP | CONTENTS_UNKNOWN | CONTENTS_BODY )