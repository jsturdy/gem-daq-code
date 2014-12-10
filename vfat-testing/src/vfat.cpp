#include "vfat.h"
#include <sstream>
#include <string>

using namespace gem;

gem::base::vfat::vfat(gem::base::vfat::configuration conf){
  //Constructor for VFAT chip control need to set a unique identifier here
  //thinking like a struct that is the chipID, but this could be the constructor argument
  std::string connectionFile = conf.connectionFile;
  char * val;
  val = std::getenv( "GLIBTEST" );
  std::string dirVal = "";
  if (val != NULL) {
    dirVal = val;
  }
  else {
    std::cout<<"$GLIBTEST not set, exiting"<<std::endl;
    exit(1);
  }
  
  char connectionPath[128];
  sprintf( connectionPath, "file://%s/data/myconnections.xml", dirVal.c_str() );
  manager = new ConnectionManager( connectionPath );
  //manager = new ConnectionManager( connectionFile );

  std::string deviceID = conf.deviceID;
  gem::base::vfat::SetChipID( conf );
  char vfatConnection[128];
  sprintf( vfatConnection, "vfat.%s.udp", (_chipID.name).c_str() );
  //hw = manager->getDevice ( vfatConnection );
  //hw = manager->getDevice ( "vfattest.udp" );
  
  gem::base::vfat::initialize( conf );
}

gem::base::vfat::~vfat(){
  //clear all memory
  //delete hw;
  delete manager;
}

//need to be able to specify a given vfat2 chip, but to do this we need a map
void gem::base::vfat::initialize(gem::base::vfat::configuration conf)
{
  //read in configuration options, trigger mode, calibration mode, etc.,
  //set hit count to zero?
  gem::base::vfat::GetRegisterValues();
  gem::base::vfat::TurnOn();
  gem::base::vfat::SetTriggerMode( conf.trigMode );
  gem::base::vfat::SetCalibrationMode( conf.calibMode, conf.calibPol );
  gem::base::vfat::SetLatency( conf.latency );

}

void gem::base::vfat::SetChipID(gem::base::vfat::configuration conf)
{//set the chip id
  _chipID.hwConnection = "vfattest.udp";
  _chipID.name = "VFAT2_0_0";
  _chipID.row = 0;
  _chipID.row = 0;
  return;
}

void gem::base::vfat::GetRegisterValues()
{
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
  char chipNode[128];
  sprintf( chipNode, "%s", (_chipID.name).c_str() );
  ValWord< uint32_t > tmpContReg0      = hw.getNode( _chipID.name+".ContReg0"      ).read();
  ValWord< uint32_t > tmpContReg1      = hw.getNode( _chipID.name+".ContReg1"      ).read();
  ValWord< uint32_t > tmpIPreampIn     = hw.getNode( _chipID.name+".IPreampIn"     ).read();
  ValWord< uint32_t > tmpIPreampFeed   = hw.getNode( _chipID.name+".IPreampFeed"   ).read();
  ValWord< uint32_t > tmpIPreampOut    = hw.getNode( _chipID.name+".IPreampOut"    ).read();
  ValWord< uint32_t > tmpIShaper       = hw.getNode( _chipID.name+".IShaper"       ).read();
  ValWord< uint32_t > tmpIShaperFeed   = hw.getNode( _chipID.name+".IShaperFeed"   ).read();
  ValWord< uint32_t > tmpIComp         = hw.getNode( _chipID.name+".IComp"         ).read();
  ValWord< uint32_t > tmpChipID0       = hw.getNode( _chipID.name+".ChipID0"       ).read();
  ValWord< uint32_t > tmpChipID1       = hw.getNode( _chipID.name+".ChipID1"       ).read();
  ValWord< uint32_t > tmpUpsetReg      = hw.getNode( _chipID.name+".UpsetReg"      ).read();
  ValWord< uint32_t > tmpHitCount0     = hw.getNode( _chipID.name+".HitCount0"     ).read();
  ValWord< uint32_t > tmpHitCount1     = hw.getNode( _chipID.name+".HitCount1"     ).read();
  ValWord< uint32_t > tmpHitCount2     = hw.getNode( _chipID.name+".HitCount2"     ).read();
  ValWord< uint32_t > tmpExtRegPointer = hw.getNode( _chipID.name+".ExtRegPointer" ).read();
  ValWord< uint32_t > tmpExtRegData    = hw.getNode( _chipID.name+".ExtRegData"    ).read();
  hw.dispatch();
  _ContReg0       = tmpContReg0.value()      ;
  _ContReg1       = tmpContReg1.value()      ;
  _IPreampIn      = tmpIPreampIn.value()     ;
  _IPreampFeed    = tmpIPreampFeed.value()   ;
  _IPreampOut     = tmpIPreampOut.value()    ;
  _IShaper        = tmpIShaper.value()       ;
  _IShaperFeed    = tmpIShaperFeed.value()   ;
  _IComp          = tmpIComp.value()         ;
  _ChipID0        = tmpChipID0.value()       ;
  _ChipID1        = tmpChipID1.value()       ;
  _UpsetReg       = tmpUpsetReg.value()      ;
  _HitCount0      = tmpHitCount0.value()     ;
  _HitCount1      = tmpHitCount1.value()     ;
  _HitCount2      = tmpHitCount2.value()     ;
  _ExtRegPointer  = tmpExtRegPointer.value() ;
  _ExtRegData     = tmpExtRegData.value()    ;
  return;
}

