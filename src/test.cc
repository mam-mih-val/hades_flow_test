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
  std::cout << "Test::UserInit(): CALL" << std::endl;
  std::vector<double> pt_axis{0.0, 0.29375, 0.35625, 0.41875, 0.48125, 0.54375, 0.61875, 0.70625, 0.81875, 1.01875, 2.0};
  std::vector<double> y_axis;
  float y=-0.75;
  while( y < 0.76 ){
    y_axis.push_back(y);
    y+=0.1;
  }
  v1_non_unifrom_binning_ = new TProfile( "v1_non_uniform", ";p_{T} [GeV/c];v_{1}", pt_axis.size()-1, pt_axis.data() );
  v1_unifrom_binning_ = new TProfile( "v1_uniform", ";p_{T} [GeV/c];v_{1}", 16, 0.0, 1.6 );

  sim_tracks_ = GetInBranch("sim_tracks");
  reco_tracks_ = GetInBranch("mdc_vtx_tracks");
  event_header_ = GetInBranch("event_header");
  sim_header_ = GetInBranch("sim_header");

  pdg_code = GetVar("mdc_vtx_tracks/pid");
  dca_z_ = GetVar("mdc_vtx_tracks/dca_z");
  dca_xy_ = GetVar("mdc_vtx_tracks/dca_xy");
  chi2_ = GetVar("mdc_vtx_tracks/chi2");
  is_primary = GetVar("sim_tracks/is_primary");
  psi_rp = GetVar("sim_header/reaction_plane");
  impact_parameter = GetVar("sim_header/impact_parameter");
  centrality = GetVar("event_header/selected_tof_rpc_hits_centrality");

  sim_tracks_->GetConfig().Print();
  event_header_->GetConfig().Print();
  sim_header_->GetConfig().Print();
  std::cout << "Test::UserInit(): RETURN" << std::endl;
}

void Test::UserExec() {
  using AnalysisTree::Track;

  auto beam_rapidity = data_header_->GetBeamRapidity();

  auto rp = (*sim_header_)[psi_rp].GetVal();
  auto b = (*sim_header_)[impact_parameter].GetVal();
  auto cent = (*event_header_)[centrality].GetVal();

  if( cent < 20 )
    return;
  if( cent > 25 )
    return;

  for( auto track : reco_tracks_->Loop() ){
    auto pid = track[pdg_code].GetInt();
    if( pid != 2212 ) {
      continue;
    }
    auto chi2 = track[chi2_].GetVal();
    auto dca_xy = track[dca_xy_].GetVal();
    auto dca_z = track[dca_z_].GetVal();
    if( chi2 > 100.0 )
      continue;
    if ( -10 > dca_xy || dca_xy > 10 )
      continue;
    if ( -10 > dca_z || dca_z > 10 )
      continue;
    auto mom4 = track.DataT<Track>()->Get4Momentum(pid);
    auto y = mom4.Rapidity()-beam_rapidity;
    if( y < 0.15 )
      continue;
    if (y > 0.25)
      continue;
    auto pT = mom4.Pt();
    auto phi = mom4.Phi();
    auto delta_phi = phi - rp;
    v1_unifrom_binning_->Fill(pT, cos(delta_phi));
    v1_non_unifrom_binning_->Fill(pT, cos(delta_phi));
  }
}
void Test::UserFinish() {
  std::cout << "Test::UserFinish(): CALL" << std::endl;
  v1_unifrom_binning_->Write();
  v1_non_unifrom_binning_->Write();
  std::cout << "Test::UserFinish(): RETURN" << std::endl;
}
