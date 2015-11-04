#ifndef __CINT__

#include "gem/readout/GEMDataAMCformat.h"

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

using namespace std;

uint16_t gem::readout::GEMslotContents::slot[24] = {
  0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,
  0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,
  0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,
};
bool gem::readout::GEMslotContents::isFileRead = false;

typedef gem::readout::GEMDataAMCformat::GEMData  AMCGEMData;
typedef gem::readout::GEMDataAMCformat::GEBData  AMCGEBData;
typedef gem::readout::GEMDataAMCformat::VFATData AMCVFATData;

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
 
  AMCGEMData   gem;
  AMCGEBData   geb;
  AMCVFATData vfat;

  string file="GEMDQMRawData.dat";
  std::string InpType = "Binary";

  std::ifstream inpf(file.c_str(), std::ios::in|std::ios::binary);
  char c = inpf.get();
  inpf.close();
  if ( c != 1 ) InpType = "Hex";
  cout << " Input File has type " << c << " " << "  " << InpType << endl;

  inpf.open(file.c_str(), std::ios::in|std::ios::binary);
  if(!inpf.is_open()) {
    cout << "\nThe file: " << file.c_str() << " is missing.\n" << endl;
    return 0;
  };

  /* ROOT Analysis Histograms */
  const TString filename = "DQMTreeLight.root";

  const Int_t ieventPrint = 3;
  const Int_t ieventMax   = 90000;
  const Int_t kUPDATE     = 1;
  bool OKpri = false;

  gem::readout::GEMslotContents::getSlotCfg();

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

    if (InpType == "Hex") {
      if(!gem::readout::GEMDataAMCformat::readGEMhd1(inpf, gem)) break;
      if(!gem::readout::GEMDataAMCformat::readGEMhd2(inpf, gem)) break;
      if(!gem::readout::GEMDataAMCformat::readGEMhd3(inpf, gem)) break;
    } else {
      if(!gem::readout::GEMDataAMCformat::readGEMhd1Binary(inpf, gem)) break;
      if(!gem::readout::GEMDataAMCformat::readGEMhd2Binary(inpf, gem)) break;
      if(!gem::readout::GEMDataAMCformat::readGEMhd3Binary(inpf, gem)) break;
    }

   /*
    *  GEB Headers Data level
    */

    if (InpType == "Hex") {
      if(!gem::readout::GEMDataAMCformat::readGEBheader(inpf, geb)) break;
    } else {
      if(!gem::readout::GEMDataAMCformat::readGEBheaderBinary(inpf, geb)) break;
    } //if(OKpri) gem::readout::GEMDataAMCformat::printGEBheader(ievent,geb);

    uint64_t ZSFlag  = (0xffffff0000000000 & geb.header) >> 40; 
    uint64_t ChamID  = (0x000000fff0000000 & geb.header) >> 28; 
    uint64_t sumVFAT = (0x000000000fffffff & geb.header);

    if (InpType == "Hex") {
      if(!gem::readout::GEMDataAMCformat::readGEBrunhed(inpf, geb)) break;
    } else {
      if(!gem::readout::GEMDataAMCformat::readGEBrunhedBinary(inpf, geb)) break;
    }

   /*
    *  GEB PayLoad Data
    */

    for(int ivfat=1; ivfat <= sumVFAT; ivfat++){

      if (InpType == "Hex") {
        if(!gem::readout::GEMDataAMCformat::readVFATdata(inpf, ivfat, vfat)) break;
      } else {
	if(!gem::readout::GEMDataAMCformat::readVFATdataBinary(inpf, ivfat, vfat)) break;
      }
  
      uint8_t   b1010  = (0xf000 & vfat.BC) >> 12;
      uint8_t   b1100  = (0xf000 & vfat.EC) >> 12;
      uint8_t   Flag   = (0x000f & vfat.EC);
      uint8_t   b1110  = (0xf000 & vfat.ChipID) >> 12;
      uint16_t  ChipID = (0x0fff & vfat.ChipID);
      uint16_t  CRC    = vfat.crc;
      uint32_t  BX     = vfat.BXfrOH;  

      uint16_t  BC     = (0x0fff & vfat.BC);
      uint8_t   EC     = (0x0fff & vfat.EC) >> 4;
      uint64_t  lsData = vfat.lsData;
      uint64_t  msData = vfat.lsData;

      uint32_t ZSFlag24 = ZSFlag;
      int islot = -1;
      for (int ibin = 0; ibin < 24; ibin++){
	if ( (ChipID == gem::readout::GEMslotContents::slot[ibin]) && ((ZSFlag >> (23-ibin)) & 0x1) ) islot = ibin;
      }//end for

      if ( (b1010 != 0xa) || (b1100 != 0xc) || (b1110 != 0xe) ){
          cout << "VFAT headers do not match expectation" << endl;
          gem::readout::GEMDataAMCformat::printVFATdataBits(ievent, vfat);
      }// if 1010,1100,1110

    }//end of GEB PayLoad Data

   /*
    *  GEB Trailers Data level
    */

    if (InpType == "Hex") {
      if(!gem::readout::GEMDataAMCformat::readGEBtrailer(inpf, geb)) break;
    } else {
      if(!gem::readout::GEMDataAMCformat::readGEBtrailerBinary(inpf, geb)) break;
    }//if(OKpri) gem::readout::GEMDataAMCformat::printGEBtrailer(ievent, geb);

   /*
    *  GEM Trailers Data level
    */
    if (InpType == "Hex") {
      if(!gem::readout::GEMDataAMCformat::readGEMtr2(inpf, gem)) break;
      if(!gem::readout::GEMDataAMCformat::readGEMtr1(inpf, gem)) break;
    } else {
      if(!gem::readout::GEMDataAMCformat::readGEMtr2Binary(inpf, gem)) break;
      if(!gem::readout::GEMDataAMCformat::readGEMtr1Binary(inpf, gem)) break;
    }

    gem::readout::GEMDataAMCformat::printVFATdataBits(ievent, vfat);

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
