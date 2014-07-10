#include "main.h"


namespace std {

  template <>
  struct hash<DistanceKey>
  {
    std::size_t operator()(const DistanceKey& k) const
    {
      using std::size_t;
      using std::hash;
      uint64_t prehashed = ((((uint64_t)k.FirstNode)<<32) | (uint64_t)k.SecondNode);
/*
	prehashed ^= prehashed >> 33;
	prehashed *= 0xff51afd7ed558ccd;
	prehashed ^= prehashed >> 33;
	prehashed *= 0xc4ceb9fe1a85ec53;
	prehashed ^= prehashed >> 33;
*/
	return prehashed;
    }
  };

}

int main(int argc, char** argv)
{
	//Code to precompute Distances
	RawSample* samples=new RawSample[250000];
	std::unordered_map<DistanceKey,uint32_t> distances;
	std::cout<<sizeof(size_t)<<std::endl;
	std::cout << "Initializing reader\n";
	io::CSVReader<33> reader("training.csv");
	reader.read_header(io::ignore_extra_column,"EventId","DER_mass_MMC","DER_mass_transverse_met_lep","DER_mass_vis","DER_pt_h","DER_deltaeta_jet_jet","DER_mass_jet_jet",
						"DER_prodeta_jet_jet","DER_deltar_tau_lep","DER_pt_tot","DER_sum_pt","DER_pt_ratio_lep_tau","DER_met_phi_centrality","DER_lep_eta_centrality",
						"PRI_tau_pt","PRI_tau_eta","PRI_tau_phi","PRI_lep_pt","PRI_lep_eta","PRI_lep_phi","PRI_met","PRI_met_phi","PRI_met_sumet","PRI_jet_num",
						"PRI_jet_leading_pt","PRI_jet_leading_eta","PRI_jet_leading_phi","PRI_jet_subleading_pt","PRI_jet_subleading_eta","PRI_jet_subleading_phi","PRI_jet_all_pt","Weight","Label");
	RawSample buffer; 
	std::cout<<"Begin loading\n";
	while(reader.read_row(buffer.EventId,buffer.Features[0],buffer.Features[1],buffer.Features[2],buffer.Features[3],buffer.Features[4],
buffer.Features[5],buffer.Features[6],buffer.Features[7],buffer.Features[8],buffer.Features[9],buffer.Features[10],buffer.Features[11],buffer.Features[12],buffer.Features[13],buffer.Features[14],buffer.Features[15],buffer.Features[16],buffer.Features[17],buffer.Features[18],buffer.Features[19],buffer.Features[20],buffer.Features[21],buffer.Features[22],buffer.Features[23],buffer.Features[24],buffer.Features[25],buffer.Features[26],buffer.Features[27],buffer.Features[28],buffer.Features[29], buffer.Weight,buffer.Label))
	{
		samples[buffer.EventId-100000]=buffer;
	}
	std::cout<<samples[0].EventId<<std::endl;
	std::cout<<samples[1].EventId<<std::endl;
	std::cout<<"Done loading\nLoading Distance Database\n";
	std::ifstream db("data/y_1000.gzip",std::ios_base::in | std::ios_base::binary);
	char* fileBuff=new char[12];
	//if(db.is_open())
	//{
		std::cout<<"Open\n";
		boost::iostreams::filtering_istream in;
    		in.push(boost::iostreams::gzip_decompressor());
		in.push(db);
    		//in.push(boost::iostreams::file_descriptor_source("data/y_1000.gzip"));
		int counter=0;
		while(true)
		{
			in.read(fileBuff,12);
			if(true)
			{	
				DistanceKey di;
				uint32_t distance=(int)*(float*)&fileBuff[8];
				di.FirstNode = *(uint32_t*)&fileBuff[0];
				di.SecondNode = *(uint32_t*)&fileBuff[4];
				counter++;
				std::cout<<*(uint32_t*)&fileBuff[0]<<":"<<*(uint32_t*)&fileBuff[4]<<std::endl;
				char temp;
				std::cin>>temp;
				distances.insert(std::make_pair<DistanceKey,uint32_t>((DistanceKey)di,(uint32_t)distance));
				if(counter % 1000000 == 0)
					std::cout<<"Loaded: "<<distances.size()<<std::endl;
			}
			in.seek(counter*12);
		}
		std::cout<<"Done\n";
	//}
	//db.close();
}
