#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

typedef struct scrAnimGlob_s
{
	const char *start;
	const char *pos;
	unsigned short using_xanim_lookup[2][128];
	int bAnimCheck;
} scrAnimGlob_t;
static_assert((sizeof(scrAnimGlob_t) == 0x20C), "ERROR: scrAnimGlob_t size is invalid!");

scrAnimGlob_t scrAnimGlob;
scrAnimPub_t scrAnimPub;

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

void Scr_EmitAnimationInternal(char *pos, unsigned int animName, unsigned int names)
{
	VariableValueInternal_u *adr;
	unsigned int animId;
	unsigned int newAnimId;
	VariableValue tempValue;

	animId = FindVariable(names, animName);

	if ( animId )
	{
		adr = GetVariableValueAddress(animId);
		*(VariableValueInternal_u *)pos = *adr;
		adr->u.codePosValue = pos;
	}
	else
	{
		newAnimId = GetNewVariable(names, animName);
		*(int *)pos = 0;
		tempValue.type = VAR_CODEPOS;
		tempValue.u.codePosValue = pos;
		SetVariableValue(newAnimId, &tempValue);
	}
}

void Scr_EmitAnimation(char *pos, unsigned int animName, unsigned int sourcePos)
{
	if ( scrAnimPub.animTreeNames )
		Scr_EmitAnimationInternal(pos, animName, scrAnimPub.animTreeNames);
	else
		CompileError(sourcePos, "#using_animtree was not specified");
}

void Scr_FindAnim(const char *filename, const char *animName, scr_anim_s *anim, int user)
{
	unsigned int tree;
	unsigned int index;
	unsigned int name;

	name = SL_GetLowercaseString_(animName, 0);
	tree = Scr_UsingTreeInternal(filename, &index, user);
	Scr_EmitAnimationInternal((char *)anim, name, tree);
	SL_RemoveRefToString(name);
}

void AnimTreeCompileError(const char *msg)
{
	const char *pos;

	pos = Com_GetLastTokenPos();
	Com_EndParseSession();
	CompileError(pos - scrAnimGlob.start, "%s", msg);
}

void ConnectScriptToAnim(unsigned int names, int index, unsigned int filename, unsigned short name, int treeIndex)
{
	scr_anim_s newAnim;
	const char *linkPointer;
	scr_anim_s *i;
	scr_anim_s **adr;
	unsigned int id;

	id = FindVariable(names, name);

	if ( id )
	{
		adr = (scr_anim_s **)GetVariableValueAddress(id);

		if ( !*adr )
		{
			Com_Error(ERR_DROP, "duplicate animation '%s' in 'animtrees/%s.atr'", SL_ConvertToString(name), SL_ConvertToString(filename));
		}

		newAnim.index = index;
		newAnim.tree = treeIndex;

		for ( i = *adr; i; i = (scr_anim_s *)linkPointer )
		{
			linkPointer = i->linkPointer;
			*i = newAnim;
		}

		*adr = 0;
	}
}

