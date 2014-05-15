#include "../../include/base/index2coord.h"
#include <cmath>

namespace kipl { namespace base {
	Index2Coordinates::Index2Coordinates(size_t const * const dims, size_t N)
	{
		nSX=dims[0];
		dInvSX=1.0/static_cast<double>(nSX);

		if (N==3) {
			nSXY=(dims[0]*dims[1]);
			dInvSXY=1.0/static_cast<double>(nSXY);
		}

	}
	bool Index2Coordinates::operator () (size_t pos, coords3D *c){
		c->z=static_cast<size_t>(floor(pos * dInvSXY));
		size_t diff=pos - nSX*c->z;
		c->y=static_cast<size_t>(floor(0.5+diff*dInvSX));
		c->x=diff-c->y;

		return true;
	}
		
	bool Index2Coordinates::operator () (size_t pos, coords2D *c)
	{

		return true;
	}
}}

