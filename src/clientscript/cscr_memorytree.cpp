#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

#define MEMORY_NODE_BITS 16
#define MEMORY_NODE_SIZE 16
#define MEMORY_NODE_COUNT (1 << MEMORY_NODE_BITS)
#define NUM_BUCKETS 256

struct __attribute__((aligned(8))) MemoryNode
{
	uint16_t prev;
	uint16_t next;
};

typedef struct __attribute__((aligned(128))) scrMemTreeGlob_s
{
	MemoryNode nodes[MEMORY_NODE_COUNT];
	char leftBits[NUM_BUCKETS];
	char numBits[NUM_BUCKETS];
	char logBits[NUM_BUCKETS];
	uint16_t head[MEMORY_NODE_BITS + 1];
	int totalAlloc;
	int totalAllocBuckets;
} scrMemTreeGlob_t;
// static_assert((sizeof(scrMemTreeGlob_t) == 0x80380), "ERROR: scrMemTreeGlob_t size is invalid!");

scrMemTreeGlob_t scrMemTreeGlob;

int Scr_GetStringUsage()
{
	return scrMemTreeGlob.totalAllocBuckets;
}

int MT_GetSubTreeSize(int nodeNum)
{
	int treeSize;

	if (nodeNum)
	{
		treeSize = MT_GetSubTreeSize(scrMemTreeGlob.nodes[nodeNum].next);
		return treeSize + MT_GetSubTreeSize(scrMemTreeGlob.nodes[nodeNum].prev) + 1;
	}
	else
	{
		return 0;
	}
}

void MT_DumpTree()
{
	int buckets;
	int size;
	int i;

	Com_Printf("********************************\n");
	buckets = scrMemTreeGlob.totalAllocBuckets;

	for ( i = 0; i <= MEMORY_NODE_BITS; ++i )
	{
		size = MT_GetSubTreeSize(scrMemTreeGlob.head[i]);
		buckets += size << i;
		Com_Printf("%d subtree has %d * %d = %d free buckets\n", i, size, 1 << i, size << i);
	}

	Com_Printf("********************************\n");
	Com_Printf("********************************\n");
	Com_Printf("total memory alloc buckets: %d (%d instances)\n", scrMemTreeGlob.totalAllocBuckets, scrMemTreeGlob.totalAlloc);
	Com_Printf("total memory free buckets: %d\n", MEMORY_NODE_COUNT - scrMemTreeGlob.totalAllocBuckets);
	Com_Printf("********************************\n");
}

int MT_GetScore(int num)
{
	char bits;

	union MTnum_t
	{
		int i;
		uint8_t b[4];
	};

	MTnum_t mtnum;

	mtnum.i = MEMORY_NODE_COUNT - num;

	bits = scrMemTreeGlob.leftBits[mtnum.b[0]];

	if (!mtnum.b[0])
	{
		bits += scrMemTreeGlob.leftBits[mtnum.b[1]];
	}

	return mtnum.i - (scrMemTreeGlob.numBits[mtnum.b[1]] + scrMemTreeGlob.numBits[mtnum.b[0]]) + (1 << bits);
}

void MT_InitBits()
{
	char bits;
	int temp;
	int i;

	for (i = 0; i < NUM_BUCKETS; ++i)
	{
		bits = 0;

		for (temp = i; temp; temp >>= 1)
		{
			if (temp & 1)
			{
				++bits;
			}
		}

		scrMemTreeGlob.numBits[i] = bits;

		for (bits = 8; i & ((1 << bits) - 1); --bits);

		scrMemTreeGlob.leftBits[i] = bits;
		bits = 0;

		for (temp = i; temp; temp >>= 1)
		{
			++bits;
		}

		scrMemTreeGlob.logBits[i] = bits;
	}
}

void MT_AddMemoryNode(int newNode, int size)
{
	int node;
	int nodeNum;
	int newScore;
	uint16_t* parentNode;
	int level;
	int score;

	parentNode = &scrMemTreeGlob.head[size];
	node = scrMemTreeGlob.head[size];
	if (scrMemTreeGlob.head[size])
	{
		newScore = MT_GetScore(newNode);
		nodeNum = 0;
		level = MEMORY_NODE_COUNT;
		do
		{
			score = MT_GetScore(node);

			if (score < newScore)
			{
				while (1)
				{
					*parentNode = newNode;
					scrMemTreeGlob.nodes[newNode] = scrMemTreeGlob.nodes[node];
					if (!node)
					{
						break;
					}
					level >>= 1;

					if (node >= nodeNum)
					{
						parentNode = &scrMemTreeGlob.nodes[newNode].next;
						nodeNum += level;
					}
					else
					{
						parentNode = &scrMemTreeGlob.nodes[newNode].prev;
						nodeNum -= level;
					}
					newNode = node;
					node = *parentNode;
				}
				return;
			}
			level >>= 1;

			if (newNode >= nodeNum)
			{
				parentNode = &scrMemTreeGlob.nodes[node].next;
				nodeNum += level;
			}
			else
			{
				parentNode = &scrMemTreeGlob.nodes[node].prev;
				nodeNum -= level;
			}

			node = *parentNode;
		}
		while (node);
	}

	*parentNode = newNode;

	scrMemTreeGlob.nodes[newNode].prev = 0;
	scrMemTreeGlob.nodes[newNode].next = 0;
}

