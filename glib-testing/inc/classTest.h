#ifndef TEST_H
#define TEST_H


#include <string>

//taken from mytest.cxx
#include "uhal/uhal.hpp"
//#include "../uhal/tests/tools.hpp"
//#include <boost/filesystem.hpp>

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

class TEST
{
  HwInterface *hw;
  ConnectionManager *manager;
  ValWord< uint32_t > mem;
  ValWord< uint32_t > r_test;
  
  std::string _firmWareDate;
  std::string _firmWareVersion;
  int _firmWare_Id_Ver_MAJOR;
  int _firmWare_Id_Ver_MINOR;
  int _firmWare_Id_Ver_BUILD;
  int _firmWare_Id_YY;
  int _firmWare_Id_MM;
  int _firmWare_Id_DD;
  
 public:
  TEST();
  ~TEST();	
  
 public:
  //Core Function to fill DataMembers
  void FetchFirmWare();
  //void getSystemFW()
  //void getUserFW()
  //void getBoardID()
  //void getSystemID()
  //void getChipID()
  uint32_t getTestReg();
  uint32_t setTestReg(uint32_t);
};

#endif // TEST_H
