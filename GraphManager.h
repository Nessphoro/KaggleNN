#pragma once
#include "stdafx.h"

extern struct RawSample;
extern struct EdgeElement;

class GraphManager
{
	std::vector<RawSample> samples;
	std::vector<EdgeElement> edges;
	std::unordered_set<int> cover;
public:
	GraphManager();
	GraphManager(std::vector<RawSample> fromSamples);
	~GraphManager();
	void AddEdge(EdgeElement edgeElement);
	void VertexCover();
	void Extract(std::vector<RawSample>& output);
	void FixLastEdge();
	void AddSample(RawSample sample);
private:
	void DeleteNodeEdges(int node, int edgeId);
};

