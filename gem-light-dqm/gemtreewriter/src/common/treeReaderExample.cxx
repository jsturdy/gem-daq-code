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
#include "gem/readout/GEMslotContents.h"

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
   TH1F* hiVFATchipa64 = new TH1F("hiVFATchipa64", "VFAT chip 0xa64 fired per event", 20, 0., 20. );
   TH1F* hiVFATchipe63 = new TH1F("hiVFATchipe63", "VFAT chip 0xe63 fired per event", 20, 0., 20. );
   TH1F* hiVFATchipe6b = new TH1F("hiVFATchipe6b", "VFAT chip 0xe6b fired per event", 20, 0., 20. );
   TH1F* hiVFATchipac0 = new TH1F("hiVFATchipac0", "VFAT chip 0xac0 fired per event", 20, 0., 20. );
   TH1F* hiVFATchipe98 = new TH1F("hiVFATchipe98", "VFAT chip 0xe98 fired per event", 20, 0., 20. );
   TH1F* hiVFATchipe7b = new TH1F("hiVFATchipe7b", "VFAT chip 0xe7b fired per event", 20, 0., 20. );
   TH1F* hiVFATchipa9c = new TH1F("hiVFATchipa9c", "VFAT chip 0xa9c fired per event", 20, 0., 20. );
   TH1F* hiVFATchipe74 = new TH1F("hiVFATchipe74", "VFAT chip 0xe74 fired per event", 20, 0., 20. );
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
   TH1F* hiCh128chip0xa64 = new TH1F("hiCh128chip0xa64", "Strips for chip 0xa64",          128, 0., 128. );
   TH1F* hiCh128chip0xe63 = new TH1F("hiCh128chip0xe63", "Strips for chip 0xe63",          128, 0., 128. );
   TH1F* hiCh128chip0xe6b = new TH1F("hiCh128chip0xe6b", "Strips for chip 0xe6b",          128, 0., 128. );
   TH1F* hiCh128chip0xac0 = new TH1F("hiCh128chip0xac0", "Strips for chip 0xac0",          128, 0., 128. );
   TH1F* hiCh128chip0xe98 = new TH1F("hiCh128chip0xe98", "Strips for chip 0xe98",          128, 0., 128. );
   TH1F* hiCh128chip0xe7b = new TH1F("hiCh128chip0xe7b", "Strips for chip 0xe7b",          128, 0., 128. );
   TH1F* hiCh128chip0xa9c = new TH1F("hiCh128chip0xa9c", "Strips for chip 0xa9c",          128, 0., 128. );
   TH1F* hiCh128chip0xe74 = new TH1F("hiCh128chip0xe74", "Strips for chip 0xe74",          128, 0., 128. );   
   TH1F* hCh_notfired = new TH1F("Ch_notfired", "Strips",          128, 0., 128. );
   TH2C* hi2DCRC   = new TH2C("CRC1_vs_CRC2", "CRC1_vs_CRC2", 100, 0x0000, 0xffff, 100, 0x0000, 0xffff);
   //hiVFAT->SetFillColor(48);
   //   

   Int_t nVFAT;
   Int_t chipa64;
   Int_t chipe63;
   Int_t chipe6b;
   Int_t chipac0;
   Int_t chipe98;
   Int_t chipe7b;
   Int_t chipa9c;
   Int_t chipe74;
   Int_t ifake;
   Int_t iSignal;
   Int_t firedchannels;
   Int_t notfiredchannels;
   // loop over tree entries
   for (Int_t i = 0; i < nentries; i++)
   {
       // clear number of VFATs
       nVFAT = 0;
       chipa64 = 0;
       chipe63 = 0;
       chipe6b = 0;
       chipac0 = 0;
       chipe98 = 0;
       chipe7b = 0;
       chipa9c = 0;
       chipe74 = 0;
       firedchannels = 0;
       notfiredchannels = 0;

       // Retrieve next entry
       branch->GetEntry(i);
       cout << std::dec << "Event number====> " << i << std::hex << endl;
       // create vector of GEBdata. For data format details look at Event.h
       vector<GEBdata> v_geb;
       v_geb = event->gebs();
       cout << "size of GEB in a event " << v_geb.size() << endl;
       cout << dec << "bunch crossing number in a event====>  " << event->BXID() << hex << endl;      

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
           cout << "vfat size"  << v_vfat.size() << endl;       
 
           ifake = 0;
           iSignal = 0;
           
           for (Int_t k = 0; k < v_vfat.size(); k++)
           {

              cout << "counter on vfat size"  << k << endl;      
              cout << "chip id of the vfat "  << v_vfat.at(k).ChipID() << endl;
              cout << "BC of the vfat "  << v_vfat.at(k).BC() << endl;            


            if ( (v_vfat.at(k).b1010() == 0xa) && (v_vfat.at(k).b1100() == 0xc) && (v_vfat.at(k).b1110() == 0xe) )
                  {
                    iSignal++;
                    cout << dec << "BC of the vfat inside loop===> "  <<  v_vfat.at(k).BC() << hex << endl;   
                    cout << "EC of the vfat inside loop===> "  << v_vfat.at(k).EC() << endl;
                    cout << dec << "chip id of the vfat inside loop===> "   << v_vfat.at(k).ChipID() << hex << endl;
                    // fill the control bits histograms
                    hi1010->Fill(v_vfat.at(k).b1010());
                    hi1100->Fill(v_vfat.at(k).b1100());
                    hi1110->Fill(v_vfat.at(k).b1110());
                    // fill Flag and chip id histograms
                    hiFlag->Fill(v_vfat.at(k).Flag());
                    hiChip->Fill(v_vfat.at(k).ChipID());
                    // calculate and fill VFAT slot number
                    //uint32_t t_chipID = static_cast<uint32_t>(v_vfat.at(k).ChipID());
                    //int sn = gem::readout::GEBslotIndex(t_chipID);
                    //hiVFATsn->Fill(sn);

                    if(v_vfat.at(k).ChipID() == 0xa64)
                    {
                      chipa64++;

                    uint16_t chan0xfchip1 = 0;
                    for (int chan = 0; chan < 128; ++chan) {
                    
                      if (chan < 64){
                      chan0xfchip1 = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                      if(chan0xfchip1 != 0) hiCh128chip0xa64->Fill(chan);
                      } else {
                      chan0xfchip1 = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                      if(chan0xfchip1 != 0) hiCh128chip0xa64->Fill(chan);       
                      }
                     }
                    }   
                  
                    if(v_vfat.at(k).ChipID() == 0xe63)
                    {
                      chipe63++;

                    uint16_t chan0xfchip2 = 0;
                    for (int chan = 0; chan < 128; ++chan) {

                      if (chan < 64){
                      chan0xfchip2 = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                      if(chan0xfchip2 != 0) hiCh128chip0xe63->Fill(chan);
                      } else {
                      chan0xfchip2 = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                      if(chan0xfchip2 != 0) hiCh128chip0xe63->Fill(chan);
                      }
                     }

                    }
                    
                    if(v_vfat.at(k).ChipID() == 0xe6b)
                    {
                      chipe6b++;

                      uint16_t chan0xfchip3 = 0;
                      for (int chan = 0; chan < 128; ++chan) {

                      if (chan < 64){
                      chan0xfchip3 = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                      if(chan0xfchip3 != 0) hiCh128chip0xe6b->Fill(chan);
                      } else {
                      chan0xfchip3 = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                      if(chan0xfchip3 != 0) hiCh128chip0xe6b->Fill(chan);
                      }
                     }


                    }

                    if(v_vfat.at(k).ChipID() == 0xac0)
                    {
                      chipac0++;
 
                    uint16_t chan0xfchip4 = 0;
                    for (int chan = 0; chan < 128; ++chan) {

                      if (chan < 64){
                      chan0xfchip4 = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                      if(chan0xfchip4 != 0) hiCh128chip0xac0->Fill(chan);
                      } else {
                      chan0xfchip4 = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                      if(chan0xfchip4 != 0) hiCh128chip0xac0->Fill(chan);
                      }
                     }

                    }

                    if(v_vfat.at(k).ChipID() == 0xe98)
                    {
                      chipe98++;

                      uint16_t chan0xfchip5 = 0;
                      for (int chan = 0; chan < 128; ++chan) {

                      if (chan < 64){
                      chan0xfchip5 = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                      if(chan0xfchip5 != 0) hiCh128chip0xe98->Fill(chan);
                      } else {
                      chan0xfchip5 = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                      if(chan0xfchip5 != 0) hiCh128chip0xe98->Fill(chan);
                      }
                     }


                    }
                  
                    if(v_vfat.at(k).ChipID() == 0xe7b)
                    {
                      chipe7b++;

                     uint16_t chan0xfchip6 = 0;
                      for (int chan = 0; chan < 128; ++chan) {

                      if (chan < 64){
                      chan0xfchip6 = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                      if(chan0xfchip6 != 0) hiCh128chip0xe7b->Fill(chan);
                      } else {
                      chan0xfchip6 = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                      if(chan0xfchip6 != 0) hiCh128chip0xe7b->Fill(chan);
                      }
                     }

                    }

                    if(v_vfat.at(k).ChipID() == 0xa9c)
                    {
                      chipa9c++;

                     uint16_t chan0xfchip7 = 0;
                      for (int chan = 0; chan < 128; ++chan) {

                      if (chan < 64){
                      chan0xfchip7 = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                      if(chan0xfchip7 != 0) hiCh128chip0xa9c->Fill(chan);
                      } else {
                      chan0xfchip7 = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                      if(chan0xfchip7 != 0) hiCh128chip0xa9c->Fill(chan);
                      }
                     }

                    }

                   if(v_vfat.at(k).ChipID() == 0xe74)
                    {
                      chipe74++;

                    uint16_t chan0xfchip8 = 0;
                      for (int chan = 0; chan < 128; ++chan) {

                      if (chan < 64){
                      chan0xfchip8 = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                      if(chan0xfchip8 != 0) hiCh128chip0xe74->Fill(chan);
                      } else {
                      chan0xfchip8 = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                      if(chan0xfchip8 != 0) hiCh128chip0xe74->Fill(chan);
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
                           //cout << "ls data from vfat   " << v_vfat.at(k).lsData() << endl;
                           //cout << "vfat channel   " << chan << endl;
                           //show64bits(v_vfat.at(k).lsData());
                           chan0xf = ((v_vfat.at(k).lsData() >> chan) & 0x1);
                           //show64bits(v_vfat.at(k).lsData() >> chan);
                           //cout << "ls vfat data after channel shifting   " << (v_vfat.at(k).lsData() >> chan) << endl;
                           //cout << "chan0xf   " << chan0xf << endl;
                           //histos[chan]->Fill(chan0xf);
                        if(chan0xf != 0) {
                            //show64bits(v_vfat.at(k).msData());
                            //cout << "chan0xf inside loop   " << chan0xf << endl;
                             hiCh128->Fill(chan);
                             firedchannels++;
                             //cout << "counter on fired channels   " << firedchannels << endl;
                             //cout << "channel fired   " << chan << endl;
                        }
                         if(chan0xf == 0) {
                            //cout << "counter on not fired channels   " << notfiredchannels << endl;
                           //cout << "channel not fired   " << chan << endl;
                           hCh_notfired->Fill(chan);
                           notfiredchannels++;
                         }
                       } else {
                          //show64bits(v_vfat.at(k).msData());
                          chan0xf = ((v_vfat.at(k).msData() >> (chan-64)) & 0x1);
                          //histos[chan]->Fill(chan0xf);
                          if(chan0xf != 0) {
                             hiCh128->Fill(chan);
                             firedchannels++;
                             //cout << "counter on fired channels   " << firedchannels << endl;
                          }
                          if(chan0xf == 0) {
                             hCh_notfired->Fill(chan);
                             notfiredchannels++;
                             //cout << "counter on not fired channels   " << notfiredchannels << endl;
                          }
                        }
                      }
                    } else {
                        cout << "it is a fake event in vfat " << endl;
                        ifake++;
                    }

              }
        }
        cout << "number of vfat with signal event " << iSignal << endl;
        cout << "number of vfat with fake event " << ifake << endl;
        cout << "number of vfat in a event " << nVFAT << endl;
        cout << "number of a64 vfat chip fired in a event " << chipa64 << endl;
        cout << "number of channels fired in a event " << firedchannels << endl;
        cout << "number of channels not fired in a event " << notfiredchannels << endl;
        hiVFAT->Fill(nVFAT);
        hiVFATchipa64->Fill(chipa64);
        hiVFATchipe63->Fill(chipe63);
        hiVFATchipe6b->Fill(chipe6b);
        hiVFATchipac0->Fill(chipac0);
        hiVFATchipe98->Fill(chipe98);
        hiVFATchipe7b->Fill(chipe7b);
        hiVFATchipa9c->Fill(chipa9c);
        hiVFATchipe74->Fill(chipe74);
        hiFake->Fill(ifake);
        hiSignal->Fill(iSignal);
        hichfired->Fill(firedchannels);
        hichnotfired->Fill(firedchannels);
    }

    setTitles(hiVFAT, "Number VFAT blocks per Event", "Number of Events");  
    setTitles(hiVFATchipa64, "VFAT chip 0xa64 fired per Event", "Number of Events"); 
    setTitles(hiVFATchipe74, "VFAT chip 0xe74 fired per Event", "Number of Events");
    setTitles(hiVFATchipe63, "VFAT chip 0xe63 fired per Event", "Number of Events");
    setTitles(hiVFATchipe6b, "VFAT chip 0xe6b fired per Event", "Number of Events");
    setTitles(hiVFATchipac0, "VFAT chip 0xac0 fired per Event", "Number of Events");
    setTitles(hiVFATchipe98, "VFAT chip 0xe98 fired per Event", "Number of Events");
    setTitles(hiVFATchipe7b, "VFAT chip 0xe7b fired per Event", "Number of Events");
    setTitles(hiVFATchipa9c, "VFAT chip 0xa9c fired per Event", "Number of Events");
    setTitles(hiChip, "ChipID value, max 0xfff", "Number of Events");
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
    setTitles(hiCh128chip0xa64, "Strips, max 128", "Number of hits");
    setTitles(hiCh128chip0xe63, "Strips, max 128", "Number of hits");
    setTitles(hiCh128chip0xe6b, "Strips, max 128", "Number of hits");
    setTitles(hiCh128chip0xac0, "Strips, max 128", "Number of hits");
    setTitles(hiCh128chip0xe98, "Strips, max 128", "Number of hits");
    setTitles(hiCh128chip0xe7b, "Strips, max 128", "Number of hits");
    setTitles(hiCh128chip0xa9c, "Strips, max 128", "Number of hits");
    setTitles(hiCh128chip0xe74, "Strips, max 128", "Number of hits");
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








