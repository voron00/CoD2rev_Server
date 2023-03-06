#include "sys_patch.h"

#include "../../src/qcommon/qcommon.h"
#include "../../src/qcommon/cm_local.h"
#include "../../src/server/server.h"
#include "../../src/qcommon/sys_thread.h"
#include "../../src/clientscript/clientscript_public.h"

#define cm_original (*((clipMap_t*)( 0x08185BE0 )))
#define cme_original (*((clipMapExtra_t*)( 0x08185CF4 )))

extern clipMap_t cm;
extern clipMapExtra_t cme;

unsigned __int16 * gecrap(cLeafBrushNodeLeaf_t *val)
{
	return val->brushes;
}

#if 0
void test()
{
	static int loaded = 0;

	if (loaded)
		return;

	loaded = 1;

	Sys_InitMainThread();
	Dvar_Init();
	Swap_Init();
	FS_InitFilesystem();
	Com_InitHunkMemory();
	Com_LoadBsp("maps/mp/mp_toujane.d3dbsp");
	CM_LoadMapFromBsp("maps/mp/mp_toujane.d3dbsp", 1);
	CM_LoadStaticModels();

	//printf("%i\n", Com_Memcmp(&cm, &cm_original, sizeof(clipMap_t)));


	assert(cm_original.numStaticModels == cm.numStaticModels);



	//printf("%i\n", cm_original.numStaticModels);
	//printf("%i\n", cm.numStaticModels);

	/*
	for (unsigned int i = 0; i <  cm_original.numStaticModels; i++)
	{
		//bool result = Com_Memcmp(&cm.staticModelList[i], &cm_original.staticModelList[i], sizeof(cStaticModel_t));
		//assert(result == 0);
		printf("(%f %f %f) (%f %f %f)\n", cm.staticModelList[i].absmax[0], cm.staticModelList[i].absmax[1], cm.staticModelList[i].absmax[2],
										  cm_original.staticModelList[i].absmax[0], cm_original.staticModelList[i].absmax[1], cm_original.staticModelList[i].absmax[2]);
	}
	*/

	assert(cm_original.numMaterials == cm.numMaterials);

	for (unsigned int i = 0; i <  cm_original.numMaterials; i++)
	{
		bool result = Com_Memcmp(&cm.materials[i], &cm_original.materials[i], sizeof(dmaterial_t));
		assert(result == 0);

	}

	assert(cm_original.numBrushSides == cm.numBrushSides);

	for (unsigned int i = 0; i <  cm_original.numBrushSides; i++)
	{
		//bool result = Com_Memcmp(&cm.brushsides[i], &cm_original.brushsides[i], sizeof(cbrushside_t));
		//assert(result == 0);
		//printf("%i %i\n", cm.brushsides[i].materialNum, cm_original.brushsides[i].materialNum);
		//bool result = Com_Memcmp(&cm.brushsides[i].plane, &cm_original.brushsides[i].plane, sizeof(cplane_t));
		//assert(result == 0);
		//printf("(%f %f %f) (%f %f %f)\n", cm.brushsides[i].plane->normal[0], cm.brushsides[i].plane->normal[1], cm.brushsides[i].plane->normal[2],
		//								  cm_original.brushsides[i].plane->normal[0], cm_original.brushsides[i].plane->normal[1], cm_original.brushsides[i].plane->normal[2]);
		//printf("%i %i\n", cm.brushsides[i].plane->signbits, cm_original.brushsides[i].plane->signbits);
	}

	assert(cm_original.numNodes == cm.numNodes);

	for (unsigned int i = 0; i <  cm_original.numNodes; i++)
	{
		//bool result = Com_Memcmp(&cm.brushsides[i], &cm_original.brushsides[i], sizeof(cbrushside_t));
		//assert(result == 0);
		//printf("%i %i\n", cm.nodes[i].children[0], cm_original.nodes[i].children[0]);
		//bool result = Com_Memcmp(&cm.nodes[i].plane, &cm_original.nodes[i].plane, sizeof(cplane_t));
		//assert(result == 0);
		//printf("(%f %f %f) (%f %f %f)\n", cm.nodes[i].plane->normal[0], cm.nodes[i].plane->normal[1], cm.nodes[i].plane->normal[2],
		//	  cm_original.nodes[i].plane->normal[0], cm_original.nodes[i].plane->normal[1], cm_original.nodes[i].plane->normal[2]);
		//printf("%i %i\n", cm.nodes[i].plane->signbits, cm_original.nodes[i].plane->signbits);
	}

	assert(cm_original.leafbrushNodesCount == cm.leafbrushNodesCount);

	for (unsigned int i = 0; i <  cm_original.leafbrushNodesCount; i++)
	{
		assert(cm_original.leafbrushNodes->leafBrushCount == cm.leafbrushNodes->leafBrushCount);
		//bool result = Com_Memcmp(&cm.leafbrushNodes[i], &cm_original.leafbrushNodes[i], sizeof(cLeafBrushNode_t));
		//assert(result == 0);

		/*
		//for (unsigned int j = 0; j <  cm_original.leafbrushNodes->leafBrushCount; j++)
		{
			uint16_t *brush = gecrap(&cm_original.leafbrushNodes[i].data.leaf);

			printf("%u \n", brush[0]);
		}
		*/
		uint16_t *brush = cm_original.leafbrushNodes[i].data.leaf.brushes;
		uint16_t *brush2 = cm.leafbrushNodes[i].data.leaf.brushes;
		printf("%p %p\n", brush, brush2);
		//bool result = Com_Memcmp(&cm.nodes[i].plane, &cm_original.nodes[i].plane, sizeof(cplane_t));
		//assert(result == 0);
		//printf("(%f %f %f) (%f %f %f)\n", cm.nodes[i].plane->normal[0], cm.nodes[i].plane->normal[1], cm.nodes[i].plane->normal[2],
		//	  cm_original.nodes[i].plane->normal[0], cm_original.nodes[i].plane->normal[1], cm_original.nodes[i].plane->normal[2]);
		//printf("%i %i\n", cm.nodes[i].plane->signbits, cm_original.nodes[i].plane->signbits);
	}
}
#endif

