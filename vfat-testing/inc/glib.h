#ifndef GLIB_H
#define GLIB_H


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

class GLIB
{
  //ConnectionManager *manager;
  //HwInterface hw;
  //ValWord< uint32_t > mem;
  
  std::string _boardName;
  std::string _systemName;
  std::string _firmWareDate;
  std::string _firmWareVersion;
  int _firmWare_Id_Ver_MAJOR;
  int _firmWare_Id_Ver_MINOR;
  int _firmWare_Id_Ver_BUILD;
  int _firmWare_Id_YY;
  int _firmWare_Id_MM;
  int _firmWare_Id_DD;

 public:
  GLIB();
  ~GLIB();	
  //GLIB(char *fileName);

 public:
  //Core Function to fill DataMembers
  void FetchBoardName();
  void FetchSystemName();
  void FetchFirmWare();
    
  void test(); 

  //Member functions for users
 public:
  std::string GetBoardDetails();
  std::string GetSystemDetails();
  std::string GetFirmwareDate();
  std::string GetFirmwareVersion();
  std::string GetFirmwareDetails();
  std::string GetAddress(unsigned int val);
  std::string GetMask(unsigned int val);
  std::string ToHexString(unsigned int val);
  std::string GetRegisName();

  //#HELPER functions

};

#endif // GLIB_H
