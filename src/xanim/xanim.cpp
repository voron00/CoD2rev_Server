#include "../qcommon/qcommon.h"

extern dvar_t *com_developer;
bool g_anim_developer;
unsigned int g_end;

#define MAX_XANIM_INFO 4096

XAnimNotify_s g_notifyList[128];
int g_notifyListSize = 0;

XAnimInfo g_xAnimInfo[MAX_XANIM_INFO];

/*
==================
Hunk_AllocXAnimPrecache
==================
*/
void *Hunk_AllocXAnimPrecache( int size )
{
	return Hunk_AllocAlignInternal( size, 4 );
}

/*
==================
Hunk_AllocXAnimTreePrecache
==================
*/
void *Hunk_AllocXAnimTreePrecache( int size )
{
	return Hunk_AllocAlignInternal( size, 4 );
}

/*
==================
ShortLerpAsVec
==================
*/
float ShortLerpAsVec( short from, short to, float frac )
{
	return from + ( to - from ) * frac;
}

/*
==================
Short2LerpAsVec2
==================
*/
void Short2LerpAsVec2( const short from[2], const short to[2], float frac, vec2_t out )
{
	out[0] = ShortLerpAsVec( from[0], to[0], frac );
	out[1] = ShortLerpAsVec( from[1], to[1], frac );
}

/*
==================
Vec2MadShort2Lerp
==================
*/
void Vec2MadShort2Lerp( const vec2_t from, float scale, const short to1[2], const short to2[2], float frac, vec2_t out )
{
	out[0] = ShortLerpAsVec( to1[0], to2[0], frac ) * scale + from[0];
	out[1] = ShortLerpAsVec( to1[1], to2[1], frac ) * scale + from[1];
}

/*
==================
Vec4MadShort4Lerp
==================
*/
void Vec4MadShort4Lerp( const vec4_t from, float scale, const short to1[4], const short to2[4], float frac, vec4_t out )
{
	out[0] = ShortLerpAsVec( to1[0], to2[0], frac ) * scale + from[0];
	out[1] = ShortLerpAsVec( to1[1], to2[1], frac ) * scale + from[1];
	out[2] = ShortLerpAsVec( to1[2], to2[2], frac ) * scale + from[2];
	out[3] = ShortLerpAsVec( to1[3], to2[3], frac ) * scale + from[3];
}

/*
==================
Vec2MadShort2
==================
*/
void Vec2MadShort2( const vec2_t from, float frac, const short to[2], vec2_t out )
{
	out[0] = to[0] * frac + from[0];
	out[1] = to[1] * frac + from[1];
}

/*
==================
Vec4MadShort4
==================
*/
void Vec4MadShort4( const vec4_t from, float frac, const short to[4], vec4_t out )
{
	out[0] = to[0] * frac + from[0];
	out[1] = to[1] * frac + from[1];
	out[2] = to[2] * frac + from[2];
	out[3] = to[3] * frac + from[3];
}

/*
==================
Short4ScaleAsVec4
==================
*/
void Short4ScaleAsVec4( short from[4], float scale, vec4_t to )
{
	to[0] = from[0] * scale;
	to[1] = from[1] * scale;
	to[2] = from[2] * scale;
	to[3] = from[3] * scale;
}

/*
==================
FloatLerp
==================
*/
float FloatLerp( float a, float b, float t )
{
	return ( b - a ) * t + a;
}

/*
==================
Vec3MadVec3Lerp
==================
*/
void Vec3MadVec3Lerp( const vec3_t from, float scale, const vec3_t to1, const vec3_t to2, float frac, vec3_t out )
{
	out[0] = FloatLerp(to1[0], to2[0], frac) * scale + from[0];
	out[1] = FloatLerp(to1[1], to2[1], frac) * scale + from[1];
	out[2] = FloatLerp(to1[2], to2[2], frac) * scale + from[2];
}

/*
==================
Short2CopyAsVec2
==================
*/
void Short2CopyAsVec2( const short from[2], vec2_t to )
{
	to[0] = from[0];
	to[1] = from[1];
}

/*
==================
IsLeafNode
==================
*/
bool IsLeafNode( const XAnimEntry *anim )
{
	return anim->numAnims == 0;
}

/*
==================
GetAnimInfo
==================
*/
XAnimInfo* GetAnimInfo( int infoIndex )
{
	assert((unsigned)infoIndex < MAX_XANIM_INFO);
	return &g_xAnimInfo[infoIndex];
}

/*
==================
XAnimNotetrackExists
==================
*/
bool XAnimNotetrackExists( const XAnim_s *anims, unsigned int animIndex, unsigned int name )
{
	int notifyIndex;
	XAnimNotifyInfo *notify;
	XAnimParts_s *parts;

	parts = anims->entries[animIndex].u.parts;
	assert(parts);

	notify = parts->notify;

	if ( !notify )
	{
		return false;
	}

	for ( notifyIndex = 0; notifyIndex < parts->notifyCount; notifyIndex++, notify++ )
	{
		if ( notify->name == name )
		{
			return true;
		}
	}

	return false;
}

/*
==================
XAnimIsLooped
==================
*/
bool XAnimIsLooped( const XAnim_s *anims, unsigned int animIndex )
{
	assert(anims);

	if ( anims->entries[animIndex].numAnims )
	{
		return anims->entries[animIndex].u.animParent.flags & XANIM_LOOP_SYNC_TIME;
	}

	return anims->entries[animIndex].u.parts->bLoop;
}

/*
==================
XAnimSetTime
==================
*/
void XAnimSetTime( XAnimTree *tree, unsigned int animIndex, float fTime )
{
	assert(tree);
	assert(tree->anims);
	assert(animIndex < tree->anims->size);

	unsigned int infoIndex = tree->infoArray[animIndex];
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	if ( !infoIndex )
	{
		return;
	}

	XAnimInfo *info = &g_xAnimInfo[infoIndex];
	//assert(XAnimHasTime( tree->anims, animIndex ));

	info->state.time = fTime;
	info->state.cycleCount = 0;
	info->state.oldTime = fTime;
	info->state.oldCycleCount = 0;

	info->notifyIndex = -1;
}

/*
==================
XAnimIsPrimitive
==================
*/
bool XAnimIsPrimitive( XAnim_s *anims, unsigned int animIndex )
{
	assert(anims);
	return anims->entries[animIndex].numAnims == 0;
}

/*
==================
DObjGetClientNotifyList
==================
*/
int DObjGetClientNotifyList( XAnimNotify_s **notifyList )
{
	*notifyList = g_notifyList;
	return g_notifyListSize;
}

/*
==================
XAnimGetAnimTreeSize
==================
*/
unsigned int XAnimGetAnimTreeSize( const XAnim_s *anims )
{
	assert(anims);
	return anims->size;
}

/*
==================
XAnimGetAnimTreeDebugName
==================
*/
const char* XAnimGetAnimTreeDebugName( const XAnim_s *anims )
{
	assert(anims);
	return anims->debugName;
}

/*
==================
XAnimGetAnimName
==================
*/
const char* XAnimGetAnimName( const XAnim_s *anims, unsigned int animIndex )
{
	assert(anims);
	assert(animIndex < anims->size);

	if ( anims->entries[animIndex].numAnims )
	{
		return "";
	}

	return anims->entries[animIndex].u.parts->name;
}

/*
==================
XAnimGetChildAt
==================
*/
unsigned int XAnimGetChildAt( const XAnim_s *anims, unsigned int animIndex, unsigned int childIndex )
{
	assert(anims);
	assert(animIndex < anims->size);
	assert(childIndex < anims->entries[animIndex].numAnims);

	return childIndex + anims->entries[animIndex].u.animParent.children;
}

/*
==================
XAnimGetNumChildren
==================
*/
int XAnimGetNumChildren( const XAnim_s *anims, unsigned int animIndex )
{
	assert(anims);
	assert(animIndex < anims->size);

	return anims->entries[animIndex].numAnims;
}

/*
==================
XAnimHasFinished
==================
*/
bool XAnimHasFinished( const XAnimTree_s *tree, unsigned int animIndex )
{
	assert(tree);
	assert(tree->anims);
	assert(animIndex < tree->anims->size);

	unsigned int infoIndex = tree->infoArray[animIndex];
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	if ( !infoIndex )
	{
		return true;
	}

	XAnimState *state = &g_xAnimInfo[infoIndex].state;
	assert(state);

	return state->oldTime > state->time || state->time == 1.0f || state->cycleCount > state->oldCycleCount;
}

/*
==================
XAnimGetWeight
==================
*/
float XAnimGetWeight( const XAnimTree_s *tree, unsigned int animIndex )
{
	assert(tree);
	assert(tree->anims);
	assert(animIndex < tree->anims->size);

	unsigned int infoIndex = tree->infoArray[animIndex];
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	if ( !infoIndex )
	{
		return 0.0f;
	}

	XAnimState *state = &g_xAnimInfo[infoIndex].state;
	assert(state);

	return state->weight;
}

/*
==================
XAnimGetTime
==================
*/
float XAnimGetTime( const XAnimTree_s *tree, unsigned int animIndex )
{
	assert(tree);
	assert(tree->anims);
	assert(animIndex < tree->anims->size);

	unsigned int infoIndex = tree->infoArray[animIndex];
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	if ( !infoIndex )
	{
		return 0.0f;
	}

	XAnimState *state = &g_xAnimInfo[infoIndex].state;
	assert(state);

	return state->time;
}

/*
==================
XAnimGetLength
==================
*/
float XAnimGetLength( const XAnim_s *anims, unsigned int animIndex )
{
	assert(anims);
	assert(animIndex < anims->size);

	const XAnimEntry *entry = &anims->entries[animIndex];
	assert(entry);
	assert(IsLeafNode( entry ));

	XAnimParts *parts = entry->u.parts;
	assert(parts);

	return parts->numframes / parts->framerate;
}

/*
==================
XAnimGetAnims
==================
*/
XAnim_s* XAnimGetAnims( const XAnimTree_s *tree )
{
	assert(tree);
	return tree->anims;
}

/*
==================
XAnimTreeSize
==================
*/
int XAnimTreeSize( int size )
{
	XAnimTree_s *tree;
	return ( sizeof( tree->anims ) + 1 ) * size + ( sizeof( XAnimTree_s ) - ( sizeof( tree->infoArray ) ) + 1 );
}

/*
==================
XAnimDisableLeakCheck
==================
*/
void XAnimDisableLeakCheck( void )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==================
XAnimAbort
==================
*/
void XAnimAbort()
{
	g_end = 0;
}

/*
==================
XAnimSetAnimRateInternal
==================
*/
static void XAnimSetAnimRateInternal( XAnimTree_s *tree, unsigned int animIndex, float rate )
{
	assert(tree);
	assert(tree->anims);
	assert(animIndex < tree->anims->size);
	assert(rate >= 0);

	unsigned int infoIndex = tree->infoArray[animIndex];
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	g_xAnimInfo[infoIndex].state.rate = rate;
}

/*
==================
XAnimSetAnimRate
==================
*/
void XAnimSetAnimRate( XAnimTree_s *tree, unsigned int animIndex, float rate )
{
	XAnimSetAnimRateInternal( tree, animIndex, rate );
}

/*
==================
XAnimGetLengthMsec
==================
*/
int XAnimGetLengthMsec( const XAnim_s *anims, unsigned int anim )
{
	return XAnimGetLength( anims, anim ) * 1000;
}

/*
==================
XAnimCreateTree
==================
*/
XAnimTree_s* XAnimCreateTree( XAnim_s *anims, Alloc_t Alloc )
{
	assert(anims);
	assert(anims->size);

	int entrySize = XAnimTreeSize( anims->size );
	XAnimTree_s *tree = (XAnimTree_s *)Alloc( entrySize );

	memset(tree, 0, entrySize);
	tree->anims = anims;

	return tree;
}

/*
==================
XAnimFreeList
==================
*/
void XAnimFreeList( XAnim_s *anims )
{
	if ( anims->debugName )
	{
		Z_Free((void *)anims->debugName);
		anims->debugName = NULL;
	}

	if ( !anims->debugAnimNames )
	{
		return;
	}

	for ( int i = 0; i < anims->size; i++ )
	{
		if ( anims->debugAnimNames[i] )
		{
			Z_Free((void *)anims->debugAnimNames[i]);
			anims->debugAnimNames[i] = NULL;
		}
	}

	Z_Free(anims->debugAnimNames);
	anims->debugAnimNames = NULL;
}

/*
==================
XAnimFreeAnims
==================
*/
void XAnimFreeAnims( XAnim_s *anims, Free_t Free )
{
	int entrySize = sizeof( XAnimEntry ) * anims->size + ( sizeof( XAnim_s ) - sizeof( anims->entries ) );

	XAnimFreeList(anims);
	Free(anims, entrySize);
}

/*
==================
XAnimGetAnimDebugName
==================
*/
const char* XAnimGetAnimDebugName( const XAnim_s *anims, unsigned int animIndex )
{
	assert(anims);
	assert(animIndex < anims->size);

	const XAnimEntry *anim = &anims->entries[animIndex];

	if ( anims->debugAnimNames )
	{
		assert(anims->debugAnimNames[animIndex]);
		const char *debugName = anims->debugAnimNames[animIndex];
		bool isDefault = false;

		if ( !anim->numAnims )
		{
			isDefault = XAnimIsDefault(anims->entries[animIndex].u.parts);
		}

		if ( isDefault )
		{
			return va("%s (missing)", debugName);
		}

		return debugName;
	}

	if ( anim->numAnims )
	{
		return va("%i", animIndex);
	}

	return anims->entries[animIndex].u.parts->name;
}

/*
==================
XAnimCreateAnims
==================
*/
XAnim_s* XAnimCreateAnims( const char *debugName, int size, Alloc_t Alloc )
{
	XAnim_s *anims;
	char *name;

	assert(debugName);
	assert(Alloc);

	anims = (XAnim_s *)Alloc( sizeof( XAnimEntry ) * size + ( sizeof( XAnim_s ) - sizeof( anims->entries ) ) );
	anims->size = size;

	if ( g_anim_developer )
	{
		name = (char *)Z_Malloc( strlen( debugName ) + 1 );
		strcpy(name, debugName);

		anims->debugName = name;
		anims->debugAnimNames = (const char **)Z_Malloc( sizeof( *anims->debugAnimNames ) * size );
	}

	if ( Hunk_DataOnHunk( anims ) )
	{
		Hunk_AddData( FILEDATA_XANIMLIST, anims, Alloc );
	}

	return anims;
}

/*
==================
XAnimBlend
==================
*/
void XAnimBlend( XAnim_s *anims, unsigned int animIndex, const char *name, unsigned int children, unsigned int num, unsigned int flags )
{
	char *debugName;
	XAnimEntry *anim;

	anim = &anims->entries[animIndex];
	assert(anim);

	assert(num > 0);
	anim->numAnims = num;
	assert(num == anim->numAnims);

	anim->u.animParent.flags = flags;
	assert(flags == anim->u.animParent.flags);

	anim->u.animParent.children = children;
	assert(children == anim->u.animParent.children);

	for ( int i = 0; i < num; i++ )
	{
		anims->entries[i + anim->u.animParent.children].parent = animIndex;
		assert(animIndex == anims->entries[anim->u.animParent.children + i].parent);
	}

	if ( !anims->debugAnimNames )
	{
		return;
	}

	assert(!anims->debugAnimNames[animIndex]);
	debugName = (char *)Z_Malloc( strlen(name) + 1 );
	strcpy(debugName, name);

	anims->debugAnimNames[animIndex] = debugName;
}

