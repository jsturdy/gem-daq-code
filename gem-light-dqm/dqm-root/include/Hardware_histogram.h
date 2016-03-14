#include "TDirectory.h"
#include "TFile.h"

class Hardware_histogram
{
  public:
    Hardware_histogram(const std::string & filename, TDirectory * dir){
      //m_file = new TFile(filename.c_str(), "UPDATE");
      m_dir = dir;
    }
    virtual ~Hardware_histogram(){}//m_file->Close();}
    void bookHistograms(){}
    TDirectory * getTDir(){return m_dir;}

  protected:
    TDirectory *m_dir;
    TFile *m_file;
};
