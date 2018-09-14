//<LICENSE>

#ifndef KIPLMODULEITEM_H
#define KIPLMODULEITEM_H

#include <ModuleItemBase.h>
#include <interactors/interactionbase.h>

class KiplModuleItem : public ModuleItemBase
{
public:
    KiplModuleItem(std::string sharedobject, std::string modulename, kipl::interactors::InteractionBase *interactor=nullptr);
	~KiplModuleItem(void);
};

#endif
