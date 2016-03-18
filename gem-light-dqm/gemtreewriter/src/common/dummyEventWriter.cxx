#define DEBUG 0

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <cstdint>
#include <bitset>
#include <memory>

#if defined(__CINT__) && !defined(__MAKECINT__)
#include "libEvent.so"
#else
#include "Event.h"
#endif

#include "TRandom3.h"
#include "gem/readout/GEMDataAMCformat.h"
#include "gem/datachecker/GEMDataChecker.h"

std::ofstream outf;
void WriteVFAT(int ev,uint16_t ChipID,uint16_t sn, uint64_t  lsData, uint64_t  msData);
bool checkBlock(const uint8_t &b1010_, const uint8_t &b1100_, const uint8_t &b1110_, const int &sn_, const uint16_t &crc_, const uint16_t &crc_calc_);

int main(int argc, char** argv)
{
        if (argc<3)
        {
                std::cout << "Please provide input and output filenames" << std::endl;
                std::cout << "Usage: <path>/dummyEventWriter outputFile.dat  slot_config.csv [--n events] [--r] [--f channelsON] [--p patternLS patternMS]" << std::endl;
                std::cout << "General Parameters:  --n NumberOfEvents (default=100) " << std::endl;
                std::cout << "                     --c NumberOfChambers (default=4) " << std::endl;
                std::cout << "Channel filling:     --r --> fill with randoms "<<std::endl; 
                std::cout << "                     --p patternLS pattern MS ---> fill with a pattern fixing the channels on for LS and MS "<<std::endl;
                std::cout << "                                             ---> example '--p 11110000011111   1011111100001' "<<std::endl;
                std::cout << "                                             ---> note reversed order: '1' == 1st channel on, '100' == 1st channel on, then 2 off (to be fixed?)."<<std::endl; 
                std::cout << "                     --f channelsON --> fill X channels. example: '--f  0xffffff' "<<std::endl;   
                std::cout << "(If no options inserted, all channels filled '--f 64')"<<std::endl; 
                return 0;
        }
        const TString ofile = argv[1];
        std::string slot_file = argv[2];
        int events =100;
        uint64_t fillValueLS=0x0, fillValueMS=0x0;
        int channelsON=64;
        bool fillRandom=false;
        bool fillPattern=false;
        int NChambers=4;
        std::string patternLS="0";
        std::string patternMS="0";
        for(int i=3; i<argc; i++){
                if(argv[i]==std::string("--n")) events = std::stoi(argv[i+1]);
                if(argv[i]==std::string("--c")) NChambers = std::stoi(argv[i+1]);
                if(argv[i]==std::string("--f")) channelsON = std::stoi(argv[i+1]);
                if(argv[i]==std::string("--r")) fillRandom =true;
                if(argv[i]==std::string("--p")) {fillPattern=true; patternLS=argv[i+1];  patternMS=argv[i+2];}
        }
        std::cout<<"Running DummyEventWriter - for debugging purposes"<<std::endl;
        std::cout<<"I will create "<<events<<" in "<<NChambers<<" chambers"<<std::endl;
 

        if(fillRandom) std::cout<<"Filling with randoms"<<std::endl;
        else if(fillPattern){
                uint64_t shift=0x1;
                for(int i=0; i<patternMS.size() && i<64; i++) {
                        if(patternMS.at(i)=='1'){
                                fillValueMS += (shift << i);
                        }
                }
                for(int i=0; i<patternLS.size() && i<64; i++) {
                        if(patternLS.at(i)=='1'){
                                fillValueLS += (shift << i);
                        }
                }
                std::cout<<"Filling ls with pattern: "<<patternLS<<"   "<<std::hex<<fillValueLS<<" "<<std::dec<<std::endl;
                std::cout<<"Filling ms with pattern: "<<patternMS<<"   "<<std::hex<<fillValueMS<<" "<<std::dec<<std::endl;
                if(patternMS.size()>64 || patternLS.size() >64) std::cout<<"Warning: I am only using the first 64 bits"<<std::endl;

        }
        else {
                uint64_t shift=0x1;
                for(int i=0; i<channelsON; i++) {
                        fillValueLS += (shift << i);
                }
                fillValueMS=fillValueLS;
                std::cout<<"Filling ls and ms with "<<std::hex<<fillValueMS<<" "<<std::dec<<std::endl;
        }  

        TRandom3 Random;  


        outf.open(ofile);

        for (int event=0; event<events; event++){

                // Header info
                //Most of this is a placeholder (all set to dummy values for now)
                uint64_t ZSFlag = 0;
                uint64_t AmcNo       = BOOST_BINARY( 1 );            // :4 
                uint64_t LV1ID       = (0x0000000000ffffff & event); // :24
                uint64_t BXID        = 0;                            // :12  // Is this the same as the other BX, below? 
                uint64_t DataLgth    = BOOST_BINARY( 1 );            // :20
                uint64_t User        = BOOST_BINARY( 1 );    // :32
                uint64_t OrN         = BOOST_BINARY( 1 );    // :16
                uint64_t BoardID     = BOOST_BINARY( 1 );    // :16
                uint64_t DAVList     = BOOST_BINARY( 1 );    // :24
                uint64_t BufStat     = BOOST_BINARY( 1 );    // :24
                uint64_t DAVCount    = BOOST_BINARY( 1 );    // :5
                uint64_t FormatVer   = BOOST_BINARY( 1 );    // :3
                uint64_t MP7BordStat = BOOST_BINARY( 1 );    // :8

                uint64_t GEMheader1=(AmcNo <<60)|(ZSFlag << 56)|(LV1ID << 32)|(BXID << 20)|(DataLgth);  
                uint64_t GEMheader2=(User << 32)|(OrN << 16)|(BoardID);    
                uint64_t GEMheader3=(BufStat << 40)|(DAVCount << 16)|(DAVCount << 11)|(FormatVer << 8)|(MP7BordStat);

                uint64_t RunType = BOOST_BINARY( 1 ); // depends on the kind of run. ignore for now.
                uint64_t GEBrunheader = (RunType << 60);

                // This one is not dummy! slot configuration
                std::unique_ptr<gem::readout::GEMslotContents> slotInfo_ = std::unique_ptr<gem::readout::GEMslotContents> (new gem::readout::GEMslotContents(slot_file));
                uint32_t sumVFAT=slotInfo_->GEBNumberOfSlots(); // this assumes all the chambers are going to have the same slots filled. 

                for (int chamber=0; chamber<NChambers; chamber++){

                        uint64_t ChamID = 3562-chamber;
                        uint64_t GEBheader=sumVFAT + (ChamID<<28) + (ZSFlag<<40);

                        outf << std::hex << GEMheader1<<std::dec<<std::endl; 
                        outf << std::hex << GEMheader2<<std::dec<<std::endl; 
                        outf << std::hex << GEMheader3<<std::dec<<std::endl;
                        outf << std::hex << GEBheader<<std::dec << std::endl;
                        outf << std::hex << GEBrunheader<<std::dec << std::endl;

                        // Iterate over one vfat
                        for (uint32_t vfat=0; vfat<24; vfat++){
                                uint16_t  chipID = slotInfo_->GEBChipIdFromSlot(vfat);
                                if(chipID==0xfff) continue;
                                if(fillRandom) {
                                        uint64_t fillValuea=(uint64_t)Random.Integer(0xffffffff);
                                        uint64_t fillValueb=(uint64_t)Random.Integer(0xffffffff);
                                        fillValueLS=(fillValuea<<32)|fillValueb;
                                        fillValuea=(uint64_t)Random.Integer(0xffffffff);
                                        fillValueb=(uint64_t)Random.Integer(0xffffffff);
                                        fillValueMS=(fillValuea<<32)|fillValueb;
                                }
                                WriteVFAT(event,chipID,vfat,fillValueLS,fillValueMS);
                        }


                        //   // GEM Event Treailer [2]
                        uint64_t EventStat  = BOOST_BINARY( 1 );    // :32
                        uint64_t GEBerrFlag = BOOST_BINARY( 1 );    // :24

                        uint64_t GEMtrailer2 = ( EventStat << 40)|(GEBerrFlag);

                        // GEM Event Treailer [1]
                        uint64_t crc      = BOOST_BINARY( 1 );    // :32
                        uint64_t LV1IDT   = BOOST_BINARY( 1 );    // :8
                        uint64_t ZeroFlag = BOOST_BINARY( 0000 ); // :4
                        uint64_t DataLgth = BOOST_BINARY( 1 );    // :20

                        uint64_t GEMtrailer1 = (crc<<32)|(LV1IDT << 24)|(ZeroFlag <<20)|(DataLgth);

                        // Chamber Trailer, OptoHybrid: crc, wordcount, Chamber status
                        uint64_t OHcrc       = BOOST_BINARY( 1 ); // :16
                        uint64_t OHwCount    = BOOST_BINARY( 1 ); // :16
                        uint64_t ChamStatus  = BOOST_BINARY( 1 ); // :16

                        uint64_t GEBtrailer = ((OHcrc << 48)|(OHwCount << 32 )|(ChamStatus << 16));

                        // the trailers are just a copy of the test beam trailers. to be checked.
                        outf << std::hex << GEBtrailer << std::dec << std::endl;
                        outf << std::hex << GEMtrailer2 << std::dec << std::endl;
                        outf << std::hex << GEMtrailer1 << std::dec << std::endl;

                }

        }

        outf.close();

        return 0;
}

