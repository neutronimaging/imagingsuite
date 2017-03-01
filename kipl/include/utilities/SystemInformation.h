//<LICENCE>

#ifndef SYSTEMINFORMATION_H_
#define SYSTEMINFORMATION_H_

#include <cstdlib>

namespace kipl { namespace utilities {
class SystemInformation
{
public:
	SystemInformation();
	size_t TotalMemory() {UpdateInformation(); return m_nTotalMemory;}
	size_t ProcessMemory() {UpdateInformation(); return m_nProcessMemory;}
	size_t CPULoad() {UpdateInformation(); return m_nCPULoad;}

protected:
	void UpdateInformation();
	size_t m_nTotalMemory;
	size_t m_nProcessMemory;
	size_t m_nCPULoad;
};

}}
#endif /* SYSTEMINFORMATION_H_ */
