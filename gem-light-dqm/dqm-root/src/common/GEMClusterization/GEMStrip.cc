#include "GEMClusterization/GEMStrip.h"

GEMStrip::GEMStrip(): _strip(500), _bx(0) {}

GEMStrip::GEMStrip(unsigned int s, int x) : _strip(s), _bx(x){}

GEMStrip::~GEMStrip(){}

int 
GEMStrip::strip() const
{
  return _strip;
}

int 
GEMStrip::bx() const
{
  return _bx;
}


bool 
GEMStrip::operator<(const GEMStrip& gs) const
{
  if(gs.bx() == this->bx())
    return gs.strip()<this->strip();
  else 
    return gs.bx()<this->bx();
}
