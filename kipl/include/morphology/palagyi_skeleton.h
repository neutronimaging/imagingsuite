//<LICENCE>

#ifndef _PALAGYI_SKELETON_
#define _PALAGYI_SKELETON_

// implement thinning templates given by 
// Palagyi and Kuba in "A Parallel 3D 12-Subiteration Thinning Algorithm", 1999
//
#include <iostream>
#include <vector>
#include <deque>

#include "../base/timage.h"
#include "morphology.h"
#include "base3dskeleton.h"
#include "label.h"

using namespace std;

namespace kipl { namespace morphology {

template<class ImgType>
class PalagyiSkeleton: public kipl::morphology::Base3Dskeleton<ImgType> {
private:
	enum Direction {
	UP_SOUTH = 0, 
	NORT_EAST,
	DOWN_WEST, 
	
	SOUTH_EAST,
	UP_WEST, 
	DOWN_NORTH, 
	
	SOUTH_WEST,
	UP_NORTH, 
	DOWN_EAST, 
	
	NORT_WEST,
	UP_EAST, 
	DOWN_SOUTH,
	
	UP,
	DOWN, 
	EAST, 
	WEST, 
	NORTH, 
	SOUTH
	};
	
public:
	PalagyiSkeleton();
	~PalagyiSkeleton(){}
	int process(kipl::base::TImage<ImgType,3> & img, bool Quiet=true);
private:
	bool MatchesATemplate(ImgType n[3][3][3]);
	bool MatchesATemplate2(ImgType n[3][3][3]);
	bool TransformNeighborhood(	ImgType n[3][3][3], 
								char direction, 
								ImgType USn[3][3][3]);
								
	bool markBoundaryInDirection(kipl::base::TImage<ImgType,3> &img,
								 vector<long> &objectVoxels, 
								 vector<long> &boundaryVoxels,
			      				 char d1, char d2);
			      				 
	bool markBoundaryInDirection(kipl::base::TImage<ImgType,3> &img,
								 vector<long> &objectVoxels, 
								 vector<long> &boundaryVoxels,
			      				 char direction);
			      				 
