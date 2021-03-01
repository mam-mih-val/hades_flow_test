//
// Created by mikhail on 11/30/20.
//

#ifndef HADES_RAPIDITY_SRC_RAPIDITY_H_
#define HADES_RAPIDITY_SRC_RAPIDITY_H_

#include <TProfile.h>

#include <at_task/UserTask.h>
#include <at_task/Task.h>

#include <AnalysisTree/BranchConfig.hpp>
#include <AnalysisTree/Detector.hpp>
#include <AnalysisTree/EventHeader.hpp>
#include <AnalysisTree/Matching.hpp>
#include <AnalysisTree/Particle.hpp>
#include <TProfile2D.h>

class Test : public UserFillTask {

public:
  void UserInit(std::map<std::string, void *> &Map) override;
  void UserExec() override;
  void UserFinish() override;
  boost::program_options::options_description GetBoostOptions() override;
  void PreInit() override;
  void PostFinish() override {
    UserTask::PostFinish();
  }

private:
  ATI2::Branch* sim_tracks_;
  ATI2::Branch* event_header_;
  ATI2::Branch* sim_header_;

  TProfile2D* v1_even_;
  TProfile2D* v1_straight_;
  TProfile2D* v1_reflected_;

TASK_DEF(Test, 0)
};

#endif // HADES_RAPIDITY_SRC_RAPIDITY_H_
