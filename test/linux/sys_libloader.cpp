#include "sys_patch.h"

#include "../../src/qcommon/qcommon.h"
#include "../../src/qcommon/cm_local.h"
#include "../../src/server/server.h"
#include "../../src/qcommon/sys_thread.h"
#include "../../src/clientscript/clientscript_public.h"

typedef struct
{
	int type;
	animStringItem_t            *values;
} animConditionTable_t;

#define net_fields (((MantleAnimTransition*)( 0x08164860 )))
#define animstrings (((animStringItem_t*)( 0x08164740 )))
#define globalScriptData (*((animScriptData_t**)( 0x0855A4E4 )))
#define level_bgs (*((bgs_t*)( 0x0859EA40 )))
#define test (*((vec3_t*)( 0x0815D6E8 )))
#define riflePriorityMap (((char*)( 0x0816778C )))

#define viewLerp_CrouchProne (((vec2_t*)( 0x08157C80 )))

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

#ifdef TESTING_LIBRARY
#define entityHandlers ((entityHandler_t*)( 0x08167880 ))
#else
const entityHandler_t entityHandlers[] =
{

};
#endif

/*
struct entityHandler_t
{
  void (__cdecl *think)(struct gentity_s *);
  void (__cdecl *reached)(struct gentity_s *);
  void (__cdecl *blocked)(struct gentity_s *, struct gentity_s *);
  void (__cdecl *touch)(struct gentity_s *, struct gentity_s *, int);
  void (__cdecl *use)(struct gentity_s *, struct gentity_s *, struct gentity_s *);
  void (__cdecl *pain)(struct gentity_s *, struct gentity_s *, int, const float *, const int, const float *, int);
  void (__cdecl *die)(struct gentity_s *, struct gentity_s *, struct gentity_s *, int, int, const int, const float *, int, int);
  void (__cdecl *controller)(struct gentity_s *, int *);
  int methodOfDeath;
  int splashMethodOfDeath;
};
*/


void test2()
{
	/*
	vec3_t start = { 1321, 369, 37 };
	vec3_t end = { 146, 723, 44 };

	vec3_t mins = { 0, 0, 0 };
	vec3_t maxs = { 0, 0, 0 };

	//int hitnum = CM_BoxSightTrace(0, start, end, mins, maxs, 0, 1);
	Com_Printf("(%f %f %f)\n", test[3], test[4], test[5]);
	*/

	//if (g_entities[1].client)
	//printf("%i\n ", g_entities[1].client->ps.iCompassFriendInfo);

	static int printed = 0;

	if (printed)
		return;

	printed = 1;

	//Scr_DumpScriptThreads();

	/*
		for (int i = 0; i < 132; i++)
			printf("{ \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", %i, %i, %i, %i, %i },\n", bg_itemlist[i].classname, bg_itemlist[i].pickup_sound, bg_itemlist[i].world_model, bg_itemlist[i].view_model, bg_itemlist[i].icon, bg_itemlist[i].pickup_name,
			bg_itemlist[i].quantity, bg_itemlist[i].giType, bg_itemlist[i].giWeaponIndex, bg_itemlist[i].giClipIndex, bg_itemlist[i].giSharedAmmoCapIndex);
			*/


	// Just print stuff until program segfaults
	for ( int i = 0; i < 20; ++i )
	{
		//if (net_fields[i].name == NULL)
		//	return;
		Com_Printf("{ %p, %p, %p, %p, %p, %p, %p, %p, %i, %i }\n", entityHandlers[i].think, entityHandlers[i].reached, entityHandlers[i].blocked, entityHandlers[i].touch, entityHandlers[i].use, entityHandlers[i].pain, entityHandlers[i].die, entityHandlers[i].controller, entityHandlers[i].methodOfDeath, entityHandlers[i].splashMethodOfDeath );
	}
}

