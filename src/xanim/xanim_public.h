#pragma once

enum XAssetType
{
	ASSET_TYPE_XMODELPIECES = 0x0,
	ASSET_TYPE_XMODEL = 0x1,
	ASSET_TYPE_XANIMPARTS = 0x2,
	ASSET_TYPE_MATERIAL = 0x3,
	ASSET_TYPE_TECHNIQUE_SET = 0x4,
	ASSET_TYPE_IMAGE = 0x5,
	ASSET_TYPE_SOUND = 0x6,
	ASSET_TYPE_SOUND_CURVE = 0x7,
	ASSET_TYPE_LOADED_SOUND = 0x8,
	ASSET_TYPE_CLIPMAP = 0x9,
	ASSET_TYPE_CLIPMAP_PVS = 0xA,
	ASSET_TYPE_COMWORLD = 0xB,
	ASSET_TYPE_GAMEWORLD_SP = 0xC,
	ASSET_TYPE_GAMEWORLD_MP = 0xD,
	ASSET_TYPE_MAP_ENTS = 0xE,
	ASSET_TYPE_GFXWORLD = 0xF,
	ASSET_TYPE_LIGHT_DEF = 0x10,
	ASSET_TYPE_UI_MAP = 0x11,
	ASSET_TYPE_FONT = 0x12,
	ASSET_TYPE_MENULIST = 0x13,
	ASSET_TYPE_MENU = 0x14,
	ASSET_TYPE_LOCALIZE_ENTRY = 0x15,
	ASSET_TYPE_WEAPON = 0x16,
	ASSET_TYPE_SNDDRIVER_GLOBALS = 0x17,
	ASSET_TYPE_FX = 0x18,
	ASSET_TYPE_IMPACT_FX = 0x19,
	ASSET_TYPE_AITYPE = 0x1A,
	ASSET_TYPE_MPTYPE = 0x1B,
	ASSET_TYPE_CHARACTER = 0x1C,
	ASSET_TYPE_XMODELALIAS = 0x1D,
	ASSET_TYPE_RAWFILE = 0x1E,
	ASSET_TYPE_STRINGTABLE = 0x1F,
	ASSET_TYPE_COUNT = 0x20,
};

union XAssetHeader
{
	struct XModelPieces *xmodelPieces;
	struct XModel *model;
	struct XAnimParts *parts;
	struct Material *material;
	struct MaterialPixelShader *pixelShader;
	struct MaterialVertexShader *vertexShader;
	struct MaterialTechniqueSet *techniqueSet;
	struct GfxImage *image;
	struct snd_alias_list_t *sound;
	struct SndCurve *sndCurve;
	struct LoadedSound_s *loadedsound;
	struct clipMap_s *clipMap;
	struct ComWorld *comWorld;
	struct GameWorldSp *gameWorldSp;
	struct GameWorldMp *gameWorldMp;
	struct MapEnts *mapEnts;
	struct GfxWorld *gfxWorld;
	struct GfxLightDef *lightDef;
	struct Font_s *font;
	struct MenuList *menuList;
	struct menuDef_t *menu;
	struct LocalizeEntry *localize;
	struct WeaponDef *weapon;
	struct SndDriverGlobals *sndDriverGlobals;
	struct FxEffectDef *fx;
	struct FxImpactTable *impactFx;
	struct RawFile *rawfile;
	struct StringTable *stringTable;
	void *data;
};


void DObjInit();
void DObjShutdown();

void XAnimFree(struct XAnimParts *parts);
void XAnimFreeList(struct XAnim_s *anims);

void XModelPartsFree(struct XModelPartsLoad *modelParts);
void XModelFree(struct XModel *model);

void XAnimInit();
void XAnimShutdown();