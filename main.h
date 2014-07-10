#pragma once
#include <stdio.h>
#include <iostream>
#include "csv.h"
#include <stdint.h>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

struct RawSample
{
	uint32_t EventId;
	float Features[30];
	float Weight;
	char Label;
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
	uint32_t FirstNode:19;
	uint32_t SecondNode:19;

	bool operator==(const DistanceKey& other) const
	{
		uint64_t fN,sN,oFn,oSn;
		fN=FirstNode;
		sN=SecondNode;
		oFn=other.FirstNode;
		oSn=other.SecondNode;
		return ((fN<<32)|sN) == ((oFn<<32)|oSn);
	}
};
#pragma pack(pop)