void WriteVFAT(int ev,uint16_t ChipID,uint16_t sn, uint64_t  lsData, uint64_t  msData){

        uint16_t  b1010  = 10;                // (0xf000 & vfat.BC) >> 12;     (Marking all as good for now) 
        uint16_t  b1100  = 12;                // (0xf000 & vfat.EC) >> 12;      
        uint16_t  Flag   = 0;                 // (0x000f & vfat.EC);            
        uint16_t  b1110  = 14;                // (0xf000 & vfat.ChipID) >> 12;  

        uint16_t  EC     = ev%256;                // (0x0ff0 & vfat.EC) >> 4;       

        uint32_t  BXfrOH = 0x3326a933; // These two are dummy
        uint16_t  BC     = 928; 

        uint16_t dataVFAT[12];
        dataVFAT[11] = (b1010<<12) | BC ;
        dataVFAT[10] = (b1100<<12)|(EC<<4)|Flag;
        dataVFAT[9]  = ChipID|(b1110<<12);
        dataVFAT[8]  = (0xffff000000000000 & msData) >> 48;
        dataVFAT[7]  = (0x0000ffff00000000 & msData) >> 32;
        dataVFAT[6]  = (0x00000000ffff0000 & msData) >> 16;
        dataVFAT[5]  = (0x000000000000ffff & msData);
        dataVFAT[4]  = (0xffff000000000000 & lsData) >> 48;
        dataVFAT[3]  = (0x0000ffff00000000 & lsData) >> 32;
        dataVFAT[2]  = (0x00000000ffff0000 & lsData) >> 16;
        dataVFAT[1]  = (0x000000000000ffff & lsData);

        gem::datachecker::GEMDataChecker *dc = new gem::datachecker::GEMDataChecker();
        uint16_t CRC = dc->checkCRC(dataVFAT, 0);

        outf << std::hex << dataVFAT[11]<< std::dec << std::endl;
        outf << std::hex << dataVFAT[10] << std::dec << std::endl;
        outf << std::hex << dataVFAT[9] << std::dec << std::endl;
        outf << std::hex << lsData << std::dec << std::endl;
        outf << std::hex << msData << std::dec << std::endl;
        outf << std::hex << BXfrOH << std::dec << std::endl;
        outf << std::hex << CRC << std::dec << std::endl;

        //if(checkBlock(b1010,b1100,b1110,sn,CRC,CRC)!=1)std::cout<<checkBlock(b1010,b1100,b1110,sn,CRC,CRC)<<std::endl;


        return;
};


bool checkBlock(const uint8_t &b1010_, const uint8_t &b1100_, const uint8_t &b1110_, const int &sn_, const uint16_t &crc_, const uint16_t &crc_calc_)
{
        if ((b1010_ == 0xa) && (b1100_ == 0xc) && (b1110_ == 0xe) && (sn_ > (-1)) && (sn_ < 24) && (crc_ == crc_calc_)) {
                return true;
        }else {
                return false;
        }
}

