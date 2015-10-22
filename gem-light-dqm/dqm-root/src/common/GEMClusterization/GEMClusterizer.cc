#include "GEMClusterization/GEMClusterizer.h"
#include "GEMClusterization/GEMCluster.h"
#include "GEMClusterization/GEMClusterContainer.h"
#include "GEMClusterization/GEMStripCollection.h"

GEMClusterizer::GEMClusterizer()
{
}

GEMClusterizer::~GEMClusterizer()
{
}
 
GEMClusterContainer
GEMClusterizer::doAction(const GEMStripCollection& strips){
  GEMClusterContainer cls;
  for (GEMStripCollection::const_iterator strip = strips.begin();
       strip != strips.end();
       strip++) {
    GEMCluster cl(strip->strip(),strip->strip(),strip->bx());
    cls.insert(cl);
  }
  GEMClusterContainer clsNew =this->doActualAction(cls);
  return clsNew;
}

GEMClusterContainer
GEMClusterizer::doActualAction(GEMClusterContainer& initialclusters){
  
  GEMClusterContainer finalCluster;
  GEMCluster prev;

  unsigned int j = 0;
  for(GEMClusterContainer::const_iterator i=initialclusters.begin();
      i != initialclusters.end(); i++){
    GEMCluster cl = *i;

    if(i==initialclusters.begin()){
      prev = cl;
      j++;
      if(j == initialclusters.size()){
	finalCluster.insert(prev);
      }
      else if(j < initialclusters.size()){
	continue;
      }
    }

    if(prev.isAdjacent(cl)) {
      prev.merge(cl);
      j++;
      if(j == initialclusters.size()){
	finalCluster.insert(prev);
      }
    }
    else {
      j++;
      if(j < initialclusters.size()){
	finalCluster.insert(prev);
	prev = cl;
      }
      if(j == initialclusters.size()){
	finalCluster.insert(prev);
	finalCluster.insert(cl);
      }
    }
  }

  return finalCluster;
} 
 

