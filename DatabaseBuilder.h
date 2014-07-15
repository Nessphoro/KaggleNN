#pragma once
extern struct PackedSample;
extern struct DistanceKey;
class DatabaseBuilder
{
	std::vector<PackedSample> signals;
	std::vector<PackedSample> backgrounds;
public:
	inline float Distance(PackedSample* x, PackedSample* y);
	DatabaseBuilder(std::vector<RawSample> samples);
	void Run(const float gamma, std::vector<DistanceKey>* edges);
	~DatabaseBuilder();
};

