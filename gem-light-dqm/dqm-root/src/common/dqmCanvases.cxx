void printDQMCanvases()
{
  printIntegrityCanvas();
  printOccupancyCanvas();
}
void printIntegrityCanvas()
{
  TString integrity_plots[8] = {"ControlBit1010", "ControlBit1100", "ControlBit1110", "Flag", "VFATSlotNumber", "NVFATsPerEvent", "2DCRC", "CRCDifference"};
  TCanvas *integrity = newCanvas("Integrity plots", 4, 2, 2400,1200);
  THStack *stack[8];
  for (int s = 0; s < 8; s++){
    stack[s] = new THStack(integrity_plots[s],integrity_plots[s]);
  }

  integrity->cd(1);
  hi1010[1]->SetLineColor(4);
  hi1010[2]->SetLineColor(2);
  stack[0]->Add(hi1010[1]);
  stack[0]->Add(hi1010[2]);
  stack[0]->Draw();
  gPad->SetLogy();

  integrity->cd(2);
  hi1100[1]->SetLineColor(4);
  hi1100[2]->SetLineColor(2);
  stack[1]->Add(hi1100[1]);
  stack[1]->Add(hi1100[2]);
  stack[1]->Draw();
  gPad->SetLogy();

  integrity->cd(3);
  hi1110[1]->SetLineColor(4);
  hi1110[2]->SetLineColor(2);
  stack[2]->Add(hi1110[1]);
  stack[2]->Add(hi1110[2]);
  stack[2]->Draw();
  gPad->SetLogy();

  integrity->cd(4);
  hiFlag[1]->SetLineColor(4);
  hiFlag[2]->SetLineColor(2);
  stack[3]->Add(hiFlag[1]);
  stack[3]->Add(hiFlag[2]);
  stack[3]->Draw();
  gPad->SetLogy();

  integrity->cd(5);
  hiVFATsn[1]->SetLineColor(4);
  hiVFATsn[1]->Scale(1/hiVFAT[0]->GetEntries());
  hiVFATsn[2]->SetLineColor(2);
  hiVFATsn[2]->Scale(1/hiVFAT[0]->GetEntries());
  stack[4]->Add(hiVFATsn[1]);
  stack[4]->Add(hiVFATsn[2]);
  stack[4]->Draw();

  integrity->cd(6);
  hiVFAT[1]->SetLineColor(4);
  hiVFAT[2]->SetLineColor(2);
  stack[5]->Add(hiVFAT[1]);
  stack[5]->Add(hiVFAT[2]);
  stack[5]->Draw();
  gPad->SetLogy();

  integrity->cd(7);
  hiDiffCRC[1]->SetLineColor(4);
  hiDiffCRC[2]->SetLineColor(2);
  stack[6]->Add(hiDiffCRC[1]);
  stack[6]->Add(hiDiffCRC[2]);
  stack[6]->Draw();
  gPad->SetLogy();

  integrity->cd(8);
  hi2DCRC[1]->SetMarkerColor(4);
  hi2DCRC[2]->SetMarkerColor(2);
  hi2DCRC[1]->Draw();
  integrity->cd(8);
  hi2DCRC[2]->Draw("same");
  //stack[7]->Add(hi2DCRC[1]);
  //stack[7]->Add(hi2DCRC[2]);
  //stack[7]->Draw();

  integrity->Print("integrity.png","png");
  integrity->Print("integrity.pdf","pdf");
  integrity->Draw();
  integrity->Update();
  integrity->WaitPrimitive();
}
void printOccupancyCanvas()
{
  TCanvas *occupancy = newCanvas("Occupancy plots", 3, 3, 1800,1800);
  occupancy->cd(1);
  hiBeamProfile[1]->Draw("colz");
  TH1D* p_temp;
  std::stringstream ss;
  for (int p_i = 1; p_i < 9; p_i++)
  {
    occupancy->cd(p_i+1);
    std::string name = "eta_";
    ss.str(std::string());
    ss << p_i;
    name+=ss.str();
    p_temp = hiBeamProfile[1]->ProjectionY(name.c_str(),p_i,p_i);
    p_temp->Draw();
  }
  occupancy->Print("occupancy.png","png");
  occupancy->Print("occupancy.pdf","pdf");
  occupancy->Draw();
  occupancy->Update();
  occupancy->WaitPrimitive();
}
