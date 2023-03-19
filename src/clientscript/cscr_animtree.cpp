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

unsigned int Scr_UsingTreeInternal(const char *filename, unsigned int *index, int user)
{
	unsigned int var;
	signed int i;
	unsigned int object;
	unsigned int parentId;
	unsigned int id;
	unsigned int variable;
	unsigned int treeIndex;
	unsigned int str;

	str = Scr_CreateCanonicalFilename(filename);
	id = FindVariable(scrAnimPub.animtrees, str);

	if ( id )
	{
		parentId = FindObject(id);
		*index = 0;

		for ( i = 1; i <= scrAnimPub.xanim_num[user]; ++i )
		{
			if ( scrAnimGlob.using_xanim_lookup[user][i] == id )
			{
				*index = i;
				break;
			}
		}
		var = GetVariable(parentId, 0);
	}
	else
	{
		variable = GetNewVariable(scrAnimPub.animtrees, str);
		object = GetObjectA(variable);
		scrAnimGlob.using_xanim_lookup[user][++scrAnimPub.xanim_num[user]] = variable;
		*index = scrAnimPub.xanim_num[user];
		var = GetVariable(object, 0);
	}

	treeIndex = GetArray(var);
	SL_RemoveRefToString(str);

	return treeIndex;
}

void Scr_UsingTree(const char *filename, unsigned int sourcePos)
{
	if ( Scr_IsIdentifier(filename) )
		scrAnimPub.animTreeNames = Scr_UsingTreeInternal(filename, &scrAnimPub.animTreeIndex, 1);
	else
		CompileError(sourcePos, "bad anim tree name");
}