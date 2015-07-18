#ifndef ROOT_Event
#define ROOT_Event

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Event                                                                //
//                                                                      //
// Description of the GEM event                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// some pre-compiler voodoo, CINT...
#ifdef __APPLE__ 
typedef char __signed;
#include <_types/_uint8_t.h> 
#include <_types/_uint16_t.h>
#include <_types/_uint32_t.h> 
#include <_types/_uint64_t.h> 
#else 
#include <stdint.h> 
#endif

#include "TObject.h"
#include "TClonesArray.h"
#include "TRefArray.h"
#include "TRef.h"
#include "TH1.h"
#include "TBits.h"
#include "TMath.h"

//#include "/usr/include/sys/_types/_int8_t.h"
//#include "stdint.h"


class TDirectory;

//class VFATdata : public TObject {
class VFATdata {
    private:
        //uint16_t BC;                    // 1010:4,   BC:12 
        //uint16_t EC;                    // 1100:4,   EC:8,      Flags:4
        //char ChipID;                    // 1110,     ChipID:12
        //uint64_t lsData;                // channels from 1to64 // SHOULD WE HAVE AN ARRAY HERE?
        //uint64_t msData;                // channels from 65to128
        //uint16_t crc;                   // :16
        //double delVT;                   // Threshold scan voltage difference, delVT = deviceVT2 - deviceVT1

        uint8_t b1010;                    // 1010:4,   BC:12 
        uint8_t b1100;                    // 1100:4,   EC:8,      Flags:4
        uint16_t ChipID;                    // 1110,     ChipID:12
        uint8_t Flag;
        uint8_t b1110;
        uint16_t crc;                   // :16
        uint64_t lsData;                // channels from 1to64 // SHOULD WE HAVE AN ARRAY HERE?
        uint64_t msData;                // channels from 65to128

     public:
        VFATdata(){}
//        VFATdata(const uint16_t &BC_, const uint16_t &EC_, const char &ChipID_, const uint64_t &lsData_, const uint64_t &msData_, const uint16_t &crc_) : 
//            BC(BC_),
//            EC(EC_),
//            ChipID(ChipID_),
//            lsData(lsData_),
//            msData(msData_),
//            crc(crc_) {}

        VFATdata(const uint8_t &b1010_, const uint8_t &b1100_, const uint16_t &ChipID_, const uint8_t &Flag_, const uint8_t &b1110_, const uint16_t &crc_) : 
            b1010(b1010_),
            b1100(b1100_),
            ChipID(ChipID_),
            Flag(Flag_),
            b1110(b1110_),
            crc(crc_) {}
         //virtual ~VFATdata();
           ~VFATdata(){}
        // setters
//        void setBC(const uint16_t BC_) {BC = BC_;}
//        void setEC(const uint16_t EC_) {EC = EC_;}
//        void setChipID(const char ChipID_) {ChipID = ChipID_;}
//        void setlsData(const uint64_t lsData_) {lsData = lsData_;}
//        void setmsData(const uint64_t msData_) {msData = msData_;}
//        void setCrc(const uint16_t crc_){crc = crc_;}
//        //void setDelVT(const double delVT_){delVT = delVT_;}
//        // getters
//        uint16_t getBC() {return BC;}
//        uint16_t getEC() {return EC;}
//        char getChipID() {return ChipID;}
//        uint64_t getlsData() {return lsData;}
//        uint64_t getmsData() {return msData;}
//        uint16_t getCrc(){return crc;}

        //ClassDef(VFATdata,1);
};

//class GEBdata : public TObject {
class GEBdata {
    private:
        //uint64_t header;              // ZSFlag:24 ChamID:12 
        //uint32_t ZSFlag;              // array of 24 for ZeroSuppresion flags
        uint64_t ZSFlag;                // array of 24 for ZeroSuppresion flags
        //char ChamID;
        uint64_t ChamID;
        std::vector<VFATdata> vfats;
        //uint64_t trailer;             // OHcrc: 16 OHwCount:16  ChamStatus:16
        //uint16_t OHcrc;
        //uint16_t OHwCount;
        //uint16_t ChamStatus;

        uint64_t OHcrc;
        uint64_t OHwCount;
        uint64_t ChamStatus;
                                        // uint64_t are used to have compatibility with Sergey's code. Review later!

    public:
        GEBdata(){}
        //GEBdata(const uint32_t &ZSFlag_, const char &ChamID_, const uint16_t &OHcrc_, const uint16_t &OHwCount_, const uint16_t &ChamStatus_) : 
//        GEBdata(const uint64_t &ZSFlag_, const uint64_t &ChamID_, const uint16_t &OHcrc_, const uint16_t &OHwCount_, const uint16_t &ChamStatus_) : 
//            ZSFlag(ZSFlag_),
//            ChamID(ChamID_),
//            OHcrc(OHcrc_),
//            OHwCount(OHwCount_),
//            ChamStatus(ChamStatus_) {}

        GEBdata(const uint64_t &ZSFlag_, const uint64_t &ChamID_) : 
            ZSFlag(ZSFlag_),
            ChamID(ChamID_){}

        ~GEBdata(){}
        //virtual ~GEBdata();

        void addVFATData(const VFATdata &vfat_){vfats.push_back(vfat_);}

        void setTrailer(const uint64_t &OHcrc_, const uint64_t &OHwCount_, const uint64_t &ChamStatus_){OHcrc = OHcrc_; OHwCount = OHwCount_; ChamStatus = ChamStatus_;}

