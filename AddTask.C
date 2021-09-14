/**
 * File              : AddTask.C
 * Author            : Anton Riedel <anton.riedel@tum.de>
 * Date              : 07.05.2021
 * Last Modified Date: 14.09.2021
 * Last Modified By  : Anton Riedel <anton.riedel@tum.de>
 */

// AddTask* macro for flow analysis
// Creates a Flow Event task and adds it to the analysis manager.
// Sets the cuts using the correction framework (CORRFW) classes.
// Also creates Flow Analysis tasks and connects them to the output of the flow
// event task.

void AddTask(Float_t centerMin = 0., Float_t centerMax = 100.,
             Bool_t bRunOverAOD = kTRUE) {

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

  // set limits for control histograms and cuts
  // transverse momentum
  Double_t pt_min = 0.2;
  Double_t pt_max = 5.0;
  Int_t pt_bins = (pt_max - pt_min) * 100;
  // azimuthal angle
  Double_t phi_min = 0.0;
  Double_t phi_max = TMath::TwoPi();
  Int_t phi_bins = 360.;
  // pseudo rapidity
  Double_t eta_min = -0.8;
  Double_t eta_max = 0.8;
  Int_t eta_bins = (eta_max - eta_min) * 100;
  // charge
  Double_t charge_min = -2.5;
  Double_t charge_max = 2.5;
  Int_t charge_bins = (charge_max - charge_min) * 1;
  // number of clusters in the TPC
  Double_t tpcncls_min = 60.;
  Double_t tpcncls_max = 159.;
  Int_t tpcncls_bins = tpcncls_max - tpcncls_min;
  // number of clusters in the ITS
  Double_t itsncls_min = 0.;
  Double_t itsncls_max = 10.;
  Int_t itsncls_bins = itsncls_max - itsncls_min;
  // chi2/NDF of the track fit
  Double_t chi2perndf_min = 0.9;
  Double_t chi2perndf_max = 4.5;
  Int_t chi2perndf_bins = (chi2perndf_max - chi2perndf_min) * 100;
  // distance of closest approach in Z direction
  Double_t dcaz_min = -3.2;
  Double_t dcaz_max = 3.2;
  Int_t dcaz_bins = (dcaz_max - dcaz_min) * 10;
  // distance of closest approach in XY plane
  Double_t dcaxy_min = -2.4;
  Double_t dcaxy_max = 2.4;
  Int_t dcaxy_bins = (dcaxy_max - dcaxy_min) * 10;
  // multiplicity, estimated by number of tracks per event
  Double_t mul_min = 0.;
  Double_t mul_max = 10000.;
  Int_t mul_bins = (mul_max - mul_min) / 10;
  // multiplicity, estimated by number of tracks per event that survive track
  // cuts this is also the number of tracks we fill into qvector
  Double_t mulq_min = 2.;
  Double_t mulq_max = 3000.;
  Int_t mulq_bins = (mulq_max - mulq_min) / 10;
  // multiplicity, estimated by sum of all paritcle weights
  Double_t mulw_min = 2.;
  Double_t mulw_max = 3000.;
  Int_t mulw_bins = (mulw_max - mulw_min) / 10;
  // reference multiplicity from AODHeadter
  // reference multiplicity is set to -999 for MC
  Double_t mulref_min = 2;
  Double_t mulref_max = 3000;
  Int_t mulref_bins = (mulref_max - mulref_min) / 10;
  // multiplicity, estimated by number of contributor to primary vertex
  Double_t ncontrib_min = 0.;
  Double_t ncontrib_max = 3000.;
  Int_t ncontrib_bins = (ncontrib_max - ncontrib_min) / 10;
  // centrality
  Double_t cen_min = centerMin;
  Double_t cen_max = centerMax;
  Int_t cen_bins = (cen_max - cen_min) * 1;
  // x coordinate of primary vertex
  Double_t x_min = -10.;
  Double_t x_max = 10.;
  Int_t x_bins = (x_max - x_min) * 10.;
  // y coordinate of primary vertey
  Double_t y_min = -10.;
  Double_t y_max = 10.;
  Int_t y_bins = (y_max - y_min) * 10.;
  // z coordinate of primary vertez
  Double_t z_min = -10.;
  Double_t z_max = 10.;
  Int_t z_bins = (z_max - z_min) * 10.;
  // distance of primary vertex from the origin
  Double_t pos_min = 1e-6;
  Double_t pos_max = 18.;
  Int_t pos_bins = (pos_max - pos_min) * 100.;

  // Base task without correlation cuts
  AliAnalysisTaskAR *task_noCorCuts = new AliAnalysisTaskAR(
      Form("%s_NOCOR_%.1f-%.1f", std::getenv("TASK_BASENAME"), cen_min,
           cen_max),
      kFALSE);

  // most setters expect enumerations as arguments
  // those enumerations are defined in AliAnalysisTaskAR.h

  // setters for track control histograms
  task_noCorCuts->SetTrackControlHistogramBinning(kPT, pt_bins, pt_min, pt_max);
  task_noCorCuts->SetTrackControlHistogramBinning(kPHI, phi_bins, phi_min,
                                                  phi_max);
  task_noCorCuts->SetTrackControlHistogramBinning(kETA, eta_bins, eta_min,
                                                  eta_max);
  task_noCorCuts->SetTrackControlHistogramBinning(kCHARGE, charge_bins,
                                                  charge_min, charge_max);
  task_noCorCuts->SetTrackControlHistogramBinning(kTPCNCLS, tpcncls_bins,
                                                  tpcncls_min, tpcncls_max);
  task_noCorCuts->SetTrackControlHistogramBinning(kITSNCLS, itsncls_bins,
                                                  itsncls_min, itsncls_max);
  task_noCorCuts->SetTrackControlHistogramBinning(
      kCHI2PERNDF, chi2perndf_bins, chi2perndf_min, chi2perndf_max);
  task_noCorCuts->SetTrackControlHistogramBinning(kDCAZ, dcaz_bins, dcaz_min,
                                                  dcaz_max);
  task_noCorCuts->SetTrackControlHistogramBinning(kDCAXY, dcaxy_bins, dcaxy_min,
                                                  dcaz_max);
  // setters for event control histograms
  task_noCorCuts->SetEventControlHistogramBinning(kMUL, mul_bins, mul_min,
                                                  mul_max);
  task_noCorCuts->SetEventControlHistogramBinning(kMULQ, mulq_bins, mulq_min,
                                                  mulq_max);
  task_noCorCuts->SetEventControlHistogramBinning(kMULW, mulw_bins, mulw_min,
                                                  mulw_max);
  task_noCorCuts->SetEventControlHistogramBinning(kMULREF, mulref_bins,
                                                  mulref_min, mulref_max);
  task_noCorCuts->SetEventControlHistogramBinning(kNCONTRIB, ncontrib_bins,
                                                  ncontrib_min, ncontrib_max);
  task_noCorCuts->SetEventControlHistogramBinning(kCEN, cen_bins, cen_min,
                                                  cen_max);
  task_noCorCuts->SetEventControlHistogramBinning(kX, x_bins, x_min, x_max);
  task_noCorCuts->SetEventControlHistogramBinning(kY, y_bins, y_min, y_max);
  task_noCorCuts->SetEventControlHistogramBinning(kZ, z_bins, z_min, z_max);
  task_noCorCuts->SetEventControlHistogramBinning(kVPOS, pos_bins, pos_min,
                                                  pos_max);

  task_noCorCuts->SetFillQAHistograms(kFALSE);
  task_noCorCuts->SetFillQACorHistogramsOnly(kTRUE);

  // setter for centrality correlation histograms
  for (int i = 0; i < LAST_ECENESTIMATORS; i++) {
    for (int j = i + 1; j < LAST_ECENESTIMATORS; j++) {
      task_noCorCuts->SetCenCorQAHistogramBinning(
          i, cen_bins, cen_min, cen_max, j, cen_bins, cen_min, cen_max);
    }
  }
  // setter for multiplicity correlation histograms
  Double_t MulCorMM[kMulEstimators][2] = {
      {mul_min, mul_max},           {mulq_min, mulq_max},
      {mulw_min, mulw_max},         {mulref_min, mulref_max},
      {ncontrib_min, ncontrib_max},
  };
  Int_t MulCorBin[kMulEstimators] = {mul_bins, mulq_bins, mulw_bins,
                                     mulref_bins, ncontrib_bins};
  for (int i = 0; i < kMulEstimators; i++) {
    for (int j = i + 1; j < kMulEstimators; j++) {
      task_noCorCuts->SetMulCorQAHistogramBinning(
          i, MulCorBin[i], MulCorMM[i][0], MulCorMM[i][1], j, MulCorBin[j],
          MulCorMM[j][0], MulCorMM[j][1]);
    }
  }

  // setters for track cuts
  task_noCorCuts->SetTrackCuts(kPT, pt_min, pt_max);
  task_noCorCuts->SetTrackCuts(kPHI, phi_min, phi_max);
  task_noCorCuts->SetTrackCuts(kETA, eta_min, eta_max);
  task_noCorCuts->SetTrackCuts(kCHARGE, charge_min, charge_max);
  task_noCorCuts->SetTrackCuts(kTPCNCLS, tpcncls_min, tpcncls_max);
  task_noCorCuts->SetTrackCuts(kITSNCLS, itsncls_min, itsncls_max);
  task_noCorCuts->SetTrackCuts(kCHI2PERNDF, chi2perndf_min, chi2perndf_max);
  task_noCorCuts->SetTrackCuts(kDCAZ, dcaz_min, dcaz_max);
  task_noCorCuts->SetTrackCuts(kDCAXY, dcaxy_min, dcaxy_max);
  // setters for event cuts
  task_noCorCuts->SetEventCuts(kMUL, mul_min, mul_max);
  task_noCorCuts->SetEventCuts(kMULQ, mulq_min, mulq_max);
  task_noCorCuts->SetEventCuts(kMULW, mulw_min, mulw_max);
  task_noCorCuts->SetEventCuts(kMULREF, mulref_min, mulref_max);
  task_noCorCuts->SetEventCuts(kNCONTRIB, ncontrib_min, ncontrib_max);
  task_noCorCuts->SetEventCuts(kCEN, centerMin, centerMax);
  task_noCorCuts->SetEventCuts(kX, x_min, x_max);
  task_noCorCuts->SetEventCuts(kY, y_min, y_max);
  task_noCorCuts->SetEventCuts(kZ, z_min, z_max);
  task_noCorCuts->SetEventCuts(kVPOS, pos_min, pos_max);

  // other cuts
  task_noCorCuts->SetFilterbit(128); // typical 1,92,128,256,768
  task_noCorCuts->SetPrimaryOnlyCut(kTRUE);
  task_noCorCuts->SetChargedOnlyCut(kTRUE);
  task_noCorCuts->SetGlobalTracksOnlyCut(
      kFALSE); // DO NOT USE in combination with filterbit
  task_noCorCuts->SetCentralityEstimator(
      kV0M); // choices: kV0M,kCL0,kCL1,kSPDTRACKLETS

  // fill control histograms and then bail out
  task_noCorCuts->SetFillControlHistogramsOnly(kTRUE);

  // new task with centrality correlation cuts
  AliAnalysisTaskAR *task_withCenCorCuts =
      dynamic_cast<AliAnalysisTaskAR *>(task_noCorCuts->Clone(
          Form("%s_CENCOR_%.1f-%.1f", std::getenv("TASK_BASENAME"), cen_min,
               cen_max)));
  task_withCenCorCuts->SetCenCorCut(1.1, 10); // slope, offset

  // new task with multiplicity correlation cuts
  AliAnalysisTaskAR *task_withCenMulCorCuts =
      dynamic_cast<AliAnalysisTaskAR *>(task_withCenCorCuts->Clone(
          Form("%s_CENMULCOR_%.1f-%.1f", std::getenv("TASK_BASENAME"), cen_min,
               cen_max)));
  task_withCenMulCorCuts->SetMulCorCut(1.4, 300); // slope, offset

  // add all tasks to the analysis manager in a loop
  std::vector<AliAnalysisTaskAR *> tasks = {task_noCorCuts, task_withCenCorCuts,
                                            task_withCenMulCorCuts};

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
