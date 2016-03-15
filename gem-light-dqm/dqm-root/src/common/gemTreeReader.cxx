#define DEBUG 1
#define NVFAT 24
#define NETA 8
#include <iomanip> 
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <array>
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
#include <TError.h>
#include <TBufferJSON.h>
#include <memory>

#include "gem/datachecker/GEMDataChecker.h"
#include "gem/readout/GEMslotContents.h"
#include "GEMClusterization/GEMStrip.h"
#include "GEMClusterization/GEMStripCollection.h"
#include "GEMClusterization/GEMClusterContainer.h"
#include "GEMClusterization/GEMClusterizer.h"
#include "plotter.cxx"
#include "logger.cxx"
#include "integrity_checker.cxx"
#include "GEMDQMerrors.cxx"
#include "AMC13_histogram.cxx"

using namespace std;

class treeReader
{
public:
  treeReader(const std::string &ifilename)
  {
    std::string tmp = ifilename.substr(ifilename.size()-9, ifilename.size());
    if (tmp != ".raw.root") throw std::runtime_error("Wrong input filename (should end with '.raw.root'): "+ifilename);
    ifile = new TFile(ifilename.c_str(), "READ");
    ofilename = ifilename.substr(0,ifilename.size()-9);
    ofilename += ".analyzed.root";
    ofile = new TFile(ofilename.c_str(), "RECREATE");
    if (DEBUG) std::cout << std::dec << "[gemTreeReader]: File for histograms created" << std::endl;   

    if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Fetching hardware" << std::endl;   
    this->fetchHardware();

    if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Booking histograms" << std::endl;   
    this->bookAllHistograms();
    this->fillAllHistograms();
  }
  ~treeReader(){}



private:
  TFile *ifile;
  TFile *ofile;
  std::string ofilename;

  std::vector<TDirectory*> AMC13dir;
  std::vector<TDirectory*> AMCdir;
  std::vector<TDirectory*> GEBdir;
  std::vector<TDirectory*> VFATdir;

  //VFAT data
  TH1I* hi1010                     ; // Control bit 1010
  TH1I* hi1100                     ; // Control bit 1100
  TH1I* hi1110                     ; // Control bit 1110
  TH1I* hiBC                       ; // Bunch Crossing Number
  TH1I* hiEC                       ; // Event Counter
  TH1I* hiFlag                     ; // VFAT Flag
  TH1I* hiChipID                   ; // VFAT Chip ID
  //GEB data
  TH1I* hiZeroSup                  ; // Zero Suppression Flag
  TH1I* hiInputID                  ; // GLIV input ID
  TH1I* hiVwh                      ; // VFAT word count (expected)
  TH1I* hiErrorC                   ; // Thirteen Flags (may need to separate into thriteen histograms)
  TH1I* hiOHCRC                    ; // OH CRC
  TH1I* hiVwt                      ; // VFAT word count (counted)
  TH1I* hiInFU                     ; // InFIFO underflow flag
  TH1I* hiStuckd                   ; // Stuck data Flag


  TDirectory *dir[3];


  vector<AMC13Event> v_amc13;
  vector<AMCdata> v_amc;
  vector<GEBdata> v_geb;
  vector<VFATdata> v_vfat;


  vector<AMC13_histogram> v_amc13H;
  vector<AMC_histogram> v_amcH;
  vector<GEB_histogram> v_gebH;
  vector<VFAT_histogram> v_vfatH;

  AMC13_histogram * m_amc13H;
  AMC_histogram * m_amcH;
  GEB_histogram * m_gebH;
  VFAT_histogram * m_vfatH;

  void fetchHardware()
  {
    TTree *tree = (TTree*)ifile->Get("GEMtree");
    Event *event = new Event();
    TBranch *branch = tree->GetBranch("GEMEvents");
    branch->SetAddress(&event);
    Int_t nentries = tree->GetEntries();
    branch->GetEntry(0);
    v_amc13 = event->amc13s();
    for(auto a13 = v_amc13.begin(); a13!= v_amc13.end(); a13++){
      v_amc = a13->amcs();
      for(auto a=v_amc.begin(); a!=v_amc.end(); a++){
        v_geb = a->gebs();
        for(auto g=v_geb.begin(); g!=v_geb.end();g++){
          v_vfat=g->vfats();
        }
      }
    }
    if (DEBUG) std::cout<< "[gemTreeReader]: " << "Number of TTree entries: " << nentries << "\n";
    if (DEBUG) std::cout<< "[gemTreeReader]: " << "Number of AMC13s: " << v_amc13.size()<< "\n";
    if (DEBUG) std::cout<< "[gemTreeReader]: " << "Number of AMCs: " << v_amc.size()<< "\n";
    if (DEBUG) std::cout<< "[gemTreeReader]: " << "Number of GEBs: " << v_geb.size()<< "\n";
    if (DEBUG) std::cout<< "[gemTreeReader]: " << "Number of VFATs: " << v_vfat.size()<< "\n";
  }


