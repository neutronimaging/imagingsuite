//<LICENSE>

#ifndef __IMAGE_IO_H
#define __IMAGE_IO_H

#include <KiplProcessConfig.h>
#include <base/timage.h>
#include <interactors/interactionbase.h>

kipl::base::TImage<float,3> LoadVolumeImage(KiplProcessConfig & config, kipl::interactors::InteractionBase *interactor=nullptr);

#endif
