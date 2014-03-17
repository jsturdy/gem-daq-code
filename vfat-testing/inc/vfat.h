#ifndef GEM_VFAT_H
#define GEM_VFAT_H


#include <string>

#include "uhal/uhal.hpp"

#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <cstdlib>
#include <typeinfo>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

using namespace uhal;
namespace gem {
  namespace base {

    class vfat
    {
    public:
    typedef struct channel {
      int channelID;
      //bits set in the vfat channel registers
      bool calChan; //only channels 2-128, bit 6
      bool calChan0;// only channel 1, bit 7
      bool calChan1;// only channel 1, bit 6
      
      bool mask;    //bit 5
      
      int trimDAC;  //5 bits [4:0]
    } channel;
    
    typedef struct configuration {
      std::string connectionFile;
      std::string deviceID;
      int trigMode;
      int calibMode;
      int calibPol;
      int latency;
      
    } configuration;
    
    typedef struct chipID {
      //identifying information for a specific vfat chip
      //relative position on the GEB wrt OptoHybrid?
      std::string hwConnection;
      std::string name;
      int row;
      int col;
    } chipID;
    
    public:
      vfat(gem::base::vfat::configuration);
      //vfat();	
      ~vfat();	
      //vfat(char *fileName);
      
    public:
      //initialization functions from configuration parameters
      void SetChipID(gem::base::vfat::configuration);
      //HwManager GetHwManager();
      
    public:
      //Core functions to set register values
      void TurnOn();
      void TurnOff();
      void SetTriggerMode(int);
      void SetCalibrationMode(int,int);
      void SetLatency(int);
      void SetHitCounterMode(int);
      void SetDACMode(int);

      void GetRegisterValues();
      void PrintRegisters();
      void PrintRegister(std::string);
      //void PrintExtRegister(gem::base::vfat::extReg, int);

      int GetChipID();
      void SendDFTestPattern();
      void SetMSPulseLength(int);

      //vfat::data ReadData();
    
      void initialize(gem::base::vfat::configuration);
      int GetHitCount();

    public:
      //individual channel functions
      void GetChannelStatus(gem::base::vfat::channel&);
      void SetChannelStatus(gem::base::vfat::channel&);
    
      void MaskChannel(gem::base::vfat::channel&);
      void SendCalibrationPulse(gem::base::vfat::channel&);
    
      

    private:
      ConnectionManager *manager;
      //currently don't know how to initialize HwInterface as a pointer
      //HwInterface *hw;

      chipID _chipID;
      
      //read-write registers
      uint8_t _ContReg0;
      uint8_t _ContReg1;
    
      uint8_t _IPreampIn;
      uint8_t _IPreampFeed;
      uint8_t _IPreampOut;
      uint8_t _IShaper;
      uint8_t _IShaperFeed;
      uint8_t _IComp;
    
      uint8_t _ExtRegPointer;//which extended register to access
      uint8_t _ExtRegData;   //data to be written to the extended register
    
      //read only registers
      uint8_t _ChipID0;
      uint8_t _ChipID1;
    
      uint8_t _UpsetReg;
    
      uint8_t _HitCount0;
      uint8_t _HitCount1;
      uint8_t _HitCount2;
    
      //extended registers
      uint8_t _Lat;//extended register 0
      
      uint8_t _ChanReg[128];//extended registers 1-128, channel specific DACs
    
      uint8_t _VCal;//extended register 129
    
      uint8_t _VThreshold1;//extended register 130
      uint8_t _VThreshold2;//extended register 131
    
      uint8_t _CalPhase;//extended register 132
    
      uint8_t _ContReg2;//extended register 133
      uint8_t _ContReg3;//extended register 134
    
      uint8_t _Spare;   //extended register 135
    
    };

    //class configuration
    //{
    //  
    //};
  }
}
#endif // GEM_VFAT_H
