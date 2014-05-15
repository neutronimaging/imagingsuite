/***************************************************************************
 *   Copyright (C) 2005 by Anders Kaestner   *
 *   anders.kaestner@env.ethz.ch   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
				fig=NULL;
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
					
					manager->fig=NULL;
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
