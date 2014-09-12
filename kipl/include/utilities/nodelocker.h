#ifndef __NODELOCKER_H
#define __NODELOCKER_H

#include "../kipl_global.h"
#include <string>
#include <list>
#include <set>
#include "../../include/logging/logger.h"

namespace kipl { namespace utilities { 

class KIPLSHARED_EXPORT NodeLocker {
private:
	kipl::logging::Logger logger;
public:
    NodeLocker(std::string path);
	/// \brief Takes a ipconfig dumpfile and creates a license file
	/// \param hwfname file name of the dump file
	/// \param key additional key 
	/// \param licfname file name of the resulting license file
	void GenerateLicenseFile(std::string node, std::string name, std::string key, std::string expires, std::string licfname);

	/// \brief Initializes the node locker
	/// \param license File name of the license file
	/// \param key Application name
	/// \param hwinfo Parameter to test a generated license key
	bool Initialize(std::string licfname, std::string key, std::string hwinfo="");

	/// \brief Initializes the node locker from a list of alternative lic files
	/// \param liclist List of license file names
	/// \param key Application name
	/// \param hwinfo Parameter to test a generated license key
	bool Initialize(std::list<std::string> &liclist, std::string key,  std::string hwinfo="");
	/// \brief Indicates whether the node lock check was successful or not.
	/// \returns true if access is granted.
    bool AccessGranted();


    std::string GetLockerMessage();
	
	/// \brief Retrieves the first MAC adress of the computer
	std::string GetMAC(std::string hwinfo="");
    std::set<std::string> GetMACaddress();

    std::set<std::string> GetNodeString(std::string hwinfo="");

    /// \brief Computes a code string using the node information and a key string
	/// \param nodeinfo Node specific information (e.g. the MAC adress)
	/// \param key Additional information could be user name, application name
	/// \returns A 32 character string  
	std::string ComputeCodeString(std::string nodeinfo, std::string name, std::string key, std::string expires);
	std::string ComputeDateString();
	
private:

	bool m_bAccessGranted; //!< Status of the check
    std::string m_sMessage;
	std::string sPath;
	void ReadLicenseFile(std::string filename, std::string);
	bool IsExpired(std::string expire);
};
}}

#endif
