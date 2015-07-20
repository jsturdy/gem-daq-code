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

/**
* ... Threshold Scan ROOT based application, could be used for analisys of XDAQ GEM data ...
*/

/*! \file */
/*! 
  \mainpage Threshold Scan ROOT based application.

  VFAT2 data reading example for Threshold Scan XDAQ GEM application, ROOT based analysis.

  \section Installation

  you can make a data file by XDAQ Threshold Scan appliaction or get example from the CERN web:

  wget https://baranov.web.cern.ch/baranov/xdaq/threshold/vfat2_9/ThresholdScan_Fri_Jan_16_14-17-59_2015.dat <br>
  ln -s ThresholdScan_Fri_Jan_16_14-17-59_2015.dat ThresholdScan.dat

  You need a ROOT code for analysis:

  git clone git@github.com:sergueibaranov/gem-root-application.git <br>

  gem-root-application/scripts/with_root_compile.sh gem-root-application/src/tbutils/thldread.cc

  That is all. You will have a root file with 128 threshold scan histograms for one VFAT2 chip.

  \author Sergey.Baranov@cern.ch
*/

using namespace std;

//! GEM VFAT2 Data class.
/*!
  \brief GEMOnline
  contents VFAT2 GEM data format 
  \author Sergey.Baranov@cern.ch
*/

class GEMOnline {
  public:

      //! VFAT2 Channel data.
      /*!
        contents VFAT2 128 channels data in two 64 bits words.
       */
    
      //! GEM Event Data Format (one chip data)
      /*! 
        Uncoding of VFAT2 data for one chip, data format.
        \image html vfat2.data.format.png
        \author Sergey.Baranov@cern.ch
       */
    
      struct VFATData {
        uint16_t BC;      /*!<Banch Crossing number "BC" 16 bits, : 1010:4 (control bits), BC:12 */
        uint16_t EC;      /*!<Event Counter "EC" 16 bits: 1100:4(control bits) , EC:8, Flag:4 */
        uint32_t bxExp;   
        uint16_t bxNum;   /*!<Event Number & SBit, 16 bits : bxNum:6, SBit:6 */
        uint16_t ChipID;  /*!<ChipID 16 bits, 1110:4 (control bits), ChipID:12 */
        uint64_t lsData;  /*!<lsData value, bits from 1to64. */ 
        uint64_t msData;  /*!<msData value, bits from 65to128. */
        uint16_t crc;     /*!<Checksum number, CRC:16 */
      };    
    
      struct GEBData {
        uint64_t header;      // ZSFlag:24 ChamID:12 
        std::vector<VFATData> vfats;
        uint64_t trailer;     // OHcrc: 16 OHwCount:16  ChamStatus:16
      };

      struct GEMData {
        uint64_t header1;      // AmcNo:4      0000:4     LV1ID:24   BXID:12     DataLgth:20 
        uint64_t header2;      // User:32      OrN:16     BoardID:16
        uint64_t header3;      // DAVList:24   BufStat:24 DAVCount:5 FormatVer:3 MP7BordStat:8 
        std::vector<GEBData> gebs;
        uint64_t trailer2;     // EventStat:32 GEBerrFlag:24  
        uint64_t trailer1;     // crc:32       LV1IDT:8   0000:4     DataLgth:20 
      };

      //! Print Event, "hex" format.
      /*! 
        Print VFAT2 event.
       */
    
      //
      // Useful printouts 
      //
      void show4bits(uint8_t x) {
        int i;
        const unsigned long unit = 1;
        for(i=(sizeof(uint8_t)*4)-1; i>=0; i--)
          (x & ((unit)<<i))?putchar('1'):putchar('0');
     	//printf("\n");
      }

