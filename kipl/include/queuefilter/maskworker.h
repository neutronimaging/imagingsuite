//<LICENCE>

#ifndef __MASKWORKER_H
#define __MASKWORKER_H
#include <string>
#include <map>
#include <vector>
#include <queuefilter/basequeueworker.h>
#include <image/drawable.h>
#include <io/io_matlab.h>
using namespace std;
namespace QueueFilter {

template<class ImgType>
class MaskWorker : public BaseQueueWorker<ImgType>
{
public:
	MaskWorker(const string &maskfile, ImgType mv, ostream &os=cout);
	MaskWorker(const string &shape, vector<int> &pars,ImgType mv, ostream &os=cout);
    virtual int insert(Image::CImage<ImgType,2> &img, Image::CImage<ImgType,2> &res);
    virtual int reset() {cnt=-1; return 0;}
    virtual int size() {return 1;}
	virtual int kernelsize() {return 1;}
	virtual int setDims(const unsigned int *Dims);
	
    virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals);

protected: 
	int MakeShape();
	CImage<unsigned char,2> mask;
	bool shapeMask;
	int cnt;
	vector<int> parvec;
	ImgType maskval;
	string shapename;
};

template <class ImgType>
MaskWorker<ImgType>::MaskWorker(const string &maskfile,ImgType mv, ostream &os): BaseQueueWorker<ImgType>(os)
{
	Fileio::ReadMAT(mask,maskfile.c_str());
	shapeMask=false;
	shapename=maskfile;
	this->workername="maskworker with file";
	cnt=-1;
	maskval=mv;
}

template <class ImgType>
MaskWorker<ImgType>::MaskWorker(const string &shape, vector<int> &pars, ImgType mv,ostream &os): BaseQueueWorker<ImgType>(os)
{
	shapename=shape;
	parvec=pars;
	shapeMask=true;
	this->workername="maskworker with shape";
	cnt=-1;
	maskval=mv;
}

template <class ImgType>
int MaskWorker<ImgType>::setDims(const unsigned int *Dims)
{
	BaseQueueWorker<ImgType>::setDims(Dims);
	//memcpy(this->dims, Dims,3*sizeof(int));
	
	if (shapeMask) {
		mask.resize(Dims);
		MakeShape();
	}

	
	return 0;
}

template <class ImgType>
int MaskWorker<ImgType>::insert(Image::CImage<ImgType,2> &img, Image::CImage<ImgType,2> &res)
{
	res.resize(this->dims);
	res=0;
	int N=std::min(img.SizeY(),mask.SizeY());
	int M=std::min(img.SizeX(),mask.SizeX());
	

	ImgType *pImg, *pRes;
	unsigned char *pMask;
	for (int i=0; i<N; i++) {
		pImg=img.getLineptr(i);
		pRes=res.getLineptr(i);
		pMask=mask.getLineptr(i);
		
		for (int j=0; j<M; j++) {
			if (pMask[j])
				pRes[j]=pImg[j];
			else	
				pRes[j]=maskval;
		}
	}
	
	cnt++;
	return cnt;
}

template <class ImgType>
int MaskWorker<ImgType>::MakeShape()
{
	map<string,int> shapeMap;
	
	shapeMap["unknown"]=0;
	shapeMap["disk"]=1;
	Drawables::CDrawable<unsigned char> * pShape=NULL;
	mask=0;
	int r,x,y;
	switch (shapeMap[shapename]) {
	default:
	case 0: this->logstream<<"Unknown shape"<<endl;
			break;
	case 1: this->logstream<<"Creating a disk mask"<<flush;
			switch (parvec.size()) {
				default:
				case 0: r=std::min(this->dims[0],this->dims[1])>>1;
						x=this->dims[0]>>1;
						y=this->dims[1]>>1;
						break;
				case 1:
				case 2: r=parvec.front();
						x=this->dims[0]>>1;
						y=this->dims[1]>>1;
						break;
				case 3: r=parvec[2];
						x=parvec[0];
						y=parvec[1];
						break;
			}
			
			this->logstream<<" at ("<<x<<", "<<y<<") with radius "<<r<<endl;
			pShape=new Drawables::CFillCircle<unsigned char>(x,y,r,1);
			break;
	}
	
	if (pShape) {
		mask.draw(*pShape);
		delete pShape;
	}
	
	return 0;
}

template <class ImgType>
int MaskWorker<ImgType>::GetWorkerParameters(vector<string> & parnames, vector<double> & parvals)
{
	parnames.clear();
	parvals.clear();
	parnames.push_back(shapename); parvals.push_back(0);
	ostringstream varname;
	for (int i=0; i<parvec.size(); i++) {
		varname.str("");
		
		varname<<"par_"<<i<<ends;
		parnames.push_back(varname.str()); parvals.push_back(parvec[i]);
	}

	return 0;
}

}
#endif

