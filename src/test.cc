//
// Created by mikhail on 11/30/20.
//

#include "test.h"

#include "TLorentzVector.h"
#include "TDatabasePDG.h"

#include <AnalysisTree/DataHeader.hpp>

TASK_IMPL(Test)

boost::program_options::options_description Test::GetBoostOptions() {
  using namespace boost::program_options;
  options_description desc(GetName() + " options");
  return desc;
}

void Test::PreInit() {
//  SetInputBranchNames({tracks_branch_, "event_header"});
}

void Test::UserInit(std::map<std::string, void *> &Map) {
  std::vector<float> pt_axis{0, 0.29375, 0.35625, 0.41875, 0.48125, 0.54375, 0.61875, 0.70625, 0.81875, 1.01875, 2.0};
  v1_pos_analysis_bins_ = new TProfile( "v1_pos_analysis_bins_", "FW-analysis", pt_axis.size()-1, pt_axis.data() );
  v1_neg_analysis_bins_ = new TProfile( "v1_neg_analysis_bins_", "BW-analysis", pt_axis.size()-1, pt_axis.data() );
  v1_pos_uniform_bins_ = new TProfile( "v1_pos_uniform_bins_", "FW-uniform", 14, 0.0, 1.4 );
  v1_neg_uniform_bins_ = new TProfile( "v1_neg_uniform_bins_", "BW-uniform", 14, 0.0, 1.4);

  sim_tracks_ = GetInBranch("sim_tracks");
  event_header_ = GetInBranch("event_header");
  sim_header_ = GetInBranch("sim_header");

  sim_tracks_->GetConfig().Print();
  event_header_->GetConfig().Print();
  sim_header_->GetConfig().Print();

}

void Test::UserExec() {
  using AnalysisTree::Track;

  auto pdg_code = GetVar("sim_tracks/pid");
  auto is_primary = GetVar("sim_tracks/is_primary");
  auto psi_rp = GetVar("sim_header/reaction_plane");
  auto impact_parameter = GetVar("sim_header/impact_parameter");
  auto centrality = GetVar("event_header/selected_tof_rpc_hits_centrality");
  auto beam_rapidity = data_header_->GetBeamRapidity();

  auto rp = (*sim_header_)[psi_rp].GetVal();
  auto b = (*sim_header_)[impact_parameter].GetVal();
  auto cent = (*event_header_)[centrality].GetVal();

  if( cent < 20 )
    return;
  if( cent > 25 )
    return;

  for( auto track : sim_tracks_->Loop() ){
    auto pid = track[pdg_code].GetInt();
    if( pid != 2212 ) {
      continue;
    }
    auto primary = track[is_primary].GetBool();
    if( !primary )
      continue;
    auto mom4 = track.DataT<Track>()->Get4Momentum(pid);
    auto y = mom4.Rapidity()-beam_rapidity;
    if( fabs(y) < 0.15 )
      continue;
    if( fabs(y) > 0.25 )
      continue;
    auto phi = mom4.Phi();
    auto delta_phi = phi - rp;
    if( y > 0.0 ){
      v1_pos_uniform_bins_->Fill(  mom4.Pt(), -cos(delta_phi) );
      v1_pos_analysis_bins_->Fill(  mom4.Pt(), -cos(delta_phi) );
    }
    if( y < 0.0 ){
      v1_neg_uniform_bins_->Fill(  mom4.Pt(), cos(delta_phi) );
      v1_neg_analysis_bins_->Fill(  mom4.Pt(), cos(delta_phi)  );
    }
  }
}
void Test::UserFinish() {
  v1_pos_analysis_bins_->Write();
  v1_pos_uniform_bins_->Write();
  v1_neg_analysis_bins_->Write();
  v1_neg_uniform_bins_->Write();
}
