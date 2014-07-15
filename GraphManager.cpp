#include "stdafx.h"
#include "GraphManager.h"



void GraphManager::AddEdge(EdgeElement edgeElement)
{
	int nextId = edges.size();
	edgeElement.Previous = nextId - 1;
	edgeElement.Next = nextId + 1;
	edges.push_back(edgeElement);
	samples[edgeElement.First].Edges.push_back(nextId);
	samples[edgeElement.Second].Edges.push_back(nextId);
}

void GraphManager::VertexCover()
{
	//Just start with the first edge and go forward
	int edgeId = 0;
#pragma loop(hint_parallel(4))
	while (edgeId != -1)
	{
		EdgeElement& edge = edges[edgeId];
		cover.insert(edge.First);
		cover.insert(edge.Second);
		DeleteNodeEdges(edge.First, edgeId);
		DeleteNodeEdges(edge.Second, edgeId);
		edgeId = edge.Next;
	}
}

void GraphManager::Extract(std::vector<RawSample>& output)
{
	//Also used to clean memory
	for (auto sample : samples)
	{
		if (cover.count(sample.EventId) != 1)
		{
			sample.Edges.clear();
			output.push_back(sample);
		}
	}

}

void GraphManager::FixLastEdge()
{
	//Make last edge point to nothing
	edges[edges.size() - 1].Next = -1;
	edges.shrink_to_fit();
}

void GraphManager::AddSample(RawSample sample)
{

	samples.push_back(sample);
}

void GraphManager::DeleteNodeEdges(int node, int edgeId)
	{
		RawSample &n = samples[node];
		for (int e : n.Edges) {
			/* only delete edges after a certain point */
			if (e > edgeId)
			{
				/* remove edge from "linked list" */
				if (edges[e].Previous != -1 && edges[e].First != -1) {
					edges[edges[e].Previous].Next = edges[e].Next;
				}
				if (edges[e].Next != -1 && edges[e].First != -1) {
					edges[edges[e].Next].Previous = edges[e].Previous;
				}
				edges[e].First = -1;
			}
		}
	}


GraphManager::GraphManager()
{
	samples.reserve(250000);
	edges.reserve(30 * 1000 * 1000);
}

GraphManager::GraphManager(std::vector<RawSample> fromSamples)
{
	samples = fromSamples;
}

GraphManager::~GraphManager()
{
}
