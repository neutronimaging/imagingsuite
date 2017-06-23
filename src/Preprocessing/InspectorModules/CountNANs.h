//<LICENSE>

#ifndef COUNTNANS_H_
#define COUNTNANS_H_

#include <PreprocModuleBase.h>

class CountNANs: public PreprocModuleBase {
public:
	CountNANs();
	virtual ~CountNANs();

	virtual std::map<std::basic_string<char>, std::basic_string<char> > GetParameters();
	virtual int Configure(ReconConfig, std::map<std::basic_string<char>, std::basic_string<char> >);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters);
	virtual int ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);

};

#endif /* COUNTNANS_H_ */
