//<LICENSE>

#ifndef IMAGINGEXCEPTION_H_
#define IMAGINGEXCEPTION_H_
#include "ImagingAlgorithms_global.h"
#include <base/KiplException.h>
#include <string>

class IMAGINGALGORITHMSSHARED_EXPORT ImagingException: public kipl::base::KiplException {
public:
	virtual ~ImagingException();
	ImagingException(std::string msg);
	ImagingException(std::string msg, std::string filename, size_t line);

	virtual std::string what();
};



#endif /* IMAGINGEXCEPTION_H_ */
