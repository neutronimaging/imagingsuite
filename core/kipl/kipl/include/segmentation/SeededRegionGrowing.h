

#ifndef __SEEDEDREGIONGROWING_H
#define __SEEDEDREGIONGROWING_H

#include <list>
#include <vector>

#include "../base/timage.h"
#include "../morphology/morphology.h"
//#include <morphology/label.h>

namespace akipl { namespace segmentation {

  template<typename ImgType, typename SegType ,size_t NDims>
  class GrowingRegion {
    kipl::base::TImage<ImgType,NDims> *m_pImg;
    kipl::base::TImage<SegType,NDims> *m_pSeg;
    kipl::morphology::CNeighborhood *m_pNeighborhood;
  public:
    GrowingRegion();
    GrowingRegion(const GrowingRegion<ImgType,SegType,NDims> &a);
    const GrowingRegion & operator=(const GrowingRegion<ImgType,SegType,NDims> &a);
    int SetRefImage(kipl::base::TImage<ImgType,NDims> *img, kipl::base::TImage<SegType,NDims> *seg,kipl::morphology::CNeighborhood *pNeighborhood);

    int Grow();
    int size() {return m_Ndata;}
    int Add(size_t p);
    const map<SegType,int> & GetTouchList();
  private:
    void copy(const GrowingRegion<ImgType,SegType,NDims> &a);
    bool IsEdge(size_t p);
    long m_nSeed;
    int m_Ndata;
    double m_dSum;
    double m_dSum2;
    std::list<size_t> m_BorderList;
    map<SegType,int> m_TouchList;
  };

