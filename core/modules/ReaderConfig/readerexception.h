#ifndef READEREXCEPTION_H
#define READEREXCEPTION_H

#include "readerconfig_global.h"

#include <string>

#include <base/KiplException.h>

class READERCONFIGSHARED_EXPORT ReaderException : public kipl::base::KiplException
{
public:
    ReaderException(void);
    ReaderException(std::string msg);
    ReaderException(const std::string &msg, const std::string &filename, size_t line);
    virtual ~ReaderException(void);
private:
};


#endif // READEREXCEPTION_H
