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
  std::vector<double> pt_axis{0, 0.29375, 0.35625, 0.41875, 0.48125, 0.54375, 0.61875, 0.70625, 0.81875, 1.01875, 2.0};
  std::vector<double> y_axis;
  float y=-0.75;
  while( y < 0.76 ){
    y_axis.push_back(y);
    y+=0.1;
  }
  v1_even_ = new TProfile2D( "v1_even_10_40pc", ";y_{cm};p_{T}",
                            y_axis.size()-1, y_axis.data(),
                            pt_axis.size()-1, pt_axis.data() );
  v1_straight_ = new TProfile2D( "v1_(y)_10_40pc", ";y_{cm};p_{T}",
                            y_axis.size()-1, y_axis.data(),
                            pt_axis.size()-1, pt_axis.data() );
  v1_reflected_ = new TProfile2D( "v1_(-y)_10_40pc", ";y_{cm};p_{T}",
                            y_axis.size()-1, y_axis.data(),
                            pt_axis.size()-1, pt_axis.data() );

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

  if( cent < 10 )
    return;
  if( cent > 40 )
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

    auto phi = mom4.Phi();
    auto delta_phi = phi - rp;

    v1_straight_->Fill( y, mom4.Pt(), cos(delta_phi) );
    v1_reflected_->Fill( -y, mom4.Pt(), cos(delta_phi) );

    v1_even_->Fill( y, mom4.Pt(), cos(delta_phi) );
    v1_even_->Fill( -y, mom4.Pt(), cos(delta_phi) );
  }
}
void Test::UserFinish() {
  v1_straight_->Write();
  v1_reflected_->Write();
  v1_even_->Write();
}