/*
==================
XAnimGetAbsDelta
==================
*/
void XAnimGetAbsDelta( const XAnim_s *anims, unsigned int animIndex, vec2_t rot, vec3_t trans, float fTime )
{
	XAnimSimpleRotPos rotPos;
	const XAnimEntry *anim;
	const XAnimParts *parts;

	anim = &anims->entries[animIndex];
	assert(anim);

	if ( !anim->numAnims )
	{
		parts = anim->u.parts;
		assert(parts);

		if ( parts->bDelta )
		{
			Vector2Clear(rotPos.rot);
			rotPos.posWeight = 0.0f;

			VectorClear(rotPos.pos);
			XAnimCalcAbsDeltaParts(parts, 1.0f, fTime, &rotPos);

			if ( rotPos.rot[0] == 0.0f && rotPos.rot[1] == 0.0f )
			{
				rot[0] = 0.0f;
				rot[1] = 1.0f;
			}
			else
			{
				Vector2Copy(rotPos.rot, rot);
			}

			VectorCopy(rotPos.pos, trans);
			return;
		}
	}

	rot[0] = 0.0f;
	rot[1] = 1.0f;

	VectorClear(trans);
}

/*
==================
XAnimGetRelDelta
==================
*/
void XAnimGetRelDelta( const XAnim_s *anims, unsigned int animIndex, vec2_t rot, vec3_t trans, float time1, float time2 )
{
	XAnimSimpleRotPos rotPos;
	const XAnimEntry *anim;
	const XAnimParts *parts;

	anim = &anims->entries[animIndex];
	assert(anim);

	if ( !anim->numAnims )
	{
		parts = anim->u.parts;
		assert(parts);

		if ( parts->bDelta )
		{
			Vector2Clear(rotPos.rot);
			rotPos.posWeight = 0.0f;

			VectorClear(rotPos.pos);
			XAnimCalcRelDeltaParts(parts, 1.0, time1, time2, &rotPos, 0);

			if ( rotPos.rot[0] == 0.0f && rotPos.rot[1] == 0.0f )
			{
				rot[0] = 0.0f;
				rot[1] = 1.0f;
			}
			else
			{
				Vector2Copy(rotPos.rot, rot);
			}

			VectorCopy(rotPos.pos, trans);
			return;
		}
	}

	rot[0] = 0.0f;
	rot[1] = 1.0f;

	VectorClear(trans);
}

/*
==================
XAnimCalcAbsDelta
==================
*/
void XAnimCalcAbsDelta( XAnimTree_s *tree, unsigned int animIndex, vec2_t rot, vec3_t trans )
{
	XAnimSimpleRotPos rotPos;

	assert(tree);
	assert(tree->anims);
	assert(animIndex < tree->anims->size);

	tree->bAbs = true;
	tree->bUseGoalWeight = true;

	XAnimCalcDeltaTree(tree, animIndex, 1.0f, true, false, &rotPos);

	if ( rotPos.rot[0] == 0.0f && rotPos.rot[1] == 0.0f )
	{
		rot[0] = 0.0f;
		rot[1] = 1.0f;
	}
	else
	{
		Vector2Copy(rotPos.rot, rot);
	}

	VectorCopy(rotPos.pos, trans);
}

/*
==================
XAnimCalcDelta
==================
*/
void XAnimCalcDelta( XAnimTree_s *tree, unsigned int animIndex, vec2_t rot, vec3_t trans, bool bUseGoalWeight )
{
	XAnimSimpleRotPos rotPos;

	assert(tree);
	assert(tree->anims);
	assert(animIndex < tree->anims->size);

	tree->bAbs = false;
	tree->bUseGoalWeight = bUseGoalWeight;

	XAnimCalcDeltaTree(tree, animIndex, 1.0f, true, false, &rotPos);

	if ( rotPos.rot[0] == 0.0 || rotPos.rot[1] == 0.0f )
	{
		rot[0] = 0.0f;
		rot[1] = 1.0f;
	}
	else
	{
		Vector2Copy(rotPos.rot, rot);
	}

	VectorCopy(rotPos.pos, trans);
}

/*
==================
XAnimClearTreeAnimCache
==================
*/
void XAnimClearTreeAnimCache( XAnimTree_s *tree )
{
	assert(tree);
	assert(tree->anims);

	int size = tree->anims->size;
	unsigned short *infoArray = &tree->infoArray[size];

	for ( int i = 0; i < size; i++ )
	{
		if ( infoArray[i] )
		{
			SL_RemoveRefToStringOfLen( infoArray[i], tree->anims->entries[i].u.parts->boneCount + DOBJ_PARTBITS_STRING_LEN );
			infoArray[i] = 0;
		}
	}
}

/*
==================
XAnimFree
==================
*/
void XAnimFree( XAnimParts_s *parts )
{
	XAnimNotifyInfo *notify;
	int notifyInfoIndex, i;

	unsigned short *boneNames = parts->names;
	int boneCount = parts->boneCount;

	for ( i = 0; i < boneCount; i++ )
	{
		SL_RemoveRefToString(boneNames[i]);
	}

	for ( notifyInfoIndex = 0, notify = parts->notify; notifyInfoIndex < parts->notifyCount; notifyInfoIndex++, notify++ )
	{
		SL_RemoveRefToString(notify->name);
	}
}

/*
==================
XAnimShutdown
==================
*/
void XAnimShutdown()
{
	if ( g_end )
	{
		SL_RemoveRefToString(g_end);
		g_end = 0;
	}
}

/*
==================
XAnimInit
==================
*/
void XAnimInit()
{
	for ( int i = 0; i < MAX_XANIM_INFO; i++ )
	{
		g_xAnimInfo[i].prev = ( i + ( MAX_XANIM_INFO - 1 ) ) % MAX_XANIM_INFO;
		g_xAnimInfo[i].next = ( i + 1 ) % MAX_XANIM_INFO;
	}

	g_xAnimInfo->state.time = 0.0f;
	g_xAnimInfo->state.oldTime = 0.0f;

	g_xAnimInfo->state.cycleCount = 0;
	g_xAnimInfo->state.oldCycleCount = 0;

	g_end = SL_GetString_("end", 0);
	g_anim_developer = com_developer->current.integer != 0;
}

/*
==================
XAnimClearGoalWeight
==================
*/
void XAnimClearGoalWeight( XAnimTree_s *tree, unsigned int animIndex, float blendTime )
{
	assert(tree);
	assert(tree->anims);

	unsigned int infoIndex = tree->infoArray[animIndex];
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	if ( !infoIndex )
	{
		return;
	}

	XAnimInfo *info = &g_xAnimInfo[infoIndex];

	if ( info->state.goalWeight == 0.0f )
	{
		if ( info->state.goalTime > blendTime )
		{
			info->state.goalTime = blendTime;
		}
	}
	else
	{
		info->state.goalTime = blendTime;
	}

	info->state.goalWeight = 0.0f;

	if ( blendTime == 0.0f )
	{
		info->state.weight = 0.0f;
	}

	XAnimClearServerNotify(info);
}

/*
==================
XAnimClearChildGoalWeights
==================
*/
void XAnimClearChildGoalWeights( XAnimTree_s *tree, unsigned int animIndex, float blendTime )
{
	XAnimEntry *anim = &tree->anims->entries[animIndex];
	assert(anim);

	int numAnims = anim->numAnims;

	for ( int i = 0; i < numAnims; i++ )
	{
		XAnimClearGoalWeight( tree, i + anim->u.animParent.children, blendTime );
	}
}

/*
==================
XAnimClearTreeGoalWeights
==================
*/
void XAnimClearTreeGoalWeights( XAnimTree_s *tree, unsigned int animIndex, float blendTime )
{
	unsigned int infoIndex = tree->infoArray[animIndex];
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	if ( !infoIndex )
	{
		return;
	}

	XAnimClearGoalWeight(tree, animIndex, blendTime);

	XAnimEntry *anim = &tree->anims->entries[animIndex];
	assert(anim);

	int numAnims = anim->numAnims;

	for ( int i = 0; i < numAnims; i++ )
	{
		XAnimClearTreeGoalWeights( tree, i + anim->u.animParent.children, blendTime );
	}
}

/*
==================
XAnimClone
==================
*/
XAnimParts* XAnimClone( XAnimParts *fromParts, Alloc_t Alloc )
{
	XAnimParts *toParts;
	unsigned int notifyInfoIndex;
	XAnimNotifyInfo *notify;
	int size, i;
	unsigned short *boneNames;

	toParts = (XAnimParts *)Alloc( sizeof( XAnimParts ) );
	*toParts = *fromParts;

	boneNames = toParts->names;
	size = toParts->boneCount;

	for ( i = 0; i < size; i++ )
	{
		SL_AddRefToString( boneNames[i] );
	}

	for ( notifyInfoIndex = 0, notify = toParts->notify; notifyInfoIndex < toParts->notifyCount; notifyInfoIndex++, notify++ )
	{
		SL_AddRefToString( notify->name );
	}

	return toParts;
}

/*
==================
XAnimPrecache
==================
*/
XAnimParts* XAnimPrecache( const char *name, Alloc_t Alloc )
{
	XAnimParts *anim = XAnimFindData(name);

	if ( anim )
	{
		return anim;
	}

	anim = XAnimLoadFile(name, Alloc);

	if ( !anim )
	{
		Com_Printf("WARNING: Couldn't find xanim '%s', using default xanim '%s' instead\n", name, "void");
		anim = XAnimFindData("void");

		if ( !anim )
		{
			anim = XAnimLoadFile("void", Alloc);

			if ( !anim )
			{
				Com_Error(ERR_DROP, "Cannot find xanim '%s'.", "void");
			}

			XAnimSetData("void", anim, Alloc);
		}

		anim = XAnimClone(anim, Alloc);
		anim->isDefault = true;
	}

	anim->name = XAnimSetData(name, anim, Alloc);
	return anim;
}

/*
==================
XAnimClearTree
==================
*/
void XAnimClearTree( XAnimTree_s *tree )
{
	assert(tree);

	XAnimClearTreeWeights(tree, 0);
	XAnimClearTreeAnimCache(tree);
}

/*
==================
XAnimClearTreeGoalWeightsStrict
==================
*/
void XAnimClearTreeGoalWeightsStrict( XAnimTree_s *tree, unsigned int animIndex, float blendTime )
{
	assert(tree);
	assert(animIndex < tree->anims->size);

	XAnimEntry *anim = &tree->anims->entries[animIndex];
	assert(anim);

	int numAnims = anim->numAnims;

	for ( int i = 0; i < numAnims; i++ )
	{
		XAnimClearTreeGoalWeights( tree, i + anim->u.animParent.children, blendTime );
	}
}

/*
==================
DObjInitServerTime
==================
*/
void DObjInitServerTime( DObj *obj, float dtime )
{
	XAnimState syncState;
	XAnimTimeInfo info;

	assert(obj);

	if ( !obj->tree )
	{
		return;
	}

	syncState.time = 0;
	syncState.cycleCount = 0;

	XAnimUpdateOldTime( obj->tree, 0, &syncState, dtime, true, &info.parentHasWeightForChild, &info.childHasTimeForParent );
}

/*
==================
XAnimFreeTree
==================
*/
void XAnimFreeTree( XAnimTree_s *tree, Free_t Free )
{
	assert(tree);
	assert(tree->anims);

	int size = tree->anims->size;
	assert(size);

	XAnimClearTree( tree );

	if ( Free )
	{
		Free( tree, XAnimTreeSize( size ) );
	}
}

/*
==================
XAnimCreate
==================
*/
void XAnimCreate( XAnim_s *anims, unsigned int animIndex, const char *name )
{
	XAnimParts_s *parts = XAnimFindData(name);

	if ( !parts )
	{
		Com_Error(ERR_DROP, "Cannot find xanim '%s'", name);
	}

	assert(parts);

	anims->entries[animIndex].numAnims = 0;
	anims->entries[animIndex].u.parts = parts;

	if ( !anims->debugAnimNames )
	{
		return;
	}

	assert(!anims->debugAnimNames[animIndex]);

	char *debugName = (char *)Z_Malloc( strlen( name ) + 1 );
	strcpy(debugName, name);

	anims->debugAnimNames[animIndex] = debugName;
}

/*
==================
XAnimAllocInfo
==================
*/
XAnimInfo* XAnimAllocInfo( XAnimTree_s *tree, unsigned int infoIndex )
{
	assert(tree);
	assert(tree->anims);
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	unsigned int next = g_xAnimInfo->next;
	assert((unsigned)next < MAX_XANIM_INFO);

	if ( !next )
	{
		Com_Error(ERR_DROP, "exceeded maximum number of anim info");
	}

	g_xAnimInfo->next = g_xAnimInfo[next].next;
	g_xAnimInfo[g_xAnimInfo->next].prev = 0;

	tree->infoArray[infoIndex] = next;
	return &g_xAnimInfo[next];
}

/*
==================
XAnimCloneClientAnimInfo
==================
*/
void XAnimCloneClientAnimInfo( const XAnimInfo *from, XAnimInfo *to )
{
	*to = *from;

	to->notifyChild = 0;
	to->notifyIndex = -1;
	to->notifyName = 0;
}

/*
==================
XAnimCloneClientAnimTree
==================
*/
void XAnimCloneClientAnimTree( const XAnimTree_s *from, XAnimTree_s *to )
{
	assert(from);
	assert(from->anims);
	assert(from->anims->size);
	assert(to);
	assert(to->anims == from->anims);

	int size = from->anims->size;

	for ( int i = 0; i < size; i++ )
	{
		int fromInfoIndex = from->infoArray[i];
		assert((unsigned)fromInfoIndex < MAX_XANIM_INFO);

		if ( fromInfoIndex )
		{
			XAnimCloneClientAnimInfo( &g_xAnimInfo[ fromInfoIndex ], XAnimAllocInfo( to, i ) );
		}
	}
}

/*
==================
XAnimCloneAnimTree
==================
*/
void XAnimCloneAnimTree( const XAnimTree_s *from, XAnimTree_s *to )
{
	XAnimInfo *toInfo;

	assert(from);
	assert(from->anims);
	assert(from->anims->size);
	assert(to);
	assert(to->anims == from->anims);

	int size = from->anims->size;

	for ( int i = 0; i < size; i++ )
	{
		int fromInfoIndex = from->infoArray[i];
		assert((unsigned)fromInfoIndex < MAX_XANIM_INFO);

		if ( fromInfoIndex )
		{
			if ( to->infoArray[i] )
			{
				toInfo = &g_xAnimInfo[to->infoArray[i]];
				XAnimClearServerNotify(toInfo);
			}
			else
			{
				toInfo = XAnimAllocInfo(to, i);
			}

			XAnimCloneAnimInfo( &g_xAnimInfo[ fromInfoIndex ], toInfo );
			continue;
		}

		if ( to->infoArray[i] )
		{
			XAnimFreeInfo(to, to->infoArray[i]);
			to->infoArray[i] = 0;
		}
	}
}

/*
==================
DObjTransfer
==================
*/
void DObjTransfer( const DObj *from, DObj *to )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==================
XAnimSetCompleteGoalWeight
==================
*/
void XAnimSetCompleteGoalWeight( XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart )
{
	if ( goalWeight < WEIGHT_EPSILON )
	{
		goalWeight = 0.0f;
	}

	XAnimSetGoalWeightInternal(tree, animIndex, goalWeight, goalTime, rate, false, notifyName, notifyType);

	unsigned int parentInfoIndex = animIndex;

	while ( parentInfoIndex )
	{
		parentInfoIndex = tree->anims->entries[parentInfoIndex].parent;
		assert((unsigned)parentInfoIndex < MAX_XANIM_INFO);

		if ( !tree->infoArray[parentInfoIndex] || g_xAnimInfo[tree->infoArray[parentInfoIndex]].state.goalWeight == 0.0f )
		{
			XAnimSetGoalWeightInternal(tree, parentInfoIndex, 1.0f, goalTime, 1.0f, false, 0, 0);
		}
	}

	if ( bRestart )
	{
		XAnimRestart(tree, animIndex);
	}
}

/*
==================
XAnimSetupSyncNodes
==================
*/
void XAnimSetupSyncNodes( XAnim_s *anims )
{
	XAnimSetupSyncNodes_r( anims, 0 );
}

