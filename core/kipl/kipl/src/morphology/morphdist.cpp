//<LICENCE>

#include <cstdlib>
#include <cfloat>

#include "../../include/base/timage.h"
//#include "image/imagemath.h"
#include "../../include/morphology/morphdist.h"

using namespace std;

namespace kipl { namespace morphology {

    CMetricBase::CMetricBase(string _name) :
    		ndata(0),
    		NDim(0),
            name(_name),
    		edge_dist(0)
    {
         this->w_fwd=nullptr;
         this->w_bck=nullptr;
         this->i_fwd=nullptr;
         this->i_bck=nullptr;

         size=0;
		 index_start=0;

    }

    float CMetricBase::forward(float *p)
    {
        float m=10000.0f;
        float s;
        int i;

        for (i=1; i<size; i++) {
            s=p[-(this->i_fwd[i])]+ this->w_fwd[i];
            m=m<s ? m : s;
        }

       return m;
    }

    float CMetricBase::backward(float *p)
    {
        float m=*p;
        float s=0.0f;

        int i;
        for (i=1; i<size; i++)  {
			s=p[(this->i_fwd[i])] + this->w_fwd[i];
            m=m < s ? m : s;
        }

        return m;
    }

	int CMetricBase::start()
	{
		return index_start;
	}

	int CMetricBase::initialize(size_t const * const d)
	{
		memcpy(dims,d,sizeof(int)*3);
		ndata=d[0];
		for (int i=1; i<NDim; i++) ndata*=d[i];

		return 0;
	}

     CMetricBase::~CMetricBase()
     {
         if (this->i_fwd!=nullptr)
             delete [] this->i_fwd;

         if (this->i_bck!=nullptr)
             delete [] this->i_bck;

         if (this->w_fwd!=nullptr)
             delete [] this->w_fwd;

         if (this->w_bck)
             delete [] this->w_bck;

     }

     std::string CMetricBase::getName()
     {
         return name;
     }

      CMetric26conn::CMetric26conn() :
          CMetricBase("CMetric26conn")
      {
    	  NDim=3;
    	  edge_dist=1;
    	  index_start=1;
    	  size=14;
    	  this->i_fwd=new int[size];
          this->w_fwd=new float[size];


      }
      int CMetric26conn::initialize(size_t const * const dim)
      {
          CMetricBase::initialize(dim);

          float w[14]={0.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};
          memcpy(this->w_fwd,w,size*sizeof(float));

          int sx=dims[0], sxy=dims[0]*dims[1];

          int index[14]={0, 1, sx-1, sx, sx+1, // Central
        		  sxy-sx-1, sxy-sx, sxy-sx+1,
        		     sxy-1,    sxy,    sxy+1,
        		  sxy+sx-1, sxy+sx, sxy+sx+1
          };

          memcpy(this->i_fwd,index,size*sizeof(int));

          return 1;

      }
      CMetric26conn::~CMetric26conn()
      {

      }


      CMetricSvensson::CMetricSvensson() :
          CMetricBase("CMetricSvensson")
      {
          i_fwd=new int[50];
          w_fwd=new float[50];
          size=50;
	      edge_dist=3;
          index_start=2;
		  NDim=3;

      }

      int CMetricSvensson::initialize(size_t const * const dim)
      {
          CMetricBase::initialize(dim);

          float v=0.0f;
          float a=1.0f;
          float b=sqrt(2.0f);
          float c=sqrt(3.0f);
          float d=sqrt(5.0f);
          float e=sqrt(6.0f);
          float f=3.0f;

          float w[50]={v,a,
                     d,b,a,b,d,
                     d,d, // z=0
                     f,e,d,e,f,
                     e,c,b,c,e,
                     d,b,a,b,d,
                     e,c,b,c,e,
                     f,e,d,e,f, // z=1
                     f,f,
                     f,e,d,e,f,
                     d,d,
                     f,e,d,e,f,
                     f,f}; // z=2
          memcpy(this->w_fwd,w,size*sizeof(float));

          int sx=dims[0], sxy=dims[0]*dims[1];

          int index[50]={0, 1,
                         sx-2,sx-1,sx,sx+1,sx+2,
                         2*sx-1,2*sx+1, // z=0
                         sxy-2*sx-2, sxy-2*sx-1,sxy-2*sx,sxy-2*sx+1, sxy-2*sx+2,
                         sxy-sx-2, sxy-sx-1,sxy-sx,sxy-sx+1, sxy-sx+2,
                         sxy-2, sxy-1,sxy,sxy+1, sxy+2,
                         sxy+sx-2, sxy+sx-1,sxy+sx,sxy+sx+1, sxy+sx+2,
                         sxy+2*sx-2, sxy+2*sx-1,sxy+2*sx,sxy+2*sx+1, 2*sxy+sx+2, // z=1
                         2*sxy-2*sx-1, 2*sxy-2*sx+1,
                         2*sxy-sx-2, 2*sxy-sx-1,2*sxy-sx,2*sxy-sx+1, 2*sxy-sx+2,
                         2*sxy-1,2*sxy+1,
                         2*sxy+sx-2, 2*sxy+sx-1,2*sxy+sx,2*sxy+sx+1, 2*sxy+sx+2,
                         2*sxy+2*sx-1,2*sxy+2*sx+1}; // z=1

          memcpy(this->i_fwd,index,size*sizeof(int));
		  return 0;
      }