int Scr_CreateAnimationTree(unsigned int parentNode, unsigned int names, XAnim_s *anims, unsigned int childIndex, const char *parentName, unsigned int parentIndex, unsigned int filename, int treeIndex)
{
	const char *name;
	unsigned int id;
	const char *xanimName;
	uint16_t refCount;
	unsigned int arrayIndex;
	unsigned int num;
	unsigned int index;
	unsigned int i;
	unsigned int j;
	int children;
	unsigned int animIndex;

	num = 0;
	for ( i = FindNextSibling(parentNode); i; i = FindNextSibling(i) )
	{
		if ( GetVariableName(i) <= 0xFFFF )
			++num;
	}

	arrayIndex = FindArrayVariable(parentNode, 0);

	if ( arrayIndex )
		refCount = GetVariableValueAddress(arrayIndex)->refCount;
	else
		refCount = 0;

	scrVarPub.checksum *= 31;
	scrVarPub.checksum += parentIndex;
	scrVarPub.checksum *= 31;
	scrVarPub.checksum += childIndex;
	scrVarPub.checksum *= 31;
	scrVarPub.checksum += num;
	scrVarPub.checksum *= 31;
	scrVarPub.checksum += refCount;

	XAnimBlend(anims, parentIndex, parentName, childIndex, num, refCount);
	animIndex = childIndex;
	children = num + childIndex;

	for ( j = FindNextSibling(parentNode); j; j = FindNextSibling(j) )
	{
		index = GetVariableName(j);

		if ( index <= 0xFFFF )
		{
			ConnectScriptToAnim(names, animIndex, filename, index, treeIndex);

			if ( GetVarType(j) == VAR_OBJECT )
			{
				name = SL_ConvertToString(index);
				id = FindObject(j);
				children = Scr_CreateAnimationTree(id, names, anims, children, name, animIndex, filename, treeIndex);
			}
			else
			{
				scrVarPub.checksum *= 31;
				scrVarPub.checksum += animIndex;
				xanimName = SL_ConvertToString(index);
				XAnimCreate(anims, animIndex, xanimName);
			}

			++animIndex;
		}
	}

	return children;
}

void Scr_CheckAnimsDefined(unsigned int names, unsigned int filename)
{
	VariableValueInternal_u *adr;
	unsigned int i;
	unsigned int name;

	for ( i = FindNextSibling(names); i; i = FindNextSibling(i) )
	{
		name = GetVariableName(i);
		adr = GetVariableValueAddress(i);

		if ( adr->u.intValue )
		{
			if ( Scr_IsInOpcodeMemory(adr->u.codePosValue) )
				CompileError2(adr->u.codePosValue, "%s", va("animation '%s' not defined in anim tree '%s'", SL_ConvertToString(name), SL_ConvertToString(filename)));

			Com_Error(ERR_DROP, "%s", va("animation '%s' not defined in anim tree '%s'", SL_ConvertToString(name), SL_ConvertToString(filename)));
		}
	}
}

void Scr_PrecacheAnimationTree(unsigned int parentNode)
{
	unsigned int id;
	const char *name;
	unsigned int index;
	unsigned int i;

	for ( i = FindNextSibling(parentNode); i; i = FindNextSibling(i) )
	{
		index = GetVariableName(i);

		if ( index <= 0xFFFF )
		{
			if ( GetVarType(i) == VAR_OBJECT )
			{
				id = FindObject(i);
				Scr_PrecacheAnimationTree(id);
			}
			else
			{
				name = SL_ConvertToString(index);
				XAnimPrecache(name, Hunk_AllocXAnimTreePrecache);
			}
		}
	}
}

int Scr_GetAnimTreeSize(unsigned int parentNode)
{
	unsigned int id;
	int count;
	unsigned int i;

	count = 0;

	for ( i = FindNextSibling(parentNode); i; i = FindNextSibling(i) )
	{
		if ( GetVariableName(i) <= 0xFFFF )
		{
			if ( GetVarType(i) == VAR_OBJECT )
			{
				id = FindObject(i);
				count += Scr_GetAnimTreeSize(id);
			}
			else
			{
				++count;
			}
		}
	}

	if ( count )
		++count;

	return count;
}

int GetAnimTreeParseProperties()
{
	int flags;
	int i;
	char *token;

	flags = 0;

	while ( 1 )
	{
		token = Com_ParseOnLine(&scrAnimGlob.pos);
		if ( !*token )
			return flags;
		for ( i = 0; (unsigned int)i <= 2 && strcasecmp(token, propertyNames[i]); ++i )
			;
		if ( i == 1 )
		{
			flags |= 2u;
		}
		else if ( i > 1 )
		{
			if ( i == 2 )
				flags |= 8u;
			else
error:
				AnimTreeCompileError("unknown anim property");
		}
		else
		{
			if ( i )
				goto error;
			flags |= 1u;
		}
	}
}

