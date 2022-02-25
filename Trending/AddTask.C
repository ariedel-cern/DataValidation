/**
 * File              : AddTask.C
 * Author            : Anton Riedel <anton.riedel@tum.de>
 * Date              : 07.05.2021
 * Last Modified Date: 18.01.2022
 * Last Modified By  : Anton Riedel <anton.riedel@tum.de>
 */

#include <fstream>
#include <nlohmann/json.hpp>

void AddTask(const char *ConfigFileName, Float_t CenterMin, Float_t CenterMax) {

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

  // trend two particle correlators to show that the result is stable
  std::vector<std::vector<Int_t>> correlators = {
      {-1, 1}, {-2, 2}, {-3, 3}, {-4, 4}, {-5, 5}, {-6, 6},
  };
  task->SetCorrelators(correlators);

  // binning for correlators
  std::vector<Double_t> ptBinning = {0.2, 0.34, 0.5, 0.7, 1.0, 2.0, 5.0};
  task->SetTrackBinning(kPT, ptBinning);
  std::vector<Double_t> etaBinning = {-0.8, -0.4, 0., 0.4, 0.8};
  task->SetTrackBinning(kETA, etaBinning);

  // add all tasks to the analysis manager in a loop
  std::vector<AliAnalysisTaskAR *> tasks = {task};

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