  void bookAllHistograms()
  {
    int a13_c=0;    //counter through AMC13s
    int a_c=0;      //counter through AMCs
    int g_c=0;      //counter through GEBs
    int v_c=0;      //counter through VFATs

    /* LOOP THROUGH AMC13s */
    for(auto a13 = v_amc13.begin(); a13!=v_amc13.end(); a13++){
      v_amc = a13->amcs();

      char diramc13[30];        //filename for AMC13 directory
      diramc13[0]='\0';         
      char serial_ch[20];       //char used to put serial number into directory name
      serial_ch[0] = '\0';
      int serial = v_amc13[a13_c].nAMC();  //obtains the serial number from the AMC13 Event
      sprintf(serial_ch, "%d", serial);
      strcat(diramc13,"AMC13-");
      strcat(diramc13,serial_ch);
      if (DEBUG) std::cout << std::dec << "[gemTreeReader]: AMC13 Directory " << diramc13 << " created" << std::endl;
      //AMC13 HISTOGRAMS HERE
      m_amc13H = new AMC13_histogram(ofilename, gDirectory->mkdir(diramc13));
      m_amc13H->bookHistograms();

      a_c=0;

      /* LOOP THROUGH AMCs */
      for(auto a=v_amc.begin(); a!=v_amc.end(); a++){
        v_geb = a->gebs();
        char diramc[30];        //filename for AMC directory  
        diramc[0]='\0';
        char aslot_ch[2];       //char used to put AMC slot number inot directory name
        aslot_ch[0] = '\0';
        int aslot = v_amc[a_c].AMCnum();  //obtains the slot number from the AMCdata
        sprintf(aslot_ch, "%d", aslot);
        strcat(diramc,"AMC-");
        strcat(diramc, aslot_ch);
        if (DEBUG) std::cout << std::dec << "[gemTreeReader]: AMC Directory " << diramc << " created" << std::endl;
        m_amcH = new AMC_histogram(ofilename, gDirectory->mkdir(diramc));
        m_amcH->bookHistograms();
        if (DEBUG) std::cout << std::dec << "[gemTreeReader]: AMC13 AMCs size " << m_amc13H->amcsH().size() << std::endl;

        g_c=0;

	      /* LOOP THROUGH GEBs */
        for(auto g=v_geb.begin(); g!=v_geb.end();g++){
          v_vfat=g->vfats();
          char dirgeb[30];    //filename for GEB directory
          dirgeb[0]='\0';    
          char g_ch[2];       //char used to put GEB number into directory name
          g_ch[0]='\0';
          sprintf(g_ch, "%d", g_c);
          strcat(dirgeb,"GTX-");
          strcat(dirgeb,g_ch);
          if (DEBUG) std::cout << std::dec << "[gemTreeReader]: GEB Directory " << dirgeb << " created" << std::endl;
          //GEB HISTOGRAMS HERE
          m_gebH = new GEB_histogram(ofilename, gDirectory->mkdir(dirgeb));
          m_gebH->bookHistograms();
          if (DEBUG) std::cout << std::dec << "[gemTreeReader]: AMC GEBs size " << m_amcH->gebsH().size() << std::endl;

          v_c=0;

	        /* LOOP THROUGH VFATs */
          for(auto v=v_vfat.begin(); v!=v_vfat.end();v++){
            char dirvfat[30];   //filename for VFAT directory
            dirvfat[0]='\0';    
            char vslot_ch[2];   //char used to put VFAT number into directory name
            vslot_ch[0] = '\0';
            std::unique_ptr<gem::readout::GEMslotContents> slotInfo_ = std::unique_ptr<gem::readout::GEMslotContents> (new gem::readout::GEMslotContents("slot_table.csv"));     
            int vslot = slotInfo_->GEBslotIndex(v->ChipID());  //converts Chip ID into VFAT slot number
            sprintf(vslot_ch, "%d", vslot);
            strcat(dirvfat,"VFAT-");
            strcat(dirvfat, vslot_ch);
            if (DEBUG) std::cout << std::dec << "[gemTreeReader]: VFAT Directory " << dirvfat << " created" << std::endl;
            //VFAT HISTOGRAMS HERE
            m_vfatH = new VFAT_histogram(ofilename, gDirectory->mkdir(dirvfat));
            m_vfatH->bookHistograms();
            //m_vfatH->fillHistograms(&*v);
            m_gebH->addVFATH(*m_vfatH);
            if (DEBUG) std::cout << std::dec << "[gemTreeReader]: GEB VFATs size " << m_gebH->vfatsH().size() << std::endl;

            gDirectory->cd("..");   //moves back to previous directory
            v_c++;
          } /* END VFAT LOOP */
          gDirectory->cd("..");     //moves back to previous directory
          g_c++;
          m_amcH->addGEBH(*m_gebH);
        } /* END GEB LOOP */
        gDirectory->cd("..");       //moves back to previous directory
       	a_c++;
        m_amc13H->addAMCH(*m_amcH);
      } /* END AMC LOOP */
      a13_c++;
    } /* END AMC13 LOOP */
  }
  void fillAllHistograms()
  {
    int a13_c=0;    //counter through AMC13s
    int a_c=0;      //counter through AMCs
    int g_c=0;      //counter through GEBs
    int v_c=0;      //counter through VFATs

    TTree *tree = (TTree*)ifile->Get("GEMtree");
    Event *event = new Event();
    TBranch *branch = tree->GetBranch("GEMEvents");
    branch->SetAddress(&event);
    Int_t nentries = tree->GetEntries();
    /* LOOP THROUGH Events */
    for (int i = 0; i < nentries; i++){
      branch->GetEntry(i);
      v_amc13 = event->amc13s();
      /* LOOP THROUGH AMC13s */
      for(auto a13 = v_amc13.begin(); a13!=v_amc13.end(); a13++){
        v_amc = a13->amcs();
        v_amcH = m_amc13H->amcsH();
        //m_amc13H->fillHistograms();
        a_c=0;
        /* LOOP THROUGH AMCs */
        for(auto a=v_amc.begin(); a!=v_amc.end(); a++){
          v_geb = a->gebs();
          v_gebH = v_amcH[a_c].gebsH();
          //AMC_histogram * t_amcH = &(m_amc13H->amcsH().at(a_c));
          //m_amcH->fillHistograms();
          g_c=0;
	        /* LOOP THROUGH GEBs */
          for(auto g=v_geb.begin(); g!=v_geb.end();g++){
            v_vfat = g->vfats();
            v_vfatH = v_gebH[g_c].vfatsH();
            //GEB_histogram * t_gebH = &(m_amcH->gebsH().at(g_c));
            //m_gebH->fillHistograms();
            v_c=0;
	          /* LOOP THROUGH VFATs */
            for(auto v=v_vfat.begin(); v!=v_vfat.end();v++){
              //m_gebH->vfatsH()[v_c].fillHistograms(&*v);
              //VFAT_histogram * t_vfatH = &(t_gebH->vfatsH().at(v_c));
              v_vfatH[v_c].fillHistograms(&*v);
              v_c++;
            } /* END VFAT LOOP */
            g_c++;
          } /* END GEB LOOP */
         	a_c++;
        } /* END AMC LOOP */
        a13_c++;
      } /* END AMC13 LOOP */
    } /* END EVENTS LOOP */
    ofile->Write();
  }
};


