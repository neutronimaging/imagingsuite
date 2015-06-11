//
// This file is part of the Inspector modules of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-21 16:13:06 +0200 (Mon, 21 May 2012) $
// $Rev $
// $Id $
//

#ifndef SAVEPROJECTIONS_H_
#define SAVEPROJECTIONS_H_

#include <string>

#include <PreprocModuleBase.h>
#include <ReconConfig.h>

class SaveProjections : public PreprocModuleBase
{
public:
	SaveProjections();
	virtual ~SaveProjections();

	virtual std::map<std::basic_string<char>, std::basic_string<char> > GetParameters();
	virtual int Configure(ReconConfig config, std::map<std::basic_string<char>, std::basic_string<char> > parameters);
protected:
	
	virtual int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);

	ReconConfig m_config;
	std::string m_sFileMask;
	ReconConfig::cProjections::eImageType m_eImageType;
};

#endif /* PROJECTIONINSPECTOR_H_ */
