//<LICENCE>

#ifndef _BASICPROJECTOR_H_
#define _BASICPROJECTOR_H_

#include "../base/timage.h"
#include "../base/kiplenums.h"

namespace kipl { namespace math {

template<typename T>
class BasicProjector {
public:
    BasicProjector() {}
    ~BasicProjector() {}
	static kipl::base::TImage<T, 2> project (kipl::base::TImage<T, 3> img, const kipl::base::eImagePlanes plane);
private:
	static kipl::base::TImage<T, 2> ProjectOnPlaneXY(kipl::base::TImage<T, 3> img);
	static kipl::base::TImage<T, 2> ProjectOnPlaneXZ(kipl::base::TImage<T, 3> img);
	static kipl::base::TImage<T, 2> ProjectOnPlaneYZ(kipl::base::TImage<T, 3> img);
};
}}
#include "core/basicprojector.hpp" 
#endif

