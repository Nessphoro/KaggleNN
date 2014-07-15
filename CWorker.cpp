// CWorker.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "Tester.h"

enum Commands
{
	help,database,index
};

Commands toCommand(std::string& str)
{
	if (str == "help")
		return help;
	else if (str == "database")
		return database;
	else if (str == "index")
		return index;
}


std::vector<RawSample> LoadSamples(char* file,bool indexify)
{
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
			samples.push_back(sample); //The data is stored in such a way that RawSamples can be directly loaded
		}
	}
	else
	{
		std::cout << "Can't load "<<file<<std::endl;
		abort();
	}
	return samples;
}

void GenerateDatabase(float gamma)
{
	
	DatabaseBuilder db(LoadSamples("samples.bin",false));
	std::vector<DistanceKey> edges;
	edges.reserve(100 * 1000 * 1000);

	db.Run(gamma, &edges);
	edges.shrink_to_fit();
	//Write out
	DistanceKey* edgesBuffer = new DistanceKey[edges.size()];
	int counter = 0;
	for (auto edge : edges)
	{
		edgesBuffer[counter++] = edge;
	}

	FILE* output = fopen("database.bin", "wb");
	fwrite(edgesBuffer, sizeof(DistanceKey), edges.size(), output);
	fclose(output);
	delete edgesBuffer;
}

void GenerateIndex(float gamma)
{
	
	std::vector<RawSample> finalSet;
	finalSet.reserve(250 * 1000);
	std::cout << "Loading samples\n";
	
	GraphManager manager(LoadSamples("samples.bin", true));

	std::cout << "Initializing edges\n";
	FILE* file = fopen("database.bin", "rb");
	file->_bufsiz = 4096 * 1024;
	unsigned char fileBuff[12];

	std::cout << "Open\n";
	int counter = 0;
	if (file)
	{
		while (fread(fileBuff, 12, 1, file))
		{
			EdgeElement di;

			uint32_t distance = (int)*(float*)&fileBuff[8];
			di.First = *(uint32_t*)&fileBuff[0] - 100000; //Also convert Id to index
			di.Second = *(uint32_t*)&fileBuff[4] - 100000;
			if (distance < gamma)
			{
				manager.AddEdge(di);
				counter++;
				if (counter % 1000000 == 0)
					std::cout << "Loaded: " << counter << std::endl;
			}
		}
		std::cout << "Done\n";
	}
	manager.FixLastEdge();
	fclose(file);
	manager.VertexCover();
	manager.Extract(finalSet);
	uint32_t* ids = new uint32_t[finalSet.size()];
	counter = 0;

	for (auto sample : finalSet)
	{
		ids[counter++] = sample.EventId + 100000; //Save index, convert Id back to canonicial form
	}

	FILE* output = fopen("index.bin", "wb");
	fwrite(ids, 4, finalSet.size(), output);
	fclose(output);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage cworker [command] [param1] [param2] ...." << std::endl;
		std::cout << "For a list of commands use the 'help' command" << std::endl;
		return 0;
	}
	switch (toCommand(std::string(argv[1])))
	{
	case help:
		std::cout << "Commands:" << std::endl;
		std::cout << "help \n\t Print this message" << std::endl;
		std::cout << "database [gamma] \n\t Generates a precomputed edges database with a specific gamma" << std::endl;
		std::cout << "index [gamma] \n\t Generates a graph indexing file which is used to load the correct samples. Gamma can be adjusted, but must not be bigger than the one the database was generated with"<<std::endl;
		break;
	case database:
		if (argc == 3)
		{
			float gamma = atof(argv[2]);
			std::cout << "Generating database with gamma = " << gamma << std::endl;
			GenerateDatabase(gamma);
		}
		else
		{
			std::cout << "You must supply a gamma" << std::endl;
		}
		break;
	case index:
		if (argc == 3)
		{
			float gamma = atof(argv[2]);
			std::cout << "Generating index with gamma = " << gamma << std::endl;
			GenerateIndex(gamma);
		}
		else
		{
			std::cout << "You must supply a gamma" << std::endl;
		}
		break;
		break;
	default:
		std::cout << "Not a valid command" << std::endl;
		break;
	}

}

