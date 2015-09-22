#ifndef DEBUG
#define DEBUG 1
#endif
#define NVFAT 24
#include <iomanip> 
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
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

#include "gem/datachecker/GEMDataChecker.h"
#include "gem/readout/GEMslotContents.h"
#include "plotter.cxx"

/**
* GEM Tree Reader example (reader) application 
*/

/*! 
  \brief GEM Tree reader reads file with the GEM Event Tree and fills the histogram with number of vfats per event
*/

using namespace std;

uint16_t gem::readout::GEMslotContents::slot[24] = {
      0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,
      0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,
      0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,0xfff,
    };
bool gem::readout::GEMslotContents::isFileRead = false;

class gemTreeReader {
  public:
    gemTreeReader(const std::string &ifilename)
    {
      std::string tmp = ifilename.substr(ifilename.size()-9, ifilename.size());
      if (tmp != ".raw.root") throw std::runtime_error("Wrong input filename (should end with '.raw.root'): "+ifilename);
      ifile = new TFile(ifilename.c_str(), "READ");
      std::string ofilename = ifilename.substr(0,ifilename.size()-9);
      ofilename += ".analyzed.root";
      ofile = new TFile(ofilename.c_str(), "RECREATE");
      this->bookHistograms();
    }
    ~gemTreeReader(){}
    void createHistograms()
    {
      this->fillHistograms();
    }
  private:
    TFile *ifile;
    TFile *ofile;
    TH1F* hiVFAT   ; 
    TH1F* hiVFATsn ; 
    TH1C* hiChip   ; 
    TH1C* hi1010   ; 
    TH1C* hi1100   ; 
    TH1C* hi1110   ; 
    TH1C* hiFlag   ; 
    TH1C* hiCRC    ; 
    TH1C* hiDiffCRC; 
    TH1C* hiFake   ; 
    TH1F* hiCh128  ;     
    TH2C* hi2DCRC  ;     
    TH1C* DiffBXandBC ;
    TH1C* RatioBXandBC;
    TH1C* hiSignal    ;
    TH1C* hichfired   ;
    TH1C* hichnotfired;
    TH1F* hCh_notfired;
    TH1F* h_VFATfired_perevent[24];
    TH1F* hiCh128chipFired[24];

