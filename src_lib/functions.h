#ifndef _FUNCTIONS_HPP_
#define _FUNCTIONS_HPP_

#include "declarations.h"

/* MAKE FUNCTIONS STATIC, SO THEY CAN BE IN EVERY FILE */

typedef unsigned int (*GetVariableName_t)(unsigned int a1);
#if COD_VERSION == COD2_1_0
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807CA72;
#elif COD_VERSION == COD2_1_2
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807CFF6;
#elif COD_VERSION == COD2_1_3
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807D0C2;
#endif

typedef unsigned int (*GetNextVariable_t)(unsigned int a1);
#if COD_VERSION == COD2_1_0
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807C9CE;
#elif COD_VERSION == COD2_1_2
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807CF52;
#elif COD_VERSION == COD2_1_3
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807D01E;
#endif

typedef int (*GetArraySize_t)(unsigned int a1);
#if COD_VERSION == COD2_1_0
static const GetArraySize_t GetArraySize = (GetArraySize_t)0x0807C9AC;
#elif COD_VERSION == COD2_1_2
static const GetArraySize_t GetArraySize = (GetArraySize_t)0x0807CF30;
#elif COD_VERSION == COD2_1_3
static const GetArraySize_t GetArraySize = (GetArraySize_t)0x0807CFFC;
#endif

typedef int (*GetVarType_t)(unsigned int a1);
#if COD_VERSION == COD2_1_0
static const GetVarType_t GetVarType = (GetVarType_t)0x0807DF7C;
#elif COD_VERSION == COD2_1_2
static const GetVarType_t GetVarType = (GetVarType_t)0x0807E500;
#elif COD_VERSION == COD2_1_3
static const GetVarType_t GetVarType = (GetVarType_t)0x0807E5CC;
#endif

typedef short (*BigShort_t)(short l);
#if COD_VERSION == COD2_1_0
static const BigShort_t BigShort = (BigShort_t)0x080B4F9A;
#elif COD_VERSION == COD2_1_2
static const BigShort_t BigShort = (BigShort_t)0x080B742E;
#elif COD_VERSION == COD2_1_3
static const BigShort_t BigShort = (BigShort_t)0x080B7572;
#endif

typedef char * (*SL_ConvertToString_t)(unsigned int index);
#if COD_VERSION == COD2_1_0
static const SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x08078896;
#elif COD_VERSION == COD2_1_2
static const SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x08078E1A;
#elif COD_VERSION == COD2_1_3
static const SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x08078EE6;
#endif

typedef unsigned int (*SL_GetString_t)(const char *str, int type);
#if COD_VERSION == COD2_1_0
static const SL_GetString_t SL_GetString = (SL_GetString_t)0x08079290;
#elif COD_VERSION == COD2_1_2
static const SL_GetString_t SL_GetString = (SL_GetString_t)0x08079814;
#elif COD_VERSION == COD2_1_3
static const SL_GetString_t SL_GetString = (SL_GetString_t)0x080798E0;
#endif

typedef void (*SL_RemoveRefToString_t)(unsigned int);
#if COD_VERSION == COD2_1_0
static const SL_RemoveRefToString_t SL_RemoveRefToString = (SL_RemoveRefToString_t)0x080796A2;
#elif COD_VERSION == COD2_1_2
static const SL_RemoveRefToString_t SL_RemoveRefToString = (SL_RemoveRefToString_t)0x08079C26;
#elif COD_VERSION == COD2_1_3
static const SL_RemoveRefToString_t SL_RemoveRefToString = (SL_RemoveRefToString_t)0x08079CF2;
#endif

typedef int (*Scr_GetFunctionHandle_t)(const char* scriptName, const char* labelName, int isNeeded);
#if COD_VERSION == COD2_1_0
static const Scr_GetFunctionHandle_t Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)0x0810DD70;
#elif COD_VERSION == COD2_1_2
static const Scr_GetFunctionHandle_t Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)0x081100AC;
#elif COD_VERSION == COD2_1_3
static const Scr_GetFunctionHandle_t Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)0x08110208;
#endif

typedef char * (*Cmd_Argv_t)(int arg);
#if COD_VERSION == COD2_1_0
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x0806001C;
#elif COD_VERSION == COD2_1_2
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x08060228;
#elif COD_VERSION == COD2_1_3
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x08060220;
#endif

typedef int (*Cmd_Argc_t)();
#if COD_VERSION == COD2_1_0
static const Cmd_Argc_t Cmd_Argc = (Cmd_Argc_t)0x0805FFDC;
#elif COD_VERSION == COD2_1_2
static const Cmd_Argc_t Cmd_Argc = (Cmd_Argc_t)0x080601E8;
#elif COD_VERSION == COD2_1_3
static const Cmd_Argc_t Cmd_Argc = (Cmd_Argc_t)0x080601E0;
#endif

typedef int (*Scr_GetNumParam_t)(void);
#if COD_VERSION == COD2_1_0
static const Scr_GetNumParam_t Scr_GetNumParam = (Scr_GetNumParam_t)0x08084AEE;
#elif COD_VERSION == COD2_1_2
static const Scr_GetNumParam_t Scr_GetNumParam = (Scr_GetNumParam_t)0x0808506A;
#elif COD_VERSION == COD2_1_3
static const Scr_GetNumParam_t Scr_GetNumParam = (Scr_GetNumParam_t)0x08085136;
#endif

typedef void (*SV_Cmd_ArgvBuffer_t)(int arg, char *buffer, int bufferLength);
#if COD_VERSION == COD2_1_0
static const SV_Cmd_ArgvBuffer_t SV_Cmd_ArgvBuffer = (SV_Cmd_ArgvBuffer_t)0x08060074;
#elif COD_VERSION == COD2_1_2
static const SV_Cmd_ArgvBuffer_t SV_Cmd_ArgvBuffer = (SV_Cmd_ArgvBuffer_t)0x08060280;
#elif COD_VERSION == COD2_1_3
static const SV_Cmd_ArgvBuffer_t SV_Cmd_ArgvBuffer = (SV_Cmd_ArgvBuffer_t)0x08060278;
#endif

typedef void (*Com_Printf_t)(const char *format, ...);
#if COD_VERSION == COD2_1_0
static const Com_Printf_t Com_Printf = (Com_Printf_t)0x08060B2C;
#elif COD_VERSION == COD2_1_2
static const Com_Printf_t Com_Printf = (Com_Printf_t)0x08060DF2;
#elif COD_VERSION == COD2_1_3
static const Com_Printf_t Com_Printf = (Com_Printf_t)0x08060DEA;
#endif

typedef void (*Com_DPrintf_t)(const char *format, ...);
#if COD_VERSION == COD2_1_0
static const Com_DPrintf_t Com_DPrintf = (Com_DPrintf_t)0x08060B7C;
#elif COD_VERSION == COD2_1_2
static const Com_DPrintf_t Com_DPrintf = (Com_DPrintf_t)0x08060E42;
#elif COD_VERSION == COD2_1_3
static const Com_DPrintf_t Com_DPrintf = (Com_DPrintf_t)0x08060E3A;
#endif

typedef int (*Com_sprintf_t)(char *dest, int size, const char *format, ...);
#if COD_VERSION == COD2_1_0
static const Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x080B5932;
#elif COD_VERSION == COD2_1_2
static const Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x080B7DC6;
#elif COD_VERSION == COD2_1_3
static const Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x080B7F0A;
#endif

typedef void (*Cmd_AddCommand_t)(const char *cmd_name, xcommand_t function);
#if COD_VERSION == COD2_1_0
static const Cmd_AddCommand_t Cmd_AddCommand = (Cmd_AddCommand_t)0x080604B2;
#elif COD_VERSION == COD2_1_2
static const Cmd_AddCommand_t Cmd_AddCommand = (Cmd_AddCommand_t)0x080606BE;
#elif COD_VERSION == COD2_1_3
static const Cmd_AddCommand_t Cmd_AddCommand = (Cmd_AddCommand_t)0x080606B6;
#endif

