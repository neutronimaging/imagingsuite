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


#include "stdafx.h"
#include "../include/KiplModuleItem.h"

KiplModuleItem::KiplModuleItem(std::string sharedobject, std::string modulename) :
	ModuleItemBase("kiptool",sharedobject, modulename)
{
}

KiplModuleItem::~KiplModuleItem(void)
{
}
