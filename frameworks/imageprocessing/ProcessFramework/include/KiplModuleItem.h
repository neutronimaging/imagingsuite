//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2008-11-11 21:09:49 +0100 (Di, 11 Nov 2008) $
// $Rev: 13 $
//

#ifndef __KIPLMODULEITEM_H
#define __KIPLMODULEITEM_H

#include <ModuleItemBase.h>
#include "KiplProcessModuleBase.h"

class KiplModuleItem : public ModuleItemBase
{
public:
	KiplModuleItem(std::string sharedobject, std::string modulename);
	~KiplModuleItem(void);

    /// Gets a reference to the processing module the held by the item.
    KiplProcessModuleBase *GetModule() {return dynamic_cast<KiplProcessModuleBase *>(ModuleItemBase::GetModule());}

protected:
    KiplProcessModuleBase *m_Module;

};

#endif