typedef void (*Cmd_ExecuteString_t)(const char *cmd);
#if COD_VERSION == COD2_1_0
static const Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x08060754;
#elif COD_VERSION == COD2_1_2
static const Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x080609D4;
#elif COD_VERSION == COD2_1_3
static const Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x080609CC;
#endif

typedef void (*ClientCommand_t)(int clientNum);
#if COD_VERSION == COD2_1_0
static const ClientCommand_t ClientCommand = (ClientCommand_t)0x080FE998;
#elif COD_VERSION == COD2_1_2
static const ClientCommand_t ClientCommand = (ClientCommand_t)0x08100D1E;
#elif COD_VERSION == COD2_1_3
static const ClientCommand_t ClientCommand = (ClientCommand_t)0x08100E62;
#endif

typedef int (*FS_ReadFile_t)(const char *qpath, void **buffer);
#if COD_VERSION == COD2_1_0
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x0809E892;
#elif COD_VERSION == COD2_1_2
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x080A0958;
#elif COD_VERSION == COD2_1_3
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x080A0A9C;
#endif

typedef void (*FS_LoadDir_t)(char *path, char *dir);
#if COD_VERSION == COD2_1_0
static const FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x080A01A4;
#elif COD_VERSION == COD2_1_2
static const FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x080A22D8;
#elif COD_VERSION == COD2_1_3
static const FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x080A241C;
#endif

typedef void (*FS_Printf_t)(fileHandle_t h, const char *fmt, ...);
#if COD_VERSION == COD2_1_0
static const FS_Printf_t FS_Printf = (FS_Printf_t)0x0809E52A;
#elif COD_VERSION == COD2_1_2
static const FS_Printf_t FS_Printf = (FS_Printf_t)0x080A05E8;
#elif COD_VERSION == COD2_1_3
static const FS_Printf_t FS_Printf = (FS_Printf_t)0x080A072C;
#endif

typedef fileHandle_t (*FS_FOpenFileWrite_t)(const char* filename);
#if COD_VERSION == COD2_1_0
static const FS_FOpenFileWrite_t FS_FOpenFileWrite = (FS_FOpenFileWrite_t)0x0809CDDE;
#elif COD_VERSION == COD2_1_2
static const FS_FOpenFileWrite_t FS_FOpenFileWrite = (FS_FOpenFileWrite_t)0x0809EE9C;
#elif COD_VERSION == COD2_1_3
static const FS_FOpenFileWrite_t FS_FOpenFileWrite = (FS_FOpenFileWrite_t)0x0809EFE0;
#endif

typedef qboolean (*FS_FCloseFile_t)(fileHandle_t f);
#if COD_VERSION == COD2_1_0
static const FS_FCloseFile_t FS_FCloseFile = (FS_FCloseFile_t)0x0809CC52;
#elif COD_VERSION == COD2_1_2
static const FS_FCloseFile_t FS_FCloseFile = (FS_FCloseFile_t)0x0809ED10;
#elif COD_VERSION == COD2_1_3
static const FS_FCloseFile_t FS_FCloseFile = (FS_FCloseFile_t)0x0809EE54;
#endif

typedef xfunction_t (*Scr_GetFunction_t)(const char** v_functionName, qboolean* v_developer);
#if COD_VERSION == COD2_1_0
static const Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08115824;
#elif COD_VERSION == COD2_1_2
static const Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08117B56;
#elif COD_VERSION == COD2_1_3
static const Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08117CB2;
#endif

typedef xmethod_t (*Scr_GetMethod_t)(const char** v_methodName, qboolean* v_developer);
#if COD_VERSION == COD2_1_0
static const Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x0811595C;
#elif COD_VERSION == COD2_1_2
static const Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x08117C8E;
#elif COD_VERSION == COD2_1_3
static const Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x08117DEA;
#endif

typedef void (*Cvar_SetString_t)(cvar_t* var, char const* string);
#if COD_VERSION == COD2_1_0
static const Cvar_SetString_t Cvar_SetString = (Cvar_SetString_t)0x080B2560;
#elif COD_VERSION == COD2_1_2
static const Cvar_SetString_t Cvar_SetString = (Cvar_SetString_t)0x080B493C;
#elif COD_VERSION == COD2_1_3
static const Cvar_SetString_t Cvar_SetString = (Cvar_SetString_t)0x080B4A80;
#endif

typedef cvar_t* (*Cvar_FindVar_t)(const char *var_name);
#if COD_VERSION == COD2_1_0
static const Cvar_FindVar_t Cvar_FindVar = (Cvar_FindVar_t)0x080B0AE4;
#elif COD_VERSION == COD2_1_2
static const Cvar_FindVar_t Cvar_FindVar = (Cvar_FindVar_t)0x080B2D94;
#elif COD_VERSION == COD2_1_3
static const Cvar_FindVar_t Cvar_FindVar = (Cvar_FindVar_t)0x080B2ED8;
#endif

typedef cvar_t* (*Cvar_RegisterBool_t)(const char *var_name, qboolean var_value, unsigned short flags);
#if COD_VERSION == COD2_1_0
static const Cvar_RegisterBool_t Cvar_RegisterBool = (Cvar_RegisterBool_t)0x080B1B72;
#elif COD_VERSION == COD2_1_2
static const Cvar_RegisterBool_t Cvar_RegisterBool = (Cvar_RegisterBool_t)0x080B3E8E;
#elif COD_VERSION == COD2_1_3
static const Cvar_RegisterBool_t Cvar_RegisterBool = (Cvar_RegisterBool_t)0x080B3FD2;
#endif

typedef cvar_t* (*Cvar_RegisterFloat_t)(const char* var_name, float var_value, float var_min, float var_max, unsigned short flags);
#if COD_VERSION == COD2_1_0
static const Cvar_RegisterFloat_t Cvar_RegisterFloat = (Cvar_RegisterFloat_t)0x080B1C2C;
#elif COD_VERSION == COD2_1_2
static const Cvar_RegisterFloat_t Cvar_RegisterFloat = (Cvar_RegisterFloat_t)0x080B3F48;
#elif COD_VERSION == COD2_1_3
static const Cvar_RegisterFloat_t Cvar_RegisterFloat = (Cvar_RegisterFloat_t)0x080B408C;
#endif

typedef cvar_t* (*Cvar_RegisterString_t)(const char *var_name, const char *var_value, unsigned short flags);
#if COD_VERSION == COD2_1_0
static const Cvar_RegisterString_t Cvar_RegisterString = (Cvar_RegisterString_t)0x080B1DD2;
#elif COD_VERSION == COD2_1_2
static const Cvar_RegisterString_t Cvar_RegisterString = (Cvar_RegisterString_t)0x080B40EE;
#elif COD_VERSION == COD2_1_3
static const Cvar_RegisterString_t Cvar_RegisterString = (Cvar_RegisterString_t)0x080B4232;
#endif

