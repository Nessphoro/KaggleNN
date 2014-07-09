#include "main.h"
#include <vector>
struct RawSample
{
	unsigned int EventId;
	float DER_mass_MMC;
	float DER_mass_transverse_met_lep;
	float DER_mass_vis;
	float DER_pt_h;
	float DER_deltaeta_jet_jet;
	float DER_mass_jet_jet;
	float DER_prodeta_jet_jet;
	float DER_deltar_tau_lep;
	float DER_pt_tot;
	float DER_sum_pt;
	float DER_pt_ratio_lep_tau;
	float DER_met_phi_centrality;
	float DER_lep_eta_centrality;
	float PRI_tau_pt;
	float PRI_tau_eta;
	float PRI_tau_phi;
	float PRI_lep_pt;
	float PRI_lep_eta;
	float PRI_lep_phi;
	float PRI_met;
	float PRI_met_phi;
	float PRI_met_sumet;
	int   PRI_jet_num;
	float PRI_jet_leading_pt;
	float PRI_jet_leading_eta;
	float PRI_jet_leading_phi;
	float PRI_jet_subleading_pt;
	float PRI_jet_subleading_eta;
	float PRI_jet_subleading_phi;
	float PRI_jet_all_pt;
	float Weight;
	char Label;
};

struct DistanceInfo
{
	unsigned int FirstNode;
	unsigned int SecondNode;
	float Distance;
};

int main(int argc, char** argv)
{
	//Code to precompute Distances
	RawSample* samples=new RawSample[250000];
	std::cout << "Initializing reader\n";
	io::CSVReader<33> reader("training.csv");
	reader.read_header(io::ignore_extra_column,"EventId","DER_mass_MMC","DER_mass_transverse_met_lep","DER_mass_vis","DER_pt_h","DER_deltaeta_jet_jet","DER_mass_jet_jet",
						"DER_prodeta_jet_jet","DER_deltar_tau_lep","DER_pt_tot","DER_sum_pt","DER_pt_ratio_lep_tau","DER_met_phi_centrality","DER_lep_eta_centrality",
						"PRI_tau_pt","PRI_tau_eta","PRI_tau_phi","PRI_lep_pt","PRI_lep_eta","PRI_lep_phi","PRI_met","PRI_met_phi","PRI_met_sumet","PRI_jet_num",
						"PRI_jet_leading_pt","PRI_jet_leading_eta","PRI_jet_leading_phi","PRI_jet_subleading_pt","PRI_jet_subleading_eta","PRI_jet_subleading_phi","PRI_jet_all_pt","Weight","Label");
	RawSample buffer; 
	std::cout<<"Begin loading\n";
	while(reader.read_row(buffer.EventId,buffer.DER_mass_MMC,buffer.DER_mass_transverse_met_lep,buffer.DER_mass_vis,buffer.DER_pt_h,buffer.DER_deltaeta_jet_jet,buffer.DER_mass_jet_jet,buffer.DER_prodeta_jet_jet,buffer.DER_deltar_tau_lep,buffer.DER_pt_tot,buffer.DER_sum_pt,buffer.DER_pt_ratio_lep_tau,buffer.DER_met_phi_centrality,buffer.DER_lep_eta_centrality,buffer.PRI_tau_pt,buffer.PRI_tau_eta,buffer.PRI_tau_phi,buffer.PRI_lep_pt,buffer.PRI_lep_eta,buffer.PRI_lep_phi,buffer.PRI_met,buffer.PRI_met_phi,buffer.PRI_met_sumet,buffer.PRI_jet_num,buffer.PRI_jet_leading_pt,buffer.PRI_jet_leading_eta,buffer.PRI_jet_leading_phi,buffer.PRI_jet_subleading_pt,buffer.PRI_jet_subleading_eta,buffer.PRI_jet_subleading_phi,buffer.PRI_jet_all_pt,buffer.Weight,buffer.Label))
	{
		std::cout<<buffer.EventId<<std::endl;
		samples[buffer.EventId-100000]=buffer;
	}
	std::cout<<samples[0].EventId<<std::endl;
	std::cout<<samples[1].EventId<<std::endl;
	std::cout<<"Done loading\n";

}
