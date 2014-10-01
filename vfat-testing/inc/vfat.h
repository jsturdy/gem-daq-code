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
	//defines a VFAT channel and the bits that can be set on a per-channel basis

	int channelID;
	//bits set in the vfat channel registers
	bool calChan; //only channels 2-128, bit 6
	bool calChan0;// only channel 1, bit 7
	bool calChan1;// only channel 1, bit 6
      
	bool mask;    //bit 5
      
	int trimDAC;  //5 bits [4:0]
      } channel;
    
      typedef struct configuration {
	//configuration object for initializing a VFAT connection
      
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
	int row; //on the GEM chamber
	int col; //on the GEM chamber
      } chipID;
    
    public:
      //constructor from configuration object
      vfat(gem::base::vfat::configuration);
      ////constructor from configuration file
      //vfat(char *fileName);
      ////no-argument constructor
      //vfat();	
      ~vfat();	
      
    private:
      //initialization functions from configuration parameters
      void initialize(gem::base::vfat::configuration);
      //set which VFAT is being communicated with
      void SetChipID(gem::base::vfat::configuration);
      
    public:
      //Core functions to set register values
      //set control bit to on
      void TurnOn();
      //set control bit to off
      void TurnOff();

      //set various running modes
      void SetTriggerMode(int);
      void SetCalibrationMode(int mode,int pol);
      void SetLatency(int);
      void SetHitCounterMode(int);
      void SetDACMode(int);
      void SetMSPulseLength(int);

      //get and read back the curent register values
      void GetRegisterValues();
      void PrintRegisters();
      void PrintRegister(std::string);
      //void PrintExtRegister(gem::base::vfat::extReg, int);

      //get the VFAT reported chip ID
      int GetChipID();
      
      //get the reported hit count
      int GetHitCount();

      //tell VFAT to send predefined test pattern to the data out
      void SendDFTestPattern();

      ////read the data from a specific VFAT chip
      //vfat::data ReadData();
    
    public:
      //individual per-channel functions
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
