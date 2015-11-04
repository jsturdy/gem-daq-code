#ifndef GEMStrip_h
#define GEMStrip_h
class GEMStrip{
 public:
  GEMStrip();
  GEMStrip(unsigned int s, int x);
  ~GEMStrip();
  int strip() const;
  int bx() const;
  /* allow the sort operator for mapping */
  bool operator<(const GEMStrip& gs) const;
 private:
  uint16_t _strip;
  int16_t _bx;
};
#endif