	int CleanObjectVector(kipl::base::TImage<ImgType,3> &img,vector<long> &objectVoxels);
};

template<class ImgType>
PalagyiSkeleton<ImgType>::PalagyiSkeleton()
{
	this->algorithm_name="Palagyi";
}

template<class ImgType>
int PalagyiSkeleton<ImgType>::process(kipl::base::TImage<ImgType,3> &img, bool Quiet) 
{
  this->quiet=Quiet;
  int L, M, N;
  int sz, slsz, nidx;
  long idx;
  int nrDel;
  int dir;
  ImgType nb[3][3][3];
  ImgType USn[3][3][3];

  int nrPasses;
  int maxnsp;
  char ii, jj, kk;

  unsigned int tmp, nrSubsets;
  int len;

  char spList[26][3];
  char spListLen;
  bool canBeDeleted = false;

  L=img.Size(0);  M =img.Size(1); N = img.Size(2);
  
  sz = L*M*N;
  slsz = L*M;

  // set all object voxels to object
	if (this->N26)
		delete this->N26;

	this->N26=new CNeighborhood(img.Dims(),3, conn26);
  vector<long> objectVoxels, boundaryVoxels;
  
  ImgType *vol=img.GetDataPtr();
  for(idx=0; idx < img.Size(); idx++) {
    if(img[idx] != 0) {
    	img[idx] = this->object;
    	objectVoxels.push_back(idx);
    }
  }

  nrDel = 1;
  nrPasses = 1;
  maxnsp = 0;

  unsigned char firstBoundary[12]= {5 , 3, 4, 2, 5, 4, 2, 5, 4, 3, 5, 4};
  unsigned char secondBoundary[12]=  { 2, 0, 1, 0, 1, 3, 1, 3, 0, 1, 0,2};

  
  deque<long> simpleVoxels, nonSimpleVoxels;
  int endpoint;
  vector<long>::iterator it;
  
  while(nrDel > 0) {
	if (!this->quiet)
    	cout<<"Pass "<<nrPasses<<"..."<<endl;
    else
    	cout<<"."<<flush;
    nrDel = 0;
    for(dir = 0; dir < 12; dir++) {
		if (!this->quiet)
			cout<<"\tDir "<<dir<<","<<flush;
	
		boundaryVoxels.clear();
 		markBoundaryInDirection(img, objectVoxels, boundaryVoxels,firstBoundary[dir]);
  		markBoundaryInDirection(img, objectVoxels, boundaryVoxels,secondBoundary[dir]);
  		
		// check each boundary point and remove it if itmacthes a template
		if (!this->quiet)
			cout<<" boundary size: "<<boundaryVoxels.size()<<" "<<flush;
		while (!boundaryVoxels.empty()) {
			idx=boundaryVoxels.back();
			boundaryVoxels.pop_back();
			if(img[idx] == this->border) {
				// copy neighborhood into buffer
				endpoint=Base3Dskeleton<ImgType>::CopyNeighborhoodInBuffer(img, idx, nb);
				TransformNeighborhood(nb, dir, USn);

				if(MatchesATemplate(USn)) {		  
					// delete the point
					// can be removed
					img[idx] = this->simple;
					simpleVoxels.push_back(idx);
					//nrDel++;
				}
				else {
				//	img[idx]= object;
					nonSimpleVoxels.push_back(idx);
				}
			}
		}
			
		nrDel=simpleVoxels.size();
		// reset all object voxels to object
		while (!simpleVoxels.empty()) {
			idx=simpleVoxels.front();
			simpleVoxels.pop_front();
			img[idx]=this->background;
		}
		while (!nonSimpleVoxels.empty()) {
			idx=nonSimpleVoxels.front();
			nonSimpleVoxels.pop_front();
			vol[idx]=this->object;
		}
		
		if (!this->quiet)
      		cout<<"done."<<endl;
    }
	if (!this->quiet)
    	cout<<"Number of deleted voxels in pass "<<nrPasses<<": "<<nrDel<<endl;
    	
//    CleanObjectVector(img,objectVoxels);
    nrPasses++;
  }
  cout<<endl;
  
  return 0;
}


template<class ImgType>
int PalagyiSkeleton<ImgType>::CleanObjectVector(kipl::base::TImage<ImgType,3> &img, vector<long> &objectVoxels)
{
	vector<long>::iterator it, toRemove;
	vector<long> tmp;
	for (it=objectVoxels.begin(); it!=objectVoxels.end(); it++) 
		if (img[*it]==this->object) 
			tmp.push_back(*it);
			
	objectVoxels=tmp;
	
	return 0;
}

template<class ImgType>
bool PalagyiSkeleton<ImgType>::MatchesATemplate(ImgType n[3][3][3]) {
  // T1
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object))
     &&
     ((n[0][0][2] + n[1][0][2] + n[2][0][2] + 
       n[0][1][2] + n[1][1][2] + n[2][1][2] + 
       n[0][2][2] + n[1][2][2] + n[2][2][2]) == 0)
     &&
     ((n[0][0][0] + n[1][0][0] + n[2][0][0] + n[0][1][0] + n[2][1][0] + n[0][2][0] + n[1][2][0] + n[2][2][0] + 
       n[0][0][1] + n[1][0][1] + n[2][0][1] + n[0][1][1] + n[2][1][1] + n[0][2][1] + n[1][2][1] + n[2][2][1]) > 0))
  {
    // it matches T1
    return true;
  }
  
  // T2
  if(
     ((n[1][1][1] == this->object) && (n[1][2][1] == this->object))
     &&
     (n[0][0][0] + n[1][0][0] + n[2][0][0] +
      n[0][0][1] + n[1][0][1] + n[2][0][1] +
      n[0][0][2] + n[1][0][2] + n[2][0][2] == 0)
     && 
     (n[0][1][0] + n[1][1][0] + n[2][1][0] + n[0][1][1] + n[2][1][1] + n[0][1][2] + n[1][1][2] + n[2][1][2] + 
      n[0][2][0] + n[1][2][0] + n[2][2][0] + n[0][2][1] + n[2][2][1] + n[0][2][2] + n[1][2][2] + n[2][2][2] > 0))
  {
    // it matches T2
    return true;
  }

  // T3
  if(
     ((n[1][1][1] == this->object) && (n[1][2][0] == this->object)) 
     &&
     ((n[0][0][0] + n[1][0][0] + n[2][0][0] + 
       n[0][0][1] + n[1][0][1] + n[2][0][1] + 
       n[0][0][2] + n[1][0][2] + n[2][0][2] + 
       n[0][1][2] + n[1][1][2] + n[2][1][2] + 
       n[0][2][2] + n[1][2][2] + n[2][2][2]) == 0)
     &&
     ((n[0][1][0] + n[0][2][0] + n[2][1][0] + n[2][2][0] +
       n[0][1][1] + n[0][2][1] + n[2][1][1] + n[2][2][1]) > 0))
  {
    // it matches T3
    return true;
  }
  
  // T4
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) && 
      (n[1][2][1] == this->object))
     &&
     ((n[1][0][1] + n[0][0][2] + n[1][0][2] + n[2][0][2] + n[1][1][2]) == 0)
     &&
     ((n[0][0][1] * n[0][1][2]) == 0)
     &&
     ((n[2][0][1] * n[2][1][2]) == 0))
  {
    // it matches T4
    return true;
  }
  
  // T5
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) && 
      (n[1][2][1] == this->object) && (n[2][0][2] == this->object))
     &&
     ((n[1][0][1] + n[0][0][2] + n[1][0][2] + n[1][1][2]) == 0)
     && 
     ((n[0][0][1] * n[0][1][2]) == 0)
     &&
     ((n[2][0][1] + n[2][1][2]) == this->object))
  {
    // matches T5
    return true;
  }

  // T6
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) && 
      (n[1][2][1] == this->object) && (n[0][0][2] == this->object))
     &&
     ((n[1][0][1] + n[1][0][2] + n[2][0][2] + n[1][1][2]) == 0)
     &&
     ((n[0][0][1] + n[0][1][2]) == this->object)
     &&
     ((n[2][0][1] * n[2][1][2]) == 0))
  {
    // matches T6
    return true;
  }

  // T7
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) &&
      (n[1][2][1] == this->object) && (n[2][1][1] == this->object))
     &&
     ((n[1][0][1] + n[0][0][2] + n[1][0][2] + n[1][1][2]) == 0)
     &&
     ((n[0][0][1] * n[0][1][2]) == 0))
  {
    return true;
  }

  // T8
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) && 
      (n[1][2][1] == this->object) && (n[0][1][1] == this->object))
     &&
     ((n[1][0][1] + n[1][0][2] + n[2][0][2] + n[1][1][2]) == 0)
     &&
     ((n[2][0][1] * n[2][1][2]) == 0))
  {
    return true;
  }

  // T9
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) &&
      (n[1][2][1] == this->object) && (n[2][1][1] == this->object) &&
      (n[0][0][2] == this->object))
     &&
     ((n[1][0][1] + n[1][0][2] + n[1][1][2]) == 0) 
     &&
     ((n[0][0][1] + n[0][1][2]) == this->object))
  {
    return true;
  }

  // T10
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) &&
      (n[0][1][1] == this->object) && (n[1][2][1] == this->object) &&
      (n[2][0][2] == this->object))
     &&
     ((n[1][0][1] + n[1][0][2] + n[1][1][2]) == 0) 
     &&
     ((n[2][0][1] + n[2][1][2]) == this->object))
  {
    return true;
  }

  // T11
  if(
     ((n[1][1][1] == this->object) && (n[2][1][0] == this->object) &&
      (n[1][2][0] == this->object))
     &&
     ((n[0][0][0] + n[1][0][0] + 
       n[0][0][1] + n[1][0][1] + 
       n[0][0][2] + n[1][0][2] + n[2][0][2] +
       n[0][1][2] + n[1][1][2] + n[2][1][2] +
       n[0][2][2] + n[1][2][2] + n[2][2][2]) == 0))
  {
    return true;
  }

  // T12
  if(
     ((n[1][1][1] == this->object) && (n[0][1][0] == this->object) && 
      (n[1][2][0] == this->object))
     &&
     ((n[1][0][0] + n[2][0][0] + 
       n[1][0][1] + n[2][0][1] + 
       n[0][0][2] + n[1][0][2] + n[2][0][2] +
       n[0][1][2] + n[1][1][2] + n[2][1][2] +
       n[0][2][2] + n[1][2][2] + n[2][2][2]) == 0))
  {
    return true;
  }

  // T13
  if(
     ((n[1][1][1] == this->object) && (n[1][2][0] == this->object) && 
      (n[2][2][1] == this->object))
     &&
     ((n[0][0][0] + n[1][0][0] + n[2][0][0] + 
       n[0][0][1] + n[1][0][1] + n[2][0][1] + 
       n[0][0][2] + n[1][0][2] + n[2][0][2] + 
       n[0][1][2] + n[1][1][2] + 
       n[0][2][2] + n[1][2][2]) == 0))
  {
    return true;
  }

  // T14
  if(
     ((n[1][1][1] == this->object) && (n[1][2][0] == this->object) &&
      (n[0][2][1] == this->object))
     &&
     ((n[0][0][0] + n[1][0][0] + n[2][0][0] + 
       n[0][0][1] + n[1][0][1] + n[2][0][1] + 
       n[0][0][2] + n[1][0][2] + n[2][0][2] + 
       n[1][1][2] + n[2][1][2] + 
       n[1][2][2] + n[2][2][2]) == 0))
  {
    return true;
  }

  return false;
}



