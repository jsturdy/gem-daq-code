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



using namespace std;

class Hardware_histogram
{
public:
  Hardware_histogram(const std::string &ifilename)
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
    this->bookHistograms();
  }
  ~Hardware_histogram(){}

  void fetchHardware()
  {
    TTree *tree = (TTree*)ifile->Get("GEMtree");
    Event *event = new Event();
    TBranch *branch = tree->GetBranch("GEMEvents");
    branch->SetAddress(&event);
    Int_t nentries = tree->GetEntries();
    branch->GetEntry(0);
    v_amc13 = event->amc13s();
    // vector<AMCdata> v_amc;
    // vector<GEBdata> v_geb;
    // vector<VFATdata> v_vfat;
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


  vector<AMC13Event> v_amc13;
  vector<AMCdata> v_amc;
  vector<GEBdata> v_geb;
  vector<VFATdata> v_vfat;


  void bookHistograms()
  {
    int a13_c=0;    //counter through AMC13s
    int a_c=0;      //counter through AMCs
    int g_c=0;      //counter through GEBs
    int v_c=0;      //counter through VFATs
    int vdir_c=0;   //running counter through total number of VDirs

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
      AMC13dir.push_back(ofile->mkdir(diramc13)); //creates a directory and adds it to vector of AMC13 directories
      ofile->cd(diramc13);                        //moves to the newly created directory
      if (DEBUG) std::cout << std::dec << "[gemTreeReader]: AMC13 Directory " << diramc13 << " created" << std::endl;
      //AMC13 HISTOGRAMS HERE
      a_c=0;
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
        AMCdir.push_back(gDirectory->mkdir(diramc)); //creates a directory and adds it to vector of AMC directories
        gDirectory->cd(diramc);                      //moves to newly created directory
        //AMC HISTOGRAMS HERE
        g_c=0;
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
          GEBdir.push_back(gDirectory->mkdir(dirgeb)); //creates a directory and adds it to vector of GEB directories
          gDirectory->cd(dirgeb);                      //moves to the newly created directory
          //GEB HISTOGRAMS HERE
          v_c=0;
          for(auto v=v_vfat.begin(); v!=v_vfat.end();v++){
            char dirvfat[30];   //filename for VFAT directory
            dirvfat[0]='\0';    
            char v_ch[2];       //char used to put VFAT slot into directory name
            v_ch[0]='\0';
            sprintf(v_ch, "%d", v_c);
            char vslot_ch[2];
            vslot_ch[0] = '\0';
            std::unique_ptr<gem::readout::GEMslotContents> slotInfo_ = std::unique_ptr<gem::readout::GEMslotContents> (new gem::readout::GEMslotContents("slot_table_TAMUv2.csv"));     
            int vslot = slotInfo_->GEBslotIndex(v_vfat[v_c].ChipID());  //converts Chip ID into VFAT slot number
            sprintf(vslot_ch, "%d", vslot);
            strcat(dirvfat,"VFAT-");
            strcat(dirvfat, vslot_ch);
            if (DEBUG) std::cout << std::dec << "[gemTreeReader]: VFAT Directory " << dirvfat << " created" << std::endl;
            VFATdir.push_back(gDirectory->mkdir(dirvfat));  //creates a directory and adds it to vector of VFAT directories
            gDirectory->cd(dirvfat);                        //moves to the newly created directory

            //VFAT HISTOGRAMS HERE
	    this->createVFATHistograms(&v_vfat[v_c],vslot,VFATdir[VFATdir.size()-1]);


            gDirectory->cd("..");   //moves back to previous directory
            v_c++;
          }
          gDirectory->cd("..");     //moves back to previous directory
          
          g_c++;
        }
        gDirectory->cd("..");       //moves back to previous directory

       	a_c++;
      }
     
      a13_c++;
    }
    ofile->Write();
  }

  void createVFATHistograms(VFATdata *vfat, int slot, TDirectory* vdir)
  {
    if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Booking VFAT Histograms for " << vdir->GetName() << std::endl;   
    //gDirectory->cd(vdir->GetPath());
    vdir->cd();

    std::string slot_s = "Slot";
    slot_s += to_string(static_cast<long long>(slot)); //string Slot#

    hi1010 = new TH1I((slot_s+"_1010").c_str(), "Control Bits 1010", 15, 0x0, 0xf );
    hi1100 = new TH1I((slot_s+"_1100").c_str(), "Control Bits 1100", 15, 0x0, 0xf );
    hi1110 = new TH1I((slot_s+"_1110").c_str(), "Control Bits 1110", 15, 0x0, 0xf );

    hiBC     = new TH1I((slot_s+"_BC").c_str(), "Bunch Crossing Number", 0xfff, 0x0, 0xfff);
    hiEC     = new TH1I((slot_s+"_EV").c_str(), "Event Counter", 255, 0x0, 0xff);
    hiFlag   = new TH1I((slot_s+"_Flag").c_str(), "Control Flag", 15, 0x0, 0xf);
    hiChipID = new TH1I((slot_s+"_ChipID").c_str(), "Chip ID", 0xfff, 0x0, 0xfff);
    hicrc    = new TH1I((slot_s+"_CRC").c_str(), "Check Sum Value", 0xffff, 0x0, 0xffff);
    hiBC->Fill(vfat->BC());
    hiEC->Fill(vfat->EC());
    hiFlag->Fill(vfat->Flag());
    hiChipID->Fill(vfat->ChipID());
    hicrc->Fill(vfat->crc());
    setTitles(hiBC, "bunch crossing number", "Number of VFAT blocks");
    setTitles(hiEC, "event counter",         "Number of VFAT blocks");
    setTitles(hiFlag, "control flag",          "Number of VFAT blocks");
    setTitles(hiChipID, "chip ID",               "Number of VFAT blocks");
    setTitles(hicrc, "check sum value",       "Number of VFAT blocks");
    
    if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Filling VFAT Histograms for " << vdir->GetName() << std::endl;   
    this->fillVFATHistograms(vfat, hi1010,hi1100,hi1110);

  }


  void fillVFATHistograms(VFATdata *m_vfat, TH1I* m_hi1010, TH1I* m_hi1100, TH1I* m_hi1110)
  {

    m_hi1010->Fill(m_vfat->b1010());
    m_hi1100->Fill(m_vfat->b1100());
    m_hi1110->Fill(m_vfat->b1110());

    setTitles(hi1010, "1010 marker, max 0xf", "Number of VFAT blocks");   
    setTitles(hi1100, "1100 marker, max 0xf", "Number of VFAT blocks");   
    setTitles(hi1110, "1110 marker, max 0xf", "Number of VFAT blocks"); 

  }

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



  /* 
     NOTE: Abandoning full integration (below). Starting over with one VFAT histogram and
     building up from there.
  */


  // void bookVFATHistograms(VFATdata vfat, int slot, TDirectory* vdir)
  // {

  //   int nVFAT[3] = {0,0,0};
  //   int nBadVFAT[3] = {0,0,0};
  //   int nGoodVFAT[3] = {0,0,0};
  //   int firedchannels[3] = {0,0,0};
  //   int notfiredchannels[3] = {0,0,0};

  //   if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Changing to Directory: " << vdir->GetName() << std::endl;   

  //   //gDirectory->cd(vdir->GetPath());
  //   vdir->cd();
  //   std::string slot_s = "Slot";
  //   slot_s += to_string(static_cast<long long>(slot));
  //   std::string dirname[3] = {"AllEvents", "GoodEvents", "BadEvents"};
  //   std::string eta_partitions[8] = {"eta_1", "eta_2", "eta_3", "eta_4", "eta_5", "eta_6", "eta_7", "eta_8"};
  //   std::string tempname = "OtherData";
  //   char name[4][128], title[4][500];

  //   // std::string type[NVFAT] = {"Slot0" , "Slot1" , "Slot2" , "Slot3" , "Slot4" , "Slot5" , "Slot6" , "Slot7", 
  //   // 			       "Slot8" , "Slot9" , "Slot10", "Slot11", "Slot12", "Slot13", "Slot14", "Slot15", 
  //   // 			       "Slot16", "Slot17", "Slot18", "Slot19", "Slot20", "Slot21", "Slot22", "Slot23"};

  //   if (DEBUG) std::cout << std::dec << "[gemTreeReader]: " << slot_s << std::endl;   



  //   if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Make directories and create histograms inside" << std::endl;   
  //   for (int i = 0; i < 3; i++) {
  //     vdir->cd();
  //     dir[i] = gDirectory->mkdir(dirname[i].c_str());
  //     gDirectory->cd(dirname[i].c_str());
  //     if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Directory " << i+1 << " created" << std::endl;   

  //     gDirectory->mkdir(tempname.c_str());
  //     gDirectory->cd(tempname.c_str());
  //     hiVFAT         [i] = new TH1F((dirname[i]+"_VFAT").c_str(), "Number VFAT blocks per event", 24,  0., 24. );
  //     hiVFATsn       [i] = new TH1F((dirname[i]+"_VFATsn").c_str(), "VFAT slot number", 24,  0., 24. );
  //     hiDiffBXandBC  [i] = new TH1I((dirname[i]+"_DiffBXandBC").c_str(), "Difference of BX and BC", 100000, 0x0, 0x1869F );
  //     hiRatioBXandBC [i] = new TH1I((dirname[i]+"_RatioBXandBC").c_str(), "Ratio of BX and BC", 1000, 0x0, 0xa );
  //     hiChip         [i] = new TH1I((dirname[i]+"_ChipID").c_str(), "ChipID",         4096, 0x0, 0xfff );
  //     hi1010         [i] = new TH1I((dirname[i]+"_1010").c_str(), "Control Bits 1010", 15, 0x0, 0xf );
  //     hi1100         [i] = new TH1I((dirname[i]+"_1100").c_str(), "Control Bits 1100", 15, 0x0, 0xf );
  //     hi1110         [i] = new TH1I((dirname[i]+"_1110").c_str(), "Control Bits 1110", 15, 0x0, 0xf );
  //     hiFlag         [i] = new TH1I((dirname[i]+"_Flag").c_str()  , "Flag",            15, 0x0, 0xf );
  //     hiCRC          [i] = new TH1I((dirname[i]+"_CRC").c_str(),     "CRC",             100, 0x0, 0xffff );
  //     hiDiffCRC      [i] = new TH1I((dirname[i]+"_DiffCRC").c_str(), "CRC_Diff",    100, -32767, 32767 );
  //     hiFake         [i] = new TH1I((dirname[i]+"_Fake").c_str(), "Number of bad VFAT blocks in event",    24, 0x0, 0x18 );
  //     hiSignal       [i] = new TH1I((dirname[i]+"_Signal").c_str(), "Number of good VFAT blocks in event",    24, 0x0, 0x18 );
  //     hichfired      [i] = new TH1I((dirname[i]+"_chfired").c_str(), "Channels fired per event",      500, 0., 500. );
  //     hichnotfired   [i] = new TH1I((dirname[i]+"_chnotfired").c_str(), "Channels not fired per event",      500, 0., 500. );
  //     hiCh_notfired  [i] = new TH1F((dirname[i]+"_Ch_notfired").c_str(), "Strips",          128, 0., 128. );
  //     hiCh128        [i] = new TH1F((dirname[i]+"_Ch128").c_str(), "Strips",          128, 0., 128. );
  //     hi2DCRC        [i] = new TH2I((dirname[i]+"_CRC1_vs_CRC2").c_str(), "CRC_{calc} vs CRC_{VFAT}", 100, 0x0000, 0xffff, 100, 0x0000, 0xffff);
  //     hiClusterMult  [i] = new TH1I((dirname[i]+"_ClusterMult").c_str(), "Cluster multiplicity", 384,  0, 384 );
  //     hiClusterSize  [i] = new TH1I((dirname[i]+"_ClusterSize").c_str(), "Cluster size", 384,  0, 384 );
  //     hiBeamProfile  [i] = new TH2I((dirname[i]+"_BeamProfile").c_str(), "Beam Profile", 8, 0, 8, 384, 0, 384);
  //     if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Main histograms ["<<i<<"] created" << std::endl;

  //     if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Filling histograms" << std::endl;
  //     //this->fillVFATHistograms(vfat, hiVFATsn[i], hiCh128[i], hiCh_notfired[i], hiChip[i], hi1010[i], hi1100[i], hi1110[i], hiFlag[i], hiCRC[i], hiDiffCRC[i], hi2DCRC[i], hi2DCRCperVFAT[i], hiCh128chipFired[i], hiStripsFired[i], hiBeamProfile[i], firedchannels[i], notfiredchannels[i]);
  //     if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Filling complete" << std::endl;

  //     for(int ie=0; ie < NETA; ie++){
  // 	hiClusterMultEta  [i][ie] = new TH1I((dirname[i]+"_ClusterMult"+eta_partitions[ie]).c_str(), "Cluster multiplicity", 384,  0, 384 );
  // 	hiClusterSizeEta  [i][ie] = new TH1I((dirname[i]+"_ClusterSize"+eta_partitions[ie]).c_str(), "Cluster size", 384,  0, 384 );
  //     }
  //     //for(int j=0; j < NVFAT; j++){
  // 	dir[i]->cd();
  // 	gDirectory->mkdir(slot_s.c_str());
  // 	gDirectory->cd(slot_s.c_str());
  // 	if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Start histograms for VFAT slot: " << slot << std::endl;   
  // 	sprintf (name[0]  , (dirname[i]+"_hiVFATfired_perevent_%s").c_str(), slot_s.c_str());
  // 	sprintf (title[0] , "VFAT chip %s fired per event", slot_s.c_str());
  // 	//if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Get name and title for hiVFATfired_perevent["<<i<<"]["<<j<<"]: name : " << name << " title : " << title << std::endl;   
  // 	sprintf (name[1] , (dirname[i]+"_hiCh128chipFired_%s").c_str(), slot_s.c_str());
  // 	sprintf (title[1], "Channels fired for VFAT chip %s", slot_s.c_str());
  // 	sprintf (name[3] , (dirname[i]+"_hiStripsFired_%s").c_str(), slot_s.c_str());
  // 	sprintf (title[3], "Strips fired for VFAT chip %s", slot_s.c_str());
  // 	//if (DEBUG) std::cout << std::dec << "[gemTreeReader]: Get name and title for hiCh128chipFired["<<i<<"]["<<j<<"]" << std::endl;   
  // 	hiVFATfired_perevent[i][slot] = new TH1F(name[0], title[0], 20, 0., 20.);
  // 	hiCh128chipFired    [i][slot] = new TH1F(name[1], title[1], 128, 0., 128.);
  // 	hiStripsFired    [i][slot] = new TH1F(name[3], title[3], 129, 0., 129.);
  // 	//if (DEBUG) std::cout << std::dec << "[gemTreeReader]: 2d array of histograms ["<<i<<"]["<<j<<"] created" << std::endl;   
  // 	sprintf (name[2] , (dirname[i]+"_2DCRCperVFAT_%s").c_str(), slot_s.c_str());
  // 	sprintf (title[2], "2D CRC for VFAT chip %s", slot_s.c_str());
  // 	hi2DCRCperVFAT      [i][slot] = new TH2I(name[2], title[2], 100, 0x0000, 0xffff, 100, 0x0000, 0xffff);
  // 	//}
  //   }

    

  // }

  // void fillVFATHistograms(VFATdata *m_vfat, TH1F* m_hiVFATsn, TH1F* m_hiCh128, TH1F* m_hiCh_notfired, TH1I* m_hiChip, TH1I* m_hi1010, TH1I* m_hi1100, 
  // 			  TH1I* m_hi1110, TH1I* m_hiFlag, TH1I* m_hiCRC, TH1I* m_hiDiffCRC, TH2I* m_hi2DCRC, TH2I* m_hi2DCRCperVFAT[], 
  // 	                  TH1F* m_hiCh128chipFired[], TH1F* m_hiStripsFired[], TH2I* m_hiBeamProfile, int & firedchannels, 
  // 			  int & notfiredchannels)
  // {



  //   // fill the control bits histograms
  //   m_hi1010->Fill(m_vfat->b1010());
  //   m_hi1100->Fill(m_vfat->b1100());
  //   m_hi1110->Fill(m_vfat->b1110());
  //   // fill Flag and chip id histograms
  //   m_hiFlag->Fill(m_vfat->Flag());
  //   m_hiChip->Fill(m_vfat->ChipID());
  //   // calculate and fill VFAT slot number
  //   //int sn_ = m_vfat->SlotNumber();
  //   int sn_ = -1;
  //   m_hiVFATsn->Fill(sn_);
  //   // calculate and fill the crc and crc_diff
  //   m_hiCRC->Fill(m_vfat->crc());
  //   // CRC check
  //   uint16_t b1010 = (0x000f & m_vfat->b1010());
  //   uint16_t b1100 = (0x000f & m_vfat->b1100());
  //   uint16_t b1110 = (0x000f & m_vfat->b1110());
  //   uint16_t flag  = (0x000f & m_vfat->Flag());
  //   uint16_t ec    = (0x00ff & m_vfat->EC());
  //   //BC             = m_vfat->BC();
  //   if (DEBUG) std::cout << "[gemTreeReader]: CRC read from vfat : " << std::hex << m_vfat->crc() << std::endl;
  //   if (DEBUG) std::cout << "[gemTreeReader]: CRC recalculated   : " << std::hex << m_vfat->crc_calc() << std::endl;
  //   m_hiDiffCRC->Fill(m_vfat->crc()-m_vfat->crc_calc());
  //   m_hi2DCRC->Fill(m_vfat->crc(), m_vfat->crc_calc());
  //   //I think it would be nice to time this...
  //   uint16_t chan0xf = 0;
  //   for (int chan = 0; chan < 128; ++chan) {
  //     if (chan < 64){
  // 	chan0xf = ((m_vfat->lsData() >> chan) & 0x1);
  // 	if(chan0xf) {
  // 	  m_hiCh128->Fill(chan);
  // 	  firedchannels++;
  // 	} else {
  // 	  m_hiCh_notfired->Fill(chan);
  // 	  notfiredchannels++;
  // 	}
  //     } else {
  // 	chan0xf = ((m_vfat->msData() >> (chan-64)) & 0x1);
  // 	if(chan0xf) {
  // 	  m_hiCh128->Fill(chan);
  // 	  firedchannels++;
  // 	} else {
  // 	  m_hiCh_notfired->Fill(chan);
  // 	  notfiredchannels++;
  // 	}
  //     }
  //   }// end of loop over channels 
  //   // strip container mapped for eta partitions...

  //   for(int m=0; m < NVFAT; m++){
  //     if(sn_ == m){
  // 	if (DEBUG) std::cout << "[gemTreeReader]: Starting to fill hiCh128chipFired for slot : " << m << std::dec << std::endl;
  // 	if (DEBUG) std::cout << "[gemTreeReader]: LS data           " << std::bitset<64>(m_vfat->lsData()) <<  std::endl;
  // 	if (DEBUG) std::cout << "[gemTreeReader]: MS data           " << std::bitset<64>(m_vfat->msData()) <<  std::endl;
  // 	m_hi2DCRCperVFAT[m]->Fill(m_vfat->crc(), m_vfat->crc_calc());
  // 	uint16_t chan0xfFiredchip = 0;
  // 	for (int chan = 0; chan < 128; ++chan) {
  // 	  if (chan < 64){
  // 	    chan0xfFiredchip = ((m_vfat->lsData() >> chan) & 0x1);
  // 	    if(chan0xfFiredchip) {
  // 	      tmp_strips[chan] += 1;
  // 	      m_hiCh128chipFired[m]->Fill(chan);
  // 	      m_hiStripsFired[m]->Fill(strip_maps[m].find(chan+1)->second);
  // 	      int m_i = (int) m_vfat->SlotNumber()%8;
  // 	      int m_j = 127 - strip_maps[m].find(chan+1)->second + ((int) m/8)*128;
  // 	      if (allstrips.find(m_i) == allstrips.end()){
  // 		GEMStripCollection strips;
  // 		allstrips[m_i]=strips;
  // 	      }
  // 	      // bx set to 0...
  // 	      GEMStrip s(m_j,0);
  // 	      allstrips[m_i].insert(s);
  // 	      if (DEBUG) std::cout << "[gemTreeReader]: Beam profile x : " << m_i << " Beam profile y : " << m_j <<  std::endl;
  // 	      m_hiBeamProfile->Fill(m_i,m_j);
  // 	    }
  // 	  } else {
  // 	    chan0xfFiredchip = ((m_vfat->msData() >> (chan-64)) & 0x1);
  // 	    if(chan0xfFiredchip) {
  // 	      tmp_strips[chan] += 1;
  // 	      m_hiCh128chipFired[m]->Fill(chan);
  // 	      m_hiStripsFired[m]->Fill(strip_maps[m].find(chan+1)->second);
  // 	      int m_i = (int) m_vfat->SlotNumber()%8;
  // 	      int m_j = 127 - strip_maps[m].find(chan+1)->second + ((int) m/8)*128;
  // 	      if (allstrips.find(m_i) == allstrips.end()){
  // 		GEMStripCollection strips;
  // 		allstrips[m_i]=strips;
  // 	      }
  // 	      // bx set to 0...
  // 	      GEMStrip s(m_j,0);
  // 	      allstrips[m_i].insert(s);
  // 	      if (DEBUG) std::cout << "[gemTreeReader]: Beam profile x : " << m_i << " Beam profile y : " << m_j <<  std::endl;
  // 	      m_hiBeamProfile->Fill(m_i,m_j);
  // 	    }
  // 	  }
  // 	}
  //     } 
  //   } // end of VFAT loop
  // }

