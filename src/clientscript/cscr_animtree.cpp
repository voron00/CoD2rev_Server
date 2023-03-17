#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

#ifdef TESTING_LIBRARY
#define scrAnimPub (*((scrAnimPub_t*)( 0x08202440 )))
#else
scrAnimPub_t scrAnimPub;
#endif

typedef struct scrAnimGlob_s
{
	const char *start;
	const char *pos;
	unsigned short using_xanim_lookup[2][128];
	int bAnimCheck;
} scrAnimGlob_t;
static_assert((sizeof(scrAnimGlob_t) == 0x20C), "ERROR: scrAnimGlob_t size is invalid!");

#ifdef TESTING_LIBRARY
#define scrAnimGlob (*((scrAnimGlob_t*)( 0x08202220 )))
#else
scrAnimGlob_t scrAnimGlob;
#endif

const char *propertyNames[] =
{
	"loopsync",
	"nonloopsync",
	"complete"
};

void SetAnimCheck(int bAnimCheck)
{
	scrAnimGlob.bAnimCheck = bAnimCheck;
}

XAnim* Scr_GetAnims(unsigned int index)
{
	return (XAnim_s *)scrAnimPub.xanim_num[index - 128];
}