void gem::base::vfat::PrintRegisters()
{
  gem::base::vfat::GetRegisterValues();
  std::cout<<"========Reading back normal VFAT registers=================="<<std::endl;
  std::cout<<"+ ContReg0     "<<std::setw(6)<<"= 0x"<<_ContReg0     <<" +"<<std::endl;
  std::cout<<"+ ContReg1     "<<std::setw(6)<<"= 0x"<<_ContReg1     <<" +"<<std::endl;
  std::cout<<"+ IPreampIn    "<<std::setw(6)<<"= 0x"<<_IPreampIn    <<" +"<<std::endl;
  std::cout<<"+ IPreampFeed  "<<std::setw(6)<<"= 0x"<<_IPreampFeed  <<" +"<<std::endl;
  std::cout<<"+ IPreampOut   "<<std::setw(6)<<"= 0x"<<_IPreampOut   <<" +"<<std::endl;
  std::cout<<"+ IShaper      "<<std::setw(6)<<"= 0x"<<_IShaper      <<" +"<<std::endl;
  std::cout<<"+ IShaperFeed  "<<std::setw(6)<<"= 0x"<<_IShaperFeed  <<" +"<<std::endl;
  std::cout<<"+ IComp        "<<std::setw(6)<<"= 0x"<<_IComp        <<" +"<<std::endl;
  std::cout<<"+ ChipID0      "<<std::setw(6)<<"= 0x"<<_ChipID0      <<" +"<<std::endl;
  std::cout<<"+ ChipID1      "<<std::setw(6)<<"= 0x"<<_ChipID1      <<" +"<<std::endl;
  std::cout<<"+ UpsetReg     "<<std::setw(6)<<"= 0x"<<_UpsetReg     <<" +"<<std::endl;
  std::cout<<"+ HitCount0    "<<std::setw(6)<<"= 0x"<<_HitCount0    <<" +"<<std::endl;
  std::cout<<"+ HitCount1    "<<std::setw(6)<<"= 0x"<<_HitCount1    <<" +"<<std::endl;
  std::cout<<"+ HitCount2    "<<std::setw(6)<<"= 0x"<<_HitCount2    <<" +"<<std::endl;
  std::cout<<"============================================================"<<std::endl;
  //std::cout<<"+ExtRegPointer = 0x"<<std::hex<< _ExtRegPointer std::dec<<" +"<<std::endl;
  //std::cout<<"+ExtRegData    = 0x"<<std::hex<< _ExtRegData    std::dec<<" +"<<std::endl;
  //std::cout<<"============================================================"<<std::endl;
  return;
}

void gem::base::vfat::PrintRegister( std::string regName )
{
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
  char chipNode[128];
  sprintf( chipNode, "%s", (_chipID.name).c_str() );
  ValWord< uint32_t > tmpReg      = hw.getNode( _chipID.name+"."+regName ).read();

  std::cout<<"========Reading back normal VFAT registers=================="<<std::endl;
  std::cout<<"+ "<<regName << "    = 0x" << tmpReg.value()     <<" +"<<std::endl; 
  std::cout<<"============================================================"<<std::endl;
  return;
}