      bool printVFATdata(int event, const VFATData& vfat){
        if( event<0) return(false);
          cout << "Received tracking data word:" << endl;
          cout << "BC      :: 0x" << std::setfill('0') << std::setw(4) << hex << vfat.BC     << dec << endl;
  	  cout << "EC      :: 0x" << std::setfill('0') << std::setw(4) << hex << vfat.EC     << dec << endl;
	  /*
          cout << "BxExp   :: 0x" << std::setfill('0') << std::setw(4) << hex << vfat.bxExp  << dec << endl; // do we need here?
          cout << "BxNum   :: 0x" << std::setfill('0') << std::setw(4) << hex << vfat.bxNum  << dec << endl; // do we need here?
          */
  	  cout << "ChipID  :: 0x" << std::setfill('0') << std::setw(4) << hex << vfat.ChipID << dec << endl;
          cout << "<127:64>:: 0x" << std::setfill('0') << std::setw(8) << hex << vfat.msData << dec << endl;
          cout << "<63:0>  :: 0x" << std::setfill('0') << std::setw(8) << hex << vfat.lsData << dec << endl;
          cout << "crc     :: 0x" << std::setfill('0') << std::setw(4) << hex << vfat.crc    << dec <<"\n"<< endl;
        return(true);
      };

      bool printVFATdataBits(int event, int ivfat, const VFATData& vfat){
          if( event<0) return(false);
	  cout << "\nReceived VFAT data word: event " << event << " ivfat  " << ivfat << endl;
  
          uint8_t   b1010 = (0xf000 & vfat.BC) >> 12;
          show4bits(b1010); cout << " BC     0x" << hex << (0x0fff & vfat.BC) << dec << endl;
  
          uint8_t   b1100 = (0xf000 & vfat.EC) >> 12;
          uint16_t   EC   = (0x0ff0 & vfat.EC) >> 4;
          uint8_t   Flag  = (0x000f & vfat.EC);
          show4bits(b1100); cout << " EC     0x" << hex << EC << dec << endl; 
          show4bits(Flag);  cout << " Flag  " << endl;
  
          uint8_t   b1110 = (0xf000 & vfat.ChipID) >> 12;
          uint16_t ChipID = (0x0fff & vfat.ChipID);
          show4bits(b1110); cout << " ChipID 0x" << hex << ChipID << dec << " " << endl;
	  /*
          cout << "     bxExp  0x" << std::setfill('0') << std::setw(4) << hex << vfat.bxExp << dec << " " << endl;
  	  cout << "     bxNum  0x" << std::setfill('0') << std::setw(2) << hex << ((0xff00 & vfat.bxNum) >> 8) << dec << endl;
  	  cout << "     SBit   0x" << std::setfill('0') << std::setw(2) << hex <<  (0x00ff & vfat.bxNum)       << dec << endl;
	  */
          cout << " <127:64>:: 0x" << std::setfill('0') << std::setw(8) << hex << vfat.msData << dec << endl;
          cout << " <63:0>  :: 0x" << std::setfill('0') << std::setw(8) << hex << vfat.lsData << dec << endl;
          cout << "     crc    0x" << hex << vfat.crc << dec << endl;
  
          //cout << " " << endl; show16bits(vfat.EC);
  
          return(true);
      };
  
      //! Print ChipID.
      /*! 
          Print ChipID "hex" number and control bits "1110"
       */
    
      bool PrintChipID(int event, const VFATData& vfat){
        if( event<0 ) return(false);
          cout << "\nevent " << event << endl;
          uint8_t bitsE = ((vfat.ChipID&0xF000)>>12);
          showbits(bitsE);
          cout << hex << "1110 0x0" << ((vfat.ChipID&0xF000)>>12) << " ChipID 0x" << (vfat.ChipID&0x0FFF) << dec << endl;
      };
    
      //! showbits function.
      /*!
       show bits function, needs for debugging
       */
    
      void showbits(uint8_t x)
        { int i; 
          for(i=(sizeof(uint8_t)*8)-1; i>=0; i--)
            (x&(1<<i))?putchar('1'):putchar('0');
          printf("\n");
        };

      bool readGEBheader(ifstream& inpf, GEBData& geb){
	inpf >> hex >> geb.header;
        return(true);
      };	  

      bool printGEBheader(const GEBData& geb){
	cout << hex << geb.header << " ChamID " << ((0x000000fff0000000 & geb.header) >> 28) 
             << dec << " sumVFAT " << (0x000000000fffffff & geb.header) << endl;
        return(true);
      };	  

      bool readGEBtrailer(ifstream& inpf, GEBData& geb){
 	inpf >> hex >> geb.trailer;
        return(true);
      };	  

