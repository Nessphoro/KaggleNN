#include "stdafx.h"
#include "KNN.h"
#include <omp.h>
#include <kgraph.h>

float KNN::mDistance(const WrappedSample x, const WrappedSample y)
{
	arma::fmat diff(30,1);

	diff = x.MatrixVectors - y.MatrixVectors;

	return  arma::as_scalar(arma::trans(diff) * KNN::metric * diff);
}



KNN::KNN()
{
	signalIndex = 0;
}

std::vector<WrappedSample> signals, backgrounds;
kgraph::VectorOracle<std::vector<WrappedSample>, WrappedSample>* signalOracle;
kgraph::VectorOracle<std::vector<WrappedSample>, WrappedSample>* backgroundOracle;

void KNN::Load(std::vector<WrappedSample>& samples)
{
	for (auto signal : samples)
	{
		if (signal.Sample.Label)
		{
			signals.push_back(signal);
		}
		else
		{
			signals.push_back(signal);
		}
	}
	signalOracle = new kgraph::VectorOracle<std::vector<WrappedSample>, WrappedSample>(signals, mDistance);
	backgroundOracle = new kgraph::VectorOracle<std::vector<WrappedSample>, WrappedSample>(backgrounds, mDistance);
}

void KNN::Rebuild(arma::fmat newM)
{
	if (signalIndex != 0)
	{
		delete signalIndex;
		delete backgroundIndex;
	}
	
	signalIndex = kgraph::KGraph::create();
	backgroundIndex = kgraph::KGraph::create();
	{
		kgraph::KGraph::IndexParams params;
		signalIndex->build(*signalOracle, params, 0);
		backgroundIndex->build(*backgroundOracle, params, 0);
	}

}

void KNN::GetImpostors(WrappedSample& sample, float range, std::vector<WrappedSample>& impostors)
{
	kgraph::KGraph::SearchParams radiusSearch;
	radiusSearch.epsilon = range;
	radiusSearch.K = 250000; //Max Result
	std::vector<unsigned int> nn(2000); //Probably won't be over than couple of thousand
	if (sample.Sample.Label)
	{
		backgroundIndex->search(signalOracle->query(sample), radiusSearch, &nn[0], 0);
	}
	else
	{
		signalIndex->search(signalOracle->query(sample), radiusSearch, &nn[0], 0);
	}
	for (int i = 0; i < nn.size(); i++)
	{
		if (sample.Sample.Label)
		{
			impostors.push_back(backgrounds[nn[i]]);
		}
		else
		{
			impostors.push_back(signals[nn[i]]);
		}
	}
	
}

void KNN::GetTargetNeighbours(WrappedSample& sample, WrappedSample targets[])
{
	kgraph::KGraph::SearchParams normalSearch;
	normalSearch.K = KNUM + 1; //Max Result
	std::vector<unsigned int> nn(KNUM + 1);
	if (sample.Sample.Label)
	{
		signalIndex->search(signalOracle->query(sample), normalSearch, &nn[0], 0);
	}
	else
	{
		backgroundIndex->search(signalOracle->query(sample), normalSearch, &nn[0], 0);
	}
	int counter = 0;
	for (int i = 0; i < nn.size(); i++)
	{
		if (sample.Sample.Label)
		{
			WrappedSample candiate = signals[nn[i]];
			if (candiate.Sample.EventId != sample.Sample.EventId)
			{
				targets[counter++] = candiate;
			}
			
		}
		else
		{
			WrappedSample candiate = backgrounds[nn[i]];
			if (candiate.Sample.EventId != sample.Sample.EventId)
			{
				targets[counter++] = candiate;
			}
		}
	}
}

KNN::~KNN()
{
}
