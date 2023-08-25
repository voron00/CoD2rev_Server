#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#define LIBCOD

// GSC MODULES
#ifdef LIBCOD
#define COMPILE_BOTS 1
#define COMPILE_ENTITY 1
#define COMPILE_EXEC 1
#define COMPILE_LEVEL 1
#define COMPILE_MEMORY 1
#define COMPILE_MYSQL 0
#define COMPILE_PLAYER 1
#define COMPILE_RATELIMITER 1
#define COMPILE_SQLITE 1
#define COMPILE_UTILS 1
#define COMPILE_WEAPONS 1
#endif

// EXTRA STUFF
#ifdef EXTRA_CONFIG_INC
#include "extra/config.hpp"
#endif

#endif