      CMetricSvensson::~CMetricSvensson()
      {}


    CMetric4connect::CMetric4connect() :
        CMetricBase("CMetric4connect")
	{
		i_fwd=new int[3];
		w_fwd=new float[3];
		size=3;
		index_start=1;
		NDim=2;

	}

	int CMetric4connect::initialize(size_t const * const d)
	{
		CMetricBase::initialize(d);
		float w[3]={0.0f,1.0f,1.0f};
		memcpy(w_fwd,w,size*sizeof(float));
        int index[3]={0,1,static_cast<int>(dims[0])};
		memcpy(i_fwd,index,size*sizeof(int));
		return 0;
	}

	CMetric4connect::~CMetric4connect()
	{ }

    CMetric8connect::CMetric8connect() :
        CMetricBase("CMetric8connect")
	{
		i_fwd=new int[5];
		w_fwd=new float[5];
		size=5;
		index_start=1;
		NDim=2;

	}

	int CMetric8connect::initialize(size_t const * const d)
	{
		CMetricBase::initialize(d);
		float w[5]={0.0f,1.0f,1.0f,1.0f,1.0f};
		memcpy(w_fwd,w,size*sizeof(float));
        int index[5]={0,1,static_cast<int>(dims[0]),static_cast<int>(dims[0]-1),static_cast<int>(dims[0]+1)};
		memcpy(i_fwd,index,size*sizeof(int));
		return 0;
	}

	CMetric8connect::~CMetric8connect()
	{ }

    CMetric3x3w::CMetric3x3w() :
        CMetricBase("CMetric3x3w")
	{
		i_fwd=new int[5];
		w_fwd=new float[5];
		size=5;
		index_start=1;
		NDim=2;

	}

	int CMetric3x3w::initialize(size_t const * const d)
	{
		CMetricBase::initialize(d);
		float w[5]={0.0f,1.0f,1.0f,sqrt(2.0f),sqrt(2.0f)};
		memcpy(w_fwd,w,size*sizeof(float));
        int index[5]={0,1,static_cast<int>(dims[0]),static_cast<int>(dims[0]-1),static_cast<int>(dims[0]+1)};
		memcpy(i_fwd,index,size*sizeof(int));
		return 0;
	}

	CMetric3x3w::~CMetric3x3w()
	{ }



}}

std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &os,kipl::morphology::CMetricBase &mb)
{
    return os<<mb.getName();
}

namespace kipl { namespace morphology { namespace old {
CMetricBase::CMetricBase(string _name) :
        ndata(0),
        NDim(0),
        name(_name),
        edge_dist(0)
{
     this->w_fwd=nullptr;
     this->w_bck=nullptr;
     this->i_fwd=nullptr;
     this->i_bck=nullptr;

     size=0;
     index_start=0;

}

float CMetricBase::forward(float *p)
{
    float m=10000.0f;
    float s;
    int i;

    for (i=1; i<size; i++) {
        s=p[-(this->i_fwd[i])]+ this->w_fwd[i];
        m=m<s ? m : s;
    }

   return m;
}

float CMetricBase::backward(float *p)
{
    float m=*p;
    float s=0.0f;

    int i;
    for (i=1; i<size; i++)  {
        s=p[(this->i_fwd[i])] + this->w_fwd[i];
        m=m < s ? m : s;
    }

    return m;
}

int CMetricBase::start()
{
    return index_start;
}

int CMetricBase::initialize(size_t const * const d)
{
    memcpy(dims,d,sizeof(int)*3);
    ndata=d[0];
    for (int i=1; i<NDim; i++) ndata*=d[i];

    return 0;
}

 CMetricBase::~CMetricBase()
 {
     if (this->i_fwd!=nullptr)
         delete [] this->i_fwd;

     if (this->i_bck!=nullptr)
         delete [] this->i_bck;

     if (this->w_fwd!=nullptr)
         delete [] this->w_fwd;

     if (this->w_bck)
         delete [] this->w_bck;

 }

 std::string CMetricBase::getName()
 {
     return name;
 }

  CMetric26conn::CMetric26conn() :
      CMetricBase("CMetric26conn")
  {
      NDim=3;
      edge_dist=1;
      index_start=1;
      size=14;
      this->i_fwd=new int[size];
      this->w_fwd=new float[size];


  }
  int CMetric26conn::initialize(size_t const * const dim)
  {
      CMetricBase::initialize(dim);

      float w[14]={0.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};
      memcpy(this->w_fwd,w,size*sizeof(float));

      int sx=dims[0], sxy=dims[0]*dims[1];

      int index[14]={0, 1, sx-1, sx, sx+1, // Central
              sxy-sx-1, sxy-sx, sxy-sx+1,
                 sxy-1,    sxy,    sxy+1,
              sxy+sx-1, sxy+sx, sxy+sx+1
      };

      memcpy(this->i_fwd,index,size*sizeof(int));

      return 1;

  }
  CMetric26conn::~CMetric26conn()
  {

  }


