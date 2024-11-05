

#ifndef __PORESIZE_H_
#define __PORESIZE_H_

#include <map>
#include <list>
#include <sstream>

#include "../base/timage.h"
#include "../morphology/morphdist.h"
#include "../base/index2coord.h"
#include "../drawing/drawing.h"
#include "../logging/logger.h"

namespace kipl { namespace porespace {

template <typename T, typename S>
void PoreSizeMap(kipl::base::TImage<T,3> &mask, kipl::base::TImage<S,3> &poremap, bool complement=false)
{
	kipl::logging::Logger logger("PoreSizeMap");
	ostringstream msg;
	logger(kipl::logging::Logger::LogMessage,"Computing distance map");

    kipl::morphology::old::EuclideanDistance(mask, poremap,complement);

	std::map<float, std::list<kipl::base::coords3D> > coords;
	S *pDist=poremap.GetDataPtr();
	short sx=poremap.Size(0);
	short sy=poremap.Size(1);
	short sz=poremap.Size(2);
	size_t i=0;

	logger(kipl::logging::Logger::LogMessage,"Initializing distance ordered processing");
	for (short z=0; z<sz; z++) {
		for (short y=0; y<sy; y++) {
			for (short x=0; x<sx; x++, i++) {
				if (pDist[i]) {
					coords[pDist[i]].push_back(kipl::base::coords3D(x,y,z));
				}
			}
		}
	}

	std::map<float, std::list<kipl::base::coords3D> >::iterator it;
	std::list<kipl::base::coords3D>::iterator cit;
	logger(kipl::logging::Logger::LogMessage,"Distance ordered processing");
	for (it=coords.begin(); it!=coords.end(); it++) {
		kipl::drawing::Sphere<S> sphere(it->first);
		msg.str("");
		msg<<"Processing distance "<<(it->first)<<" with "<<(it->second.size())<<" elements";
		logger(kipl::logging::Logger::LogMessage,msg.str());

		for (cit=it->second.begin(); cit!=it->second.end(); cit++) {
			sphere.Draw(poremap,cit->x,cit->y,cit->z,it->first);
		}
	}
}

}}

#endif
