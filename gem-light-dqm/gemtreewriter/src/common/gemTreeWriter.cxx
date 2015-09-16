#define DEBUG 0

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
#if defined(__CINT__) && !defined(__MAKECINT__)
    #include "libEvent.so"
#else
    #include "Event.h"
#endif

#include "gem/readout/GEMDataAMCformat.h"

/**
* GEM Tree Composer (gtc) application provides translation of the GEM output HEX file to ROOT-based TTree with GEM Events
*/

/*! \file */
/*! 
  \mainpage GEM Tree Composer application.

  HEX data reading module for light DQM package. Converts HEX data to ROOT TTree.

  \section Installation

  Getting source code:

  git clone https://github.com/mexanick/gem-daq-code.git <br>
  cd gem-daq-code/
  git checkout lightDQM

  Make the executable:
  cd gem-light-dqm/gemtreewriter/
  make -j4

  Now you will have executables:
  - GEM Tree Writer bin/gtc
  - Example of GEMtree reader bin/reader
  Then run the executable providing input and output filenames:
  ./bin/gtc GEMDQMRawData.dat outputROOTtree.root

  You can download sample HEX-ASCII data file:
  wget https://baranov.web.cern.ch/baranov/xdaq/Testing/CERN.904/OHv2/24.Aug.2015/GEM_DAQ_Mon_Aug_24_12-59-28_2015.dat

  After producing the outputROOTtree.root you can run the tree reader example, which should serve as basis for future analyzers:
  ./bin/reader outputROOTtree.root outputAnalyzer.root

  The source code for the reader you can find here:
  ./src/common/treeReaderExample.cxx

  All the compilation settings you can find in the Makefile

  \author Sergey.Baranov@cern.ch, Mykhailo.Dalchenko@cern.ch
*/