private:
  TFile *ifile;
  TFile *ofile;
  std::string ofilename;

  std::vector<int> tmp_strips;

  std::map<int,int> strip_maps[NVFAT];
  std::string maps[NVFAT];
  std::map<int, GEMStripCollection> allstrips;

  std::vector<TDirectory*> AMC13dir;
  std::vector<TDirectory*> AMCdir;
  std::vector<TDirectory*> GEBdir;
  std::vector<TDirectory*> VFATdir;

  TH1F* hiVFAT                     [3]; // Number of VFATs in event
  TH1F* hiVFATsn                   [3]; // VFAT slot number distribution
  TH1I* hiChip                     [3]; // VFAT ChipID distribution
  TH1I* hi1010                     ; // Control bit 1010
  TH1I* hi1100                     ; // Control bit 1100
  TH1I* hi1110                     ; // Control bit 1110
  TH1I* hiDiffCRC                  [3]; // CRC difference between the one supplied by VFAT and recomputed one
  TH1I* hiFake                     [3]; // Number of bad VFAT blocks in event 
  TH1F* hiCh128                    [3];     
  TH2I* hi2DCRC                    [3];     
  TH1I* hiDiffBXandBC              [3];
  TH1I* hiRatioBXandBC             [3];
  TH1I* hiSignal                   [3]; // Number of good VFAT blocks in event
  TH1I* hichfired                  [3];
  TH1I* hichnotfired               [3];
  TH1F* hiCh_notfired              [3];
  TH1F* hiVFATfired_perevent[3][NVFAT];
  TH1F* hiCh128chipFired    [3][NVFAT];
  TH1F* hiStripsFired       [3][NVFAT];
  TH2I* hi2DCRCperVFAT      [3][NVFAT];
  TH1I* hiClusterMult              [3];
  TH1I* hiClusterSize              [3];
  TH1I* hiClusterMultEta     [3][NETA];
  TH1I* hiClusterSizeEta     [3][NETA];
  TH2I* hiBeamProfile              [3];
  TH1I* hiBC                          ; //Bunch Crossing Number
  TH1I* hiEC                          ; //Event Counter
  TH1I* hiFlag                        ; //VFAT Flag
  TH1I* hiChipID                      ; //VFAT Chip ID
  TH1I* hicrc                         ; //Check Value Sum


  TDirectory *dir[3];


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
//       GEMDAV     = new TH1F("GEMDAV", "GEM DAV list", 0xffffff,  0x0 , 0xffffff)
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
