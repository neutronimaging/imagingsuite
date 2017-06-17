//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $File$
// $Date$
// $Rev$
// $Id$
//

#ifndef __MODULEITEM_H
#define __MODULEITEM_H
#include "ReconFramework_global.h"
//#include "stdafx.h"

#include <string>

#include <ModuleItemBase.h>

#include <logging/logger.h>
#include <interactors/interactionbase.h>

#include "PreprocModuleBase.h"
#include "BackProjectorModuleBase.h"

#ifdef _MSC_VER // Shared object specific for msvc
typedef void * (__cdecl *BP_FACTORY)(const char *, const char *, void *);
#else // Shared object specific for gcc
typedef void * (*BP_FACTORY)(const char *, const char *, void *) ;
#endif

/// Implementation of the preprocess module items. The instances are the elements of the preprocessing list.
class RECONFRAMEWORKSHARED_EXPORT ModuleItem : public ModuleItemBase
{
public:
    /// Loads the module from the shared object library and perpares it for execution.
    /// \param application the name of the application. Needed to aviod loading modules from a different application.
    /// \param sharedobject filename of the library containing the module.
    /// \param modulename name of the module to load.
    ModuleItem(std::string application, std::string sharedobject, std::string modulename, kipl::interactors::InteractionBase *interactor=nullptr);

    /// Gets a reference to the processing module the held by the item.
	PreprocModuleBase * GetModule() {return dynamic_cast<PreprocModuleBase *>(ModuleItemBase::GetModule());}

    /// The destructor unloads the module object
	virtual ~ModuleItem();
private:
};

/// The class manages the handling of back projector modules. It is very similar to the PreprocModuleItem but handles BackProjectorBases.
///
/// For some reason this class is not derived from the ModuleItemBase. I have to check in the code...
class RECONFRAMEWORKSHARED_EXPORT BackProjItem
{
protected:
    kipl::logging::Logger logger; ///< The logger object
public:
    /// Loads the module from the shared object library and perpares it for execution.
    /// \param application the name of the application. Needed to aviod loading modules from a different application.
    /// \param sharedobject filename of the library containing the module.
    /// \param modulename name of the module to load.
    /// \param interactor A reference to an interactor object to enhance the user experience.
    BackProjItem(std::string application,std::string sharedobject, std::string modulename, kipl::interactors::InteractionBase *interactor=nullptr);

    /// Copy constructor
    /// \param The object to copy
	BackProjItem(BackProjItem & item);

    /// Assignment operator
    /// \param item
	BackProjItem & operator=(BackProjItem & item);

    /// Provides a reference to the BackProjector object held by the item.
	BackProjectorModuleBase * GetModule();

    /// The status of the created module.
    /// \returns true if there is an allocated module in the item.
	bool Valid() {return m_Module!=NULL;}
	
    /// Unloads the back-projection module
	virtual ~BackProjItem();
protected:
    /// Dynamicly Loading the module from a shared object library.
    /// \param interactor A reference to an interactor object that can be used to communicate with the main application.
    void LoadModuleObject(kipl::interactors::InteractionBase *interactor);

    /// Unloads the module.
	void Destroy();


    HINSTANCE hinstLib; ///< Handle to the open shared object library.
    BP_FACTORY m_fnModuleFactory; ///< Handle to the module creation factory.
    DESTROYER m_fnDestroyer;    ///< Handle to the module destroyer.

    std::string m_sApplication; ///< Name of the application.
    std::string m_sSharedObject;    ///< Name of the library file.
    std::string  m_sModuleName; ///< Name of the loaded module.

    BackProjectorModuleBase *m_Module;    ///< Reference to the loaded module.
};

#endif