/*
==================
XAnimSetCompleteGoalWeightKnobAll
==================
*/
void XAnimSetCompleteGoalWeightKnobAll( XAnimTree_s *tree, unsigned int animIndex, unsigned int rootIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, int bRestart )
{
	assert(animIndex != rootIndex);

	if ( goalWeight < WEIGHT_EPSILON )
	{
		goalWeight = 0.0f;
	}

	XAnimClearGoalWeightKnobInternal(tree, animIndex, goalWeight, goalTime);
	XAnimSetGoalWeightInternal(tree, animIndex, goalWeight, goalTime, rate, false, notifyName, 0);

	XAnimEnsureGoalWeightParent(tree, animIndex, goalTime);

	if ( bRestart )
	{
		XAnimRestart(tree, animIndex);
	}

	while ( animIndex )
	{
		animIndex = tree->anims->entries[animIndex].parent;

		if ( animIndex == rootIndex )
		{
			break;
		}

		XAnimClearGoalWeightKnobInternal(tree, animIndex, 1.0f, goalTime);
		XAnimSetGoalWeightInternal(tree, animIndex, 1.0f, goalTime, 1.0f, false, 0, 0);

		if ( bRestart )
		{
			XAnimRestart(tree, animIndex);
		}
	}
}

/*
==================
XAnimSetCompleteGoalWeightKnob
==================
*/
void XAnimSetCompleteGoalWeightKnob( XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, int bRestart )
{
	if ( goalWeight < WEIGHT_EPSILON )
	{
		goalWeight = 0.0f;
	}

	XAnimClearGoalWeightKnobInternal(tree, animIndex, goalWeight, goalTime);
	XAnimSetCompleteGoalWeight(tree, animIndex, goalWeight, goalTime, rate, notifyName, 0, bRestart);
}

/*
==================
XAnimSetGoalWeight
==================
*/
void XAnimSetGoalWeight( XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart )
{
	if ( goalWeight < WEIGHT_EPSILON )
	{
		goalWeight = 0.0f;
	}

	XAnimSetGoalWeightInternal(tree, animIndex, goalWeight, goalTime, rate, false, notifyName, notifyType);
	XAnimEnsureGoalWeightParent(tree, animIndex, goalTime);

	if ( bRestart )
	{
		XAnimRestart(tree, animIndex);
	}
}

/*
==================
XAnimSetGoalWeightKnob
==================
*/
void XAnimSetGoalWeightKnob( XAnimTree_s *tree, unsigned int infoIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, int bRestart )
{
	if ( goalWeight < WEIGHT_EPSILON )
	{
		goalWeight = 0.0f;
	}

	XAnimClearGoalWeightKnobInternal(tree, infoIndex, goalWeight, goalTime);
	XAnimSetGoalWeight(tree, infoIndex, goalWeight, goalTime, rate, notifyName, 0, bRestart);
}

/*
==================
DObjDisplayAnim
==================
*/
void DObjDisplayAnim( DObj *obj )
{
	if ( !obj->tree )
	{
		Com_Printf("NO TREE\n");
		return;
	}

	XAnimDisplay(obj->tree, 0, 0);
	Com_Printf("\n");
}

/*
==================
DObjCalcAnim
==================
*/
void DObjCalcAnim( const DObj *obj, int *partBits )
{
	XAnimCalcAnimInfo animInfo;
	bool bFinished;
	XBoneHierarchy *hierarchy;
	short *quat;
	XModelParts_s *parts;
	DObjAnimMat *mat;
	DSkel_t *skel;
	int i, j, boneIndex;

	assert(obj);
	skel = obj->skel;
	assert(skel);

	bFinished = true;

	for ( i = 0; i < DOBJ_MAX_PART_BITS; i++ )
	{
		animInfo.animPartBits[i] = skel->partBits.anim[i] | ~partBits[i];

		if ( animInfo.animPartBits[i] != -1 )
		{
			bFinished = false;
		}
	}

	if ( bFinished )
	{
		return;
	}

	for ( i = 0; i < DOBJ_MAX_PART_BITS; i++ )
	{
		skel->partBits.anim[i] |= partBits[i];
		animInfo.ignorePartBits[i] = animInfo.animPartBits[i];
	}

	mat = &skel->Mat;
	assert(mat);

#ifdef DEBUG
	for ( i = 0; i < obj->numBones; i++ )
	{
		if ( testBit( skel->partBits.anim, i ) )
		{
			assert(!IS_NAN((mat[i].quat)[0]) && !IS_NAN((mat[i].quat)[1]) && !IS_NAN((mat[i].quat)[2]) && !IS_NAN((mat[i].quat)[3]));
			assert(!IS_NAN((mat[i].trans)[0]) && !IS_NAN((mat[i].trans)[1]) && !IS_NAN((mat[i].trans)[2]));
		}
	}
#endif

	if ( obj->tree )
	{
		animInfo.ignorePartBits[3] |= HIGH_BIT;
		XAnimCalc(obj, 0, 1.0f, mat, 1, 0, &animInfo, 0);
	}

	boneIndex = 0;

	for ( j = 0; j < obj->numModels; j++ )
	{
		for ( parts = obj->models[j]->parts, i = parts->numRootBones; i; i--, mat++, boneIndex++ )
		{
			if ( testBit( animInfo.animPartBits, boneIndex ) )
			{
				if ( testBit( skel->partBits.anim, boneIndex ) )
				{
					assert(boneIndex < obj->numBones);

					assert(!IS_NAN((mat->quat)[0]) && !IS_NAN((mat->quat)[1]) && !IS_NAN((mat->quat)[2]) && !IS_NAN((mat->quat)[3]));
					assert(!IS_NAN((mat->trans)[0]) && !IS_NAN((mat->trans)[1]) && !IS_NAN((mat->trans)[2]));
				}
			}
			else
			{
				mat->quat[0] = 0.0f;
				mat->quat[1] = 0.0f;
				mat->quat[2] = 0.0f;
				mat->quat[3] = 1.0f;

				VectorClear(mat->trans);
			}
		}

		for ( quat = parts->quats, hierarchy = parts->hierarchy, i = parts->numBones - parts->numRootBones; i; i--, mat++, boneIndex++, quat += 4 )
		{
			if ( testBit( animInfo.animPartBits, boneIndex ) )
			{
				if ( testBit( skel->partBits.anim, boneIndex ) )
				{
					assert(!IS_NAN((mat->quat)[0]) && !IS_NAN((mat->quat)[1]) && !IS_NAN((mat->quat)[2]) && !IS_NAN((mat->quat)[3]));
					assert(!IS_NAN((mat->trans)[0]) && !IS_NAN((mat->trans)[1]) && !IS_NAN((mat->trans)[2]));
				}
			}
			else
			{
				Short4ScaleAsVec4(quat, RANDSCALE, mat->quat);
				VectorClear(mat->trans);
			}
		}
	}
}

/*
==================
DObjUpdateServerInfo
==================
*/
int DObjUpdateServerInfo( DObj *obj, float dtime, int bNotify )
{
	assert(dtime >= 0);

	if ( !obj->tree )
	{
		return 0;
	}

	if ( !bNotify )
	{
		XAnimUpdateInfoInternal(obj->tree, 0, dtime, false);
		return 0;
	}

	float notifyFrac = XAnimFindServerNoteTrack(obj->tree, 0, dtime);

	if ( notifyFrac != 1.0f )
	{
		float notifyTime = dtime * notifyFrac + 0.001f;

		if ( dtime >= notifyTime )
		{
			XAnimUpdateInfoInternal(obj->tree, 0, notifyTime, true);
			return 1;
		}
	}

	XAnimUpdateInfoInternal(obj->tree, 0, dtime, true);
	return 0;
}

/*
==================
DObjUpdateClientInfo
==================
*/
void DObjUpdateClientInfo( DObj *obj, float dtime )
{
	XAnimState syncState;
	XAnimTimeInfo info;

	assert(obj);
	assert(dtime >= 0);
	assert(Sys_IsMainThread());

	g_notifyListSize = 0;

	if ( !obj->tree )
	{
		return;
	}

	syncState.time = 0.0f;
	syncState.cycleCount = 0;

	XAnimUpdateOldTime( obj->tree, 0, &syncState, dtime, true, &info.parentHasWeightForChild, &info.childHasTimeForParent );
	XAnimUpdateInfoInternal( obj->tree, 0, dtime, true );
}

/*
==================
XAnim_GetTimeIndex
==================
*/
static void XAnim_GetTimeIndex_unsigned_short_( const XAnimTime *animTime, const unsigned short *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac )
{
	int index, frameIndex, low, high;

	index = (int)((float)tableSize * animTime->time);
	frameIndex = animTime->frameIndex;

	if ( frameIndex < indices[index] )
	{
		low = 0;
		high = (int)((float)tableSize * animTime->time);

		while ( 1 )
		{
			while ( 1 )
			{
				index = ( high + low ) / 2;

				if ( frameIndex >= indices[index] )
				{
					break;
				}

				high = ( high + low ) / 2;
			}

			if ( frameIndex < indices[index + 1] )
			{
				break;
			}

			low = index + 1;
			high--;

			if ( frameIndex >= indices[high] )
			{
				index = high;
				break;
			}
		}
	}
	else if ( frameIndex >= indices[index + 1] )
	{
		low = index + 1;
		high = tableSize;

		do
		{
			while ( 1 )
			{
				index = ( high + low ) / 2;

				if ( frameIndex < indices[index] )
				{
					break;
				}

				if ( frameIndex < indices[index + 1] )
				{
					assert(frameIndex >= indices[index]);
					assert(frameIndex < indices[index + 1]);

					*keyFrameLerpFrac = ( animTime->frameFrac - (float)indices[index] ) / (float)( indices[index + 1] - indices[index] );
					*keyFrameIndex = index;

					assert(*keyFrameLerpFrac >= 0.0f && *keyFrameLerpFrac <= 1.0f);
					return;
				}

				low = index + 1;
			}

			high = ( high + low ) / 2;
			low++;
		}
		while ( frameIndex >= indices[low] );

		index = low - 1;
	}

	assert(frameIndex >= indices[index]);
	assert(frameIndex < indices[index + 1]);

	*keyFrameLerpFrac = ( animTime->frameFrac - (float)indices[index] ) / (float)( indices[index + 1] - indices[index] );
	*keyFrameIndex = index;

	assert(*keyFrameLerpFrac >= 0.0f && *keyFrameLerpFrac <= 1.0f);
}

/*
==================
XAnim_GetTimeIndexCompressed
==================
*/
void XAnim_GetTimeIndexCompressed_unsigned_short_( const XAnimTime *animTime, const unsigned short *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac )
{
	if ( tableSize < animTime->frameCount )
	{
		XAnim_GetTimeIndex_unsigned_short_(animTime, indices, tableSize, keyFrameIndex, keyFrameLerpFrac);
		return;
	}

	*keyFrameLerpFrac = animTime->frameFrac - (float)animTime->frameIndex;
	*keyFrameIndex = animTime->frameIndex;
}

/*
==================
XAnim_GetTimeIndex
==================
*/
static void XAnim_GetTimeIndex_unsigned_char_( const XAnimTime *animTime, const unsigned char *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac )
{
	int index, frameIndex, low, high;

	index = (int)((float)tableSize * animTime->time);
	frameIndex = animTime->frameIndex;

	if ( frameIndex < indices[index] )
	{
		low = 0;
		high = (int)((float)tableSize * animTime->time);

		while ( 1 )
		{
			while ( 1 )
			{
				index = ( high + low ) / 2;

				if ( frameIndex >= indices[index] )
				{
					break;
				}

				high = ( high + low ) / 2;
			}

			if ( frameIndex < indices[index + 1] )
			{
				break;
			}

			low = index + 1;
			high--;

			if ( frameIndex >= indices[high] )
			{
				index = high;
				break;
			}
		}
	}
	else if ( frameIndex >= indices[index + 1] )
	{
		low = index + 1;
		high = tableSize;

		do
		{
			while ( 1 )
			{
				index = ( high + low ) / 2;

				if ( frameIndex < indices[index] )
				{
					break;
				}

				if ( frameIndex < indices[index + 1] )
				{
					assert(frameIndex >= indices[index]);
					assert(frameIndex < indices[index + 1]);

					*keyFrameLerpFrac = ( animTime->frameFrac - (float)indices[index] ) / (float)( indices[index + 1] - indices[index] );
					*keyFrameIndex = index;

					assert(*keyFrameLerpFrac >= 0.0f && *keyFrameLerpFrac <= 1.0f);
					return;
				}

				low = index + 1;
			}

			high = ( high + low ) / 2;
			low++;
		}
		while ( frameIndex >= indices[low] );

		index = low - 1;
	}

	assert(frameIndex >= indices[index]);
	assert(frameIndex < indices[index + 1]);

	*keyFrameLerpFrac = ( animTime->frameFrac - (float)indices[index] ) / (float)( indices[index + 1] - indices[index] );
	*keyFrameIndex = index;

	assert(*keyFrameLerpFrac >= 0.0f && *keyFrameLerpFrac <= 1.0f);
}

/*
==================
XAnim_GetTimeIndexCompressed
==================
*/
void XAnim_GetTimeIndexCompressed_unsigned_char_( const XAnimTime *animTime, const unsigned char *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac )
{
	if ( tableSize < animTime->frameCount )
	{
		XAnim_GetTimeIndex_unsigned_char_(animTime, indices, tableSize, keyFrameIndex, keyFrameLerpFrac);
		return;
	}

	*keyFrameLerpFrac = animTime->frameFrac - (float)animTime->frameIndex;
	*keyFrameIndex = animTime->frameIndex;
}

/*
==================
XAnimGetDescendantWithGreatestWeight
==================
*/
unsigned int XAnimGetDescendantWithGreatestWeight( const XAnimTree_s *tree, unsigned int infoIndex )
{
	XAnimEntry *anim;
	unsigned int result, test;
	float bestWeight, testWeight;

	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	anim = &tree->anims->entries[infoIndex];
	assert(anim);

	if ( !anim->numAnims )
	{
		return infoIndex;
	}

	bestWeight = 0.0f;
	result = 0;

	for ( int i = 0; i < anim->numAnims; i++ )
	{
		testWeight = g_xAnimInfo[ tree->infoArray[ i + anim->u.animParent.children ] ].state.goalWeight;

		if ( bestWeight < testWeight )
		{
			test = XAnimGetDescendantWithGreatestWeight( tree, i + anim->u.animParent.children );

			if ( test )
			{
				bestWeight = testWeight;
				result = test;
			}
		}
	}

	return result;
}

/*
==================
XAnimInitTime
==================
*/
bool XAnimInitTime( XAnimInfo *info )
{
	assert(info);

	if ( info->state.time == 0.0f && !info->state.cycleCount )
	{
		return false;
	}

	info->state.time = 0.0f;
	info->state.cycleCount = 0;

	info->state.oldTime = 0.0f;
	info->state.oldCycleCount = 0;

	info->notifyIndex = -1;

	return true;
}

/*
==================
XAnimInitInfo
==================
*/
void XAnimInitInfo( XAnimInfo *info )
{
	assert(info);
	memset( &info->state, 0, sizeof( info->state ) );

	info->notifyName = 0;
	info->notifyIndex = -1;

	info->notifyChild = 0;
	info->notifyType = 0;
}

/*
==================
XAnimGetNotifyFracLeaf
==================
*/
float XAnimGetNotifyFracLeaf( const XAnimState *state, const XAnimState *nextState, float fTime, float dtime )
{
	assert(dtime);

	if ( nextState->oldTime == 1.0f )
	{
		return 1.0f;
	}

	if ( nextState->time < nextState->oldTime )
	{
		if ( fTime < nextState->time )
		{
			return ( ( nextState->oldCycleCount - state->oldCycleCount + 1 ) + fTime - state->oldTime ) / dtime;
		}

		if ( nextState->oldTime <= fTime )
		{
			return ( (nextState->oldCycleCount - state->oldCycleCount ) + fTime - state->oldTime ) / dtime;
		}

		return 1.0f;
	}

	if ( ( nextState->time > fTime || nextState->time == 1.0f ) && fTime >= nextState->oldTime )
	{
		return ( ( nextState->oldCycleCount - state->oldCycleCount ) + fTime - state->oldTime ) / dtime;
	}

	return 1.0f;
}

