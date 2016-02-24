#include <vector>
class VFATdata 
{
  private:
    uint8_t fb1010;                    // 1010:4 Control bits, shoud be 1010
    uint16_t fBC;                      // Bunch Crossing number, 12 bits
    uint8_t fb1100;                    // 1100:4, Control bits, shoud be 1100
    uint8_t fEC;                      // Event Counter, 8 bits
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

    // read first word from the block
    void read_fw(uint64_t word)
    {
      fb1010 = 0x0f & (word >> 60);
      fBC = 0x0fff & (word >> 48);
      fb1100 = 0x0f & (word >> 44);
      fEC = word >> 36;
      fFlag = 0x0f & (word >> 32);
      fb1110 = 0x0f & (word >> 28);
      fChipID = 0x0fff & (word >> 16);
      fmsData = 0xffff000000000000 & (word << 48);
    }
    
    // read second word from the block
    void read_sw(uint64_t word)
    {
      fmsData = fmsData & (word >> 16);
      flsData = 0xffff000000000000 & (word << 48);
    }
    
    // read third word from the block
    void read_tw(uint64_t word)
    {
      flsData = flsData & (word >> 16);
      fcrc = word;
    }
    
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

class GEBdata
{
  private:
    std::vector<VFATdata> vfatd;

    //GEM chamber header

      uint32_t m_ZeroSup;  //Zero Suppression Flags:24  (8 zeroes):8
                           //Bitmask indicating if certain VFAT blocks have been zero suppressed
      uint8_t m_InputID;   //Input ID:5                 000:3
                           //GLIB input ID (starting at 0)
      uint16_t m_Vwh;      //VFAT word count:12         0000:4
                           //Size of VFAT payload in 64 bit words
      uint16_t m_ErrorC;
                           //EvtFIFO full:1       InFIFO full:1      L1AFIFO full:1    Even size overflow:1  EvtFIFO near full:1  InFIFO near full:1 
                           //L1AFIFO near full:1  Event size warn:1  No VFAT marker:1  OOS GLIB VFAT:1       OOS GLIB OH:1        
                           //BX mismatch GLIB VFAT:1                 BX mismatch GLIB OH:1                   
                           //000:3

    //GEM chamber trailer
		
      uint16_t m_OHCRC;    //OH CRC:16
                           //CRC of OH data (currently not available)
      uint16_t m_Vwt;      //VFAT word count:12   0000:4
                           //Same as in header. This one actually counts the number of valid words that were sent to AMC13; the one in header is 
                           //what we expected to send to AMC13
      uint8_t m_InFu;      //InFIFO underflow:1   (7 0's):7
                           //Input status (critical): Input FIFO underflow occured while sending this event
      uint8_t m_Stuckd;    //Stuck data:1    (7 0's):7
                           //Input status (warning): data in InFIFO or EvtFIFO when L1A FIFO was empty. Only resets with resync or reset
		
  public:
    GEBdata(){};
    GEBdata(const uint32_t &ZeroSup_, 
              const uint8_t &InputID_, 
              const uint16_t &Vwh_, 
              const uint16_t &ErrorC_, 
              const uint16_t &OHCRC_, 
              const uint16_t &Vwt_,
              const uint8_t &InFu_,
              const uint8_t &Stuckd_) : 
          m_ZeroSup(ZeroSup_),                                
          m_InputID(InputID_),
          m_Vwh(Vwh_),                                  
          m_ErrorC(ErrorC_),
          m_OHCRC(OHCRC_),                                    
          m_Vwt(Vwt_),                             
          m_InFu(InFu_),                                   
          m_Stuckd(Stuckd_){}         
    ~GEBdata(){vfatd.clear();}

    // need to include all the flags
    void setChamberHeader(uint64_t word)
    {
      m_ZeroSup = 0x00ffffff & (word >> 40);
      m_InputID = 0b00011111 & (word >> 35);
      m_Vwh = 0x0fff & (word >> 23);
    }

    // need to include all the flags
    void setChamberTrailer(uint64_t word)
    {
      m_OHCRC = word >> 48;
      m_Vwt = 0x0fff & (word >> 36);
    }

    uint32_t ZeroSup()  {return m_ZeroSup;}
    uint8_t  InputID()  {return m_InputID;}
    uint16_t Vwh()      {return m_Vwh;}
    uint16_t ErrorC()   {return m_ErrorC;}

    uint16_t OHCRC()    {return m_OHCRC;}
    uint16_t Vwt()      {return m_Vwt;}
    uint8_t  InFu()     {return m_InFu;}
    uint8_t  Stuckd()   {return m_Stuckd;}


    void v_add(VFATdata v){vfatd.push_back(v);}
    std::vector<VFATdata> vfats(){return vfatd;}
};

class AMCdata
{
  private:
    std::vector<GEBdata> gebd;

