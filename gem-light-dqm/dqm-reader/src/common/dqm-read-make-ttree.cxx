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
 
  gem::readout::GEMData   gem;
  gem::readout::GEBData   geb;
  gem::readout::VFATData vfat;

  string file="GEMDQMRawData.dat";

  std::ifstream inpf(file.c_str(), std::ios::in|std::ios::binary);
  if(!inpf.is_open()) {
    cout << "\nThe file: " << file.c_str() << " is missing.\n" << endl;
    return 0;
  };

  /* ROOT Analysis Histograms */
  const TString filename = "DQMTreeLight.root";

  const Int_t ieventPrint = 5;
  const Int_t ieventMax   = 90000;
  const Int_t kUPDATE     = 1;
  bool OKpri = false;

  /*
   *  Events Loop
   */

  for(int ievent=1; ievent <= ieventMax; ievent++){
    OKpri = OKprint(ievent,ieventPrint);
    if(inpf.eof()) break;
    if(!inpf.good()) break;

    if(OKpri) cout << "\nievent Start loop" << ievent << endl;

   /*
    *  GEM Chamber's Data level
    */

    if(!gem::readout::readGEMhd1(inpf, gem)) break;
    if(!gem::readout::readGEMhd2(inpf, gem)) break;
    if(!gem::readout::readGEMhd3(inpf, gem)) break;

   /*
    *  GEB Headers Data level
    */

    if(!gem::readout::readGEBheader(inpf, geb));
    //if(OKpri) gem::readout::printGEBheader(ievent,geb);
    uint64_t sumVFAT = (0x000000000fffffff & geb.header);

    if(!gem::readout::readGEBrunhed(inpf, geb)) break;

   /*
    *  GEB PayLoad Data
    */

    for(int ivfat=1; ivfat <= sumVFAT; ivfat++){

      if(!gem::readout::readVFATdata(inpf, ievent, vfat)) break;
  
      uint8_t   b1010  = (0xf000 & vfat.BC) >> 12;
      uint8_t   b1100  = (0xf000 & vfat.EC) >> 12;
      uint8_t   Flag   = (0x000f & vfat.EC);
      uint8_t   b1110  = (0xf000 & vfat.ChipID) >> 12;
      uint16_t  ChipID = (0x0fff & vfat.ChipID);
      uint16_t  CRC    = vfat.crc;
      uint16_t  BX     = vfat.BXfrOH;  

      uint16_t  BC     = (0x0fff & vfat.BC);
      uint8_t   EC     = (0x0fff & vfat.EC) >> 4;
      uint64_t  lsData = vfat.lsData;
      uint64_t  msData = vfat.lsData;

      if ( (b1010 != 0xa) || (b1100 != 0xc) || (b1110 != 0xe) ){
          cout << "VFAT headers do not match expectation" << endl;
          gem::readout::printVFATdataBits(ievent, vfat);
      }// if 1010,1100,1110

    }//end of GEB PayLoad Data

   /*
    *  GEB Trailers Data level
    */

    if(!gem::readout::readGEBtrailer(inpf, geb)) break;
    //if(OKpri) gem::readout::printGEBtrailer(ievent, geb);

   /*
    *  GEM Trailers Data level
    */
    if(!gem::readout::readGEMtr2(inpf, gem)) break;
    if(!gem::readout::readGEMtr1(inpf, gem)) break;

    gem::readout::printVFATdataBits(ievent, vfat);

    if (ievent%kUPDATE == 0 && ievent != 0) {
      cout << "event " << ievent << " ievent%kUPDATE " << ievent%kUPDATE << endl;
    }
  }// end GEB event
  inpf.close();

  // Save all objects in this file
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
