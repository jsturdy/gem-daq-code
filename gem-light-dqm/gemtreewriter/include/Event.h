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

//class VFATdata : public TObject {
class VFATdata {
    private:
        uint8_t fb1010;                    // 1010:4 Control bits, shoud be 1010
        uint16_t fBC;                      // Bunch Crossing number, 12 bits
        uint8_t fb1100;                    // 1100:4, Control bits, shoud be 1100
        uint16_t fEC;                      // Event Counter, 8 bits
        uint8_t fFlag;                     // Control Flags: 4 bits, Hamming Error/AFULL/SEUlogic/SUEI2C
        uint8_t fb1110;                    // 1110:4 Control bits, shoud be 1110
        uint16_t fChipID;                  // Chip ID, 12 bits
        uint64_t flsData;                  // channels from 1to64 
        uint64_t fmsData;                  // channels from 65to128
        uint16_t fcrc;                     // Check Sum value, 16 bits
        uint16_t fcrc_calc;                // Check Sum value recalculated, 16 bits
        int fSlotNumber;                   // Calculated chip position
        bool fisBlockGood;                 // Shows if block is good (control bits, chip ID and CRC checks)

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
                const uint16_t &crc_,
                const uint16_t &crc_calc_,
                const int &SlotNumber_,
                const bool &isBlockGood_) : 
            fb1010(b1010_),
            fBC(BC_),
            fb1100(b1100_),
            fEC(EC_),
            fFlag(Flag_),
            fb1110(b1110_),
            fChipID(ChipID_),
            flsData(lsData_),
            fmsData(msData_),
            fcrc(crc_),
            fcrc_calc(crc_calc_),
            fSlotNumber(SlotNumber_),
            fisBlockGood(isBlockGood_){}
        ~VFATdata(){}

        uint8_t   b1010      (){ return fb1010;      }
        uint16_t  BC         (){ return fBC;         }
        uint8_t   b1100      (){ return fb1100;      }
        uint8_t   EC         (){ return fEC;         }
        uint8_t   Flag       (){ return fFlag;       }
        uint8_t   b1110      (){ return fb1110;      }
        uint16_t  ChipID     (){ return fChipID;     }
        uint64_t  lsData     (){ return flsData;     }
        uint64_t  msData     (){ return fmsData;     }
        uint16_t  crc        (){ return fcrc;        }
        uint16_t  crc_calc   (){ return fcrc_calc;   }
        int       SlotNumber (){ return fSlotNumber; }
        bool      isBlockGood(){ return fisBlockGood;}

};

//class GEBdata : public TObject {
class GEBdata {
    private:
        uint32_t fZSFlag;                // ZeroSuppresion flags, 24 bits
        uint16_t fChamID;                // Chamber ID, 12 bits
        uint32_t fsumVFAT;               // Rest part of the header, reserved for the moment
        uint64_t fRunHeader;             // RunType:4 VT1:8 VT2:8 minTH:8 maxTH:8 Step:8 - Threshold Scan Header
        // 0 data, 1 local-data, 2 cosmic-data, 3 threshold scan, 4 S-curve, 5 latency
                                         // – Run Control header will be filled in according applications.
        //uint8_t fRunType;              // Run Type (TS, LS, Cosmic or Collision)
        //uint8_t fVT1;
        //uint8_t fVT2;
        //uint8_t fMinThreshold;
        //uint8_t fMaxThreshold;
        //uint8_t fStep;
        /*
         * Need to add the header instances for other types of run
         */
        std::vector<VFATdata> fvfats;
        uint16_t fOHcrc;                 // OH Check Sum, 16 bits
        uint16_t fOHwCount;              // OH Counter, 16 bits
        uint16_t fChamStatus;            // Chamber Status, 16 bits
        uint16_t fGEBres;                // Reserved part of trailer

