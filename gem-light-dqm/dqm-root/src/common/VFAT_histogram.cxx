#include "Hardware_histogram.h"
#include "TH1.h"
#include <Event.h>

class VFAT_histogram: public Hardware_histogram
{
  public:
    VFAT_histogram(const std::string & filename, TDirectory * dir, const std::string & hwid):Hardware_histogram(filename, dir, hwid){}//call base constructor
    VFAT_histogram(VFAT_histogram * vH):Hardware_histogram("dummy", vH->m_dir, vH->m_HWID){}//call base constructor
    ~VFAT_histogram(){}
    void bookHistograms(){
      m_dir->cd();
      b1010    = new TH1F("b1010", "Control Bits", 15,  0x0 , 0xf);
      BC       = new TH1F("BC", "Bunch Crossing Number", 4095,  0x0 , 0xfff);
      b1100    = new TH1F("b1100", "Control Bits", 15,  0x0 , 0xf);
      EC       = new TH1F("EC", "Event Counter", 255,  0x0 , 0xff);
      Flag     = new TH1F("Flag", "Control Flags", 15,  0x0 , 0xf);
      b1110    = new TH1F("b1110", "Control Bits", 15,  0x0 , 0xf);
      ChipID   = new TH1F("ChipID", "Chip ID", 4095,  0x0 , 0xfff);
      FiredChannels   = new TH1F("FiredChannels", "FiredChannels", 128,  0, 128);
      crc      = new TH1F("crc", "check sum value", 0xffff,  0x0 , 0xffff);
      crc_calc = new TH1F("crc_calc", "check sum value recalculated", 0xffff,  0x0 , 0xffff);
    }
    void fillHistograms(VFATdata * vfat){
      b1010->Fill(vfat->b1010());
      b1100->Fill(vfat->b1100());
      b1110->Fill(vfat->b1110());
      BC->Fill(vfat->BC());
      EC->Fill(vfat->EC());
      Flag->Fill(vfat->Flag());
      ChipID->Fill(vfat->ChipID());
      uint16_t chan0xf = 0;
      for (int chan = 0; chan < 128; ++chan) {
        if (chan < 64){
          chan0xf = ((vfat->lsData() >> chan) & 0x1);
          if(chan0xf) FiredChannels->Fill(chan);
        } else {
          chan0xf = ((vfat->msData() >> (chan-64)) & 0x1);
          if(chan0xf) FiredChannels->Fill(chan);
        }
      }
    }
  private:
    TH1F* b1010;
    TH1F* BC;
    TH1F* b1100;
    TH1F* EC;
    TH1F* Flag;
    TH1F* b1110;
    TH1F* ChipID;
    TH1F* FiredChannels;
    TH1F* crc;
    TH1F* crc_calc;
};