/*
==================
XAnimGetNextNotifyTime
==================
*/
unsigned short XAnimGetNextNotifyTime( const XAnimEntry *entry, float fTime )
{
	XAnimNotifyInfo *notifyInfo, *bestNotifyInfo;
	float bestTime, testTime;
	XAnimParts_s *parts;

	parts = entry->u.parts;
	assert(parts);

	assert(fTime >= 0);
	assert(fTime < 1.f);

	bestNotifyInfo = NULL;
	bestTime = 2.0f;

	notifyInfo = parts->notify;
	assert(notifyInfo);

	for ( int i = 0; i < parts->notifyCount; i++, notifyInfo++ )
	{
		testTime = notifyInfo->time;
		assert(testTime >= 0);

		if ( fTime <= testTime && bestTime > testTime )
		{
			bestTime = notifyInfo->time;
			bestNotifyInfo = notifyInfo;
		}
	}

	assert(bestNotifyInfo);
	assert(bestNotifyInfo == parts->notify || bestNotifyInfo->time > (bestNotifyInfo - 1)->time);

	return bestNotifyInfo - parts->notify;
}

/*
==================
XAnimGetAverageRateFrequency
==================
*/
float XAnimGetAverageRateFrequency( const XAnimTree_s *tree, unsigned int animIndex )
{
	XAnimEntry *anim;
	XAnimInfo *info;
	float totalDtime, totalWeight, weight, frequency;
	unsigned int infoIndex;

	assert(tree);
	assert(tree->anims);

	anim = &tree->anims->entries[animIndex];
	assert(anim);

	int numAnims = anim->numAnims;

	if ( !numAnims )
	{
		const XAnimParts *parts = tree->anims->entries[animIndex].u.parts;
		assert(parts);

		return parts->frequency;
	}

	totalWeight = 0.0f;
	totalDtime = 0.0f;

	for ( int i = 0; i < numAnims; i++ )
	{
		infoIndex = tree->infoArray[i + anim->u.animParent.children];
		assert((unsigned)infoIndex < MAX_XANIM_INFO);

		if ( !infoIndex )
		{
			continue;
		}

		info = &g_xAnimInfo[infoIndex];
		assert(info);

		weight = info->state.weight;
		assert(weight >= 0.0f);

		if ( weight == 0.0f )
		{
			continue;
		}

		frequency = XAnimGetAverageRateFrequency(tree, i + anim->u.animParent.children);

		if ( frequency == 0.0f )
		{
			continue;
		}

		totalWeight = totalWeight + weight;
		totalDtime = frequency * weight * info->state.rate + totalDtime;
	}

	if ( totalWeight == 0.0f )
	{
		return 0.0f;
	}

	return totalDtime / totalWeight;
}

/*
==================
TransformToQuatRefFrame
==================
*/
void TransformToQuatRefFrame( const vec2_t rot, vec3_t trans )
{
	float r, zw, zz, temp;

	zz = rot[0] * rot[0];
	r = rot[1] * rot[1] + zz;

	if ( r == 0.0f )
	{
		return;
	}

	r = 2.0f / r;

	zz = zz * r;
	zw = rot[0] * rot[1] * r;

	temp = ( 1.0f - zz ) * trans[0] + zw * trans[1];

	trans[1] = trans[1] - ( zw * trans[0] + zz * trans[1] );
	trans[0] = temp;
}

/*
==================
XAnim_SetTime
==================
*/
void XAnim_SetTime( float fTime, int frameCount, XAnimTime *animTime )
{
	animTime->time = fTime;
	animTime->frameCount = frameCount;

	animTime->frameFrac = frameCount * fTime;
	animTime->frameIndex = animTime->frameFrac;
}

/*
==================
XAnim_CalcPosDeltaDuringInternal
==================
*/
static void XAnim_CalcPosDeltaDuringInternal_unsigned_short_( XAnimDeltaPartTrans *trans, const float fTime, int frameCount, vec3_t posDelta )
{
	float keyFrameLerpFrac;
	int keyFrameIndex;
	XAnimTime animTime;

	if ( !trans->size )
	{
		VectorCopy( trans->u.frame0, posDelta );
		return;
	}

	XAnim_SetTime( fTime, frameCount, &animTime );
	XAnim_GetTimeIndexCompressed_unsigned_short_( &animTime, trans->u.frames.indices._2, trans->size, &keyFrameIndex, &keyFrameLerpFrac );

	Vec3Lerp( trans->u.frames.frames[keyFrameIndex], trans->u.frames.frames[keyFrameIndex + 1], keyFrameLerpFrac, posDelta );
}

/*
==================
XAnim_CalcPosDeltaDuringInternal
==================
*/
static void XAnim_CalcPosDeltaDuringInternal_unsigned_char_( XAnimDeltaPartTrans *trans, const float fTime, int frameCount, vec3_t posDelta )
{
	float keyFrameLerpFrac;
	int keyFrameIndex;
	XAnimTime animTime;

	if ( !trans->size )
	{
		VectorCopy( trans->u.frame0, posDelta );
		return;
	}

	XAnim_SetTime( fTime, frameCount, &animTime );
	XAnim_GetTimeIndexCompressed_unsigned_char_( &animTime, trans->u.frames.indices._1, trans->size, &keyFrameIndex, &keyFrameLerpFrac );

	Vec3Lerp( trans->u.frames.frames[keyFrameIndex], trans->u.frames.frames[keyFrameIndex + 1], keyFrameLerpFrac, posDelta );
}

/*
==================
XAnim_CalcPosDeltaDuring
==================
*/
void XAnim_CalcPosDeltaDuring_unsigned_short_( XAnimDeltaPart *animDelta, const float fTime, int frameCount, vec3_t posDelta )
{
	assert(frameCount && fTime != 1.0f);

	if ( !animDelta->trans )
	{
		VectorClear( posDelta );
		return;
	}

	XAnim_CalcPosDeltaDuringInternal_unsigned_short_( animDelta->trans, fTime, frameCount, posDelta );
}

/*
==================
XAnim_CalcPosDeltaDuring
==================
*/
void XAnim_CalcPosDeltaDuring_unsigned_char_( XAnimDeltaPart *animDelta, const float fTime, int frameCount, vec3_t posDelta )
{
	assert(frameCount && fTime != 1.0f);

	if ( !animDelta->trans )
	{
		VectorClear( posDelta );
		return;
	}

	XAnim_CalcPosDeltaDuringInternal_unsigned_char_( animDelta->trans, fTime, frameCount, posDelta );
}

/*
==================
XAnimUpdateServerNotify
==================
*/
void XAnimUpdateServerNotify( const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry )
{
	if ( info->state.time == 1.0f )
	{
		return;
	}

	if ( entry->numAnims )
	{
		if ( !info->notifyChild )
		{
			return;
		}

		entry = &tree->anims->entries[info->notifyChild];
	}

	info->notifyIndex = XAnimGetNextNotifyTime( entry, info->state.time );
}

/*
==================
XAnimResetTime
==================
*/
void XAnimResetTime( const XAnimTree_s *tree, unsigned int animIndex )
{
	unsigned int infoIndex = tree->infoArray[animIndex];
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	if ( !infoIndex )
	{
		return;
	}

	XAnimInitTime( &g_xAnimInfo[infoIndex] );

	XAnimEntry *anim = &tree->anims->entries[animIndex];
	assert(anim);

	int numAnims = anim->numAnims;

	for ( int i = 0; i < numAnims; i++ )
	{
		XAnimResetTime( tree, i + anim->u.animParent.children );
	}
}

/*
==================
XAnimGetNotifyFracServer
==================
*/
float XAnimGetNotifyFracServer( const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry, const XAnimState *state, const XAnimState *nextState, float dtime )
{
	assert(dtime);

	if ( !tree->entnum || !info->notifyName )
	{
		return 1.0f;
	}

	if ( entry->numAnims )
	{
		if ( !info->notifyChild )
		{
			return XAnimGetNotifyFracLeaf(state, nextState, 1.0f, dtime);
		}

		entry = &tree->anims->entries[info->notifyChild];
	}

	XAnimParts *parts = entry->u.parts;
	assert(parts);

	if ( info->notifyIndex < 0 )
	{
		XAnimUpdateServerNotify(tree, info, entry);

		if ( info->notifyIndex < 0 )
		{
			return XAnimGetNotifyFracLeaf(state, nextState, 1.0f, dtime);
		}

		return XAnimGetNotifyFracLeaf(state, nextState, parts->notify[info->notifyIndex].time, dtime);
	}

	return XAnimGetNotifyFracLeaf(state, nextState, parts->notify[info->notifyIndex].time, dtime);
}

/*
==================
XAnim_CalcPosDeltaEntireInternal
==================
*/
static void XAnim_CalcPosDeltaEntireInternal( const XAnimDeltaPartTrans *trans, vec3_t posDelta )
{
	if ( !trans->size )
	{
		VectorCopy( trans->u.frame0, posDelta );
		return;
	}

	VectorCopy( trans->u.frames.frames[trans->size], posDelta );
}

/*
==================
XAnim_CalcRotDeltaEntireInternal
==================
*/
static void XAnim_CalcRotDeltaEntireInternal( const XAnimDeltaPartQuat *quat, vec2_t rotDelta )
{
	const short *rotDeltaLastFrame;

	if ( quat->size )
		rotDeltaLastFrame = quat->u.frames.frames[quat->size];
	else
		rotDeltaLastFrame = quat->u.frame0;

	Short2CopyAsVec2( rotDeltaLastFrame, rotDelta );
}

/*
==================
ClearQuat
==================
*/
static void ClearQuat( DObjAnimMat *mat )
{
	assert(mat);

	mat->quat[0] = 0.0f;
	mat->quat[1] = 0.0f;
	mat->quat[2] = 0.0f;
	mat->quat[3] = 0.0f;
}

/*
==================
XAnimClearData
==================
*/
void XAnimClearData( const DObj *obj, DObjAnimMat *rotTransArray, XAnimCalcAnimInfo *info )
{
	for ( int modelPartIndex = 0; modelPartIndex < obj->numBones; modelPartIndex++, rotTransArray++ )
	{
		if ( testBit( info->ignorePartBits, modelPartIndex ) )
		{
			continue;
		}

		ClearQuat(rotTransArray);
		rotTransArray->transWeight = 0.0f;
		VectorClear(rotTransArray->trans);
	}
}

/*
==================
XAnim_CalcPosEnd
==================
*/
void XAnim_CalcPosEnd( const XAnimPartTrans *trans, float weightScale, vec3_t posDelta )
{
	if ( !trans->size )
	{
		VectorMA(posDelta, weightScale, trans->u.frame0, posDelta);
		return;
	}

	VectorMA(posDelta, weightScale, trans->u.frames.frames[trans->size], posDelta);
}

/*
==================
XAnim_CalcRotEnd
==================
*/
void XAnim_CalcRotEnd( const XAnimPartQuat *quat, float weightScale, vec4_t rotDelta )
{
	if ( !quat->size )
	{
		Vec4MadShort4(rotDelta, weightScale, quat->u.frame0, rotDelta);
		return;
	}

	Vec4MadShort4(rotDelta, weightScale, quat->u.frames.u.frames[quat->size], rotDelta);
}

/*
==================
XAnim_CalcSimpleRotEnd
==================
*/
void XAnim_CalcSimpleRotEnd( const XAnimPartQuat *quat, float weightScale, vec2_t rotDelta )
{
	if ( !quat->size )
	{
		Vec2MadShort2(rotDelta, weightScale, quat->u.frame0, rotDelta);
		return;
	}

	Vec2MadShort2(rotDelta, weightScale, quat->u.frames.u.frames2[quat->size], rotDelta);
}

/*
==================
XAnim_CalcPosDuring
==================
*/
void XAnim_CalcPosDuring_unsigned_short_( const XAnimTime *animTime, const XAnimPartTrans *trans, float weightScale, vec3_t posDelta )
{
	float keyFrameLerpFrac;
	int keyFrameIndex;

	if ( !trans->size )
	{
		VectorMA(posDelta, weightScale, trans->u.frame0, posDelta);
		return;
	}

	XAnim_GetTimeIndexCompressed_unsigned_short_( animTime, trans->u.frames.indices._2, trans->size, &keyFrameIndex, &keyFrameLerpFrac );
	Vec3MadVec3Lerp( posDelta, weightScale, trans->u.frames.frames[keyFrameIndex], trans->u.frames.frames[keyFrameIndex + 1], keyFrameLerpFrac, posDelta );
}

/*
==================
XAnim_CalcRotDuring
==================
*/
void XAnim_CalcRotDuring_unsigned_short_( const XAnimTime *animTime, const XAnimPartQuat *quat, float weightScale, vec4_t rotDelta )
{
	float keyFrameLerpFrac;
	int keyFrameIndex;

	if ( !quat->size )
	{
		Vec4MadShort4(rotDelta, weightScale, quat->u.frame0, rotDelta);
		return;
	}

	XAnim_GetTimeIndexCompressed_unsigned_short_( animTime, quat->u.frames.indices._2, quat->size, &keyFrameIndex, &keyFrameLerpFrac );
	Vec4MadShort4Lerp( rotDelta, weightScale, quat->u.frames.u.frames[keyFrameIndex], quat->u.frames.u.frames[keyFrameIndex + 1], keyFrameLerpFrac, rotDelta );
}

/*
==================
XAnim_CalcSimpleRotDuring
==================
*/
void XAnim_CalcSimpleRotDuring_unsigned_short_( const XAnimTime *animTime, const XAnimPartQuat *quat, float weightScale, vec2_t rotDelta )
{
	float keyFrameLerpFrac;
	int keyFrameIndex;

	if ( !quat->size )
	{
		Vec2MadShort2(rotDelta, weightScale, quat->u.frame0, rotDelta);
		return;
	}

	XAnim_GetTimeIndexCompressed_unsigned_short_( animTime, quat->u.frames.indices._2, quat->size, &keyFrameIndex, &keyFrameLerpFrac );
	Vec2MadShort2Lerp( rotDelta, weightScale, quat->u.frames.u.frames2[keyFrameIndex], quat->u.frames.u.frames2[keyFrameIndex + 1], keyFrameLerpFrac, rotDelta );
}

/*
==================
XAnim_CalcPosDuring
==================
*/
void XAnim_CalcPosDuring_unsigned_char_( const XAnimTime *animTime, const XAnimPartTrans *trans, float weightScale, vec3_t posDelta )
{
	float keyFrameLerpFrac;
	int keyFrameIndex;

	if ( !trans->size )
	{
		VectorMA(posDelta, weightScale, trans->u.frame0, posDelta);
		return;
	}

	XAnim_GetTimeIndexCompressed_unsigned_char_( animTime, trans->u.frames.indices._1, trans->size, &keyFrameIndex, &keyFrameLerpFrac );
	Vec3MadVec3Lerp( posDelta, weightScale, trans->u.frames.frames[keyFrameIndex], trans->u.frames.frames[keyFrameIndex + 1], keyFrameLerpFrac, posDelta );
}

/*
==================
XAnim_CalcRotDuring
==================
*/
void XAnim_CalcRotDuring_unsigned_char_( const XAnimTime *animTime, const XAnimPartQuat *quat, float weightScale, vec4_t rotDelta )
{
	float keyFrameLerpFrac;
	int keyFrameIndex;

	if ( !quat->size )
	{
		Vec4MadShort4(rotDelta, weightScale, quat->u.frame0, rotDelta);
		return;
	}

	XAnim_GetTimeIndexCompressed_unsigned_char_( animTime, quat->u.frames.indices._1, quat->size, &keyFrameIndex, &keyFrameLerpFrac );
	Vec4MadShort4Lerp( rotDelta, weightScale, quat->u.frames.u.frames[keyFrameIndex], quat->u.frames.u.frames[keyFrameIndex + 1], keyFrameLerpFrac, rotDelta );
}