    public:
        GEBdata(){}
        GEBdata(const uint32_t &ZSFlag_, const uint16_t &ChamID_, const uint32_t &sumVFAT_) : 
            fZSFlag(ZSFlag_),
            fChamID(ChamID_),
            fsumVFAT(sumVFAT_){}
        ~GEBdata(){}
        void setRunHeader(const uint64_t &runHeader_){fRunHeader = runHeader_;}
        void addVFATData(const VFATdata &vfat_){fvfats.push_back(vfat_);}
        void setTrailer(const uint16_t &OHcrc_, const uint16_t &OHwCount_, const uint16_t &ChamStatus_, const uint16_t &GEBres_)
        {fOHcrc = OHcrc_; fOHwCount = OHwCount_; fChamStatus = ChamStatus_; fGEBres = GEBres_;}

        uint32_t ZSFlag    (){ return fZSFlag;     }       
        uint16_t ChamID    (){ return fChamID;     }       
        uint32_t sumVFAT   (){ return fsumVFAT;    }       
        uint32_t runHeader (){ return fRunHeader;  }       
        uint16_t OHcrc     (){ return fOHcrc;      }       
        uint16_t OHwCount  (){ return fOHwCount;   }       
        uint16_t ChamStatus(){ return fChamStatus; }       
        uint16_t GEBres    (){ return fGEBres;     }       
        std::vector<VFATdata> vfats(){ return fvfats;}

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
        uint8_t fAmcNo;
        uint8_t fb0000;
        uint32_t fLV1ID;                    // What is this? Which var format should be used?
        uint32_t fBXID;                     // Is it Bunch crossing ID? Should it be Int_t?
        uint32_t fDataLgth;                 // What is this?
        //uint64_t header2;             // User:32      OrN:16     BoardID:16
        uint16_t fOrN;                   // What is this?
        uint16_t fBoardID;
        //uint64_t header3;               // DAVList:24   BufStat:24 DAVCount:5 FormatVer:3 MP7BordStat:8 
        uint32_t fDAVList;
        uint32_t fBufStat;
        uint8_t fDAVCount;
        uint8_t fFormatVer;
        uint8_t fMP7BordStat;

        std::vector<GEBdata> fgebs;      // Should we use vector or better have TClonesArray here?
        //uint64_t trailer2;            // EventStat:32 GEBerrFlag:24  
        uint32_t fEventStat;
        uint32_t fGEBerrFlag;
        //uint64_t trailer1;            // crc:32       LV1IDT:8   0000:4     DataLgth:20 
        uint32_t fcrc;
        uint8_t fLV1IDT;
        uint8_t fb0000T;
        uint32_t fDataLgthT;

        bool fisEventGood;

    public:
        Event();
        virtual ~Event();
        void SetHeader(Int_t i, Int_t run, Int_t date);
        int GetEventNumber(){return fEvtHdr.GetEvtNum();}
        void Build(const uint8_t &AmcNo_, 
            const uint8_t &b0000_,
            const uint32_t &LV1ID_, 
            const uint32_t &BXID_, 
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
            const uint32_t &DataLgthT_,
            bool isEventGood_);
        void addGEBdata(const GEBdata &geb){fgebs.push_back(geb);}
        void Clear();

        uint8_t  AmcNo      (){ return fAmcNo;      }
        uint8_t  b0000      (){ return fb0000;      }
        uint32_t LV1ID      (){ return fLV1ID;      }       
        uint32_t BXID       (){ return fBXID;       }        
        uint32_t DataLgth   (){ return fDataLgth;   }     
        uint16_t OrN        (){ return fOrN;        }    
        uint16_t BoardID    (){ return fBoardID;    }
        uint32_t DAVList    (){ return fDAVList;    }
        uint32_t BufStat    (){ return fBufStat;    }
        uint8_t  DAVCount   (){ return fDAVCount;   }
        uint8_t  FormatVer  (){ return fFormatVer;  }
        uint8_t  MP7BordStat(){ return fMP7BordStat;}

        std::vector<GEBdata> gebs(){ return fgebs;}

        uint32_t EventStat  (){ return fEventStat;  }
        uint32_t GEBerrFlag (){ return fGEBerrFlag; }
        uint32_t crc        (){ return fcrc;        }
        uint8_t  LV1IDT     (){ return fLV1IDT;     }
        uint8_t  b0000T     (){ return fb0000T;     }
        uint32_t DataLgthT  (){ return fDataLgthT;  }
        bool     isEventGood(){ return fisEventGood;}

        ClassDef(Event,1)               //Event structure
};

#endif
