#include "../qcommon/qcommon.h"

extern dvar_t *com_developer;
bool g_anim_developer;
unsigned int g_end;

XAnimClientNotify g_notifyList[128];
int g_notifyListSize = 0;
XAnimInfo g_xAnimInfo[1024];

void *Hunk_AllocXAnimPrecache(int size)
{
	return Hunk_AllocAlignInternal(size, 4);
}

void *Hunk_AllocXAnimServer(int size)
{
	return Hunk_AllocLowInternal(size);
}

void *Hunk_AllocXAnimTreePrecache(int size)
{
	return Hunk_AllocAlignInternal(size, 4);
}

void XAnimCreate(XAnim_s *anims, unsigned int animIndex, const char *name)
{
	size_t len;
	char *dest;
	XAnimParts_s *parts;

	parts = XAnimFindData(name);

	if ( !parts )
		Com_Error(ERR_DROP, "Cannot find xanim '%s'", name);

	anims->entries[animIndex].numAnims = 0;
	anims->entries[animIndex].u.parts = parts;

	if ( anims->debugAnimNames )
	{
		len = strlen(name);
		dest = (char *)Z_MallocInternal(len + 1);
		strcpy(dest, name);
		anims->debugAnimNames[animIndex] = dest;
	}
}

XAnim_s* XAnimCreateAnims(const char *debugName, int size, void *(*Alloc)(int))
{
	size_t len;
	char *dest;
	XAnim_s *newAnim;

	newAnim = (XAnim_s *)Alloc(8 * size + 12);
	newAnim->size = size;

	if ( g_anim_developer )
	{
		len = strlen(debugName);
		dest = (char *)Z_MallocInternal(len + 1);
		strcpy(dest, debugName);
		newAnim->debugName = dest;
		newAnim->debugAnimNames = (const char **)Z_MallocInternal(4 * size);
	}

	if ( Hunk_DataOnHunk(newAnim) )
		Hunk_AddData(FILEDATA_XANIMLIST, newAnim, Alloc);

	return newAnim;
}

void XAnimBlend(XAnim_s *anims, unsigned int animIndex,const char *name, unsigned int children, unsigned int num, unsigned int flags)
{
	size_t len;
	char *dest;
	unsigned int i;

	anims->entries[animIndex].numAnims = num;
	anims->entries[animIndex].u.animParent.flags = flags;
	anims->entries[animIndex].u.animParent.children = children;

	for ( i = 0; i < num; ++i )
		anims->entries[i + anims->entries[animIndex].u.animParent.children].parent = animIndex;

	if ( anims->debugAnimNames )
	{
		len = strlen(name);
		dest = (char *)Z_MallocInternal(len + 1);
		strcpy(dest, name);
		anims->debugAnimNames[animIndex] = dest;
	}
}

XAnim_s* XAnimGetAnims(const XAnimTree_s *tree)
{
	return tree->anims;
}

const char* XAnimGetAnimTreeDebugName(const XAnim_s *anims)
{
	return anims->debugName;
}

bool XAnimNotetrackExists(const XAnim_s *anims, unsigned int animIndex, unsigned int name)
{
	int i;
	XAnimNotifyInfo *info;
	XAnimParts_s *parts;

	parts = anims->entries[animIndex].u.parts;
	info = parts->notify;

	if ( !info )
		return 0;

	for ( i = 0; i < parts->notifyCount; ++i )
	{
		if ( info->name == name )
			return 1;

		++info;
	}

	return 0;
}

bool XanimIsDefaultPart(XAnimParts *animParts)
{
	return animParts->isDefault;
}

unsigned int XAnimGetAnimTreeSize(const XAnim_s *anims)
{
	return anims->size;
}

const char* XAnimGetAnimName(const XAnim_s *anims, unsigned int animIndex)
{
	if ( anims->entries[animIndex].numAnims )
		return "";
	else
		return anims->entries[animIndex].u.parts->name;
}

float XAnimGetTime(const XAnimTree_s *tree, unsigned int animIndex)
{
	uint16_t childIndex;

	childIndex = tree->children[animIndex];

	if ( childIndex )
		return g_xAnimInfo[childIndex].state.time;
	else
		return 0.0;
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

	treeIndex = tree->children[infoIndex];

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

void XAnimCloneInfo(XAnimInfo *from, XAnimInfo *to)
{
	*to = *from;

	if ( to->notifyName )
		SL_AddRefToString(to->notifyName);
}

void XAnimFreeNotifyStrings(XAnimInfo *info)
{
	if ( info->notifyName )
	{
		SL_RemoveRefToString(info->notifyName);
		info->notifyName = 0;
	}

	info->notifyIndex = -1;
}

XAnimInfo* XAnimGetInfo(XAnimTree_s *tree, unsigned int infoIndex)
{
	uint16_t next;

	next = g_xAnimInfo->next;

	if ( !next )
		Com_Error(ERR_DROP, "exceeded maximum number of anim info");

	g_xAnimInfo->next = g_xAnimInfo[g_xAnimInfo->next].next;
	g_xAnimInfo[g_xAnimInfo->next].prev = 0;
	tree->children[infoIndex] = next;

	return &g_xAnimInfo[next];
}

void XAnimFreeInfo(XAnimTree_s *tree, unsigned int infoIndex)
{
	XAnimInfo *info;

	info = &g_xAnimInfo[infoIndex];
	XAnimFreeNotifyStrings(info);
	info->prev = 0;
	info->next = g_xAnimInfo->next;
	g_xAnimInfo[g_xAnimInfo->next].prev = infoIndex;
	g_xAnimInfo->next = infoIndex;
}

void XAnimCloneAnimTree(const XAnimTree_s *from, XAnimTree_s *to)
{
	signed int size;
	XAnimInfo *info;
	int index;
	signed int infoIndex;

	size = from->anims->size;

	for ( infoIndex = 0; infoIndex < size; ++infoIndex )
	{
		index = from->children[infoIndex];

		if ( from->children[infoIndex] )
		{
			if ( to->children[infoIndex] )
			{
				info = &g_xAnimInfo[to->children[infoIndex]];
				XAnimFreeNotifyStrings(info);
			}
			else
			{
				info = XAnimGetInfo(to, infoIndex);
			}

			XAnimCloneInfo(&g_xAnimInfo[index], info);
		}
		else if ( to->children[infoIndex] )
		{
			XAnimFreeInfo(to, to->children[infoIndex]);
			to->children[infoIndex] = 0;
		}
	}
}

void XAnimClearGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float blendTime)
{
	XAnimInfo *info;
	uint16_t childIndex;

	childIndex = tree->children[animIndex];

	if ( childIndex )
	{
		info = &g_xAnimInfo[childIndex];

		if ( info->state.goalWeight == 0.0 )
		{
			if ( info->state.goalTime > blendTime )
				info->state.goalTime = blendTime;
		}
		else
		{
			info->state.goalTime = blendTime;
		}

		info->state.goalWeight = 0.0;

		if ( blendTime == 0.0 )
			info->state.weight = 0.0;

		XAnimFreeNotifyStrings(info);
	}
}

