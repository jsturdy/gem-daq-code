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
#include <TGaxis.h>
#include <TFrame.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TString.h>
#include <TRandom3.h>
#include <TBenchmark.h>
#include <TInterpreter.h>
#include <TApplication.h>

#include "gem/readout/GEMDataAMCformat.h"
#include "gem/datachecker/GEMDataChecker.h"

/**
* ... Threshold Scan ROOT based application, could be used for analisys of XDAQ GEM data ...
*/

/*! \file */
/*! 
  \mainpage DQM Light ROOT based application.

  GEM events data reading program for any GEM DQM light application, needs for ROOT analysis.

  \section Installation

  It's possible to make a GEM data file by xDAQ GEMGLIBsuprvisor or get as example from CERN web:

  ln -s /afs/cern.ch/user/b/baranov/www/xdaq/Testing/15.July.2015/GEM_DAQ_Fri_Jul_17_14-46-56_2015.dat GEMDQMRawData.dat

  You need a ROOT code for analysis:

  git clone git@github.com:cms-gem-daq-project/gem-daq-code.git <br>

  cd gem-daq-code
    cd dqm-light
    mkdir work
    ../scripts/compile_code_for_root.sh ../src/common/gem-read-events.cc

  That is all. You will have a root file DQMlight.root with few monitoring data histograms.

  \author Sergey.Baranov@cern.ch, Mykhailo Dalchenko <mexxxanick@gmail.com> 
*/

using namespace std;

//! GEM VFAT2 Data class.
/*!
  \brief GEMOnline
  contents VFAT2 GEM data format 
  \author Sergey.Baranov@cern.ch
*/

/*
 *  CRC ******************************************************************
 */
      uint16_t dataVFAT[11];

      uint16_t crc_calc(uint16_t crc_in, uint16_t dato){
      uint16_t v = 0x0001;
      uint16_t mask = 0x0001;    
      bool d=0;
      uint16_t crc_temp = crc_in;
      unsigned char datalen = 16;
       
      for (int i=0; i<datalen; i++){
        if (dato & v) d = 1;
        else d = 0;
        if ((crc_temp & mask)^d) crc_temp = crc_temp>>1 ^ 0x8408;
        else crc_temp = crc_temp>>1;
        v<<=1;
      }
      return(crc_temp);
    }

    // unsigned short int 
    uint16_t checkCRC(){
      uint16_t crc_fin = 0xffff;
      for (int i = 11; i >= 1; i--){
        crc_fin = crc_calc(crc_fin, dataVFAT[i]);
      }
      return(crc_fin);
    }

    // Ok printing
    bool OKprint(int ievent, int iMaxPrint ){
      if( ievent <= iMaxPrint ){
        return (true);
      } else { 
        return (false);
      }
    }
    

//! root function.
/*!
https://root.cern.ch/drupal/content/documentation
*/

TROOT root("",""); // static TROOT object

