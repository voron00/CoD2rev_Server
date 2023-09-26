#include "gsc_memory.hpp"

#if COMPILE_MEMORY == 1

void gsc_memory_malloc()
{
	int bytes;

	if ( ! stackGetParams("i", &bytes))
	{
		stackError("gsc_memory_malloc() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushInt((intptr_t)malloc(bytes));
}

void gsc_memory_free()
{
	int memory;

	if ( ! stackGetParams("i", &memory))
	{
		stackError("gsc_memory_free() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	free((void*)memory);
	stackPushInt(0);
}

void gsc_memory_int_get()
{
	int memory;

	if ( ! stackGetParams("i", &memory))
	{
		stackError("gsc_memory_int_get() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushInt(*(int*)memory);
}

void gsc_memory_int_set()
{
	int memory, value;

	if ( ! stackGetParams("ii", &memory, &value))
	{
		stackError("gsc_memory_int_set() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	*(int*)memory = value;
	stackPushInt(1);
}

void gsc_memory_memset()
{
	int memory, value, bytes;

	if ( ! stackGetParams("iii", &memory, &value, &bytes))
	{
		stackError("gsc_memory_memset() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	memset((void*)memory, value, bytes);
	stackPushInt(1);
}

#include <vector>
struct binarybuffer
{
	int address;
	int pos;
	std::vector<char *> *strings;
};

void gsc_binarybuffer_new()
{
	int address;
	if ( ! stackGetParams("i", &address))
	{
		stackError("gsc_binarybuffer_new() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	struct binarybuffer *bb = (struct binarybuffer *)malloc(sizeof(struct binarybuffer));
	bb->address = address;
	bb->pos = 0;
	bb->strings = new std::vector<char *>();
	stackPushInt((intptr_t)bb);
}

void gsc_binarybuffer_free()
{
	struct binarybuffer *bb;
	if ( ! stackGetParams("i", &bb))
	{
		stackError("gsc_binarybuffer_free() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	for (std::vector<char *>::const_iterator i = bb->strings->begin(); i != bb->strings->end(); i++)
		free(*i);
	delete bb->strings;
	free(bb);
	stackPushInt(1);
}

void gsc_binarybuffer_seek()
{
	struct binarybuffer *bb;
	int pos;
	if ( ! stackGetParams("ii", &bb, &pos))
	{
		stackError("gsc_binarybuffer_seek() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	bb->pos = pos;
	stackPushInt(1);
}

void gsc_binarybuffer_write()
{
	struct binarybuffer *bb;
	const char *type;
	if ( ! stackGetParams("is", &bb, &type))
	{
		stackError("gsc_binarybuffer_write() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	switch (type[0])
	{
	case 'i':
	{
		int tmp_int;
		stackGetParamInt(2, &tmp_int);
		*(int *)(bb->address + bb->pos) = tmp_int;
		bb->pos += 4;
		break;
	}
	case 'f':
	{
		float tmp_float;
		stackGetParamFloat(2, &tmp_float);
		*(float *)(bb->address + bb->pos) = tmp_float;
		bb->pos += 4;
		break;
	}
	case 'd':
	{
		float tmp_float;
		stackGetParamFloat(2, &tmp_float);
		*(double *)(bb->address + bb->pos) = (double)tmp_float;
		bb->pos += 8;
		break;
	}
	case 's':
	{
		const char *tmp_str;
		stackGetParamString(2, &tmp_str);
		char *copy = (char *)malloc(strlen(tmp_str) + 1);
		strcpy(copy, tmp_str);
		bb->strings->push_back(copy);
		*(char **)(bb->address + bb->pos) = copy;
		bb->pos += 4;
		break;
	}
	case 'c':
	{
		const char *tmp_str;
		stackGetParamString(2, &tmp_str);
		*(char *)(bb->address + bb->pos) = tmp_str[0];
		bb->pos += 1;
		break;
	}
	case 'v':
	{
		float tmp_vector[3];
		stackGetParamVector(2, tmp_vector);
		*(float *)(bb->address + bb->pos + 0) = tmp_vector[0];
		*(float *)(bb->address + bb->pos + 4) = tmp_vector[1];
		*(float *)(bb->address + bb->pos + 8) = tmp_vector[2];
		bb->pos += 12;
		break;
	}
	}
	stackPushInt(1);
}

void gsc_binarybuffer_read()
{
	struct binarybuffer *bb;
	const char *type;
	if ( ! stackGetParams("is", &bb, &type))
	{
		stackError("gsc_binarybuffer_read() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	switch (type[0])
	{
	case 'i':
	{
		int tmp_int;
		tmp_int = *(int *)(bb->address + bb->pos);
		bb->pos += 4;
		stackPushInt(tmp_int);
		return;
	}
	case 'f':
	{
		float tmp_float;
		tmp_float = *(float *)(bb->address + bb->pos);
		bb->pos += 4;
		stackPushFloat(tmp_float);
		return;
	}
	case 'd':
	{
		float tmp_float;
		tmp_float = (float)*(double *)(bb->address + bb->pos);
		bb->pos += 8;
		stackPushFloat(tmp_float);
		return;
	}
	case 's':
	{
		char *tmp_str;
		tmp_str = *(char **)(bb->address + bb->pos);
		bb->pos += 4;
		stackPushString(tmp_str);
		return;
	}
	case 'c':
	{
		char tmp_str[2];
		tmp_str[0] = *(char *)(bb->address + bb->pos);
		tmp_str[1] = '\0';
		bb->pos += 1;
		stackPushString(tmp_str);
		return;
	}
	case 'v':
	{
		float *tmp_vector;
		tmp_vector = (float *)(bb->address + bb->pos + 0);
		bb->pos += 12;
		stackPushVector(tmp_vector);
		return;
	}
	}
	stackPushUndefined();
}
#endif