// transform neighborhood from a different direction
template<class ImgType>
bool PalagyiSkeleton<ImgType>::TransformNeighborhood(ImgType n[3][3][3], 
													 char direction, 
													 ImgType USn[3][3][3]) 
{
  int i, j, k,k2,j2;
  ImgType tmp[3][3][3];
  
  switch(direction) {
  case 0:  //UP_SOUTH = 0, 
    // just copy
    memcpy(USn, n, 27*sizeof(unsigned char));
    break;
  case 1:  //NORT_EAST,
    // 1
    for(k=0; k < 3; k++) {
      for(j=0; j < 3; j++) {
		for(i=0; i < 3; i++) {
	  		tmp[j][2-i][k] = n[i][j][k];
		}
      }
    }
    // 2
    for(k=0; k < 3; k++) {
      k2=2-k;
      for(j=0; j < 3; j++) {
		for(i=0; i < 3; i++) {
			USn[k2][j][i] = tmp[i][j][k];
		}
      }
    }
    break;
  case 2: // DOWN_WEST, 
    // 1
    for(k=0; k < 3; k++) {
      for(j=0; j < 3; j++) {
      	j2=2-j;
		for(i=0; i < 3; i++) {
	  		tmp[j2][i][k] = n[i][j][k];
		}
      }
    }
    // 2
    for(k=0; k < 3; k++) {
    	k2=2-k;
      for(j=0; j < 3; j++) {
		for(i=0; i < 3; i++) {
	  		USn[i][j][k2] = tmp[i][j][k];
		}
      }
    }
    break;
  case 3: // SOUTH_EAST,
    // 1
    for(k=0; k < 3; k++) {
    	k2=2-k;
      for(j=0; j < 3; j++) {
		for(i=0; i < 3; i++) {
	  		USn[k2][j][i] = n[i][j][k];
		}
      }
    }
    break;
  case 4: //UP_WEST, 
    // 1
    for(k=0; k < 3; k++) {
      for(j=0; j < 3; j++) {
      	j2=2-j;
		for(i=0; i < 3; i++) {
	  		USn[j2][i][k] = n[i][j][k];
		}
      }
    }
    break;
  case 5: // DOWN_NORTH, 
    // 1
    for(k=0; k < 3; k++) {
      for(j=0; j < 3; j++) {
      	j2=2-j;
		for(i=0; i < 3; i++) {
	  		tmp[i][j2][k] = n[i][j][k];
		}
      }
    }
    // 2
    for(k=0; k < 3; k++) {
    	k2=2-k;
      for(j=0; j < 3; j++) {
		for(i=0; i < 3; i++) {
	  		USn[i][j][k2] = tmp[i][j][k];
		}
      }
    }
    break;
  case 6: //SOUTH_WEST,
    // 1
    for(k=0; k < 3; k++) {
      for(j=0; j < 3; j++) {
		for(i=0; i < 3; i++) {
		  USn[k][j][2-i] = n[i][j][k];
		}
      }
    }
    break;
  case 7: //UP_NORTH, 
    // 1
    for(k=0; k < 3; k++) {
      for(j=0; j < 3; j++) {
      	j2=2-j;
		for(i=0; i < 3; i++) {
	  		USn[i][j2][k] = n[i][j][k];
		}
      }
    }
    break;
  case 8: // DOWN_EAST, 
     // 1
    for(k=0; k < 3; k++) {
      for(j=0; j < 3; j++) {
		for(i=0; i < 3; i++) {
	  		tmp[j][2-i][k] = n[i][j][k];
		}
      }
    }
    // 2
    for(k=0; k < 3; k++) {
      k2=2-k;
      for(j=0; j < 3; j++) {
		for(i=0; i < 3; i++) {
	  		USn[i][j][2-k] = tmp[i][j][k];
		}
      }
    }
    break;
  case 9: // NORT_WEST,
    // 1
    for(k=0; k < 3; k++) {
      for(j=0; j < 3; j++) {
      	j2=2-j;
		for(i=0; i < 3; i++) {
	  		tmp[2-j][i][k] = n[i][j][k];
		}
      }
    }
    // 2
    for(k=0; k < 3; k++) {
      for(j=0; j < 3; j++) {
		for(i=0; i < 3; i++) {
	  		USn[k][j][2-i] = tmp[i][j][k];
		}
      }
    }
    break;
  case 10: // UP_EAST, 
    // 1
    for(k=0; k < 3; k++) {
      for(j=0; j < 3; j++) {
		for(i=0; i < 3; i++) {
		  USn[j][2-i][k] = n[i][j][k];
		}
      }
    }
    break;
  case 11: //  DOWN_SOUTH,
    // 1
    for(k=0; k < 3; k++) {
    	k2=2-k;
      for(j=0; j < 3; j++) {
		for(i=0; i < 3; i++) {
	  		USn[i][j][k2] = n[i][j][k];
		}
      }
    }
    break;
  }
  
  return true; 
}

