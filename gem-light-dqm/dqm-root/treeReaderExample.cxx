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
#include "plotter.cxx"
#include "gem/readout/GEMslotContens.h"

/**
  * GEM Tree Reader example (reader) application 
*/

/*! 
   \brief GEM Tree reader reads file with the GEM Event Tree and fills the histogram with number of vfats per event
*/

using namespace std;
    void show64bits(uint64_t x) {
      int i;
      const unsigned long unit = 1;
      for(i=(sizeof(uint64_t)*8)-1; i>=0; i--)
        (x & ((unit)<<i))?putchar('1'):putchar('0');
      printf("\n");
    }

   TH1F *h_VFATfired_perevent[24];
   TH1F *hiCh128chipFired[24];

int main(int argc, char** argv)
{
    if (argc<3) 
    {
        cout << "Please provide input and output filenames" << endl;
        cout << "Usage: <path>/reader inputFile.root outputFile.root" << endl;
        return 0;
    }

    TString ifilename = argv[1];
    TString ofilename = argv[2];

    // read the tree generated with gtc

    TFile *ifile = new TFile(ifilename, "READ");
    TTree *tree = (TTree*)ifile->Get("GEMtree");

   // create a pointer to an event object. This will be used
   // to read the branch values.
   Event *event = new Event();
   // get branch and set the branch address
   TBranch *branch = tree->GetBranch("GEMEvents");
   branch->SetAddress(&event);
   Int_t nentries = tree->GetEntries();
  
   // create the output file
   TFile *ofile = new TFile(ofilename, "RECREATE");
   //book histograms
   TH1F* hiVFAT    = new TH1F("VFAT", "Number_VFAT_blocks_per_event", 100,  0., 100. );
   TH1F* hiVFATsn = new TH1F("VFATsn", "VFAT_slot_number", 24, 0., 24. );
   TH1C* DiffBXandBC   = new TH1C("DiffBXandBC", "difference of BX and BC",         100000, 0x0, 0x1869F );
   TH1C* RatioBXandBC   = new TH1C("RatioBXandBC", "ratio of BX and BC",         1000, 0x0, 0xa );
   TH1C* hiChip    = new TH1C("ChipID", "ChipID",         4096, 0x0, 0xfff );
   TH1C* hi1010    = new TH1C("1010", "Control_Bits_1010", 16, 0x0, 0xf );
   TH1C* hi1100    = new TH1C("1100", "Control_Bits_1100", 16, 0x0, 0xf );
   TH1C* hi1110    = new TH1C("1110", "Control_Bits_1110", 16, 0x0, 0xf );
   TH1C* hiFlag    = new TH1C("Flag"  , "Flag",            16, 0x0, 0xf );
   TH1C* hiCRC     = new TH1C("CRC",     "CRC",             100, 0x0, 0xffff );
   TH1C* hiDiffCRC = new TH1C("DiffCRC", "CRC_Diff",    100, 0xffff, 0xffff );
   TH1C* hiFake    = new TH1C("iFake", "VFAT with Fake_Events",      24, 0., 24. );
   TH1C* hiSignal  = new TH1C("iSignal", "VFAT with Signal_Events",      24, 0., 24. );
   TH1C* hichfired = new TH1C("chfired", "channels fired per event",      500, 0., 500. );
   TH1C* hichnotfired = new TH1C("chnotfired", "channels not fired per event",      500, 0., 500. );
   TH1F* hiCh128   = new TH1F("Ch128", "Strips",          128, 0., 128. );
   TH1F* hCh_notfired = new TH1F("Ch_notfired", "Strips",          128, 0., 128. );
   TH2C* hi2DCRC   = new TH2C("CRC1_vs_CRC2", "CRC1_vs_CRC2", 100, 0x0000, 0xffff, 100, 0x0000, 0xffff);
   //hiVFAT->SetFillColor(48);
   //   
   const int SIZE = 24;
   char name[128], title[500],name1[128], title1[500];
   std::string type[24] = {"0xa64_at_Slot0", "0xe74_at_Slot1", "0xac0_at_Slot2", "0xe98_at_Slot3", "0xe7b_at_Slot4", "0xa9c_at_Slot5", "0xe63_at_Slot6", "0xe6b_at_Slot7", "0xe80_at_Slot8", "0xeaf_at_Slot9", "0xea3_at_Slot10", "0xb44_at_Slot11", "0xe5b_at_Slot12", "0xb40_at_Slot13", "0xeb4_at_Slot14", "0xe5f_at_Slot15", "0xe97_at_Slot16", "0xe9f_at_Slot17", "0xea7_at_Slot18", "0xa84_at_Slot19", "0xa78_at_Slot20", "0xe78_at_Slot21", "0xeab_at_Slot22", "0xe7f_at_Slot23"};
   for(int k=0; k<SIZE; k++){
   sprintf (name, "h_VFATfired_perevent_%s", type[k].c_str());
   sprintf (title, "VFAT chips %s fired per event", type[k].c_str());
   sprintf (name1, "hiCh128chipFired_%s", type[k].c_str());
   sprintf (title1, "Strips fired for VFAT chips %s", type[k].c_str());
   h_VFATfired_perevent[k] = new TH1F(name, title, 20, 0., 20.);
   hiCh128chipFired[k] = new TH1F(name1, title1, 128, 0., 128.);
   }

   Int_t vfatId[SIZE];

   Int_t BX_event;
   Int_t BC_vfat;
   Int_t sn;
   Int_t nVFAT;
   Int_t ifake;
   Int_t iSignal;
   Int_t firedchannels;
   Int_t notfiredchannels;
   // loop over tree entries
   for (Int_t i = 0; i < nentries; i++)
   {
       // clear number of VFATs
       BX_event = 0;
     //  BC_vfat = 0;
       nVFAT = 0;
       firedchannels = 0;
       notfiredchannels = 0;

       for(int j=0; j<SIZE; j++){
	 vfatId[j] = 0;
       }


       // Retrieve next entry
       branch->GetEntry(i);
       cout << std::dec << "Event number====> " << i << std::hex << endl;
       // create vector of GEBdata. For data format details look at Event.h
       vector<GEBdata> v_geb;
       v_geb = event->gebs();
       int BX_event = event->BXID();
       cout << "bunch crossing number from variable ====>  " << BX_event << endl; 

       // loop over gebs  
       for (Int_t j = 0; j < v_geb.size(); j++)
       {
           // create vector of VFATdata. For data format details look at Event.h
           vector<VFATdata> v_vfat;
           v_vfat = v_geb.at(j).vfats();
           // Increment number of VFATs in the given event
           nVFAT += v_vfat.size();
           // loop over vfats
           //gem::readout::printVFATdataBits(i, vfat);
 
           ifake = 0;
           iSignal = 0;
           
           for (Int_t k = 0; k < v_vfat.size(); k++)
           {


            if ( (v_vfat.at(k).b1010() == 0xa) && (v_vfat.at(k).b1100() == 0xc) && (v_vfat.at(k).b1110() == 0xe) )
                  {
                    iSignal++;
                    cout << dec << "BC of the vfat inside loop===> "  <<  v_vfat.at(k).BC() << hex << endl;   
                    cout << "EC of the vfat inside loop===> "  << v_vfat.at(k).EC() << endl;

                    BC_vfat = v_vfat.at(k).BC();

                  // fill the control bits histograms
                    hi1010->Fill(v_vfat.at(k).b1010());
                    hi1100->Fill(v_vfat.at(k).b1100());
                    hi1110->Fill(v_vfat.at(k).b1110());
                    // fill Flag and chip id histograms
                    hiFlag->Fill(v_vfat.at(k).Flag());
                    hiChip->Fill(v_vfat.at(k).ChipID());
                    // calculate and fill VFAT slot number
                    uint32_t t_chipID = static_cast<uint32_t>(v_vfat.at(k).ChipID());
                    sn = gem::readout::GEBslotIndex(t_chipID);
                    hiVFATsn->Fill(sn);

                    vfatId[sn]++;
	
                   	    
                   for(Int_t m=0; m<24; m++){
                       if(sn == m){
                    uint16_t chan0xfFiredchip = 0;
                    for (int chan = 0; chan < 128; ++chan) {
                      if (chan < 64){

                    chan0xfFiredchip = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                      if(chan0xfFiredchip != 0) hiCh128chipFired[m]->Fill(chan);
                      } else {
                      chan0xfFiredchip = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                      if(chan0xfFiredchip != 0) hiCh128chipFired[m]->Fill(chan);
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
                    //dataChecker *dc = new dataChecker();
                    gem::datachecker::GEMDataChecker *dc = new gem::datachecker::GEMDataChecker::GEMDataChecker(); 

                    uint16_t checkedCRC = dc->checkCRC(dataVFAT, 0);
                    std::cout << "read  crc            " << std::hex << v_vfat.at(k).crc() << std::endl;
                    std::cout << "check crc            " << std::hex << checkedCRC << std::endl;
                    hiDiffCRC->Fill(v_vfat.at(k).crc()-checkedCRC);
                    hi2DCRC->Fill(v_vfat.at(k).crc(), checkedCRC);
                    delete dc;
                    //I think it would be nice to time this...
                    //show64bits(v_vfat.at(k).lsData());
                    //show64bits(v_vfat.at(k).msData());
                   
 
                    uint16_t chan0xf = 0;
                    for (int chan = 0; chan < 128; ++chan) {

                        if (chan < 64){
                           chan0xf = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                        if(chan0xf != 0) {
                             hiCh128->Fill(chan);
                             firedchannels++;
                        }
                         if(chan0xf == 0) {
                           hCh_notfired->Fill(chan);
                           notfiredchannels++;
                         }
                       } else {
                          chan0xf = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                          if(chan0xf != 0) {
                             hiCh128->Fill(chan);
                             firedchannels++;
                          }
                          if(chan0xf == 0) {
                             hCh_notfired->Fill(chan);
                             notfiredchannels++;
                          }
                        }
                      }
                    } else {
                        cout << "it is a fake event in vfat " << endl;
                        ifake++;
                    }

              }
        }
        
        if(iSignal != 0){     
        int diffBXandBC =  fabs(BX_event - BC_vfat);  
        //float ratioBXandBC;  
	double ratioBXandBC = (double) BC_vfat / BX_event;

        DiffBXandBC->Fill(diffBXandBC); 
        RatioBXandBC->Fill(ratioBXandBC);
        }

        hiVFAT->Fill(nVFAT);
	for(Int_t x=0; x<24; x++)
        	h_VFATfired_perevent[x]->Fill(vfatId[x]);
        hiFake->Fill(ifake);
        hiSignal->Fill(iSignal);
        hichfired->Fill(firedchannels);
        hichnotfired->Fill(firedchannels);
    }

    setTitles(RatioBXandBC, "Ratio of BX and BC per event", "Number of Events");
    setTitles(DiffBXandBC, "BX - BC per event", "Number of Events");
    setTitles(hiVFAT, "Number VFAT blocks per Event", "Number of Events");  
    setTitles(hi1010, "1010 marker, max 0xf", "Number of Events");   
    setTitles(hi1100, "1100 marker, max 0xf", "Number of Events");   
    setTitles(hi1110, "1110 marker, max 0xf", "Number of Events");   
    setTitles(hiFlag, "Flag marker value, max 0xf", "Number of Events");   
    setTitles(hiCRC, "CRC value, max 0xffff", "Number of Events");
    setTitles(hiDiffCRC, "CRC difference", "Number of Events");
    setTitles(hiFake, "Vfat with Fake events", "Number of Events");
    setTitles(hiSignal, "Vfat with Signal events", "Number of Events");
    setTitles(hichfired, "Number of fired channels", "Number of Events");
    setTitles(hichnotfired, "Number of not fired channels", "Number of Events");
    setTitles(hiCh128, "Strips, max 128", "Number of hits"); 
    setTitles(hCh_notfired, "Strips, max 128", "Number of times not fired");
    setTitles(hi2DCRC, "CRC VFAT", "CRC calc"); 

    ofile->Write();
    
    TStyle * style = getStyle("ZZ");
    gROOT->SetStyle("ZZStyle");
    gROOT->ForceStyle();
    printHistograms("pdf", "data/hist/");
    printHistograms("png", "data/hist/");
    printHistograms("C", "data/hist/");
    printHistograms("root", "data/hist/");
    
    return 0;
}








