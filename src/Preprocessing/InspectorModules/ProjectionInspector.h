//
// This file is part of the Inspector modules of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-10-05 15:47:22 +0200 (Fri, 05 Oct 2012) $
// $Rev $
// $Id $
//
#ifndef PROJECTIONINSPECTOR_H_
#define PROJECTIONINSPECTOR_H_

#include <PreprocModuleBase.h>

class ProjectionInspector : public PreprocModuleBase
{
public:
	ProjectionInspector();
	virtual ~ProjectionInspector();

	virtual std::map<std::basic_string<char>, std::basic_string<char> > GetParameters();
	virtual int Configure(ReconConfig, std::map<std::basic_string<char>, std::basic_string<char> >);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);
};

#endif /* PROJECTIONINSPECTOR_H_ */
