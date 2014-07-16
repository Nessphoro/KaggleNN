// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define AVX

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <stdint.h>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <iostream>
#include <fstream>
#include "GraphManager.h"
#include "DatabaseBuilder.h"

struct RawSample
{
	uint32_t EventId;
	float Features[30];
	float Weight;
	int Label;


	std::vector<int> Edges;
};

struct PackedSample
{
	uint32_t EventId;
	float Weight;
	int Label;
	float __padding;
	float Features[32]; //This is now 16 byte alingned.
	
};



struct EdgeElement
{
	uint32_t First;
	uint32_t Second;
	int Previous;
	int Next;
};
struct DistanceKey
{
	uint32_t FirstNode;
	uint32_t SecondNode;
	float Distance;
};


// TODO: reference additional headers your program requires here
