#include <map>
#include <string>
#include <vector>
struct dqmerror
{
  std::string amcID;
  std::string chamberID;
  std::string chipID;
  int slotNumber;
  int errorCode;
}

class GEMDQMerrors
{
  public:
    GEMDQMerrors(){}
    ~GEMDQMchecker(){}
    void addError(int evN_, std::string & amcID_, std::string & chamberID_, std::string & chipID_, int slotNumber_, int errorCode_)
    {
      m_error.amcID = amcID_;
      m_error.chamberID = chamberID_;
      m_error.chipID = chipID_;
      m_error.slotNumber = slotNumber_;
      m_error.errorCode = errorCode_;
      auto search = m_errors.find(evN_);
      if (search != m_errors.end()){
        search->second.push_back(m_error);
      } else {
        t_dqmerrors.push_back(m_error);
        m_errors.insert(std::pair<int,std::vector<dqmerror>>(evN_,t_dqmerrors));
        t_dqmerrors.clear();
      }
    }
    void saveErrors()
    {
      for (auto it=m_errors.begin()l it!=m_errors.end(); ++it){
        std::cout << "Error codes for event # " << it->first << std::endl;
        for (auto jt=it->second.begin(); jt!=it->second.end(); ++jt){
          std::cout << jt->errorCode << ", ";
        }
      }
    }
  private:
    dqmerror m_error;
    std::vector<dqmerror> t_dqmerrors;
    std::map<int, vector<dqmerror>> m_errors;
};
