#ifndef NONLOCALMEANS_H
#define NONLOCALMEANS_H

#include "../base/timage.h"

namespace akipl {

class NonLocalMeans {
    public:
        NonLocalMeans(int k, double h);

        void operator()(kipl::base::TImage<float,2> &f, kipl::base::TImage<float,2> &g);
        void operator()(kipl::base::TImage<float,3> &f, kipl::base::TImage<float,3> &g);

    protected:
        float weight(float a,float b);
        void nlm_core(float *f, float *ff, float *g, size_t N);

        float m_fWidth;
        float m_fWidthLimit;
        int m_nBoxSize;
};

}

//#include "core/nonlocalmeans.hpp"
#endif // NONLOCALMEANS_H

