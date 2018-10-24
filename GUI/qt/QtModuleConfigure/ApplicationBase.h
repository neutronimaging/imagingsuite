//<LICENSE>

#ifndef APPLICATIONBASE_H_
#define APPLICATIONBASE_H_

#include "QtModuleConfigure_global.h"

class QTMODULECONFIGURESHARED_EXPORT ApplicationBase {
public:
	ApplicationBase();
	virtual ~ApplicationBase();

	virtual void UpdateDialog()=0;
	virtual void UpdateConfig()=0;
};

#endif /* APPLICATIONBASE_H_ */
