

#include <cmath>

#include "../../include/base/index2coord.h"

namespace kipl { namespace base {
    Index2Coordinates::Index2Coordinates(const std::vector<size_t> &dims, size_t /*N*/) :
        nSX(dims[0]),
        nSXY(dims[0]*dims[1]),
        dInvSX(1.0/static_cast<double>(nSX)),
        dInvSXY(1.0/static_cast<double>(nSXY))
	{
		 //nSX=;
    
		// ;

		// nSXY=(dims[0]*dims[1]);
		// dInvSXY=1.0/static_cast<double>(nSXY);
	}
    bool Index2Coordinates::operator () (size_t pos, coords3D &c){
        size_t diff=0UL;
        if (pos<nSXY) {
            c.z=0;
            diff=pos;
        }
        else {
            c.z=static_cast<size_t>(pos/ nSXY);
            diff=pos % nSXY;
        }
        c.y=static_cast<size_t>(diff /nSX);
        c.x=diff % nSX;

		return true;
	}
		
    bool Index2Coordinates::operator () (size_t pos, coords2D &c)
	{
        size_t diff=0UL;

        if (pos<nSXY) {
            diff=pos;
        }
        else {
            diff=pos % nSXY;
        }

        c.y=static_cast<size_t>(diff /nSX);
        c.x=diff % nSX;

		return true;
	}

    size_t Index2Coordinates::sx()     { return nSX; }
    size_t Index2Coordinates::sxy()    { return nSXY; }
    double Index2Coordinates::invSX()  { return dInvSX; }
    double Index2Coordinates::invSXY() { return dInvSXY; }
        
}}