extern void CMod_LoadBrushes();
extern void CMod_LoadLeafBrushes();
extern void CMod_LoadCollisionAabbTrees();
extern void CMod_LoadLeafs(bool usePvs);
extern void CMod_LoadSubmodels();
extern int CMod_GetLeafTerrainContents(cLeaf_s *leaf);
extern cLeafBrushNode_s* CMod_AllocLeafBrushNode();
extern double CMod_GetPartitionScore(uint16_t *leafBrushes, int numLeafBrushes, int axis, const float *mins, const float *maxs, float *dist);
extern cLeafBrushNode_s * CMod_PartionLeafBrushes_r(uint16_t *leafBrushes, int numLeafBrushes, const float *mins, const float *maxs);
extern void CMod_PartionLeafBrushes(uint16_t *leafBrushes, int numLeafBrushes, cLeaf_s *leaf);
extern void CMod_LoadLeafBrushNodes();
extern void CMod_LoadSubmodelBrushNodes();
extern void CM_InitBoxHull();
extern void CMod_LoadBrushRelated(bool usePvs);
extern void CMod_LoadPlanes();
extern void CMod_LoadMaterials();
extern void CMod_LoadNodes();
extern void CMod_LoadLeafSurfaces();
extern void CMod_LoadCollisionVerts();
extern void CMod_LoadCollisionEdges();
extern void CMod_LoadCollisionTriangles();
extern void CMod_LoadCollisionBorders();
extern void CMod_LoadCollisionPartitions();

void CMod_LoadLeafs_wrap(int a1, bool usePvs)
{
	CMod_LoadLeafs(usePvs);
}


void CMod_LoadBrushRelated_wrap(int a1, bool usePvs)
{
	CMod_LoadLeafs(usePvs);
}

#ifdef TESTING_LIBRARY
#define level_bgs (*((bgs_t*)( 0x0859EA40 )))
#else
extern bgs_t level_bgs;
#endif



//#define cm_world (*((cm_world_t*)( 0x08185D80 )))

#define net_fields (((cspField_t*)( 0x081665C0 )))
void test2()
{
	/*
	static int printed = 0;

	if (printed)
		return;

	printed = 1;
	// Just print stuff until program segfaults
	for(int i = 0; i < 16384; i++)
	{
		if (net_fields[i].szName == NULL)
			return;
		Com_Printf("{ \"%s\", %i, %i, },\n", net_fields[i].szName, net_fields[i].iOffset, net_fields[i].iFieldType);
	}
	*/

	//printf("%i\n", BG_GetWeaponDef(11)->unk);
}

