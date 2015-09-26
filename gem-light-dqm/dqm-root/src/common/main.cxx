#include "gemtreewriter/src/common/gemTreeWriter.cxx"
#include "dqm-root/src/common/treeReader.cxx"

using namespace std;
int main(int argc, char** argv)
{
  if (argc<2) 
  {
      cout << "Please provide input filenames" << endl;
      cout << "Usage: <path>/rundqm inputFile.dat" << endl;
      return 0;
  }
  string m_dat_filename = argv[1];
  gemTreeWriter *m_gemTreeWriter = new gemTreeWriter();
  m_gemTreeWriter->makeTree(m_dat_filename);
  string m_raw_filename;
  m_raw_filename = m_gemTreeWriter->getOutputFileName();
  gemTreeReader *m_gemTreeReader = new gemTreeReader(m_raw_filename);
  m_gemTreeReader->createHistograms();

  return 0;
}