//void gem::base::vfat::PrintExtRegister( gem::base::vfat::extReg, int )
//{
//  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
//  char chipNode[128];
//  sprintf( chipNode, "%s", (_chipID.name).c_str() );
//  ValWord< uint32_t > tmpReg      = hw.getNode( _chipID.name+"."regName ).read();
//
//  std::cout<<"========Reading back extended VFAT registers=================="<<std::endl;
//  std::cout<<"+ "<<regName" <<     = 0x" << tmpReg     <<" +"<<std::endl; 
//  std::cout<<"============================================================"<<std::endl;
//  return;
//}


void gem::base::vfat::TurnOn()
{//toggles the sleep bit high
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
  char chipNode[128];
  sprintf( chipNode, "%s.ContReg0", (_chipID.name).c_str() );
  _ContReg0 |= 0x01;
  hw.getNode( chipNode ).write( _ContReg0 );
  hw.dispatch();
  return;
}

void gem::base::vfat::TurnOff()
{//toggles the sleep bit low
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
  char chipNode[128];
  sprintf( chipNode, "%s.ContReg0", (_chipID.name).c_str() );
  _ContReg0 &= 0xFE;
  hw.getNode( chipNode ).write( _ContReg0 );
  hw.dispatch();
  return;
}

void gem::base::vfat::SetLatency(int latency)
{//sets the LV1A latency in terms of clock cycles (1-256, default value is 128)
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
  char chipNode[128];
  sprintf( chipNode, "%s.", (_chipID.name).c_str() );
  //select Lat (0) from the extended registers
  sprintf( chipNode, "%s.ExtRegPointer", (_chipID.name).c_str() );
  _ExtRegPointer = 0;
  hw.getNode( chipNode ).write( _ExtRegPointer );
  
  sprintf( chipNode, "%s.ExtRegData", (_chipID.name).c_str() );
  _ExtRegData |= latency;
  hw.getNode( chipNode ).write( _ExtRegData );
  hw.dispatch();
  return;
}

enum calMode {Noraml, VCal, Baseline, External};

void gem::base::vfat::SetCalibrationMode(int mode, int pol)
{/*Calibration mode (0-3)
  0: normal running
  1: CalOut = VCal
  2: CalOut = Baseline
  3: CalOut = External
  CalPolarity: for positive, 1 for negative
 */
  
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );

  //if !(mode == gem::base::vfat::calMode::Normal) {
  //    
  //}
    
  char chipNode[128];
  sprintf( chipNode, "%s.ContReg0", (_chipID.name).c_str() );
  _ContReg0 |= ( mode<<6 ); //bits 7:6 on ContReg0
  _ContReg0 |= ( mode<<5 ); //bit 5 on ContReg0
  hw.getNode( chipNode ).write( _ContReg0 );

  hw.dispatch();
  return;
}

void gem::base::vfat::SetTriggerMode(int mode)
{/*selects GEM running and toggles the trigger mode
   0: no trigger (default)
   1: one sector (S1)
   2: four sectors (S1-S4)
   3: eight sectors (S1-S8)
   4-: GEM mode, S1-S8 defined elsewhere
  */
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
  char chipNode[128];
  sprintf( chipNode, "%s.ContReg0", (_chipID.name).c_str() );
  _ContReg0 |= ( mode<<1 ); //bits 4:1 on ContReg0
  hw.getNode( chipNode ).write( _ContReg0 );
  hw.dispatch();
  return;
}

void gem::base::vfat::SetHitCounterMode(int mode)
{/*selects the value read out by the HitCount counter
   0: FastOR of all 128 channels
   1-8: counts in S1-S8, respectively
 */
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
  char chipNode[128];
  //select ContReg2 from the extended registers
  sprintf( chipNode, "%s.ExtRegPointer", (_chipID.name).c_str() );
  _ExtRegPointer = 133;
  hw.getNode( chipNode ).write( _ExtRegPointer );
  
  //set the bits (hit count mode are bits [3:0]
  sprintf( chipNode, "%s.ExtRegData", (_chipID.name).c_str() );
  _ExtRegData |= mode;
  hw.getNode( chipNode ).write( _ExtRegData );
  
  hw.dispatch();
  return;
}