bool MT_RemoveMemoryNode(int oldNode, int size)
{
	MemoryNode tempNodeValue;
	int node;
	MemoryNode oldNodeValue;
	int nodeNum;
	uint16_t* parentNode;
	int prevScore;
	int nextScore;
	int level;

	nodeNum = 0;
	level = MEMORY_NODE_COUNT;
	parentNode = &scrMemTreeGlob.head[size];

	for (node = *parentNode; node; node = *parentNode)
	{
		if (oldNode == node)
		{
			oldNodeValue = scrMemTreeGlob.nodes[oldNode];

			while (1)
			{
				if (oldNodeValue.prev)
				{
					if (oldNodeValue.next)
					{
						prevScore = MT_GetScore(oldNodeValue.prev);
						nextScore = MT_GetScore(oldNodeValue.next);

						if (prevScore >= nextScore)
						{
							oldNode = oldNodeValue.prev;
							*parentNode = oldNodeValue.prev;
							parentNode = &scrMemTreeGlob.nodes[oldNodeValue.prev].prev;
						}
						else
						{
							oldNode = oldNodeValue.next;
							*parentNode = oldNodeValue.next;
							parentNode = &scrMemTreeGlob.nodes[oldNodeValue.next].next;
						}
					}
					else
					{
						oldNode = oldNodeValue.prev;
						*parentNode = oldNodeValue.prev;
						parentNode = &scrMemTreeGlob.nodes[oldNodeValue.prev].prev;
					}
				}
				else
				{
					oldNode = oldNodeValue.next;
					*parentNode = oldNodeValue.next;

					if (!oldNodeValue.next)
					{
						return true;
					}

					parentNode = &scrMemTreeGlob.nodes[oldNodeValue.next].next;
				}

				tempNodeValue = oldNodeValue;
				oldNodeValue = scrMemTreeGlob.nodes[oldNode];
				scrMemTreeGlob.nodes[oldNode] = tempNodeValue;
			}
		}

		if (oldNode == nodeNum)
		{
			return false;
		}

		level >>= 1;

		if (oldNode >= nodeNum)
		{
			parentNode = &scrMemTreeGlob.nodes[node].next;
			nodeNum += level;
		}
		else
		{
			parentNode = &scrMemTreeGlob.nodes[node].prev;
			nodeNum -= level;
		}
	}

	return false;
}

void MT_RemoveHeadMemoryNode(int size)
{
	MemoryNode tempNodeValue;
	int oldNode;
	MemoryNode oldNodeValue;
	uint16_t* parentNode;
	int prevScore;
	int nextScore;

	parentNode = &scrMemTreeGlob.head[size];
	oldNodeValue = scrMemTreeGlob.nodes[*parentNode];

	while (1)
	{
		if (!oldNodeValue.prev)
		{
			oldNode = oldNodeValue.next;
			*parentNode = oldNodeValue.next;
			if (!oldNode)
			{
				break;
			}
			parentNode = &scrMemTreeGlob.nodes[oldNode].next;
		}
		else
		{
			if (oldNodeValue.next)
			{
				prevScore = MT_GetScore(oldNodeValue.prev);
				nextScore = MT_GetScore(oldNodeValue.next);

				assert(prevScore != nextScore);

				if (prevScore >= nextScore)
				{
					oldNode = oldNodeValue.prev;
					*parentNode = oldNode;
					parentNode = &scrMemTreeGlob.nodes[oldNode].prev;
				}
				else
				{
					oldNode = oldNodeValue.next;
					*parentNode = oldNode;
					parentNode = &scrMemTreeGlob.nodes[oldNode].next;
				}
			}
			else
			{
				oldNode = oldNodeValue.prev;
				*parentNode = oldNode;
				parentNode = &scrMemTreeGlob.nodes[oldNode].prev;
			}
		}
		assert(oldNode != 0);

		tempNodeValue = oldNodeValue;
		oldNodeValue = scrMemTreeGlob.nodes[oldNode];
		scrMemTreeGlob.nodes[oldNode] = tempNodeValue;
	}
}

void MT_Error(const char *funcName, int numBytes)
{
	MT_DumpTree();
	Com_Printf("%s: failed memory allocation of %d bytes for script usage\n", funcName, numBytes);
	Scr_TerminalError("failed memory allocation for script usage");
}

int MT_GetSize(int numBytes)
{
	int numBuckets;

	if ( numBytes > MEMORY_NODE_COUNT )
	{
		MT_Error("MT_GetSize: max allocation exceeded", numBytes);
		return 0;
	}
	else
	{
		numBuckets = (numBytes + 7) / 8 - 1;

		if ( numBuckets > NUM_BUCKETS - 1 )
			return scrMemTreeGlob.logBits[numBuckets >> 8] + 8;
		else
			return scrMemTreeGlob.logBits[(numBytes + 7) / 8 - 1];
	}
}