    //AMC header #1	

      uint8_t  m_AMCnum;      //0000:4   AMC#:4
                              //Slot number of AMC(GLIB/MP7/EC7, etc.)
      uint32_t m_L1A;         //L1A ID:24        (8 0's):8
                              //basically like event number, but reset by resync
      uint16_t m_BX;          //BX ID:12         0000:4
                              //Bunch crossing ID
      uint32_t m_Dlength;     //Data length:20   (12 0's):12
                              //Overall size of this FED event fragment in 64bit words (including headers and trailers)

    //AMC header #2
      uint8_t m_FV;           //Format Version:4    0000:4
      uint8_t m_Rtype;        //Run Type:4          0000:4
                              //current version = 0x0;  Could be used to encode run types like physics, cosmics, threshold scan, etc.
      uint8_t m_Param1;       //Run param1:8 
      uint8_t m_Param2;       //Run param2:8
      uint8_t m_Param3;       //Run param3:8 
      uint16_t m_Onum;        //Orbit number:16 
      uint16_t m_BID;         //Board ID:16
                              //This is currently filled with 8bit long GLIB serial number

    //GEM event header
      uint32_t m_GEMDAV;   //GEM DAV list:24    (8 zeroes):8
                           //Bitmask indicating which inputs/chambers have data
      uint64_t m_Bstatus;  // Buffer Status:34  (30 zeroes):30
                           //Bitmask indicating buffer error in given inputs
      uint8_t  m_GDcount;  //GEM DAV count:5    000:3
                           //Number of chamber blocks
      uint8_t  m_Tstate;   //TTS state:4        0000:4
                           //Debug: GLIB TTS state at the moment when this event was built


    //GEM event trailer
      uint32_t m_ChamT;    //Chamber timeout:24   (8 0's):8
                           //Bitmask indicating if GLIB did not recieve data from particular input for this L1A in X amount of GTX clock cycles
      uint8_t  m_OOSG;     //OOS GLIB:1    (7 0's):7
                           //GLIB is out-of-sync (critical): L1A ID is different for different chambers in this event.

    //AMC_trailer
      uint32_t m_CRC;
      uint8_t m_L1AT;
      uint32_t m_DlengthT;
	
  public:
    AMCdata(){};
    AMCdata(const uint8_t &AMCnum_, 
              const uint32_t &L1A_,
              const uint16_t &BX_, 
              const uint32_t &Dlength_,
              const uint8_t &FV_,
              const uint8_t &Rtype_, 
              const uint8_t &Param1_, 
              const uint8_t &Param2_, 
              const uint8_t &Param3_, 
              const uint16_t &Onum_, 
              const uint16_t &BID_,
              const uint32_t &GEMDAV_, 
              const uint64_t &Bstatus_,
              const uint8_t &GDcount_, 
              const uint8_t &Tstate_,
              const uint32_t &ChamT_,
              const uint8_t OOSG_) :
          m_AMCnum(AMCnum_), 
          m_L1A(L1A_),
          m_BX(BX_),                                 
          m_Dlength(Dlength_),
          m_FV(FV_),
          m_Rtype(Rtype_),                                
          m_Param1(Param1_),
          m_Param2(Param2_),                                  
          m_Param3(Param3_),
          m_Onum(Onum_),                                    
          m_BID(BID_),
          m_GEMDAV(GEMDAV_), 
          m_Bstatus(Bstatus_),
          m_GDcount(GDcount_),                                 
          m_Tstate(Tstate_),                             
          m_ChamT(ChamT_),                                   
          m_OOSG(OOSG_){}
    ~AMCdata(){gebd.clear();}

    void setAMCheader1(uint64_t word)
    {
      m_AMCnum = 0x0f & (word >> 56);
      m_L1A = 0x00ffffff & (word >> 32);
      m_BX = 0x0fff & (word >> 20);
      m_Dlength = 0x000fffff & word;
    }

    void setAMCheader2(uint64_t word)
    {
      m_FV = 0x0f & (word >> 60);
      m_Rtype = 0x0f & (word >> 56);
      m_Param1 = word >> 48;
      m_Param2 = word >> 40;
      m_Param3 = word >> 32;
      m_Onum = word >> 16;
      m_BID = word;
    }

    void setGEMeventHeader(uint64_t word)
    {
      m_GEMDAV = 0x00ffffff & (word >> 40);
      m_Bstatus = 0x00ffffff & (word >> 16);
      m_GDcount = 0b00011111 & (word >> 11);
      m_Tstate = 0b00000111 & word;
    }

    void setGEMeventTrailer(uint64_t word)
    {
      m_ChamT = 0x00ffffff & (word >> 40);
      m_OOSG = 0b00000001 & (word >> 39);
    }

