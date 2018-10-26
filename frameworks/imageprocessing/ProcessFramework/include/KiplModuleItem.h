//<LICENSE>

#ifndef KIPLMODULEITEM_H
#define KIPLMODULEITEM_H

#include <ModuleItemBase.h>
#include "KiplProcessModuleBase.h"
#include <interactors/interactionbase.h>


class KiplModuleItem : public ModuleItemBase
{
public:

    KiplModuleItem(std::string sharedobject, std::string modulename, kipl::interactors::InteractionBase *interactor=nullptr);
	~KiplModuleItem(void);

    /// Gets a reference to the processing module the held by the item.
    KiplProcessModuleBase *GetModule() {return dynamic_cast<KiplProcessModuleBase *>(ModuleItemBase::GetModule());}

protected:
    KiplProcessModuleBase *m_Module;

};

#endif
