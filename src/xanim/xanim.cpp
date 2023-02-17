#include "../qcommon/qcommon.h"

extern dvar_t *com_developer;
bool g_anim_developer;
unsigned int g_end;

#ifdef TESTING_LIBRARY
#define g_xAnimInfo (((XAnimInfo*)( 0x08527A80 )))
#else
XAnimInfo g_xAnimInfo[1024];
#endif

bool XanimIsDefaultPart(XAnimParts *animParts)
{
	return animParts->isDefault;
}

const char* XAnimGetAnimDebugName(const XAnim_s *anims, unsigned int animIndex)
{
	bool missing;
	const char *format;
	const XAnimEntry *entry;

	entry = &anims->entries[animIndex];

	if ( anims->debugAnimNames )
	{
		format = anims->debugAnimNames[animIndex];
		missing = 0;

		if ( !entry->numAnims && XanimIsDefaultPart(anims->entries[animIndex].u.parts) )
			missing = 1;

		if ( missing )
			return va("^3%s (missing)", format);
		else
			return format;
	}
	else if ( entry->numAnims )
	{
		return va("%i", animIndex);
	}
	else
	{
		return anims->entries[animIndex].u.parts->name;
	}
}

bool XanimIsComplete(XAnim_s *anim, int index)
{
	if ( !anim->entries[index].numAnims || (anim->entries[index].u.animParent.flags & 3) != 0 )
		return true;

	return false;
}

void XAnimDisplay(const XAnimTree_s *tree, unsigned int infoIndex, int depth)
{
	float deltaTime;
	const char *format;
	const char *AnimDebugName;
	float goalTime;
	XAnimInfo *anim;
	XAnimEntry *animEntry;
	unsigned short treeIndex;
	int numAnims;
	int i;
	int j;

	treeIndex = *(&tree->children + infoIndex);

	if ( treeIndex )
	{
		animEntry = &tree->anims->entries[infoIndex];
		numAnims = animEntry->numAnims;
		anim = &g_xAnimInfo[treeIndex];

		for ( i = 0; i < depth; ++i )
			Com_Printf(" ");

		AnimDebugName = XAnimGetAnimDebugName(tree->anims, infoIndex);

		if ( anim->state.goalWeight <= anim->state.weight )
		{
			if ( anim->state.weight <= anim->state.goalWeight )
				format = "";
			else
				format = "^1";
		}
		else
		{
			format = "^4";
		}

		if ( numAnims )
		{
			if ( anim->notifyName )
			{
				if ( XanimIsComplete(tree->anims, infoIndex) )
				{
					Com_Printf(
					    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f, '%s'\n",
					    format,
					    AnimDebugName,
					    anim->state.weight,
					    anim->state.goalWeight,
					    anim->state.oldTime,
					    anim->state.time,
					    SL_ConvertToString(anim->notifyName));
				}
				else
				{
					Com_Printf(
					    "%s%s: (weight) %.2f -> %.2f, '%s'\n",
					    format,
					    AnimDebugName,
					    anim->state.weight,
					    anim->state.goalWeight,
					    SL_ConvertToString(anim->notifyName));
				}
			}
			else if ( XanimIsComplete(tree->anims, infoIndex) )
			{
				Com_Printf(
				    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f\n",
				    format,
				    AnimDebugName,
				    anim->state.weight,
				    anim->state.goalWeight,
				    anim->state.oldTime,
				    anim->state.time);
			}
			else
			{
				Com_Printf("%s%s: (weight) %.2f -> %.2f\n", format, AnimDebugName, anim->state.weight, anim->state.goalWeight);
			}

			for ( j = 0; j < numAnims; ++j )
				XAnimDisplay(tree, j + animEntry->u.animParent.children, depth + 1);
		}
		else
		{
			goalTime = anim->state.time - anim->state.oldTime;

			if ( goalTime < 0.0 )
				goalTime = goalTime + 1.0;

			if ( animEntry->u.parts->frequency == 0.0 )
				deltaTime = 0.0;
			else
				deltaTime = goalTime / animEntry->u.parts->frequency;

			if ( anim->notifyName )
			{
				Com_Printf(
				    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f, (realtimedelta) %.2f, '%s'\n",
				    format,
				    AnimDebugName,
				    anim->state.weight,
				    anim->state.goalWeight,
				    anim->state.oldTime,
				    anim->state.time,
				    deltaTime,
				    SL_ConvertToString(anim->notifyName));
			}
			else
			{
				Com_Printf(
				    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f, (realtimedelta) %.2f\n",
				    format,
				    AnimDebugName,
				    anim->state.weight,
				    anim->state.goalWeight,
				    anim->state.oldTime,
				    anim->state.time,
				    deltaTime);
			}
		}
	}
}

