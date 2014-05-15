#ifndef __LAMBDAEST_H
#define __LAMBDAEST_H

#include <base/timage.h>
//#include "imagestats.h"
//#include "../misc/stlvecio.h"
//#include "../misc/sortalg.h"
//#include "../math/statistics.h"
//#include <image/drawable.h>
#include <vector>
#include <base/thistogram.h>
#include <base/textractor.h>
#include <math/median.h>

using namespace std;

/// Namespace for scale space operations and filters
namespace akipl { namespace scalespace {
    /// Base class for contrast parameter estimators, provides a constant value of lambda
	template<class ImgType,int NDim>
	class LambdaEstBase {
	public:
		LambdaEstBase() {call_cnt=0;update_frequency=1; update_data=-1; lambda=0; mask=NULL; newROI=false;};
		float operator()(kipl::base::TImage<float,NDim> &img) {
			if (!(call_cnt%update_frequency)) {
//
//				if ((ROIcoord[0]!=-1) && (newROI)) {
//					newROI=false;
//					if (mask) delete mask;
//					mask=new kipl::base::TImage<char,2>(img.getDimsptr());
//					switch (ROIcoord.size()) {
//					case 3: {
//						Drawables::CFillCircle<char> circ(ROIcoord[1],ROIcoord[2],ROIcoord[0],1);
//						mask->draw(circ);
//						break;}
//					case 1: {
//						Drawables::CFillCircle<char> circ(img.SizeX()>>1,img.SizeY()>>1,ROIcoord[0],1);
//						mask->draw(circ);
//						break;
//					}
//					}
//
//				}
				update(img);
				cout<<" lambda="<<lambda<<endl;
			}
			call_cnt++;
			return lambda;
		}
		virtual int SetLambda(float l) {lambda=l; return 1;}
		virtual int SetLambda(vector<float> & l) {lambda_vec=l; return 1;}
		int SetUpdateFocus(int n) {update_data=n; return n;}
		int SetUpdateFrequency(int n) {update_frequency=n; return n;}
		void SetUpdateROI(vector<int> &coords) {
			if (coords.size()) {
				ROIcoord=coords;
				newROI=true;
			}

		};
		virtual ~LambdaEstBase() {if (mask) delete mask;}

	protected:
		virtual float update(kipl::base::TImage<float,NDim> &img) {
            if (call_cnt/update_frequency<static_cast<int>(lambda_vec.size()))
				lambda=lambda_vec[call_cnt/update_frequency];
			else
				lambda=lambda_vec[lambda_vec.size()-1];
		
			return lambda;
		}
		int update_data;
		float lambda;
		vector<float> lambda_vec;
		kipl::base::TImage<char,2> *mask;
	private:
		int update_frequency;
		int call_cnt;
			
		vector<int> ROIcoord;
		bool newROI;
		
	};

    /// Contrast parameter estimator according to Black et al. (Robust statistic)
	template<class ImgType, int NDim>
	class LambdaEst_Black : public LambdaEstBase<ImgType,NDim>
	{
	public:
		LambdaEst_Black() {scope=true;}
		bool scope;	// true=global, false=local;
	protected:
		float update(kipl::base::TImage<float, NDim> &img);
        /// Change the access, method SetLambda is obsolete for Black
		int SetLambda(float l) {return int(l);}
		int SetLambda(vector<float> &l) {return 1;}

	};

    /// Contrast parameter estimator according to Perona-Malik (percentile of histogram)
	template<class ImgType, int NDim>
	class LambdaEst_PeronaMalik : public LambdaEstBase<ImgType,NDim>
	{
	public:
		LambdaEst_PeronaMalik() :
			percentile(0.95f),
			HistLength(2048),
			histo(HistLength,0,0)
		{


			/*if (this->mask)
				histo.setMask(this->mask);*/
		}
		void SetPercentile(float p) {if ((p>=0) && (p<=1)) percentile=p;}
		void SetHistogramLength(int N) {if (N>0) HistLength=N;}
	protected:
		float update(kipl::base::TImage<float, NDim> &img);
		float percentile;
        // Change the access, method SetLambda is obsolete for Perona Malik
		int SetLambda(float l) {return 1;}
		int SetLambda(vector<float> &l) {return 1;}
		int HistLength;
		kipl::base::THistogram<float> histo;
	};

	template<class ImgType, int NDim>
	float LambdaEst_Black<ImgType,NDim>::update(kipl::base::TImage<float,NDim> &img)
	{
		kipl::base::TImage<float,2> tmp;
		float *pTmp;

//		img.ExtractSlice(tmp,plane_XY,img.getDimsptr()[2]/2);
		tmp= kipl::base::ExtractSlice(img,img.Size(2)/2,kipl::base::ImagePlaneXY);
		
		pTmp=tmp.GetDataPtr();

		for (size_t i=0; i<tmp.Size(); i++)
			pTmp[i]=abs(pTmp[i]);

		//float med=Math::median_quick_select(pTmp,tmp.N());
		
		//cout<<"Updating Lambda with Blacks method using 2D data"
		//float med=Imagemath::Median(tmp,mask);
		
		cout<<"Updating Lambda with Blacks method using 3D data"<<endl;
		float med=0.0f;
		kipl::math::median(img.GetDataPtr(),img.Size(),&med);
		
		for (size_t i=0; i<tmp.Size(); i++)
			pTmp[i]=abs(pTmp[i]-med);

		//lambda=1.4826*Math::median_quick_select(pTmp,tmp.N());
		kipl::math::median(tmp.GetDataPtr(),tmp.Size(),&med);
		this->lambda=1.4826*med;
		return this->lambda;
	}

	template<class ImgType, int NDim>
	float LambdaEst_PeronaMalik<ImgType,NDim>::update(kipl::base::TImage<float,NDim> &img)
	{
		vector<long> hist;
		vector<float> interval;
		kipl::base::TImage<float,2> tmp;

		//img.ExtractSlice(tmp,plane_XY,img.getDimsptr()[2]/2);
		tmp= kipl::base::ExtractSlice(img,img.Size(2)/2,kipl::base::ImagePlaneXY);

		int Npercentile=int(img.Size()*percentile);
		int sum=0,i=0;
		vector<long>::iterator histIt;
		vector<float>::iterator intIt;
		do {
			if (i)
				// reduce dynamics
				histo.SetInterval(HistLength,interval[0],interval[1]);
			else
				histo.SetInterval(HistLength); // Reset to full dynamics

			histo(tmp,hist,interval);
			
			sum=0;
			for (intIt=interval.begin(), histIt=hist.begin(); 
					histIt!=hist.end(); 
					histIt++, intIt++) {
				sum+=*histIt;
				if (sum>Npercentile) 
					break;
			}
			
			cout<<'.'<<flush;
			i++;
		} while ((intIt==interval.begin()) && (i<10));
		cout<<endl;
	
		if (intIt==interval.begin()) {
		//if (!lambda) {
		//	WriteMAT(hist,"c:\\tmp\\hist.mat","lhist");
		//	WriteMAT(interval,"c:\\tmp\\interv.mat","linterv");
			cerr<<endl<<"LambdaEst_PeronaMalik: bad histogram, sum:"
				<<sum<<" Npercentile:"<<Npercentile<<endl;
		
			exit(1000);
		}
		this->lambda=*(--intIt);
		return this->lambda;
	}
}}

#endif
