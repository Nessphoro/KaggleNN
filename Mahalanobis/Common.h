#pragma once
#include <stdint.h>

struct RawSample
{
	uint32_t EventId;
	float Features[30];
	float Weight;
	int Label;
};