void XAnimSetTime(XAnimTree_s *tree, unsigned int animIndex, float time)
{
	XAnimInfo *info;
	uint16_t childIndex;

	childIndex = tree->children[animIndex];

	if ( childIndex )
	{
		info = &g_xAnimInfo[childIndex];
		info->state.time = time;
		info->state.cycleCount = 0;
		info->state.oldTime = time;
		info->state.oldCycleCount = 0;
		info->notifyIndex = -1;
	}
}

bool XAnimNeedClearState(XAnimInfo *info)
{
	if ( info->state.time == 0.0 && !info->state.cycleCount )
		return false;

	info->state.time = 0.0;
	info->state.cycleCount = 0;
	info->state.oldTime = 0.0;
	info->state.oldCycleCount = 0;
	info->notifyIndex = -1;

	return true;
}

void XAnimResetInfo(XAnimInfo *info)
{
	memset(&info->state, 0, sizeof(info->state));
	info->notifyName = 0;
	info->notifyIndex = -1;
	info->notifyChild = 0;
	info->notifyType = 0;
}

void XAnimResetTime(const XAnimTree_s *tree, unsigned int animIndex)
{
	XAnimEntry *entry;
	uint16_t childIndex;
	int numAnims;
	int i;

	childIndex = tree->children[animIndex];

	if ( childIndex )
	{
		XAnimNeedClearState(&g_xAnimInfo[childIndex]);
		entry = &tree->anims->entries[animIndex];
		numAnims = entry->numAnims;

		for ( i = 0; i < numAnims; ++i )
			XAnimResetTime(tree, i + entry->u.animParent.children);
	}
}

void XAnimRestart(XAnimTree_s *tree, unsigned int infoIndex)
{
	int j;
	int numAnims;
	XAnimEntry *entry;
	unsigned int i;

	for ( i = infoIndex; i; i = tree->anims->entries[i].parent )
	{
		entry = &tree->anims->entries[i];

		if ( entry->numAnims && (tree->anims->entries[i].u.animParent.flags & 4) != 0 )
		{
			if ( XAnimNeedClearState(&g_xAnimInfo[tree->children[i]]) )
			{
				numAnims = entry->numAnims;

				for ( j = 0; j < numAnims; ++j )
					XAnimResetTime(tree, j + entry->u.animParent.children);
			}
			return;
		}
	}

	if ( !tree->anims->entries[infoIndex].numAnims )
		XAnimNeedClearState(&g_xAnimInfo[tree->children[infoIndex]]);
}

unsigned int XAnimGetDescendantWithGreatestWeight(const XAnimTree_s *tree, unsigned int infoIndex)
{
	XAnimEntry *entry;
	int i;
	unsigned int greatest;
	unsigned int result;
	float goalWeight;
	float weight;

	entry = &tree->anims->entries[infoIndex];

	if ( !entry->numAnims )
		return infoIndex;

	weight = 0.0;
	greatest = 0;

	for ( i = 0; i < entry->numAnims; ++i )
	{
		goalWeight = g_xAnimInfo[tree->children[i + entry->u.animParent.children]].state.goalWeight;

		if ( weight < goalWeight )
		{
			result = XAnimGetDescendantWithGreatestWeight(tree, i + entry->u.animParent.children);

			if ( result )
			{
				weight = goalWeight;
				greatest = result;
			}
		}
	}

	return greatest;
}

int XAnimSetGoalWeightInternal(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, bool useGoalWeight, unsigned int notifyName, unsigned int notifyType)
{
	XAnimInfo *info;

	if ( tree->children[animIndex] )
	{
		info = &g_xAnimInfo[tree->children[animIndex]];
		XAnimFreeNotifyStrings(info);
LABEL_7:
		if ( !animIndex )
		{
			goalWeight = 1.0;
			goalTime = 0.0;
			rate = 1.0;
		}

		if ( goalTime == 0.0 )
		{
			info->state.weight = goalWeight;
		}
		else if ( info->state.weight == 0.0 )
		{
			info->state.weight = goalWeight * 0.001;
		}

		if ( goalWeight == 0.0 )
		{
			if ( info->state.goalWeight == 0.0 )
			{
				if ( info->state.goalTime > goalTime )
					info->state.goalTime = goalTime;
			}
			else
			{
				info->state.goalTime = goalTime;
			}
		}
		else if ( goalWeight < info->state.weight )
		{
			info->state.goalTime = (info->state.weight - goalWeight) * goalTime / info->state.weight;
		}
		else
		{
			info->state.goalTime = (goalWeight - info->state.weight) * goalTime / goalWeight;
		}

		info->state.goalWeight = goalWeight;
		info->state.rate = rate;
		info->notifyName = notifyName;

		if ( notifyName )
			SL_AddRefToString(notifyName);

		if ( notifyName
		        && tree->anims->entries[animIndex].numAnims
		        && (tree->anims->entries[animIndex].u.animParent.flags & 3) != 0 )
		{
			info->notifyChild = XAnimGetDescendantWithGreatestWeight(tree, animIndex);

			if ( !info->notifyChild )
				return 2;
		}
		else
		{
			info->notifyChild = 0;
		}

		info->notifyType = notifyType;
		return 0;
	}

	if ( goalWeight != 0.0 || useGoalWeight )
	{
		info = XAnimGetInfo(tree, animIndex);
		XAnimResetInfo(info);
		goto LABEL_7;
	}

	return 0;
}

void XAnimSetCompleteGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart)
{
	unsigned int childIndex;

	if ( goalWeight < 0.001 )
		goalWeight = 0.0;

	XAnimSetGoalWeightInternal(tree, animIndex, goalWeight, goalTime, rate, 0, notifyName, notifyType);
	childIndex = animIndex;

	while ( childIndex )
	{
		childIndex = tree->anims->entries[childIndex].parent;

		if ( !tree->children[childIndex] || g_xAnimInfo[tree->children[childIndex]].state.goalWeight == 0.0 )
			XAnimSetGoalWeightInternal(tree, childIndex, 1.0, goalTime, 1.0, 0, 0, 0);
	}

	if ( bRestart )
		XAnimRestart(tree, animIndex);
}

void XAnimClearGoalWeightKnobInternal(XAnimTree_s *tree, unsigned int infoIndex, float goalWeight, float goalTime)
{
	float weightDiff;
	float absWeight;
	float weight;
	uint16_t childIndex;
	float newGoalWeight;
	float blendTime;
	XAnimEntry *entry;
	int numAnims;
	int i;
	int j;

	if ( infoIndex )
	{
		entry = &tree->anims->entries[tree->anims->entries[infoIndex].parent];
		newGoalWeight = 0.0;
		numAnims = entry->numAnims;

		for ( i = 0; i < numAnims; ++i )
		{
			childIndex = tree->children[i + entry->u.animParent.children];

			if ( childIndex )
				weight = g_xAnimInfo[childIndex].state.weight;
			else
				weight = 0.0;

			if ( i + entry->u.animParent.children == infoIndex )
			{
				weightDiff = goalWeight - weight;
				absWeight = fabs(weightDiff);
			}
			else
			{
				absWeight = weight;
			}
			if ( absWeight > newGoalWeight )
				newGoalWeight = absWeight;
		}

		for ( j = 0; j < numAnims; ++j )
		{
			if ( j + entry->u.animParent.children != infoIndex )
			{
				blendTime = newGoalWeight * goalTime;
				XAnimClearGoalWeight(tree, j + entry->u.animParent.children, blendTime);
			}
		}
	}
}

void XAnimInitTime(XAnimTree_s *tree, unsigned int infoIndex, float goalTime)
{
	while ( infoIndex )
	{
		infoIndex = tree->anims->entries[infoIndex].parent;

		if ( tree->children[infoIndex] )
			break;

		XAnimSetGoalWeightInternal(tree, infoIndex, 0.0, goalTime, 1.0, 1, 0, 0);
	}
}

void XAnimSetCompleteGoalWeightKnobAll(XAnimTree_s *tree, unsigned int animIndex, unsigned int rootIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, int bRestart)
{
	if ( goalWeight < 0.001 )
		goalWeight = 0.0;

	XAnimClearGoalWeightKnobInternal(tree, animIndex, goalWeight, goalTime);
	XAnimSetGoalWeightInternal(tree, animIndex, goalWeight, goalTime, rate, 0, notifyName, 0);
	XAnimInitTime(tree, animIndex, goalTime);

	if ( bRestart )
RESTART:
		XAnimRestart(tree, animIndex);

	while ( animIndex )
	{
		animIndex = tree->anims->entries[animIndex].parent;

		if ( animIndex == rootIndex )
			break;

		XAnimClearGoalWeightKnobInternal(tree, animIndex, 1.0, goalTime);
		XAnimSetGoalWeightInternal(tree, animIndex, 1.0, goalTime, 1.0, 0, 0, 0);

		if ( bRestart )
			goto RESTART;
	}
}

void XAnimClearTreeGoalWeights(XAnimTree_s *tree, unsigned int animIndex, float blendTime)
{
	XAnimEntry *entry;
	int i;
	int numAnims;

	if ( tree->children[animIndex] )
	{
		XAnimClearGoalWeight(tree, animIndex, blendTime);
		entry = &tree->anims->entries[animIndex];
		numAnims = entry->numAnims;

		for ( i = 0; i < numAnims; ++i )
			XAnimClearTreeGoalWeights(tree, i + entry->u.animParent.children, blendTime);
	}
}

void XAnimClearTreeGoalWeightsStrict(XAnimTree_s *tree, unsigned int animIndex, float blendTime)
{
	XAnimEntry *entry;
	int i;
	int numAnims;

	entry = &tree->anims->entries[animIndex];
	numAnims = entry->numAnims;

	for ( i = 0; i < numAnims; ++i )
		XAnimClearTreeGoalWeights(tree, i + entry->u.animParent.children, blendTime);
}

int XAnimGetNumChildren(const XAnim_s *anim, unsigned int animIndex)
{
	return anim->entries[animIndex].numAnims;
}

unsigned int XAnimGetChildAt(const XAnim_s *anim, unsigned int animIndex, unsigned int childIndex)
{
	return childIndex + anim->entries[animIndex].u.animParent.children;
}

void XAnimSetGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart)
{
	if ( goalWeight < 0.001 )
		goalWeight = 0.0;

	XAnimSetGoalWeightInternal(tree, animIndex, goalWeight, goalTime, rate, 0, notifyName, notifyType);
	XAnimInitTime(tree, animIndex, goalTime);

	if ( bRestart )
		XAnimRestart(tree, animIndex);
}

float XAnimGetAverageRateFrequency(const XAnimTree_s *tree, unsigned int infoIndex)
{
	float freq;
	XAnimEntry *entry;
	float rate;
	float trans;
	float weight;
	XAnimInfo *info;
	uint16_t index;
	int i;
	int numAnims;

	entry = &tree->anims->entries[infoIndex];
	numAnims = entry->numAnims;

	if ( entry->numAnims )
	{
		trans = 0.0;
		rate = 0.0;

		for ( i = 0; i < numAnims; ++i )
		{
			index = tree->children[i + entry->u.animParent.children];

			if ( index )
			{
				info = &g_xAnimInfo[index];
				weight = info->state.weight;

				if ( weight != 0.0 )
				{
					freq = XAnimGetAverageRateFrequency(tree, i + entry->u.animParent.children);

					if ( freq != 0.0 )
					{
						trans = trans + weight;
						rate = freq * weight * info->state.rate + rate;
					}
				}
			}
		}

		if ( trans == 0.0 )
			return 0.0;
		else
			return rate / trans;
	}
	else
	{
		return tree->anims->entries[infoIndex].u.parts->frequency;
	}
}

void XAnimFillInSyncNodes_r(XAnim_s *anims, unsigned int animIndex, bool bLoop)
{
	uint16_t loopFlag;
	uint16_t flags;
	int count;
	int i;
	int numAnims;
	XAnimEntry *entry;

	entry = &anims->entries[animIndex];
	numAnims = entry->numAnims;

	if ( entry->numAnims )
	{
		if ( (anims->entries[animIndex].u.animParent.flags & 3) != 0 )
		{
			count = 0;
			do
			{
				++count;
				entry = &anims->entries[entry->u.animParent.children];
			}
			while ( entry->numAnims );
			Com_Error(ERR_DROP, "duplicate specification of animation sync in '%s', %d nodes above '%s'", anims->debugName, count, XAnimGetAnimDebugName(anims, animIndex));
		}

		flags = anims->entries[animIndex].u.animParent.flags;

		if ( bLoop )
			loopFlag = flags | 1;
		else
			loopFlag = flags | 2;

		anims->entries[animIndex].u.animParent.flags = loopFlag;

		for ( i = 0; i < numAnims; ++i )
			XAnimFillInSyncNodes_r(anims, i + anims->entries[animIndex].u.animParent.children, bLoop);
	}
	else if ( anims->entries[animIndex].u.parts->looping != bLoop )
	{
		if ( !XanimIsDefaultPart(anims->entries[animIndex].u.parts) )
		{
			if ( bLoop )
			{
				Com_Error(ERR_DROP, "animation '%s' in '%s' cannot be sync looping and nonlooping", XAnimGetAnimDebugName(anims, animIndex), anims->debugName);
			}

			Com_Error(ERR_DROP, "animation '%s' in '%s' cannot be sync nonlooping and looping", XAnimGetAnimDebugName(anims, animIndex), anims->debugName);
		}

		XAnimPrecache("void_loop", Hunk_AllocXAnimPrecache);
		anims->entries[animIndex].u.parts = XAnimFindData("void_loop");

		if ( !anims->entries[animIndex].u.parts )
			Com_Error(ERR_DROP, "Cannot find 'xanim/%s'.\nThis is a default xanim file that you should have.\n", "void_loop");
	}
}

void XAnimSetupSyncNodes_r(XAnim_s *anims, unsigned int animIndex)
{
	int loop;
	int i;
	int j;
	int numAnims;

	numAnims = anims->entries[animIndex].numAnims;

	if ( anims->entries[animIndex].numAnims )
	{
		loop = anims->entries[animIndex].u.animParent.flags & 3;

		if ( loop )
		{
			if ( loop == 3 )
				Com_Error(ERR_DROP, "animation cannot be sync looping and sync nonlooping");

			anims->entries[animIndex].u.animParent.flags |= 4u;

			for ( i = 0; i < numAnims; ++i )
				XAnimFillInSyncNodes_r(anims, i + anims->entries[animIndex].u.animParent.children, loop == 1);
		}
		else
		{
			for ( j = 0; j < numAnims; ++j )
				XAnimSetupSyncNodes_r(anims, j + anims->entries[animIndex].u.animParent.children);
		}
	}
}

void XAnimSetupSyncNodes(XAnim_s *anims)
{
	XAnimSetupSyncNodes_r(anims, 0);
}

unsigned short XAnimGetNextNotifyIndex(const XAnimEntry *entry, float dtime)
{
	int i;
	XAnimNotifyInfo *animNotify;
	float time;
	float partsTime;
	XAnimNotifyInfo *partsNotify;
	XAnimParts *parts;

	parts = entry->u.parts;
	animNotify = 0;
	time = 2.0;
	partsNotify = parts->notify;

	for ( i = 0; i < parts->notifyCount; ++i )
	{
		partsTime = partsNotify->time;

		if ( dtime <= partsTime && time > partsTime )
		{
			time = partsNotify->time;
			animNotify = partsNotify;
		}

		++partsNotify;
	}

	return animNotify - parts->notify;
}

void XAnimSetNotifyIndex(const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry)
{
	if ( info->state.time != 1.0 )
	{
		if ( entry->numAnims )
		{
			if ( !info->notifyChild )
				return;

			entry = &tree->anims->entries[info->notifyChild];
		}

		info->notifyIndex = XAnimGetNextNotifyIndex(entry, info->state.time);
	}
}

