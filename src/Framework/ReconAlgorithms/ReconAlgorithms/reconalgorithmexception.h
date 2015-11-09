#ifndef RECONALGORITHMEXCEPTION_H
#define RECONALGORITHMEXCEPTION_H

#include <base/KiplException.h>

class ReconAlgorithmException : public kipl::base::KiplException
{
public:
    ~ReconAlgorithmException();

    ReconAlgorithmException(std::string msg);
    ReconAlgorithmException(std::string msg, std::string filename, size_t line);

    virtual std::string what();
};

#endif // RECONALGORITHMEXCEPTION_H
