#ifndef RecoLocalMuon_GEMClusterizer_h
#define RecoLocalMuon_GEMClusterizer_h
/** \class GEMClusterizer
 *  \author M. Maggi -- INFN Bari
 */

#include "GEMClusterContainer.h"
#include "GEMStripCollection.h"  
#include "GEMCluster.h"
class GEMClusterizer{
 public:
  GEMClusterizer();
  ~GEMClusterizer();
  GEMClusterContainer doAction(const GEMStripCollection& strips);

 private:
  GEMClusterContainer doActualAction(GEMClusterContainer& initialclusters);

 private:
  GEMClusterContainer cls;
};
#endif
