#include "folders.h"

#include <strings/filenames.h>
#include <logging/logger.h>
#include "readerexception.h"

#include <string>
#include <sstream>
// This code requires C++17
#include <filesystem> 
namespace fs = std::filesystem;

void CheckFolders(const std::string &path, bool create)
{
    kipl::logging::Logger logger("CheckFolders");

    std::string p=path;
    if (*p.rbegin()==kipl::strings::filenames::slash)
    {
        logger.debug("Removed trailing slash");
        p.pop_back();
    }

    if (fs::is_directory(p))
    {
        std::ostringstream msg;
        msg<<"Directory "<<p<<" exists.";
        logger.message("");
        return ;
    }
    
    if (!fs::exists(p))
    {
        if (create)
        {
            fs::create_directories(p);
        }
        else 
        {
            std::ostringstream msg;
            msg<<"Directory "<<p<<" doesn't exist and was not created.";
            logger.error(msg.str());
            throw ReaderException(msg.str(),__FILE__,__LINE__);
        }
    }
        
}
