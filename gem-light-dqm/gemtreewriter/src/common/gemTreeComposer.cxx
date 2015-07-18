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
  make all -j4

  Now you will have executable bin/gtc
  In order to run the application you have to provide input and output filenames:
  ./bin/gtc inputHEXdata.dat outputROOTtree.root

  You can download sample HEX data file:
  wget https://baranov.web.cern.ch/baranov/xdaq/threshold/vfat2_9/ThresholdScan_Fri_Jan_16_14-17-59_2015.dat

  \author Sergey.Baranov@cern.ch, Mykhailo.Dalchenko@cern.ch
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
            uint16_t BC;      /*!<Bunch Crossing number "BC" 16 bits, : 1010:4 (control bits), BC:12 */
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
        void show4bits(uint8_t x) 
        {
            int i;
            const unsigned long unit = 1;
            for(i=(sizeof(uint8_t)*4)-1; i>=0; i--)
              (x & ((unit)<<i))?putchar('1'):putchar('0');
     	    //printf("\n");
        }

        bool printVFATdata(int event, const VFATData& vfat)
        {
            if( event<0) return(false);
            cout << "Received tracking data word:" << endl;
            cout << "BC      :: 0x" << std::setfill('0') << std::setw(4) << hex << vfat.BC     << dec << endl;
  	        cout << "EC      :: 0x" << std::setfill('0') << std::setw(4) << hex << vfat.EC     << dec << endl;
            cout << "ChipID  :: 0x" << std::setfill('0') << std::setw(4) << hex << vfat.ChipID << dec << endl;
            cout << "<127:64>:: 0x" << std::setfill('0') << std::setw(8) << hex << vfat.msData << dec << endl;
            cout << "<63:0>  :: 0x" << std::setfill('0') << std::setw(8) << hex << vfat.lsData << dec << endl;
            cout << "crc     :: 0x" << std::setfill('0') << std::setw(4) << hex << vfat.crc    << dec <<"\n"<< endl;
            return true;
        };

        bool printVFATdataBits(int event, int ivfat, const VFATData& vfat)
        {
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
            cout << " <127:64>:: 0x" << std::setfill('0') << std::setw(8) << hex << vfat.msData << dec << endl;
            cout << " <63:0>  :: 0x" << std::setfill('0') << std::setw(8) << hex << vfat.lsData << dec << endl;
            cout << "     crc    0x" << hex << vfat.crc << dec << endl;
            return true;
        };
  
        //! Print ChipID.
        /*! 
            Print ChipID "hex" number and control bits "1110"
         */
    
        bool PrintChipID(int event, const VFATData& vfat)
        {
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
        { 
            int i; 
            for(i=(sizeof(uint8_t)*8)-1; i>=0; i--)
                (x&(1<<i))?putchar('1'):putchar('0');
            printf("\n");
        };

        bool readGEBheader(ifstream& inpf, GEBData& geb)
        {
	        inpf >> hex >> geb.header;
            return(true);
        };	  

        bool printGEBheader(const GEBData& geb)
        {
	        cout << hex << geb.header << " ChamID " << ((0x000000fff0000000 & geb.header) >> 28) 
             << dec << " sumVFAT " << (0x000000000fffffff & geb.header) << endl;
            return(true);
        };	  

        bool readGEBtrailer(ifstream& inpf, GEBData& geb)
        {
 	        inpf >> hex >> geb.trailer;
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
    
        bool readEvent(ifstream& inpf, int event, VFATData& vfat)
        {
            if(event<0) return(false);
            inpf >> hex >> vfat.BC;
            inpf >> hex >> vfat.EC;
	        inpf >> hex >> vfat.ChipID;
            inpf >> hex >> vfat.lsData;
            inpf >> hex >> vfat.msData;
            inpf >> hex >> vfat.crc;
            return(true);
        };	  
};

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
{  
    cout<<"---> Main()"<<endl;
    if (argc<3) 
    {
        cout << "Please provide input and output filenames" << endl;
        cout << "Usage: <path>/gtc inputFile.dat outputFile.root" << endl;
        return 0;
    }
    string ifile=argv[1];
    const TString ofile = argv[2];
    ifstream inpf(ifile.c_str());
    if(!inpf.is_open()) {
      cout << "\nThe file: " << ifile.c_str() << " is missing.\n" << endl;
      return 0;
    };

    TFile *hfile = new TFile(ofile,"RECREATE","Threshold Scan ROOT file with histograms");
    TTree GEMtree("GEMtree","A Tree with GEM Events");
    Event *ev = new Event(); 
    GEMtree.Branch("GEMEvents", &ev);

    GEMOnline         Online;   
    GEMOnline::VFATData vfat;
    GEMOnline::GEBData   geb;

    const Int_t ieventPrint = 3;
    const Int_t ieventMax   = 9000000;
    const Int_t kUPDATE     = 10;

    for(int ievent=0; ievent<ieventMax; ievent++)
    {
        if(inpf.eof()) break;
        if(!inpf.good()) break;

        cout << "Processing event " << ievent << endl;

        // read Event Chamber Header 
        Online.readGEBheader(inpf, geb);
        if(ievent <= ieventPrint) Online.printGEBheader(geb);

        uint32_t ZSFlag  = (0xffffff0000000000 & geb.header) >> 40; 
        uint16_t ChamID  = (0x000000fff0000000 & geb.header) >> 28; 
        uint32_t sumVFAT = (0x000000000fffffff & geb.header);

        GEBdata *GEBdata_ = new GEBdata(ZSFlag, ChamID, sumVFAT);

        for(int ivfat=0; ivfat<sumVFAT; ivfat++)
        {
            Online.readEvent(inpf, ievent, vfat);

            uint8_t   b1010  = (0xf000 & vfat.BC) >> 12;
            uint16_t  BC     = (0x0fff & vfat.BC);
            uint8_t   b1100  = (0xf000 & vfat.EC) >> 12;
            uint8_t   EC     = (0x0fff & vfat.EC) >> 4;
            uint8_t   Flag   = (0x000f & vfat.EC);
            uint8_t   b1110  = (0xf000 & vfat.ChipID) >> 12;
            uint16_t  ChipID = (0x0fff & vfat.ChipID);
            uint16_t  CRC    = vfat.crc;
            uint64_t lsData = vfat.lsData;
            uint64_t msData = vfat.lsData;

            VFATdata *VFATdata_ = new VFATdata(b1010, BC, b1100, EC, Flag, b1110, ChipID, lsData, msData, CRC);
            GEBdata_->addVFATData(*VFATdata_);
            delete VFATdata_;

            if(ievent <= ieventPrint)
            {
	            Online.printVFATdataBits(ievent, ivfat, vfat);
            }
        }

        // read Event Chamber Header 
        Online.readGEBtrailer(inpf, geb);

        uint16_t OHcrc      = (0xffff000000000000 & geb.trailer) >> 48; 
        uint16_t OHwCount   = (0x0000ffff00000000 & geb.trailer) >> 32; 
        uint16_t ChamStatus = (0x00000000ffff0000 & geb.trailer) >> 16;
        uint16_t GEBres     = (0x000000000000ffff & geb.trailer);

        GEBdata_->setTrailer(OHcrc, OHwCount, ChamStatus, GEBres);

        ev->Build(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
        ev->addGEBdata(*GEBdata_);
        GEMtree.Fill();
        ev->Clear();

        if(ievent <= ieventPrint)
        {
            cout << "GEM Chamber Trailer: OHcrc " << hex << OHcrc 
            << " OHwCount " << OHwCount << " ChamStatus " << ChamStatus << dec 
            << " ievent " << ievent << endl;
        }
    }// End loop on events
    inpf.close();
    hfile->Write();// Save file with tree
    cout<<"=== hfile->Write()"<<endl;
	return 0;
}
