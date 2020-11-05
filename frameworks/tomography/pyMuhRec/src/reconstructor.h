//LICENSE
#ifndef RECONSTRUCTOR_H
#define RECONSTRUCTOR_H

#include <BackProjectorModuleBase.h>

enum eBackProjectors
{
    bpMultiProj,
    bpMultiProjParallel,
    bpNearestNeighbor
};

class Reconstructor
{
public:
  Reconstructor(eBackProjectors bp);
  ~Reconstructor();

  BackProjectorModuleBase *backProjector;

private:
  void createBP(eBackProjectors bp);
};

#endif