unsigned short MT_AllocIndex(int numBytes)
{
	int nodeNum;
	int size;
	int newSize;

	size = MT_GetSize(numBytes);

	for (newSize = size; ; ++newSize)
	{
		if (newSize > MEMORY_NODE_BITS)
		{
			MT_Error("MT_AllocIndex", numBytes);
			return 0;
		}

		nodeNum = scrMemTreeGlob.head[newSize];

		if (scrMemTreeGlob.head[newSize])
		{
			break;
		}
	}

	MT_RemoveHeadMemoryNode(newSize);

	while (newSize != size)
	{
		--newSize;
		MT_AddMemoryNode(nodeNum + (1 << newSize), newSize);
	}

	++scrMemTreeGlob.totalAlloc;
	scrMemTreeGlob.totalAllocBuckets += 1 << size;

	return nodeNum;
}

void MT_FreeIndex(unsigned int nodeNum, int numBytes)
{
	int size;
	int lowBit;

	size = MT_GetSize(numBytes);

	--scrMemTreeGlob.totalAlloc;
	scrMemTreeGlob.totalAllocBuckets -= 1 << size;

	while (1)
	{
		lowBit = 1 << size;

		if (size == MEMORY_NODE_BITS || !MT_RemoveMemoryNode(lowBit ^ nodeNum, size))
		{
			break;
		}

		nodeNum &= ~lowBit;
		++size;
	}

	MT_AddMemoryNode(nodeNum, size);
}

void* MT_Alloc(int numBytes)
{
	return scrMemTreeGlob.nodes + MT_AllocIndex(numBytes);
}

void MT_Free(void* p, int numBytes)
{
	MT_FreeIndex((MemoryNode*)p - scrMemTreeGlob.nodes, numBytes);
}

qboolean MT_Realloc(int oldNumBytes, int newNumbytes)
{
	int size;

	size = MT_GetSize(oldNumBytes);
	return size >= MT_GetSize(newNumbytes);
}

byte* MT_GetRefByIndex(int index)
{
	if (index > MEMORY_NODE_COUNT)
	{
		MT_Error("MT_GetRefByIndex: out of bounds index", index);
		return NULL;
	}

	return (byte*)&scrMemTreeGlob.nodes[index];
}

int MT_GetIndexByRef(byte* p)
{
	int index = (MemoryNode *)p - scrMemTreeGlob.nodes;

	if (index < 0 || index >= MEMORY_NODE_COUNT)
	{
		MT_Error("MT_GetIndexByRef: out of bounds ref", index);
		return 0;
	}

	return index;
}

void MT_RelocateNode(int nodeNum)
{
	int newNode;
	int size;
	int bits;

	size = 0;

	for ( newNode = nodeNum; !MT_RemoveMemoryNode(newNode, size); newNode &= ~(1 << size++) )
	{
		if ( size == MEMORY_NODE_SIZE )
		{
			size = 0;
			newNode = nodeNum;

			while ( 1 )
			{
				bits = 1 << size;

				if ( size == MEMORY_NODE_SIZE || !MT_RemoveMemoryNode(newNode ^ bits, size) )
					break;

				newNode &= ~bits;
				++size;
			}

			break;
		}
	}

	MT_AddMemoryNode(newNode, size);
}

void MT_FreeForLength(byte *p, unsigned int data, int length)
{
	int count;
	char size;

	size = MT_GetSize(length);
	++scrMemTreeGlob.totalAlloc;
	count = 1 << size;
	scrMemTreeGlob.totalAllocBuckets += 1 << size;

	while ( count )
	{
		p[data >> 3] |= 1 << (data & 7);
		++data;
		--count;
	}
}

void *MT_BeginRelocate()
{
	scrMemTreeGlob.totalAlloc = 0;
	scrMemTreeGlob.totalAllocBuckets = 0;

	return Z_Malloc(0x2000);
}

void MT_EndRelocate(byte *ptr)
{
	int i;

	for ( i = 1; i <= 0xFFFF; ++i )
	{
		if ( (((int)ptr[i >> 3] >> (i & 7)) & 1) == 0 )
			MT_RelocateNode(i);
	}

	Z_Free(ptr);
}

void MT_Init()
{
	int i;

	MT_InitBits();

	for ( i = 0; i <= MEMORY_NODE_BITS; ++i )
	{
		scrMemTreeGlob.head[i] = 0;
	}

	scrMemTreeGlob.nodes[0].prev = 0;
	scrMemTreeGlob.nodes[0].next = 0;

	for ( i = 0; i < MEMORY_NODE_BITS; ++i )
	{
		MT_AddMemoryNode(1 << i, i);
	}

	scrMemTreeGlob.totalAlloc = 0;
	scrMemTreeGlob.totalAllocBuckets = 0;
}