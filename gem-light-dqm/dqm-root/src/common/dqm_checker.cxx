class GEMDQMchecker
{
  public:
    GEMDQMchecker(){}
    virtual ~GEMDQMchecker(){}
    bool check(){return m_check;}
    int getErrorCode(){return m_errorCode;}
  private:
    int m_errorCode;
    bool m_check;
};
