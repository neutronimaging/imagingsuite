//
// This file is part of the i KIPL image processing tool by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//

#ifndef __IMAGE_IO_H
#define __IMAGE_IO_H

#include <KiplProcessConfig.h>
#include <base/timage.h>

kipl::base::TImage<float,3> LoadVolumeImage(KiplProcessConfig & config);

#endif
