#ifndef __I_FUNCTIONS_H__
#define __I_FUNCTIONS_H__

#include "i_common.h"
#include "dvar.h"
#include "entity.h"
#include "netchan.h"
#include "sys_net.h"

/* Low-level functions that are required but has not been decompiled yet */
typedef void (*Com_Error_t)(int level, const char *error, ...);
static const Com_Error_t Com_Error = (Com_Error_t)0x08060E66;

typedef int (*Cmd_Argc_t)();
static const Cmd_Argc_t Cmd_Argc = (Cmd_Argc_t)0x0805FFDC;

typedef char * (*Cmd_Argv_t)(int arg);
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x0806001C;

typedef void (*Cmd_AddCommand_t)(const char *cmd_name, xcommand_t function);
static const Cmd_AddCommand_t Cmd_AddCommand = (Cmd_AddCommand_t)0x080604B2;

typedef cvar_t* (*Cvar_FindVar_t)(const char *var_name);
static const Cvar_FindVar_t Cvar_FindVar = (Cvar_FindVar_t)0x080B0AE4;

typedef int (*FS_iwIwd_t)(const char *a1, const char *a2);
static const FS_iwIwd_t FS_iwIwd = (FS_iwIwd_t)0x08064ECC;

typedef long (*FS_SV_FOpenFileRead_t)(const char *filename, fileHandle_t *fp);
static const FS_SV_FOpenFileRead_t FS_SV_FOpenFileRead = (FS_SV_FOpenFileRead_t)0x08064100;

typedef int (*FS_Read_t)(void *buffer, int len, fileHandle_t f);
static const FS_Read_t FS_Read = (FS_Read_t)0x0809E328;

typedef int (*FS_ReadFile_t)(const char *qpath, void **buffer);
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x0809E892;

typedef qboolean (*FS_FCloseFile_t)(fileHandle_t f);
static const FS_FCloseFile_t FS_FCloseFile = (FS_FCloseFile_t)0x0809CC52;

typedef int (*MSG_ReadDeltaEntity_t)(msg_t *msg, entityState_t *from, entityState_t *to, int number);
static const MSG_ReadDeltaEntity_t MSG_ReadDeltaEntity = (MSG_ReadDeltaEntity_t)0x080699C8;

typedef const char* (*NET_AdrToString_t)(netadr_t a);
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806AD14;

typedef qboolean (*NET_SendPacket_t)(netsrc_t sock, int length, const void *data, netadr_t to);
static const NET_SendPacket_t NET_SendPacket = (NET_SendPacket_t)0x0806C32C;

typedef void (*Cmd_TokenizeString_t)(const char *text_in);
static const Cmd_TokenizeString_t Cmd_TokenizeString = (Cmd_TokenizeString_t)0x0806042C;

typedef void (*Cbuf_ExecuteText_t)(int exec_when, const char *text);
static const Cbuf_ExecuteText_t Cbuf_ExecuteText = (Cbuf_ExecuteText_t)0x0805FB06;

#define SV_Cmd_Argc Cmd_Argc
#define SV_Cmd_Argv Cmd_Argv

/* Hooks */
void hook_sv_init(const char *format, ...);

#endif
