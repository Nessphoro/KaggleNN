// Mahanalobis.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//Seperate into signals and backgrounds
//Find KNN of the same type for each sample
//Run algorithm

std::vector<RawSample> loadSamples(char* file, bool indexify)
{
	float weights[30] = { 2.231676, 2.618324, 0.4098572, 1.973202, 0, 0.7555036, 0.01353469, 0.1752428, 0.8146871, 0.215681, 0.05175544, 0.1973748, 0, 1.964669, 0, 0, 0.02905582, 0, 0, 1.286265, 0.005125745, 0.08013587, 1.141656, 0, 0, 0, 0.05508045, 0, 0.08856917, 0 };
	FILE* samplesFiles = fopen(file, "rb");
	std::vector<RawSample> samples;
	samples.reserve(250000);
	if (samplesFiles)
	{
		RawSample sample;
		while (fread(&sample, 132, 1, samplesFiles))
		{
			if (indexify)
				sample.EventId -= 100000; //Convet Id to index;
			for (int i = 0; i < 30; i++)
			{
				//sample.Features[i] *= weights[i];
			}
			samples.push_back(sample); //The data is stored in such a way that WrappedSamples can be directly loaded
		}
	}
	else
	{
		std::cout << "Can't load " << file << std::endl;
		abort();
	}
	return samples;
}

float distance(WrappedSample* x, WrappedSample* y)
{
	float accumulator[32];
	float distance = 0;
	for (int i = 0; i < 30; i++)
	{
		accumulator[i] = x->Sample.Features[i] - y->Sample.Features[i];
	}

	//If done properly this should be 4(8) instructions
	for (int i = 0; i < 30; i++)
	{
		distance += accumulator[i] * accumulator[i];
	}

	return distance;
}


float distance(RawSample* x, RawSample* y)
{
	float accumulator[32];
	float distance = 0;
	for (int i = 0; i < 30; i++)
	{
		accumulator[i] = x->Features[i] - y->Features[i];
	}

	//If done properly this should be 4(8) instructions
	for (int i = 0; i < 30; i++)
	{
		distance += accumulator[i] * accumulator[i];
	}

	return distance;
}


arma::fmat mat_C(WrappedSample* x, WrappedSample* y)
{
	return (x->MatrixVectors - y->MatrixVectors) * arma::trans((x->MatrixVectors - y->MatrixVectors));
}

std::vector<TriggerEntry> subtractVector(std::vector<TriggerEntry>& x, std::vector<TriggerEntry>& y){ //remove commom elements leaving x
	std::unordered_set<uint64_t> observed(x.size());
	std::vector<TriggerEntry> output(x.size());
	for (auto entry : y)
	{
		observed.insert(entry.Center ^ (entry.Impostor << 19) ^ (entry.Target << (19 * 2)));
	}
	for (auto entry : x)
	{
		uint64_t hash = entry.Center ^ (entry.Impostor << 19) ^ (entry.Target << (19 * 2));
		if (observed.count(hash) == 0)
			output.push_back(entry);
	}

	return output;
}

std::vector<TriggerEntry> setUnion(std::vector<TriggerEntry>& x, std::vector<TriggerEntry>& y)
{
	std::unordered_set<uint64_t> observed(x.size());
	std::vector<TriggerEntry> output(x.size());
	for (auto entry : x)
	{
		output.push_back(entry);
		observed.insert(entry.Center ^ (entry.Impostor << 19) ^ (entry.Target << (19 * 2)));
	}
	for (auto entry : y)
	{
		uint64_t hash = entry.Center ^ (entry.Impostor << 19) ^ (entry.Target << (19 * 2));
		if (observed.count(hash) == 0)
			output.push_back(entry);
	}

	return output;
}


void ComputeExactly(std::vector<TriggerEntry>& triggers, std::vector<WrappedSample>& samples, KNN& helper)
{
	for (auto sample : samples)
	{
		for (int i = 0; i < KNUM; i++)
		{
			WrappedSample target = samples[sample.Targets[i]->EventId - 100000];
			float d = distance(&sample, &target);
			std::vector<WrappedSample> impostors;
			impostors.clear();
			helper.GetImpostors(sample, d, impostors);
			if (impostors.size() != 0)
			{
				TriggerEntry trigger;
				trigger.Center = sample.Sample.EventId;
				trigger.Target = target.Sample.EventId;
				for (auto impostor : impostors)
				{
					trigger.Impostor = impostor.Sample.EventId;
					triggers.push_back(trigger);
				}
			}
		}
	}
}

