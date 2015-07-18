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
        uint8_t b1010;                    // 1010:4 Control bits, shoud be 1010
        uint16_t BC;                      // Bunch Crossing number, 12 bits
        uint8_t b1100;                    // 1100:4, Control bits, shoud be 1100
        uint16_t EC;                      // Event Counter, 8 bits
        uint8_t Flag;                     // Control Flags: 4 bits, Hamming Error/AFULL/SEUlogic/SUEI2C
        uint8_t b1110;                    // 1110:4 Control bits, shoud be 1110
        uint16_t ChipID;                  // Chip ID, 12 bits
        uint64_t lsData;                  // channels from 1to64 
        uint64_t msData;                  // channels from 65to128
        uint16_t crc;                     // Check Sum value, 16 bits

    public:
        VFATdata(){}
        VFATdata(const uint8_t &b1010_, 
                const uint16_t &BC_,
                const uint8_t &b1100_, 
                const uint8_t &EC_,
                const uint8_t &Flag_, 
                const uint8_t &b1110_, 
                const uint16_t &ChipID_, 
                const uint64_t &lsData_, 
                const uint64_t &msData_, 
                const uint16_t &crc_) : 
            b1010(b1010_),
            BC(BC_),
            b1100(b1100_),
            EC(EC_),
            Flag(Flag_),
            b1110(b1110_),
            ChipID(ChipID_),
            lsData(lsData_),
            msData(msData_),
            crc(crc_) {}
        ~VFATdata(){}
};

//class GEBdata : public TObject {
class GEBdata {
    private:
        uint32_t ZSFlag;                // ZeroSuppresion flags, 24 bits
        uint16_t ChamID;                // Chamber ID, 12 bits
        uint32_t sumVFAT;               // Rest part of the header, reserved for the moment
        std::vector<VFATdata> vfats;
        uint16_t OHcrc;                 // OH Check Sum, 16 bits
        uint16_t OHwCount;              // OH Counter, 16 bits
        uint16_t ChamStatus;            // Chamber Status, 16 bits
        uint16_t GEBres;                // Reserved part of trailer

    public:
        GEBdata(){}
        GEBdata(const uint32_t &ZSFlag_, const uint16_t &ChamID_, const uint32_t &sumVFAT_) : 
            ZSFlag(ZSFlag_),
            ChamID(ChamID_),
            sumVFAT(sumVFAT_){}
        ~GEBdata(){}

        void addVFATData(const VFATdata &vfat_){vfats.push_back(vfat_);}

        void setTrailer(const uint16_t &OHcrc_, const uint16_t &OHwCount_, const uint16_t &ChamStatus_, const uint16_t &GEBres_)
        {OHcrc = OHcrc_; OHwCount = OHwCount_; ChamStatus = ChamStatus_; GEBres = GEBres_;}
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
        EventHeader    fEvtHdr;
        
        //uint64_t header1;             // AmcNo:4      0000:4     LV1ID:24   BXID:12     DataLgth:20 
        uint8_t AmcNo;
        uint8_t b0000;
        uint32_t LV1ID;                    // What is this? Which var format should be used?
        uint16_t BXID;                     // Is it Bunch crossing ID? Should it be Int_t?
        uint32_t DataLgth;                 // What is this?
        //uint64_t header2;             // User:32      OrN:16     BoardID:16
        uint16_t OrN;                   // What is this?
        uint16_t BoardID;
        //uint64_t header3;               // DAVList:24   BufStat:24 DAVCount:5 FormatVer:3 MP7BordStat:8 
        uint32_t DAVList;
        uint32_t BufStat;
        uint8_t DAVCount;
        uint8_t FormatVer;
        uint8_t MP7BordStat;

        std::vector<GEBdata> gebs;      // Should we use vector or better have TClonesArray here?
        //uint64_t trailer2;            // EventStat:32 GEBerrFlag:24  
        uint32_t EventStat;
        uint32_t GEBerrFlag;
        //uint64_t trailer1;            // crc:32       LV1IDT:8   0000:4     DataLgth:20 
        uint32_t crc;
        uint8_t LV1IDT;
        uint8_t b0000T;
        uint32_t DataLgthT;

    public:
        Event();
        virtual ~Event();
        void SetHeader(Int_t i, Int_t run, Int_t date);
        void Build(const uint8_t &AmcNo_, 
            const uint8_t &b0000_,
            const uint32_t &LV1ID_, 
            const uint16_t &BXID_, 
            const uint32_t &DataLgth_, 
            const uint16_t &OrN_, 
            const uint16_t &BoardID_, 
            const uint32_t &DAVList_, 
            const uint32_t &BufStat_, 
            const uint8_t &DAVCount_, 
            const uint8_t &FormatVer_, 
            const uint8_t &MP7BordStat_, 
            const uint32_t &EventStat_, 
            const uint32_t &GEBerrFlag_, 
            const uint32_t &crc_, 
            const uint8_t &LV1IDT_, 
            const uint8_t &b0000T_, 
            const uint32_t &DataLgthT_);
        void addGEBdata(const GEBdata &geb){gebs.push_back(geb);}
        void Clear();

        ClassDef(Event,1)               //Event structure
};

#endif
