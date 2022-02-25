/**
 * File              : AddTask.C
 * Author            : Anton Riedel <anton.riedel@tum.de>
 * Date              : 07.05.2021
 * Last Modified Date: 20.01.2022
 * Last Modified By  : Anton Riedel <anton.riedel@tum.de>
 */

#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>

R__ADD_INCLUDE_PATH($GRID_UTILITY_SCRIPTS)
#include "GridHelperMacros.H"

void AddTask(const char *ConfigFileName, Int_t RunNumber, Float_t CenterMin,
             Float_t CenterMax) {

  // Get the pointer to the existing analysis manager
  // via the static access method.
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    Error("AddTask.C macro", "No analysis manager to connect to.");
    return;
  }

  // Check the analysis type using the event handlers connected to the analysis
  // manager. The availability of MC handler can also be checked here.
  if (!mgr->GetInputEventHandler()) {
    Error("AddTask.C macro", "This task requires an input event handler");
    return;
  }

  // load config file
  std::fstream ConfigFile(ConfigFileName);
  nlohmann::json Jconfig = nlohmann::json::parse(ConfigFile);

  // CONFIGURE TASK BELOW THIS LINE

  // create first task here
  AliAnalysisTaskAR *task = new AliAnalysisTaskAR(Form(
      "%s_%.1f-%.1f", Jconfig["task"]["BaseName"].get<std::string>().c_str(),
      CenterMin, CenterMax));

  // set default cuts
  task->SetDefaultConfiguration();
  task->SetDefaultBinning();
  task->SetDefaultCuts(128, CenterMin, CenterMax);
  // should be includedi n default cuts
  task->SetTrackCuts(kTPCCROSSEDROWS, kFALSE);

  std::vector<std::vector<Int_t>> sc = {
      {2, 3},    {2, 4},    {2, 5},    {2, 6},    {3, 4},    {3, 5},
      {3, 6},    {4, 5},    {4, 6},    {5, 6},    {2, 3, 4}, {2, 3, 5},
      {2, 3, 6}, {2, 4, 5}, {2, 4, 6}, {3, 4, 5}, {3, 4, 6}, {4, 5, 6}};
  task->SetSymmetricCumulants(sc);

  std::vector<Double_t> ptBinning = {0.2, 0.34, 0.5, 0.7, 1.0, 2.0, 5.0};
  std::vector<Double_t> etaBinning = {-0.8, -0.4, 0., 0.4, 0.8};

  task->SetTrackBinning(kPT, ptBinning);
  task->SetTrackBinning(kETA, etaBinning);

  SetWeights(RunNumber, CenterMin, CenterMax, task, "");

  // systematic checks
  // change centrality estimator
  AliAnalysisTaskAR *task_CenSPD = dynamic_cast<AliAnalysisTaskAR *>(
      task->Clone(Form("%s_CenSPD", task->GetName())));
  task_CenSPD->SetCentralityEstimator(kSPDTRACKLETS);
  SetWeights(RunNumber, CenterMin, CenterMax, task_CenSPD, "_CenSPD");

  // open vertex cuts in z direction
  AliAnalysisTaskAR *task_Vz14 = dynamic_cast<AliAnalysisTaskAR *>(
      task->Clone(Form("%s_Vz14", task->GetName())));
  task_Vz14->SetEventCuts(kZ, -14., 14.);
  SetWeights(RunNumber, CenterMin, CenterMax, task_Vz14, "_Vz14");

  // allow more multipliticty outliers
  AliAnalysisTaskAR *task_MC16350 = dynamic_cast<AliAnalysisTaskAR *>(
      task->Clone(Form("%s_MC16350", task->GetName())));
  task_MC16350->SetMulCorCut(1.6, 350);
  SetWeights(RunNumber, CenterMin, CenterMax, task_MC16350, "_MC16350");

  // allow more centrality outliers
  AliAnalysisTaskAR *task_CC1111 = dynamic_cast<AliAnalysisTaskAR *>(
      task->Clone(Form("%s_CC1111", task->GetName())));
  task_CC1111->SetCenCorCut(1.1, 11);
  SetWeights(RunNumber, CenterMin, CenterMax, task_CC1111, "_CC1111");

  // narrow up dca XY
  AliAnalysisTaskAR *task_DcaXY22 = dynamic_cast<AliAnalysisTaskAR *>(
      task->Clone(Form("%s_DcaXY22", task->GetName())));
  task_DcaXY22->SetTrackCuts(kDCAXY, -2.2, 2.2);
  SetWeights(RunNumber, CenterMin, CenterMax, task_DcaXY22, "_DcaXY22");

  // narrow up dca Z
  AliAnalysisTaskAR *task_DcaZ24 = dynamic_cast<AliAnalysisTaskAR *>(
      task->Clone(Form("%s_DcaZ24", task->GetName())));
  task_DcaZ24->SetTrackCuts(kDCAZ, -2.4, 2.4);
  SetWeights(RunNumber, CenterMin, CenterMax, task_DcaZ24, "_DcaZ24");

  // open up number of clusters
  AliAnalysisTaskAR *task_Cluster60 = dynamic_cast<AliAnalysisTaskAR *>(
      task->Clone(Form("%s_Cluster60", task->GetName())));
  task_Cluster60->SetTrackCuts(kTPCNCLS, 60, 160);
  SetWeights(RunNumber, CenterMin, CenterMax, task_Cluster60, "_Cluster60");

  // narrow number of clusters
  AliAnalysisTaskAR *task_Cluster80 = dynamic_cast<AliAnalysisTaskAR *>(
      task->Clone(Form("%s_Cluster80", task->GetName())));
  task_Cluster80->SetTrackCuts(kTPCNCLS, 80, 160);
  SetWeights(RunNumber, CenterMin, CenterMax, task_Cluster80, "_Cluster80");

  // filterbit 1
  AliAnalysisTaskAR *task_Fb1 = dynamic_cast<AliAnalysisTaskAR *>(
      task->Clone(Form("%s_Fb1", task->GetName())));
  task_Fb1->SetTrackCuts(kDCAZ, kFALSE);
  task_Fb1->SetTrackCuts(kDCAXY, kFALSE);
  task_Fb1->SetFilterbit(1);
  SetWeights(RunNumber, CenterMin, CenterMax, task_Fb1, "_Fb1");

  // filterbit 96
  AliAnalysisTaskAR *task_Fb96 = dynamic_cast<AliAnalysisTaskAR *>(
      task->Clone(Form("%s_Fb96", task->GetName())));
  task_Fb96->SetTrackCuts(kDCAZ, kFALSE);
  task_Fb96->SetTrackCuts(kDCAXY, kFALSE);
  task_Fb96->SetFilterbit(96);
  SetWeights(RunNumber, CenterMin, CenterMax, task_Fb96, "_Fb96");

  // filterbit 768
  AliAnalysisTaskAR *task_Fb768 = dynamic_cast<AliAnalysisTaskAR *>(
      task->Clone(Form("%s_Fb768", task->GetName())));
  task_Fb768->SetTrackCuts(kDCAZ, kFALSE);
  task_Fb768->SetTrackCuts(kDCAXY, kFALSE);
  task_Fb768->SetFilterbit(768);
  SetWeights(RunNumber, CenterMin, CenterMax, task_Fb768, "_Fb768");

  // add all tasks to the analysis manager in a loop
  std::vector<AliAnalysisTaskAR *> tasks = {
      task,           task_CenSPD,  task_Vz14,   task_CC1111,
      task_MC16350,   task_DcaXY22, task_DcaZ24, task_Cluster60,
      task_Cluster80, task_Fb1,     task_Fb96,   task_Fb768};

  // CONFIGURE TASKS ABOVE THIS LINE

  // Define input/output containers:
  std::string OutputFile =
      Jconfig["task"]["GridOutputFile"].get<std::string>() + std::string(":") +
      Jconfig["task"]["OutputTDirectory"].get<std::string>();
  AliAnalysisDataContainer *cinput = nullptr;
  AliAnalysisDataContainer *coutput = nullptr;

  // loop over all tasks
  for (auto T : tasks) {
    mgr->AddTask(T);
    cout << "Added to manager: " << T->GetName() << endl;
    cinput = mgr->GetCommonInputContainer();
    coutput = mgr->CreateContainer(T->GetName(), TList::Class(),
                                   AliAnalysisManager::kOutputContainer,
                                   OutputFile.c_str());
    mgr->ConnectInput(T, 0, cinput);
    mgr->ConnectOutput(T, 1, coutput);
  }
}