/*
==================
XAnim_CalcSimpleRotDuring
==================
*/
void XAnim_CalcSimpleRotDuring_unsigned_char_( const XAnimTime *animTime, const XAnimPartQuat *quat, float weightScale, vec2_t rotDelta )
{
	float keyFrameLerpFrac;
	int keyFrameIndex;

	if ( !quat->size )
	{
		Vec2MadShort2(rotDelta, weightScale, quat->u.frame0, rotDelta);
		return;
	}

	XAnim_GetTimeIndexCompressed_unsigned_char_( animTime, quat->u.frames.indices._1, quat->size, &keyFrameIndex, &keyFrameLerpFrac );
	Vec2MadShort2Lerp( rotDelta, weightScale, quat->u.frames.u.frames2[keyFrameIndex], quat->u.frames.u.frames2[keyFrameIndex + 1], keyFrameLerpFrac, rotDelta );
}

/*
==================
XAnim_CalcRotDeltaDuringInternal
==================
*/
void XAnim_CalcRotDeltaDuringInternal_unsigned_short_( const XAnimDeltaPartQuat *quat, const float fTime, int frameCount, vec2_t rotDelta )
{
	float keyFrameIndex;
	int keyFrameLerpFrac;
	XAnimTime animTime;

	assert(frameCount && fTime != 1.0f);

	if ( !quat->size )
	{
		Short2CopyAsVec2(quat->u.frame0, rotDelta);
		return;
	}

	XAnim_SetTime( fTime, frameCount, &animTime );

	XAnim_GetTimeIndexCompressed_unsigned_short_( &animTime, quat->u.frames.indices._2, quat->size, &keyFrameLerpFrac, &keyFrameIndex );
	Short2LerpAsVec2( quat->u.frames.frames[keyFrameLerpFrac], quat->u.frames.frames[keyFrameLerpFrac + 1], keyFrameIndex, rotDelta );
}

/*
==================
XAnim_CalcRotDeltaDuringInternal
==================
*/
void XAnim_CalcRotDeltaDuringInternal_unsigned_char_( const XAnimDeltaPartQuat *quat, const float fTime, int frameCount, vec2_t rotDelta )
{
	float keyFrameLerpFrac;
	int keyFrameIndex;
	XAnimTime animTime;

	assert(frameCount && fTime != 1.0f);

	if ( !quat->size )
	{
		Short2CopyAsVec2(quat->u.frame0, rotDelta);
		return;
	}

	XAnim_SetTime( fTime, frameCount, &animTime );

	XAnim_GetTimeIndexCompressed_unsigned_char_( &animTime, quat->u.frames.indices._1, quat->size, &keyFrameIndex, &keyFrameLerpFrac );
	Short2LerpAsVec2( quat->u.frames.frames[keyFrameIndex], quat->u.frames.frames[keyFrameIndex + 1], keyFrameLerpFrac, rotDelta );
}

/*
==================
XAnimCalcParts
==================
*/
void XAnimCalcParts_unsigned_short_( const XAnimParts *parts, const unsigned char *animToModel, float fTime, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits )
{
	XAnimTime animTime;
	char *simpleQuatBits;
	int modelPartIndex;
	float scale;
	XAnimPart *part;
	DObjAnimMat *mat;

	assert(parts);
	assert(parts->numframes);
	assert(fTime >= 0);
	assert(fTime < 1.f);

	int numframes = parts->numframes;
	XAnim_SetTime(fTime, numframes, &animTime);

	scale = weightScale * RANDSCALE;
	simpleQuatBits = parts->simpleQuatBits;

	int boneCount = parts->boneCount;

	for ( int i = 0; i < boneCount; i++ )
	{
		modelPartIndex = animToModel[i];
		assert(modelPartIndex < DOBJ_MAX_PARTS);

		if ( testBit( ignorePartBits, modelPartIndex ) )
		{
			continue;
		}

		mat = &rotTransArray[modelPartIndex];
		part = &parts->parts[i];

		if ( testQuatBit( simpleQuatBits, i ) )
		{
			if ( part->quat )
			{
				XAnim_CalcSimpleRotDuring_unsigned_short_( &animTime, part->quat, scale, &mat->quat[2] );
			}
			else
			{
				mat->quat[3] = mat->quat[3] + weightScale;
			}
		}
		else
		{
			XAnim_CalcRotDuring_unsigned_short_( &animTime, part->quat, scale, mat->quat );
		}

		if ( part->trans )
		{
			XAnim_CalcPosDuring_unsigned_short_( &animTime, part->trans, weightScale, mat->trans );
		}

		mat->transWeight = mat->transWeight + weightScale;
	}
}

/*
==================
XAnimCalcParts
==================
*/
void XAnimCalcParts_unsigned_char_( const XAnimParts *parts, const unsigned char *animToModel, float fTime, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits )
{
	XAnimTime animTime;
	char *simpleQuatBits;
	int modelPartIndex;
	float scale;
	XAnimPart *part;
	DObjAnimMat *mat;

	assert(parts);
	assert(parts->numframes);
	assert(fTime >= 0);
	assert(fTime < 1.f);

	int numframes = parts->numframes;
	XAnim_SetTime(fTime, numframes, &animTime);

	scale = weightScale * RANDSCALE;
	simpleQuatBits = parts->simpleQuatBits;

	int boneCount = parts->boneCount;

	for ( int i = 0; i < boneCount; i++ )
	{
		modelPartIndex = animToModel[i];
		assert(modelPartIndex < DOBJ_MAX_PARTS);

		if ( testBit( ignorePartBits, modelPartIndex ) )
		{
			continue;
		}

		mat = &rotTransArray[modelPartIndex];
		part = &parts->parts[i];

		if ( testQuatBit( simpleQuatBits, i ) )
		{
			if ( part->quat )
			{
				XAnim_CalcSimpleRotDuring_unsigned_char_( &animTime, part->quat, scale, &mat->quat[2] );
			}
			else
			{
				mat->quat[3] = mat->quat[3] + weightScale;
			}
		}
		else
		{
			XAnim_CalcRotDuring_unsigned_char_( &animTime, part->quat, scale, mat->quat );
		}

		if ( part->trans )
		{
			XAnim_CalcPosDuring_unsigned_char_( &animTime, part->trans, weightScale, mat->trans );
		}

		mat->transWeight = mat->transWeight + weightScale;
	}
}

/*
==================
XAnim_CalcRotDeltaDuring
==================
*/
void XAnim_CalcRotDeltaDuring_unsigned_short_( XAnimDeltaPart *animDelta, const float fTime, int frameCount, vec2_t rotDelta )
{
	if ( !animDelta->quat )
	{
		rotDelta[0] = 0;
		rotDelta[1] = SHRT_MAX;
		return;
	}

	XAnim_CalcRotDeltaDuringInternal_unsigned_short_( animDelta->quat, fTime, frameCount, rotDelta );
}

/*
==================
XAnim_CalcRotDeltaDuring
==================
*/
void XAnim_CalcRotDeltaDuring_unsigned_char_( XAnimDeltaPart *animDelta, const float fTime, int frameCount, vec2_t rotDelta )
{
	if ( !animDelta->quat )
	{
		rotDelta[0] = 0;
		rotDelta[1] = SHRT_MAX;
		return;
	}

	XAnim_CalcRotDeltaDuringInternal_unsigned_char_( animDelta->quat, fTime, frameCount, rotDelta );
}

/*
==================
XAnimCloneAnimInfo
==================
*/
void XAnimCloneAnimInfo( const XAnimInfo *from, XAnimInfo *to )
{
	*to = *from;

	if ( to->notifyName )
	{
		SL_AddRefToString( to->notifyName );
	}
}

/*
==================
XAnimRestart
==================
*/
void XAnimRestart( XAnimTree_s *tree, unsigned int infoIndex )
{
	XAnimEntry *anim;

	assert(tree);
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	for ( int parentInfoIndex = infoIndex; parentInfoIndex; parentInfoIndex = tree->anims->entries[parentInfoIndex].parent )
	{
		assert((unsigned)parentInfoIndex < MAX_XANIM_INFO);
		anim = &tree->anims->entries[parentInfoIndex];

		if ( !anim->numAnims )
		{
			continue;
		}

		if ( !( tree->anims->entries[parentInfoIndex].u.animParent.flags & XANIM_SYNC_ROOT ) )
		{
			continue;
		}

		if ( !XAnimInitTime( &g_xAnimInfo[ tree->infoArray[parentInfoIndex] ] ) )
		{
			return;
		}

		int numAnims = anim->numAnims;
		assert(numAnims);

		for ( int i = 0; i < numAnims; i++ )
		{
			XAnimResetTime(tree, i + anim->u.animParent.children);
		}

		return;
	}

	if ( !tree->anims->entries[infoIndex].numAnims )
	{
		XAnimInitTime( &g_xAnimInfo[ tree->infoArray[infoIndex]] );
	}
}

/*
==================
XAnimIsDefault
==================
*/
bool XAnimIsDefault( XAnimParts *parts )
{
	assert(parts);
	return parts->isDefault;
}

/*
==================
XAnimGetServerNotifyFracSyncTotal
==================
*/
float XAnimGetServerNotifyFracSyncTotal( const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry, const XAnimState *state, const XAnimState *nextState, float dtime )
{
	int infoIndex, childInfoIndex;
	float frac, totalFrac;
	XAnimInfo *notifyInfo;

	assert(info);
	assert(dtime);
	assert(info->state.goalWeight);

	frac = XAnimGetNotifyFracServer(tree, info, entry, state, nextState, dtime);

	for ( int i = 0; i < entry->numAnims; i++ )
	{
		childInfoIndex = i + entry->u.animParent.children;
		infoIndex = tree->infoArray[childInfoIndex];
		assert((unsigned)infoIndex < MAX_XANIM_INFO);

		if ( !infoIndex )
		{
			continue;
		}

		notifyInfo = &g_xAnimInfo[infoIndex];

		if ( notifyInfo->state.weight == 0.0f )
		{
			continue;
		}

		if ( notifyInfo->state.goalWeight == 0.0f )
		{
			continue;
		}

		totalFrac = XAnimGetServerNotifyFracSyncTotal(tree, notifyInfo, &tree->anims->entries[childInfoIndex], state, nextState, dtime);

		if ( frac > totalFrac )
		{
			frac = totalFrac;
		}
	}

	return frac;
}

/*
==================
XAnimAddClientNotify
==================
*/
void XAnimAddClientNotify( const XAnimEntry *entry, unsigned int notetrackName, float frac, unsigned int notifyType )
{
	XAnimNotify_s *current, *next;
	int i;

	for ( i = g_notifyListSize - 1; i >= 0; i-- )
	{
		current = &g_notifyList[i];
		next = &g_notifyList[i + 1];

		if ( frac >= current->timeFrac )
		{
			break;
		}

		*next = *current;
	}

	next = &g_notifyList[i + 1];

	next->name = SL_ConvertToString(notetrackName);
	next->timeFrac = frac;
	next->type = notifyType;

	g_notifyListSize++;
}

/*
==================
XAnim_CalcPosDeltaEntire
==================
*/
void XAnim_CalcPosDeltaEntire( const XAnimDeltaPart *animDelta, vec3_t posDelta )
{
	if ( !animDelta->trans )
	{
		VectorClear(posDelta);
		return;
	}

	XAnim_CalcPosDeltaEntireInternal( animDelta->trans, posDelta );
}

/*
==================
XAnim_CalcRotDeltaEntire
==================
*/
void XAnim_CalcRotDeltaEntire( const XAnimDeltaPart *animDelta, vec2_t rotDelta )
{
	if ( !animDelta->quat )
	{
		rotDelta[0] = 0;
		rotDelta[1] = SHRT_MAX;
		return;
	}

	XAnim_CalcRotDeltaEntireInternal( animDelta->quat, rotDelta );
}

/*
==================
XAnimCalcNonLoopEnd
==================
*/
void XAnimCalcNonLoopEnd( const XAnimParts_s *parts, const unsigned char *animToModel, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits )
{
	char *simpleQuatBits;
	int modelPartIndex;
	float scale;
	XAnimPart *part;
	DObjAnimMat *mat;

	assert(!parts->bLoop);

	scale = weightScale * RANDSCALE;
	simpleQuatBits = parts->simpleQuatBits;

	int boneCount = parts->boneCount;

	for ( int i = 0; i < boneCount; i++ )
	{
		modelPartIndex = animToModel[i];
		assert(modelPartIndex < DOBJ_MAX_PARTS);

		if ( testBit( ignorePartBits, modelPartIndex ) )
		{
			continue;
		}

		mat = &rotTransArray[modelPartIndex];
		part = &parts->parts[i];

		if ( testQuatBit( simpleQuatBits, i ) )
		{
			if ( part->quat )
			{
				XAnim_CalcSimpleRotEnd(part->quat, scale, &mat->quat[2]);
			}
			else
			{
				mat->quat[3] = mat->quat[3] + weightScale;
			}
		}
		else
		{
			XAnim_CalcRotEnd(part->quat, scale, mat->quat);
		}

		if ( part->trans )
		{
			XAnim_CalcPosEnd(part->trans, weightScale, mat->trans);
		}

		mat->transWeight = mat->transWeight + weightScale;
	}
}

/*
==================
XAnimFindServerNoteTrack
==================
*/
float XAnimFindServerNoteTrack( const XAnimTree_s *tree, unsigned int infoIndex, float dtime )
{
	XAnimState *state, nextState;
	float frac, totalFrac, fTime;
	int cycleCount, numAnims, i, nextInfoIndex;
	XAnimParts *parts;
	XAnimEntry *anim;
	XAnimInfo *info;

	assert(tree);
	assert(dtime >= 0);
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	nextInfoIndex = tree->infoArray[infoIndex];
	assert((unsigned)nextInfoIndex < MAX_XANIM_INFO);

	if ( !nextInfoIndex )
	{
		return 1.0;
	}

	info = &g_xAnimInfo[nextInfoIndex];
	assert(info);

	state = &info->state;
	assert(state);

	if ( info->state.weight == 0.0f || state->goalWeight == 0.0f )
	{
		return 1.0f;
	}

	anim = &tree->anims->entries[infoIndex];
	assert(anim);

	numAnims = anim->numAnims;

	if ( numAnims )
	{
		if ( anim->u.animParent.flags & ( XANIM_LOOP_SYNC_TIME | XANIM_NONLOOP_SYNC_TIME ) )
		{
			dtime *= XAnimGetAverageRateFrequency(tree, infoIndex) * state->rate;

			if ( dtime == 0.0f )
			{
				return 1.0f;
			}

			fTime = state->oldTime + dtime;
			cycleCount = state->oldCycleCount;

			if ( anim->u.animParent.flags & XANIM_NONLOOP_SYNC_TIME )
			{
				if ( fTime >= 1.0f )
				{
					fTime = 1.0f;
				}
			}
			else
			{
				while ( fTime >= 1.0f )
				{
					fTime -= 1.0f;
					cycleCount++;
				}
			}

			if ( state->time - fTime > cycleCount - state->cycleCount )
			{
				return 1.0f;
			}

			nextState.oldTime = state->time;
			nextState.oldCycleCount = state->cycleCount;

			nextState.time = fTime;
			nextState.cycleCount = cycleCount;

			return XAnimGetServerNotifyFracSyncTotal(tree, info, anim, state, &nextState, dtime);

		}
		else
		{
			dtime *= state->rate;

			if ( dtime == 0.0f )
			{
				return 1.0f;
			}

			frac = 1.0f;

			for ( i = 0; i < numAnims; i++ )
			{
				totalFrac = XAnimFindServerNoteTrack(tree, i + anim->u.animParent.children, dtime);

				if ( frac > totalFrac )
				{
					frac = totalFrac;
				}
			}

			return frac;
		}
	}
	else
	{
		parts = anim->u.parts;
		assert(parts);

		dtime *= state->rate * parts->frequency;

		if ( dtime == 0.0f )
		{
			return 1.0f;
		}

		fTime = state->oldTime + dtime;
		cycleCount = state->oldCycleCount;

		if ( parts->bLoop )
		{
			while ( fTime >= 1.0f )
			{
				fTime -= 1.0f;
				cycleCount++;
			}
		}
		else if ( fTime >= 1.0f )
		{
			fTime = 1.0f;
		}

		if ( state->time - fTime > cycleCount - state->cycleCount )
		{
			return 1.0f;
		}

		nextState.oldTime = state->time;
		nextState.oldCycleCount = state->cycleCount;

		nextState.time = fTime;
		nextState.cycleCount = cycleCount;

		return XAnimGetNotifyFracServer(tree, info, anim, state, &nextState, dtime);
	}
}

