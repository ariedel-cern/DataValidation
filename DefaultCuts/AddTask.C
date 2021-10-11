/**
 * File              : AddTask.C
 * Author            : Anton Riedel <anton.riedel@tum.de>
 * Date              : 07.05.2021
 * Last Modified Date: 11.10.2021
 * Last Modified By  : Anton Riedel <anton.riedel@tum.de>
 */

// AddTask* macro for flow analysis
// Creates a Flow Event task and adds it to the analysis manager.
// Sets the cuts using the correction framework (CORRFW) classes.
// Also creates Flow Analysis tasks and connects them to the output of the flow
// event task.

void AddTask(Float_t centerMin = 0., Float_t centerMax = 100.) {

  TString OutputFile(std::getenv("GRID_OUTPUT_ROOT_FILE"));

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

  // task with default cuts
  AliAnalysisTaskAR *defaultTask = new AliAnalysisTaskAR(
      Form("%s_%.1f-%.1f", std::getenv("TASK_BASENAME"), centerMin, centerMax));
  defaultTask->SetDefaultConfiguration();
  defaultTask->SetDefaultBinning();
  defaultTask->SetDefaultCuts(128, centerMin, centerMax);
  defaultTask->SetFillQACorHistogramsOnly(kTRUE);

  // task without centrality correlation cuts
  AliAnalysisTaskAR *noCenCorCuts =
      dynamic_cast<AliAnalysisTaskAR *>(defaultTask->Clone(
          Form("%s_NOCENCOR_%.1f-%.1f", std::getenv("TASK_BASENAME"), centerMin,
               centerMax)));
  noCenCorCuts->SetCenCorCut(kFALSE);

  // task without multiplicity correlation cut
  AliAnalysisTaskAR *noMulCorCuts =
      dynamic_cast<AliAnalysisTaskAR *>(defaultTask->Clone(
          Form("%s_NOMULCOR_%.1f-%.1f", std::getenv("TASK_BASENAME"), centerMin,
               centerMax)));
  noMulCorCuts->SetMulCorCut(kFALSE);

  // task without centrality and multiplicity correlation cut
  AliAnalysisTaskAR *noCenMulCorCuts =
      dynamic_cast<AliAnalysisTaskAR *>(defaultTask->Clone(
          Form("%s_NOCENMULCOR_%.1f-%.1f", std::getenv("TASK_BASENAME"),
               centerMin, centerMax)));
  noCenMulCorCuts->SetCenCorCut(kFALSE);
  noCenMulCorCuts->SetMulCorCut(kFALSE);

  // add all tasks to the analysis manager in a loop
  std::vector<AliAnalysisTaskAR *> tasks = {defaultTask, noCenCorCuts,
                                            noMulCorCuts, noCenMulCorCuts};
  // Define input/output containers:
  OutputFile += TString(":") + TString(std::getenv("OUTPUT_TDIRECTORY_FILE"));
  AliAnalysisDataContainer *cinput = nullptr;
  AliAnalysisDataContainer *coutput = nullptr;

  // loop over all tasks
  for (auto T : tasks) {
    mgr->AddTask(T);
    cout << "Added to manager: " << T->GetName() << endl;
    cinput = mgr->GetCommonInputContainer();
    coutput =
        mgr->CreateContainer(T->GetName(), TList::Class(),
                             AliAnalysisManager::kOutputContainer, OutputFile);
    mgr->ConnectInput(T, 0, cinput);
    mgr->ConnectOutput(T, 1, coutput);
  }
}