    void bookHistograms()
    {
      hiVFAT       = new TH1F("VFAT", "Number_VFAT_blocks_per_event", 24,  0., 24. );
      hiVFATsn     = new TH1F("VFATsn", "VFAT_slot_number", 24,  0., 24. );
      DiffBXandBC  = new TH1C("DiffBXandBC", "difference of BX and BC", 100000, 0x0, 0x1869F );
      RatioBXandBC = new TH1C("RatioBXandBC", "ratio of BX and BC", 1000, 0x0, 0xa );
      hiChip       = new TH1C("ChipID", "ChipID",         4096, 0x0, 0xfff );
      hi1010       = new TH1C("1010", "Control_Bits_1010", 16, 0x0, 0xf );
      hi1100       = new TH1C("1100", "Control_Bits_1100", 16, 0x0, 0xf );
      hi1110       = new TH1C("1110", "Control_Bits_1110", 16, 0x0, 0xf );
      hiFlag       = new TH1C("Flag"  , "Flag",            16, 0x0, 0xf );
      hiCRC        = new TH1C("CRC",     "CRC",             100, 0x0, 0xffff );
      hiDiffCRC    = new TH1C("DiffCRC", "CRC_Diff",    100, 0xffff, 0xffff );
      hichfired    = new TH1C("chfired", "channels fired per event",      500, 0., 500. );
      hichnotfired = new TH1C("chnotfired", "channels not fired per event",      500, 0., 500. );
      hCh_notfired = new TH1F("Ch_notfired", "Strips",          128, 0., 128. );
      hiCh128      = new TH1F("Ch128", "Strips",          128, 0., 128. );
      hi2DCRC      = new TH2C("CRC1_vs_CRC2", "CRC1_vs_CRC2", 100, 0x0000, 0xffff, 100, 0x0000, 0xffff);
      char name[128], title[500],name1[128], title1[500];
      std::string type[24] = {"Slot0" , "Slot1" , "Slot2" , "Slot3" , "Slot4" , "Slot5" , "Slot6" , "Slot7", 
                              "Slot8" , "Slot9" , "Slot10", "Slot11", "Slot12", "Slot13", "Slot14", "Slot15", 
                              "Slot16", "Slot17", "Slot18", "Slot19", "Slot20", "Slot21", "Slot22", "Slot23"};
      for(int k=0; k<NVFAT; k++){
        sprintf (name  , "h_VFATfired_perevent_%s", type[k].c_str());
        sprintf (title , "VFAT chips %s fired per event", type[k].c_str());
        sprintf (name1 , "hiCh128chipFired_%s", type[k].c_str());
        sprintf (title1, "Strips fired for VFAT chips %s", type[k].c_str());
        h_VFATfired_perevent[k] = new TH1F(name, title, 20, 0., 20.);
        hiCh128chipFired[k] = new TH1F(name1, title1, 128, 0., 128.);
      }
    }
    void fillHistograms()
    {
      if (DEBUG) std::cout << "[gemTreeReader]: Starting filling the histograms" << std::endl;
      TTree *tree = (TTree*)ifile->Get("GEMtree");
      Event *event = new Event();
      TBranch *branch = tree->GetBranch("GEMEvents");
      branch->SetAddress(&event);
      Int_t nentries = tree->GetEntries();
      if (DEBUG) std::cout << "[gemTreeReader]: Number of entries in the TTree : " << nentries << std::endl;
      // loop over tree entries
      for (Int_t i = 0; i < nentries; i++)
      {
        // clear number of VFATs
        Int_t nVFAT = 0;
        Int_t firedchannels = 0;
        Int_t notfiredchannels = 0;
        Int_t vfatId[NVFAT];
        for (int l = 0; i<NVFAT; i++){vfatId[l] = 0;}
        // Retrieve next entry
        branch->GetEntry(i);
        if (DEBUG) std::cout << "[gemTreeReader]: Processing event " << i << std::endl;
        // retrieve bunch crossing from evet
        uint16_t BX_event = event->BXID();
        uint16_t BC;
        // create vector of GEBdata. For data format details look at Event.h
        vector<GEBdata> v_geb;
        v_geb = event->gebs();
        // loop over gebs
        for (Int_t j = 0; j < v_geb.size(); j++)
        {
          // create vector of VFATdata. For data format details look at Event.h
          vector<VFATdata> v_vfat;
          v_vfat = v_geb.at(j).vfats();
          // Increment number of VFATs in the given event
          nVFAT += v_vfat.size();
          // loop over vfats
          for (Int_t k = 0; k < v_vfat.size(); k++)
          {
            if (DEBUG) std::cout << std::dec << "[gemTreeReader]: EC of the vfat inside loop===> "  <<  static_cast<uint32_t>(v_vfat.at(k).EC()) << std::hex << std::endl;   
            if (DEBUG) std::cout << std::dec << "[gemTreeReader]: BC of the vfat inside loop===> "  <<  v_vfat.at(k).BC() << std::hex << std::endl;   
            // fill the control bits histograms
            hi1010->Fill(v_vfat.at(k).b1010());
            hi1100->Fill(v_vfat.at(k).b1100());
            hi1110->Fill(v_vfat.at(k).b1110());
            // fill Flag and chip id histograms
            hiFlag->Fill(v_vfat.at(k).Flag());
            hiChip->Fill(v_vfat.at(k).ChipID());
            // calculate and fill VFAT slot number
            uint32_t t_chipID = static_cast<uint32_t>(v_vfat.at(k).ChipID());
            //gem::readout::GEMslotContents m_GEMslotContents;
            int sn = gem::readout::GEMslotContents::GEBslotIndex(t_chipID);
            hiVFATsn->Fill(sn);
            vfatId[sn]++;
            // fill occupancy plots
            for(Int_t m=0; m<24; m++){
              if(sn == m){
                uint16_t chan0xfFiredchip = 0;
                  for (int chan = 0; chan < 128; ++chan) {
                    if (chan < 64){
                      chan0xfFiredchip = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                      if(chan0xfFiredchip) hiCh128chipFired[m]->Fill(chan);
                    } else {
                      chan0xfFiredchip = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                      if(chan0xfFiredchip) hiCh128chipFired[m]->Fill(chan);
                    }
                 }
              } 
            }
            // calculate and fill the crc and crc_diff
            hiCRC->Fill(v_vfat.at(k).crc());
            uint16_t dataVFAT[11];
            // CRC check
            uint16_t b1010 = (0x000f & v_vfat.at(k).b1010());
            uint16_t b1100 = (0x000f & v_vfat.at(k).b1100());
            uint16_t b1110 = (0x000f & v_vfat.at(k).b1110());
            uint16_t flag  = (0x000f & v_vfat.at(k).Flag());
            uint16_t ec    = (0x00ff & v_vfat.at(k).EC());
            BC             = v_vfat.at(k).BC();
            dataVFAT[11]   = (0xf000 & (b1010 << 12)) | (0x0fff & v_vfat.at(k).BC());
            dataVFAT[10]   = ((0xf000 & (b1100 << 12)) | (0x0ff0 & (ec << 4))) | (0x000f & flag);
            dataVFAT[9]    = (0xf000 & (b1110 << 12)) | (0x0fff & v_vfat.at(k).ChipID());
            dataVFAT[8]    = (0xffff000000000000 & v_vfat.at(k).msData()) >> 48;
            dataVFAT[7]    = (0x0000ffff00000000 & v_vfat.at(k).msData()) >> 32;
            dataVFAT[6]    = (0x00000000ffff0000 & v_vfat.at(k).msData()) >> 16;
            dataVFAT[5]    = (0x000000000000ffff & v_vfat.at(k).msData());
            dataVFAT[4]    = (0xffff000000000000 & v_vfat.at(k).lsData()) >> 48;
            dataVFAT[3]    = (0x0000ffff00000000 & v_vfat.at(k).lsData()) >> 32;
            dataVFAT[2]    = (0x00000000ffff0000 & v_vfat.at(k).lsData()) >> 16;
            dataVFAT[1]    = (0x000000000000ffff & v_vfat.at(k).lsData());
            gem::datachecker::GEMDataChecker *dc = new gem::datachecker::GEMDataChecker::GEMDataChecker();
            uint16_t checkedCRC = dc->checkCRC(dataVFAT, 0);
            if (DEBUG) std::cout << "[gemTreeReader]: CRC read from vfat : " << std::hex << v_vfat.at(k).crc() << std::endl;
            if (DEBUG) std::cout << "[gemTreeReader]: CRC recalculated   : " << std::hex << checkedCRC << std::endl;
            hiDiffCRC->Fill(v_vfat.at(k).crc()-checkedCRC);
            hi2DCRC->Fill(v_vfat.at(k).crc(), checkedCRC);
            delete dc;
            //I think it would be nice to time this...
            uint16_t chan0xf = 0;
            for (int chan = 0; chan < 128; ++chan) {
              if (chan < 64){
                chan0xf = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                if(chan0xf) {
                   hiCh128->Fill(chan);
                   firedchannels++;
                } else {
                  hCh_notfired->Fill(chan);
                  notfiredchannels++;
                }
              } else {
                chan0xf = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                if(chan0xf) {
                   hiCh128->Fill(chan);
                   firedchannels++;
                } else {
                  hCh_notfired->Fill(chan);
                  notfiredchannels++;
                }
              }
            }// end of loop over channels 
          }// end of loop over VFATs
        }// end of loop over GEBs
        int diffBXandBC =  fabs(BX_event - BC);  
        //float ratioBXandBC;  
	      double ratioBXandBC = (double) BX_event / BC;
        DiffBXandBC->Fill(diffBXandBC); 
        RatioBXandBC->Fill(ratioBXandBC);
        hiVFAT->Fill(nVFAT);
	      for(Int_t x=0; x<24; x++) h_VFATfired_perevent[x]->Fill(vfatId[x]);
        hichfired->Fill(firedchannels);
        hichnotfired->Fill(firedchannels);
      }// end of loop over events
      
      setTitles(hiVFAT, "Number VFAT blocks per Event", "Number of Events");   
      setTitles(hiChip, "ChipID value, max 0xfff", "Number of VFAT blocks");
      setTitles(hi1010, "1010 marker, max 0xf", "Number of VFAT blocks");   
      setTitles(hi1100, "1100 marker, max 0xf", "Number of VFAT blocks");   
      setTitles(hi1110, "1110 marker, max 0xf", "Number of VFAT blocks");   
      setTitles(hiFlag, "Flag marker value, max 0xf", "Number of VFAT blocks");   
      setTitles(hiCRC, "CRC value, max 0xffff", "Number of VFAT blocks");
      setTitles(hiDiffCRC, "CRC difference", "Number of VFAT blocks");
      //setTitles(hiFake, "Fake events", "Number of Events");
      setTitles(hiCh128, "Strips, max 128", "Number of VFAT blocks"); 
      setTitles(hi2DCRC, "CRC VFAT", "CRC calc");  
      
      ofile->Write();
    }

};
