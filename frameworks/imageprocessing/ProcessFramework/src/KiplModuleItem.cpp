//<LICENSE>


#include "stdafx.h"
#include "../include/KiplModuleItem.h"

KiplModuleItem::KiplModuleItem(std::string sharedobject, std::string modulename, kipl::interactors::InteractionBase *interactor) :
    ModuleItemBase("kiptool",sharedobject, modulename,interactor)
{
}

KiplModuleItem::~KiplModuleItem(void)
{
}

