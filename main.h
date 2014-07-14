

#pragma once


#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <stdint.h>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <iostream>
#include <fstream>


struct RawSample
{
	uint32_t EventId;
	float Features[30];
	float Weight;
	int Label;


	std::unordered_set<uint32_t>* Map; //Using a raw pointer here because the parent object is never out of scope
};

//Conserve RAM with this hack
#pragma pack(push)
#pragma pack(1)
/*
struct DistanceInfo
{
uint32_t FirstNode:19;
uint32_t SecondNode:19;
uint32_t Distance:10;
};
*/
struct DistanceKey
{
	uint32_t FirstNode : 19;
	uint32_t SecondNode : 19;

	bool operator==(const DistanceKey& other) const
	{
		uint64_t fN, sN, oFn, oSn;
		fN = FirstNode;
		sN = SecondNode;
		oFn = other.FirstNode;
		oSn = other.SecondNode;
		return ((fN << 32) | sN) == ((oFn << 32) | oSn);
	}

	bool operator<(const DistanceKey& other) const
	{
		uint64_t fN, sN, oFn, oSn;
		fN = FirstNode;
		sN = SecondNode;
		oFn = other.FirstNode;
		oSn = other.SecondNode;
		return ((fN << 32) | sN) < ((oFn << 32) | oSn);
	}
};
#pragma pack(pop)


// TODO: reference additional headers your program requires here
