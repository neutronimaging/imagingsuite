//<LICENSE>

#ifndef MODULEITEMBASE_H_
#define MODULEITEMBASE_H_

#ifdef _MSC_VER // Shared object specific for msvc
typedef void * (__cdecl *FACTORY)(const char *,const char *, void *);
typedef int (__cdecl *DESTROYER)(const char *,void *);
#else // Shared object specific for gcc
typedef void * (*FACTORY)(const char *, const char *, void *) ;
typedef int (*DESTROYER)(const char *,void *);
typedef  void * HINSTANCE;
#endif

#include "ModuleConfig_global.h"
#include <string>
#include <logging/logger.h>
#include "ProcessModuleBase.h"

/// Container class that manages the loading and destruction of ProcessModules.
/// This class is only specialized for the application framework and does not
/// need to be inherited for each new module implementation.
/// \todo Implement reference counting.
class MODULECONFIGSHARED_EXPORT ModuleItemBase
{
protected:
    kipl::logging::Logger logger; ///< Logging facility
public:
    /// Constructor that loads a module from a shared object file.
    /// \param application The name of the application. The name is used to verify that the requested module can be used by the current application.
    /// \param sharedobject File name of the shared object file.
    /// \param modulename Name of the requested module.
	ModuleItemBase(std::string application, std::string sharedobject, std::string modulename, kipl::interactors::InteractionBase *interactor=nullptr);

    /// Copy constructor makes a shallow copy. Be careful with the destructor.
    /// \param item The object to copy.
	ModuleItemBase(ModuleItemBase & item);

    /// Assigns a module to another instance. Shallow copy, be careful with the destructor.
	ModuleItemBase & operator=(ModuleItemBase & item);

    /// Get the module contained by the item container.
    /// \returns A reference to the module
	virtual ProcessModuleBase * GetModule();

    /// Check if the item contains a valid module
    /// \returns true if the module pointer is non-NULL.
	bool Valid() {return m_Module!=NULL;}

    /// Destructor that unloads the loaded module.
	virtual ~ModuleItemBase();

protected:
    /// Module unload method. It calls the destroy function in the shared object.
	void Destroy();

private:
    /// Loads the specified module from the shared object file.
	void LoadModuleObject(kipl::interactors::InteractionBase *interactor=nullptr);

    HINSTANCE hinstLib;           ///< Handler to the shared object file.
    FACTORY m_fnModuleFactory;    ///< Handler to the module factory function in the SO file.
    DESTROYER m_fnDestroyer;      ///< Handler to the module destruction function in the SO file.

    std::string m_sApplication;   ///< Name of the application.
    std::string m_sSharedObject;  ///< Name of the shared object file.
    std::string  m_sModuleName;   ///< Name of the module.

    ProcessModuleBase *m_Module;  ///< Reference to the created module instance.

};


#endif /* MODULEITEMBASE_H_ */
