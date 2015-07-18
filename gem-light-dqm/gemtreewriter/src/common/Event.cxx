// Author: Mykhailo Dalchenko   12/07/2015

////////////////////////////////////////////////////////////////////////
//
//                          GEM Event classes
//                       =======================
//
//  The VFATdata class for the GEM detector
//
//    private:
//        uint16_t BC;                    // 1010:4,   BC:12 
//        uint16_t EC;                    // 1100:4,   EC:8,      Flags:4
//        char ChipID;                    // 1110,     ChipID:12
//        uint64_t lsData;                // channels from 1to64 // SHOULD WE HAVE AN ARRAY HERE?
//        uint64_t msData;                // channels from 65to128
//        uint16_t crc;                   // :16
//        double delVT;                   // Threshold scan voltage difference, delVT = deviceVT2 - deviceVT1
//
//  All these variables are accessible bia public setters and getters. Also the constructor which sets all the values is provided. 
//  Destructor is virtual due to requirement of TObject inheritance. 
//
//  The GEBdata class represents the data structure coming from a single GEB board.
//
//    private:
//        //uint64_t header;              // ZSFlag:24 ChamID:12 
//        bool[24] ZSFlag;                // array of 24 for ZeroSuppresion flags
//        char ChamID;
//        std::vector<VFATdata> vfats;
//        //uint64_t trailer;             // OHcrc: 16 OHwCount:16  ChamStatus:16
//        uint16_t OHcrc;
//        uint16_t OHwCount;
//        uint16_t ChamStatus;
//
//  The Event class is a naive/simple example of a GEM event structure.
//    private:
//        //char fType[20];                 //event type
//        //char *fEventName;               //run+event number in character format
//        EventHeader    fEvtHdr;
//        
//        //uint64_t header1;             // AmcNo:4      0000:4     LV1ID:24   BXID:12     DataLgth:20 
//        short AmcNo;
//        Int_t LV1ID;                    // What is this? Which var format should be used?
//        Int_t BXID;                     // Is it Bunch crossing ID? Should it be Int_t?
//        Int_t DataLgth;                 // What is this?
//        //uint64_t header2;             // User:32      OrN:16     BoardID:16
//        uint16_t OrN;                   // What is this?
//        char BoardID;
//        //uint64_t header3;               // DAVList:24   BufStat:24 DAVCount:5 FormatVer:3 MP7BordStat:8 
//        uint32_t DAVList;
//        uint32_t BufStat;
//        uint8_t DAVCount;
//        unsigned char FormatVer;
//        uint8_t MP7BordStat;
//
//        std::vector<GEBdata> gebs;      // Should we use vector or better have TClonesArray here?
//        //uint64_t trailer2;            // EventStat:32 GEBerrFlag:24  
//        uint32_t EventStat;
//        bool[24] GEBerrFlag;
//        Int_t nGEBs;                    // Number of GEBs
//        //uint64_t trailer1;            // crc:32       LV1IDT:8   0000:4     DataLgth:20 
//        uint32_t crc;
//        uint8_t LV1IDT;
//        Int_t DataLgthT;
//
//  The EventHeader class has 3 data members (integers):
//     public:
//        Int_t          fEvtNum;
//        Int_t          fRun;
//        Int_t          fDate;
//
//  There's also a placeholder for the histogram maker. More details can be found in the Event.cxx ROOT example.
//  A good example of how to fill the tree with events is provided here:
//  https://root.cern.ch/root/html/tutorials/tree/tree4.C.html
//
////////////////////////////////////////////////////////////////////////

#include "RVersion.h"
#include "TRandom.h"
#include "TDirectory.h"
#include "TProcessID.h"

#include "Event.h"


//ClassImp(Track)
ClassImp(EventHeader)
ClassImp(Event)
//ClassImp(HistogramManager)

//TH1F *Event::fgHist = 0;

Event::Event()
{
   // Create an Event object.
   // When the constructor is invoked for the first time, the class static
   // variable fgTracks is 0 and the TClonesArray fgTracks is created.

   Clear();
}

//______________________________________________________________________________
Event::~Event()
{
   Clear();
}

//______________________________________________________________________________
//void Event::Build(const short &AmcNo_, const Int_t &LV1ID_, const Int_t &BXID_, const Int_t &DataLgth_, const uint16_t &OrN_, const char &BoardID_, const uint32_t &DAVList_, const uint32_t &BufStat_, const uint8_t &DAVCount_, const unsigned char &FormatVer_, const uint8_t &MP7BordStat_, const std::vector<GEBdata> &gebs_, const uint32_t &EventStat_, const uint32_t &GEBerrFlag_, const uint32_t &crc_, const uint8_t &LV1IDT_, const Int_t &DataLgthT_) 
void Event::Build(const short &AmcNo_, const Int_t &LV1ID_, const Int_t &BXID_, const Int_t &DataLgth_, const uint16_t &OrN_, const char &BoardID_, const uint32_t &DAVList_, const uint32_t &BufStat_, const uint8_t &DAVCount_, const unsigned char &FormatVer_, const uint8_t &MP7BordStat_, const uint32_t &EventStat_, const uint32_t &GEBerrFlag_, const uint32_t &crc_, const uint8_t &LV1IDT_, const Int_t &DataLgthT_) 
{
    //Save current Object count
    Int_t ObjectNumber = TProcessID::GetObjectCount();
    Clear();
    AmcNo = AmcNo_;
    LV1ID = LV1ID_;
    BXID = BXID_;
    DataLgth = DataLgth_;
    OrN = OrN_;
    BoardID = BoardID_;
    DAVList = DAVList_;
    BufStat = BufStat_;
    DAVCount = DAVCount_;
    FormatVer = FormatVer_;
    MP7BordStat = MP7BordStat_;
    //gebs = gebs_;
    EventStat = EventStat_;
    GEBerrFlag = GEBerrFlag_;
    //nGEBs = gebs.size();
    crc = crc_;
    LV1IDT = LV1IDT_;
    DataLgthT = DataLgthT_;

    //Restore Object count 
    //To save space in the table keeping track of all referenced objects
    //we assume that our events do not address each other. We reset the 
    //object count to what it was at the beginning of the event.
    TProcessID::SetObjectCount(ObjectNumber);
}  

