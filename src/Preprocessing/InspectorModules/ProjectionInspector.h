//<LICENSE>

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
