//<LICENSE>

#ifndef COUNTNANS_H
#define COUNTNANS_H

#include "inspectormodules_global.h"
#include <PreprocModuleBase.h>

class INSPECTORMODULESSHARED_EXPORT CountNANs: public PreprocModuleBase {
public:
	using PreprocModuleBase::Configure;
	CountNANs();
	~CountNANs();

	std::map<std::basic_string<char>, std::basic_string<char> > GetParameters() override;
	int Configure(ReconConfig, std::map<std::basic_string<char>, std::basic_string<char> >) override;
protected:
	using PreprocModuleBase::ProcessCore;
	int ProcessCore(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters) override;
	int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters) override;

};

#endif /* COUNTNANS_H_ */
