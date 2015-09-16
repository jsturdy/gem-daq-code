#include <iomanip> 
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <cstdint>
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
#include "gem/readout/GEMslotContens.h"
#include "plotter.cxx"

/**
* GEM Tree Reader example (reader) application 
*/

/*! 
  \brief GEM Tree reader reads file with the GEM Event Tree and fills the histogram with number of vfats per event
*/

using namespace std;
//namespace gem {
//  namespace datachecker{
//    class GEMDataChecker;
//  }
//}
class treeReader {
  public:
    treeReader(const std::string &ifilename)
    {
      std::string tmp = ifilename.substr(ifilename.size()-9, ifilename.size());
      if (tmp != ".raw.root") throw std::runtime_error("Wrong input filename (should end with '.raw.root'): "+ifilename);
      ifile = new TFile(ifilename, "READ");
      this->bookHistograms();
      std::string ofilename = ifilename.substr(0,ifilename.size()-9);
      ofilename += ".analyzed.root";
      ofile = new TFile(ofilename, "RECREATE");
    }
    ~treeReader(){}
    void createHistograms()
    {
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
    void bookHistograms()
    {
      hiVFAT    = new TH1F("VFAT", "Number_VFAT_blocks_per_event", 24,  0., 24. );
      hiVFATsn  = new TH1F("VFATsn", "VFAT_slot_number", 24,  0., 24. );
      hiChip    = new TH1C("ChipID", "ChipID",         4096, 0x0, 0xfff );
      hi1010    = new TH1C("1010", "Control_Bits_1010", 16, 0x0, 0xf );
      hi1100    = new TH1C("1100", "Control_Bits_1100", 16, 0x0, 0xf );
      hi1110    = new TH1C("1110", "Control_Bits_1110", 16, 0x0, 0xf );
      hiFlag    = new TH1C("Flag"  , "Flag",            16, 0x0, 0xf );
      hiCRC     = new TH1C("CRC",     "CRC",             100, 0x0, 0xffff );
      hiDiffCRC = new TH1C("DiffCRC", "CRC_Diff",    100, 0xffff, 0xffff );
      hiFake    = new TH1C("iFake", "Fake_Events",      100, 0., 100. );
      hiCh128   = new TH1F("Ch128", "Strips",          128, 0., 128. );
      hi2DCRC   = new TH2C("CRC1_vs_CRC2", "CRC1_vs_CRC2", 100, 0x0000, 0xffff, 100, 0x0000, 0xffff);
    }
    void fillHistograms()
    {
      TTree *tree = (TTree*)ifile->Get("GEMtree");
      Event *event = new Event();
      TBranch *branch = tree->GetBranch("GEMEvents");
      branch->SetAddress(&event);
      Int_t nentries = tree->GetEntries();
      Int_t nVFAT = 0;
      Int_t ifake = 0;
      // loop over tree entries
      for (Int_t i = 0; i < nentries; i++)
      {
          // clear number of VFATs
          nVFAT = 0;
          // Retrieve next entry
          branch->GetEntry(i);
          cout << "Event number " << i << endl;
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
                  //if ( (v_vfat.at(k).b1010() == 0xa) && (v_vfat.at(k).b1100() == 0xc) && (v_vfat.at(k).b1110() == 0xe) )
                  //    {
                          // fill the control bits histograms
                          hi1010->Fill(v_vfat.at(k).b1010());
                          hi1100->Fill(v_vfat.at(k).b1100());
                          hi1110->Fill(v_vfat.at(k).b1110());
                          // fill Flag and chip id histograms
                          hiFlag->Fill(v_vfat.at(k).Flag());
                          hiChip->Fill(v_vfat.at(k).ChipID());
                          // calculate and fill VFAT slot number
                          uint32_t t_chipID = static_cast<uint32_t>(v_vfat.at(k).ChipID());
                          int sn = gem::readout::GEBslotIndex(t_chipID);
                          hiVFATsn->Fill(sn);
                          // calculate and fill the crc and crc_diff
                          hiCRC->Fill(v_vfat.at(k).crc());
                          uint16_t dataVFAT[11];
                          // CRC check
                          uint16_t b1010 = (0x000f & v_vfat.at(k).b1010());
                          uint16_t b1100 = (0x000f & v_vfat.at(k).b1100());
                          uint16_t b1110 = (0x000f & v_vfat.at(k).b1110());
                          uint16_t flag = (0x000f & v_vfat.at(k).Flag());
                          uint16_t ec = (0x00ff & v_vfat.at(k).EC());
                          dataVFAT[11] = (0xf000 & (b1010 << 12)) | (0x0fff & v_vfat.at(k).BC());
                          dataVFAT[10] = ((0xf000 & (b1100 << 12)) | (0x0ff0 & (ec << 4))) | (0x000f & flag);
                          dataVFAT[9]  = (0xf000 & (b1110 << 12)) | (0x0fff & v_vfat.at(k).ChipID());
                          dataVFAT[8]  = (0xffff000000000000 & v_vfat.at(k).msData()) >> 48;
                          dataVFAT[7]  = (0x0000ffff00000000 & v_vfat.at(k).msData()) >> 32;
                          dataVFAT[6]  = (0x00000000ffff0000 & v_vfat.at(k).msData()) >> 16;
                          dataVFAT[5]  = (0x000000000000ffff & v_vfat.at(k).msData());
                          dataVFAT[4]  = (0xffff000000000000 & v_vfat.at(k).lsData()) >> 48;
                          dataVFAT[3]  = (0x0000ffff00000000 & v_vfat.at(k).lsData()) >> 32;
                          dataVFAT[2]  = (0x00000000ffff0000 & v_vfat.at(k).lsData()) >> 16;
                          dataVFAT[1]  = (0x000000000000ffff & v_vfat.at(k).lsData());
                          //GEMDataChecker *dc = new GEMDataChecker();
                          gem::datachecker::GEMDataChecker *dc = new gem::datachecker::GEMDataChecker::GEMDataChecker();
                          //uint16_t checkedCRC = dc->gem::datachecker::GEMDataChecker::checkCRC(dataVFAT, 0);
                          uint16_t checkedCRC = dc->checkCRC(dataVFAT, 0);
                          std::cout << "read  crc            " << std::hex << v_vfat.at(k).crc() << std::endl;
                          std::cout << "check crc            " << std::hex << checkedCRC << std::endl;
                          hiDiffCRC->Fill(v_vfat.at(k).crc()-checkedCRC);
                          hi2DCRC->Fill(v_vfat.at(k).crc(), checkedCRC);
                          delete dc;
                          //I think it would be nice to time this...
                          uint16_t chan0xf = 0;
                          for (int chan = 0; chan < 128; ++chan) {
                            if (chan < 64){
                              chan0xf = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                              //histos[chan]->Fill(chan0xf);
                            if(!chan0xf) {
                               hiCh128->Fill(chan);
                            }
                            } else {
                              chan0xf = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                              //histos[chan]->Fill(chan0xf);
                            if(!chan0xf) hiCh128->Fill(chan);
                            }
                          }
                      //} else {
                      //    ifake++;
                      //}
              }
          }
          hiVFAT->Fill(nVFAT);
          hiFake->Fill(ifake);
      }
      
      setTitles(hiVFAT, "Number VFAT blocks per Event", "Number of Events");   
      setTitles(hiChip, "ChipID value, max 0xfff", "Number of VFAT blocks");
      setTitles(hi1010, "1010 marker, max 0xf", "Number of VFAT blocks");   
      setTitles(hi1100, "1100 marker, max 0xf", "Number of VFAT blocks");   
      setTitles(hi1110, "1110 marker, max 0xf", "Number of VFAT blocks");   
      setTitles(hiFlag, "Flag marker value, max 0xf", "Number of VFAT blocks");   
      setTitles(hiCRC, "CRC value, max 0xffff", "Number of VFAT blocks");
      setTitles(hiDiffCRC, "CRC difference", "Number of VFAT blocks");
      setTitles(hiFake, "Fake events", "Number of Events");
      setTitles(hiCh128, "Strips, max 128", "Number of VFAT blocks"); 
      setTitles(hi2DCRC, "CRC VFAT", "CRC calc");  
      
      ofile->Write();
    }

};