template<class ImgType>
bool PalagyiSkeleton<ImgType>::MatchesATemplate2(ImgType n[3][3][3]) {
  // T1
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object))
     &&
     ((n[0][0][2] | n[1][0][2] | n[2][0][2] | 
       n[0][1][2] | n[1][1][2] | n[2][1][2] | 
       n[0][2][2] | n[1][2][2] | n[2][2][2]) == 0)
     &&
     ((n[0][0][0] | n[1][0][0] | n[2][0][0] | n[0][1][0] | n[2][1][0] | n[0][2][0] | n[1][2][0] | n[2][2][0] | 
       n[0][0][1] | n[1][0][1] | n[2][0][1] | n[0][1][1] | n[2][1][1] | n[0][2][1] | n[1][2][1] | n[2][2][1]) != 0))
  {
    // it matches T1
    return true;
  }
  
  // T2
  if(
     ((n[1][1][1] == this->object) && (n[1][2][1] == this->object))
     &&
     (n[0][0][0] | n[1][0][0] | n[2][0][0] |
      n[0][0][1] | n[1][0][1] | n[2][0][1] |
      n[0][0][2] | n[1][0][2] | n[2][0][2] == 0)
     && 
     (n[0][1][0] | n[1][1][0] | n[2][1][0] | n[0][1][1] | n[2][1][1] | n[0][1][2] | n[1][1][2] | n[2][1][2] | 
      n[0][2][0] | n[1][2][0] | n[2][2][0] | n[0][2][1] | n[2][2][1] | n[0][2][2] | n[1][2][2] | n[2][2][2] > 0))
  {
    // it matches T2
    return true;
  }

  // T3
  if(
     ((n[1][1][1] == this->object) && (n[1][2][0] == this->object)) 
     &&
     ((n[0][0][0] | n[1][0][0] | n[2][0][0] | 
       n[0][0][1] | n[1][0][1] | n[2][0][1] | 
       n[0][0][2] | n[1][0][2] | n[2][0][2] | 
       n[0][1][2] | n[1][1][2] | n[2][1][2] | 
       n[0][2][2] | n[1][2][2] | n[2][2][2]) == 0)
     &&
     ((n[0][1][0] | n[0][2][0] | n[2][1][0] | n[2][2][0] |
       n[0][1][1] | n[0][2][1] | n[2][1][1] | n[2][2][1]) > 0))
  {
    // it matches T3
    return true;
  }
  
  // T4
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) && 
      (n[1][2][1] == this->object))
     &&
     ((n[1][0][1] | n[0][0][2] | n[1][0][2] | n[2][0][2] | n[1][1][2]) == 0)
     &&
     ((n[0][0][1] & n[0][1][2]) == 0)
     &&
     ((n[2][0][1] & n[2][1][2]) == 0))
  {
    // it matches T4
    return true;
  }
  
  // T5
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) && 
      (n[1][2][1] == this->object) && (n[2][0][2] == this->object))
     &&
     ((n[1][0][1] | n[0][0][2] | n[1][0][2] | n[1][1][2]) == 0)
     && 
     ((n[0][0][1] & n[0][1][2]) == 0)
     &&
     ((n[2][0][1]== this->object ^ n[2][1][2]==this->object) == this->object))
  {
    // matches T5
    return true;
  }

  // T6
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) && 
      (n[1][2][1] == this->object) && (n[0][0][2] == this->object))
     &&
     ((n[1][0][1] | n[1][0][2] | n[2][0][2] | n[1][1][2]) == 0)
     &&
     ((n[0][0][1]== this->object ^ n[0][1][2]== this->object) == this->object)
     &&
     ((n[2][0][1] & n[2][1][2]) == 0))
  {
    // matches T6
    return true;
  }

  // T7
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) &&
      (n[1][2][1] == this->object) && (n[2][1][1] == this->object))
     &&
     ((n[1][0][1] | n[0][0][2] | n[1][0][2] | n[1][1][2]) == 0)
     &&
     ((n[0][0][1] & n[0][1][2]) == 0))
  {
    return true;
  }

  // T8
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) && 
      (n[1][2][1] == this->object) && (n[0][1][1] == this->object))
     &&
     ((n[1][0][1] | n[1][0][2] | n[2][0][2] | n[1][1][2]) == 0)
     &&
     ((n[2][0][1] & n[2][1][2]) == 0))
  {
    return true;
  }

  // T9
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) &&
      (n[1][2][1] == this->object) && (n[2][1][1] == this->object) &&
      (n[0][0][2] == this->object))
     &&
     ((n[1][0][1] | n[1][0][2] | n[1][1][2]) == 0) 
     &&
     ((n[0][0][1]== this->object ^ n[0][1][2]== this->object) == this->object))
  {
    return true;
  }

  // T10
  if(
     ((n[1][1][1] == this->object) && (n[1][1][0] == this->object) &&
      (n[0][1][1] == this->object) && (n[1][2][1] == this->object) &&
      (n[2][0][2] == this->object))
     &&
     ((n[1][0][1] | n[1][0][2] | n[1][1][2]) == 0) 
     &&
     ((n[2][0][1]== this->object ^ n[2][1][2]== this->object) == this->object))
  {
    return true;
  }

  // T11
  if(
     ((n[1][1][1] == this->object) && (n[2][1][0] == this->object) &&
      (n[1][2][0] == this->object))
     &&
     ((n[0][0][0] | n[1][0][0] | 
       n[0][0][1] | n[1][0][1] | 
       n[0][0][2] | n[1][0][2] | n[2][0][2] |
       n[0][1][2] | n[1][1][2] | n[2][1][2] |
       n[0][2][2] | n[1][2][2] | n[2][2][2]) == 0))
  {
    return true;
  }

  // T12
  if(
     ((n[1][1][1] == this->object) && (n[0][1][0] == this->object) && 
      (n[1][2][0] == this->object))
     &&
     ((n[1][0][0] | n[2][0][0] | 
       n[1][0][1] | n[2][0][1] | 
       n[0][0][2] | n[1][0][2] | n[2][0][2] |
       n[0][1][2] | n[1][1][2] | n[2][1][2] |
       n[0][2][2] | n[1][2][2] | n[2][2][2]) == 0))
  {
    return true;
  }

  // T13
  if(
     ((n[1][1][1] == this->object) && (n[1][2][0] == this->object) && 
      (n[2][2][1] == this->object))
     &&
     ((n[0][0][0] | n[1][0][0] | n[2][0][0] | 
       n[0][0][1] | n[1][0][1] | n[2][0][1] | 
       n[0][0][2] | n[1][0][2] | n[2][0][2] | 
       n[0][1][2] | n[1][1][2] | 
       n[0][2][2] | n[1][2][2]) == 0))
  {
    return true;
  }

  // T14
  if(
     ((n[1][1][1] == this->object) && (n[1][2][0] == this->object) &&
      (n[0][2][1] == this->object))
     &&
     ((n[0][0][0] | n[1][0][0] | n[2][0][0] | 
       n[0][0][1] | n[1][0][1] | n[2][0][1] | 
       n[0][0][2] | n[1][0][2] | n[2][0][2] | 
       n[1][1][2] | n[2][1][2] | 
       n[1][2][2] | n[2][2][2]) == 0))
  {
    return true;
  }

  return false;
}

