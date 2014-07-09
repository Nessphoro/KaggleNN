#include "main.h"


int main(int argc, char** argv)
{
	std::cout << "Initializing reader";
	io::CSVReader<1> reader("training.csv"); //Only read EnventID for now
	reader.read_header(io::ignore_extra_column,"EventId");
	int id;	
	while(reader.read_row(id))
	{
		std::cout << id << std::endl;
	}

}