//______________________________________________________________________________
void Event::SetHeader(Int_t i, Int_t run, Int_t date)
{
   fEvtHdr.Set(i, run, date);
}

//______________________________________________________________________________
void Event::Clear()
{
    AmcNo = -1;
    LV1ID = -1;
    BXID = -1;
    DataLgth = -1;
    OrN = 0;
    BoardID = -1;
    DAVList = 0;
    BufStat = 0;
    DAVCount = 0;
    FormatVer = 0;
    MP7BordStat = 0;
    gebs.clear();
    EventStat = 0;
    GEBerrFlag = 0;
    nGEBs = 0;
    crc = 0;
    LV1IDT = 0;
    DataLgthT = -1;
}

/*
HistogramManager::HistogramManager(TDirectory *dir)
{
   // Create histogram manager object. Histograms will be created
   // in the "dir" directory.

   // Save current directory and cd to "dir".
   TDirectory *saved = gDirectory;
   dir->cd();

   fNtrack      = new TH1F("hNtrack",    "Ntrack",100,575,625);
   fNseg        = new TH1F("hNseg",      "Nseg",100,5800,6200);
   fTemperature = new TH1F("hTemperature","Temperature",100,19.5,20.5);
   fPx          = new TH1F("hPx",        "Px",100,-4,4);
   fPy          = new TH1F("hPy",        "Py",100,-4,4);
   fPz          = new TH1F("hPz",        "Pz",100,0,5);
   fRandom      = new TH1F("hRandom",    "Random",100,0,1000);
   fMass2       = new TH1F("hMass2",     "Mass2",100,0,12);
   fBx          = new TH1F("hBx",        "Bx",100,-0.5,0.5);
   fBy          = new TH1F("hBy",        "By",100,-0.5,0.5);
   fMeanCharge  = new TH1F("hMeanCharge","MeanCharge",100,0,0.01);
   fXfirst      = new TH1F("hXfirst",    "Xfirst",100,-40,40);
   fXlast       = new TH1F("hXlast",     "Xlast",100,-40,40);
   fYfirst      = new TH1F("hYfirst",    "Yfirst",100,-40,40);
   fYlast       = new TH1F("hYlast",     "Ylast",100,-40,40);
   fZfirst      = new TH1F("hZfirst",    "Zfirst",100,0,80);
   fZlast       = new TH1F("hZlast",     "Zlast",100,0,250);
   fCharge      = new TH1F("hCharge",    "Charge",100,-1.5,1.5);
   fNpoint      = new TH1F("hNpoint",    "Npoint",100,50,80);
   fValid       = new TH1F("hValid",     "Valid",100,0,1.2);

   // cd back to original directory
   saved->cd();
}

//______________________________________________________________________________
HistogramManager::~HistogramManager()
{
   // Clean up all histograms.

   // Nothing to do. Histograms will be deleted when the directory
   // in which tey are stored is closed.
}

//______________________________________________________________________________
void HistogramManager::Hfill(Event *event)
{
   // Fill histograms.

   fNtrack->Fill(event->GetNtrack());
   fNseg->Fill(event->GetNseg());
   fTemperature->Fill(event->GetTemperature());

   for (Int_t itrack = 0; itrack < event->GetNtrack(); itrack++) {
      Track *track = (Track*)event->GetTracks()->UncheckedAt(itrack);
      fPx->Fill(track->GetPx());
      fPy->Fill(track->GetPy());
      fPz->Fill(track->GetPz());
      fRandom->Fill(track->GetRandom());
      fMass2->Fill(track->GetMass2());
      fBx->Fill(track->GetBx());
      fBy->Fill(track->GetBy());
      fMeanCharge->Fill(track->GetMeanCharge());
      fXfirst->Fill(track->GetXfirst());
      fXlast->Fill(track->GetXlast());
      fYfirst->Fill(track->GetYfirst());
      fYlast->Fill(track->GetYlast());
      fZfirst->Fill(track->GetZfirst());
      fZlast->Fill(track->GetZlast());
      fCharge->Fill(track->GetCharge());
      fNpoint->Fill(track->GetNpoint());
      fValid->Fill(track->GetValid());
   }
}
*/
