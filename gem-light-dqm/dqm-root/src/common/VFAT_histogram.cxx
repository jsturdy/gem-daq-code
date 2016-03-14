#include "Hardware_histogram.h"
#include "TH1.h"

class VFAT_histogram: public Hardware_histogram
{
  public:
    VFAT_histogram(const std::string & filename, TDirectory * dir):Hardware_histogram(filename, dir){}//call base constructor
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
      lsData   = new TH1F("lsData", "channels from 1 to 64", 64,  0x0 , 63);
      msData   = new TH1F("msData", "cahnnels from 65 to 128", 64,  0x0 , 63);
      crc      = new TH1F("crc", "check sum value", 0xffff,  0x0 , 0xffff);
      crc_calc = new TH1F("crc_calc", "check sum value recalculated", 0xffff,  0x0 , 0xffff);
    }
  private:
    TH1F* b1010;
    TH1F* BC;
    TH1F* b1100;
    TH1F* EC;
    TH1F* Flag;
    TH1F* b1110;
    TH1F* ChipID;
    TH1F* lsData;
    TH1F* msData;
    TH1F* crc;
    TH1F* crc_calc;
};