// control_bits = new TH1F("Control_Bits", "Control Bits ", 15,  0x0 , 0xf)
// Evt_ty       = new TH1F("Evt_ty", "Evt_ty", 15, 0x0, 0xf)
// LV1_id;      = new TH1F("LV1_id", "LV1_id", 0xffffff, 0x0, 0xffffff)
// Bx_id;       = new TH1F("Bx_id", "Bx_id", 4095, 0x0, 0xfff)
// Source_id;   = new TH1F("Source_id", "Source_id", 4095, 0x0, 0xfff)
// CalTyp;      = new TH1F("CalTyp", "CalTyp", 15, 0x0, 0xf)
// nAMC;        = new TH1F("nAMC", "nAMC", 15, 0x0, 0xf)
// OrN;         = new TH1F("OrN", "OrN", 0xffffffff, 0x0, 0xffffffff)
// CRC_amc13;   = new TH1F("CRC_amc13", "CRC_amc13", 0xffffffff, 0x0, 0xffffffff)
// Blk_Not;     = new TH1F("Blk_Not", "Blk_Not", 255, 0x0, 0xff)
// LV1_idT;     = new TH1F("LV1_idT", "LV1_idT", 255, 0x0, 0xff)
// BX_idT;      = new TH1F("BX_idT", "BX_idT", 4095, 0x0, 0xfff)
// EvtLength;   = new TH1F("EvtLength", "EvtLength", 0xffffff, 0x0, 0xffffff)
// CRC_cdf;     = new TH1F("CRC_cdf", "CRC_cdf", 0xffff, 0x0, 0xffff)
    