void XAnimAddServerNotifyNamed(const XAnimTree_s *tree, unsigned int notifyName, unsigned int name)
{
	Scr_AddConstString(name);
	Scr_NotifyNum(tree->parent - 1, 0, notifyName, 1u);
}

void XAnimProcessServerNotify(const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry, float dtime)
{
	int notifyIndex;
	XAnimNotifyInfo *notify;
	XAnimNotifyInfo *notifyIter;
	XAnimParts *parts;

	if ( tree->parent && info->notifyName )
	{
		if ( info->state.time == 1.0 )
		{
			Scr_AddConstString(g_end);
			Scr_NotifyNum(tree->parent - 1, 0, info->notifyName, 1u);
			return;
		}

		if ( info->notifyIndex >= 0 || (XAnimSetNotifyIndex(tree, info, entry), info->notifyIndex >= 0) )
		{
			if ( entry->numAnims )
				entry = &tree->anims->entries[info->notifyChild];

			parts = entry->u.parts;
			notifyIndex = info->notifyIndex;
			notify = &parts->notify[notifyIndex];

			if ( info->state.time <= dtime )
			{
				if ( dtime == 1.0 )
				{
					if ( info->state.time <= notify->time )
					{
						do
						{
							XAnimAddServerNotifyNamed(tree, info->notifyName, notify->name);
							++notify;
							++notifyIndex;
						}
						while ( notifyIndex < parts->notifyCount );
					}
				}
				else if ( notify->time < dtime && info->state.time <= notify->time )
				{
					do
					{
						XAnimAddServerNotifyNamed(tree, info->notifyName, notify->name);
						++notify;
						++notifyIndex;
					}
					while ( notifyIndex < parts->notifyCount && dtime > notify->time );
				}
			}
			else if ( dtime <= notify->time )
			{
				if ( info->state.time <= notify->time )
				{
					do
					{
						XAnimAddServerNotifyNamed(tree, info->notifyName, notify->name);
						++notify;
						++notifyIndex;
					}
					while ( notifyIndex < parts->notifyCount );

					for ( notifyIter = parts->notify; dtime > notifyIter->time; ++notifyIter )
						XAnimAddServerNotifyNamed(tree, info->notifyName, notifyIter->name);
				}
			}
			else
			{
				do
				{
					XAnimAddServerNotifyNamed(tree, info->notifyName, notify->name);
					++notify;
					++notifyIndex;
				}
				while ( notifyIndex < parts->notifyCount && dtime > notify->time );
			}
		}
		else if ( info->state.time > dtime || dtime == 1.0 )
		{
			Scr_AddConstString(g_end);
			Scr_NotifyNum(tree->parent - 1, 0, info->notifyName, 1u);
		}
	}
}

float XAnimGetWeight(const XAnimTree_s *tree, unsigned int animIndex)
{
	uint16_t childIndex;

	childIndex = tree->children[animIndex];

	if ( childIndex )
		return g_xAnimInfo[childIndex].state.weight;
	else
		return 0.0;
}

void XAnimUpdateOldTime(XAnimTree_s *tree, unsigned int infoIndex, XAnimState *syncState, float dtime, bool parentHasWeight, bool *childHasTimeForParent1, bool *childHasTimeForParent2)
{
	bool bTime;
	bool bWeight;
	XAnimDeltaInfo childInfo;
	XAnimDeltaInfo parentInfo;
	XAnimState *state;
	XAnimInfo *info;
	int index;
	int numAnims;
	int i;

	parentInfo.hasWeight = parentHasWeight;
	index = tree->children[infoIndex];

	if ( index )
	{
		info = &g_xAnimInfo[index];
		state = &info->state;
		bWeight = 0;

		if ( parentInfo.hasWeight && state->weight != 0.0 )
			bWeight = 1;

		childInfo.hasWeight = bWeight;

		if ( parentInfo.hasWeight && dtime < state->goalTime )
		{
			state->weight = (state->goalWeight - state->weight) * dtime / state->goalTime + state->weight;

			if ( state->weight < 0.0000010000001 )
				state->weight = state->goalWeight * 0.001;

			state->goalTime = state->goalTime - dtime;
		}
		else
		{
			state->weight = state->goalWeight;
			state->goalTime = 0.0;
		}

		bTime = 0;

		if ( childInfo.hasWeight || state->goalWeight != 0.0 )
			bTime = 1;

		parentInfo.hasTime = bTime;
		childInfo.entry = &tree->anims->entries[infoIndex];
		numAnims = childInfo.entry->numAnims;

		if ( numAnims )
		{
			childInfo.hasTime = 0;

			if ( (childInfo.entry->u.animParent.flags & 4) != 0 )
				syncState = state;

			for ( i = 0; i < numAnims; ++i )
				XAnimUpdateOldTime(
				    tree,
				    i + childInfo.entry->u.animParent.children,
				    syncState,
				    dtime,
				    childInfo.hasWeight,
				    &parentInfo.hasTime,
				    &childInfo.hasTime);
		}
		else
		{
			childInfo.hasTime = 0.0 != childInfo.entry->u.parts->frequency;
		}
		if ( parentInfo.hasTime )
		{
			if ( childInfo.hasWeight && childInfo.hasTime )
			{
				*childHasTimeForParent2 = 1;
			}
			else if ( numAnims && (childInfo.entry->u.animParent.flags & 4) != 0 )
			{
				if ( XAnimNeedClearState(info) )
				{
					for ( i = 0; i < numAnims; ++i )
						XAnimResetTime(tree, i + childInfo.entry->u.animParent.children);
				}
			}
			else if ( state->time != syncState->time || state->cycleCount != syncState->cycleCount )
			{
				state->time = syncState->time;
				state->cycleCount = syncState->cycleCount;
				info->notifyIndex = -1;
			}

			state->oldTime = state->time;
			state->oldCycleCount = state->cycleCount;
			*childHasTimeForParent1 = 1;
		}
		else
		{
			XAnimFreeInfo(tree, index);
			tree->children[infoIndex] = 0;
		}
	}
}

