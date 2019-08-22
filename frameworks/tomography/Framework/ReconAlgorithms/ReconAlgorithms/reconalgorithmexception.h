#ifndef RECONALGORITHMEXCEPTION_H
#define RECONALGORITHMEXCEPTION_H

#include "reconalgorithms_global.h"
#include <base/KiplException.h>

class RECONALGORITHMSSHARED_EXPORT ReconAlgorithmException : public kipl::base::KiplException
{
public:
    ~ReconAlgorithmException();

    ReconAlgorithmException(std::string msg);
    ReconAlgorithmException(std::string msg, std::string filename, size_t line);

    virtual const char* what() const _NOEXCEPT;
};

#endif // RECONALGORITHMEXCEPTION_H
