#include <cstdio>
#include <iostream>
#include <cstdint>
#include <vector>
#include <array>
#include <bitset>
#include "GEMAMC13EventFormat.h"

class GEMUnpacker
{
  public:
    GEMUnpacker(const std::string & ifilename)
    {
      try {
        m_file = std::fopen(ifilename.c_str(), "rb");
      }
      catch (int e)
      {
        std::cout << "An exception occured. Exception code " << e << std::endl;
      }
    }
    ~GEMUnpacker()
    {
      if (m_file != NULL) std::fclose(m_file);
    }
    void unpack()
    {
      while (true){
        std::size_t sz = std::fread(&m_word, sizeof(uint64_t), 1, m_file);
        if (sz == 0 ) break;
        // read and print "BADC0FFEEBADCAFE" and another artificial header
        printf("%016llX\n", m_word);
        std::fread(&m_word, sizeof(uint64_t), 1, m_file);
        printf("%016llX\n", m_word);
        // skip the rest event content for the moment
        for (int i = 0; i<20; i++){
          std::fread(&m_word, sizeof(uint64_t), 1, m_file);
        }
      }
    }
  private:
    std::FILE *m_file;
    uint64_t m_word;
    bool type;
    AMC13Event * m_AMC13Event;
};
 
int main (int argc, char** argv)
{
  std::cout << "---> Main()" << std::endl;
  if (argc<2) 
  {
    std::cout << "Please provide input filename and conversion type" << std::endl;
      //cout << "Usage: <path>/gtc inputFile.dat outputFile.root slot_config.csv" << endl;
      return 0;
  }
  std::string ifile   = argv[1];

  GEMUnpacker * m_unpacker = new GEMUnpacker(ifile);
  m_unpacker->unpack();
  delete m_unpacker;
  // read the same data and print it to the standard output
  //if(std::FILE *f2 = std::fopen("test.out", "rb")) {
//  std::FILE *f2 = std::fopen("test.out", "rb");
//  int c; // note: int, not char, required to handle EOF
//  while (true) { // standard C I/O file reading loop
//    uint64_t head[2];
//    std::size_t sz = std::fread(&head[0], sizeof(uint64_t), 2, f2);
//    if (sz == 0 ) break;
//    for(std::size_t n = 0; n < sz; ++n) {
//        //std::printf("%d\n", head[n]);
//      //std::cout << std::hex << head[n] << std::endl;
//      printf("%016llX\n", head[n]);
//    }
//    //uint64_t pEvt[head[1]];
//    ////std::cout << "Event size " << head[1]<< std::endl;
//    //sz = std::fread(&pEvt[0], sizeof(uint64_t), head[1], f2);
//    //std::cout << "Event " << std::dec << i << " size " << sz << std::endl;
//    ////std::cout << "sz " << sz << std::endl;
//    //for(std::size_t n = 0; n < sz; ++n) {
//    //    //std::printf("%d\n", head[n]);
//    //  //std::cout << std::hex << pEvt[n] << std::endl;
//    //  printf("%016llX\n", pEvt[n]);
//    //}
//    //
//    //***Read the CDF and AMC13 headers, print some extracted info for debug
//    uint64_t cdf_header;
//    uint64_t amc13_h1;
//    std::fread(&cdf_header, sizeof(uint64_t), 1, f2);
//    std::fread(&amc13_h1, sizeof(uint64_t), 1, f2);
//    printf("%016llX\n", cdf_header);
//    printf("%016llX\n", amc13_h1);
//    uint32_t l1 = 0x00ffffff & (cdf_header >> 32); // L1AID
//    uint16_t bx = 0x0fff & (cdf_header >> 20);
//    uint8_t nAMC = 0x0f & (amc13_h1 >> 52);
//    uint32_t orn = 0xffffffff & (amc13_h1 >> 4);
//    std::cout << "L1AID " << std::hex << l1 << " BX_ID " << std::hex << bx << " Orbit Number " << orn << std::endl;
//    std::cout << "Number of AMCs (GLIBs) " << unsigned(nAMC) << std::endl;
//    uint64_t amc13_h2[nAMC];
//    uint8_t sns[nAMC];
//    uint16_t boardID[nAMC];
//    uint32_t amcSize[nAMC];
//    std::fread(&amc13_h2[0], sizeof(uint64_t), nAMC, f2);
//    for (short i = 0; i < nAMC; i++){
//      printf("%016llX\n", amc13_h2[i]);
//      sns[i] = 0x0f & (amc13_h2[i] >> 16);
//      boardID[i] = amc13_h2[i];
//      amcSize[i] = 0x00ffffff & (amc13_h2[i] >> 32);
//      std::cout << "Board " << std::hex << boardID[i] << " in slot " << std::dec << unsigned(sns[i]) << " with size " << std::dec << amcSize[i]<< std::endl;
//    }
//  
//    for (short i = 0; i < nAMC; i++){
//      //***Read the AMC(GLIB) header 1, print some extracted info for debug
//      uint64_t amc_h1;
//      uint64_t amc_h2;
//      std::fread(&amc_h1, sizeof(uint64_t), 1, f2);
//      printf("%016llX\n", amc_h1);
//      uint8_t sna = 0x0f & (amc_h1 >> 56); // slot number reported by AMC
//      uint32_t l1a = 0x00ffffff & (amc_h1 >> 32); // LV1ID from amc
//      uint16_t bxa = 0x0fff & (amc_h1 >> 20); // BX ID from amc
//      uint32_t dla = 0x000fffff & amc_h1; // Data Length from amc. Curently FFFF
//      std::cout << "AMC"<<std::dec<<unsigned(sna)<< " reported: " << std::endl;
//      std::cout << "        "<<"LV1ID       : " << std::hex<< l1a << std::endl;
//      std::cout << "        "<<"BX ID       : " << std::hex<< bxa << std::endl;
//      std::cout << "        "<<"Data Length : " << std::dec<< dla << std::endl;
//  
//      //***Read the AMC(GLIB) header 1, print some extracted info for debug
//      std::fread(&amc_h2, sizeof(uint64_t), 1, f2);
//      printf("%016llX\n", amc_h2);
//      uint8_t fwa = 0x0f & (amc_h2 >> 60); // format version reported by AMC
//      uint8_t rta = 0x0f & (amc_h2 >> 56); // run type reported by AMC
//      uint16_t ona = 0xffff & (amc_h2 >> 16); // orbit number from amc
//      uint16_t bid = 0xffff & (amc_h2 ); // board id from amc
//      std::cout << "        "<<"Format ver. : " << std::hex<<unsigned(fwa) << std::endl;
//      std::cout << "        "<<"Run Type    : " << std::dec<<unsigned(rta) << std::endl;
//      std::cout << "        "<<"Orbit Num.  : " << std::hex<< ona << std::endl;
//      std::cout << "        "<<"Board ID    : " << std::hex<< bid << std::endl;
//  
//      //***Read the GEM event header, print some extracted info for debug
//      uint64_t gem_header;
//      std::fread(&gem_header, sizeof(uint64_t), 1, f2);
//      printf("%016llX\n", gem_header);
//      uint32_t davl = 0x00ffffff & (gem_header >> 40); // GEB DAV List
//      uint32_t bufs = 0x00ffffff & (gem_header >> 16); // AMC Buffers Status
//      uint8_t davc = 0b00011111 & (gem_header >> 11);  // GEB DAV Count
//      std::cout << "        "<<"DAV List    : " << std::bitset<24>(davl) << std::endl;
//      std::cout << "        "<<"Buffer Err  : " << std::bitset<24>(bufs) << std::endl;
//      std::cout << "        "<<"DAV count   : " << std::dec << unsigned(davc) << std::endl;
//      for (unsigned short c = 0; c < unsigned(davc); c++){
//        //***Read the GEM chamber header, print some extracted info for debug
//        uint64_t geb_header;
//        std::fread(&geb_header, sizeof(uint64_t), 1, f2);
//        printf("%016llX\n", geb_header);
//        uint8_t iid = 0b00011111 & (geb_header >> 35);  // AMC input id
//        uint16_t vwc = 0x0fff & (geb_header >> 23); // VFAT word count
//        std::cout << "        "<<"AMC input id: " << std::dec << unsigned(iid) << std::endl;
//        std::cout << "        "<<"VFAT word # : " << std::dec << vwc << std::endl;
//        //for (short v = 0; v < vwc; v++){ Currently wvc is wrong
//        for (short v = 0; v < 3; v++){
//          //***Read the vfat words, print some extracted info for debug
//          uint64_t vfat_word;
//          std::fread(&vfat_word, sizeof(uint64_t), 1, f2);
//          printf("%016llX\n", vfat_word);
//        } // endfor vfat payload
//
//        //***Read the GEM chamber trailer, print some extracted info for debug
//        uint64_t geb_trailer;
//        std::fread(&geb_trailer, sizeof(uint64_t), 1, f2);
//        printf("%016llX\n", geb_trailer);
//       } // endfor amc payload
//      //
//      //***Read the GEM event trailer, print some extracted info for debug
//      uint64_t gem_trailer;
//      std::fread(&gem_trailer, sizeof(uint64_t), 1, f2);
//      printf("%016llX\n", gem_trailer);
//
//      //***Read the AMC trailer, print some extracted info for debug
//      uint64_t amc_t;
//      std::fread(&amc_t, sizeof(uint64_t), 1, f2);
//      printf("%016llX\n", amc_t);
//    }
//    //***Read the CDF and AMC13 headers, print some extracted info for debug
//    uint64_t amc13_t;
//    uint64_t cdf_trailer;
//    std::fread(&amc13_t, sizeof(uint64_t), 1, f2);
//    std::fread(&cdf_trailer, sizeof(uint64_t), 1, f2);
//    printf("%016llX\n", amc13_t);
//    printf("%016llX\n", cdf_header);
//
//  }
//  std::fclose(f2);
}