void SetTargets(std::vector<WrappedSample>& samples, KNN& helper)
{
	for (auto sample : samples)
	{
		WrappedSample targets[KNUM];
		helper.GetTargetNeighbours(sample, targets);
		for (int i = 0; i < KNUM; i++)
		{
			samples[sample.Sample.EventId - 100000].Targets[i] = &samples[targets[i].Sample.EventId - 100000].Sample;
		}
	}
}

void GetNewSet(std::vector<TriggerEntry>& triggers, std::vector<TriggerEntry>& newSet, std::vector<WrappedSample>& samples, KNN& helper)
{
	for (auto trigger : triggers)
	{
		WrappedSample center, target, impostor;
		center = samples[trigger.Center - 100000];
		target = samples[trigger.Target - 100000];
		impostor = samples[trigger.Impostor - 100000];

		float ct = distance(&center, &target);
		float ci = distance(&center, &impostor);
	}
}

arma::fmat updateGradient(arma::fmat gt, std::vector<TriggerEntry> nt, std::vector<TriggerEntry> nt1, std::vector<WrappedSample> wrapped)
{
	std::vector<TriggerEntry> removed = subtractVector(nt, nt1);
	std::vector<TriggerEntry> added = subtractVector(nt1, nt);
	for (auto sample : removed)
	{
		gt -= 0.5*(mat_C(&wrapped[sample.Center], &wrapped[sample.Impostor]) - mat_C(&wrapped[sample.Center], &wrapped[sample.Target]));

	}
	for (auto sample : added)
	{
		gt += 0.5*(mat_C(&wrapped[sample.Center], &wrapped[sample.Impostor]) - mat_C(&wrapped[sample.Center], &wrapped[sample.Target]));

	}
	return gt;
}


int _tmain(int argc, _TCHAR* argv[])
{
	std::vector<RawSample> samples = loadSamples("samples.bin", false); //TODO: load
	std::vector<WrappedSample> wrapped;
	for (auto sample : samples)
	{
		WrappedSample ws;
		ws.Sample = sample;
		wrapped.push_back(ws);
	}

	std::vector<TriggerEntry> triggers; //set N
	KNN classifier;

	for (auto v = wrapped.begin(); v != wrapped.end(); v++)
	{
		v->MatrixVectors = arma::fmat(30, 1);
		memcpy(v->MatrixVectors.memptr(), v->Sample.Features, sizeof(float) * 30);
	}
	/*
	for (auto v = wrapped.begin(); v != wrapped.end(); v++)
	{
		WrappedSample& sample = *v;
		RawSample temp[3];
		GetKClosest(&sample.Sample, samples, temp);
		sample.Targets[0] = &samples[temp[0].EventId - 100000];
		sample.Targets[1] = &samples[temp[1].EventId - 100000];
		sample.Targets[2] = &samples[temp[2].EventId - 100000];
	}
	*/
	
	classifier.Load(wrapped);
	arma::fmat Identity(30, 30);
	Identity.fill(arma::fill::eye);
	classifier.Rebuild(Identity);
	std::clock_t start;
	double duration;

	start = std::clock();
	//Build the set
	
	SetTargets(wrapped, classifier);
	//Main loop
	int counter = 0;
	std::vector<TriggerEntry> oldSet, newSet, active, removed, added;
	arma::fmat metric(30, 30);
	arma::fmat gradient(30, 30);


	//Compute the initial gradient
	const float mew = 0.5;
	for (auto sample : wrapped)
	{
		gradient += 0.5* mat_C(&sample, &wrapped[sample.Targets[0]->EventId - 100000]);
		gradient += 0.5* mat_C(&sample, &wrapped[sample.Targets[1]->EventId - 100000]);
		gradient += 0.5* mat_C(&sample, &wrapped[sample.Targets[2]->EventId - 100000]);
	}

	while (true)
	{
		
		if (counter % 15 == 0)
		{
			oldSet = newSet;
			ComputeExactly(active, wrapped, classifier);
			newSet = active; 
			
		}
		else
		{

		}
		updateGradient(gradient, subtractVector(oldSet, newSet), subtractVector(newSet, oldSet), wrapped);
		classifier.Rebuild(metric);
	}
	
	return 0;
}
