#ifndef __CINT__
#include <iomanip> 
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <cstdint>

#include <TFile.h>
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
#include <TObject.h>

#include "TClonesArray.h"
#include "TRefArray.h"
#include "TRef.h"
#include "TBits.h"
#include "TMath.h"

#include "gem/readout/GEMDataAMCformat.h"

using namespace std;

// Ok printing
bool OKprint(int ievent, int iMaxPrint ){
  if( ievent <= iMaxPrint ){
    return (true);
  } else { 
    return (false);
  }
}

TROOT root("",""); // static TROOT object

int main(int argc, char** argv)
#else
TFile* thldread(Int_t get=0)
#endif
{ cout<<"---> Main()"<<endl;

#ifndef __CINT__
  TApplication App("App", &argc, argv);
#endif
 
  gem::readout::GEBData   geb;
  gem::readout::VFATData vfat;

  string file="GEMDQMRawData.dat";

  ifstream inpf(file.c_str());
  if(!inpf.is_open()) {
    cout << "\nThe file: " << file.c_str() << " is missing.\n" << endl;
    return 0;
  };

  /* ROOT Analysis Histograms */
  const TString filename = "DQMTreeLight.root";

  // Create a new canvas.
  TCanvas *c1 = new TCanvas("c1","Dynamic Filling Example",0,0,700,300);
  c1->SetFillColor(42);
  c1->GetFrame()->SetFillColor(21);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);
  c1->Divide(2,1);

  TFile* hfile = NULL;
  hfile = new TFile(filename,"RECREATE","ROOT file with histograms");

  const Int_t ieventPrint = 2;
  const Int_t ieventMax   = 90000;
  const Int_t kUPDATE     = 50;
  bool OKpri = false;


  for(int ievent=0; ievent<ieventMax; ievent++){
    OKpri = OKprint(ievent,ieventPrint);
    if(inpf.eof()) break;
    if(!inpf.good()) break;

    if(OKpri) cout << "\nievent " << ievent << endl;

    // read Event Chamber Header 
    gem::readout::readGEBheader(inpf, geb);
    if(OKpri) gem::readout::printGEBheader(ievent,geb);

    uint64_t ZSFlag  = (0xffffff0000000000 & geb.header) >> 40; 
    uint64_t ChamID  = (0x000000fff0000000 & geb.header) >> 28; 
    uint64_t sumVFAT = (0x000000000fffffff & geb.header);

    for(int ivfat=0; ivfat<sumVFAT; ivfat++){
     /*
      *  GEM Event Reading
      */
      gem::readout::readVFATdata(inpf, ievent, vfat);
  
      uint8_t   b1010  = (0xf000 & vfat.BC) >> 12;
      uint8_t   b1100  = (0xf000 & vfat.EC) >> 12;
      uint8_t   Flag   = (0x000f & vfat.EC);
      uint8_t   b1110  = (0xf000 & vfat.ChipID) >> 12;
      uint16_t  ChipID = (0x0fff & vfat.ChipID);
      uint16_t  CRC    = vfat.crc;

      uint16_t  BC     = (0x0fff & vfat.BC);
      uint8_t   EC     = (0x0fff & vfat.EC) >> 4;
      uint64_t  lsData = vfat.lsData;
      uint64_t  msData = vfat.lsData;

      if ( (b1010 == 0xa) && (b1100==0xc) && (b1110==0xe) ){
        if(OKpri){
          gem::readout::printVFATdataBits(ievent, vfat);
        }
      }// if 1010,1100,1110, ChipID

    }//end ivfat

    // read Event Chamber Header 
    gem::readout::readGEBtrailer(inpf, geb);
    if(OKpri) gem::readout::printGEBtrailer(ievent, geb);

    uint64_t OHcrc      = (0xffff000000000000 & geb.trailer) >> 48; 
    uint64_t OHwCount   = (0x0000ffff00000000 & geb.trailer) >> 32; 
    uint64_t ChamStatus = (0x00000000ffff0000 & geb.trailer) >> 16;

    uint16_t GEBres     = (0x000000000000ffff & geb.trailer);

    if(OKpri){
      cout << "GEM Camber Treiler: OHcrc " << hex << OHcrc << " OHwCount " << OHwCount << " ChamStatus " << ChamStatus << dec 
           << " ievent " << ievent << endl;
    }

    if (ievent%kUPDATE == 0 && ievent != 0) {
      //c1->Update();
      cout << "event " << ievent << " ievent%kUPDATE " << ievent%kUPDATE << endl;
    }
    if(OKpri) cout<<"ievent "<< ievent <<endl;
  }// end GEB event
  inpf.close();

  // Save all objects in this file
  hfile->Write();
  cout<<"=== hfile->Write()"<<endl;

#ifndef __CINT__
     App.Run();
#endif

#ifdef __CINT__
   return hfile;
#else
   return 0;
#endif

}