void Sys_RedirectFunctions()
{
	/*
	SetJump(0x080534B8, (DWORD)CMod_LoadBrushes);
	SetJump(0x08053D34, (DWORD)CMod_LoadLeafBrushes);
	SetJump(0x0805474E, (DWORD)CMod_LoadCollisionAabbTrees);
	SetJump(0x080538BC, (DWORD)CMod_LoadLeafs_wrap);
	SetJump(0x08052768, (DWORD)CMod_LoadSubmodels);
	SetJump(0x0805319E, (DWORD)CMod_GetLeafTerrainContents);
	//SetJump(0x08052728, (DWORD)CMod_AllocLeafBrushNode);
	SetJump(0x080529AE, (DWORD)CMod_GetPartitionScore);
	//SetJump(0x08052B12, (DWORD)CMod_PartionLeafBrushes_r);
	//SetJump(0x08052FD4, (DWORD)CMod_PartionLeafBrushes);
	//SetJump(0x08053A80, (DWORD)CMod_LoadLeafBrushNodes);
	SetJump(0x08053200, (DWORD)CMod_LoadSubmodelBrushNodes);
	SetJump(0x08054D08, (DWORD)CM_InitBoxHull);
	//SetJump(0x08054A26, (DWORD)CMod_LoadBrushRelated_wrap);
	SetJump(0x08053B7E, (DWORD)CMod_LoadPlanes);
	SetJump(0x080525E0, (DWORD)CMod_LoadMaterials);
	SetJump(0x08053354, (DWORD)CMod_LoadNodes);
	SetJump(0x08053E0E, (DWORD)CMod_LoadLeafSurfaces);
	SetJump(0x08053EC2, (DWORD)CMod_LoadCollisionVerts);
	SetJump(0x08053FC0, (DWORD)CMod_LoadCollisionEdges);
	SetJump(0x08054220, (DWORD)CMod_LoadCollisionTriangles);
	SetJump(0x0805448C, (DWORD)CMod_LoadCollisionBorders);
	SetJump(0x0805461E, (DWORD)CMod_LoadCollisionPartitions);
	*/

	// SetJump(0x080583FA, (DWORD)CM_LoadStaticModels);

	//SetJump(0x08094F02, (DWORD)test);
	SetJump(0x08094F02, (DWORD)test2);


	//SetJump(0x080C501A, (DWORD)XModelGetStaticBounds);

	SetJump(0x08051C90, (DWORD)Com_LoadBsp);
	SetJump(0x08051E5C, (DWORD)Com_UnloadBsp);
	SetJump(0x08051F9E, (DWORD)CM_LoadMap);

	// Don't hook that for now, just init
	Swap_Init();

	// Common
	SetJump(0x08060B2C, (DWORD)Com_Printf);
	SetJump(0x08060B7C, (DWORD)Com_DPrintf);
	SetJump(0x080B5932, (DWORD)Com_sprintf);
	SetJump(0x080D3728, (DWORD)Sys_MilliSeconds);
	SetJump(0x08060E66, (DWORD)Com_Error);

	// Huffman
	SetJump(0x08066A70, (DWORD)Huff_Init);
	SetJump(0x0806610E, (DWORD)Huff_addRef);
	SetJump(0x080663F6, (DWORD)Huff_offsetReceive);
	SetJump(0x08066550, (DWORD)Huff_offsetTransmit);

	// MSG
	SetJump(0x08067718, (DWORD)MSG_Init);
	SetJump(0x0806ACDE, (DWORD)MSG_initHuffman);
	SetJump(0x0806775C, (DWORD)MSG_BeginReading);
	SetJump(0x080677A2, (DWORD)MSG_WriteBits);
	SetJump(0x08067868, (DWORD)MSG_WriteBit0);
	SetJump(0x080678C6, (DWORD)MSG_WriteBit1);
	SetJump(0x08067960, (DWORD)MSG_ReadBits);
	SetJump(0x08067A0E, (DWORD)MSG_ReadBit);
	SetJump(0x08067A8E, (DWORD)MSG_WriteBitsCompress);
	SetJump(0x08067AE2, (DWORD)MSG_ReadBitsCompress);
	SetJump(0x08067B4C, (DWORD)MSG_WriteByte);
	SetJump(0x08067B84, (DWORD)MSG_WriteData);
	SetJump(0x08067C2A, (DWORD)MSG_WriteLong);
	SetJump(0x08067BDA, (DWORD)MSG_WriteShort);
	SetJump(0x08067C78, (DWORD)MSG_ReadString);
	SetJump(0x08067CE4, (DWORD)MSG_WriteString);
	SetJump(0x08067D96, (DWORD)MSG_WriteBigString);
	SetJump(0x08067EE8, (DWORD)MSG_ReadByte);
	SetJump(0x08067F32, (DWORD)MSG_ReadShort);
	SetJump(0x08067F90, (DWORD)MSG_ReadLong);
	SetJump(0x08067FEC, (DWORD)MSG_ReadLong64);
	SetJump(0x080681EC, (DWORD)MSG_ReadData);
	SetJump(0x08068062, (DWORD)MSG_ReadCommandString);
	SetJump(0x080680CC, (DWORD)MSG_ReadBigString);
	SetJump(0x08068146, (DWORD)MSG_ReadStringLine);
	SetJump(0x080684E6, (DWORD)MSG_SetDefaultUserCmd);
	SetJump(0x08068ADE, (DWORD)MSG_ReadDeltaUsercmdKey);
	SetJump(0x08068E1C, (DWORD)MSG_WriteDeltaField);
	SetJump(0x080691FC, (DWORD)MSG_WriteDeltaStruct);
	SetJump(0x08069142, (DWORD)MSG_WriteDeltaObjective);
	SetJump(0x0806938C, (DWORD)MSG_WriteDeltaEntity);
	SetJump(0x080693DA, (DWORD)MSG_WriteDeltaArchivedEntity);
	SetJump(0x08069428, (DWORD)MSG_WriteDeltaClient);
	SetJump(0x080694A0, (DWORD)MSG_ReadDeltaField);
	SetJump(0x080697D8, (DWORD)MSG_ReadDeltaObjective);
	SetJump(0x0806988A, (DWORD)MSG_ReadDeltaStruct);
	SetJump(0x080699C8, (DWORD)MSG_ReadDeltaEntity);
	SetJump(0x08069A0E, (DWORD)MSG_ReadDeltaArchivedEntity);
	SetJump(0x08069A54, (DWORD)MSG_ReadDeltaClient);
	SetJump(0x08069AC4, (DWORD)MSG_WriteDeltaHudElems);
	SetJump(0x08069C10, (DWORD)MSG_ReadDeltaHudElems);
	SetJump(0x08069D40, (DWORD)MSG_WriteDeltaPlayerstate);
	SetJump(0x0806A608, (DWORD)MSG_ReadDeltaPlayerstate);

	// Threads
	SetJump(0x080D4444, (DWORD)Sys_InitMainThread);
	SetJump(0x080D446C, (DWORD)Sys_IsMainThread);
	SetJump(0x080D4492, (DWORD)Sys_SetValue);
	SetJump(0x080D44A4, (DWORD)Sys_GetValue);
	SetJump(0x080B59CE, (DWORD)va);
	SetJump(0x08051E9C, (DWORD)CM_InitThreadData);
	SetJump(0x080590DE, (DWORD)CM_TempBoxModel);

	// Com_Memory
	SetJump(0x080A95F0, (DWORD)Com_InitHunkMemory);
	SetJump(0x080A96D6, (DWORD)Hunk_Shutdown);
	SetJump(0x080A97A6, (DWORD)Hunk_FindDataForFile);
	SetJump(0x080A97DE, (DWORD)Hunk_DataOnHunk);
	SetJump(0x080A981E, (DWORD)Hunk_SetDataForFile);
	SetJump(0x080A98A0, (DWORD)Hunk_AddData);
	SetJump(0x080A98DC, (DWORD)Hunk_OverrideDataForFile);
	SetJump(0x080A9A04, (DWORD)Hunk_ClearData);
	SetJump(0x080A9B52, (DWORD)Hunk_SetMark);
	SetJump(0x080A9B5C, (DWORD)Hunk_ClearHigh);
	SetJump(0x080A9B80, (DWORD)Hunk_ClearLow);
	SetJump(0x080A9B9A, (DWORD)Hunk_Clear);
	SetJump(0x080A9BE0, (DWORD)Hunk_AllocInternal);
	SetJump(0x080A9BFC, (DWORD)Hunk_AllocAlignInternal);
	SetJump(0x080A9CF0, (DWORD)Hunk_AllocateTempMemoryHighInternal);
	SetJump(0x080A9DB4, (DWORD)Hunk_ClearTempMemoryHighInternal);
	SetJump(0x080A9DC4, (DWORD)Hunk_AllocLowInternal);
	SetJump(0x080A9DE0, (DWORD)Hunk_AllocLowAlignInternal);
	SetJump(0x080A9ECE, (DWORD)Hunk_ConvertTempToPermLowInternal);
	SetJump(0x080A9EDE, (DWORD)Hunk_AllocateTempMemoryInternal);
	SetJump(0x080AA00A, (DWORD)Hunk_ReallocateTempMemoryInternal);
	SetJump(0x080AA0C8, (DWORD)Hunk_FreeTempMemory);
	SetJump(0x080AA124, (DWORD)Hunk_ClearTempMemoryInternal);
	SetJump(0x080AA13C, (DWORD)Hunk_HideTempMemory);
	SetJump(0x080AA15A, (DWORD)Hunk_ShowTempMemory);

	// Dvar_Cmds
	SetJump(0x08062C50, (DWORD)Dvar_ForEach);
	SetJump(0x08062C80, (DWORD)Dvar_GetCombinedString);
	SetJump(0x08062D2C, (DWORD)Dvar_Command);
	// SetJump(0x08062E4C, (DWORD)Dvar_ToggleSimple);
	// SetJump(0x08063088, (DWORD)Dvar_ToggleInternal);
	// SetJump(0x08063222, (DWORD)Dvar_Toggle_f);
	// SetJump(0x08063230, (DWORD)Dvar_TogglePrint_f);
	// SetJump(0x08063288, (DWORD)Dvar_Set_f);
	// SetJump(0x08063328, (DWORD)Dvar_RegisterBool_f);
	// SetJump(0x080633F4, (DWORD)Dvar_RegisterInt_f);
	// SetJump(0x0806352E, (DWORD)Dvar_RegisterFloat_f);
	// SetJump(0x08063664, (DWORD)Dvar_SetU_f);
	// SetJump(0x080636BC, (DWORD)Dvar_SetS_f);
	// SetJump(0x08063714, (DWORD)Dvar_SetA_f);
	// SetJump(0x0806376C, (DWORD)Dvar_SetFromDvar_f);
	// SetJump(0x080637F2, (DWORD)Dvar_Reset_f);
	SetJump(0x08063844, (DWORD)Dvar_WriteVariables);
	SetJump(0x080638BE, (DWORD)Dvar_WriteDefaults);
	// SetJump(0x0806393A, (DWORD)Dvar_List_f);
	// SetJump(0x08063B04, (DWORD)Com_DvarDump);
	// SetJump(0x08063CC0, (DWORD)Dvar_Dump_f);
	SetJump(0x08063D30, (DWORD)Dvar_InfoString);
	SetJump(0x08063D92, (DWORD)Dvar_InfoString_Big);
	SetJump(0x08063DF4, (DWORD)Dvar_AddCommands);

	// Dvar
	SetJump(0x080AEEC8, (DWORD)Dvar_SetInAutoExec);
	SetJump(0x080AEEE0, (DWORD)Dvar_IsSystemActive);
	// SetJump(0x080AEEEC, (DWORD)generateHashValue);
	SetJump(0x080AEF62, (DWORD)Dvar_IsValidName);
	SetJump(0x080AF06A, (DWORD)Dvar_FreeString);
	SetJump(0x080AF080, (DWORD)Dvar_CopyString);
	// SetJump(0x080AF094, (DWORD)Dvar_FreeNameString);
	// SetJump(0x080AF43E, (DWORD)Dvar_EnumToString);
	SetJump(0x080AF474, (DWORD)Dvar_IndexStringToEnumString);
	// SetJump(0x080AF502, (DWORD)Dvar_ValueToString);
	SetJump(0x080AF6DE, (DWORD)Dvar_StringToBool);
	SetJump(0x080AF704, (DWORD)Dvar_StringToInt);
	SetJump(0x080AF718, (DWORD)Dvar_StringToFloat);
	SetJump(0x080AF738, (DWORD)Dvar_StringToVec2);
	SetJump(0x080AF782, (DWORD)Dvar_StringToVec3);
	SetJump(0x080AF7D6, (DWORD)Dvar_StringToVec4);
	SetJump(0x080AF834, (DWORD)Dvar_StringToEnum);
	// SetJump(0x080AF95E, (DWORD)Dvar_StringToColorInternal);
	SetJump(0x080AFB8C, (DWORD)Dvar_DisplayableValue);
	SetJump(0x080AFBB2, (DWORD)Dvar_DisplayableResetValue);
	SetJump(0x080AFBD8, (DWORD)Dvar_DisplayableLatchedValue);
	// SetJump(0x080AFF9A, (DWORD)Dvar_VectorDomainToString);
	// SetJump(0x080B009A, (DWORD)Dvar_DomainToString_Internal);
	SetJump(0x080B04A4, (DWORD)Dvar_PrintDomain);
	SetJump(0x080B04FE, (DWORD)Dvar_ValuesEqual);
	SetJump(0x080B06C4, (DWORD)Dvar_HasLatchedValue);
	// SetJump(0x080B06F8, (DWORD)Dvar_IsAtDefaultValue);
	SetJump(0x080B072A, (DWORD)Dvar_SetVariant);
	// SetJump(0x080B0A84, (DWORD)Dvar_FindMalleableVar);
	SetJump(0x080B0AE4, (DWORD)Dvar_FindVar);
	SetJump(0x080B0AF8, (DWORD)Dvar_ClearModified);
	SetJump(0x080B0BB6, (DWORD)Dvar_GetBool);
	SetJump(0x080B0C08, (DWORD)Dvar_GetInt);
	SetJump(0x080B0C60, (DWORD)Dvar_GetFloat);
	// SetJump(0x080B0CAC, (DWORD)Dvar_GetVec2);
	// SetJump(0x080B0CFA, (DWORD)Dvar_GetVec3);
	// SetJump(0x080B0D48, (DWORD)Dvar_GetVec4);
	SetJump(0x080B0D96, (DWORD)Dvar_GetString);
	SetJump(0x080B0DE0, (DWORD)Dvar_GetVariantString);
	// SetJump(0x080B0E28, (DWORD)Dvar_StringToColor);
	// SetJump(0x080B0ED2, (DWORD)Dvar_GetUnpackedColor);
	SetJump(0x080B0F16, (DWORD)Dvar_Shutdown);
	// SetJump(0x080B1334, (DWORD)Dvar_MakeExplicitType);
	// SetJump(0x080B168C, (DWORD)Dvar_Reregister);
	// SetJump(0x080B180C, (DWORD)Dvar_RegisterNew);
	// SetJump(0x080B1AC2, (DWORD)Dvar_RegisterVariant);
	SetJump(0x080B1B72, (DWORD)Dvar_RegisterBool);
	SetJump(0x080B1BDA, (DWORD)Dvar_RegisterInt);
	SetJump(0x080B1C2C, (DWORD)Dvar_RegisterFloat);
	SetJump(0x080B1DD2, (DWORD)Dvar_RegisterString);
	SetJump(0x080B1EA0, (DWORD)Dvar_RegisterColor);
	SetJump(0x080B1FDA, (DWORD)Dvar_SetBoolFromSource);
	SetJump(0x080B2030, (DWORD)Dvar_SetIntFromSource);
	SetJump(0x080B2096, (DWORD)Dvar_SetFloatFromSource);
	// SetJump(0x080B229C, (DWORD)Dvar_SetStringFromSource);
	SetJump(0x080B2460, (DWORD)Dvar_SetBool);
	SetJump(0x080B248A, (DWORD)Dvar_SetInt);
	SetJump(0x080B24AC, (DWORD)Dvar_SetFloat);
	SetJump(0x080B2560, (DWORD)Dvar_SetString);
	// SetJump(0x080B25BA, (DWORD)Dvar_SetFromStringFromSource);
	// SetJump(0x080B269C, (DWORD)Dvar_SetFromString);
	// SetJump(0x080B26BE, (DWORD)Dvar_SetBoolByName);
	// SetJump(0x080B2726, (DWORD)Dvar_SetIntByName);
	// SetJump(0x080B2792, (DWORD)Dvar_SetFloatByName);
	// SetJump(0x080B294E, (DWORD)Dvar_SetStringByName);
	SetJump(0x080B2AE4, (DWORD)Dvar_SetCommand);
	SetJump(0x080B2B42, (DWORD)Dvar_AddFlags);
	SetJump(0x080B2B5A, (DWORD)Dvar_Reset);
	SetJump(0x080B2BD0, (DWORD)Dvar_Init);

	// Cmd
	// SetJump(0x0805FA5E, (DWORD)Cmd_Wait_f);
	SetJump(0x0805FA96, (DWORD)Cbuf_Init);
	SetJump(0x0805FB06, (DWORD)Cbuf_AddText);
	SetJump(0x0805FB7E, (DWORD)Cbuf_InsertText);
	SetJump(0x0805FC38, (DWORD)Cbuf_ExecuteText);
	// SetJump(0x0805FCB2, (DWORD)Cbuf_ExecuteInternal);
	SetJump(0x0805FDFC, (DWORD)Cbuf_Execute);
	// SetJump(0x0805FE0A, (DWORD)Cmd_ExecFromDisk);
	// SetJump(0x0805FE66, (DWORD)Cmd_Exec_f);
	// SetJump(0x0805FEF2, (DWORD)Cmd_Vstr_f);
	SetJump(0x0805FFDC, (DWORD)Cmd_Argc);
	SetJump(0x0805FFE6, (DWORD)SV_Cmd_Argc);
	SetJump(0x0805FFF0, (DWORD)Cmd_Argv);
	SetJump(0x0806001C, (DWORD)SV_Cmd_Argv);
	SetJump(0x08060048, (DWORD)Cmd_ArgvBuffer);
	SetJump(0x08060074, (DWORD)SV_Cmd_ArgvBuffer);
	SetJump(0x080600A0, (DWORD)Cmd_Args);
	// SetJump(0x08060142, (DWORD)Cmd_TokenizeStringInternal);
	// SetJump(0x080603CC, (DWORD)Cmd_TokenizeStringWithLimit);
	// SetJump(0x080603FC, (DWORD)SV_Cmd_TokenizeStringWithLimit);
	SetJump(0x0806042C, (DWORD)Cmd_TokenizeString);
	SetJump(0x08060448, (DWORD)SV_Cmd_TokenizeString);
	// SetJump(0x08060464, (DWORD)Cmd_FindCommand);
	SetJump(0x080604B2, (DWORD)Cmd_AddCommand);
	SetJump(0x0806052A, (DWORD)Cmd_RemoveCommand);
	SetJump(0x08060598, (DWORD)Cmd_SetAutoComplete);
	SetJump(0x080605C0, (DWORD)Cmd_Shutdown);
	SetJump(0x080606A2, (DWORD)Cmd_ExecuteString);
	SetJump(0x08060754, (DWORD)SV_Cmd_ExecuteString);
	// SetJump(0x08060768, (DWORD)Cmd_List_f);
	SetJump(0x0806080A, (DWORD)Cmd_Init);

	// Filesystem
	SetJump(0x08060864, (DWORD)FS_FileRead);
	SetJump(0x08060892, (DWORD)FS_FileWrite);
	SetJump(0x080608BA, (DWORD)FS_FileOpen);
	SetJump(0x080608DA, (DWORD)FS_FileClose);
	SetJump(0x080608EE, (DWORD)FS_FileSeek);
	SetJump(0x08063F90, (DWORD)FS_SV_FOpenFileWrite);
	SetJump(0x08064100, (DWORD)FS_SV_FOpenFileRead);
	// SetJump(0x08064420, (DWORD)FS_SV_Rename);
	// SetJump(0x08064774, (DWORD)FS_GetModList);
	// SetJump(0x08064C7C, (DWORD)FS_Dir_f);
	// SetJump(0x08064D88, (DWORD)FS_NewDir_f);
	// SetJump(0x08064E98, (DWORD)FS_TouchFile_f);
	SetJump(0x08064ECC, (DWORD)FS_iwIwd);
	// SetJump(0x08064FF8, (DWORD)FS_CompareIwds);
	// SetJump(0x080652E2, (DWORD)FS_RemoveCommands);
	// SetJump(0x0806531A, (DWORD)FS_AddCommands);
	// SetJump(0x08065386, (DWORD)FS_SetRestrictions);
	SetJump(0x08065448, (DWORD)FS_LoadedIwdChecksums);
	SetJump(0x080654C0, (DWORD)FS_LoadedIwdNames);
	SetJump(0x0806554C, (DWORD)FS_LoadedIwdPureChecksums);
	SetJump(0x080655C4, (DWORD)FS_ReferencedIwdChecksums);
	SetJump(0x08065670, (DWORD)FS_ReferencedIwdNames);
	// SetJump(0x08065774, (DWORD)FS_ReferencedIwdPureChecksums);
	SetJump(0x080658D8, (DWORD)FS_PureServerSetLoadedIwds);
	SetJump(0x0809C444, (DWORD)FS_Initialized);
	// SetJump(0x0809C456, (DWORD)FS_CheckFileSystemStarted);
	// SetJump(0x0809C45C, (DWORD)FS_IwdIsPure);
	SetJump(0x0809C4BA, (DWORD)FS_LoadStack);
	// SetJump(0x0809C4C4, (DWORD)FS_UseSearchPath);
	SetJump(0x0809C4F4, (DWORD)FS_LanguageHasAssets);
	SetJump(0x0809C53E, (DWORD)FS_HashFileName);
	// SetJump(0x0809C5D2, (DWORD)FS_HandleForFile);
	// SetJump(0x0809C690, (DWORD)FS_FileForHandle);
	// SetJump(0x0809C6A4, (DWORD)FS_filelength);
	// SetJump(0x0809C7B4, (DWORD)FS_BuildOSPath_Internal);
	SetJump(0x0809C8AE, (DWORD)FS_BuildOSPath);
	// SetJump(0x0809C8DE, (DWORD)FS_CreatePath);
	SetJump(0x0809C974, (DWORD)FS_CopyFile);
	SetJump(0x0809CC52, (DWORD)FS_FCloseFile);
	// SetJump(0x0809CD3C, (DWORD)FS_GetHandleAndOpenFile);
	SetJump(0x0809CDDE, (DWORD)FS_FOpenFileWrite);
	SetJump(0x0809CE88, (DWORD)FS_FOpenTextFileWrite);
	// SetJump(0x0809CFDA, (DWORD)FS_FOpenFileAppend);
	SetJump(0x0809D12A, (DWORD)FS_FilenameCompare);
	// SetJump(0x0809D498, (DWORD)FS_PureIgnoreFiles);
	// SetJump(0x0809D55C, (DWORD)FS_IsBackupSubStr);
	// SetJump(0x0809D59E, (DWORD)FS_SanitizeFilename);
	// SetJump(0x0809D756, (DWORD)FS_FOpenFileRead_Internal);
	SetJump(0x0809E034, (DWORD)FS_FOpenFileReadStream);
	SetJump(0x0809E064, (DWORD)FS_FOpenFileRead);
	// SetJump(0x0809E09E, (DWORD)FS_TouchFile);
	SetJump(0x0809E328, (DWORD)FS_Read);
	SetJump(0x0809E454, (DWORD)FS_Write);
	SetJump(0x0809E52A, (DWORD)FS_Printf);
	SetJump(0x0809E892, (DWORD)FS_ReadFile);
	SetJump(0x0809E970, (DWORD)FS_ResetFiles);
	SetJump(0x0809E980, (DWORD)FS_FreeFile);
	// SetJump(0x0809EB54, (DWORD)FS_LoadZipFile);
	SetJump(0x0809F1B2, (DWORD)FS_ListFilteredFiles);
	SetJump(0x0809F7B2, (DWORD)FS_ListFiles);
	SetJump(0x0809FAE4, (DWORD)FS_FreeFileList);
	SetJump(0x0809FB40, (DWORD)FS_GetFileList);
	SetJump(0x0809FC4E, (DWORD)FS_ConvertPath);
	// SetJump(0x0809FC7C, (DWORD)FS_PathCmp);
	// SetJump(0x0809FD2A, (DWORD)FS_SortFileList);
	// SetJump(0x0809FE44, (DWORD)FS_DisplayPath);
	// SetJump(0x0809FF8E, (DWORD)FS_FullPath_f);
	// SetJump(0x0809FFA2, (DWORD)FS_Path_f);
	// SetJump(0x0809FFB6, (DWORD)IwdFileLanguage);
	// SetJump(0x080A0076, (DWORD)iwdsort);
	// SetJump(0x080A015C, (DWORD)FS_AddSearchPath);
	// SetJump(0x080A01A4, (DWORD)FS_AddIwdFilesForGameDirectory);
	// SetJump(0x080A04B4, (DWORD)FS_AddGameDirectoryInternal);
	// SetJump(0x080A0710, (DWORD)FS_AddGameDirectory);
	// SetJump(0x080A075C, (DWORD)FS_ShutdownSearchPaths);
	SetJump(0x080A07DE, (DWORD)FS_ShutdownServerIwdNames);
	SetJump(0x080A0838, (DWORD)FS_ShutdownServerReferencedIwds);
	SetJump(0x080A0892, (DWORD)FS_Shutdown);
	// SetJump(0x080A0920, (DWORD)FS_RegisterDvars);
	SetJump(0x080A0AC4, (DWORD)FS_Startup);
	SetJump(0x080A0F06, (DWORD)FS_ClearIwdReferences);
	SetJump(0x080A0F3E, (DWORD)FS_InitFilesystem);
	SetJump(0x080A1024, (DWORD)FS_Restart);
	// SetJump(0x080A119C, (DWORD)FS_ConditionalRestart);
	SetJump(0x080A1200, (DWORD)FS_FOpenFileByMode);
	// SetJump(0x080A1330, (DWORD)FS_FTell);
	SetJump(0x080A1380, (DWORD)FS_Flush);

	// NET
	SetJump(0x080D33EC, (DWORD)NET_Init);
	SetJump(0x080D3654, (DWORD)NET_Sleep);
	SetJump(0x080D2D34, (DWORD)Sys_GetPacket);
	SetJump(0x080D2EA8, (DWORD)Sys_SendPacket);

	Netchan_Init(1024); // <-- FIX ME
	// SetJump(0x0806B3FE, (DWORD)Netchan_Init);

	SetJump(0x0806AD14, (DWORD)NET_AdrToString);
	SetJump(0x0806AE1A, (DWORD)NetProf_PrepProfiling);
	SetJump(0x0806AF0A, (DWORD)NetProf_AddPacket);
	SetJump(0x0806AF90, (DWORD)NetProf_SendProfile);
	SetJump(0x0806B012, (DWORD)NetProf_ReceiveProfile);
	SetJump(0x0806B09A, (DWORD)NetProf_UpdateStatistics);
	// SetJump(0x0806B3D2, (DWORD)Net_DumpProfile_f);
	SetJump(0x0806B510, (DWORD)Netchan_Prepare);
	SetJump(0x0806B58C, (DWORD)Netchan_TransmitNextFragment);
	SetJump(0x0806B7AC, (DWORD)Netchan_Transmit);
	SetJump(0x0806B9CA, (DWORD)Netchan_Process);
	// SetJump(0x0806BE8E, (DWORD)NET_CompareBaseAdrSigned);
	SetJump(0x0806BF64, (DWORD)NET_CompareBaseAdr);
	// SetJump(0x0806BF92, (DWORD)NET_CompareAdrSigned);
	SetJump(0x0806C08C, (DWORD)NET_CompareAdr);
	SetJump(0x0806C0BA, (DWORD)NET_IsLocalAddress);
	SetJump(0x0806C0E0, (DWORD)NET_GetPacket);
	// SetJump(0x0806C0FA, (DWORD)NET_GetLoopPacketInternal);
	SetJump(0x0806C240, (DWORD)NET_GetLoopPacket);
	SetJump(0x0806C262, (DWORD)NET_SendLoopPacket);
	SetJump(0x0806C32C, (DWORD)NET_SendPacket);
	SetJump(0x0806C40C, (DWORD)NET_OutOfBandPrint);
	// SetJump(0x0806C57E, (DWORD)NET_OutOfBandData);
	// SetJump(0x0806C69C, (DWORD)NET_OutOfBandVoiceData);
	SetJump(0x0806C750, (DWORD)NET_StringToAdr);

	// SV_net_chan
	SetJump(0x0809558C, (DWORD)SV_Netchan_TransmitNextFragment);
	SetJump(0x080955EA, (DWORD)SV_Netchan_AddOOBProfilePacket);
	SetJump(0x08095626, (DWORD)SV_Netchan_SendOOBPacket);
	SetJump(0x08095696, (DWORD)SV_Netchan_UpdateProfileStats);
	SetJump(0x08095742, (DWORD)SV_Netchan_PrintProfileStats);

	// Server
	SV_Init(); // <-- FIX ME
	Com_InitDvars(); // <-- FIX ME

	SetJump(0x08063CD4, (DWORD)SV_SetConfig);
	SetJump(0x08092D5C, (DWORD)SV_AddServerCommand);
	SetJump(0x0809301C, (DWORD)SV_SendServerCommand);

	// Game
	SetJump(0x08107356, (DWORD)CalculateRanks);

	// MemoryTree
	SetJump(0x080766E6, (DWORD)MT_Init);
	SetJump(0x0807626A, (DWORD)MT_AddMemoryNode);
	SetJump(0x080763CA, (DWORD)MT_RemoveMemoryNode);
	SetJump(0x080765A0, (DWORD)MT_RemoveHeadMemoryNode);
	SetJump(0x080767AA, (DWORD)MT_GetSize);
	SetJump(0x0807682A, (DWORD)MT_AllocIndex);
	SetJump(0x080768E2, (DWORD)MT_FreeIndex);
	SetJump(0x08076A2A, (DWORD)MT_Alloc);
	SetJump(0x08076A50, (DWORD)MT_Free);
	SetJump(0x08076960, (DWORD)MT_RelocateNode);

	// SL
	SetJump(0x08078AB2, (DWORD)SL_CheckInit);
	SetJump(0x08078896, (DWORD)SL_ConvertToString);
	SetJump(0x08078946, (DWORD)SL_ConvertFromString);
	SetJump(0x08078962, (DWORD)GetHashCode);
	SetJump(0x08079544, (DWORD)SL_FreeString);
	SetJump(0x08078D94, (DWORD)SL_AddUserInternal);
	SetJump(0x08078DEC, (DWORD)SL_GetStringOfLen);
	SetJump(0x080796A2, (DWORD)SL_RemoveRefToString);
	SetJump(0x08078AE8, (DWORD)SL_FindStringOfLen);

	// Scr
	SetJump(0x0807C9CE, (DWORD)FindNextSibling);
	SetJump(0x0807A4D2, (DWORD)FindVariableIndex);
	SetJump(0x0807BB58, (DWORD)FindObjectVariable);
	SetJump(0x0807CB1E, (DWORD)FindObject);
	SetJump(0x0807B9A6, (DWORD)AddRefToVector);
	SetJump(0x0807B9CA, (DWORD)RemoveRefToVector);
	SetJump(0x0807AC94, (DWORD)MakeVariableExternal);
	SetJump(0x0807C144, (DWORD)GetNewObjectVariableReverse);
	SetJump(0x0807C0A8, (DWORD)GetVariable);
	SetJump(0x0807B4A0, (DWORD)FreeVariable);
	SetJump(0x0807C4C2, (DWORD)SetNewVariableValue);
	SetJump(0x0807B8D4, (DWORD)RemoveRefToEmptyObject);
	SetJump(0x0807B7BA, (DWORD)AddRefToObject);
	SetJump(0x0807578C, (DWORD)AddRefToValue);
	SetJump(0x0807B778, (DWORD)FreeChildValue);
	SetJump(0x0807AF82, (DWORD)ClearObject);
	SetJump(0x0807C16E, (DWORD)RemoveVariable);
	SetJump(0x0807C1CC, (DWORD)RemoveNextVariable);
	SetJump(0x0807C26C, (DWORD)RemoveArrayVariable);
	SetJump(0x0807B7DC, (DWORD)RemoveRefToObject);
	SetJump(0x080757AC, (DWORD)RemoveRefToValue);
	SetJump(0x0807B1DC, (DWORD)GetParentLocalId);
	SetJump(0x0807B1F0, (DWORD)GetSafeParentLocalId);
	SetJump(0x0807B3A8, (DWORD)AllocVariable);
	SetJump(0x0807B562, (DWORD)AllocValue);
	SetJump(0x0807B5EA, (DWORD)AllocEntity);
	SetJump(0x0807B5A0, (DWORD)AllocObject);
	SetJump(0x0807A368, (DWORD)Var_Init);
	SetJump(0x0807F91E, (DWORD)Scr_Init);

	SetJump(0x08084B88, (DWORD)Scr_AddUndefined);
	SetJump(0x08084AF8, (DWORD)Scr_AddBool);
	SetJump(0x08084B1C, (DWORD)Scr_AddInt);
	SetJump(0x08084B40, (DWORD)Scr_AddFloat);
	SetJump(0x08084B64, (DWORD)Scr_AddAnim);
	SetJump(0x08084BA2, (DWORD)Scr_AddObject);
	SetJump(0x08084BD2, (DWORD)Scr_AddEntityNum);
	SetJump(0x08084BF4, (DWORD)Scr_AddStruct);
	SetJump(0x08084C1A, (DWORD)Scr_AddString);
	SetJump(0x08084C8E, (DWORD)Scr_AddConstString);
	SetJump(0x08084CBE, (DWORD)Scr_AddVector);
	SetJump(0x08084D1C, (DWORD)Scr_AddArray);
	SetJump(0x08084D6E, (DWORD)Scr_AddArrayStringIndexed);
	SetJump(0x080823CE, (DWORD)VM_CancelNotify);
	SetJump(0x0807B25E, (DWORD)Scr_KillThread);
	SetJump(0x0807E5E0, (DWORD)Scr_EvalArray);
	SetJump(0x08118AAC, (DWORD)Scr_GetGenericField);
	SetJump(0x081188EE, (DWORD)Scr_SetGenericField);
	SetJump(0x08118CDE, (DWORD)Scr_GetEntity);
	SetJump(0x0808404C, (DWORD)Scr_GetInt);
	SetJump(0x08084320, (DWORD)Scr_GetFloat);
	SetJump(0x0808456A, (DWORD)Scr_GetString);
	SetJump(0x080846F8, (DWORD)Scr_GetVector);
	SetJump(0x0810E290, (DWORD)Scr_ConstructMessageString);
	SetJump(0x0807E1F6, (DWORD)Scr_AddClassField);

	SetJump(0x0807C2F6, (DWORD)CopyArray);
	SetJump(0x0807E856, (DWORD)Scr_EvalArrayRef);
	SetJump(0x0807C290, (DWORD)SafeRemoveVariable);
	SetJump(0x0807EB00, (DWORD)ClearArray);
	SetJump(0x0807BC7C, (DWORD)Scr_FindVariableField);
	SetJump(0x0807BF8E, (DWORD)ClearVariableField);
	SetJump(0x0808244A, (DWORD)VM_ArchiveStack);
	
	
	SetJump(0x08082EE2, (DWORD)Scr_TerminateThread);
	SetJump(0x08082F56, (DWORD)VM_Notify);
	
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! WARNING
	//SetJump(0x0807FB08, (DWORD)VM_ExecuteInternal);
	
	
	SetJump(0x08077DBA, (DWORD)Scr_PrintPrevCodePos);
	SetJump(0x0807C83C, (DWORD)Scr_EvalVariableField);
	SetJump(0x0807DDD2, (DWORD)Scr_EvalBinaryOperator);
	
	
	
	

	// ALL referenced Hud elem stuff
	SetJump(0x0810169E, (DWORD)Scr_GetHudElemField);
	SetJump(0x08101716, (DWORD)Scr_SetHudElemField);
	SetJump(0x0810178E, (DWORD)Scr_FreeHudElemConstStrings);
	SetJump(0x0810193A, (DWORD)GScr_AddFieldsForHudElems);

	SetJump(0x08100EDA, (DWORD)HudElem_Alloc);
	SetJump(0x08100F84, (DWORD)HudElem_ClientDisconnect);
	SetJump(0x08100FE6, (DWORD)HudElem_DestroyAll);
	SetJump(0x081023F2, (DWORD)HudElem_UpdateClient);
	SetJump(0x08118D3A, (DWORD)Scr_FreeHudElem);
	SetJump(0x08118D6E, (DWORD)Scr_AddHudElem);
	SetJump(0x0810236E, (DWORD)HudElem_GetMethod);



	SetJump(0x080B6F64, (DWORD)DObjAbort);
	SetJump(0x080B6F34, (DWORD)DObjShutdown);
	SetJump(0x080B6EE8, (DWORD)DObjInit);
	SetJump(0x080B819E, (DWORD)DObjFree);
	SetJump(0x080B71A8, (DWORD)DObjCreateDuplicateParts);


	SetJump(0x080B7EFC, (DWORD)DObjCreate);

	SetJump(0x08062A66, (DWORD)Com_ServerDObjCreate);
	SetJump(0x08062B4C, (DWORD)Com_ServerDObjFree);
	SetJump(0x0806289C, (DWORD)Com_GetServerDObj);
	SetJump(0x08062BAC, (DWORD)Com_InitDObj);
	SetJump(0x08062C26, (DWORD)Com_ShutdownDObj);
	SetJump(0x08062C40, (DWORD)Com_AbortDObj);



	SetJump(0x0809A45E, (DWORD)SV_LinkEntity);
	SetJump(0x0809A3BA, (DWORD)SV_UnlinkEntity);



	extern void XAnimDisplay(const XAnimTree_s *tree, unsigned int infoIndex, int depth);
	SetJump(0x080BDE00, (DWORD)XAnimDisplay);


	SetJump(0x080F5E80, (DWORD)SetClientViewAngle);

	SetJump(0x0811B0F8, (DWORD)G_CachedModelForIndex);
	SetJump(0x0811B00C, (DWORD)G_ModelIndex);
	SetJump(0x0811B422, (DWORD)G_OverrideModel);


	extern void CM_UnlinkEntity(svEntity_t *ent);
	SetJump(0x0805DAA0, (DWORD)CM_UnlinkEntity);

	extern int sub_805D8C4(float *mins, float *maxs);
	SetJump(0x0805D8C4, (DWORD)sub_805D8C4);

	extern void sub_805DD50(cStaticModel_s *model, unsigned short index);
	SetJump(0x0805DD50, (DWORD)sub_805DD50);

	extern void sub_805DCC8(svEntity_t *svEnt, unsigned short sectorId);
	SetJump(0x0805DCC8, (DWORD)sub_805DCC8);

	extern void CM_SortNode(unsigned short nodeIndex, float *mins, float *maxs);
	SetJump(0x0805DDD0, (DWORD)CM_SortNode);

	extern void CM_LinkEntity(svEntity_t *ent, float *absmin, float *absmax, clipHandle_t clipHandle);
	SetJump(0x0805E18C, (DWORD)CM_LinkEntity);












	SetJump(0x080F6506, (DWORD)ClientUserinfoChanged);






	SetJump(0x080F5E1E, (DWORD)Scr_GetClientField);
	SetJump(0x080F5DBC, (DWORD)Scr_SetClientField);
	SetJump(0x080F5D66, (DWORD)GScr_AddFieldsForClient);


	SetJump(0x08118822, (DWORD)Scr_SetEntityField);
	SetJump(0x08118A22, (DWORD)Scr_GetEntityField);

}

class cCallOfDuty2Pro
{
public:
	cCallOfDuty2Pro()
	{
		// dont inherit lib of parent
		unsetenv("LD_PRELOAD");

		// otherwise the printf()'s are printed at crash/end on older os/compiler versions
		setbuf(stdout, NULL);

		// allow to write in executable memory
		mprotect((void *)0x08048000, 0x135000, PROT_READ | PROT_WRITE | PROT_EXEC);

		Sys_RedirectFunctions();

		printf("> [COD2REV] Compiled %s %s using GCC %s\n", __DATE__, __TIME__, __VERSION__);
	}

	~cCallOfDuty2Pro()
	{
		printf("> [COD2REV] Library unloaded\n");
	}
};

cCallOfDuty2Pro *pro;

void __attribute__ ((constructor)) lib_load(void)
{
	pro = new cCallOfDuty2Pro;
}

void __attribute__ ((destructor)) lib_unload(void)
{
	delete pro;
}