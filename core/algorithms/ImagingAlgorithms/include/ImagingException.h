//<LICENSE>

#ifndef IMAGINGEXCEPTION_H_
#define IMAGINGEXCEPTION_H_
#include "ImagingAlgorithms_global.h"
#include <string>

class IMAGINGALGORITHMSSHARED_EXPORT ImagingException {
public:
	virtual ~ImagingException();
	ImagingException(std::string msg);
    ImagingException(std::string msg, std::string filename, size_t line);

    const char* what() const;
protected :
    std::string sExceptionName;
    std::string sMessage;
    std::string sFileName;
    size_t nLineNumber;
};



#endif /* IMAGINGEXCEPTION_H_ */