  CMetricSvensson::CMetricSvensson() :
      CMetricBase("CMetricSvensson")
  {
      i_fwd=new int[50];
      w_fwd=new float[50];
      size=50;
      edge_dist=3;
      index_start=2;
      NDim=3;

  }

  int CMetricSvensson::initialize(size_t const * const dim)
  {
      CMetricBase::initialize(dim);

      float v=0.0f;
      float a=1.0f;
      float b=sqrt(2.0f);
      float c=sqrt(3.0f);
      float d=sqrt(5.0f);
      float e=sqrt(6.0f);
      float f=3.0f;

      float w[50]={v,a,
                 d,b,a,b,d,
                 d,d, // z=0
                 f,e,d,e,f,
                 e,c,b,c,e,
                 d,b,a,b,d,
                 e,c,b,c,e,
                 f,e,d,e,f, // z=1
                 f,f,
                 f,e,d,e,f,
                 d,d,
                 f,e,d,e,f,
                 f,f}; // z=2
      memcpy(this->w_fwd,w,size*sizeof(float));

      int sx=dims[0], sxy=dims[0]*dims[1];

      int index[50]={0, 1,
                     sx-2,sx-1,sx,sx+1,sx+2,
                     2*sx-1,2*sx+1, // z=0
                     sxy-2*sx-2, sxy-2*sx-1,sxy-2*sx,sxy-2*sx+1, sxy-2*sx+2,
                     sxy-sx-2, sxy-sx-1,sxy-sx,sxy-sx+1, sxy-sx+2,
                     sxy-2, sxy-1,sxy,sxy+1, sxy+2,
                     sxy+sx-2, sxy+sx-1,sxy+sx,sxy+sx+1, sxy+sx+2,
                     sxy+2*sx-2, sxy+2*sx-1,sxy+2*sx,sxy+2*sx+1, 2*sxy+sx+2, // z=1
                     2*sxy-2*sx-1, 2*sxy-2*sx+1,
                     2*sxy-sx-2, 2*sxy-sx-1,2*sxy-sx,2*sxy-sx+1, 2*sxy-sx+2,
                     2*sxy-1,2*sxy+1,
                     2*sxy+sx-2, 2*sxy+sx-1,2*sxy+sx,2*sxy+sx+1, 2*sxy+sx+2,
                     2*sxy+2*sx-1,2*sxy+2*sx+1}; // z=1

      memcpy(this->i_fwd,index,size*sizeof(int));
      return 0;
  }

  CMetricSvensson::~CMetricSvensson()
  {}


CMetric4connect::CMetric4connect() :
    CMetricBase("CMetric4connect")
{
    i_fwd=new int[3];
    w_fwd=new float[3];
    size=3;
    index_start=1;
    NDim=2;

}

int CMetric4connect::initialize(size_t const * const d)
{
    CMetricBase::initialize(d);
    float w[3]={0.0f,1.0f,1.0f};
    memcpy(w_fwd,w,size*sizeof(float));
    int index[3]={0,1,static_cast<int>(dims[0])};
    memcpy(i_fwd,index,size*sizeof(int));
    return 0;
}

CMetric4connect::~CMetric4connect()
{ }

CMetric8connect::CMetric8connect() :
    CMetricBase("CMetric8connect")
{
    i_fwd=new int[5];
    w_fwd=new float[5];
    size=5;
    index_start=1;
    NDim=2;

}

int CMetric8connect::initialize(size_t const * const d)
{
    CMetricBase::initialize(d);
    float w[5]={0.0f,1.0f,1.0f,1.0f,1.0f};
    memcpy(w_fwd,w,size*sizeof(float));
    int index[5]={0,1,static_cast<int>(dims[0]),static_cast<int>(dims[0]-1),static_cast<int>(dims[0]+1)};
    memcpy(i_fwd,index,size*sizeof(int));
    return 0;
}

CMetric8connect::~CMetric8connect()
{ }

CMetric3x3w::CMetric3x3w() :
    CMetricBase("CMetric3x3w")
{
    i_fwd=new int[5];
    w_fwd=new float[5];
    size=5;
    index_start=1;
    NDim=2;

}

int CMetric3x3w::initialize(size_t const * const d)
{
    CMetricBase::initialize(d);
    float w[5]={0.0f,1.0f,1.0f,sqrt(2.0f),sqrt(2.0f)};
    memcpy(w_fwd,w,size*sizeof(float));
    int index[5]={0,1,static_cast<int>(dims[0]),static_cast<int>(dims[0]-1),static_cast<int>(dims[0]+1)};
    memcpy(i_fwd,index,size*sizeof(int));
    return 0;
}

CMetric3x3w::~CMetric3x3w()
{ }


}}}