  template <typename ImgType, typename SegType, size_t NDims>
  class SeededRegionGrowing{
  public:
    int operator()(kipl::base::TImage<ImgType,NDims> &img, kipl::base::TImage<SegType,NDims> &seg);
    SeededRegionGrowing() {m_pNeighborhood=nullptr;}
    ~SeededRegionGrowing() {if (m_pNeighborhood) delete m_pNeighborhood;}
  private:
    int PrepareRegions(kipl::base::TImage<ImgType,NDims> &img, kipl::base::TImage<SegType,NDims> &seg);
    int Grow();
    map<int,GrowingRegion<ImgType,SegType,NDims> > m_regionList;
    kipl::morphology::CNeighborhood *m_pNeighborhood;
  };


template<typename ImgType, typename SegType ,size_t NDims>
GrowingRegion<ImgType,SegType,NDims>::GrowingRegion()
{
  m_nSeed=0;
  m_Ndata=0;
  m_dSum=0.0;
  m_dSum2=0.0;
}

template<typename ImgType, typename SegType ,size_t NDims>
GrowingRegion<ImgType,SegType,NDims>::GrowingRegion(const GrowingRegion<ImgType,SegType,NDims> &a)
{

}

template<typename ImgType, typename SegType ,size_t NDims>
const GrowingRegion<ImgType,SegType,NDims> & GrowingRegion<ImgType,SegType,NDims>::operator=(const GrowingRegion<ImgType,SegType,NDims> &a)
{

}

template<typename ImgType, typename SegType ,size_t NDims>
void GrowingRegion<ImgType,SegType,NDims>::copy(const GrowingRegion<ImgType,SegType,NDims> &a)
{
  m_nSeed=a.m_nSeed;
  m_Ndata=a.m_Ndata;
  m_dSum=a.m_dSum;
  m_dSum2=a.m_dSum2;
  m_BorderList.clear();
  std::copy(a.m_BorderList.begin(),a.m_BorderList.end(),std::back_inserter<long>(m_BorderList));
  m_TouchList;
}

template<typename ImgType, typename SegType ,size_t NDims>
int GrowingRegion<ImgType,SegType,NDims>::Grow()
{
  ImgType *pImg=m_pImg->getDataptr();
  SegType *pSeg=m_pSeg->getDataptr();
  SegType label=pSeg[m_nSeed];
  std::list<size_t> newBorder;

  ImgType val;
  long p,pos;
  int i;
  int cnt=0;
  const int N=this->m_pNeighborhood->N();
  while (!m_BorderList.empty()){
    p=m_BorderList.front();
    m_BorderList.pop_front();
    for (i=0; i<N ; i++)
      if ((pos=this->m_pNeighborhood->neighbor(p,i))>-1) {
        if (!pSeg[pos]) {
          pSeg[pos]=label;
          newBorder.push_back(pos);
          val=pImg[pos];
          m_dSum+=val;
          m_dSum2+=val*val;
          m_Ndata++;
          cnt++;
        }
        else if (pSeg[pos]!=label) {
          m_TouchList[pSeg[pos]++];
        }
      }
  }
  while (!newBorder.empty()) {
    p=newBorder.front();
    newBorder.pop_front();
    if (IsEdge(p))
      m_BorderList.push_back(p);
  }
  return cnt;
}

template<typename ImgType, typename SegType ,size_t NDims>
int GrowingRegion<ImgType,SegType,NDims>::Add(size_t p)
{
  m_nSeed=p;
  if (IsEdge(p))
    m_BorderList.push_back(p);

  m_Ndata++;
  ImgType val=m_pImg->operator[](p);
  m_dSum+=val;
  m_dSum2+=val*val;

  return 0;
}

template<typename ImgType, typename SegType ,size_t NDims>
bool GrowingRegion<ImgType,SegType,NDims>::IsEdge(size_t p)
{
  int N=m_pNeighborhood->N();
  long pos=0L;
  bool inside=true;
  for (int i=0; i<N; i++) {
    if ((pos=m_pNeighborhood->neighbor(p,i))>-1) {
      inside&=((m_pSeg->operator [](pos))!=0);
    }
  }
  return !inside;
}

template<typename ImgType, typename SegType ,size_t NDims>
const map<SegType,int> & GrowingRegion<ImgType,SegType,NDims>::GetTouchList()
{
  return m_TouchList;
}


template<typename ImgType, typename SegType ,size_t NDims>
int GrowingRegion<ImgType,SegType,NDims>::SetRefImage(kipl::base::TImage<ImgType,NDims> *pImg, 
													  kipl::base::TImage<SegType,NDims> *pSeg, 
                                                      kipl::morphology::CNeighborhood *pNeighborhood)
{
  m_pSeg=pSeg;
  m_pImg=pImg;
  m_pNeighborhood=pNeighborhood;;
  return 0;
}

template <typename ImgType, typename SegType, size_t NDims>
int SeededRegionGrowing<ImgType,SegType,NDims>::operator()(kipl::base::TImage<ImgType,NDims> &img, 
		kipl::base::TImage<SegType,NDims> &seg)
{
    PrepareRegions(img,seg);
    while (Grow()!=0) {
      cout<<"."<<flush;
    }
    cout<<endl;

    // MergeRegions
    return 0;
}

template <typename ImgType, typename SegType, size_t NDims>
int SeededRegionGrowing<ImgType,SegType,NDims>::PrepareRegions(kipl::base::TImage<ImgType,NDims> &img, 
		kipl::base::TImage<SegType,NDims> &seg)
{
  kipl::base::TImage<int,NDims> lbl;
  std::vector<kipl::morphology::CLabelledItem > itemlist;
  int Nlbl=kipl::morphology::LabelImage(seg,lbl,kipl::morphology::conn26);
  m_pNeighborhood=new kipl::morphology::CNeighborhood(img.Dims(), NDims,kipl::morphology::conn26);
  for (int l=1; l<=Nlbl; l++) {
    m_regionList[l].SetRefImage(&img,&seg, m_pNeighborhood);
  }
  SegType *pSeg=seg.getDataptr();
  int *pLbl=lbl.getDataptr();
  for (size_t i=0L; i<lbl.Size(); i++) {
    pSeg[i]=pLbl[i];
    if (pSeg[i]) 
      m_regionList[pSeg[i]].Add(i);
  }

  return 0;
}

template <typename ImgType, typename SegType, size_t NDims>
int SeededRegionGrowing<ImgType,SegType,NDims>::Grow()
{
  typename std::map<int, GrowingRegion<ImgType,SegType,NDims> >::iterator it;
  int sum=0;
  for (it=m_regionList.begin(); it!=m_regionList.end() ; ++it) {
    cout<<(it->first)<<" "<<flush;
    sum+=it->second.Grow();
  }
  cout<<" sum="<<sum<<endl;

  return sum;
}


}}

#endif