void XAnimSetAnimRate(XAnimTree_s *tree, unsigned int animIndex, float rate)
{
	g_xAnimInfo[tree->children[animIndex]].state.rate = rate;
}

bool XAnimIsLooped(const XAnim_s *anim, unsigned int animIndex)
{
	if ( anim->entries[animIndex].numAnims )
		return anim->entries[animIndex].u.animParent.flags & 1;
	else
		return anim->entries[animIndex].u.parts->looping;
}

bool XAnimIsPrimitive(XAnim_s *anim, unsigned int animIndex)
{
	return anim->entries[animIndex].numAnims == 0;
}

float XAnimGetLength(const XAnim_s *anims, unsigned int animIndex)
{
	return (float)anims->entries[animIndex].u.parts->numframes / anims->entries[animIndex].u.parts->framerate;
}

int XAnimGetLengthMsec(const XAnim_s *anim, unsigned int animIndex)
{
	return (int)(XAnimGetLength(anim, animIndex) * 1000.0);
}

float XAnimGetNotifyFracLeaf(const XAnimState *state, const XAnimState *nextState, float time, float dtime)
{
	if ( nextState->oldTime == 1.0 )
	{
		return 1.0;
	}
	else if ( nextState->oldTime <= nextState->time )
	{
		if ( (nextState->time > time || nextState->time == 1.0) && time >= nextState->oldTime )
			return ((float)(nextState->oldCycleCount - state->oldCycleCount) + time - state->oldTime) / dtime;
		else
			return 1.0;
	}
	else if ( nextState->time <= time )
	{
		if ( time < nextState->oldTime )
			return 1.0;
		else
			return ((float)(nextState->oldCycleCount - state->oldCycleCount) + time - state->oldTime) / dtime;
	}
	else
	{
		return ((float)(nextState->oldCycleCount - state->oldCycleCount + 1) + time - state->oldTime) / dtime;
	}
}

void XAnimAddClientNotify(const XAnimEntry *entry, unsigned int notetrackName, float frac, unsigned int notifyType)
{
	int i;
	XAnimClientNotify *current;
	XAnimClientNotify *next;

	for ( i = g_notifyListSize - 1; i >= 0; --i )
	{
		current = &g_notifyList[i];
		next = &g_notifyList[i + 1];

		if ( frac >= current->timeFrac )
			break;

		next->name = current->name;
		next->notifyName = current->notifyName;
		next->timeFrac = current->timeFrac;
	}

	next = &g_notifyList[i + 1];
	next->name = SL_ConvertToString(notetrackName);
	next->timeFrac = frac;
	next->notifyName = notifyType;

	++g_notifyListSize;
}

void XAnimProcessClientNotify(XAnimInfo *info, const XAnimEntry *entry, float dtime)
{
	float frac;
	uint16_t type;
	const XAnimState *state;
	unsigned short nextNotify;
	XAnimNotifyInfo *noteTrack;
	XAnimNotifyInfo *notifyIter;
	XAnimParts *parts;

	state = &info->state;
	type = info->notifyType;

	if ( type )
	{
		if ( info->state.oldTime == 1.0 )
		{
			frac = XAnimGetNotifyFracLeaf(state, state, 1.0, dtime);
			XAnimAddClientNotify(entry, g_end, frac, type);
			return;
		}
		if ( entry->numAnims )
		{
			if ( info->state.oldTime > info->state.time || state->time == 1.0 )
			{
				frac = XAnimGetNotifyFracLeaf(state, state, 1.0, dtime);
				XAnimAddClientNotify(entry, g_end, frac, type);
				return;
			}
		}
		else
		{
			parts = entry->u.parts;
			nextNotify = XAnimGetNextNotifyIndex(entry, info->state.oldTime);
			noteTrack = &parts->notify[nextNotify];
			if ( info->state.oldTime <= info->state.time )
			{
				if ( state->time == 1.0 )
				{
					if ( info->state.oldTime <= noteTrack->time )
					{
						do
						{
							frac = XAnimGetNotifyFracLeaf(state, state, noteTrack->time, dtime);
							XAnimAddClientNotify(entry, noteTrack->name, frac, type);
							++noteTrack;
							++nextNotify;
						}
						while ( nextNotify < (unsigned int)parts->notifyCount );
					}
				}
				else if ( noteTrack->time < state->time && info->state.oldTime <= noteTrack->time )
				{
					do
					{
						frac = XAnimGetNotifyFracLeaf(state, state, noteTrack->time, dtime);
						XAnimAddClientNotify(entry, noteTrack->name, frac, type);
						++noteTrack;
						++nextNotify;
					}
					while ( nextNotify < (unsigned int)parts->notifyCount && state->time > noteTrack->time );
				}
			}
			else if ( state->time <= noteTrack->time )
			{
				if ( info->state.oldTime <= noteTrack->time )
				{
					do
					{
						frac = XAnimGetNotifyFracLeaf(state, state, noteTrack->time, dtime);
						XAnimAddClientNotify(entry, noteTrack->name, frac, type);
						++noteTrack;
						++nextNotify;
					}
					while ( nextNotify < (unsigned int)parts->notifyCount );
					for ( notifyIter = parts->notify; state->time > notifyIter->time; notifyIter += 2 )
					{
						frac = XAnimGetNotifyFracLeaf(state, state, notifyIter->time, dtime);
						XAnimAddClientNotify(entry, notifyIter->name, frac, type);
					}
				}
			}
			else
			{
				do
				{
					frac = XAnimGetNotifyFracLeaf(state, state, noteTrack->time, dtime);
					XAnimAddClientNotify(entry, noteTrack->name, frac, type);
					++noteTrack;
					++nextNotify;
				}
				while ( nextNotify < (unsigned int)parts->notifyCount && state->time > noteTrack->time );
			}
		}
	}
}