typedef void (*SV_ConnectionlessPacket_t)(netadr_t from, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const SV_ConnectionlessPacket_t SV_ConnectionlessPacket = (SV_ConnectionlessPacket_t)0x08093F1E;
#elif COD_VERSION == COD2_1_2
static const SV_ConnectionlessPacket_t SV_ConnectionlessPacket = (SV_ConnectionlessPacket_t)0x08095894;
#elif COD_VERSION == COD2_1_3
static const SV_ConnectionlessPacket_t SV_ConnectionlessPacket = (SV_ConnectionlessPacket_t)0x0809594E;
#endif

typedef void (*NET_OutOfBandPrint_t)(netsrc_t net_socket, netadr_t adr, const char *format, ...);
#if COD_VERSION == COD2_1_0
static const NET_OutOfBandPrint_t NET_OutOfBandPrint = (NET_OutOfBandPrint_t)0x0806C40C;
#elif COD_VERSION == COD2_1_2
static const NET_OutOfBandPrint_t NET_OutOfBandPrint = (NET_OutOfBandPrint_t)0x0806C8D4;
#elif COD_VERSION == COD2_1_3
static const NET_OutOfBandPrint_t NET_OutOfBandPrint = (NET_OutOfBandPrint_t)0x0806C8CC;
#endif

typedef void (*SV_GameSendServerCommand_t)(int clientnum, int svscmd_type, const char *text);
#if COD_VERSION == COD2_1_0
static const SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x0808FE96;
#elif COD_VERSION == COD2_1_2
static const SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x080916A6;
#elif COD_VERSION == COD2_1_3
static const SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x080917AA;
#endif

typedef void (*SV_DropClient_t)(client_t *drop, const char *reason);
#if COD_VERSION == COD2_1_0
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x0808DC8C;
#elif COD_VERSION == COD2_1_2
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x0808EF9A;
#elif COD_VERSION == COD2_1_3
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x0808F02E;
#endif

typedef void (*ClientUserinfoChanged_t)(int clientNum);
#if COD_VERSION == COD2_1_0
static const ClientUserinfoChanged_t ClientUserinfoChanged = (ClientUserinfoChanged_t)0x080F6506;
#elif COD_VERSION == COD2_1_2
static const ClientUserinfoChanged_t ClientUserinfoChanged = (ClientUserinfoChanged_t)0x080F8B1A;
#elif COD_VERSION == COD2_1_3
static const ClientUserinfoChanged_t ClientUserinfoChanged = (ClientUserinfoChanged_t)0x080F8C5E;
#endif

typedef int (*SV_ClientHasClientMuted_t)(int clientnum, int mutedclientnum);
#if COD_VERSION == COD2_1_0
static const SV_ClientHasClientMuted_t SV_ClientHasClientMuted = (SV_ClientHasClientMuted_t)0x0809A01E;
#elif COD_VERSION == COD2_1_2
static const SV_ClientHasClientMuted_t SV_ClientHasClientMuted = (SV_ClientHasClientMuted_t)0x0809C0B6;
#elif COD_VERSION == COD2_1_3
static const SV_ClientHasClientMuted_t SV_ClientHasClientMuted = (SV_ClientHasClientMuted_t)0x0809C1FA;
#endif

typedef int (*SV_MapExists_t)(const char *mapname);
#if COD_VERSION == COD2_1_0
static const SV_MapExists_t SV_MapExists = (SV_MapExists_t)0x080909EE;
#elif COD_VERSION == COD2_1_2
static const SV_MapExists_t SV_MapExists = (SV_MapExists_t)0x080921FE;
#elif COD_VERSION == COD2_1_3
static const SV_MapExists_t SV_MapExists = (SV_MapExists_t)0x08092302;
#endif

typedef void (*Info_SetValueForKey_t)(char *s, const char *key, const char *value);
#if COD_VERSION == COD2_1_0
static const Info_SetValueForKey_t Info_SetValueForKey = (Info_SetValueForKey_t)0x080B5FF6;
#elif COD_VERSION == COD2_1_2
static const Info_SetValueForKey_t Info_SetValueForKey = (Info_SetValueForKey_t)0x080B848A;
#elif COD_VERSION == COD2_1_3
static const Info_SetValueForKey_t Info_SetValueForKey = (Info_SetValueForKey_t)0x080B85CE;
#endif

typedef char* (*Info_ValueForKey_t)(const char *s, const char *key);
#if COD_VERSION == COD2_1_0
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x080B5B30;
#elif COD_VERSION == COD2_1_2
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x080B7FC4;
#elif COD_VERSION == COD2_1_3
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x080B8108;
#endif

typedef short (*Scr_ExecThread_t)(int callbackHook, unsigned int numArgs);
#if COD_VERSION == COD2_1_0
static const Scr_ExecThread_t Scr_ExecThread = (Scr_ExecThread_t)0x0808398E;
#elif COD_VERSION == COD2_1_2
static const Scr_ExecThread_t Scr_ExecThread = (Scr_ExecThread_t)0x08083F0A;
#elif COD_VERSION == COD2_1_3
static const Scr_ExecThread_t Scr_ExecThread = (Scr_ExecThread_t)0x08083FD6;
#endif

typedef short (*Scr_ExecEntThread_t)(gentity_t* ent, int callbackHook, unsigned int numArgs);
#if COD_VERSION == COD2_1_0
static const Scr_ExecEntThread_t Scr_ExecEntThread = (Scr_ExecEntThread_t)0x08118DF4;
#elif COD_VERSION == COD2_1_2
static const Scr_ExecEntThread_t Scr_ExecEntThread = (Scr_ExecEntThread_t)0x0811B128;
#elif COD_VERSION == COD2_1_3
static const Scr_ExecEntThread_t Scr_ExecEntThread = (Scr_ExecEntThread_t)0x0811B284;
#endif

typedef short (*Scr_FreeThread_t)(short thread_id);
#if COD_VERSION == COD2_1_0
static const Scr_FreeThread_t Scr_FreeThread = (Scr_FreeThread_t)0x08083B8E;
#elif COD_VERSION == COD2_1_2
static const Scr_FreeThread_t Scr_FreeThread = (Scr_FreeThread_t)0x0808410A;
#elif COD_VERSION == COD2_1_3
static const Scr_FreeThread_t Scr_FreeThread = (Scr_FreeThread_t)0x080841D6;
#endif

typedef void (*SVC_RemoteCommand_t)(netadr_t from, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const SVC_RemoteCommand_t SVC_RemoteCommand = (SVC_RemoteCommand_t)0x080951B4;
#elif COD_VERSION == COD2_1_2
static const SVC_RemoteCommand_t SVC_RemoteCommand = (SVC_RemoteCommand_t)0x080970CC;
#elif COD_VERSION == COD2_1_3
static const SVC_RemoteCommand_t SVC_RemoteCommand = (SVC_RemoteCommand_t)0x08097188;
#endif

typedef void (*SV_GetChallenge_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SV_GetChallenge_t SV_GetChallenge = (SV_GetChallenge_t)0x0808BE54;
#elif COD_VERSION == COD2_1_2
static const SV_GetChallenge_t SV_GetChallenge = (SV_GetChallenge_t)0x0808D0C2;
#elif COD_VERSION == COD2_1_3
static const SV_GetChallenge_t SV_GetChallenge = (SV_GetChallenge_t)0x0808D18E;
#endif

typedef void (*SVC_Info_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SVC_Info_t SVC_Info = (SVC_Info_t)0x08093980;
#elif COD_VERSION == COD2_1_2
static const SVC_Info_t SVC_Info = (SVC_Info_t)0x080952C4;
#elif COD_VERSION == COD2_1_3
static const SVC_Info_t SVC_Info = (SVC_Info_t)0x0809537C;
#endif

typedef void (*SVC_Status_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SVC_Status_t SVC_Status = (SVC_Status_t)0x08093288;
#elif COD_VERSION == COD2_1_2
static const SVC_Status_t SVC_Status = (SVC_Status_t)0x08094BCC;
#elif COD_VERSION == COD2_1_3
static const SVC_Status_t SVC_Status = (SVC_Status_t)0x08094C84;
#endif

typedef const char* (*NET_AdrToString_t)(netadr_t a);
#if COD_VERSION == COD2_1_0
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806AD14;
#elif COD_VERSION == COD2_1_2
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806B1DC;
#elif COD_VERSION == COD2_1_3
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806B1D4;
#endif

typedef qboolean (*NET_StringToAdr_t)(char *s, netadr_t *a);
#if COD_VERSION == COD2_1_0
static const NET_StringToAdr_t NET_StringToAdr = (NET_StringToAdr_t)0x0806C750;
#elif COD_VERSION == COD2_1_2
static const NET_StringToAdr_t NET_StringToAdr = (NET_StringToAdr_t)0x0806CCCC;
#elif COD_VERSION == COD2_1_3
static const NET_StringToAdr_t NET_StringToAdr = (NET_StringToAdr_t)0x0806CD98;
#endif

typedef void (*Scr_Error_t)(const char *string);
#if COD_VERSION == COD2_1_0
static const Scr_Error_t Scr_Error = (Scr_Error_t)0x08084DB4;
#elif COD_VERSION == COD2_1_2
static const Scr_Error_t Scr_Error = (Scr_Error_t)0x08085330;
#elif COD_VERSION == COD2_1_3
static const Scr_Error_t Scr_Error = (Scr_Error_t)0x080853FC;
#endif

typedef int (*Sys_MilliSeconds_t)(void);
#if COD_VERSION == COD2_1_0
static const Sys_MilliSeconds_t Sys_MilliSeconds = (Sys_MilliSeconds_t)0x080D3728;
#elif COD_VERSION == COD2_1_2
static const Sys_MilliSeconds_t Sys_MilliSeconds = (Sys_MilliSeconds_t)0x080D5C54;
#elif COD_VERSION == COD2_1_3
static const Sys_MilliSeconds_t Sys_MilliSeconds = (Sys_MilliSeconds_t)0x080D5D98;
#endif

typedef int (*G_FindConfigstringIndex_t)(const char *name, int start, int max, qboolean create, const char *fieldname);
#if COD_VERSION == COD2_1_0
static const G_FindConfigstringIndex_t G_FindConfigstringIndex = (G_FindConfigstringIndex_t)0x0811AE70;
#elif COD_VERSION == COD2_1_2
static const G_FindConfigstringIndex_t G_FindConfigstringIndex = (G_FindConfigstringIndex_t)0x0811D1A4;
#elif COD_VERSION == COD2_1_3
static const G_FindConfigstringIndex_t G_FindConfigstringIndex = (G_FindConfigstringIndex_t)0x0811D300;
#endif

typedef const char * (*SV_GetConfigstringConst_t)(int index);
#if COD_VERSION == COD2_1_0
static const SV_GetConfigstringConst_t SV_GetConfigstringConst = (SV_GetConfigstringConst_t)0x08091108;
#elif COD_VERSION == COD2_1_2
static const SV_GetConfigstringConst_t SV_GetConfigstringConst = (SV_GetConfigstringConst_t)0x08092918;
#elif COD_VERSION == COD2_1_3
static const SV_GetConfigstringConst_t SV_GetConfigstringConst = (SV_GetConfigstringConst_t)0x08092A1C;
#endif

typedef void (*SV_SetConfigstring_t)(int index, const char *val);
#if COD_VERSION == COD2_1_0
static const SV_SetConfigstring_t SV_SetConfigstring = (SV_SetConfigstring_t)0x08090E6C;
#elif COD_VERSION == COD2_1_2
static const SV_SetConfigstring_t SV_SetConfigstring = (SV_SetConfigstring_t)0x0809267C;
#elif COD_VERSION == COD2_1_3
static const SV_SetConfigstring_t SV_SetConfigstring = (SV_SetConfigstring_t)0x08092780;
#endif

typedef void * (*Z_MallocInternal_t)(int size);
#if COD_VERSION == COD2_1_0
static const Z_MallocInternal_t Z_MallocInternal = (Z_MallocInternal_t)0x080A92FA;
#elif COD_VERSION == COD2_1_2
static const Z_MallocInternal_t Z_MallocInternal = (Z_MallocInternal_t)0x080AB51A;
#elif COD_VERSION == COD2_1_3
static const Z_MallocInternal_t Z_MallocInternal = (Z_MallocInternal_t)0x080AB65E;
#endif

typedef int (*FS_Read_t)(void *buffer, int len, fileHandle_t f);
#if COD_VERSION == COD2_1_0
static const FS_Read_t FS_Read = (FS_Read_t)0x0809E328;
#elif COD_VERSION == COD2_1_2
static const FS_Read_t FS_Read = (FS_Read_t)0x080A03E6;
#elif COD_VERSION == COD2_1_3
static const FS_Read_t FS_Read = (FS_Read_t)0x080A052A;
#endif

typedef void (*MSG_Init_t)(msg_t *buf, byte *data, int length);
#if COD_VERSION == COD2_1_0
static const MSG_Init_t MSG_Init = (MSG_Init_t)0x08067718;
#elif COD_VERSION == COD2_1_2
static const MSG_Init_t MSG_Init = (MSG_Init_t)0x08067BE0;
#elif COD_VERSION == COD2_1_3
static const MSG_Init_t MSG_Init = (MSG_Init_t)0x08067BD8;
#endif

typedef void (*MSG_WriteByte_t)(msg_t *msg, int c);
#if COD_VERSION == COD2_1_0
static const MSG_WriteByte_t MSG_WriteByte = (MSG_WriteByte_t)0x08067B4C;
#elif COD_VERSION == COD2_1_2
static const MSG_WriteByte_t MSG_WriteByte = (MSG_WriteByte_t)0x08068014;
#elif COD_VERSION == COD2_1_3
static const MSG_WriteByte_t MSG_WriteByte = (MSG_WriteByte_t)0x0806800C;
#endif

typedef void (*MSG_WriteShort_t)(msg_t *msg, int c);
#if COD_VERSION == COD2_1_0
static const MSG_WriteShort_t MSG_WriteShort = (MSG_WriteShort_t)0x08067BDA;
#elif COD_VERSION == COD2_1_2
static const MSG_WriteShort_t MSG_WriteShort = (MSG_WriteShort_t)0x080680A2;
#elif COD_VERSION == COD2_1_3
static const MSG_WriteShort_t MSG_WriteShort = (MSG_WriteShort_t)0x0806809A;
#endif

typedef void (*MSG_WriteLong_t)(msg_t *msg, int c);
#if COD_VERSION == COD2_1_0
static const MSG_WriteLong_t MSG_WriteLong = (MSG_WriteLong_t)0x08067C2A;
#elif COD_VERSION == COD2_1_2
static const MSG_WriteLong_t MSG_WriteLong = (MSG_WriteLong_t)0x080680F2;
#elif COD_VERSION == COD2_1_3
static const MSG_WriteLong_t MSG_WriteLong = (MSG_WriteLong_t)0x080680EA;
#endif

typedef void (*MSG_WriteString_t)(msg_t *msg, const char *s);
#if COD_VERSION == COD2_1_0
static const MSG_WriteString_t MSG_WriteString = (MSG_WriteString_t)0x08067CE4;
#elif COD_VERSION == COD2_1_2
static const MSG_WriteString_t MSG_WriteString = (MSG_WriteString_t)0x080681AC;
#elif COD_VERSION == COD2_1_3
static const MSG_WriteString_t MSG_WriteString = (MSG_WriteString_t)0x080681A4;
#endif

typedef void (*MSG_WriteData_t)(msg_t *buf, const void *data, int length);
#if COD_VERSION == COD2_1_0
static const MSG_WriteData_t MSG_WriteData = (MSG_WriteData_t)0x08067B84;
#elif COD_VERSION == COD2_1_2
static const MSG_WriteData_t MSG_WriteData = (MSG_WriteData_t)0x0806804C;
#elif COD_VERSION == COD2_1_3
static const MSG_WriteData_t MSG_WriteData = (MSG_WriteData_t)0x08068044;
#endif

typedef long (*FS_SV_FOpenFileRead_t)(const char *filename, fileHandle_t *fp);
#if COD_VERSION == COD2_1_0
static const FS_SV_FOpenFileRead_t FS_SV_FOpenFileRead = (FS_SV_FOpenFileRead_t)0x08064100;
#elif COD_VERSION == COD2_1_2
static const FS_SV_FOpenFileRead_t FS_SV_FOpenFileRead = (FS_SV_FOpenFileRead_t)0x08064560;
#elif COD_VERSION == COD2_1_3
static const FS_SV_FOpenFileRead_t FS_SV_FOpenFileRead = (FS_SV_FOpenFileRead_t)0x08064558;
#endif

typedef int (*FS_iwIwd_t)(const char *a1, const char *a2);
#if COD_VERSION == COD2_1_0
static const FS_iwIwd_t FS_iwIwd = (FS_iwIwd_t)0x08064ECC;
#elif COD_VERSION == COD2_1_2
static const FS_iwIwd_t FS_iwIwd = (FS_iwIwd_t)0x0806532C;
#elif COD_VERSION == COD2_1_3
static const FS_iwIwd_t FS_iwIwd = (FS_iwIwd_t)0x08065324;
#endif

typedef signed int (*SV_WWWRedirectClient_t)(client_t *cl, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const SV_WWWRedirectClient_t SV_WWWRedirectClient = (SV_WWWRedirectClient_t)0x0; // Does not exist in 1.0
#elif COD_VERSION == COD2_1_2
static const SV_WWWRedirectClient_t SV_WWWRedirectClient = (SV_WWWRedirectClient_t)0x0808FB78;
#elif COD_VERSION == COD2_1_3
static const SV_WWWRedirectClient_t SV_WWWRedirectClient = (SV_WWWRedirectClient_t)0x0808FC0C;
#endif

typedef WeaponDef_t * (*BG_WeaponDefs_t)(unsigned int weaponIndex);
#if COD_VERSION == COD2_1_0
static const BG_WeaponDefs_t BG_WeaponDefs = (BG_WeaponDefs_t)0x080E9270;
#elif COD_VERSION == COD2_1_2
static const BG_WeaponDefs_t BG_WeaponDefs = (BG_WeaponDefs_t)0x080EB860;
#elif COD_VERSION == COD2_1_3
static const BG_WeaponDefs_t BG_WeaponDefs = (BG_WeaponDefs_t)0x080EB9A4;
#endif

typedef int (*BG_FindWeaponIndexForName_t)(const char *name);
#if COD_VERSION == COD2_1_0
static const BG_FindWeaponIndexForName_t BG_FindWeaponIndexForName = (BG_FindWeaponIndexForName_t)0x080E949C;
#elif COD_VERSION == COD2_1_2
static const BG_FindWeaponIndexForName_t BG_FindWeaponIndexForName = (BG_FindWeaponIndexForName_t)0x080EBA8C;
#elif COD_VERSION == COD2_1_3
static const BG_FindWeaponIndexForName_t BG_FindWeaponIndexForName = (BG_FindWeaponIndexForName_t)0x080EBBD0;
#endif

typedef int (*BG_GetNumWeapons_t)(void);
#if COD_VERSION == COD2_1_0
static const BG_GetNumWeapons_t BG_GetNumWeapons = (BG_GetNumWeapons_t)0x080E9322;
#elif COD_VERSION == COD2_1_2
static const BG_GetNumWeapons_t BG_GetNumWeapons = (BG_GetNumWeapons_t)0x080EB912;
#elif COD_VERSION == COD2_1_3
static const BG_GetNumWeapons_t BG_GetNumWeapons = (BG_GetNumWeapons_t)0x080EBA56;
#endif

typedef signed int (*sub_80E9758_t)(int a1);
#if COD_VERSION == COD2_1_0
static const sub_80E9758_t sub_80E9758 = (sub_80E9758_t)0x080E9758;
#elif COD_VERSION == COD2_1_2
static const sub_80E9758_t sub_80E9758 = (sub_80E9758_t)0x080EBD48;
#elif COD_VERSION == COD2_1_3
static const sub_80E9758_t sub_80E9758 = (sub_80E9758_t)0x080EBE8C;
#endif

typedef int (*sub_80D9E84_t)(int a1, signed int a2);
#if COD_VERSION == COD2_1_0
static const sub_80D9E84_t sub_80D9E84 = (sub_80D9E84_t)0x080D9E84;
#elif COD_VERSION == COD2_1_2
static const sub_80D9E84_t sub_80D9E84 = (sub_80D9E84_t)0x080DC464;
#elif COD_VERSION == COD2_1_3
static const sub_80D9E84_t sub_80D9E84 = (sub_80D9E84_t)0x080DC5A8;
#endif

typedef void (*scriptError_t)(int a1, int a2, int a3, void *a4);
#if COD_VERSION == COD2_1_0
static const scriptError_t scriptError = (scriptError_t)0x08078282;
#elif COD_VERSION == COD2_1_2
static const scriptError_t scriptError = (scriptError_t)0x08078806;
#elif COD_VERSION == COD2_1_3
static const scriptError_t scriptError = (scriptError_t)0x080788D2;
#endif

typedef void (*runtimeError_t)(int a1, int a2, int a3, int a4);
#if COD_VERSION == COD2_1_0
static const runtimeError_t runtimeError = (runtimeError_t)0x0807818C;
#elif COD_VERSION == COD2_1_2
static const runtimeError_t runtimeError = (runtimeError_t)0x08078710;
#elif COD_VERSION == COD2_1_3
static const runtimeError_t runtimeError = (runtimeError_t)0x080787DC;
#endif

typedef int (*BG_AnimationIndexForString_t)(char *src);
#if COD_VERSION == COD2_1_0
static const BG_AnimationIndexForString_t BG_AnimationIndexForString = (BG_AnimationIndexForString_t)0x080D46AC;
#elif COD_VERSION == COD2_1_2
static const BG_AnimationIndexForString_t BG_AnimationIndexForString = (BG_AnimationIndexForString_t)0x080D6C8C;
#elif COD_VERSION == COD2_1_3
static const BG_AnimationIndexForString_t BG_AnimationIndexForString = (BG_AnimationIndexForString_t)0x080D6DD0;
#endif

typedef int (*Sys_IsLANAddress_t)(netadr_t adr);
#if COD_VERSION == COD2_1_0
static const Sys_IsLANAddress_t Sys_IsLANAddress = (Sys_IsLANAddress_t)0x080D2FC8;
#elif COD_VERSION == COD2_1_2
static const Sys_IsLANAddress_t Sys_IsLANAddress = (Sys_IsLANAddress_t)0x080D54F4;
#elif COD_VERSION == COD2_1_3
static const Sys_IsLANAddress_t Sys_IsLANAddress = (Sys_IsLANAddress_t)0x080D5638;
#endif

typedef int (*Scr_IsSystemActive_t)();
#if COD_VERSION == COD2_1_0
static const Scr_IsSystemActive_t Scr_IsSystemActive = (Scr_IsSystemActive_t)0x08084030;
#elif COD_VERSION == COD2_1_2
static const Scr_IsSystemActive_t Scr_IsSystemActive = (Scr_IsSystemActive_t)0x080845AC;
#elif COD_VERSION == COD2_1_3
static const Scr_IsSystemActive_t Scr_IsSystemActive = (Scr_IsSystemActive_t)0x08084678;
#endif

typedef void* (*Sys_GetValue_t)(int key);
#if COD_VERSION == COD2_1_0
static const Sys_GetValue_t Sys_GetValue = (Sys_GetValue_t)0x080D44A4;
#elif COD_VERSION == COD2_1_2
static const Sys_GetValue_t Sys_GetValue = (Sys_GetValue_t)0x080D6A7C;
#elif COD_VERSION == COD2_1_3
static const Sys_GetValue_t Sys_GetValue = (Sys_GetValue_t)0x080D6BC0;
#endif

typedef void (*Sys_SetValue_t)(int key, void* value);
#if COD_VERSION == COD2_1_0
static const Sys_SetValue_t Sys_SetValue = (Sys_SetValue_t)0x080D4492;
#elif COD_VERSION == COD2_1_2
static const Sys_SetValue_t Sys_SetValue = (Sys_SetValue_t)0x080D6A6A;
#elif COD_VERSION == COD2_1_3
static const Sys_SetValue_t Sys_SetValue = (Sys_SetValue_t)0x080D6BAE;
#endif

typedef void (*Scr_AddBool_t)(qboolean);
#if COD_VERSION == COD2_1_0
static const Scr_AddBool_t Scr_AddBool = (Scr_AddBool_t)0x08084AF8;
#elif COD_VERSION == COD2_1_2
static const Scr_AddBool_t Scr_AddBool = (Scr_AddBool_t)0x08085074;
#elif COD_VERSION == COD2_1_3
static const Scr_AddBool_t Scr_AddBool = (Scr_AddBool_t)0x08085140;
#endif

typedef void (*Scr_AddInt_t)(int value);
#if COD_VERSION == COD2_1_0
static const Scr_AddInt_t Scr_AddInt = (Scr_AddInt_t)0x08084B1C;
#elif COD_VERSION == COD2_1_2
static const Scr_AddInt_t Scr_AddInt = (Scr_AddInt_t)0x08085098;
#elif COD_VERSION == COD2_1_3
static const Scr_AddInt_t Scr_AddInt = (Scr_AddInt_t)0x08085164;
#endif

typedef void (*Scr_AddFloat_t)(float);
#if COD_VERSION == COD2_1_0
static const Scr_AddFloat_t Scr_AddFloat = (Scr_AddFloat_t)0x08084B40;
#elif COD_VERSION == COD2_1_2
static const Scr_AddFloat_t Scr_AddFloat = (Scr_AddFloat_t)0x080850BC;
#elif COD_VERSION == COD2_1_3
static const Scr_AddFloat_t Scr_AddFloat = (Scr_AddFloat_t)0x08085188;
#endif

typedef void (*Scr_AddString_t)(const char *string);
#if COD_VERSION == COD2_1_0
static const Scr_AddString_t Scr_AddString = (Scr_AddString_t)0x08084C1A;
#elif COD_VERSION == COD2_1_2
static const Scr_AddString_t Scr_AddString = (Scr_AddString_t)0x08085196;
#elif COD_VERSION == COD2_1_3
static const Scr_AddString_t Scr_AddString = (Scr_AddString_t)0x08085262;
#endif

typedef void (*Scr_AddUndefined_t)(void);
#if COD_VERSION == COD2_1_0
static const Scr_AddUndefined_t Scr_AddUndefined = (Scr_AddUndefined_t)0x08084B88;
#elif COD_VERSION == COD2_1_2
static const Scr_AddUndefined_t Scr_AddUndefined = (Scr_AddUndefined_t)0x08085104;
#elif COD_VERSION == COD2_1_3
static const Scr_AddUndefined_t Scr_AddUndefined = (Scr_AddUndefined_t)0x080851D0;
#endif

typedef void (*Scr_AddVector_t)(vec3_t vec);
#if COD_VERSION == COD2_1_0
static const Scr_AddVector_t Scr_AddVector = (Scr_AddVector_t)0x08084CBE;
#elif COD_VERSION == COD2_1_2
static const Scr_AddVector_t Scr_AddVector = (Scr_AddVector_t)0x0808523A;
#elif COD_VERSION == COD2_1_3
static const Scr_AddVector_t Scr_AddVector = (Scr_AddVector_t)0x08085306;
#endif

typedef void (*Scr_AddEntity_t)(gentity_t* ent);
#if COD_VERSION == COD2_1_0
static const Scr_AddEntity_t Scr_AddEntity = (Scr_AddEntity_t)0x08118CC0;
#elif COD_VERSION == COD2_1_2
static const Scr_AddEntity_t Scr_AddEntity = (Scr_AddEntity_t)0x0811AFF4;
#elif COD_VERSION == COD2_1_3
static const Scr_AddEntity_t Scr_AddEntity = (Scr_AddEntity_t)0x08117F50;
#endif

typedef void (*Scr_MakeArray_t)(void);
#if COD_VERSION == COD2_1_0
static const Scr_MakeArray_t Scr_MakeArray = (Scr_MakeArray_t)0x08084CF0;
#elif COD_VERSION == COD2_1_2
static const Scr_MakeArray_t Scr_MakeArray = (Scr_MakeArray_t)0x0808526C;
#elif COD_VERSION == COD2_1_3
static const Scr_MakeArray_t Scr_MakeArray = (Scr_MakeArray_t)0x08085338;
#endif

typedef void (*Scr_AddArray_t)(void);
#if COD_VERSION == COD2_1_0
static const Scr_AddArray_t Scr_AddArray = (Scr_AddArray_t)0x08084D1C;
#elif COD_VERSION == COD2_1_2
static const Scr_AddArray_t Scr_AddArray = (Scr_AddArray_t)0x08085298;
#elif COD_VERSION == COD2_1_3
static const Scr_AddArray_t Scr_AddArray = (Scr_AddArray_t)0x08085364;
#endif

typedef void (*Scr_AddObject_t)(unsigned int object);
#if COD_VERSION == COD2_1_0
static const Scr_AddObject_t Scr_AddObject = (Scr_AddObject_t)0x08084BA2;
#elif COD_VERSION == COD2_1_2
static const Scr_AddObject_t Scr_AddObject = (Scr_AddObject_t)0x0808511E;
#elif COD_VERSION == COD2_1_3
static const Scr_AddObject_t Scr_AddObject = (Scr_AddObject_t)0x080851EA;
#endif

typedef gentity_t * (*G_TempEntity_t)(vec3_t origin, int event);
#if COD_VERSION == COD2_1_0
static const G_TempEntity_t G_TempEntity = (G_TempEntity_t)0x0811CB34;
#elif COD_VERSION == COD2_1_2
static const G_TempEntity_t G_TempEntity = (G_TempEntity_t)0x0811EE68;
#elif COD_VERSION == COD2_1_3
static const G_TempEntity_t G_TempEntity = (G_TempEntity_t)0x0811EFC4;
#endif

typedef int (*DirToByte_t)(vec3_t dir);
#if COD_VERSION == COD2_1_0
static const DirToByte_t DirToByte = (DirToByte_t)0x080A1C2A;
#elif COD_VERSION == COD2_1_2
static const DirToByte_t DirToByte = (DirToByte_t)0x080A3E4A;
#elif COD_VERSION == COD2_1_3
static const DirToByte_t DirToByte = (DirToByte_t)0x080A3F8E;
#endif

typedef void (*G_LocationalTrace_t)(trace_t *results, const vec3_t start, const vec3_t end, int passEntityNum, int contentmask);
#if COD_VERSION == COD2_1_0
static const G_LocationalTrace_t G_LocationalTrace = (G_LocationalTrace_t)0x08108134;
#elif COD_VERSION == COD2_1_2
static const G_LocationalTrace_t G_LocationalTrace = (G_LocationalTrace_t)0x0810A470;
#elif COD_VERSION == COD2_1_3
static const G_LocationalTrace_t G_LocationalTrace = (G_LocationalTrace_t)0x0810A5CC;
#endif

typedef void (*G_AddEvent_t)(gentity_t *ent, int event, int eventParm);
#if COD_VERSION == COD2_1_0
static const G_AddEvent_t G_AddEvent = (G_AddEvent_t)0x0811CDA2;
#elif COD_VERSION == COD2_1_2
static const G_AddEvent_t G_AddEvent = (G_AddEvent_t)0x0811F0D6;
#elif COD_VERSION == COD2_1_3
static const G_AddEvent_t G_AddEvent = (G_AddEvent_t)0x0811F232;
#endif

typedef void (*G_AddPredictableEvent_t)(gentity_t *ent, int event, int eventParm);
#if COD_VERSION == COD2_1_0
static const G_AddPredictableEvent_t G_AddPredictableEvent = (G_AddPredictableEvent_t)0x0811CD6C;
#elif COD_VERSION == COD2_1_2
static const G_AddPredictableEvent_t G_AddPredictableEvent = (G_AddPredictableEvent_t)0x0811F0A0;
#elif COD_VERSION == COD2_1_3
static const G_AddPredictableEvent_t G_AddPredictableEvent = (G_AddPredictableEvent_t)0x0811F1FC;
#endif

typedef playerState_t * (*SV_GameClientNum_t)(int num);
#if COD_VERSION == COD2_1_0
static const SV_GameClientNum_t SV_GameClientNum = (SV_GameClientNum_t)0x0808FE02;
#elif COD_VERSION == COD2_1_2
static const SV_GameClientNum_t SV_GameClientNum = (SV_GameClientNum_t)0x08091612;
#elif COD_VERSION == COD2_1_3
static const SV_GameClientNum_t SV_GameClientNum = (SV_GameClientNum_t)0x08091716;
#endif

typedef gentity_t * (*SV_GentityNum_t)(int num);
#if COD_VERSION == COD2_1_0
static const SV_GentityNum_t SV_GentityNum = (SV_GentityNum_t)0x0808FDE4;
#elif COD_VERSION == COD2_1_2
static const SV_GentityNum_t SV_GentityNum = (SV_GentityNum_t)0x080915F4;
#elif COD_VERSION == COD2_1_3
static const SV_GentityNum_t SV_GentityNum = (SV_GentityNum_t)0x080916F8;
#endif

typedef gentity_t * (*SV_ClientThink_t)(client_t *cl, usercmd_t *);
#if COD_VERSION == COD2_1_0
static const SV_ClientThink_t SV_ClientThink = (SV_ClientThink_t)0x0808F488;
#elif COD_VERSION == COD2_1_2
static const SV_ClientThink_t SV_ClientThink = (SV_ClientThink_t)0x08090D18;
#elif COD_VERSION == COD2_1_3
static const SV_ClientThink_t SV_ClientThink = (SV_ClientThink_t)0x08090DAC;
#endif

typedef void (*G_LogPrintf_t)(const char *fmt, ...);
#if COD_VERSION == COD2_1_0
static const G_LogPrintf_t G_LogPrintf = (G_LogPrintf_t)0x08107502;
#elif COD_VERSION == COD2_1_2
static const G_LogPrintf_t G_LogPrintf = (G_LogPrintf_t)0x08109836;
#elif COD_VERSION == COD2_1_3
static const G_LogPrintf_t G_LogPrintf = (G_LogPrintf_t)0x08109992;
#endif

typedef char * (*I_strlwr_t)(char *s1);
#if COD_VERSION == COD2_1_0
static const I_strlwr_t I_strlwr = (I_strlwr_t)0x080B573C;
#elif COD_VERSION == COD2_1_2
static const I_strlwr_t I_strlwr = (I_strlwr_t)0x080B7BD0;
#elif COD_VERSION == COD2_1_3
static const I_strlwr_t I_strlwr = (I_strlwr_t)0x080B7D14;
#endif

typedef char * (*I_strupr_t)(char *s1);
#if COD_VERSION == COD2_1_0
static const I_strupr_t I_strupr = (I_strupr_t)0x080B577E;
#elif COD_VERSION == COD2_1_2
static const I_strupr_t I_strupr = (I_strupr_t)0x080B7C12;
#elif COD_VERSION == COD2_1_3
static const I_strupr_t I_strupr = (I_strupr_t)0x080B7D56;
#endif

typedef gentity_t * (*G_Spawn_t)(void);
#if COD_VERSION == COD2_1_0
static const G_Spawn_t G_Spawn = (G_Spawn_t)0x0811C546;
#elif COD_VERSION == COD2_1_2
static const G_Spawn_t G_Spawn = (G_Spawn_t)0x0811E87A;
#elif COD_VERSION == COD2_1_3
static const G_Spawn_t G_Spawn = (G_Spawn_t)0x0811E9D6;
#endif

typedef void (*SV_LinkEntity_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const SV_LinkEntity_t SV_LinkEntity = (SV_LinkEntity_t)0x0809A45E;
#elif COD_VERSION == COD2_1_2
static const SV_LinkEntity_t SV_LinkEntity = (SV_LinkEntity_t)0x0809C4F6;
#elif COD_VERSION == COD2_1_3
static const SV_LinkEntity_t SV_LinkEntity = (SV_LinkEntity_t)0x0809C63A;
#endif

typedef void (*SV_UnlinkEntity_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const SV_UnlinkEntity_t SV_UnlinkEntity = (SV_UnlinkEntity_t)0x0809A3BA;
#elif COD_VERSION == COD2_1_2
static const SV_UnlinkEntity_t SV_UnlinkEntity = (SV_UnlinkEntity_t)0x0809C452;
#elif COD_VERSION == COD2_1_3
static const SV_UnlinkEntity_t SV_UnlinkEntity = (SV_UnlinkEntity_t)0x0809C596;
#endif

typedef void (*SV_SetBrushModel_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const SV_SetBrushModel_t SV_SetBrushModel = (SV_SetBrushModel_t)0x0808FF44;
#elif COD_VERSION == COD2_1_2
static const SV_SetBrushModel_t SV_SetBrushModel = (SV_SetBrushModel_t)0x08091754;
#elif COD_VERSION == COD2_1_3
static const SV_SetBrushModel_t SV_SetBrushModel = (SV_SetBrushModel_t)0x08091858;
#endif

typedef void (*Scr_SetString_t)(unsigned short *strindexptr, unsigned const stringindex);
#if COD_VERSION == COD2_1_0
static const Scr_SetString_t Scr_SetString = (Scr_SetString_t)0x08079790;
#elif COD_VERSION == COD2_1_2
static const Scr_SetString_t Scr_SetString = (Scr_SetString_t)0x08079D14;
#elif COD_VERSION == COD2_1_3
static const Scr_SetString_t Scr_SetString = (Scr_SetString_t)0x08079DE0;
#endif

typedef gitem_t * (*BG_FindItemForWeapon_t)(unsigned int weaponIndex);
#if COD_VERSION == COD2_1_0
static const BG_FindItemForWeapon_t BG_FindItemForWeapon = (BG_FindItemForWeapon_t)0x080DCAE6;
#elif COD_VERSION == COD2_1_2
static const BG_FindItemForWeapon_t BG_FindItemForWeapon = (BG_FindItemForWeapon_t)0x080DF0C6;
#elif COD_VERSION == COD2_1_3
static const BG_FindItemForWeapon_t BG_FindItemForWeapon = (BG_FindItemForWeapon_t)0x080DF20A;
#endif

typedef XModel_t * (*SV_XModelGet_t)(const char *name);
#if COD_VERSION == COD2_1_0
static const SV_XModelGet_t SV_XModelGet = (SV_XModelGet_t)0x08090534;
#elif COD_VERSION == COD2_1_2
static const SV_XModelGet_t SV_XModelGet = (SV_XModelGet_t)0x08091D44;
#elif COD_VERSION == COD2_1_3
static const SV_XModelGet_t SV_XModelGet = (SV_XModelGet_t)0x08091E48;
#endif

typedef XModel_t * (*SV_XModelForIndex_t)(unsigned int modelIndex);
#if COD_VERSION == COD2_1_0
static const SV_XModelForIndex_t SV_XModelForIndex = (SV_XModelForIndex_t)0x0811B0F8;
#elif COD_VERSION == COD2_1_2
static const SV_XModelForIndex_t SV_XModelForIndex = (SV_XModelForIndex_t)0x0811D42C;
#elif COD_VERSION == COD2_1_3
static const SV_XModelForIndex_t SV_XModelForIndex = (SV_XModelForIndex_t)0x0811D588;
#endif

typedef qboolean (*SV_DObjExists_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const SV_DObjExists_t SV_DObjExists = (SV_DObjExists_t)0x08090A5A;
#elif COD_VERSION == COD2_1_2
static const SV_DObjExists_t SV_DObjExists = (SV_DObjExists_t)0x0809226A;
#elif COD_VERSION == COD2_1_3
static const SV_DObjExists_t SV_DObjExists = (SV_DObjExists_t)0x0809236E;
#endif

typedef DObj_t * (*Com_GetServerDObj_t)(int entNum);
#if COD_VERSION == COD2_1_0
static const Com_GetServerDObj_t Com_GetServerDObj = (Com_GetServerDObj_t)0x0806289C;
#elif COD_VERSION == COD2_1_2
static const Com_GetServerDObj_t Com_GetServerDObj = (Com_GetServerDObj_t)0x08062BCC;
#elif COD_VERSION == COD2_1_3
static const Com_GetServerDObj_t Com_GetServerDObj = (Com_GetServerDObj_t)0x08062BC4;
#endif

typedef void (*SV_DObjDumpInfo_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const SV_DObjDumpInfo_t SV_DObjDumpInfo = (SV_DObjDumpInfo_t)0x08090584;
#elif COD_VERSION == COD2_1_2
static const SV_DObjDumpInfo_t SV_DObjDumpInfo = (SV_DObjDumpInfo_t)0x08091D94;
#elif COD_VERSION == COD2_1_3
static const SV_DObjDumpInfo_t SV_DObjDumpInfo = (SV_DObjDumpInfo_t)0x08091E98;
#endif

typedef qboolean (*DObjSkelExists_t)(DObj_t *dobj, int skelTimeStamp);
#if COD_VERSION == COD2_1_0
static const DObjSkelExists_t DObjSkelExists = (DObjSkelExists_t)0x080B83A2;
#elif COD_VERSION == COD2_1_2
static const DObjSkelExists_t DObjSkelExists = (DObjSkelExists_t)0x080BA836;
#elif COD_VERSION == COD2_1_3
static const DObjSkelExists_t DObjSkelExists = (DObjSkelExists_t)0x080BA97A;
#endif

typedef void (*BG_EvaluateTrajectory_t)(const trajectory_t *tr, int atTime, vec3_t result);
#if COD_VERSION == COD2_1_0
static const BG_EvaluateTrajectory_t BG_EvaluateTrajectory = (BG_EvaluateTrajectory_t)0x080DCEB0;
#elif COD_VERSION == COD2_1_2
static const BG_EvaluateTrajectory_t BG_EvaluateTrajectory = (BG_EvaluateTrajectory_t)0x080DF490;
#elif COD_VERSION == COD2_1_3
static const BG_EvaluateTrajectory_t BG_EvaluateTrajectory = (BG_EvaluateTrajectory_t)0x080DF5D4;
#endif

typedef void (*BG_AddPredictableEventToPlayerstate_t)(int newEvent, unsigned int eventParm, playerState_s *ps);
#if COD_VERSION == COD2_1_0
static const BG_AddPredictableEventToPlayerstate_t BG_AddPredictableEventToPlayerstate = (BG_AddPredictableEventToPlayerstate_t)0x080DD554;
#elif COD_VERSION == COD2_1_2
static const BG_AddPredictableEventToPlayerstate_t BG_AddPredictableEventToPlayerstate = (BG_AddPredictableEventToPlayerstate_t)0x080DFB34;
#elif COD_VERSION == COD2_1_3
static const BG_AddPredictableEventToPlayerstate_t BG_AddPredictableEventToPlayerstate = (BG_AddPredictableEventToPlayerstate_t)0x080DFC78;
#endif

typedef int (*BG_AnimScriptEvent_t)(struct playerState_s* ps, enum scriptAnimEventTypes_t event, int isContinue, int force);
#if COD_VERSION == COD2_1_0
static const BG_AnimScriptEvent_t BG_AnimScriptEvent = (BG_AnimScriptEvent_t)0x080D6FCA;
#elif COD_VERSION == COD2_1_2
static const BG_AnimScriptEvent_t BG_AnimScriptEvent = (BG_AnimScriptEvent_t)0x080D95AA;
#elif COD_VERSION == COD2_1_3
static const BG_AnimScriptEvent_t BG_AnimScriptEvent = (BG_AnimScriptEvent_t)0x080D96EE;
#endif

typedef unsigned int (*PM_GroundSurfaceType_t)(pml_t *pml);
#if COD_VERSION == COD2_1_0
static const PM_GroundSurfaceType_t PM_GroundSurfaceType = (PM_GroundSurfaceType_t)0x080DFAFC;
#elif COD_VERSION == COD2_1_2
static const PM_GroundSurfaceType_t PM_GroundSurfaceType = (PM_GroundSurfaceType_t)0x080E20DC;
#elif COD_VERSION == COD2_1_3
static const PM_GroundSurfaceType_t PM_GroundSurfaceType = (PM_GroundSurfaceType_t)0x080E2220;
#endif

typedef vec_t (*Vec3Normalize_t)(vec3_t v);
#if COD_VERSION == COD2_1_0
static const Vec3Normalize_t Vec3Normalize = (Vec3Normalize_t)0x080A20C0;
#elif COD_VERSION == COD2_1_2
static const Vec3Normalize_t Vec3Normalize = (Vec3Normalize_t)0x080A42E0;
#elif COD_VERSION == COD2_1_3
static const Vec3Normalize_t Vec3Normalize = (Vec3Normalize_t)0x080A4424;
#endif

typedef StanceState (*PM_GetEffectiveStance_t)(struct playerState_s *ps);
#if COD_VERSION == COD2_1_0
static const PM_GetEffectiveStance_t PM_GetEffectiveStance = (PM_GetEffectiveStance_t)0x080DEE0E;
#elif COD_VERSION == COD2_1_2
static const PM_GetEffectiveStance_t PM_GetEffectiveStance = (PM_GetEffectiveStance_t)0x080E13EE;
#elif COD_VERSION == COD2_1_3
static const PM_GetEffectiveStance_t PM_GetEffectiveStance = (PM_GetEffectiveStance_t)0x080E1532;
#endif

typedef void (*Scr_Notify_t)(gentity_t *ent, unsigned short constString, unsigned int numArgs);
#if COD_VERSION == COD2_1_0
static const Scr_Notify_t Scr_Notify = (Scr_Notify_t)0x08118E4E;
#elif COD_VERSION == COD2_1_2
static const Scr_Notify_t Scr_Notify = (Scr_Notify_t)0x0811B182;
#elif COD_VERSION == COD2_1_3
static const Scr_Notify_t Scr_Notify = (Scr_Notify_t)0x0811B2DE;
#endif

#endif