//! main function.
/*!
C++ any documents
*/

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

  /* Threshould Analysis Histograms */
  const TString filename = "DQMlight.root";

  // Create a new canvas.
  TCanvas *c1 = new TCanvas("c1","Dynamic Filling Example",0,0,950,950);
  gROOT->SetStyle("Plain");
  gStyle->GetAttDate()->SetTextColor(1);
  gStyle->SetOptStat(111111);

  TGaxis *axis1 = new TGaxis(-4.5,-0.2,5.5,-0.2,-6,8,510,"");
  axis1->SetMaxDigits(3);

  c1->SetFillColor(42);
  c1->GetFrame()->SetFillColor(21);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);
  c1->Divide(3,3);

  TFile* hfile = NULL;
  hfile = new TFile(filename,"RECREATE","Threshold Scan ROOT file with histograms");

  TH1F* hiVFAT = new TH1F("VFAT", "Number VFAT blocks per event", 100,  0., 100. );
  hiVFAT->SetFillColor(48);
  //hiVFAT->SetStats();
  hiVFAT->GetXaxis()->SetTitle("Number of VFAT Blocks");
  hiVFAT->GetXaxis()->CenterTitle();
  hiVFAT->GetYaxis()->SetTitle("Number of Event");
  hiVFAT->GetYaxis()->CenterTitle();
 
  TH1C* hiChip = new TH1C("ChipID", "ChipID",         4096, 0x0, 0xfff );
  hiChip->SetFillColor(48);
  hiChip->GetXaxis()->SetTitle("ChipID value, max 0xfff");
  hiChip->GetXaxis()->CenterTitle();
  hiChip->GetYaxis()->SetTitle("Number of VFAT Blocks");
  hiChip->GetYaxis()->CenterTitle();
 
  TH1C* hiBX = new TH1C("BX",     "BX from OH",      100, 0x0, 0xffff );
  hiBX->SetFillColor(48);
  hiBX->GetXaxis()->SetTitle("BX value, max 0xffff");
  hiBX->GetXaxis()->CenterTitle();
  hiBX->GetYaxis()->SetTitle("Number of VFAT Blocks");
  hiBX->GetYaxis()->CenterTitle();
 
  TH1C* hi1010 = new TH1C("1010", "Control Bits 1010", 16, 0x0, 0xf );
  hi1010->SetFillColor(48);
  hi1010->GetXaxis()->SetTitle("1010 marker, max 0xf");
  hi1010->GetXaxis()->CenterTitle();
  hi1010->GetYaxis()->SetTitle("Number of VFAT Blocks");
  hi1010->GetYaxis()->CenterTitle();

  TH1C* hi1100 = new TH1C("1100", "Control Bits 1100", 16, 0x0, 0xf );
  hi1100->SetFillColor(48);
  hi1100->GetXaxis()->SetTitle("1100 marker value, max 0xf");
  hi1100->GetXaxis()->CenterTitle();
  hi1100->GetYaxis()->SetTitle("Number of VFAT Blocks");
  hi1100->GetYaxis()->CenterTitle();

  TH1C* hi1110 = new TH1C("1110", "Control Bits 1110", 16, 0x0, 0xf );
  hi1110->SetFillColor(48);
  hi1110->GetXaxis()->SetTitle("1110 marker value, max 0xf");
  hi1110->GetXaxis()->CenterTitle();
  hi1110->GetYaxis()->SetTitle("Number of VFAT Blocks");
  hi1110->GetYaxis()->CenterTitle();

  TH1C* hiFlag = new TH1C("Flag"  , "Flag",            16, 0x0, 0xf );
  hiFlag->SetFillColor(48);
  hiFlag->GetXaxis()->SetTitle("Flag marker value, max 0xf");
  hiFlag->GetXaxis()->CenterTitle();
  hiFlag->GetYaxis()->SetTitle("Number of VFAT Blocks");
  hiFlag->GetYaxis()->CenterTitle();

  /*
  TH1C* hiCRC = new TH1C("CRC",     "CRC",             100, 0x0, 0xffff );
  hiCRC->SetFillColor(48);
  hiCRC->GetXaxis()->SetTitle("CRC value, max 0xffff");
  hiCRC->GetXaxis()->CenterTitle();
  hiCRC->GetYaxis()->SetTitle("Number of Events");
  hiCRC->GetYaxis()->CenterTitle();
  */

  TH2C* hiVsCRC = new TH2C("CRC", "CRC vs calCRC",     100, 0xffff, 0xffff, 100, 0xffff, 0xffff);
  hiVsCRC->SetFillColor(48);
  hiVsCRC->GetXaxis()->SetTitle("CRC vs CRC calc, max 0xffff");
  hiVsCRC->GetXaxis()->CenterTitle();

  TH1C* hiFake = new TH1C("iFake", "Fake Events",      100, 0., 100. );
  hiFake->SetFillColor(48);

  // Booking of all 128 histograms for each VFAT2 channel
  TH1F* hiCh128 = new TH1F("Ch128", "Strips",          128, 0., 128. );
  hiCh128->SetFillColor(48);
  hiCh128->GetXaxis()->SetTitle("Strips, max 128");
  hiCh128->GetXaxis()->CenterTitle();
  hiCh128->GetYaxis()->SetTitle("Number of VFAT Blocks");
  hiCh128->GetYaxis()->CenterTitle();

  stringstream histName, histTitle;
  TH1F* histos[128];

  for (unsigned int hi = 0; hi < 128; ++hi) {
    histName.clear(); histName.str(std::string());
    histTitle.clear(); histTitle.str(std::string());
    histName  << "channel"<<(hi+1);
    histTitle << "Threshold scan for channel "<<(hi+1);
    histos[hi] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), 100, 0., 0xf );
  }

  const Int_t ieventPrint = 1;
  const Int_t ieventMax   = 90000;
  const Int_t kUPDATE     = 25;
  bool  OKpri = false;

  for(int ievent=0; ievent<ieventMax; ievent++){
    OKpri = OKprint(ievent,ieventPrint);
    if(inpf.eof()) break;
    if(!inpf.good()) break;

    if(OKpri) cout << "\nievent " << ievent << endl;

    // read Event Chamber Header
    gem::readout::readGEBheader(inpf, geb);
    //if(OKpri) gem::readout::printGEBheader(ievent,geb);

    uint64_t ZSFlag  = (0xffffff0000000000 & geb.header) >> 40; 
    uint64_t ChamID  = (0x000000fff0000000 & geb.header) >> 28; 
    uint64_t sumVFAT = (0x000000000fffffff & geb.header);

    int iSumVFAT = 0;
    int ifake = 0;
    for(int ivfat=0; ivfat<sumVFAT; ivfat++){
      iSumVFAT++;

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
      uint16_t  BX     = vfat.BXfrOH;  

      //      if ( (b1010 == 0xa) && (b1100==0xc) && (b1110==0xe) /* && (ChipID==0x68) */ ){

        // CRC check
        dataVFAT[11] = vfat.BC;
        dataVFAT[10] = vfat.EC;
        dataVFAT[9]  = vfat.ChipID;
        dataVFAT[8]  = (0xffff000000000000 & vfat.msData) >> 48;
        dataVFAT[7]  = (0x0000ffff00000000 & vfat.msData) >> 32;
        dataVFAT[6]  = (0x00000000ffff0000 & vfat.msData) >> 16;
        dataVFAT[5]  = (0x000000000000ffff & vfat.msData);
        dataVFAT[4]  = (0xffff000000000000 & vfat.lsData) >> 48;
        dataVFAT[3]  = (0x0000ffff00000000 & vfat.lsData) >> 32;
        dataVFAT[2]  = (0x00000000ffff0000 & vfat.lsData) >> 16;
        dataVFAT[1]  = (0x000000000000ffff & vfat.lsData);

        uint16_t checkedCRC = checkCRC();
	/*
        if(OKpri){
           cout << " vfat.crc " << std::setfill('0') << std::setw(4) << hex << CRC 
                << "     crc " << std::setfill('0') << std::setw(4) << checkedCRC << dec << "\n" << endl;
	} 
        */
  
       /*
        * GEM Event Analyse
        */

        hi1010->Fill(b1010);
        hi1100->Fill(b1100);
        hiFlag->Fill(Flag);
        hi1110->Fill(b1110);
        hiChip->Fill(ChipID);
        hiBX->Fill(BX);
        //hiCRC->Fill(CRC);
        hiVsCRC->Fill(CRC,checkedCRC);
    
        //I think it would be nice to time this...
        uint8_t chan0xf = 0;
        for (int chan = 0; chan < 128; ++chan) {
          if (chan < 64){
            chan0xf = ((vfat.lsData >> chan) & 0x1);
            histos[chan]->Fill(chan0xf);
          if(!chan0xf) hiCh128->Fill(chan);
    	  } else {
            chan0xf = ((vfat.msData >> (chan-64)) & 0x1);
      	    histos[chan]->Fill(chan0xf);
    	  if(!chan0xf) hiCh128->Fill(chan);
          }
        }
    
        if(OKpri){
          gem::readout::printVFATdataBits(ievent, vfat);
          //gem::readout::printVFATdata(ievent, vfat);
        }

     /*
      }// if 1010,1100,1110
      else {
        ifake++;
      }// if 1010,1100,1110
      */

    }//end ivfat

    hiFake->Fill(ifake);

    // read Event Chamber Header 
    gem::readout::readGEBtrailer(inpf, geb);
    if(OKpri) gem::readout::printGEBtrailer(ievent, geb);

    uint64_t OHcrc      = (0xffff000000000000 & geb.trailer) >> 48; 
    uint64_t OHwCount   = (0x0000ffff00000000 & geb.trailer) >> 32; 
    uint64_t ChamStatus = (0x00000000ffff0000 & geb.trailer) >> 16;

    if (ievent%kUPDATE == 0 && ievent != 0) {
      c1->cd(1)->SetLogy(); hiVFAT->Draw();
      c1->cd(2)->SetLogy(); hiChip->Draw();
      c1->cd(3)->SetLogy(); hiBX->Draw();

      c1->cd(4)->SetLogy(); hi1010->Draw();
      c1->cd(5)->SetLogy(); hi1100->Draw();
      c1->cd(6)->SetLogy(); hi1110->Draw();

      c1->cd(7)->SetLogy(); hiFlag->Draw();
      c1->cd(8); hiCh128->Draw();
      c1->cd(9); hiVsCRC->Draw();
      c1->Update();
      cout << "event " << ievent << " ievent%kUPDATE " << ievent%kUPDATE << endl;
    }
    if(OKpri) cout<<"ievent "<< ievent << " iSumVFAT  " << iSumVFAT << " ifake " << ifake << endl;

   /*
    * GEM End-Event Analyse
    */
    hiVFAT->Fill(iSumVFAT);
   
  } // End ievent
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