template<class ImgType>
bool PalagyiSkeleton<ImgType>::markBoundaryInDirection(kipl::base::TImage<ImgType,3> &img,
								 vector<long> &objectVoxels, 
								 vector<long> &boundaryVoxels,
			      				 char direction)
{
	vector<long>::iterator it;
	long pos1;
	ImgType val=0;
	for (it=objectVoxels.begin(); it!=objectVoxels.end(); it++) {
		pos1=this->N26->neighbor(*it,direction);
		
		val=img[*it];
		if (val== this->object) {
			if ((pos1<0)) {
				img[*it]= this->border;
				boundaryVoxels.push_back(*it);
			}
			else {
				if ((img[pos1]==this->background)){
					img[*it]=this->border;
					boundaryVoxels.push_back(*it);
				}
			}
		}
	}
		
	return true;
}

template<class ImgType>
bool PalagyiSkeleton<ImgType>::markBoundaryInDirection(kipl::base::TImage<ImgType,3> &img,
								 vector<long> &objectVoxels, 
								 vector<long> &boundaryVoxels,
			      				 char d1, char d2)
{
	vector<long>::iterator it;
	long pos1, pos2;
	ImgType val=0;
	for (it=objectVoxels.begin(); it!=objectVoxels.end(); it++) {
		pos1=this->N26->neighbor(*it,d1);
		pos2=this->N26->neighbor(*it,d1);
		val=img[*it];
		if (val== this->object) {
			if ((pos1<0) || (pos2<0)) {
				img[*it]= this->border;
				boundaryVoxels.push_back(*it);
			}
			else {
				if ((img[pos1]== this->background) || (img[pos2]== this->background)){
					img[*it]= this->border;
					boundaryVoxels.push_back(*it);
				}
			}
		}
	}
		
	return true;
}

}}

#endif
