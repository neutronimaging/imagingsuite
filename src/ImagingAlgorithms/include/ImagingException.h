/*
 * ImagingException.h
 *
 *  Created on: Jan 25, 2013
 *      Author: kaestner
 */

#ifndef IMAGINGEXCEPTION_H_
#define IMAGINGEXCEPTION_H_

#include <base/KiplException.h>
#include <string>

class ImagingException: public kipl::base::KiplException {
public:
	ImagingException();
	virtual ~ImagingException();
	ImagingException(std::string msg);
	ImagingException(std::string msg, std::string filename, size_t line);

	virtual std::string what();
};



#endif /* IMAGINGEXCEPTION_H_ */