    void setAMCTrailer(uint64_t word)
    {
      m_CRC = word >> 32;
      m_L1AT = word >> 24;
      m_DlengthT = 0x000fffff & word;
    }

    uint8_t  AMCnum()  {return m_AMCnum;}
    uint32_t L1A()     {return m_L1A;}
    uint16_t BX()      {return m_BX;}
    uint32_t Dlength() {return m_Dlength;}

    uint8_t  FV()      {return m_FV;}
    uint8_t  Rtype()   {return m_Rtype;}
    uint8_t  Param1()  {return m_Param1;}
    uint8_t  Param2()  {return m_Param2;}
    uint8_t  Param3()  {return m_Param3;}
    uint16_t Onum()    {return m_Onum;}
    uint16_t BID()     {return m_BID;}

    uint32_t GEMDAV ()  {return m_GEMDAV;}
    uint64_t Bstatus()  {return m_Bstatus;}
    int  GDcount()  {return unsigned(m_GDcount);}
    uint8_t  Tstate()   {return m_Tstate;}

    uint32_t ChamT()    {return m_ChamT;}
    uint8_t  OOSG()     {return m_OOSG;}    

    uint32_t CRC()    {return m_CRC;}
    uint8_t L1AT()    {return m_L1AT;}
    uint32_t DlengthT()    {return m_DlengthT;}

    void g_add(GEBdata g){gebd.push_back(g);}
    std::vector<GEBdata> gebs(){return gebd;}
};

class AMC13Event
{
  private:
    // CDF Header
    uint8_t m_cb5; // control bit, should be 0x5 bits 60-63
    uint8_t m_Evt_ty;
    uint32_t m_LV1_id;
    uint16_t m_BX_id;
    uint16_t m_Source_id;
    // AMC13 header
    uint8_t m_CalTyp;
    uint8_t m_nAMC;
    uint32_t m_OrN;
    uint8_t m_cb0; // control bit, should be 0b0000
    // AMC headers
    std::vector<uint32_t> m_AMC_size;
    std::vector<uint8_t> m_Blk_No;
    std::vector<uint8_t> m_AMC_No;
    std::vector<uint16_t> m_BoardID;
    // AMCs payload
    std::vector<AMCdata> m_amcs;
    //AMC13 trailer
    uint32_t m_CRC_amc13;
    uint8_t m_Blk_NoT;
    uint8_t m_LV1_idT;
    uint16_t m_BX_idT;
    //CDF trailer
    uint8_t m_cbA; // control bit, should be 0xA bits 60-63
    uint32_t m_EvtLength;
    uint16_t m_CRC_cdf;

  public:

    AMC13Event(){}
    ~AMC13Event(){m_AMC_size.clear(); m_Blk_No.clear(); m_AMC_No.clear(); m_BoardID.clear(); m_amcs.clear();}

    int nAMC(){return unsigned(m_nAMC);}
    int LV1_id(){return unsigned(m_LV1_id);}
    std::vector<AMCdata> amcs(){return m_amcs;}
    //*** Set the CDF header. Not full header implemented yet. Doc:http://ohm.bu.edu/~hazen/CMS/AMC13/AMC13DataFormatDrawingv3.pdf
    void setCDFHeader(uint64_t word)
    {
      m_cb5 = 0x0f & (word >> 60);
      m_Evt_ty = 0x0f & (word >> 56);
      m_LV1_id = 0x00ffffff & (word >> 32);
      m_BX_id = 0x0fff & (word >> 20);
      m_Source_id = 0x0fff & (word >> 8);
    }
    //*** Set the AMC13 header
    void setAMC13header(uint64_t word)
    {
      m_CalTyp = 0x0f & (word >> 56);
      m_nAMC = 0x0f & (word >> 52);
      m_OrN = word >> 4;
      m_cb0 = 0x0f & word;
    }
    //
    void addAMCheader(uint64_t word)
    {
      m_AMC_size.push_back(0x00ffffff&(word>>32));
      m_Blk_No.push_back(0xff&(word>>20));
      m_AMC_No.push_back(0x0f&(word>>16));
      m_BoardID.push_back(0xffff&word);
    }
    //
    void addAMCpayload(AMCdata a){m_amcs.push_back(a);}
    //
    void setAMC13trailer(uint64_t word)
    {
      m_CRC_amc13 = word >> 32;
      m_Blk_NoT = 0xff & (word >> 20);
      m_LV1_idT = 0xff & (word >> 12);
      m_BX_idT = 0x0fff & word;
    }
    void setCDFTrailer(uint64_t word)
    {
      m_cbA = 0x0f & (word >> 60);
      m_EvtLength = 0x00ffffff & (word >> 32);
      m_CRC_cdf = 0xffff & (word >> 16);
    }

};