        //ClassDef(GEBdata,1);
};

class EventHeader {

    private:
       Int_t   fEvtNum;
       Int_t   fRun;
       Int_t   fDate;
    
    public:
       EventHeader() : fEvtNum(0), fRun(0), fDate(0) { }
       virtual ~EventHeader() { }
       void   Set(Int_t i, Int_t r, Int_t d) { fEvtNum = i; fRun = r; fDate = d; }
       Int_t  GetEvtNum() const { return fEvtNum; }
       Int_t  GetRun() const { return fRun; }
       Int_t  GetDate() const { return fDate; }
    
       ClassDef(EventHeader,1)              //Event Header
};

class Event : public TObject {

    private:
        //char fType[20];                 //event type
        //char *fEventName;               //run+event number in character format
        EventHeader    fEvtHdr;
        
        //uint64_t header1;             // AmcNo:4      0000:4     LV1ID:24   BXID:12     DataLgth:20 
        short AmcNo;
        Int_t LV1ID;                    // What is this? Which var format should be used?
        Int_t BXID;                     // Is it Bunch crossing ID? Should it be Int_t?
        Int_t DataLgth;                 // What is this?
        //uint64_t header2;             // User:32      OrN:16     BoardID:16
        uint16_t OrN;                   // What is this?
        char BoardID;
        //uint64_t header3;               // DAVList:24   BufStat:24 DAVCount:5 FormatVer:3 MP7BordStat:8 
        uint32_t DAVList;
        uint32_t BufStat;
        uint8_t DAVCount;
        unsigned char FormatVer;
        uint8_t MP7BordStat;

        std::vector<GEBdata> gebs;      // Should we use vector or better have TClonesArray here?
        //uint64_t trailer2;            // EventStat:32 GEBerrFlag:24  
        uint32_t EventStat;
        uint32_t GEBerrFlag;
        Int_t nGEBs;                    // Number of GEBs
        //uint64_t trailer1;            // crc:32       LV1IDT:8   0000:4     DataLgth:20 
        uint32_t crc;
        uint8_t LV1IDT;
        Int_t DataLgthT;

        //static TH1F         *fgHist;

    public:
        Event();
        virtual ~Event();
        void SetHeader(Int_t i, Int_t run, Int_t date);
        void Build(const short &AmcNo_, const Int_t &LV1ID_, const Int_t &BXID_, const Int_t &DataLgth_, const uint16_t &OrN_, const char &BoardID_, const uint32_t &DAVList_, const uint32_t &BufStat_, const uint8_t &DAVCount_, const unsigned char &FormatVer_, const uint8_t &MP7BordStat_, const uint32_t &EventStat_, const uint32_t &GEBerrFlag_, const uint32_t &crc_, const uint8_t &LV1IDT_, const Int_t &DataLgthT_);
        //void Build(const short &AmcNo_, const Int_t &LV1ID_, const Int_t &BXID_, const Int_t &DataLgth_, const uint16_t &OrN_, const char &BoardID_, const uint32_t &DAVList_, const uint32_t &BufStat_, const uint8_t &DAVCount_, const unsigned char &FormatVer_, const uint8_t &MP7BordStat_, const std::vector<GEBdata> &gebs_, const uint32_t &EventStat_, const uint32_t &GEBerrFlag_, const uint32_t &crc_, const uint8_t &LV1IDT_, const Int_t &DataLgthT_);
        void addGEBdata(const GEBdata &geb){gebs.push_back(geb); nGEBs = gebs.size();}
        void Clear();
/*
 ____  _        _    ____ _____ _   _  ___  _     ____  _____ ____  
|  _ \| |      / \  / ___| ____| | | |/ _ \| |   |  _ \| ____|  _ \
| |_) | |     / _ \| |   |  _| | |_| | | | | |   | | | |  _| | |_) |
|  __/| |___ / ___ \ |___| |___|  _  | |_| | |___| |_| | |___|  _ < 
|_|   |_____/_/   \_\____|_____|_| |_|\___/|_____|____/|_____|_| \_\
  
*/
        ClassDef(Event,1)               //Event structure
};


/*
class HistogramManager {
 ____  _        _    ____ _____ _   _  ___  _     ____  _____ ____  
|  _ \| |      / \  / ___| ____| | | |/ _ \| |   |  _ \| ____|  _ \
| |_) | |     / _ \| |   |  _| | |_| | | | | |   | | | |  _| | |_) |
|  __/| |___ / ___ \ |___| |___|  _  | |_| | |___| |_| | |___|  _ < 
|_|   |_____/_/   \_\____|_____|_| |_|\___/|_____|____/|_____|_| \_\
                                                                    
    private:
        TH1F  *fNtrack;
        TH1F  *fNseg;
        TH1F  *fTemperature;
        TH1F  *fPx;
        TH1F  *fPy;
        TH1F  *fPz;
        TH1F  *fRandom;
        TH1F  *fMass2;
        TH1F  *fBx;
        TH1F  *fBy;
        TH1F  *fMeanCharge;
        TH1F  *fXfirst;
        TH1F  *fXlast;
        TH1F  *fYfirst;
        TH1F  *fYlast;
        TH1F  *fZfirst;
        TH1F  *fZlast;
        TH1F  *fCharge;
        TH1F  *fNpoint;
        TH1F  *fValid;

    public:
        HistogramManager(TDirectory *dir);
        virtual ~HistogramManager();

        void Hfill(Event *event);

        ClassDef(HistogramManager,1)    //Manages all histograms
};
*/

#endif
