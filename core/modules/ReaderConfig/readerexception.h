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
    ReaderException(std::string msg, std::string filename, size_t line);
    virtual ~ReaderException(void);

    virtual const char* what() const noexcept;
private:
};


#endif // READEREXCEPTION_H
