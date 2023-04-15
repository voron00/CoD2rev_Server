#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include "../qcommon/qcommon.h"
#include "../game/g_shared.h"

#define LIBCOD

// RATE LIMITER
#define COMPILE_RATELIMITER 1

// GSC MODULES
#define COMPILE_BOTS 1
#define COMPILE_ENTITY 1
#define COMPILE_EXEC 1
#define COMPILE_LEVEL 1
#define COMPILE_MEMORY 1
#define COMPILE_MYSQL 0
#define COMPILE_PLAYER 1
#define COMPILE_SQLITE 1
#define COMPILE_UTILS 1
#define COMPILE_WEAPONS 1

// EXTRA STUFF
#ifdef EXTRA_CONFIG_INC
#include "extra/config.hpp"
#endif

#endif
