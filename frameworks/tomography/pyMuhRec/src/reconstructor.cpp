//LICENSE

#include "reconstructor.h"
#include <MultiProjBP.h>
#include <MultiProjBPparallel.h>
#include <NNMultiProjBP.h>
#include <fdkbp_single.h>
#include <fdkbp.h>

Reconstructor::Reconstructor(eBackProjectors bp) :
    backProjector(nullptr)
{
    createBP(bp);
}

Reconstructor::~Reconstructor()
{
    if (backProjector!=nullptr)
        delete backProjector;
}

void Reconstructor::createBP(eBackProjectors bp)
{
    if (backProjector!=nullptr)
        delete backProjector;

    switch (bp)
    {
    case bpMultiProj:
        backProjector = new MultiProjectionBP(nullptr);
        break;

    case bpMultiProjParallel:
        backProjector = new MultiProjectionBPparallel(nullptr);
        break;

    case bpNearestNeighbor:
        backProjector = new NearestNeighborBP(nullptr);
        break;
    case bpFDKSingle :
        backProjector = new FDKbp_single(nullptr);
        break;
    case bpFDKDouble :
        backProjector = new FDKbp(nullptr);
        break;
    }
}