/*
==================
XAnimProcessClientNotify
==================
*/
void XAnimProcessClientNotify( XAnimInfo *info, const XAnimEntry *entry, float dtime )
{
	unsigned int notifyType, notifyIndex;
	const XAnimState *state;
	XAnimNotifyInfo *notifyInfo;
	XAnimParts *parts;

	state = &info->state;
	assert(state);
	assert(dtime);
	assert(state->goalWeight);

	notifyType = info->notifyType;

	if ( !notifyType )
	{
		return;
	}

	if ( info->state.oldTime == 1.0f )
	{
		XAnimAddClientNotify(entry, g_end, XAnimGetNotifyFracLeaf(state, state, 1.0f, dtime), notifyType);
		return;
	}

	if ( entry->numAnims )
	{
		if ( info->state.oldTime > info->state.time || state->time == 1.0f )
		{
			XAnimAddClientNotify(entry, g_end, XAnimGetNotifyFracLeaf(state, state, 1.0f, dtime), notifyType);
		}

		return;
	}

	parts = entry->u.parts;
	assert(parts);

	notifyIndex = XAnimGetNextNotifyTime(entry, info->state.oldTime);
	assert(notifyIndex < parts->notifyCount);

	notifyInfo = &parts->notify[notifyIndex];

	if ( info->state.time < info->state.oldTime )
	{
		if ( notifyInfo->time < state->time )
		{
			do
			{
				XAnimAddClientNotify(entry, notifyInfo->name, XAnimGetNotifyFracLeaf(state, state, notifyInfo->time, dtime), notifyType);

				notifyInfo++;
				notifyIndex++;
			}
			while ( notifyIndex < parts->notifyCount && state->time > notifyInfo->time );
		}
		else if ( notifyInfo->time >= info->state.oldTime )
		{
			do
			{
				assert(state->oldTime <= notifyInfo->time);
				XAnimAddClientNotify(entry, notifyInfo->name, XAnimGetNotifyFracLeaf(state, state, notifyInfo->time, dtime), notifyType);

				notifyInfo++;
				notifyIndex++;
			}
			while ( notifyIndex < parts->notifyCount );

			for ( notifyInfo = parts->notify; state->time > notifyInfo->time; notifyInfo++ )
			{
				XAnimAddClientNotify(entry, notifyInfo->name, XAnimGetNotifyFracLeaf(state, state, notifyInfo->time, dtime), notifyType);
				assert(notifyIndex < parts->notifyCount);
			}
		}
	}
	else if ( state->time == 1.0f )
	{
		assert(!parts->bLoop);

		if ( notifyInfo->time >= info->state.oldTime )
		{
			do
			{
				assert(state->oldTime <= notifyInfo->time);
				XAnimAddClientNotify(entry, notifyInfo->name, XAnimGetNotifyFracLeaf(state, state, notifyInfo->time, dtime), notifyType);

				notifyInfo++;
				notifyIndex++;
			}
			while ( notifyIndex < parts->notifyCount );
		}
	}
	else if ( state->time > notifyInfo->time && notifyInfo->time >= info->state.oldTime )
	{
		do
		{
			assert(state->oldTime <= notifyInfo->time);
			XAnimAddClientNotify(entry, notifyInfo->name, XAnimGetNotifyFracLeaf(state, state, notifyInfo->time, dtime), notifyType);

			notifyInfo++;
			notifyIndex++;
		}
		while ( notifyIndex < parts->notifyCount && state->time > notifyInfo->time );
	}
}

/*
==================
XAnim_CalcDeltaForTime
==================
*/
void XAnim_CalcDeltaForTime( const XAnimParts *parts, float fTime, vec2_t rotDelta, vec3_t posDelta )
{
	assert(fTime >= 0.0f || fTime <= 1.0f);

	XAnimDeltaPart *animDelta = parts->deltaPart;
	int frameCount = parts->numframes;

	if ( fTime != 1.0f && frameCount )
	{
		if ( frameCount <= UCHAR_MAX )
		{
			XAnim_CalcRotDeltaDuring_unsigned_char_(animDelta, fTime, frameCount, rotDelta);
			XAnim_CalcPosDeltaDuring_unsigned_char_(animDelta, fTime, frameCount, posDelta);
		}
		else
		{
			XAnim_CalcRotDeltaDuring_unsigned_short_(animDelta, fTime, frameCount, rotDelta);
			XAnim_CalcPosDeltaDuring_unsigned_short_(animDelta, fTime, frameCount, posDelta);
		}
	}
	else
	{
		XAnim_CalcRotDeltaEntire(animDelta, rotDelta);
		XAnim_CalcPosDeltaEntire(animDelta, posDelta);
	}
}

/*
==================
XAnimCalcAbsDeltaParts
==================
*/
void XAnimCalcAbsDeltaParts( const XAnimParts *parts, float weightScale, float fTime, XAnimSimpleRotPos *rotPos )
{
	vec2_t Q;
	vec3_t pos;

	XAnim_CalcDeltaForTime(parts, fTime, Q, pos);
	VectorMA2(rotPos->rot, weightScale * RANDSCALE, Q, rotPos->rot);

	rotPos->posWeight += weightScale;
	VectorMA(rotPos->pos, weightScale, pos, rotPos->pos);
}

/*
==================
XAnimCalcRelDeltaParts
==================
*/
void XAnimCalcRelDeltaParts( const XAnimParts *parts, float weightScale, float time1, float time2, XAnimSimpleRotPos *rotPos, int quatIndex )
{
	XAnimDeltaPartTrans *trans;
	vec3_t vec1, vec2, pos;
	vec2_t Q[2];

	XAnim_CalcDeltaForTime(parts, time1, Q[0], vec1);
	XAnim_CalcDeltaForTime(parts, time2, Q[1], vec2);

	if ( parts->bLoop )
	{
		if ( time1 > time2 )
		{
			trans = parts->deltaPart->trans;

			if ( trans )
			{
				if ( trans->size )
				{
					VectorAdd(vec2, trans->u.frames.frames[trans->size], vec2);
					VectorSubtract(vec2, trans->u.frames.frames[0], vec2);
				}
			}
		}
	}

	rotPos->rot[0] = ( Q[1][0] * Q[0][1] - Q[1][1] * Q[0][0] ) * weightScale * WEIGHTSCALE_EPSILON + rotPos->rot[0];
	rotPos->rot[1] = ( Q[1][0] * Q[0][0] + Q[1][1] * Q[0][1] ) * weightScale * WEIGHTSCALE_EPSILON + rotPos->rot[1];

	VectorSubtract(vec2, vec1, pos);
	TransformToQuatRefFrame(Q[quatIndex], pos);

	rotPos->posWeight += weightScale;
	VectorMA(rotPos->pos, weightScale, pos, rotPos->pos);
}

/*
==================
XAnimCalcDeltaTree
==================
*/
void XAnimCalcDeltaTree( const XAnimTree_s *tree, unsigned int infoIndex, float weightScale, bool bClear, bool bNormQuat, XAnimSimpleRotPos *rotPos )
{
	float firstWeight, weight, r;
	XAnimParts *parts;
	XAnimEntry *entry;
	int childIndex, infoIndex1, infoIndex2, numAnims;
	XAnimSimpleRotPos newRotPos;

	assert((unsigned)infoIndex < MAX_XANIM_INFO);
	entry = &tree->anims->entries[infoIndex];
	assert(entry);

	numAnims = entry->numAnims;

	if ( numAnims )
	{
		for ( infoIndex1 = 0; ; infoIndex1++ )
		{
			if ( infoIndex1 >= numAnims )
			{
				if ( bClear )
				{
					Vector2Clear(rotPos->rot);
					rotPos->posWeight = 0.0f;
					VectorClear(rotPos->pos);
				}

				return;
			}

			assert((unsigned)infoIndex1 < MAX_XANIM_INFO);
			childIndex = tree->infoArray[infoIndex1 + entry->u.animParent.children];

			if ( childIndex )
			{
				firstWeight = tree->bUseGoalWeight ? g_xAnimInfo[childIndex].state.goalWeight : g_xAnimInfo[childIndex].state.weight;
				assert(firstWeight >= 0.0f);

				if ( firstWeight != 0.0f )
				{
					break;
				}
			}
		}

		for ( infoIndex2 = infoIndex1 + 1; ; infoIndex2++ )
		{
			if ( infoIndex2 >= numAnims )
			{
				XAnimCalcDeltaTree( tree, infoIndex1 + entry->u.animParent.children, weightScale, bClear, bNormQuat, rotPos );
				return;
			}

			assert((unsigned)infoIndex2 < MAX_XANIM_INFO);
			childIndex = tree->infoArray[infoIndex2 + entry->u.animParent.children];

			if ( childIndex )
			{
				weight = tree->bUseGoalWeight ? g_xAnimInfo[childIndex].state.goalWeight : g_xAnimInfo[childIndex].state.weight;
				assert(weight >= 0);

				if ( weight != 0.0f )
				{
					break;
				}
			}
		}

		if ( !bClear )
		{
			rotPos = &newRotPos;
		}

		XAnimCalcDeltaTree( tree, infoIndex1 + entry->u.animParent.children, firstWeight, true, true, rotPos );
		XAnimCalcDeltaTree( tree, infoIndex2 + entry->u.animParent.children, weight, false, true, rotPos );

		for ( infoIndex2 = infoIndex2 + 1; infoIndex2 < numAnims; infoIndex2++ )
		{
			childIndex = tree->infoArray[infoIndex2 + entry->u.animParent.children];

			if ( childIndex )
			{
				weight = tree->bUseGoalWeight ? g_xAnimInfo[childIndex].state.goalWeight : g_xAnimInfo[childIndex].state.weight;
				assert(weight >= 0);

				if ( weight != 0.0f )
				{
					XAnimCalcDeltaTree( tree, infoIndex2 + entry->u.animParent.children, weight, false, true, rotPos );
				}
			}
		}

		if ( bNormQuat )
		{
			if ( bClear )
			{
				r = Vec2LengthSq(rotPos->rot);

				if ( r != 0.0f )
				{
					Vec2Scale(rotPos->rot, I_rsqrt(r) * weightScale, rotPos->rot);
				}

				if ( rotPos->posWeight != 0.0f )
				{
					VectorScale(rotPos->pos, weightScale / rotPos->posWeight, rotPos->pos);
					rotPos->posWeight = weightScale;
				}
			}
			else
			{
				r = Vec2LengthSq(rotPos->rot);

				if ( r != 0.0f )
				{
					VectorMA2(rotPos->rot, I_rsqrt(r) * weightScale, rotPos->rot, rotPos->rot);
				}

				if ( rotPos->posWeight != 0.0f )
				{
					VectorMA(rotPos->pos, weightScale / rotPos->posWeight, rotPos->pos, rotPos->pos);
					rotPos->posWeight += weightScale;
				}
			}
		}
		else if ( rotPos->posWeight != 0.0f )
		{
			VectorScale(rotPos->pos, 1.0f / rotPos->posWeight, rotPos->pos);
		}
	}
	else
	{
		if ( bClear )
		{
			Vector2Clear(rotPos->rot);
			rotPos->posWeight = 0.0f;
			VectorClear(rotPos->pos);
		}

		parts = entry->u.parts;
		assert(parts);

		if ( parts->bDelta )
		{
			childIndex = tree->infoArray[infoIndex];

			if ( childIndex )
			{
				if ( tree->bAbs )
				{
					XAnimCalcAbsDeltaParts( parts, weightScale, g_xAnimInfo[childIndex].state.time, rotPos );
				}
				else
				{
					XAnimCalcRelDeltaParts( parts, weightScale, g_xAnimInfo[childIndex].state.oldTime, g_xAnimInfo[childIndex].state.time, rotPos, 1 );
				}
			}
		}
	}
}

/*
==================
XAnimSetModel
==================
*/
unsigned int XAnimSetModel( const XAnimEntry *animEntry, XModel *const *models, int numModels )
{
	XAnimParts_s *parts;
	XModelParts_s *modelParts;
	unsigned short *partNames, *modelPartNames;
	int numBones, boneCount, boneIter, boneIndex, partCount, partIndex, i;
	XAnimToXModel animToModel;

	parts = animEntry->u.parts;
	assert(parts);

	partNames = parts->names;
	boneCount = parts->boneCount;

	for ( i = 0; i < DOBJ_MAX_PART_BITS; i++ )
	{
		animToModel.partBits[i] = 0;
	}

	for ( i = boneCount - 1; i >= 0; i-- )
	{
		animToModel.boneIndex[i] = UNDEFINED_BONEINDEX;
	}

	for ( boneIndex = 0, partIndex = 0; partIndex < numModels; partIndex++ )
	{
		modelParts = models[partIndex]->parts;
		assert(modelParts);

		modelPartNames = modelParts->hierarchy->names;
		numBones = modelParts->numBones;

		for ( boneIter = 0; boneIter < numBones; boneIter++, boneIndex++ )
		{
			for ( partCount = boneCount - 1; partCount >= 0; partCount-- )
			{
				if ( modelPartNames[boneIter] == partNames[partCount] )
				{
					if ( animToModel.boneIndex[partCount] == UNDEFINED_BONEINDEX )
					{
						animToModel.boneIndex[partCount] = boneIndex;
						setBit( animToModel.partBits, boneIndex );
					}

					break;
				}
			}
		}
	}

	return SL_GetStringOfLen( (const char *)&animToModel.partBits, 0, boneCount + DOBJ_PARTBITS_STRING_LEN );
}

/*
==================
XAnimClearServerNotify
==================
*/
void XAnimClearServerNotify( XAnimInfo *info )
{
	if ( info->notifyName )
	{
		SL_RemoveRefToString( info->notifyName );
		info->notifyName = 0;
	}

	info->notifyIndex = -1;
}

/*
==================
XAnimFreeInfo
==================
*/
void XAnimFreeInfo( XAnimTree_s *tree, unsigned int infoIndex )
{
	XAnimInfo *info;

	assert(tree);
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	info = &g_xAnimInfo[infoIndex];
	XAnimClearServerNotify(info);

	info->prev = 0;
	info->next = g_xAnimInfo->next;

	g_xAnimInfo[g_xAnimInfo->next].prev = infoIndex;
	g_xAnimInfo->next = infoIndex;
}

/*
==================
XAnimFindData
==================
*/
XAnimParts* XAnimFindData( const char *name )
{
	return (XAnimParts *)Hunk_FindDataForFile( FILEDATA_XANIM, name );
}

/*
==================
XAnimSetData
==================
*/
const char* XAnimSetData( const char *modelName, XAnimParts *parts, Alloc_t Alloc )
{
	return Hunk_SetDataForFile( FILEDATA_XANIM, modelName, parts, Alloc );
}

/*
==================
XAnimClearTreeWeights
==================
*/
void XAnimClearTreeWeights( XAnimTree *tree, unsigned int animIndex )
{
	assert(tree);

	unsigned int infoIndex = tree->infoArray[animIndex];
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	if ( !infoIndex )
	{
		return;
	}

	XAnimEntry *anim = &tree->anims->entries[animIndex];
	assert(anim);

	int numAnims = anim->numAnims;

	for ( int i = 0; i < numAnims; i++ )
	{
		XAnimClearTreeWeights( tree, i + anim->u.animParent.children );
	}

	XAnimFreeInfo( tree, infoIndex );
	tree->infoArray[animIndex] = 0;
}

