#ifndef REPAIRHOLE_H
#define REPAIRHOLE_H

#include <list>
#include <limits>
#include "../base/timage.h"
#include "morphology.h"


namespace kipl {
namespace morphology {

template <class T>
void RepairHoles(kipl::base::TImage<T> &img, std::list<size_t> &holelist, kipl::morphology::MorphConnect connect);
}

}

#include "core/repairhole.hpp"
#endif // REPAIRHOLE_H
