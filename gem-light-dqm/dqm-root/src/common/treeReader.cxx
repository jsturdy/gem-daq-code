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

uint16_t gem::readout::GEMslotContents::slot[NVFAT] = {
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

    TH1F* hiVFAT                     [3]; 
    TH1F* hiVFATsn                   [3]; 
    TH1C* hiChip                     [3]; 
    TH1C* hi1010                     [3]; 
    TH1C* hi1100                     [3]; 
    TH1C* hi1110                     [3]; 
    TH1C* hiFlag                     [3]; 
    TH1C* hiCRC                      [3]; 
    TH1C* hiDiffCRC                  [3]; 
    TH1C* hiFake                     [3]; 
    TH1F* hiCh128                    [3];     
    TH2C* hi2DCRC                    [3];     
    TH1C* hiDiffBXandBC              [3];
    TH1C* hiRatioBXandBC             [3];
    TH1C* hiSignal                   [3];
    TH1C* hichfired                  [3];
    TH1C* hichnotfired               [3];
    TH1F* hiCh_notfired              [3];
    TH1F* hiVFATfired_perevent[3][NVFAT];
    TH1F* hiCh128chipFiredi   [3][NVFAT];

    TDirectory *dir[3];
    std::string dirname[3] = {"AllEvents", "GoodEvents", "BadEvents"};

    int counters_[4]; // [0] - total events
                      // [1] - good events
                      // [2] - bad events 
                      // [3] - good events failed CRC check

    void bookHistograms()
    {
      char name[128], title[500],name1[128], title1[500];
      std::string type[NVFAT] = {"Slot0" , "Slot1" , "Slot2" , "Slot3" , "Slot4" , "Slot5" , "Slot6" , "Slot7", 
                              "Slot8" , "Slot9" , "Slot10", "Slot11", "Slot12", "Slot13", "Slot14", "Slot15", 
                              "Slot16", "Slot17", "Slot18", "Slot19", "Slot20", "Slot21", "Slot22", "Slot23"};
      for (int i = 0; i < 3; i++) {
        dir[i] = ofile->mkdir(dirname[i].c_str());
        dir[i]->cd();
        hiVFAT         [i] = new TH1F(dirname[i].c_str()+"_VFAT", "Number VFAT blocks per event", 24,  0., 24. );
        hiVFATsn       [i] = new TH1F(dirname[i].c_str()+"_VFATsn", "VFAT slot number", 24,  0., 24. );
        hiDiffBXandBC  [i] = new TH1C(dirname[i].c_str()+"_DiffBXandBC", "Difference of BX and BC", 100000, 0x0, 0x1869F );
        hiRatioBXandBC [i] = new TH1C(dirname[i].c_str()+"_RatioBXandBC", "Ratio of BX and BC", 1000, 0x0, 0xa );
        hiChip         [i] = new TH1C(dirname[i].c_str()+"_ChipID", "ChipID",         4096, 0x0, 0xfff );
        hi1010         [i] = new TH1C(dirname[i].c_str()+"_1010", "Control Bits 1010", 16, 0x0, 0xf );
        hi1100         [i] = new TH1C(dirname[i].c_str()+"_1100", "Control Bits 1100", 16, 0x0, 0xf );
        hi1110         [i] = new TH1C(dirname[i].c_str()+"_1110", "Control Bits 1110", 16, 0x0, 0xf );
        hiFlag         [i] = new TH1C(dirname[i].c_str()+"_Flag"  , "Flag",            16, 0x0, 0xf );
        hiCRC          [i] = new TH1C(dirname[i].c_str()+"_CRC",     "CRC",             100, 0x0, 0xffff );
        hiDiffCRC      [i] = new TH1C(dirname[i].c_str()+"_DiffCRC", "CRC_Diff",    100, 0xffff, 0xffff );
        hichfired      [i] = new TH1C(dirname[i].c_str()+"_chfired", "Channels fired per event",      500, 0., 500. );
        hichnotfired   [i] = new TH1C(dirname[i].c_str()+"_chnotfired", "Channels not fired per event",      500, 0., 500. );
        hiCh_notfired  [i] = new TH1F(dirname[i].c_str()+"_Ch_notfired", "Strips",          128, 0., 128. );
        hiCh128        [i] = new TH1F(dirname[i].c_str()+"_Ch128", "Strips",          128, 0., 128. );
        hi2DCRC        [i] = new TH2C(dirname[i].c_str()+"_CRC1_vs_CRC2", "CRC1 vs_ RC2", 100, 0x0000, 0xffff, 100, 0x0000, 0xffff);
        for(int j=0; j < NVFAT; j++){
          sprintf (name  , dirname[i].c_str()+"_hiVFATfired_perevent_%s", type[j].c_str());
          sprintf (title , "VFAT chip %s fired per event", type[j].c_str());
          sprintf (name1 , dirname[i].c_str()+"_hiCh128chipFired_%s", type[j].c_str());
          sprintf (title1, "Strips fired for VFAT chip %s", type[j].c_str());
          hiVFATfired_perevent[i][j] = new TH1F(name, title, 20, 0., 20.);
          hiCh128chipFired    [i][j] = new TH1F(name1, title1, 128, 0., 128.);
        }
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
      // init counters
      for (int c_=0; c_<4; c_++) counters_[c_] = 0;
      // loop over tree entries
      for (Int_t i = 0; i < nentries; i++)
      {
        if (DEBUG) std::cout << "[gemTreeReader]: Processing event " << i << std::endl;
        // clear number of VFATs
        Int_t nVFAT = 0;
        Int_t t_firedchannels = 0;
        Int_t t_notfiredchannels = 0;
        Int_t b_firedchannels = 0;
        Int_t b_notfiredchannels = 0;
        Int_t g_firedchannels = 0;
        Int_t g_notfiredchannels = 0;
        Int_t vfatId[NVFAT];
        for (int l = 0; l<NVFAT; l++){
          vfatId[l] = 0;
          if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Fired chip counter when initializing event"  <<  vfatId[l]  << std::endl;   
        }
        // Retrieve next entry
        branch->GetEntry(i);
        bool eventIsOK = event->isEventGood();
        // retrieve bunch crossing from evet
        uint32_t t_BX_event = event->BXID();
        uint16_t t_BC;
        uint32_t g_BX_event;
        uint16_t g_BC;
        uint32_t b_BX_event;
        uint16_t b_BC;
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
            if (DEBUG) std::cout << std::dec << "[gemTreeReader]: VFAT # "  <<  k << std::endl;   
            if (DEBUG) std::cout << std::dec << "[gemTreeReader]: EC of the vfat inside loop===> "  <<  static_cast<uint32_t>(v_vfat.at(k).EC()) << std::hex << std::endl;   
            if (DEBUG) std::cout << std::dec << "[gemTreeReader]: BC of the vfat inside loop===> "  <<  v_vfat.at(k).BC() << std::hex << std::endl;   
            if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Slot number of responded chip "  <<  v_vfat.at(k).SlotNumber()  << std::endl;   
            if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Fired chip counter before incrementing"  <<  vfatId[v_vfat.at(k).SlotNumber()]  << std::endl;   
            if (v_vfat.at(k).SlotNumber()>(-1)) vfatId[v_vfat.at(k).SlotNumber()]++;
            if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Fired chip counter "  <<  vfatId[v_vfat.at(k).SlotNumber()]  << std::endl;   
            // fill histograms for all events
            dir[0]->cd();
            this->fillVFATHistograms(&v_vfat.at(k), hiVFAT[0], hiVFATsn[0], hiCh128[0], hiCh_notfired[0], hichnotfired[0], hiDiffBXandBC[0], hiRatioBXandBC[0], hiChip[0], hi1010[0], hi1100[0], hi1110[0], hiFlag[0], hiCRC[0], hiDiffCRC[0], hichfired[0], hi2DCRC[0], &g_firedchannels, &g_notfiredchannels);
            // fill histograms for good and bad events
            if (v_vfat.at(k).isBlockGood()){
              dir[1]->cd();
              this->fillVFATHistograms(&v_vfat.at(k), hiVFAT[1], hiVFATsn[1], hiCh128[1], hiCh_notfired[1], hichnotfired[1], DiffBXandBC[1], RatioBXandBC[1], hiChip[1], hi1010[1], hi1100[1], hi1110[1], hiFlag[1], hiCRC[1], hiDiffCRC[1], hichfired[1], hi2DCRC[1], &g_firedchannels, &g_notfiredchannels);
            } else {
              dir[2]->cd();
              this->fillVFATHistograms(&v_vfat.at(k), hiVFAT[2], hiVFATsn[2], hiCh128[2], hiCh_notfired[2], hichnotfired[2], DiffBXandBC[2], RatioBXandBC[2], hiChip[2], hi1010[2], hi1100[2], hi1110[2], hiFlag[2], hiCRC[2], hiDiffCRC[2], hichfired[2], hi2DCRC[2], &g_firedchannels, &g_notfiredchannels);
            }
            
            t_BC             = v_vfat.at(k).BC();

            // fill occupancy plots
          
          }// end of loop over VFATs
        }// end of loop over GEBs
        int diffBXandBC =  fabs(t_BX_event - t_BC);  
        //float ratioBXandBC;  
	      double ratioBXandBC = (double) t_BX_event / t_BC;
        t_DiffBXandBC->Fill(diffBXandBC); 
        t_RatioBXandBC->Fill(ratioBXandBC);
        t_hiVFAT->Fill(nVFAT);
	      for(Int_t x=0; x<NVFAT; x++) {
          t_h_VFATfired_perevent[x]->Fill(vfatId[x]);
          if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Fired chip counter when filling event"  <<  vfatId[x]  << std::endl;   
        }
        t_hichfired->Fill(t_firedchannels);
        t_hichnotfired->Fill(t_notfiredchannels);
        if (eventIsOK){
          diffBXandBC =  fabs(g_BX_event - g_BC);  
  	      ratioBXandBC = (double) g_BX_event / g_BC;
          g_DiffBXandBC->Fill(diffBXandBC); 
          g_RatioBXandBC->Fill(ratioBXandBC);
          g_hiVFAT->Fill(nVFAT);
  	      for(Int_t x=0; x<NVFAT; x++) g_h_VFATfired_perevent[x]->Fill(vfatId[x]);
          g_hichfired->Fill(g_firedchannels);
          g_hichnotfired->Fill(g_notfiredchannels);
        } else {
          diffBXandBC =  fabs(b_BX_event - b_BC);  
  	      ratioBXandBC = (double) b_BX_event / b_BC;
          b_DiffBXandBC->Fill(diffBXandBC); 
          b_RatioBXandBC->Fill(ratioBXandBC);
          b_hiVFAT->Fill(nVFAT);
  	      for(Int_t x=0; x<NVFAT; x++) b_h_VFATfired_perevent[x]->Fill(vfatId[x]);
          b_hichfired->Fill(b_firedchannels);
          b_hichnotfired->Fill(b_notfiredchannels);
        }
      }// end of loop over events
      
      setTitles(t_hiVFAT, "Number VFAT blocks per Event", "Number of Events");   
      setTitles(t_hiChip, "ChipID value, max 0xfff", "Number of VFAT blocks");
      setTitles(t_hi1010, "1010 marker, max 0xf", "Number of VFAT blocks");   
      setTitles(t_hi1100, "1100 marker, max 0xf", "Number of VFAT blocks");   
      setTitles(t_hi1110, "1110 marker, max 0xf", "Number of VFAT blocks");   
      setTitles(t_hiFlag, "Flag marker value, max 0xf", "Number of VFAT blocks");   
      setTitles(t_hiCRC, "CRC value, max 0xffff", "Number of VFAT blocks");
      setTitles(t_hiDiffCRC, "CRC difference", "Number of VFAT blocks");
      //setTitles(hiFake, "Fake events", "Number of Events");
      setTitles(t_hiCh128, "Strips, max 128", "Number of VFAT blocks"); 
      setTitles(t_hi2DCRC, "CRC VFAT", "CRC calc");  
      
      ofile->Write();
    }

    void fillVFATHistograms(VFATdata *m_vfat, TH1F* m_hiVFAT, TH1F* m_hiVFATsn, TH1F* m_hiCh128, TH1F* m_hiCh_notfired, TH1C* m_hi_chnotfired, TH1C* m_hiDiffBXandBC, TH1C* m_hiRatioBXandBC, TH1C* m_hiChip, TH1C* m_hi1010, TH1C* m_hi1100, TH1C* m_hi1110, TH1C* m_hiFlag, TH1C* m_hiCRC, TH1C* m_hiDiffCRC, TH1C* m_hichfired, TH2C* m_hi2DCRC, Int_t * firedchannels, Int_t * notfiredchannels)
    {
      // fill the control bits histograms
      m_hi1010->Fill(m_vfat->b1010());
      m_hi1100->Fill(m_vfat->b1100());
      m_hi1110->Fill(m_vfat->b1110());
      // fill Flag and chip id histograms
      m_hiFlag->Fill(m_vfat->Flag());
      m_hiChip->Fill(m_vfat->ChipID());
      // calculate and fill VFAT slot number
      int sn_ = m_vfat->SlotNumber();
      m_hiVFATsn->Fill(sn_);
      // calculate and fill the crc and crc_diff
      m_hiCRC->Fill(m_vfat->crc());
      // CRC check
      uint16_t b1010 = (0x000f & m_vfat->b1010());
      uint16_t b1100 = (0x000f & m_vfat->b1100());
      uint16_t b1110 = (0x000f & m_vfat->b1110());
      uint16_t flag  = (0x000f & m_vfat->Flag());
      uint16_t ec    = (0x00ff & m_vfat->EC());
      //BC             = m_vfat->BC();
      if (DEBUG) std::cout << "[gemTreeReader]: CRC read from vfat : " << std::hex << m_vfat->crc() << std::endl;
      if (DEBUG) std::cout << "[gemTreeReader]: CRC recalculated   : " << std::hex << m_vfat->crc_calc() << std::endl;
      m_hiDiffCRC->Fill(m_vfat->crc()-m_vfat->crc_calc());
      m_hi2DCRC->Fill(m_vfat->crc(), m_vfat->crc_calc());
      //I think it would be nice to time this...
      uint16_t chan0xf = 0;
      for (int chan = 0; chan < 128; ++chan) {
        if (chan < 64){
          chan0xf = ((m_vfat->lsData() >> chan) & 0x1);
          if(chan0xf) {
             m_hiCh128->Fill(chan);
             firedchannels++;
          } else {
            m_hiCh_notfired->Fill(chan);
            notfiredchannels++;
          }
        } else {
          chan0xf = ((m_vfat->msData() >> (chan-64)) & 0x1);
          if(chan0xf) {
             m_hiCh128->Fill(chan);
             firedchannels++;
          } else {
            m_hiCh_notfired->Fill(chan);
            notfiredchannels++;
          }
        }
      }// end of loop over channels 

      for(Int_t m=0; m < NVFAT; m++){
        if(sn_ == m){
          uint16_t chan0xfFiredchip = 0;
          for (int chan = 0; chan < 128; ++chan) {
            if (chan < 64){
              chan0xfFiredchip = ((m_vfat->lsData() >> chan) & 0x1);
              if(chan0xfFiredchip) m_hiCh128chipFired[m]->Fill(chan);
            } else {
              chan0xfFiredchip = ((m_vfat->msData() >> (chan-64)) & 0x1);
              if(chan0xfFiredchip) m_hiCh128chipFired[m]->Fill(chan);
            }
          }
        } 
      }
 
    }
};