bool AnimTreeParseInternal(unsigned int parentNode, unsigned int names, bool bIncludeParent, bool bLoop, bool bComplete)
{
	unsigned int arrayIndex;
	const char *s;
	bool completeSet;
	char ignoreSet;
	bool result;
	char ignore;
	int props;
	unsigned int node;
	VariableValue tempValue;
	unsigned int index;
	unsigned int id;
	char *str;
	bool complete;
	bool loop;
	bool parent;

	parent = bIncludeParent;
	loop = bLoop;
	complete = bComplete;
	tempValue.type = VAR_INTEGER;
	index = 0;
	id = 0;
	props = 0;
	ignore = 0;

	while ( 1 )
	{
		while ( 1 )
		{
			while ( 1 )
			{
				str = Com_Parse(&scrAnimGlob.pos);
				if ( !scrAnimGlob.pos )
				{
					result = 1;
					goto out;
				}
				if ( !Scr_IsIdentifier(str) )
					break;
				if ( ignore )
					RemoveVariable(parentNode, index);
				index = SL_GetLowercaseString_(str, 2u);
				if ( FindVariable(parentNode, index) )
					AnimTreeCompileError("duplicate animation");
				id = GetVariable(parentNode, index);
				ignoreSet = 0;
				if ( !complete && !FindVariable(names, index) && !scrAnimGlob.bAnimCheck )
					ignoreSet = 1;
				ignore = ignoreSet;
				props = 0;
				str = Com_ParseOnLine(&scrAnimGlob.pos);
				if ( *str )
				{
					if ( Scr_IsIdentifier(str) )
						AnimTreeCompileError("FIXME: aliases not yet implemented");
					if ( *str != 58 || str[1] )
						AnimTreeCompileError("bad token");
					props = GetAnimTreeParseProperties();
					str = Com_Parse(&scrAnimGlob.pos);
					if ( *str != 123 || str[1] )
						AnimTreeCompileError("properties cannot be applied to primitive animations");
					break;
				}
			}
			if ( *str != 123 )
				break;
			if ( str[1] )
				AnimTreeCompileError("bad token");
			if ( *Com_ParseOnLine(&scrAnimGlob.pos) )
				AnimTreeCompileError("token not allowed after '{'");
			if ( !id )
				AnimTreeCompileError("no animation specified for this block");
			node = GetArray(id);
			completeSet = 0;
			if ( complete || (props & 8) != 0 && !ignore )
				completeSet = 1;
			if ( AnimTreeParseInternal(node, names, ignore != 1, props & 1, completeSet) )
				AnimTreeCompileError("unexpected end of file");
			if ( GetArraySize(node) )
			{
				tempValue.u.intValue = props;
				arrayIndex = GetArrayVariable(node, 0);
				SetVariableValue(arrayIndex, &tempValue);
			}
			else
			{
				RemoveVariable(parentNode, index);
			}
			id = 0;
			ignore = 0;
		}
		if ( *str == 125 )
			break;
		AnimTreeCompileError("bad token");
	}
	if ( str[1] )
		AnimTreeCompileError("bad token");
	if ( *Com_ParseOnLine(&scrAnimGlob.pos) )
		AnimTreeCompileError("token not allowed after '}'");
	result = 0;
out:
	if ( ignore )
		RemoveVariable(parentNode, index);
	if ( parent && !GetArraySize(parentNode) )
	{
		if ( loop )
			s = "void_loop";
		else
			s = "void";
		index = SL_GetString_(s, 0);
		GetVariable(parentNode, index);
		SL_RemoveRefToString(index);
	}

	return result;
}

void Scr_AnimTreeParse(const char *pos, unsigned int parentNode, unsigned int names)
{
	Com_BeginParseSession("Scr_AnimTreeParse");
	scrAnimGlob.pos = pos;
	scrAnimGlob.start = pos;

	if ( !AnimTreeParseInternal(parentNode, names, 1, 0, 0) )
		AnimTreeCompileError("bad token");

	Com_EndParseSession();
}