XAnimParts* XAnimFindData(const char *name)
{
	return (XAnimParts *)Hunk_FindDataForFile(FILEDATA_XANIM, name);
}

const char* XAnimSetData(const char *modelName, XAnimParts *anim, void *(*Alloc)(int))
{
	return Hunk_SetDataForFile(FILEDATA_XANIM, modelName, anim, Alloc);
}

XAnimParts* XAnimLoadDefaultAnim(XAnimParts *animParts, void *(*Alloc)(int))
{
	XAnimParts *anim;
	short count;
	XAnimNotifyInfo *notify;
	int bones;
	int i;
	uint16_t *names;

	anim = (XAnimParts *)Alloc(sizeof(XAnimParts));
	memcpy(anim, animParts, sizeof(XAnimParts));
	names = anim->names;
	bones = anim->boneCount;

	for ( i = 0; i < bones; ++i )
		SL_AddRefToString(names[i]);

	count = 0;
	notify = anim->notify;

	while ( count < anim->notifyCount )
	{
		SL_AddRefToString(notify->name);
		++count;
		++notify;
	}

	return anim;
}

XAnimParts* XAnimPrecache(const char *name, void *(*Alloc)(int))
{
	XAnimParts *anim;

	anim = XAnimFindData(name);

	if ( anim )
		return anim;

	anim = XAnimLoadFile(name, Alloc);

	if ( !anim )
	{
		Com_Printf("^3WARNING: Couldn't find xanim '%s', using default xanim '%s' instead\n", name, "void");
		anim = XAnimFindData("void");

		if ( !anim )
		{
			anim = XAnimLoadFile("void", Alloc);

			if ( !anim )
				Com_Error(ERR_DROP, "\x15" "Cannot find xanim '%s'.", "void");

			XAnimSetData("void", anim, Alloc);
		}

		anim = XAnimLoadDefaultAnim(anim, Alloc);
		anim->isDefault = 1;
	}

	anim->name = XAnimSetData(name, anim, (void *(*)(int))Alloc);
	return anim;
}

void XAnimFree(XAnimParts *parts)
{
	short count;
	XAnimNotifyInfo *notify;
	uint16_t *names;
	int boneCount;
	int i;

	names = parts->names;
	boneCount = parts->boneCount;

	for ( i = 0; i < boneCount; ++i )
		SL_RemoveRefToString(names[i]);

	count = 0;
	notify = parts->notify;

	while ( count < parts->notifyCount )
	{
		SL_RemoveRefToString(notify->name);
		++count;
		++notify;
	}
}

void XAnimFreeList(XAnim_s *anims)
{
	unsigned int i;

	if ( anims->debugName )
	{
		Z_FreeInternal((void *)anims->debugName);
		anims->debugName = 0;
	}

	if ( anims->debugAnimNames )
	{
		for ( i = 0; i < anims->size; ++i )
		{
			if ( anims->debugAnimNames[i] )
			{
				Z_FreeInternal((void *)anims->debugAnimNames[i]);
				anims->debugAnimNames[i] = 0;
			}
		}

		Z_FreeInternal(anims->debugAnimNames);
		anims->debugAnimNames = 0;
	}
}

void XAnimInit()
{
	int i;

	for ( i = 0; i < 1024; ++i )
	{
		g_xAnimInfo[i].prev = (i + 1023) % 1024;
		g_xAnimInfo[i].next = (i + 1) % 1024;
	}

	g_xAnimInfo->state.time = 0.0;
	g_xAnimInfo->state.oldTime = 0.0;
	g_xAnimInfo->state.cycleCount = 0;
	g_xAnimInfo->state.oldCycleCount = 0;

	g_end = SL_GetString_("end", 0);
	g_anim_developer = com_developer->current.integer != 0;
}

void XAnimShutdown()
{
	if ( g_end )
	{
		SL_RemoveRefToString(g_end);
		g_end = 0;
	}
}