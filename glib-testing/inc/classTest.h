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
  ConnectionManager *manager;
  ValWord< uint32_t > mem;
  
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
};

#endif // TEST_H