bool Scr_LoadAnimTreeInternal(const char *filename, unsigned int parentNode, unsigned int names)
{
	const char *sourceBuf;
	const char *scriptfilename;
	char *buffer;
	char extFilename[64];

	sprintf(extFilename, "animtrees/%s.atr", filename);
	sourceBuf = scrParserPub.sourceBuf;
	buffer = Scr_AddSourceBuffer(0, extFilename, 0, 1);

	if ( !buffer )
		return 0;

	scriptfilename = scrParserPub.scriptfilename;
	scrParserPub.scriptfilename = extFilename;
	Scr_AnimTreeParse(buffer, parentNode, names);
	scrParserPub.scriptfilename = scriptfilename;
	scrParserPub.sourceBuf = sourceBuf;
	Hunk_ClearTempMemoryHighInternal();

	return GetArraySize(parentNode) != 0;
}

void Scr_LoadAnimTreeAtIndex(unsigned int index, void *(*Alloc)(int), int user)
{
	unsigned int variable;
	unsigned int id;
	unsigned int parentId;
	unsigned int name;
	XAnim_s *anims;
	unsigned int filenameId;
	int size;
	unsigned int names;
	unsigned int fileId;
	VariableValue tempValue;

	id = scrAnimGlob.using_xanim_lookup[user][index];
	filenameId = (unsigned short)GetVariableName(id);
	parentId = FindObject(id);

	if ( !FindVariable(parentId, 1u) )
	{
		names = FindVariable(parentId, 0);

		if ( names )
		{
			fileId = FindObject(names);
			scrAnimPub.animtree_node = Scr_AllocArray();

			if ( !Scr_LoadAnimTreeInternal(SL_ConvertToString(filenameId), scrAnimPub.animtree_node, fileId) )
			{
				Com_Error(ERR_DROP, va("unknown anim tree '%s'", SL_ConvertToString(filenameId)));
			}

			size = Scr_GetAnimTreeSize(scrAnimPub.animtree_node);
			anims = XAnimCreateAnims(SL_ConvertToString(filenameId), size, Alloc);
			name = SL_GetString_("root", 0);
			ConnectScriptToAnim(fileId, 0, filenameId, name, index);
			SL_RemoveRefToString(name);
			Scr_PrecacheAnimationTree(scrAnimPub.animtree_node);
			Scr_CreateAnimationTree(scrAnimPub.animtree_node, fileId, anims, 1u, "root", 0, filenameId, index);
			Scr_CheckAnimsDefined(fileId, filenameId);
			RemoveVariable(parentId, 0);
			RemoveRefToObject(scrAnimPub.animtree_node);
			scrAnimPub.animtree_node = 0;
			tempValue.type = VAR_CODEPOS;
			tempValue.u.intValue = (int)anims;
			variable = GetVariable(parentId, 1u);
			SetVariableValue(variable, &tempValue);
			XAnimSetupSyncNodes(anims);
			scrAnimPub.xanim_lookup[user][index].anims = anims;
		}
		else
		{
			scrAnimPub.xanim_lookup[user][index].anims = 0;
		}
	}
}

void Scr_FindAnimTreeInternal(scr_animtree_t *pTree, const char *filename)
{
	VariableValue tempValue;
	unsigned int index;
	unsigned int name;
	unsigned int id;

	name = Scr_CreateCanonicalFilename(filename);
	id = FindVariable(scrAnimPub.animtrees, name);
	SL_RemoveRefToString(name);
	pTree->anims = 0;

	if ( id )
	{
		name = (unsigned short)GetVariableName(id);
		id = FindObject(id);
		index = FindVariable(id, 1u);

		if ( index )
		{
			Scr_EvalVariable(&tempValue, index);
			pTree->anims = (XAnim_s *)tempValue.u.intValue;
		}
	}
}

scr_animtree_t Scr_FindAnimTree(const char *filename)
{
	scr_animtree_t tree;

	Scr_FindAnimTreeInternal(&tree, filename);

	return tree;
}