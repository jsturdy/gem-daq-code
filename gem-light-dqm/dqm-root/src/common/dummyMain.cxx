//#include "gemtreewriter/src/common/gemTreeWriter.cxx"
//#include "dqm-root/src/common/treeReader.cxx"
#include "dqm-root/src/common/gemTreeReader.cxx"
#include "dqm-root/src/common/GEMClusterization/GEMStrip.cc"
#include "dqm-root/src/common/GEMClusterization/GEMCluster.cc"
#include "dqm-root/src/common/GEMClusterization/GEMClusterizer.cc"
#include "TApplication.h"

using namespace std;
int main(int argc, char** argv)
{

  if (argc<2) 
    {
      cout << "Please provide input filenames" << endl;
      cout << "Usage: <path>/rundqm inputFile.root" << endl;
      return 0;
    }
  string m_root_filename = argv[1];
  // string m_slot_file = argv[2];
  bool print_hist = false;
  if (argc > 2){
    string option = argv[2];
    if ((option == "--with-print") || (option == "-p")) {
      print_hist = true;
      std::cout << "[MAIN]: will print histograms" << std::endl;
    }
  }

  // std::cout << "[MAIN]: Slot table : " << m_slot_file << std::endl;
  // gemTreeWriter *m_gemTreeWriter = new gemTreeWriter();
  // m_gemTreeWriter->makeTree(m_dat_filename, m_slot_file);
  // string m_raw_filename;
  // m_raw_filename = m_gemTreeWriter->getOutputFileName();

  string m_raw_filename = m_root_filename;

  // std::cout << "[MAIN]: ROOT TTree is finished" << std::endl;
  // std::cout << "Raw file name " << m_raw_filename << std::endl;

  //gemTreeReader *m_gemTreeReader = new gemTreeReader(m_raw_filename, print_hist);
  //m_gemTreeReader->bookHistograms();

  Hardware_histogram *hw_histogram = new Hardware_histogram(m_raw_filename);
 
  std::cout << "[MAIN]: DQM analysis done" << std::endl;

  return 0;


}
