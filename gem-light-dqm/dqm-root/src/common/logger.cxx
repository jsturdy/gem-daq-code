class logger{
  public:
    logger(std::string m_filename, std::string m_run){
      filename_ = m_filename;
      run_ = m_run;
      nEvents_ = 0;
      nGoodEvents_ = 0;
      nBadEvents_ = 0;
      nVFATBlocks_ = 0;
      nGoodVFATBlocks_ = 0;
      nBadVFATBlocks_ = 0;
      averageBlockPerEvent_ = 0;
      averageBadBlockPerBadEvent_ = 0;
      badEventsList_.clear();
    }
    addEvent(long int m_eventNumber, bool m_isEventGood, m_nVFATBlocks, int m_nGoodVFATBlocks_, int m_nBadVFATBlocks_){
      nEvents_++;
      if (m_isEventGood){ nGoodEvents_++;} else { nBadEvents_++; badEventsList_.push_back(m_eventNumber);}
      nVFATBlocks_ += m_nVFATBlocks_;
      nGoodVFATBlocks_ += m_nGoodVFATBlocks_;
      nBadVFATBlocks_ += m_nBadVFATBlocks_;
    }
    writeLog(){
      averageBlockPerEvent_ = (double) nVFATBlocks_/nEvents_;
      averageBadBlockPerBadEvent_ = (double) nBadVFATBlocks_/nBadEvents_;
      ofstream myfile;
      myfile.open (filename_);
      myfile << "GEM Log For Run " << run << "\n" ;
      myfile << "Number of events processed     : " << nEvents << "\n" ;
      myfile << "Number of good events          : " << nGoodEvents << "\n" ;
      myfile << "Number of bad events           : " << nBadEvents << "\n" ;
      myfile << "Number of VFAT blocks          : " << nVFATBlocks_ << "\n" ;
      myfile << "Number of good VFAT blocks     : " << nGoodVFATBlocks_ << "\n" ;
      myfile << "Number of bad VFAT blocks      : " << nBadVFATBlocks_ << "\n" ;
      myfile << "Average N blocks per event     : " << averageBlockPerEvent_ << "\n" ;
      myfile << "Average N bad blocks per event : " << averageBadBlockPerBadEvent_ << "\n" ;
      std::string tmp = "{ ";
      for (int it = badEventsList_.begin(); it < (badEventsList_.end() - 1); it++){
        tmp += badEventsList_.at(it);
        tmp += ", ";
      }
      tmp += badEventsList_.back();
      tmp += " }";
      myfile << "List of the bad events : \n" << tmp << "\n" ;
      myfile.close();
    }
    ~logger(){}
  private:
    std::string filename_;
    std::string run_;
    long int nEvents_;
    long int nGoodEvents_;
    long int nBadEvents_;
    long long int nVFATBlocks_;
    long long int nGoodVFATBlocks_;
    long long int nBadVFATBlocks_;
    double averageBlockPerEvent_;
    double averageBadBlockPerBadEvent_;
    std::vector<long int> badEventsList_;
};
