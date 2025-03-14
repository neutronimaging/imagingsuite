//<LICENSE>

#ifndef PROJECTIONINSPECTOR_H_
#define PROJECTIONINSPECTOR_H_

#include "inspectormodules_global.h"
#include <PreprocModuleBase.h>

class INSPECTORMODULESSHARED_EXPORT ProjectionInspector : public PreprocModuleBase
{
public:
	using PreprocModuleBase::Configure;
	ProjectionInspector();
	virtual ~ProjectionInspector();

	std::map<std::basic_string<char>, std::basic_string<char> > GetParameters() override;
	int Configure(ReconConfig, std::map<std::basic_string<char>, std::basic_string<char> >) override;
protected:
	using PreprocModuleBase::ProcessCore;
	int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters) override;
};

#endif /* PROJECTIONINSPECTOR_H_ */