void XAnimUpdateInfoSyncInternal(const XAnimTree_s *tree, unsigned int index, bool update, XAnimState *state, float dtime)
{
	XAnimEntry *entry;
	float *pOldTime;
	XAnimInfo *info;
	uint16_t childIndex;
	int numAnims;
	int i;

	childIndex = tree->children[index];

	if ( childIndex )
	{
		info = &g_xAnimInfo[childIndex];
		pOldTime = &info->state.time;

		if ( info->state.weight != 0.0 )
		{
			if ( info->state.goalWeight == 0.0 )
				update = 0;

			entry = &tree->anims->entries[index];

			if ( info->state.oldTime != state->oldTime || info->state.oldCycleCount != state->oldCycleCount )
			{
				*pOldTime = state->oldTime;
				info->state.cycleCount = state->oldCycleCount;
				info->state.oldTime = state->oldTime;
				info->state.oldCycleCount = state->oldCycleCount;
				info->notifyIndex = -1;
			}

			if ( update )
				XAnimProcessServerNotify(tree, info, entry, state->time);

			*pOldTime = state->time;
			info->state.cycleCount = state->cycleCount;
			info->notifyIndex = -1;

			if ( update )
				XAnimProcessClientNotify(info, entry, dtime);

			numAnims = entry->numAnims;

			for ( i = 0; i < numAnims; ++i )
				XAnimUpdateInfoSyncInternal(tree, i + entry->u.animParent.children, update, state, dtime);
		}
	}
}

void QDECL XAnimUpdateInfoInternal(XAnimTree_s *tree, unsigned int infoIndex, float dtime, bool update)
{
	int16_t cycleCount;
	int16_t oldCycleCount;
	float frameTime;
	XAnimParts *parts;
	XAnimEntry *entry;
	XAnimState *state;
	XAnimInfo *info;
	uint16_t index;
	int numAnims;
	int i;
	int j;
	float freq;

	index = tree->children[infoIndex];

	if ( index )
	{
		info = &g_xAnimInfo[index];
		state = &info->state;

		if ( info->state.weight != 0.0 )
		{
			if ( info->state.goalWeight == 0.0 )
				update = 0;

			entry = &tree->anims->entries[infoIndex];
			numAnims = entry->numAnims;

			if ( entry->numAnims )
			{
				if ( (tree->anims->entries[infoIndex].u.animParent.flags & 3) != 0 )
				{
					freq = XAnimGetAverageRateFrequency(tree, infoIndex) * info->state.rate * dtime;

					if ( freq != 0.0 )
					{
						frameTime = info->state.oldTime + freq;
						oldCycleCount = info->state.oldCycleCount;

						if ( frameTime >= 1.0 )
						{
							if ( (entry->u.animParent.flags & 2) != 0 )
							{
								frameTime = 1.0;
							}
							else
							{
								do
								{
									frameTime = frameTime - 1.0;
									++oldCycleCount;
								}
								while ( frameTime >= 1.0 );
							}
						}

						if ( info->state.time - frameTime <= (float)(oldCycleCount - info->state.cycleCount) )
						{
							if ( update )
								XAnimProcessServerNotify(tree, info, entry, frameTime);

							state->time = frameTime;
							info->state.cycleCount = oldCycleCount;
							info->notifyIndex = -1;

							if ( update )
								XAnimProcessClientNotify(info, entry, freq);
							for ( i = 0; i < numAnims; ++i )
								XAnimUpdateInfoSyncInternal(tree, i + entry->u.animParent.children, update, state, freq);
						}
					}
				}
				else
				{
					freq = dtime * info->state.rate;

					if ( freq != 0.0 )
					{
						for ( j = 0; j < numAnims; ++j )
							XAnimUpdateInfoInternal(tree, j + entry->u.animParent.children, freq, update);
					}
				}
			}
			else
			{
				parts = tree->anims->entries[infoIndex].u.parts;
				freq = info->state.rate * parts->frequency * dtime;

				if ( freq != 0.0 )
				{
					frameTime = info->state.oldTime + freq;
					cycleCount = info->state.cycleCount;

					if ( frameTime >= 1.0 )
					{
						if ( parts->looping )
						{
							do
							{
								frameTime = frameTime - 1.0;
								++cycleCount;
							}
							while ( frameTime >= 1.0 );
						}
						else
						{
							frameTime = 1.0;
						}
					}

					if ( info->state.time - frameTime <= (float)(cycleCount - info->state.cycleCount) )
					{
						if ( update )
							XAnimProcessServerNotify(tree, info, entry, frameTime);

						state->time = frameTime;
						info->state.cycleCount = cycleCount;
						info->notifyIndex = -1;

						if ( update )
							XAnimProcessClientNotify(info, entry, freq);
					}
				}
			}
		}
	}
}

float XAnimGetNotifyFracServer(const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry, const XAnimState *state, const XAnimState *nextState, float dtime)
{
	XAnimParts *parts;

	if ( tree->parent && info->notifyName )
	{
		if ( entry->numAnims )
		{
			if ( !info->notifyChild )
				return XAnimGetNotifyFracLeaf(state, nextState, 1.0, dtime);
			entry = &tree->anims->entries[info->notifyChild];
		}

		parts = entry->u.parts;

		if ( info->notifyIndex >= 0 )
			return XAnimGetNotifyFracLeaf(state, nextState, parts->notify[info->notifyIndex].time, dtime);

		XAnimSetNotifyIndex(tree, info, entry);

		if ( info->notifyIndex >= 0 )
			return XAnimGetNotifyFracLeaf(state, nextState, parts->notify[info->notifyIndex].time, dtime);

		return XAnimGetNotifyFracLeaf(state, nextState, 1.0, dtime);
	}

	return 1.0;
}