      bool printGEBtrailer(const GEBData& geb){
        uint64_t OHcrc      = (0xffff000000000000 & geb.trailer) >> 48; 
        uint64_t OHwCount   = (0x0000ffff00000000 & geb.trailer) >> 32; 
        uint64_t ChamStatus = (0x00000000ffff0000 & geb.trailer) >> 16;
        cout << "GEM Camber Treiler: OHcrc " << hex << OHcrc << " OHwCount " << OHwCount << " ChamStatus " << ChamStatus << dec 
             << endl;
        return(true);
      };	  

      //! Read 1-128 channels data
      /*!
        reading two 64 bits words (lsData & msData) with data from all channels for one VFAT2 chip 
       */
    
      //! Read GEM event data
      /*!
        reading GEM VFAT2 data (BC,EC,bxNum,ChipID,(lsData & msData), crc.
       */
    
      bool readEvent(ifstream& inpf, int event, VFATData& vfat){
        if(event<0) return(false);
          inpf >> hex >> vfat.BC;
          inpf >> hex >> vfat.EC;
          /* inpf >> hex >> vfat.bxExp;
          inpf >> hex >> vfat.bxNum;
          */
	  inpf >> hex >> vfat.ChipID;
          inpf >> hex >> vfat.lsData;
          inpf >> hex >> vfat.msData;
          inpf >> hex >> vfat.crc;
        return(true);
      };	  

};// end of GEMOnline

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
    uint16_t checkCRC(bool OKprint){
      uint16_t crc_fin = 0xffff;
      for (int i = 11; i >= 1; i--){
        crc_fin = crc_calc(crc_fin, dataVFAT[i]);
	if(OKprint){
          cout << " dataVFAT[" << std::setfill('0') << std::setw(2) << i << "] " << hex << std::setfill('0') << std::setw(4) << dataVFAT[i]
               << " crc_temp " << std::setfill('0') << std::setw(4) << crc_fin << dec << endl;
        }
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
 
  GEMOnline         Online;   
  GEMOnline::VFATData vfat;
  GEMOnline::GEBData   geb;

  string file="DataParker.dat";

  ifstream inpf(file.c_str());
  if(!inpf.is_open()) {
    cout << "\nThe file: " << file.c_str() << " is missing.\n" << endl;
    return 0;
  };

  /* Threshould Analysis Histograms */
  const TString filename = "DQMlight.root";

  // Create a new canvas.
  TCanvas *c1 = new TCanvas("c1","Dynamic Filling Example",0,0,900,900);
  c1->SetFillColor(42);
  c1->GetFrame()->SetFillColor(21);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);
  c1->Divide(3,3);

  TFile* hfile = NULL;
  hfile = new TFile(filename,"RECREATE","Threshold Scan ROOT file with histograms");

  TH1F* hiVFAT = new TH1F("VFAT", "Number VFAT per event", 100,  0., 1000. );
  hiVFAT->SetFillColor(48);

  TH1C* hi1010 = new TH1C("1010", "Control Bits 1010", 100, 0x0, 0xf );
  hi1010->SetFillColor(48);

  TH1C* hi1100 = new TH1C("1100", "Control Bits 1100", 100, 0x0, 0xf );
  hi1100->SetFillColor(48);

  TH1C* hi1110 = new TH1C("1110", "Control Bits 1110", 100, 0x0, 0xf );
  hi1110->SetFillColor(48);

  TH1C* hiChip = new TH1C("ChipID", "ChipID",          100, 0x0, 0xfff );
  hiChip->SetFillColor(48);

  TH1C* hiFlag = new TH1C("Flag"  , "Flag",            100, 0x0, 0xf );
  hiFlag->SetFillColor(48);

  TH1C* hiCRC = new TH1C("CRC",     "CRC",             100, 0x0, 0xffff );
  hiCRC->SetFillColor(48);

  TH1C* hiDiffCRC = new TH1C("DiffCRC", "CRC Diff",    100, 0xffff, 0xffff );
  hiDiffCRC->SetFillColor(48);

  // Booking of all 128 histograms for each VFAT2 channel
  TH1F* hiCh128 = new TH1F("Ch128", "all channels",    128, 0.,   128. );
  hiCh128->SetFillColor(48);

  stringstream histName, histTitle;
  TH1F* histos[128];

  for (unsigned int hi = 0; hi < 128; ++hi) {
    histName.clear(); histName.str(std::string());
    histTitle.clear(); histTitle.str(std::string());
    histName  << "channel"<<(hi+1);
    histTitle << "Threshold scan for channel "<<(hi+1);
    histos[hi] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), 100, 0., 0xf );
  }

  const Int_t ieventPrint = 2;
  const Int_t ieventMax   = 90000;
  const Int_t kUPDATE     = 25;
  bool OKpri = false;

  for(int ievent=0; ievent<ieventMax; ievent++){
    OKpri = OKprint(ievent,ieventPrint);
    if(inpf.eof()) break;
    if(!inpf.good()) break;

    if(OKpri) cout << "\nievent " << ievent << endl;

    // read Event Chamber Header 
    Online.readGEBheader(inpf, geb);
    if(OKpri) Online.printGEBheader(geb);

    uint64_t ZSFlag  = (0xffffff0000000000 & geb.header) >> 40; 
    uint64_t ChamID  = (0x000000fff0000000 & geb.header) >> 28; 
    uint64_t sumVFAT = (0x000000000fffffff & geb.header);

    for(int ivfat=0; ivfat<sumVFAT; ivfat++){

     /*
      *  GEM Event Reading
      */
      Online.readEvent(inpf, ievent, vfat);
  
      uint8_t   b1010  = (0xf000 & vfat.BC) >> 12;
      uint8_t   b1100  = (0xf000 & vfat.EC) >> 12;
      uint8_t   Flag   = (0x000f & vfat.EC);
      uint8_t   b1110  = (0xf000 & vfat.ChipID) >> 12;
      uint16_t  ChipID = (0x0fff & vfat.ChipID);
      uint16_t  CRC    = vfat.crc;
  
      if ( (b1010 == 0xa) && (b1100==0xc) && (b1110==0xe) /* && (ChipID==0x68) */ ){

        if(OKpri){
          cout << "\nvfat.lsData " << std::setfill('0') << std::setw(16) << hex << vfat.lsData  
               << " vfat.msData " << std::setfill('0') << std::setw(16) << vfat.msData 
               << " ChipID 0x" << std::setfill('0') << std::setw(3) << ChipID << dec << "\n" << endl;
        }
  
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

        uint16_t checkedCRC = checkCRC(OKpri);
        if(OKpri){
           cout << " vfat.crc " << std::setfill('0') << std::setw(4) << hex << CRC 
                << "     crc " << std::setfill('0') << std::setw(4) << checkedCRC << dec << "\n" << endl;
        }
  
       /*
        * GEM Event Analyse
        */

        hiVFAT->Fill(ivfat);
        hi1010->Fill(b1010);
        hi1100->Fill(b1100);
        hiFlag->Fill(Flag);
        hi1110->Fill(b1110);
        hiChip->Fill(ChipID);
        hiCRC->Fill(CRC);
        hiDiffCRC->Fill(CRC-checkedCRC);
    
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
          Online.printVFATdataBits(ievent, ivfat, vfat);
          //Online.printVFATdata(ievent, vfat);
          //Online.PrintChipID(ievent,vfat);
        }
    
      }// if 1010,1100,1110, ChipID
    }//end ivfat

    // read Event Chamber Header 
    Online.readGEBtrailer(inpf, geb);
    if(OKpri) Online.printGEBtrailer(geb);

    uint64_t OHcrc      = (0xffff000000000000 & geb.trailer) >> 48; 
    uint64_t OHwCount   = (0x0000ffff00000000 & geb.trailer) >> 32; 
    uint64_t ChamStatus = (0x00000000ffff0000 & geb.trailer) >> 16;

    if (ievent%kUPDATE == 0 && ievent != 0) {
      c1->cd(1)->SetLogy(); hiVFAT->Draw();
      c1->cd(2); hi1010->Draw();
      c1->cd(3); hi1100->Draw();
      c1->cd(4)->SetLogy(); hiFlag->Draw();
      c1->cd(5)->SetLogy(); hi1110->Draw();
      c1->cd(6)->SetLogy(); hiChip->Draw();
      c1->cd(7)->SetLogy(); hiCRC->Draw();
      c1->cd(8)->SetLogy(); hiCh128->Draw();
      c1->cd(9)->SetLogy(); hiDiffCRC->Draw();
      c1->Update();

      cout << "event " << ievent << " ievent%kUPDATE " << ievent%kUPDATE << endl;
    }

    if(OKpri) cout<<"ievent "<< ievent <<endl;
  }
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
