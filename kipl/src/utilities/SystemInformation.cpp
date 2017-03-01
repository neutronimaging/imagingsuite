//<LICENCE>

#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>

#ifdef _MSC_VER

#else
    #include "sys/types.h"
    #ifdef __APPLE__

    #else
        #include "sys/sysinfo.h"
    #endif
#endif

#include "../../include/utilities/SystemInformation.h"

namespace kipl { namespace utilities {

SystemInformation::SystemInformation() : m_nTotalMemory(0),m_nProcessMemory(0),m_nCPULoad(0)
{}
#ifdef _MSC_VER
void SystemInformation::UpdateInformation()
{

}
#else
#ifdef __APPLE__
void SystemInformation::UpdateInformation()
{
}
#else
// Linux
void SystemInformation::UpdateInformation()
{
	struct sysinfo memInfo;

	sysinfo (&memInfo);
	m_nTotalMemory = memInfo.totalram * memInfo.mem_unit;

	std::ifstream statusfile("/proc/self/status");
	std::string line;
	statusfile>>line;
	size_t pos=0;
	do
	{
		std::cout<<line<<std::endl;
	if ((pos=line.find("VmSize:"))!=std::string::npos) {
			m_nProcessMemory=1024*static_cast<size_t>(atoi(line.substr(pos).c_str()));
			break;
		}
		statusfile>>line;
	}
	while (!statusfile.eof());
}
#endif // OS X
#endif // Windows

}}