float XAnimGetServerNotifyFracSyncTotal(const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry, const XAnimState *state, const XAnimState *nextState, float dtime)
{
	uint16_t animIndex;
	int childIndex;
	float totalFrac;
	int i;
	float notifyFrac;
	XAnimInfo *animInfo;

	notifyFrac = XAnimGetNotifyFracServer(tree, info, entry, state, nextState, dtime);

	for ( i = 0; i < entry->numAnims; ++i )
	{
		childIndex = i + entry->u.animParent.children;
		animIndex = tree->children[childIndex];

		if ( animIndex )
		{
			animInfo = &g_xAnimInfo[animIndex];

			if ( animInfo->state.weight != 0.0 && animInfo->state.goalWeight != 0.0 )
			{
				totalFrac = XAnimGetServerNotifyFracSyncTotal(
				                tree,
				                animInfo,
				                &tree->anims->entries[childIndex],
				                state,
				                nextState,
				                dtime);

				if ( notifyFrac > totalFrac )
					notifyFrac = totalFrac;
			}
		}
	}

	return notifyFrac;
}

float QDECL XAnimFindServerNoteTrack(const XAnimTree_s *anim, unsigned int infoIndex, float dtime)
{
	XAnimState localState;
	float rate;
	float noteTrack;
	int16_t newCycleCount;
	float newAnimTime;
	XAnimParts *animParts;
	XAnimEntry *animEntry;
	XAnimState *animState;
	XAnimInfo *info;
	unsigned short childIndex;
	int numAnims;
	int i;
	float frameRate;
	float averageRate;
	float frameTime;

	childIndex = anim->children[infoIndex];

	if ( childIndex )
	{
		info = &g_xAnimInfo[childIndex];
		animState = &info->state;

		if ( info->state.weight == 0.0 || animState->goalWeight == 0.0 )
		{
			return 1.0;
		}
		else
		{
			animEntry = &anim->anims->entries[infoIndex];
			numAnims = animEntry->numAnims;

			if ( numAnims )
			{
				if ( (animEntry->u.animParent.flags & 3) != 0 )
				{
					averageRate = XAnimGetAverageRateFrequency(anim, infoIndex) * animState->rate * dtime;

					if ( averageRate == 0.0 )
					{
						return 1.0;
					}
					else
					{
						newAnimTime = animState->oldTime + averageRate;
						newCycleCount = animState->oldCycleCount;

						if ( (animEntry->u.animParent.flags & 2) != 0 )
						{
							if ( newAnimTime >= 1.0 )
								newAnimTime = 1.0;
						}
						else
						{
							while ( newAnimTime >= 1.0 )
							{
								newAnimTime = newAnimTime - 1.0;
								++newCycleCount;
							}
						}

						if ( animState->time - newAnimTime <= (float)(newCycleCount - animState->cycleCount) )
						{
							localState.oldTime = animState->time;
							localState.oldCycleCount = animState->cycleCount;
							localState.time = newAnimTime;
							localState.cycleCount = newCycleCount;

							return XAnimGetServerNotifyFracSyncTotal(anim, info, animEntry, animState, &localState, averageRate);
						}
						else
						{
							return 1.0;
						}
					}
				}
				else
				{
					frameTime = dtime * animState->rate;

					if ( frameTime == 0.0 )
					{
						return 1.0;
					}
					else
					{
						rate = 1.0;

						for ( i = 0; i < numAnims; ++i )
						{
							noteTrack = XAnimFindServerNoteTrack(anim, i + animEntry->u.animParent.children, frameTime);

							if ( rate > noteTrack )
								rate = noteTrack;
						}

						return rate;
					}
				}
			}
			else
			{
				animParts = animEntry->u.parts;
				frameRate = animState->rate * animParts->frequency * dtime;

				if ( frameRate == 0.0 )
				{
					return 1.0;
				}
				else
				{
					newAnimTime = animState->oldTime + frameRate;
					newCycleCount = animState->oldCycleCount;

					if ( animParts->looping )
					{
						while ( newAnimTime >= 1.0 )
						{
							newAnimTime = newAnimTime - 1.0;
							++newCycleCount;
						}
					}
					else if ( newAnimTime >= 1.0 )
					{
						newAnimTime = 1.0;
					}

					if ( animState->time - newAnimTime <= (float)(newCycleCount - animState->cycleCount) )
					{
						localState.oldTime = animState->time;
						localState.oldCycleCount = animState->cycleCount;
						localState.time = newAnimTime;
						localState.cycleCount = newCycleCount;

						return XAnimGetNotifyFracServer(anim, info, animEntry, animState, &localState, frameRate);
					}
					else
					{
						return 1.0;
					}
				}
			}
		}
	}
	else
	{
		return 1.0;
	}
}

void XAnimClearTreeWeights(XAnimTree_s *info, int index)
{
	XAnimEntry *entry;
	int childIndex;
	int i;
	int numAnims;

	childIndex = info->children[index];

	if ( info->children[index] )
	{
		entry = &info->anims->entries[index];
		numAnims = entry->numAnims;

		for ( i = 0; i < numAnims; ++i )
			XAnimClearTreeWeights(info, i + entry->u.animParent.children);

		XAnimFreeInfo(info, childIndex);
		info->children[index] = 0;
	}
}

void XAnimClearTreeParts(XAnimTree_s *tree)
{
	signed int i;
	uint16_t *childIndex;
	signed int size;

	size = tree->anims->size;
	childIndex = &tree->children[size];

	for ( i = 0; i < size; ++i )
	{
		if ( childIndex[i] )
		{
			SL_RemoveRefToStringOfLen(childIndex[i], tree->anims->entries[i].u.parts->boneCount + 16);
			childIndex[i] = 0;
		}
	}
}

void XAnimClearTree(XAnimTree_s *tree)
{
	XAnimClearTreeWeights(tree, 0);
	XAnimClearTreeParts(tree);
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