void Sys_RedirectFunctions()
{
	//SetJump(0x08094F02, (DWORD)test2);
	//SetJump(0x0806283E, (DWORD)test2);




	// All referenced bsp funcs
	SetJump(0x08051C90, (DWORD)Com_LoadBsp);
	SetJump(0x08051E5C, (DWORD)Com_UnloadBsp);
	SetJump(0x08051C54, (DWORD)Com_IsBspLoaded);
	SetJump(0x08051C66, (DWORD)Com_GetBspHeader);
	SetJump(0x08051E7C, (DWORD)Com_CleanupBsp);



	// ALL referenced cm. funcs
	SetJump(0x08051E9C, (DWORD)CM_InitThreadData);
	SetJump(0x08051F9E, (DWORD)CM_LoadMap);
	SetJump(0x080524EA, (DWORD)CM_EntityString);
	SetJump(0x080524F4, (DWORD)CM_LeafCluster);
	SetJump(0x080525E0, (DWORD)CMod_LoadMaterials);
	SetJump(0x08052768, (DWORD)CMod_LoadSubmodels);
	SetJump(0x080529AE, (DWORD)CMod_GetPartitionScore);
	SetJump(0x08052B12, (DWORD)CMod_PartionLeafBrushes_r);
	SetJump(0x08052FD4, (DWORD)CMod_PartionLeafBrushes);
	SetJump(0x0805319E, (DWORD)CMod_GetLeafTerrainContents);
	SetJump(0x08053200, (DWORD)CMod_LoadSubmodelBrushNodes);
	SetJump(0x08053354, (DWORD)CMod_LoadNodes);
	SetJump(0x080534B8, (DWORD)CMod_LoadBrushes);
	SetJump(0x080538BC, (DWORD)CMod_LoadLeafs);
	SetJump(0x08053A80, (DWORD)CMod_LoadLeafBrushNodes);
	SetJump(0x08053D34, (DWORD)CMod_LoadLeafBrushes);
	SetJump(0x08053E0E, (DWORD)CMod_LoadLeafSurfaces);
	SetJump(0x08053EC2, (DWORD)CMod_LoadCollisionVerts);
	SetJump(0x08053FC0, (DWORD)CMod_LoadCollisionEdges);
	SetJump(0x08054220, (DWORD)CMod_LoadCollisionTriangles);
	SetJump(0x0805448C, (DWORD)CMod_LoadCollisionBorders);
	SetJump(0x0805461E, (DWORD)CMod_LoadCollisionPartitions);
	SetJump(0x0805474E, (DWORD)CMod_LoadCollisionAabbTrees);
	SetJump(0x080548DC, (DWORD)CMod_LoadEntityString);
	SetJump(0x08054936, (DWORD)CMod_LoadVisibility);
	SetJump(0x08054A26, (DWORD)CMod_LoadBrushRelated);
	SetJump(0x08054B6C, (DWORD)CM_LoadMapFromBsp);
	SetJump(0x08054D08, (DWORD)CM_InitBoxHull);
	SetJump(0x08054E4A, (DWORD)CM_Cleanup);
	SetJump(0x0805DD50, (DWORD)CM_AddStaticModelToNode);
	SetJump(0x080583FA, (DWORD)CM_LoadStaticModels);
	SetJump(0x0805DDD0, (DWORD)CM_SortNode);
	SetJump(0x0805E51A, (DWORD)CM_LinkAllStaticModels);
	SetJump(0x0805E7C6, (DWORD)CM_PointTraceStaticModels_r);
	SetJump(0x0805EAD0, (DWORD)CM_PointTraceStaticModelsComplete_r);
	SetJump(0x080553B2, (DWORD)CM_PositionTestCapsuleInTriangle);
	SetJump(0x080558EC, (DWORD)CM_TraceThroughAabbTree_r);
	SetJump(0x08055B48, (DWORD)CM_PositionTestInAabbTree_r);
	SetJump(0x08055C74, (DWORD)CM_TraceThroughAabbTree);
	SetJump(0x08055CFE, (DWORD)CM_SightTraceThroughAabbTree);
	SetJump(0x08055D4C, (DWORD)CM_MeshTestInLeaf);
	SetJump(0x0805628C, (DWORD)CM_TraceCapsuleThroughTriangle);
	SetJump(0x0805898C, (DWORD)CM_PointLeafnum_r);
	SetJump(0x08058A48, (DWORD)CM_StoreLeafs);
	SetJump(0x08058ABE, (DWORD)CM_BoxLeafnums_r);
	SetJump(0x08058BD8, (DWORD)CM_PointContentsLeafBrushNode_r);
	SetJump(0x08058D88, (DWORD)CM_PointContents);
	SetJump(0x08058F82, (DWORD)CM_ClusterPVS);
	SetJump(0x08059154, (DWORD)CM_ClipHandleToModel);
	SetJump(0x08059432, (DWORD)CM_TestInLeafBrushNode_r);
	SetJump(0x080595CA, (DWORD)CM_TestInLeafBrushNode);
	SetJump(0x08059A48, (DWORD)CM_PositionTest);
	SetJump(0x08059BBC, (DWORD)CM_TraceThroughBrush);
	SetJump(0x08059FDC, (DWORD)CM_TraceThroughLeafBrushNode_r);
	SetJump(0x0805A3A4, (DWORD)CM_TraceThroughLeafBrushNode);
	SetJump(0x0805A4AA, (DWORD)CM_TraceThroughLeaf);
	SetJump(0x0805AE54, (DWORD)CM_TraceThroughTree);
	SetJump(0x0805B9E2, (DWORD)CM_SightTraceThroughBrush);
	SetJump(0x0805BCEE, (DWORD)CM_SightTraceThroughLeafBrushNode_r);
	SetJump(0x0805C0BC, (DWORD)CM_SightTraceThroughLeafBrushNode);
	SetJump(0x0805C16E, (DWORD)CM_SightTraceThroughLeaf);
	SetJump(0x0805C9DE, (DWORD)CM_SightTraceThroughTree);
	SetJump(0x0805CCFA, (DWORD)CM_BoxSightTrace);


	SetJump(0x0805F0A4, (DWORD)CM_ClipMoveToEntities);


	SetJump(0x080596DA, (DWORD)CM_TestCapsuleInCapsule);
	SetJump(0x08059674, (DWORD)CM_TestInLeaf);
	SetJump(0x0805AA0E, (DWORD)CM_TraceCapsuleThroughCapsule);


	SetJump(0x0805D668, (DWORD)CM_CalcTraceExtents);
	SetJump(0x0805B198, (DWORD)CM_SetAxialCullOnly);

	SetJump(0x0805F61E, (DWORD)CM_PointTraceToEntities);


	SetJump(0x0805B210, (DWORD)CM_Trace);
	SetJump(0x0805B6F2, (DWORD)CM_TransformedBoxTrace);

	SetJump(0x0808FFBC, (DWORD)SV_inSnapshot);
	SetJump(0x08096C56, (DWORD)SV_AddCachedEntitiesVisibleFromPoint);
	SetJump(0x08097A28, (DWORD)SV_GetCachedSnapshot);
	SetJump(0x0809822C, (DWORD)SV_BuildClientSnapshot);
	SetJump(0x08097EDA, (DWORD)SV_GetArchivedClientInfo);

	SetJump(0x0809A45E, (DWORD)SV_LinkEntity);
	SetJump(0x0809A3BA, (DWORD)SV_UnlinkEntity);
	SetJump(0x0809AB88, (DWORD)SV_ClipMoveToEntity);
	SetJump(0x0809AD2C, (DWORD)SV_PointTraceToEntity);

	SetJump(0x0809B30E, (DWORD)SV_PointSightTraceToEntity);
	SetJump(0x0805D124, (DWORD)CM_TransformedBoxSightTrace);

	SetJump(0x0809B6A8, (DWORD)SV_Trace);
	SetJump(0x0809B9E2, (DWORD)SV_TracePassed);
	SetJump(0x0809BC5C, (DWORD)SV_SightTrace);

	SetJump(0x0805EA28, (DWORD)CM_PointTraceStaticModels);
	SetJump(0x0805F8C6, (DWORD)CM_PointSightTraceToEntities);

	SetJump(0x0809C18C, (DWORD)SV_PointContents);


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

	SetJump(0x080955A2, (DWORD)SV_Netchan_Transmit);
	SetJump(0x08095454, (DWORD)SV_Netchan_Encode);
	SetJump(0x080954DE, (DWORD)SV_Netchan_Decode);

	SetJump(0x0808C7CA, (DWORD)SV_AuthorizeIpPacket);

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



	// Dobj
	SetJump(0x080B6F64, (DWORD)DObjAbort);
	SetJump(0x080B6F34, (DWORD)DObjShutdown);
	SetJump(0x080B6EE8, (DWORD)DObjInit);
	SetJump(0x080B819E, (DWORD)DObjFree);
	SetJump(0x080B71A8, (DWORD)DObjCreateDuplicateParts);
	SetJump(0x080B7EFC, (DWORD)DObjCreate);
	SetJump(0x080B955A, (DWORD)DObjGeomTraceline);
	SetJump(0x080B8CA6, (DWORD)DObjTraceline);
	SetJump(0x080B73C0, (DWORD)DObjGetHierarchyBits);

	SetJump(0x0811BF30, (DWORD)G_DObjCalcPose);
	SetJump(0x0811BFC4, (DWORD)G_DObjCalcBone);


	// Dobj management
	SetJump(0x08062A66, (DWORD)Com_ServerDObjCreate);
	SetJump(0x08062B4C, (DWORD)Com_SafeServerDObjFree);
	SetJump(0x0806289C, (DWORD)Com_GetServerDObj);
	SetJump(0x08062BAC, (DWORD)Com_InitDObj);
	SetJump(0x08062C26, (DWORD)Com_ShutdownDObj);
	SetJump(0x08062C40, (DWORD)Com_AbortDObj);


	// SV Dobj stuff will be here
	SetJump(0x08090776, (DWORD)SV_DObjUpdateServerTime);
	SetJump(0x080905FA, (DWORD)SV_AllocSkelMemory);
	SetJump(0x080905C8, (DWORD)SV_ResetSkeletonCache);
	SetJump(0x08090666, (DWORD)SV_DObjCreateSkelForBone);
	SetJump(0x080906EE, (DWORD)SV_DObjCreateSkelForBones);



	// Random client stuff
	SetJump(0x080F5E80, (DWORD)SetClientViewAngle);
	SetJump(0x0811B0F8, (DWORD)G_GetModel);
	SetJump(0x0811B00C, (DWORD)G_ModelIndex);
	SetJump(0x0811B422, (DWORD)G_OverrideModel);
	SetJump(0x080F6506, (DWORD)ClientUserinfoChanged);

	SetJump(0x081079C8, (DWORD)G_RunThink);




	// All referenced XAnim stuff
	SetJump(0x080BA0EC, (DWORD)XAnimInit);
	SetJump(0x080BBA62, (DWORD)XAnimFreeInfo);
	SetJump(0x080BBAD0, (DWORD)XAnimGetAverageRateFrequency);
	SetJump(0x080BC382, (DWORD)XAnimUpdateInfoSyncInternal);
	SetJump(0x080BC52E, (DWORD)XAnimUpdateInfoInternal);
	SetJump(0x080BCB3A, (DWORD)XAnimUpdateOldTime);
	SetJump(0x080BCE56, (DWORD)XAnimGetServerNotifyFracSyncTotal);
	SetJump(0x080BCF74, (DWORD)XAnimFindServerNoteTrack);
	SetJump(0x080BD62C, (DWORD)XAnimCalc);
	SetJump(0x080BDE00, (DWORD)XAnimDisplay);
	SetJump(0x080BE1F4, (DWORD)XAnimCalcDeltaTree);
	SetJump(0x080BE7D2, (DWORD)XAnimGetTime);
	SetJump(0x080BF2AE, (DWORD)XAnimGetInfo);
	SetJump(0x080BF346, (DWORD)XAnimClearGoalWeight);
	SetJump(0x080BFB06, (DWORD)XAnimSetGoalWeightInternal);
	SetJump(0x080C00E2, (DWORD)XAnimSetTime);
	SetJump(0x080C01C8, (DWORD)XAnimRestart);
	SetJump(0x080C063C, (DWORD)XAnimCloneAnimTree);



	// Not referenced XAnim stuff, just for testing
	SetJump(0x080BA57A, (DWORD)XAnimBlend);
	SetJump(0x080BA626, (DWORD)XAnimCreateAnims);



	// Fields
	SetJump(0x080F5E1E, (DWORD)Scr_GetClientField);
	SetJump(0x080F5DBC, (DWORD)Scr_SetClientField);
	SetJump(0x080F5D66, (DWORD)GScr_AddFieldsForClient);
	SetJump(0x08118822, (DWORD)Scr_SetEntityField);
	SetJump(0x08118A22, (DWORD)Scr_GetEntityField);





	// !!! TEMP MEMORY !!!
	SetJump(0x08079C94, (DWORD)TempMalloc);
	SetJump(0x08079CC8, (DWORD)TempMallocAlign);
	SetJump(0x08079CDC, (DWORD)TempMallocAlignStrict);
	SetJump(0x08079CF0, (DWORD)TempMemorySetPos);
	SetJump(0x08079C84, (DWORD)TempMemoryReset);


	// !!! SCRIPT METHODS !!!
	SetJump(0x080FB640, (DWORD)Player_GetMethod);
	SetJump(0x08117D6A, (DWORD)ScriptEnt_GetMethod);
	SetJump(0x081158C2, (DWORD)BuiltIn_GetMethod);
	SetJump(0x0811595C, (DWORD)Scr_GetMethod);

	SetJump(0x08115824, (DWORD)Scr_GetFunction);


	// !!! COMPILER !!!
	SetJump(0x080759F2, (DWORD)Scr_GetCanonicalStringIndex);

	extern int GetExpressionCount(sval_u exprlist);
	SetJump(0x0806F96C, (DWORD)GetExpressionCount);
	extern void Scr_CompileRemoveRefToString(unsigned int stringValue);
	SetJump(0x0806D9EC, (DWORD)Scr_CompileRemoveRefToString);
	extern void EmitCanonicalString(unsigned int stringValue);
	SetJump(0x0806DA0A, (DWORD)EmitCanonicalString);
	extern void CompileTransferRefToString(unsigned int stringValue, unsigned char user);
	SetJump(0x0806DA94, (DWORD)CompileTransferRefToString);
	extern void EmitCodepos(const char *pos);
	SetJump(0x0806DF0E, (DWORD)EmitCodepos);
	extern void EmitShort(short value);
	SetJump(0x0806DFDE, (DWORD)EmitShort);
	SetJump(0x0806DF60, (DWORD)EmitShort);
	SetJump(0x0806DF32, (DWORD)EmitShort);

	SetJump(0x08076D92, (DWORD)AddOpcodePos);
	SetJump(0x08076FAE, (DWORD)RemoveOpcodePos);

	SetJump(0x0806DAD8, (DWORD)EmitOpcode);

	extern void LinkThread(unsigned int threadCountId, VariableValue *pos, bool allowFarCall);
	SetJump(0x0807098C, (DWORD)LinkThread);

	extern void EmitIncludeList(sval_u val);
	SetJump(0x080753A6, (DWORD)EmitIncludeList);

	extern void Scr_RegisterLocalVar(unsigned int name, sval_u sourcePos, scr_block_s *block);
	SetJump(0x0806E638, (DWORD)Scr_RegisterLocalVar);

	extern void Scr_CalcLocalVarsArrayPrimitiveExpressionRef(sval_u expr, scr_block_s *block);
	SetJump(0x08071D66, (DWORD)Scr_CalcLocalVarsArrayPrimitiveExpressionRef);

	extern void Scr_CopyBlock(scr_block_s *from, scr_block_s **to);
	SetJump(0x0806E69A, (DWORD)Scr_CopyBlock);

	extern void Scr_MergeChildBlocks(scr_block_s **childBlocks, int childCount, scr_block_s *block);
	SetJump(0x0806E94E, (DWORD)Scr_MergeChildBlocks);

	extern void Scr_AppendChildBlocks(scr_block_s **childBlocks, int childCount, scr_block_s *block);
	SetJump(0x0806E846, (DWORD)Scr_AppendChildBlocks);

	extern void Scr_CreateVector(VariableCompileValue *constValue, VariableValue *value);
	SetJump(0x08070DF8, (DWORD)Scr_CreateVector);

	extern void Scr_AddContinueBlock(scr_block_s *block);
	SetJump(0x0807270E, (DWORD)Scr_AddContinueBlock);

	extern bool EvalPrimitiveExpression(sval_u expr, VariableCompileValue *constValue);
	SetJump(0x08071180, (DWORD)EvalPrimitiveExpression);

	extern char EvalBinaryOperatorExpression(sval_u expr1, sval_u expr2, sval_u opcode, sval_u sourcePos, VariableCompileValue *constValue);
	SetJump(0x080716CE, (DWORD)EvalBinaryOperatorExpression);

	SetJump(0x08074842, (DWORD)Scr_CalcLocalVarsStatement);
	SetJump(0x08074ED8, (DWORD)Scr_CalcLocalVarsThread);

	extern unsigned int Scr_GetBuiltin(sval_u func_name);
	SetJump(0x080702D8, (DWORD)Scr_GetBuiltin);

	extern int Scr_FindLocalVarIndex(unsigned int name, sval_u sourcePos, bool create, scr_block_s *block);
	SetJump(0x0806E36A, (DWORD)Scr_FindLocalVarIndex);

	SetJump(0x0806CA94, (DWORD)Scr_EmitAnimation);

	SetJump(0x080704FA, (DWORD)EmitCall);
	SetJump(0x0806FADC, (DWORD)EmitFunction);
	SetJump(0x0807071E, (DWORD)EmitMethod);

	SetJump(0x080742D2, (DWORD)EmitStatement);
	SetJump(0x080751B4, (DWORD)EmitThread);

	SetJump(0x0807799E, (DWORD)Scr_AddSourceBuffer);
	SetJump(0x0810DF86, (DWORD)GScr_LoadScripts);

	SetJump(0x080837EC, (DWORD)VM_Execute);
	SetJump(0x08084FD2, (DWORD)Scr_RunCurrentThreads);
	SetJump(0x08083E96, (DWORD)Scr_InitSystem);
	SetJump(0x0807FA00, (DWORD)Scr_ErrorInternal);

	SetJump(0x08075ECE, (DWORD)Scr_PrecacheAnimTrees);
	SetJump(0x0806D8CE, (DWORD)Scr_FindAnimTree);



	SetJump(0x080B645E, (DWORD)ParseConfigStringToStruct);





	// DO NOT REMOVE
	SetJump(0x08120068, (DWORD)GScr_LoadConsts);



	// SCR functions
	SetJump(0x08111390, (DWORD)Scr_BulletTrace);



	// Game
	SetJump(0x08108270, (DWORD)TeleportPlayer);
	SetJump(0x0810950C, (DWORD)G_ClientStopUsingTurret);
	SetJump(0x0811B928, (DWORD)G_EntUnlink);
	SetJump(0x0810592C, (DWORD)G_RunCorpse);
	SetJump(0x080F2FA4, (DWORD)G_PlayerStateToEntityStateExtrapolate);

	// BGS
	SetJump(0x080EE226, (DWORD)BG_WeaponFireRecoil);
	SetJump(0x080D69B2, (DWORD)BG_PlayAnim);
	SetJump(0x080D6BC4, (DWORD)BG_ExecuteCommand);
	SetJump(0x080D6FCA, (DWORD)BG_AnimScriptEvent);
	SetJump(0x080D8882, (DWORD)BG_AnimPlayerConditions);
	SetJump(0x080D75DC, (DWORD)BG_SetNewAnimation);
	SetJump(0x080D7D06, (DWORD)BG_RunLerpFrameRate);
	SetJump(0x080D80B8, (DWORD)BG_SwingAngles);
	SetJump(0x080D9698, (DWORD)BG_PlayerAnimation);
	SetJump(0x080DCEB0, (DWORD)BG_EvaluateTrajectory);
	SetJump(0x080DD59E, (DWORD)BG_PlayerStateToEntityState);
	SetJump(0x080D978C, (DWORD)BG_UpdatePlayerDObj);
	SetJump(0x080D954C, (DWORD)BG_Player_DoControllers);
	SetJump(0x080DE734, (DWORD)BG_CheckProne);

	SetJump(0x080EFDF0, (DWORD)BG_CalculateWeaponPosition_Sway);
	SetJump(0x080EFD94, (DWORD)BG_CalculateViewMovementAngles);
	SetJump(0x080EF52C, (DWORD)BG_CalculateWeaponMovement);



	// Pmove
	SetJump(0x080EDF3A, (DWORD)PM_Weapon);
	SetJump(0x080DEBE4, (DWORD)PM_trace);

	SetJump(0x080EC2BA, (DWORD)PM_AdjustAimSpreadScale);
	SetJump(0x080DF16A, (DWORD)PM_Accelerate);
	SetJump(0x080E2CDE, (DWORD)PM_FootstepEvent);
	SetJump(0x080E7CC0, (DWORD)PM_StepSlideMove);
	SetJump(0x080DFB3A, (DWORD)PM_FlyMove);
	SetJump(0x080E178C, (DWORD)PM_ViewHeightAdjust);
	SetJump(0x080E1F20, (DWORD)PM_CheckDuck);
	SetJump(0x080DF868, (DWORD)PM_SetMovementDir);
	SetJump(0x080DFCD4, (DWORD)PM_AirMove);
	SetJump(0x080E5D4A, (DWORD)PM_LadderMove);
	SetJump(0x080DFE82, (DWORD)PM_WalkMove);
	SetJump(0x080E037E, (DWORD)PM_NoclipMove);
	SetJump(0x080E05C8, (DWORD)PM_UFOMove);
	SetJump(0x080E117E, (DWORD)PM_GroundTrace);
	SetJump(0x080E3008, (DWORD)PM_Footsteps);
	SetJump(0x080E4794, (DWORD)PM_UpdateViewAngles);
	SetJump(0x080E5936, (DWORD)PM_CheckLadderMove);


	SetJump(0x080E6D30, (DWORD)Pmove);


	// Mantle
	SetJump(0x080DAAF2, (DWORD)Mantle_GetAnimDelta);
	SetJump(0x080DAA8E, (DWORD)Mantle_GetAnim);
	SetJump(0x080DBB3C, (DWORD)Mantle_Move);
	SetJump(0x080DAE2E, (DWORD)Mantle_Start);
	SetJump(0x080DB964, (DWORD)Mantle_Check);
	SetJump(0x080DB380, (DWORD)Mantle_CreateAnims);
	SetJump(0x080DB6CA, (DWORD)Mantle_ShutdownAnims);



	// Game #2
	SetJump(0x0811EE84, (DWORD)Player_UpdateActivate);
	SetJump(0x080F24C6, (DWORD)G_TouchTriggers);
	SetJump(0x08115F2A, (DWORD)Scr_PlayerDamage);
	SetJump(0x080FF7C8, (DWORD)G_Damage);
	SetJump(0x0811AA18, (DWORD)G_CheckHitTriggerDamage);
	SetJump(0x0811DFF4, (DWORD)Bullet_Fire);
	SetJump(0x0811E0C2, (DWORD)Weapon_Throw_Grenade);
	SetJump(0x0811E18A, (DWORD)Weapon_RocketLauncher_Fire);
	SetJump(0x080F61BE, (DWORD)G_GetPlayerViewOrigin);
	SetJump(0x0811E3E0, (DWORD)FireWeapon);
	SetJump(0x0811D3B4, (DWORD)Weapon_Melee);
	SetJump(0x080FB79C, (DWORD)DeathmatchScoreboardMessage);
	SetJump(0x080FF2CA, (DWORD)player_die);
	SetJump(0x080F2B86, (DWORD)ClientEvents);
	SetJump(0x080F35DA, (DWORD)ClientThink_real);
	SetJump(0x080F3EA4, (DWORD)ClientThink);
	SetJump(0x08115AE2, (DWORD)Scr_ParseGameTypeList);
	SetJump(0x0811B20E, (DWORD)G_DObjUpdate);
	SetJump(0x08118320, (DWORD)G_ParseEntityFields);

	SetJump(0x081083B2, (DWORD)G_InitTurrets);
	SetJump(0x0810A21A, (DWORD)G_SpawnTurret);

	SetJump(0x081186D2, (DWORD)G_CallSpawnEntity);
	SetJump(0x081185EA, (DWORD)G_CallSpawn);


	// g_combat.cpp
	SetJump(0x080FF652, (DWORD)G_GetWeaponHitLocationMultiplier);
	SetJump(0x081003E6, (DWORD)G_GetHitLocationString);
	SetJump(0x081003F6, (DWORD)G_GetHitLocationIndexFromString);
	SetJump(0x080FEEDA, (DWORD)G_ParseHitLocDmgTable);



	// Server #2
	SetJump(0x0808E544, (DWORD)SV_WriteDownloadToClient);
	SetJump(0x080F6D74, (DWORD)ClientDisconnect);
	SetJump(0x0808DC8C, (DWORD)SV_DropClient);
	SetJump(0x0808F510, (DWORD)SV_UserMove);
	SetJump(0x0809479A, (DWORD)SV_BotUserMove);
	SetJump(0x0809443E, (DWORD)SV_CalcPings);
	SetJump(0x080945AC, (DWORD)SV_CheckTimeouts);
	SetJump(0x080989C6, (DWORD)SV_SendMessageToClient);
	SetJump(0x0808DECA, (DWORD)SV_SendClientGameState);
	SetJump(0x0808D0E6, (DWORD)SV_DirectConnect);
	SetJump(0x080951B4, (DWORD)SVC_RemoteCommand);
	SetJump(0x080963EC, (DWORD)SV_WriteSnapshotToClient);
	SetJump(0x08098C0E, (DWORD)SV_SendClientSnapshot);
	SetJump(0x0809315A, (DWORD)SV_VoicePacket);
	SetJump(0x08099EC6, (DWORD)SV_SendClientVoiceData);
	SetJump(0x0808BE54, (DWORD)SV_GetChallenge);
	SetJump(0x08093980, (DWORD)SVC_Info);
	SetJump(0x08093288, (DWORD)SVC_Status);
	SetJump(0x08093F1E, (DWORD)SV_ConnectionlessPacket);
	SetJump(0x0808F82A, (DWORD)SV_ExecuteClientMessage);
	SetJump(0x080941AE, (DWORD)SV_PacketEvent);
	SetJump(0x08098E3A, (DWORD)SV_ArchiveSnapshot);


	SetJump(0x08090BE6, (DWORD)SV_SetGametype);
	SetJump(0x0808FAC6, (DWORD)SV_AddTestClient);


	// g_cmds
	SetJump(0x080FE998, (DWORD)ClientCommand);


	// Items
	SetJump(0x08104EA2, (DWORD)IsItemRegistered);
	SetJump(0x08104C84, (DWORD)RegisterItem);
	SetJump(0x08104AEC, (DWORD)SaveRegisteredItems);
	SetJump(0x08104A02, (DWORD)ClearRegisteredItems);
	SetJump(0x08104D86, (DWORD)G_RegisterWeapon);
	//SetJump(0x0811E940, (DWORD)G_GetWeaponIndexForName);
	SetJump(0x08103D3C, (DWORD)LaunchItem);
	SetJump(0x0810404E, (DWORD)Drop_Weapon);


	// Scr functions
	SetJump(0x08118E82, (DWORD)Scr_GetEnt);
	SetJump(0x08118F7E, (DWORD)Scr_GetEntArray);
	SetJump(0x0810E9BE, (DWORD)GScr_SetDvar);
	SetJump(0x0810ECE4, (DWORD)GScr_GetAnimLength);
	SetJump(0x08110514, (DWORD)Scr_Objective_Add);
	SetJump(0x08111684, (DWORD)Scr_SightTracePassed);
	SetJump(0x081128EC, (DWORD)GScr_GetMoveDelta);
	SetJump(0x08112A04, (DWORD)GScr_GetAngleDelta);
	SetJump(0x08111B82, (DWORD)GScr_CastInt);
	SetJump(0x08112490, (DWORD)Scr_PrecacheItem);
	SetJump(0x08112FDE, (DWORD)Scr_PlayLoopedFX);
	SetJump(0x080FFE10, (DWORD)G_RadiusDamage);
	SetJump(0x08114C98, (DWORD)GScr_OpenFile);
	SetJump(0x08115138, (DWORD)GScr_FReadLn);
	SetJump(0x0810F392, (DWORD)ScrCmd_LinkTo);



	// Game #3
	SetJump(0x0810B4D8, (DWORD)G_ExplodeMissile);
	SetJump(0x0810381C, (DWORD)Touch_Item);
	SetJump(0x08109672, (DWORD)turret_think_client);
	SetJump(0x081099D6, (DWORD)turret_think);
	SetJump(0x0811FB7A, (DWORD)Player_UpdateLookAtEntity);
	SetJump(0x0811F736, (DWORD)Player_UpdateCursorHints);
	SetJump(0x080F20C8, (DWORD)P_DamageFeedback);
	SetJump(0x080F41C2, (DWORD)SpectatorClientEndFrame);
	SetJump(0x080F474A, (DWORD)StuckInClient);
	//SetJump(0x080F6ED0, (DWORD)G_GetNonPVSFriendlyInfo);
	SetJump(0x080F69B4, (DWORD)ClientSpawn);
	SetJump(0x080F4DBE, (DWORD)ClientEndFrame);
	
	
	
	SetJump(0x08105956, (DWORD)G_RunItem);
	SetJump(0x0810D966, (DWORD)G_RunMover);
	
	

	G_RegisterDvars(); // <-- FIX ME
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