// class AMC13_histogram: public Hardware_histogram
// {
//   public:
//   AMC13_histogram(const std::string &ifilename, const TDirectory *d)
//     {
//       dir = *d;
//       std::string tmp = ifilename.substr(ifilename.size()-9, ifilename.size());
//       if (tmp != ".raw.root") throw std::runtime_error("Wrong input filename (should end with '.raw.root'): "+ifilename);
//       ifile = new TFile(ifilename.c_str(), "READ");
//       ofilename = ifilename.substr(0,ifilename.size()-9);
//       ofilename += ".analyzed.root";
//       ofile = new TFile(ofilename.c_str(), "RECREATE");
//       this->bookHistograms();
//     }
  

//   private:
//     TFile *ifile;
//     TFile *ofile;
//     std::string ofilename;
//     TDirectory dir;

//     TH1F* control_bits;
//     TH1F* Evt_ty;
//     TH1F* LV1_id;
//     TH1F* Bx_id;
//     TH1F* Source_id;
//     TH1F* CalTyp;
//     TH1F* nAMC;
//     TH1F* OrN;
//     TH1F* CRC_amc13;
//     TH1F* Blk_Not;
//     TH1F* LV1_idT;
//     TH1F* BX_idT;
//     TH1F* EvtLength;
//     TH1F* CRC_cdf;

//     void bookHistograms()
//     {
      
//       //dir[i] = ofile->mkdir(dirname[i].c_str());
//       control_bits = new TH1F("Control_Bits", "Control Bits ", 15,  0x0 , 0xf)
//       Evt_ty       = new TH1F("Evt_ty", "Evt_ty", 15, 0x0, 0xf)
//       LV1_id;      = new TH1F("LV1_id", "LV1_id", 0xffffff, 0x0, 0xffffff)
//       Bx_id;       = new TH1F("Bx_id", "Bx_id", 4095, 0x0, 0xfff)
//       Source_id;   = new TH1F("Source_id", "Source_id", 4095, 0x0, 0xfff)
//       CalTyp;      = new TH1F("CalTyp", "CalTyp", 15, 0x0, 0xf)
//       nAMC;        = new TH1F("nAMC", "nAMC", 15, 0x0, 0xf)
//       OrN;         = new TH1F("OrN", "OrN", 0xffffffff, 0x0, 0xffffffff)
//       CRC_amc13;   = new TH1F("CRC_amc13", "CRC_amc13", 0xffffffff, 0x0, 0xffffffff)
//       Blk_Not;     = new TH1F("Blk_Not", "Blk_Not", 255, 0x0, 0xff)
//       LV1_idT;     = new TH1F("LV1_idT", "LV1_idT", 255, 0x0, 0xff)
//       BX_idT;      = new TH1F("BX_idT", "BX_idT", 4095, 0x0, 0xfff)
//       EvtLength;   = new TH1F("EvtLength", "EvtLength", 0xffffff, 0x0, 0xffffff)
//       CRC_cdf;     = new TH1F("CRC_cdf", "CRC_cdf", 0xffff, 0x0, 0xffff)
//     }

// };

// class AMC_histogram: public Hardware_histogram
// {
//   public:

//     AMC_histogram(const std::string &ifilename, const TDirectory *d)
//     {
//       dir = *d;
//       std::string tmp = ifilename.substr(ifilename.size()-9, ifilename.size());
//       if (tmp != ".raw.root") throw std::runtime_error("Wrong input filename (should end with '.raw.root'): "+ifilename);
//       ifile = new TFile(ifilename.c_str(), "READ");
//       ofilename = ifilename.substr(0,ifilename.size()-9);
//       ofilename += ".analyzed.root";
//       ofile = new TFile(ofilename.c_str(), "RECREATE");
//       this->bookHistograms();
//     }

//   private:
//     TFile *ifile;
//     TFile *ofile;
//     std::string ofilename;
//     TDirectory dir;

//     TH1F* AMCnum;
//     TH1F* L1A;
//     TH1F* BX;
//     TH1F* Dlength;
//     TH1F* FV;
//     TH1F* Rtype;
//     TH1F* Param1;
//     TH1F* Param2;
//     TH1F* Param3;
//     TH1F* Onum;
//     TH1F* BID;
//     TH1F* GEMDAV;
//     TH1F* Bstatus;
//     TH1F* GDcount;
//     TH1F* Tsate;
//     TH1F* ChamT;
//     TH1F* OOSG;
//     TH1F* CRC;
//     TH1F* L1AT;
//     TH1F* DlengthT;

//     void bookHistograms()
//     {
//       AMCnum     = new TH1F("AMCnum", "AMC number", 15,  0x0 , 0xf)
//       L1A        = new TH1F("L1A", "L1A ID", 0xffffff,  0x0 , 0xffffff)      
//       BX         = new TH1F("BX", "BX ID", 4095,  0x0 , 0xfff)
//       Dlength    = new TH1F("Dlength", "Data Length", 0xfffff,  0x0 , 0xfffff)
//       FV         = new TH1F("FV", "Format Version", 15,  0x0 , 0xf)
//       Rtype      = new TH1F("Rtype", "Run Type", 15,  0x0 , 0xf)
//       Param1     = new TH1F("Param1", "Run Param 1", 255,  0x0 , 0xff)
//       Param2     = new TH1F("Param2", "Run Param 2", 255,  0x0 , 0xff)
//       Param3     = new TH1F("Param3", "Run Param 3", 255,  0x0 , 0xff)
//       Onum       = new TH1F("Onum", "Orbit Number", 0xffff,  0x0 , 0xffff)
//       BID        = new TH1F("BID", "Board ID", 0xffff,  0x0 , 0xffff)
//       GEMDAV     = new TH1F("GEMDAV", "GEM DAV list", ZeroSup  = new TH1F("ZeroSup", "Zero Suppression", 0xffffff,  0x0 , 0xffffff)
//       InputID  = new TH1F("InputID", "GLIB input ID", 31,  0x0 , 0b11111)      
//       Vwh      = new TH1F("Vwh", "VFAT word count", 4095,  0x0 , 0xfff)
//       ErrorC   = new TH1F("ErrorC", "Thirteen Flags", 0b1111111111111111,  0x0 , 0b1111111111111111)
//       OHCRC    = new TH1F("OHCRC", "OH CRC", 0xffff,  0x0 , 0xffff)
//       Vwt      = new TH1F("Vwt", "VFAT word count", 4095,  0x0 , 0xfff)
//       InFU     = new TH1F("InFu", "InFIFO underflow flag", 15,  0x0 , 0xf)
//       Stuckd   = new TH1F("Stuckd", "Stuck data flag", 1,  0x0 , 0b1)
//       Bstatus    = new TH1F("Bstatus", "Buffer Status", 0xffffff,  0x0 , 0xffffff)
//       GDcount    = new TH1F("GDcount", "GEM DAV count", 31,  0x0 , 0b11111)
//       Tsate      = new TH1F("Tstate", "TTS state", 7,  0x0 , 0b111)
//       ChamT      = new TH1F("ChamT", "Chamber Timeout", 0xffffff,  0x0 , 0xffffff)
//       OOSG       = new TH1F("OOSG", "OOS GLIB", 1,  0x0 , 0b1)
//       CRC        = new TH1F("CRC", "CRC", 0xffffffff,  0x0 , 0xffffffff)
//       L1AT       = new TH1F("L1AT", "L1AT", 0xffffff,  0x0 , 0xffffff)
//       DlengthT   = new TH1F("DlengthT", "DlengthT", 0xffffff,  0x0 , 0xffffff)
//     }

// }

