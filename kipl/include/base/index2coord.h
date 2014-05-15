#ifndef __INDEX2COORD_H	
#define __INDEX2COORD_H	

#include <iostream>

namespace kipl { namespace base {

	struct coords3D {
		coords3D(short xx,short yy, short zz) : x(xx), y(yy), z(zz) {}
		coords3D() : x(0), y(0), z(0), reserved(0) {}
		coords3D(const coords3D &c) : x(c.x), y(c.y), z(c.z), reserved(c.reserved) {}
		const coords3D & operator=(const coords3D & c) {
			x=c.x; y=c.y; z=c.z; reserved=c.reserved;
			return *this;
		}

		short x;
		short y;
		short z;
		short reserved;
		bool operator==(const coords3D &c) {
			return (c.x==x) && (c.y==y) && (c.z==z);
		}
		bool operator!=(const coords3D &c) {
			return !(operator==(c));
		}
	};

	struct coords2D {
		size_t x;
		size_t y;

		bool operator==(coords2D &c) {
			return (c.x==x) && (c.y==y);
		}
		bool operator!=(coords2D &c) {
			return !(c==*this);
		}
	};

	class Index2Coordinates {
	public:
		Index2Coordinates(size_t const * const dims, size_t N);
		bool operator () (size_t pos, coords3D *c);
		bool operator () (size_t pos, coords2D *c);
	private:
		size_t nSX;
		size_t nSXY;
		double dInvSX;
		double dInvSXY;
		
	}; 
}}
#endif

