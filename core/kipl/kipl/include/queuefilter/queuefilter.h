//<LICENCE>

#ifndef __QUEUEFITLER_H
#define __QUEUEFITLER_H

#include <image/image.h>
#include "shockworker.h"
#include "queuefiltermanager.h"
#include <string>
#include "../visualization/imgviewer.h"

namespace QueueFilter {

	
	template <class ImgType>
	class ShockFilter
	{
		public :
			ShockFilter(float Sigma, float Tau, float Lambda, int N, ShockType st=Alvarez_Mazorra_Filter) 
			{
				worker=new ShockWorker<ImgType>(Tau,Sigma,Lambda,st);
				manager=new QueueFilterManager<ImgType>(*worker);
                fig=nullptr;
				Nit=N;
			}
			
			~ShockFilter() 
			{
				delete manager;
				delete worker;
			}
			
			int operator() (Image::CImage<ImgType,3> &img)	
			{
				return (*manager)(img,Nit);
			}
			
			int ShowIterations(bool show,int N=-1) {
				if (show) {
					fig=new Visualization::CImgViewer;
					manager->fig=fig;
				}
				else {
					if (fig) 
						delete fig;
					
                    manager->fig=nullptr;
				}
					
				return N;
			}
			
			int SaveIterations(bool show, string fname=string("")) {return 0;}
		private :
			ShockWorker<ImgType> *worker;
			QueueFilterManager<ImgType> *manager;
			int Nit;
			Visualization::CImgViewer *fig;
	};
}

#endif
