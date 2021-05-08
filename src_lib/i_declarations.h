#ifndef __I_DECLARATIONS_H__
#define __I_DECLARATIONS_H__

#include "dvar.h"
#include "sys_thread.h"

/* Low-level declarations that are required but has not been decompiled yet */
#define sv (*((server_t*)( 0x0842BC80 )))
#define svs (*((serverStatic_t*)( 0x0841FB00 )))
#define cm (*((clipMap_t*)( 0x08185BE0 )))

/* Hooks */
extern cvar_t *sv_maxclients;
extern cvar_t *sv_allowDownload;
extern cvar_t *sv_pure;

#endif