/*
==================
XAnimClearGoalWeightKnobInternal
==================
*/
void XAnimClearGoalWeightKnobInternal( XAnimTree_s *tree, unsigned int infoIndex, float goalWeight, float goalTime )
{
	float weight;
	unsigned int childIndex;
	float largestWeightDiff;
	XAnimEntry *anim;
	int i, numAnims;

	assert(tree);
	assert(tree->anims);
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	if ( !infoIndex )
	{
		return;
	}

	anim = &tree->anims->entries[tree->anims->entries[infoIndex].parent];
	assert(anim);

	largestWeightDiff = 0.0f;
	numAnims = anim->numAnims;

	for ( i = 0; i < numAnims; i++ )
	{
		childIndex = tree->infoArray[i + anim->u.animParent.children];
		assert((unsigned)childIndex < MAX_XANIM_INFO);

		if ( childIndex )
			weight = g_xAnimInfo[childIndex].state.weight;
		else
			weight = 0.0f;

		if ( i + anim->u.animParent.children == infoIndex )
		{
			weight = I_fabs( goalWeight - weight );
		}

		if ( weight > largestWeightDiff )
		{
			largestWeightDiff = weight;
		}
	}

	for ( i = 0; i < numAnims; i++ )
	{
		if ( i + anim->u.animParent.children != infoIndex )
		{
			XAnimClearGoalWeight( tree, i + anim->u.animParent.children, largestWeightDiff * goalTime );
		}
	}
}

