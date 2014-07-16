#include "stdafx.h"
#include "DatabaseBuilder.h"
#include <immintrin.h>

DatabaseBuilder::DatabaseBuilder(std::vector<RawSample> samples)
{
	//Seperate the samples into two different sets
	for (auto begin = samples.begin(); begin != samples.end();begin++)
	{
		RawSample sample = *begin;
		PackedSample ps;
		ps.Label = sample.Label;
		ps.EventId = sample.EventId;
		ps.Weight = sample.Weight;
		memcpy(ps.Features, sample.Features, 30*4);
		if (sample.Label)
		{
			signals.push_back(ps);
		}
		else
		{
			backgrounds.push_back(ps);
		}
	}
}

inline float DatabaseBuilder::Distance(PackedSample* x, PackedSample* y)
{ 
#ifdef AVX
	//Black magic
	//But it does produce the same results as the not AVX code
	__m256 accumulator;
	__m256 x_s = _mm256_load_ps(x->Features);
	__m256 y_s = _mm256_load_ps(y->Features);
	__m256 result = _mm256_sub_ps(x_s, y_s);
	accumulator = _mm256_mul_ps(result, result);

	x_s = _mm256_load_ps(&x->Features[8]);
	y_s = _mm256_load_ps(&y->Features[8]);
	result = _mm256_sub_ps(x_s, y_s);
	result = _mm256_mul_ps(result, result);
	accumulator = _mm256_add_ps(accumulator, result);

	x_s = _mm256_load_ps(&x->Features[16]);
	y_s = _mm256_load_ps(&y->Features[16]);
	result = _mm256_sub_ps(x_s, y_s);
	result = _mm256_mul_ps(result, result);
	accumulator = _mm256_add_ps(accumulator, result);

	x_s = _mm256_load_ps(&x->Features[24]);
	y_s = _mm256_load_ps(&y->Features[24]);
	result = _mm256_sub_ps(x_s, y_s);
	result = _mm256_mul_ps(result, result);
	accumulator = _mm256_add_ps(accumulator, result);
	//We now have a vector of 8 floats

	__m256 t1 = _mm256_hadd_ps(accumulator, accumulator);
	__m256 t2 = _mm256_hadd_ps(t1, t1);
	__m128 t3 = _mm256_extractf128_ps(t2, 1);
	__m128 t4 = _mm_add_ss(_mm256_castps256_ps128(t2), t3);
	//And now we don't
	return std::sqrtf(_mm_cvtss_f32(t4));
#endif
#ifndef AVX
	//Can be autovectorized
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

	return std::sqrtf(distance);
#endif

	
}

void DatabaseBuilder::Run(const float gamma, std::vector<DistanceKey>* edges)
{
	DistanceKey edge;
#pragma omp parallel for \
	private(edge)
	for (int i = 0; i < signals.size();i++)
	{
		edge.FirstNode = signals[i].EventId;

		for (int j = 0; j < backgrounds.size(); j++)
		{
			edge.SecondNode = backgrounds[j].EventId;
			edge.Distance = Distance(&signals[i], &backgrounds[j]);
			if (edge.Distance < gamma)
			{
#pragma omp critical
				edges->push_back(edge);
			}
		}
		
		if (i % 100==0)
		{
#pragma omp critical
			std::cout << "Done:" << edges->size() <<" on Points:" << i << std::endl;
		}
	}
}

DatabaseBuilder::~DatabaseBuilder()
{
}
