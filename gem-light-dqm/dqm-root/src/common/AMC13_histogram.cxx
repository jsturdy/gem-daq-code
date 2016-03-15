#include "AMC_histogram.cxx"
#include "TH1.h"

class AMC13_histogram: public Hardware_histogram
{
  public:
    AMC13_histogram(const std::string & filename, TDirectory * dir):Hardware_histogram(filename, dir){}//call base constructor
    void bookHistograms()
    {
      m_dir->cd();
      // temp list of histograms, needs update
      ZeroSup  = new TH1F("ZeroSup", "Zero Suppression", 0xffffff,  0x0 , 0xffffff);
      InputID  = new TH1F("InputID", "GLIB input ID", 31,  0x0 , 0b11111);      
      Vwh      = new TH1F("Vwh", "VFAT word count", 4095,  0x0 , 0xfff);
      ErrorC   = new TH1F("ErrorC", "Thirteen Flags", 0b1111111111111111,  0x0 , 0b1111111111111111);
      OHCRC    = new TH1F("OHCRC", "OH CRC", 0xffff,  0x0 , 0xffff);
      Vwt      = new TH1F("Vwt", "VFAT word count", 4095,  0x0 , 0xfff);
      InFu     = new TH1F("InFu", "InFIFO underflow flag", 15,  0x0 , 0xf);
      Stuckd   = new TH1F("Stuckd", "Stuck data flag", 1,  0x0 , 0b1);
    }
    void addAMCH(AMC_histogram amcH){m_amcsH.push_back(amcH);}
    std::vector<AMC_histogram> amcsH(){return m_amcsH;}
  private:
    std::vector<AMC_histogram> m_amcsH;
    // temp list of histograms, needs update
    TH1F* ZeroSup;
    TH1F* InputID;
    TH1F* Vwh;
    TH1F* ErrorC;
    TH1F* OHCRC;
    TH1F* Vwt;
    TH1F* InFu;
    TH1F* Stuckd;
};