/*
==================
XAnimUpdateOldTime
==================
*/
void XAnimUpdateOldTime( XAnimTree *tree, unsigned int infoIndex, XAnimState *syncState, float dtime, bool parentHasWeight, bool *parentHasWeightForChild, bool *childHasTimeForParent )
{
	XAnimState *state;
	XAnimInfo *info;
	int childIndex, numAnims, i;
	XAnimTimeInfo childInfo, parentInfo;

	assert(tree);
	assert(tree->anims);
	assert(dtime >= 0);
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	parentInfo.parentHasWeightForChild = parentHasWeight;

	childIndex = tree->infoArray[infoIndex];
	assert((unsigned)childIndex < MAX_XANIM_INFO);

	if ( !childIndex )
	{
		return;
	}

	info = &g_xAnimInfo[childIndex];
	assert(info);

	state = &info->state;
	assert(state);

	childInfo.parentHasWeightForChild = parentInfo.parentHasWeightForChild && state->weight != 0.0f;

	if ( parentInfo.parentHasWeightForChild && dtime < state->goalTime )
	{
		state->weight = (state->goalWeight - state->weight) * dtime / state->goalTime + state->weight;

		if ( state->weight < 0.0000010000001f )
		{
			state->weight = state->goalWeight * WEIGHT_EPSILON;
		}

		state->goalTime = state->goalTime - dtime;
	}
	else
	{
		state->weight = state->goalWeight;
		state->goalTime = 0.0f;
	}

	parentInfo.childHasTimeForParent = childInfo.parentHasWeightForChild || state->goalWeight != 0.0f;

	childInfo.entry = &tree->anims->entries[infoIndex];
	assert(childInfo.entry);

	numAnims = childInfo.entry->numAnims;

	if ( numAnims )
	{
		childInfo.childHasTimeForParent = false;

		if ( childInfo.entry->u.animParent.flags & XANIM_SYNC_ROOT )
		{
			syncState = state;
		}

		for ( i = 0; i < numAnims; i++ )
		{
			XAnimUpdateOldTime( tree, i + childInfo.entry->u.animParent.children, syncState, dtime, childInfo.parentHasWeightForChild, &parentInfo.childHasTimeForParent, &childInfo.childHasTimeForParent );
		}
	}
	else
	{
		childInfo.childHasTimeForParent = childInfo.entry->u.parts->frequency != 0.0f;
	}

	if ( parentInfo.childHasTimeForParent )
	{
		if ( childInfo.parentHasWeightForChild && childInfo.childHasTimeForParent )
		{
			*childHasTimeForParent = true;
		}
		else if ( numAnims && childInfo.entry->u.animParent.flags & XANIM_SYNC_ROOT )
		{
			if ( XAnimInitTime(info) )
			{
				for ( i = 0; i < numAnims; i++ )
				{
					XAnimResetTime(tree, i + childInfo.entry->u.animParent.children);
				}
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

		*parentHasWeightForChild = true;
	}
	else
	{
		XAnimFreeInfo(tree, childIndex);
		tree->infoArray[infoIndex] = 0;
	}
}

/*
==================
XAnimFillInSyncNodes_r
==================
*/
void XAnimFillInSyncNodes_r( XAnim_s *anims, unsigned int animIndex, bool bLoop )
{
	XAnimEntry *anim;
	int numAnims, i, count;

	assert(anims);

	anim = &anims->entries[animIndex];
	assert(anim);

	numAnims = anim->numAnims;

	if ( numAnims )
	{
		if ( anim->u.animParent.flags & ( XANIM_LOOP_SYNC_TIME | XANIM_NONLOOP_SYNC_TIME ) )
		{
			count = 0;

			do
			{
				count++;
				anim = &anims->entries[anim->u.animParent.children];
			}
			while ( anim->numAnims );

			Com_Error( ERR_DROP, "duplicate specification of animation sync in '%s', %d nodes above '%s'", anims->debugName, count, XAnimGetAnimDebugName( anims, animIndex ) );
		}

		anim->u.animParent.flags = bLoop ? anim->u.animParent.flags | XANIM_LOOP_SYNC_TIME : anim->u.animParent.flags | XANIM_NONLOOP_SYNC_TIME;

		for ( i = 0; i < numAnims; i++ )
		{
			XAnimFillInSyncNodes_r( anims, i + anims->entries[animIndex].u.animParent.children, bLoop );
		}

		return;
	}

	if ( anims->entries[animIndex].u.parts->bLoop == bLoop )
	{
		return;
	}

	if ( !XAnimIsDefault( anims->entries[animIndex].u.parts ) )
	{
		if ( bLoop )
		{
			Com_Error( ERR_DROP, "animation '%s' in '%s' cannot be sync looping and nonlooping", XAnimGetAnimDebugName( anims, animIndex ), anims->debugName );
		}

		Com_Error( ERR_DROP, "animation '%s' in '%s' cannot be sync nonlooping and looping", XAnimGetAnimDebugName( anims, animIndex ), anims->debugName );
	}

	XAnimPrecache( "void_loop", Hunk_AllocXAnimPrecache );

	if ( !XAnimFindData( "void_loop" ) )
	{
		Com_Error( ERR_DROP, "Cannot find 'xanim/%s'.\nThis is a default xanim file that you should have.\n", "void_loop" );
	}
}

/*
==================
XAnimTransfer
==================
*/
void XAnimTransfer( XAnimTree_s *from, XAnimTree_s *to )
{
	XAnimInfo *info;
	int infoIndex, size, i;

	size = from->anims->size;

	for ( i = 0; i < size; i++ )
	{
		infoIndex = from->infoArray[i];
		assert((unsigned)infoIndex < MAX_XANIM_INFO);

		if ( from->infoArray[i] )
		{
			if ( to->infoArray[i] )
			{
				info = &g_xAnimInfo[to->infoArray[i]];
			}
			else
			{
				info = XAnimAllocInfo(to, i);
				XAnimInitInfo(info);
			}

			info->state.time = g_xAnimInfo[infoIndex].state.oldTime;
			info->state.cycleCount = g_xAnimInfo[infoIndex].state.oldCycleCount;
			info->state.goalWeight = g_xAnimInfo[infoIndex].state.goalWeight;
			info->state.rate = g_xAnimInfo[infoIndex].state.rate;
			info->state.goalTime = g_xAnimInfo[infoIndex].state.goalTime;

			continue;
		}

		if ( to->infoArray[i] )
		{
			XAnimFreeInfo(to, to->infoArray[i]);
			to->infoArray[i] = 0;
		}
	}
}

/*
==================
XAnimSetupSyncNodes_r
==================
*/
void XAnimSetupSyncNodes_r( XAnim_s *anims, unsigned int animIndex )
{
	int parFlags, numAnims, i;
	XAnimEntry *anim;

	anim = &anims->entries[animIndex];
	assert(anim);

	numAnims = anim->numAnims;

	if ( !numAnims )
	{
		return;
	}

	parFlags = anim->u.animParent.flags & ( XANIM_LOOP_SYNC_TIME | XANIM_NONLOOP_SYNC_TIME );

	if ( parFlags )
	{
		if ( parFlags == ( XANIM_LOOP_SYNC_TIME | XANIM_NONLOOP_SYNC_TIME ) )
		{
			Com_Error( ERR_DROP, "animation cannot be sync looping and sync nonlooping" );
		}

		anim->u.animParent.flags |= XANIM_SYNC_ROOT;

		for ( i = 0; i < numAnims; i++ )
		{
			XAnimFillInSyncNodes_r( anims, i + anim->u.animParent.children, parFlags == XANIM_LOOP_SYNC_TIME );
		}
	}
	else
	{
		for ( i = 0; i < numAnims; i++ )
		{
			XAnimSetupSyncNodes_r( anims, i + anim->u.animParent.children );
		}
	}
}

/*
==================
XAnimSetGoalWeightInternal
==================
*/
int XAnimSetGoalWeightInternal( XAnimTree *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, bool bBypass, unsigned int notifyName, unsigned int notifyType )
{
	XAnimInfo *info;

	assert(tree);
	assert(tree->anims);
	assert(!goalWeight || goalWeight >= WEIGHT_EPSILON);
	assert(goalTime >= 0);
	assert(goalTime >= 0);
	assert((!notifyName && !notifyType) || goalWeight);

	if ( tree->infoArray[animIndex] )
	{
		info = &g_xAnimInfo[tree->infoArray[animIndex]];
		XAnimClearServerNotify(info);
	}
	else
	{
		if ( goalWeight == 0.0f && !bBypass )
		{
			return 0;
		}

		info = XAnimAllocInfo(tree, animIndex);
		XAnimInitInfo(info);
	}

	assert(animIndex < tree->anims->size);

	if ( !animIndex )
	{
		goalWeight = 1.0f;
		goalTime = 0.0f;
		rate = 1.0f;
	}

	if ( goalTime == 0.0f )
	{
		info->state.weight = goalWeight;
	}
	else if ( info->state.weight == 0.0f )
	{
		info->state.weight = goalWeight * WEIGHT_EPSILON;
	}

	if ( goalWeight == 0.0f )
	{
		if ( info->state.goalWeight == 0.0f )
		{
			if ( info->state.goalTime > goalTime )
			{
				info->state.goalTime = goalTime;
			}
		}
		else
		{
			info->state.goalTime = goalTime;
		}
	}
	else
	{
		if ( goalWeight < info->state.weight )
		{
			info->state.goalTime = ( info->state.weight - goalWeight ) * goalTime / info->state.weight;
		}
		else
		{
			info->state.goalTime = ( goalWeight - info->state.weight ) * goalTime / goalWeight;
		}
	}

	info->state.goalWeight = goalWeight;
	info->state.rate = rate;

	info->notifyName = notifyName;

	if ( notifyName )
	{
		SL_AddRefToString(notifyName);
	}

	assert(info->notifyIndex == -1);

	if ( notifyName && tree->anims->entries[animIndex].numAnims && tree->anims->entries[animIndex].u.animParent.flags & ( XANIM_LOOP_SYNC_TIME | XANIM_NONLOOP_SYNC_TIME ) )
	{
		assert(goalWeight);
		info->notifyChild = XAnimGetDescendantWithGreatestWeight( tree, animIndex );

		if ( !info->notifyChild )
		{
			return 2;
		}
	}
	else
	{
		info->notifyChild = 0;
	}

	info->notifyType = notifyType;
	return 0;
}

/*
==================
XAnimEnsureGoalWeightParent
==================
*/
void XAnimEnsureGoalWeightParent( XAnimTree *tree, unsigned int animIndex, float goalTime )
{
	assert(tree);

	while ( animIndex )
	{
		animIndex = tree->anims->entries[animIndex].parent;

		if ( tree->infoArray[animIndex] )
		{
			break;
		}

		XAnimSetGoalWeightInternal( tree, animIndex, 0.0f, goalTime, 1.0f, true, 0, 0 );
	}
}

/*
==================
XAnimHasTime
==================
*/
bool XAnimHasTime( const XAnim_s *anims, unsigned int animIndex )
{
	return IsLeafNode( &anims->entries[animIndex] ) || anims->entries[animIndex].u.animParent.flags & ( XANIM_LOOP_SYNC_TIME | XANIM_NONLOOP_SYNC_TIME );
}

/*
==================
XAnimDisplay
==================
*/
void XAnimDisplay( const XAnimTree *tree, unsigned int infoIndex, int depth )
{
	const char *debugName, *color;
	float delta, realtimedelta;
	XAnimParts_s *parts;
	XAnimInfo *info;
	XAnimEntry *anim;
	unsigned int animIndex;
	int numAnims, i;

	assert(tree);
	assert(tree->anims);
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	animIndex = tree->infoArray[infoIndex];
	assert(animIndex < tree->anims->size);

	if ( !animIndex )
	{
		return;
	}

	anim = &tree->anims->entries[infoIndex];
	assert(anim);

	numAnims = anim->numAnims;

	info = &g_xAnimInfo[animIndex];
	assert(info);
	assert(info->state.weight >= 0);

	for ( i = 0; i < depth; i++ )
	{
		Com_Printf(" ");
	}

	debugName = XAnimGetAnimDebugName(tree->anims, infoIndex);

	if ( info->state.weight < info->state.goalWeight )
	{
		color = "^4";
	}
	else if ( info->state.goalWeight < info->state.weight )
	{
		color = "^1";
	}
	else
	{
		color = "";
	}

	if ( numAnims )
	{
		if ( info->notifyName )
		{
			if ( XAnimHasTime(tree->anims, infoIndex) )
			{
				Com_Printf(
				    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f, '%s'\n",
				    color,
				    debugName,
				    info->state.weight,
				    info->state.goalWeight,
				    info->state.oldTime,
				    info->state.time,
				    SL_ConvertToString(info->notifyName));
			}
			else
			{
				Com_Printf(
				    "%s%s: (weight) %.2f -> %.2f, '%s'\n",
				    color,
				    debugName,
				    info->state.weight,
				    info->state.goalWeight,
				    SL_ConvertToString(info->notifyName));
			}
		}
		else if ( XAnimHasTime(tree->anims, infoIndex) )
		{
			Com_Printf(
			    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f\n",
			    color,
			    debugName,
			    info->state.weight,
			    info->state.goalWeight,
			    info->state.oldTime,
			    info->state.time);
		}
		else
		{
			Com_Printf("%s%s: (weight) %.2f -> %.2f\n", color, debugName, info->state.weight, info->state.goalWeight);
		}

		for ( i = 0; i < numAnims; i++ )
		{
			XAnimDisplay(tree, i + anim->u.animParent.children, depth + 1);
		}
	}
	else
	{
		parts = anim->u.parts;
		assert(parts);

		delta = info->state.time - info->state.oldTime;

		if ( delta < 0.0f )
		{
			delta += 1.0f;
		}

		if ( parts->frequency == 0.0f )
		{
			realtimedelta = 0.0f;
		}
		else
		{
			realtimedelta = delta / parts->frequency;
		}

		if ( info->notifyName )
		{
			Com_Printf(
			    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f, (realtimedelta) %.2f, '%s'\n",
			    color,
			    debugName,
			    info->state.weight,
			    info->state.goalWeight,
			    info->state.oldTime,
			    info->state.time,
			    realtimedelta,
			    SL_ConvertToString(info->notifyName));
		}
		else
		{
			Com_Printf(
			    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f, (realtimedelta) %.2f\n",
			    color,
			    debugName,
			    info->state.weight,
			    info->state.goalWeight,
			    info->state.oldTime,
			    info->state.time,
			    realtimedelta);
		}
	}
}

/*
==================
XAnimCalc
==================
*/
void XAnimCalc( const DObj *obj, unsigned int animIndex, float weightScale, DObjAnimMat *rotTransArray, bool bClear, bool bNormQuat, XAnimCalcAnimInfo *animInfo, int bufferIndex )
{
	int firstInfoIndex, secondInfoIndex, thirdInfoIndex, firstWeight, secondWeight, thirdWeight, numAnims, i;
	XAnimParts_s *parts;
	const int *animToModel;
	XAnimTree_s *tree;
	byte *models;
	XAnimEntry *anim;
	unsigned int childIndex;
	float fTime, r;
	DObjAnimMat *calcBuffer;

	assert(obj);
	tree = obj->tree;
	assert(tree);
	assert(tree->anims);

	anim = &obj->tree->anims->entries[animIndex];
	assert(anim);

	numAnims = anim->numAnims;

	if ( numAnims )
	{
		for ( firstInfoIndex = 0; ; firstInfoIndex++ )
		{
			if ( firstInfoIndex >= numAnims )
			{
				if ( bClear )
				{
					XAnimClearData(obj, rotTransArray, animInfo);
				}

				return;
			}

			childIndex = tree->infoArray[firstInfoIndex + anim->u.animParent.children];

			if ( childIndex )
			{
				firstWeight = g_xAnimInfo[childIndex].state.weight;

				if ( firstWeight != 0.0f )
				{
					break;
				}
			}
		}

		for ( secondInfoIndex = firstInfoIndex + 1; ; secondInfoIndex++ )
		{
			if ( secondInfoIndex >= numAnims )
			{
				XAnimCalc( obj, firstInfoIndex + anim->u.animParent.children, weightScale, rotTransArray, bClear, bNormQuat, animInfo, bufferIndex );
				return;
			}

			childIndex = tree->infoArray[secondInfoIndex + anim->u.animParent.children];

			if ( childIndex )
			{
				secondWeight = g_xAnimInfo[childIndex].state.weight;

				if ( secondWeight != 0.0f )
				{
					break;
				}
			}
		}

		if ( bClear )
		{
			calcBuffer = rotTransArray;
		}
		else
		{
			calcBuffer = &animInfo->rotTransArray[bufferIndex];
			bufferIndex += obj->numBones;

			if ( bufferIndex > MAX_CALC_ANIM_BUFFER )
			{
				Com_Printf("MAX_CALC_ANIM_BUFFER exceeded\n");
				return;
			}
		}

		assert(calcBuffer);

		XAnimCalc( obj, firstInfoIndex + anim->u.animParent.children, firstWeight, calcBuffer, true, true, animInfo, bufferIndex );
		XAnimCalc( obj, secondInfoIndex + anim->u.animParent.children, secondWeight, calcBuffer, false, true, animInfo, bufferIndex );

		assert(bNormQuat || bClear);

		for ( thirdInfoIndex = secondInfoIndex + 1; thirdInfoIndex < numAnims; thirdInfoIndex++ )
		{
			childIndex = tree->infoArray[thirdInfoIndex + anim->u.animParent.children];

			if ( childIndex )
			{
				thirdWeight = g_xAnimInfo[childIndex].state.weight;

				if ( thirdWeight != 0.0f )
				{
					XAnimCalc( obj, thirdInfoIndex + anim->u.animParent.children, thirdWeight, calcBuffer, false, true, animInfo, bufferIndex );
				}
			}
		}

		if ( bNormQuat )
		{
			if ( bClear )
			{
				for ( i = 0; i < obj->numBones; i++, rotTransArray++ )
				{
					if ( testBit( animInfo->ignorePartBits, i) )
					{
						continue;
					}

					r = Vec4LengthSq(rotTransArray->quat);

					if ( r != 0.0f )
					{
						VectorScale4(rotTransArray->quat, I_rsqrt(r) * weightScale, rotTransArray->quat);
					}

					if ( rotTransArray->transWeight != 0.0f )
					{
						VectorScale(rotTransArray->trans, weightScale / rotTransArray->transWeight, rotTransArray->trans);
						rotTransArray->transWeight = weightScale;
					}
				}
			}
			else
			{
				for ( i = 0; i < obj->numBones; i++, rotTransArray++, calcBuffer++ )
				{
					if ( testBit( animInfo->ignorePartBits, i ) )
					{
						continue;
					}

					r = Vec4LengthSq(calcBuffer->quat);

					if ( r != 0.0f )
					{
						VectorMA4(rotTransArray->quat, I_rsqrt(r) * weightScale, calcBuffer->quat, rotTransArray->quat);
					}

					if ( calcBuffer->transWeight != 0.0f )
					{
						VectorMA(rotTransArray->trans, weightScale / calcBuffer->transWeight, calcBuffer->trans, rotTransArray->trans);
						rotTransArray->transWeight += weightScale;
					}
				}
			}
		}
		else
		{
			for ( i = 0; i < obj->numBones; i++, rotTransArray++ )
			{
				if ( testBit( animInfo->ignorePartBits, i ) )
				{
					continue;
				}

				if ( rotTransArray->transWeight == 0.0f )
				{
					continue;
				}

				VectorScale4(rotTransArray->quat, 1.0f / rotTransArray->transWeight, rotTransArray->quat);
				VectorScale(rotTransArray->trans, 1.0f / rotTransArray->transWeight, rotTransArray->trans);
			}
		}
	}
	else
	{
		if ( bClear )
		{
			XAnimClearData(obj, rotTransArray, animInfo);
		}

		assert(animIndex);

		models = (byte *)&obj->animToModel[tree->anims->size];

		if ( obj->animToModel[animIndex] )
		{
			if ( models[animIndex + 1] != *models )
			{
				models[animIndex + 1] = *models;
				SL_RemoveRefToStringOfLen( obj->animToModel[animIndex], anim->u.parts->boneCount + DOBJ_PARTBITS_STRING_LEN );
				obj->animToModel[animIndex] = XAnimSetModel( anim, obj->models, obj->numModels );
			}
		}
		else
		{
			models[animIndex + 1] = *models;
			obj->animToModel[animIndex] = XAnimSetModel(anim, obj->models, obj->numModels);
		}

		animToModel = (const int *)SL_ConvertToString(obj->animToModel[animIndex]);

		for ( i = 0; i < DOBJ_MAX_PART_BITS; i++ )
		{
			animInfo->animPartBits[i] |= animToModel[i] & ~animInfo->ignorePartBits[i];
		}

		parts = anim->u.parts;
		assert(parts);

		fTime = g_xAnimInfo[tree->infoArray[animIndex]].state.time;
		assert(parts->bLoop ? (fTime < 1.f) : (fTime <= 1.f));

		if ( fTime == 1.0f || !parts->numframes )
		{
			assert(!parts->bLoop);
			XAnimCalcNonLoopEnd( parts, (const unsigned char *)animToModel + DOBJ_PARTBITS_STRING_LEN, weightScale, rotTransArray, animInfo->ignorePartBits );
		}
		else if ( parts->numframes <= UCHAR_MAX )
		{
			XAnimCalcParts_unsigned_char_( parts, (const unsigned char *)animToModel + DOBJ_PARTBITS_STRING_LEN, fTime, weightScale, rotTransArray, animInfo->ignorePartBits );
		}
		else
		{
			XAnimCalcParts_unsigned_short_( parts, (const unsigned char *)animToModel + DOBJ_PARTBITS_STRING_LEN, fTime, weightScale, rotTransArray, animInfo->ignorePartBits );
		}
	}
}

/*
==================
NotifyServerNotetrack
==================
*/
void NotifyServerNotetrack( const XAnimTree_s *tree, unsigned int notifyName, unsigned int notetrackName )
{
	assert(tree);

	Scr_AddConstString( notetrackName );
	Scr_NotifyNum( tree->entnum - 1, CLASS_NUM_ENTITY, notifyName, 1 );
}

/*
==================
XAnimProcessServerNotify
==================
*/
void XAnimProcessServerNotify( const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry, float fTime )
{
	int iCurrNotify;
	XAnimNotifyInfo *notify;
	XAnimParts *parts;

	assert(tree);
	assert(info->state.goalWeight);
	assert(fTime <= 1.f);

	if ( !tree->entnum )
	{
		return;
	}

	if ( !info->notifyName )
	{
		return;
	}

	if ( info->state.time == 1.0f )
	{
		Scr_AddConstString( g_end );
		Scr_NotifyNum( tree->entnum - 1, CLASS_NUM_ENTITY, info->notifyName, 1 );
		return;
	}

	if ( info->notifyIndex < 0 )
	{
		XAnimUpdateServerNotify(tree, info, entry);

		if ( info->notifyIndex < 0 )
		{
			if ( info->state.time > fTime || fTime == 1.0f )
			{
				Scr_AddConstString( g_end );
				Scr_NotifyNum( tree->entnum - 1, CLASS_NUM_ENTITY, info->notifyName, 1 );
				return;
			}

			return;
		}
	}

	if ( entry->numAnims )
	{
		entry = &tree->anims->entries[info->notifyChild];
	}

	parts = entry->u.parts;
	assert(parts);

	iCurrNotify = info->notifyIndex;

	notify = &parts->notify[iCurrNotify];
	assert(notify);

	if ( fTime < info->state.time )
	{
		if ( notify->time < fTime )
		{
			do
			{
				NotifyServerNotetrack(tree, info->notifyName, notify->name);

				notify++;
				iCurrNotify++;
			}
			while ( iCurrNotify < parts->notifyCount && fTime > notify->time );
		}
		else if ( notify->time >= info->state.time )
		{
			do
			{
				NotifyServerNotetrack(tree, info->notifyName, notify->name);

				notify++;
				iCurrNotify++;
			}
			while ( iCurrNotify < parts->notifyCount );

			for ( notify = parts->notify; fTime > notify->time; notify++ )
			{
				NotifyServerNotetrack(tree, info->notifyName, notify->name);
			}
		}
	}
	else if ( fTime == 1.0f )
	{
		if ( notify->time >= info->state.time )
		{
			do
			{
				NotifyServerNotetrack(tree, info->notifyName, notify->name);

				notify++;
				iCurrNotify++;
			}
			while ( iCurrNotify < parts->notifyCount );
		}
	}
	else if ( fTime > notify->time && notify->time >= info->state.time )
	{
		do
		{
			NotifyServerNotetrack(tree, info->notifyName, notify->name);

			notify++;
			iCurrNotify++;
		}
		while ( iCurrNotify < parts->notifyCount && fTime > notify->time );
	}
}

/*
==================
XAnimUpdateInfoSyncInternal
==================
*/
static void XAnimUpdateInfoSyncInternal( const XAnimTree *tree, unsigned int animIndex, bool hasWeight, XAnimState *syncState, float dtime )
{
	XAnimEntry *anim;
	XAnimState *state;
	XAnimInfo *info;
	unsigned int infoIndex;
	int numAnims, i;

	assert(dtime > 0);

	infoIndex = tree->infoArray[animIndex];
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	if ( !infoIndex )
	{
		return;
	}

	info = &g_xAnimInfo[infoIndex];
	assert(info);

	state = &info->state;
	assert(state);

	if ( info->state.weight == 0.0f )
	{
		return;
	}

	if ( info->state.goalWeight == 0.0f )
	{
		hasWeight = false;
	}

	anim = &tree->anims->entries[animIndex];
	assert(anim);

	if ( info->state.oldTime != syncState->oldTime || info->state.oldCycleCount != syncState->oldCycleCount )
	{
		state->time = syncState->oldTime;

		info->state.cycleCount = syncState->oldCycleCount;
		info->state.oldTime = syncState->oldTime;
		info->state.oldCycleCount = syncState->oldCycleCount;

		info->notifyIndex = -1;
	}

	if ( hasWeight )
	{
		XAnimProcessServerNotify( tree, info, anim, syncState->time );
	}

	state->time = syncState->time;

	info->state.cycleCount = syncState->cycleCount;
	info->notifyIndex = -1;

	if ( hasWeight )
	{
		XAnimProcessClientNotify( info, anim, dtime );
	}

	numAnims = anim->numAnims;

	for ( i = 0; i < numAnims; i++ )
	{
		XAnimUpdateInfoSyncInternal( tree, i + anim->u.animParent.children, hasWeight, syncState, dtime );
	}
}

/*
==================
XAnimUpdateInfoInternal
==================
*/
void XAnimUpdateInfoInternal( XAnimTree *tree, unsigned int animIndex, float dtime, bool hasWeight )
{
	XAnimParts_s *parts;
	XAnimEntry *anim;
	XAnimState *state;
	XAnimInfo *info;
	unsigned int infoIndex, cycleCount;
	int numAnims, i;
	float frameTime, fTime;

	assert(tree);
	assert(dtime >= 0);

	infoIndex = tree->infoArray[animIndex];
	assert((unsigned)infoIndex < MAX_XANIM_INFO);

	if ( !infoIndex )
	{
		return;
	}

	info = &g_xAnimInfo[infoIndex];
	assert(info);

	state = &info->state;
	assert(state);

	if ( info->state.weight == 0.0f )
	{
		return;
	}

	if ( info->state.goalWeight == 0.0f )
	{
		hasWeight = false;
	}

	anim = &tree->anims->entries[animIndex];
	numAnims = anim->numAnims;

	if ( numAnims )
	{
		if ( tree->anims->entries[animIndex].u.animParent.flags & ( XANIM_LOOP_SYNC_TIME | XANIM_NONLOOP_SYNC_TIME ) )
		{
			assert(anim->u.animParent.flags & XANIM_SYNC_ROOT);
			assert(state->oldTime >= 0.0f && state->oldTime <= 1.0f);

			fTime = XAnimGetAverageRateFrequency( tree, animIndex ) * info->state.rate * dtime;

			if ( fTime == 0.0f )
			{
				return;
			}

			frameTime = info->state.oldTime + fTime;
			cycleCount = info->state.oldCycleCount;

			assert(frameTime >= 0);

			if ( frameTime >= 1.0f )
			{
				if ( anim->u.animParent.flags & XANIM_NONLOOP_SYNC_TIME )
				{
					assert(!(anim->u.animParent.flags & XANIM_LOOP_SYNC_TIME));
					frameTime = 1.0f;
				}
				else
				{
					assert(anim->u.animParent.flags & XANIM_LOOP_SYNC_TIME);
					do
					{
						frameTime -= 1.0f;
						cycleCount++;
					}
					while ( frameTime >= 1.0f );
					assert(frameTime >= 0);
				}
			}

			assert(anim->u.animParent.flags & XANIM_LOOP_SYNC_TIME ? (frameTime < 1.f) : (frameTime <= 1.f));

			if ( info->state.time - frameTime > cycleCount - info->state.cycleCount )
			{
				return;
			}

			if ( hasWeight )
			{
				XAnimProcessServerNotify(tree, info, anim, frameTime);
			}

			state->time = frameTime;

			info->state.cycleCount = cycleCount;
			info->notifyIndex = -1;

			if ( hasWeight )
			{
				XAnimProcessClientNotify( info, anim, fTime );
			}

			for ( i = 0; i < numAnims; i++ )
			{
				XAnimUpdateInfoSyncInternal( tree, i + anim->u.animParent.children, hasWeight, state, fTime );
			}
		}
		else
		{
			assert(!(anim->u.animParent.flags & XANIM_SYNC_ROOT));
			fTime = dtime * info->state.rate;

			if ( fTime == 0.0f )
			{
				return;
			}

			for ( i = 0; i < numAnims; i++ )
			{
				XAnimUpdateInfoInternal( tree, i + anim->u.animParent.children, fTime, hasWeight );
			}
		}
	}
	else
	{
		parts = tree->anims->entries[animIndex].u.parts;
		assert(parts);

		assert(state->oldTime >= 0);
		fTime = info->state.rate * parts->frequency * dtime;

		if ( fTime == 0.0f )
		{
			return;
		}

		frameTime = info->state.oldTime + fTime;
		cycleCount = info->state.cycleCount;

		assert(frameTime >= 0);

		if ( frameTime >= 1.0f )
		{
			if ( parts->bLoop )
			{
				do
				{
					frameTime -= 1.0f;
					cycleCount++;
				}
				while ( frameTime >= 1.0f );
				assert(frameTime >= 0);
			}
			else
			{
				frameTime = 1.0f;
			}
		}

		if ( info->state.time - frameTime > cycleCount - info->state.cycleCount )
		{
			return;
		}

		if ( hasWeight )
		{
			XAnimProcessServerNotify( tree, info, anim, frameTime );
		}

		state->time = frameTime;

		info->state.cycleCount = cycleCount;
		info->notifyIndex = -1;

		if ( hasWeight )
		{
			XAnimProcessClientNotify( info, anim, fTime );
		}
	}
}
