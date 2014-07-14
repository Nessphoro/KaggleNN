// CWorker.cpp : Defines the entry point for the console application.
//

#include "main.h"

namespace std {

	template <>
	struct hash<DistanceKey>
	{
		std::size_t operator()(const DistanceKey& k) const
		{
			using std::size_t;
			using std::hash;
			uint64_t prehashed = ((((uint64_t)k.FirstNode) << 32) | (uint64_t)k.SecondNode);

			prehashed ^= prehashed >> 33;
			prehashed *= 0xff51afd7ed558ccd;
			prehashed ^= prehashed >> 33;
			prehashed *= 0xc4ceb9fe1a85ec53;
			prehashed ^= prehashed >> 33;

			return prehashed;
		}
	};

}




const uint32_t Gamma = 750;
uint32_t TotalEdges = 0;

int main(int argc, char* argv[])
{
	//Data structures
	std::unordered_map<uint32_t, RawSample> signals;
	std::unordered_map<uint32_t, RawSample> backgrounds;
	std::vector<RawSample> finalSet;

	std::cout << "Loading samples\n of size:" << sizeof(RawSample);
	FILE* samplesFiles = fopen("data.bin", "rb");

	if (samplesFiles)
	{
		RawSample sample;
		while (fread(&sample, 132, 1, samplesFiles))
		{
			sample.Map = new std::unordered_set<uint32_t>();
			if (sample.Label == 1)
			{
				signals.insert(std::pair<const uint32_t, RawSample>(sample.EventId, sample));
			}
			else
			{
				backgrounds.insert(std::pair<const uint32_t, RawSample>(sample.EventId, sample));
			}
		}
	}

	fclose(samplesFiles);

	std::cout << "Initializing reader\n";
	FILE* file = fopen("distances.bin", "rb");
	file->_bufsiz = 4096 * 1024;
	unsigned char fileBuff[12];

	std::cout << "Open\n";
	int counter = 0;
	if (file)
	{
		int lc = 0;
		while (fread(fileBuff, 12, 1, file))
		{
			if (lc++ % 3 != 0)
				continue; // We only load every third sample to speed it up during debugging
			DistanceKey di;

			uint32_t distance = (int)*(float*)&fileBuff[8];
			di.FirstNode = *(uint32_t*)&fileBuff[0];
			di.SecondNode = *(uint32_t*)&fileBuff[4];
			//Load the data structures
			if (distance < Gamma)
			{
				signals.find(di.FirstNode)->second.Map->insert(di.SecondNode);
				backgrounds.find(di.SecondNode)->second.Map->insert(di.FirstNode);
				counter++;
				TotalEdges++;
				if (counter % 1000000 == 0)
					std::cout << "Loaded: " << counter << std::endl;  //INFO
			}
		}
		std::cout << "Done\n";
	}
	fclose(file);


	std::cout << "Starting minimal vertex cover algorithm\nScanning for isolated nodes...\n";
	std::vector<uint32_t> keysToRemove;
	for (std::unordered_map<uint32_t, RawSample>::iterator start = signals.begin(); start != signals.end(); start++)
	{
		if (start->second.Map->size() == 0)
		{
			keysToRemove.push_back(start->first);
		}
	}

	std::cout << "To be removed: " << keysToRemove.size() << std::endl;

	for (std::vector<uint32_t>::iterator start = keysToRemove.begin(); start != keysToRemove.end(); start++)
	{
		delete signals.find(*start)->second.Map;
		finalSet.push_back(signals.find(*start)->second);
		signals.erase(*start);
	}
	keysToRemove.clear();

	std::cout << "Scanning for isolated nodes pass 2...\n";
	for (std::unordered_map<uint32_t, RawSample>::iterator start = backgrounds.begin(); start != backgrounds.end(); start++)
	{
		if (start->second.Map->size() == 0)
		{
			keysToRemove.push_back(start->first);
		}
	}
	

	std::cout << "To be removed: " << keysToRemove.size() << std::endl;

	for (std::vector<uint32_t>::iterator start = keysToRemove.begin(); start != keysToRemove.end(); start++)
	{
		delete backgrounds.find(*start)->second.Map;
		finalSet.push_back(backgrounds.find(*start)->second);
		backgrounds.erase(*start);
	}
	keysToRemove.clear();
	//The above code clears nodes that aren't connected to anything


	//Minimum vertex cover algo is below
	while (signals.size() != 0)
	{
		std::cout << "Left: " << signals.size() << std::endl; //INFO

		RawSample signal = signals.begin()->second;
		uint32_t signalId = signal.EventId;
		uint32_t connection = *signal.Map->begin();
		RawSample background = backgrounds.find(connection)->second;

		for (std::unordered_set<uint32_t>::iterator start = signal.Map->begin(); start != signal.Map->end(); start++)
		{
			RawSample bg = backgrounds.find(*start)->second;
			bg.Map->erase(signalId);
			if (bg.Map->size() == 0)
			{
				delete bg.Map;
				bg.Map = 0;
				if (background.EventId == bg.EventId)
				{
					background.Map = 0;
				}
				finalSet.push_back(bg);
				backgrounds.erase(bg.EventId);
			}
		}

		if (background.Map)
		{
			for (std::unordered_set<uint32_t>::iterator start = background.Map->begin(); start != background.Map->end(); start++)
			{
				RawSample sg = signals.find(*start)->second;
				sg.Map->erase(connection);
				if (sg.Map->size() == 0)
				{
					delete sg.Map;
					sg.Map = 0;
					finalSet.push_back(sg);
					signals.erase(sg.EventId);
				}
			}
		}
		signals.erase(signalId);
		backgrounds.erase(connection);
	}


}