void gem::base::vfat::SetDACMode(int mode)
{/*selects mode for DAC to DCU
   0:  normal running (DACo-V "low", DACo-I "hiZ"
   1:  IPreampIn to to DACo-I (DACo-V "low")
   2:  IPreampFeed to DACo-I (DACo-V "low")
   3:  IPreampOut to DACo-I (DACo-V "low")
   4:  IShaper to DACo-I (DACo-V "low")
   5:  IShaperFeed to DACo-I (DACo-V "low")
   6:  IComp to DACo-I (DACo-V "low")
   //are these correct? vfat2 manual table 17 has VThreshold1/2 but no IThreshold1/2
   7:  IThreshold1 to DACo-I (DACo-V "low")
   8:  IThreshold2 to DACo-I (DACo-V "low")
   9:  VCal to DACo-V (DACo-I "hiZ")
   10: CalOut to DACo-V (DACo-I "hiZ")
   11-15: spare
 */
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
  char chipNode[128];
  //select ContReg2 from the extended registers
  sprintf( chipNode, "%s.ContReg1", (_chipID.name).c_str() );
  //bits of interest are 3:0 on ContReg1
  _ContReg1 |= mode;
  hw.getNode( chipNode ).write( _ContReg1 );
  hw.dispatch();
  return;
}

int gem::base::vfat::GetHitCount()
{//reads the hit count from the VFAT according to the trigger logic
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
  char chipNode[128];
  sprintf( chipNode, "%s", (_chipID.name).c_str() );
  ValWord< uint32_t > tmpHitCount0   = hw.getNode( _chipID.name+".HitCount0" ).read();
  ValWord< uint32_t > tmpHitCount1   = hw.getNode( _chipID.name+".HitCount1" ).read();
  ValWord< uint32_t > tmpHitCount2   = hw.getNode( _chipID.name+".HitCount2" ).read();
  hw.dispatch();
  _HitCount0      = tmpHitCount0.value();
  _HitCount1      = tmpHitCount1.value();
  _HitCount2      = tmpHitCount2.value();
  //should be just a 24 bit number
  uint32_t count = 0;
  count = count<<8+_HitCount2;
  count = count<<8+_HitCount1;
  count = count<<8+_HitCount0;
  //return ( _HitCount2<<16 )|( _HitCount1<<8 )|_HitCount0;
  return count;
}

int gem::base::vfat::GetChipID()
{//returns the 24 bit value of the chipID register
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
  char chipNode[128];
  sprintf( chipNode, "%s", (_chipID.name).c_str() );
  ValWord< uint32_t > tmpChipID0   = hw.getNode( _chipID.name+".ChipID0" ).read();
  ValWord< uint32_t > tmpChipID1   = hw.getNode( _chipID.name+".ChipID1" ).read();
  //ValWord< uint32_t > tmpChipID2   = hw.getNode( _chipID.name+".ChipID2" ).read();
  hw.dispatch();
  _ChipID0      = tmpChipID0.value();
  _ChipID1      = tmpChipID1.value();
  uint32_t chipID = 0;
  chipID = chipID<<8+_ChipID1;
  chipID = chipID<<8+_ChipID0;
  //return ( _ChipID1<<8 )|_ChipID0;
  return chipID;
}

void gem::base::vfat::SendDFTestPattern()
{/*Sends a predefined packet to the DataOut without
   the need for a trigger.
 */
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
  char chipNode[128];
  //select ContReg3 from the extended registers
  sprintf( chipNode, "%s.ExtRegPointer", (_chipID.name).c_str() );
  _ExtRegPointer = 134;
  hw.getNode( chipNode ).write( _ExtRegPointer );
  
  //set the bit (DFTestPattern is bit 4)
  sprintf( chipNode, "%s.ExtRegData", (_chipID.name).c_str() );
  _ExtRegData |= 0x10;//16
  hw.getNode( chipNode ).write( _ExtRegData );
  
  hw.dispatch();
  return;
}

void gem::base::vfat::SetMSPulseLength(int length)
{/*Sets the pulse length of the monostable
   1-8 clock periods
   ContReg2 bits 6:4
 */
  HwInterface hw = manager->getDevice ( _chipID.hwConnection );
  char chipNode[128];
  //select ContReg2 from the extended registers
  sprintf( chipNode, "%s.ExtRegPointer", (_chipID.name).c_str() );
  _ExtRegPointer = 133;
  hw.getNode( chipNode ).write( _ExtRegPointer );
  
  //set the bits (pulse length are bits [6:4]
  sprintf( chipNode, "%s.ExtRegData", (_chipID.name).c_str() );
  _ExtRegData |= ( length<<4 );
  hw.getNode( chipNode ).write( _ExtRegData );
  
  hw.dispatch();
  return;
}
