// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include <stdio.h>
#include <tchar.h>
#include <armadillo>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Common.h"
#include <ctime>
#include <kgraph.h>
#include <kgraph-data.h>
#include "KNN.h"
#define KNUM 3

// TODO: reference additional headers your program requires here
struct WrappedSample
{
	RawSample Sample;

	//Extra
	arma::fmat MatrixVectors;
	RawSample* Targets[KNUM];
};

struct TriggerEntry
{
	/*Corresponds to i,j,l respectively*/
	uint32_t Center, Target, Impostor; 
};