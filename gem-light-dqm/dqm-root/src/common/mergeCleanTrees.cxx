#ifndef DEBUG
#define DEBUG 0
#endif
#define NVFAT 24
#include <iomanip> 
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <TFile.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TNtuple.h>
#include <TH2.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TRandom3.h>
#include <TBenchmark.h>
#include <TInterpreter.h>
#include <TApplication.h>
#include <TString.h>
#include <Event.h>
#include <TObject.h>
#include <TClonesArray.h>
#include <TRefArray.h>
#include <TRef.h>
#include <TH1.h>
#include <TBits.h>
#include <TMath.h>
#include <TFile.h>
#include <TClassTable.h>
#include <TTree.h>
#include <TBranch.h>
#include <TError.h>

int getNfiredChannels(std::vector<GEBdata> & v_geb);

int main(int argc, char** argv)
{
  if (argc<4) 
  {
      std::cout << "Please provide input filenames" << std::endl;
      std::cout << "Usage: <path>/mergeTree inputFile1.raw.root inputFile2.raw.root outputFile.raw.root" << std::endl;
      return 0;
  }
  std::string ifilename1 = argv[1];
  std::string ifilename2 = argv[2];
  std::string ofilename  = argv[3];
  TFile *ifile1 = new TFile(ifilename1.c_str(), "READ");
  TFile *ifile2 = new TFile(ifilename2.c_str(), "READ");
  TFile *ofile  = new TFile(ofilename.c_str(), "RECREATE");
  TTree *tree1 = (TTree*)ifile1->Get("GEMtree");
  Event *event1 = new Event();
  TBranch *branch1 = tree1->GetBranch("GEMEvents");
  branch1->SetAddress(&event1);
  TTree *tree2 = (TTree*)ifile2->Get("GEMtree");
  Event *event2 = new Event();
  TBranch *branch2 = tree2->GetBranch("GEMEvents");
  branch2->SetAddress(&event2);
  TTree GEMtree("GEMtree","A Tree with GEM Events");
  Event *ev = new Event(); 
  GEMtree.Branch("GEMEvents", &ev);
  Int_t nentries1 = tree1->GetEntries();
  Int_t nentries2 = tree2->GetEntries();
  if (DEBUG) std::cout << "[gemTreeReader]: Number of entries in the TTree 1: " << nentries1 << std::endl;
  if (DEBUG) std::cout << "[gemTreeReader]: Number of entries in the TTree 2: " << nentries2 << std::endl;
  int nentries;
  (nentries1<nentries2)?nentries=nentries1:nentries=nentries2;
  for (Int_t i = 0; i < nentries; i++)
  {
    branch1->GetEntry(i);
    branch2->GetEntry(i);
    bool event1IsOK = event1->isEventGood();
    bool event2IsOK = event2->isEventGood();
    if ((!event1IsOK)||(!event2IsOK)) continue;
    uint32_t evSignature1 = event1->GetEventNumber();
    uint32_t evSignature2 = event2->GetEventNumber();
    if (evSignature1!=evSignature2) continue;
    std::vector<GEBdata> v_geb1;
    std::vector<GEBdata> v_geb2;
    v_geb1 = event1->gebs();
    v_geb2 = event2->gebs();
    if (!getNfiredChannels(v_geb1)){continue;}else if (!getNfiredChannels(v_geb2)){continue;}
    ev->Build(0,0,0,evSignature1,0,0,0,0,0,0,0,0,0,0,0,0,0,0, true);
    ev->SetHeader(i,0,0);
    for (auto igeb = v_geb1.begin(); igeb != v_geb1.end(); igeb++)
    {
      ev->addGEBdata(*igeb);
    }
    for (auto igeb = v_geb2.begin(); igeb != v_geb2.end(); igeb++)
    {
      ev->addGEBdata(*igeb);
    }
    GEMtree.Fill();
    ev->Clear();
  }
  ofile->Write();
}
int getNfiredChannels(std::vector<GEBdata> & v_geb)
{
  int firedchannels = 0;
  uint16_t chan0xf = 0;
  for (Int_t ig = 0; ig < v_geb.size(); ig++)
  {
    // create vector of VFATdata. For data format details look at Event.h
    std::vector<VFATdata> v_vfat;
    v_vfat = v_geb[ig].vfats();
    for (auto m_vfat = v_vfat.begin(); m_vfat != v_vfat.end(); m_vfat++) 
    {
      for (int chan = 0; chan < 128; ++chan) 
      {
        if (chan < 64){
          chan0xf = ((m_vfat->lsData() >> chan) & 0x1);
          if(chan0xf) {firedchannels++;} 
        } else {
          chan0xf = ((m_vfat->msData() >> (chan-64)) & 0x1);
          if(chan0xf) {firedchannels++;}
        }
      }// end of loop over channels 
    }// end of loop over vfats
  }// end of loop over gebs
  return firedchannels;
}