class gemTreeWriter {
  public:
    gemTreeWriter(){}
    ~gemTreeWriter(){}
    void makeTree(std::string ifile)
    {
      std::string tmp = ifile.substr(ifile.size()-4. ifile.size());
      if (tmp!=".dat") {
        std::cout << "Error! Input file should be *.dat format" << std::endl;
        return;
      }
      tmp = ifile.substr(0, ifile.size()-4);
      tmp += "*.raw.root";
      makeTree(ifile, tmp);
    }
    void makeTree(std::string ifile, std::string ofile)
    {
      TFile *hfile = new TFile(ofile,"RECREATE","Threshold Scan ROOT file with histograms");
      TTree GEMtree("GEMtree","A Tree with GEM Events");
      Event *ev = new Event(); 
      GEMtree.Branch("GEMEvents", &ev);
      std::ifstream inpf(ifile.c_str(), std::ios::in|std::ios::binary);
      inpf.open(ifile.c_str());
      inpf.open(ifile.c_str());
      if(!inpf.is_open()) {std::cout << "\nThe file: " << ifile.c_str() << " is missing.\n" << std::endl;}
      ievent=0;
      while(true){
        if(inpf.eof()) break;
        if(!inpf.good()) break;
        if DEBUG std::cout << "Processing event " << ievent << std::endl;
        /*
         *  GEM Headers Data level
         */
        if (InpType == "Hex") {
          if(!gem::readout::readGEMhd1(inpf, gem)) break;
          if(!gem::readout::readGEMhd2(inpf, gem)) break;
          if(!gem::readout::readGEMhd3(inpf, gem)) break;
        } else {
          if(!gem::readout::readGEMhd1Binary(inpf, gem)) break;
          if(!gem::readout::readGEMhd2Binary(inpf, gem)) break;
          if(!gem::readout::readGEMhd3Binary(inpf, gem)) break;
        }
    
        /*
         *  GEB Headers Data level
         */
        if (InpType == "Hex") {
          if(!gem::readout::readGEBheader(inpf, geb)) break;
        } else {
          if(!gem::readout::readGEBheaderBinary(inpf, geb)) break;
        } //if(OKpri) gem::readout::printGEBheader(ievent,geb);
    
        uint32_t ZSFlag  = (0xffffff0000000000 & geb.header) >> 40; 
        uint16_t ChamID  = (0x000000fff0000000 & geb.header) >> 28; 
        uint32_t sumVFAT = (0x000000000fffffff & geb.header);
        uint16_t  BX     = 0;

        if (InpType == "Hex") {
          if(!gem::readout::readGEBrunhed(inpf, geb)) break;
        } else {
          if(!gem::readout::readGEBrunhedBinary(inpf, geb)) break;
        }
    
       /*
        *  GEB PayLoad Data
        */
        GEBdata *GEBdata_ = new GEBdata(ZSFlag, ChamID, sumVFAT);

        int ifake = 0;
        for(int ivfat=0; ivfat<sumVFAT; ivfat++){
          if (InpType == "Hex") {
            if(!gem::readout::readVFATdata(inpf, ivfat, vfat)) break;
          } else {
            if(!gem::readout::readVFATdataBinary(inpf, ivfat, vfat)) break;
          }
      
          uint8_t   b1010  = (0xf000 & vfat.BC) >> 12;
          uint16_t  BC     = (0x0fff & vfat.BC);
          uint8_t   b1100  = (0xf000 & vfat.EC) >> 12;
          uint8_t   EC     = (0x0ff0 & vfat.EC) >> 4;
          uint8_t   Flag   = (0x000f & vfat.EC);
          uint8_t   b1110  = (0xf000 & vfat.ChipID) >> 12;
          uint16_t  ChipID = (0x0fff & vfat.ChipID);
          uint16_t  CRC    = vfat.crc;
          uint64_t  lsData = vfat.lsData;
          uint64_t  msData = vfat.msData;
          BX     = vfat.BXfrOH;  
          VFATdata *VFATdata_ = new VFATdata(b1010, BC, b1100, EC, Flag, b1110, ChipID, lsData, msData, CRC);
          GEBdata_->addVFATData(*VFATdata_);
          delete VFATdata_;
        }//end of GEB PayLoad Data
        /*
         *  GEB Trailers Data level
         */
        if (InpType == "Hex") {
          if(!gem::readout::readGEBtrailer(inpf, geb)) break;
        } else {
          if(!gem::readout::readGEBtrailerBinary(inpf, geb)) break;
        }//if(OKpri) gem::readout::printGEBtrailer(ievent, geb);
    
        uint16_t OHcrc      = (0xffff000000000000 & geb.trailer) >> 48; 
        uint16_t OHwCount   = (0x0000ffff00000000 & geb.trailer) >> 32; 
        uint16_t ChamStatus = (0x00000000ffff0000 & geb.trailer) >> 16;
        uint16_t GEBres     = (0x000000000000ffff & geb.trailer);

        GEBdata_->setTrailer(OHcrc, OHwCount, ChamStatus, GEBres);

        /*
         *  GEM Trailers Data level
         */
        if (InpType == "Hex") {
          if(!gem::readout::readGEMtr2(inpf, gem)) break;
          if(!gem::readout::readGEMtr1(inpf, gem)) break;
        } else {
          if(!gem::readout::readGEMtr2Binary(inpf, gem)) break;
          if(!gem::readout::readGEMtr1Binary(inpf, gem)) break;
        }
        
        ev->Build(0,0,0,BX,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
        ev->addGEBdata(*GEBdata_);
        GEMtree.Fill();
        ev->Clear();
      }// End loop on events
      inpf.close();
      hfile->Write();// Save file with tree
    }
  private:
    int ievent;
    std::string InpType;
    gem::readout::GEMData   gem;
    gem::readout::GEBData   geb;
    gem::readout::VFATData vfat;
    void checkInputFileFormat(std::string ifile)
    {
      std::ifstream inpf(ifile.c_str(), std::ios::in|std::ios::binary);
      char c = inpf.get();
      inpf.close();
      (c==1)?InpType="Bin":InpType="Hex";
      if DEBUG std::cout << "File type: " << InpType << std::endl;
      //if ( c != 1 ) InpType = "Hex" else InpType = "Bin";
    }
};