// class GEB_histogram: public Hardware_histogram
// {
//   public:
//     GEB_histogram(const std::string &ifilename, const TDirectory *d)
//     {
//       dir = *d;
//       std::string tmp = ifilename.substr(ifilename.size()-9, ifilename.size());
//       if (tmp != ".raw.root") throw std::runtime_error("Wrong input filename (should end with '.raw.root'): "+ifilename);
//       ifile = new TFile(ifilename.c_str(), "READ");
//       ofilename = ifilename.substr(0,ifilename.size()-9);
//       ofilename += ".analyzed.root";
//       ofile = new TFile(ofilename.c_str(), "RECREATE");
//       this->bookHistograms();
//     }

//   private:

//     TFile *ifile;
//     TFile *ofile;
//     std::string ofilename;
//     TDirectory dir;

//     TH1F* ZeroSup;
//     TH1F* InputID;
//     TH1F* Vwh;
//     TH1F* ErrorC;
//     TH1F* OHCRC;
//     TH1F* Vwt;
//     TH1F* InFu;
//     TH1F* Stuckd;
   
//     void bookHistograms()
//     {
//       ZeroSup  = new TH1F("ZeroSup", "Zero Suppression", 0xffffff,  0x0 , 0xffffff)
//       InputID  = new TH1F("InputID", "GLIB input ID", 31,  0x0 , 0b11111)      
//       Vwh      = new TH1F("Vwh", "VFAT word count", 4095,  0x0 , 0xfff)
//       ErrorC   = new TH1F("ErrorC", "Thirteen Flags", 0b1111111111111111,  0x0 , 0b1111111111111111)
//       OHCRC    = new TH1F("OHCRC", "OH CRC", 0xffff,  0x0 , 0xffff)
//       Vwt      = new TH1F("Vwt", "VFAT word count", 4095,  0x0 , 0xfff)
//       InFU     = new TH1F("InFu", "InFIFO underflow flag", 15,  0x0 , 0xf)
//       Stuckd   = new TH1F("Stuckd", "Stuck data flag", 1,  0x0 , 0b1)
//     }

// }

// class VFAT_histogram: public Hardware_histogram
// {
//   public:

//     VFAT_histogram(const std::string &ifilename, const TDirectory *d)
//     {
//       dir = *d;
//       std::string tmp = ifilename.substr(ifilename.size()-9, ifilename.size());
//       if (tmp != ".raw.root") throw std::runtime_error("Wrong input filename (should end with '.raw.root'): "+ifilename);
//       ifile = new TFile(ifilename.c_str(), "READ");
//       ofilename = ifilename.substr(0,ifilename.size()-9);
//       ofilename += ".analyzed.root";
//       ofile = new TFile(ofilename.c_str(), "RECREATE");
//       this->bookHistograms();
//     }



//   private:

//     TFile *ifile;
//     TFile *ofile;
//     std::string ofilename;
//     TDirectory dir;

//     TH1F* b1010;
//     TH1F* BC;
//     TH1F* b1100;
//     TH1F* EC;
//     TH1F* Flag;
//     TH1F* b1110;
//     TH1F* ChipID;
//     TH1F* lsData;
//     TH1F* msData;
//     TH1F* crc;
//     TH1F* crc_calc;

//     void bookHistograms()
//     {
//       b1010    = new TH1F("b1010", "Control Bits", 15,  0x0 , 0xf)
//       BC       = new TH1F("BC", "Bunch Crossing Number", 4095,  0x0 , 0xfff)      
//       b1100    = new TH1F("b1100", "Control Bits", 15,  0x0 , 0xf)
//       EC       = new TH1F("EC", "Event Counter", 255,  0x0 , 0xff)
//       Flag     = new TH1F("Flag", "Control Flags", 15,  0x0 , 0xf)
//       b1110    = new TH1F("b1110", "Control Bits", 15,  0x0 , 0xf)
//       ChipID   = new TH1F("ChipID", "Chip ID", 4095,  0x0 , 0xfff)
//       lsData   = new TH1F("lsData", "channels from 1 to 64", ?,  0x0 , ?)
//       msData   = new TH1F("msData", "cahnnels from 65 to 128", ?,  0x0 , ?)
//       crc      = new TH1F("crc", "check sum value", 0xffff,  0x0 , 0xffff)
//       crc_calc = new TH1F("crc_calc", "check sum value recalculated", 0xffff,  0x0 , 0xffff)
//     }

// }
