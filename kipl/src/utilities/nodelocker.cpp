#ifdef _MSC_VER
#include <winsock2.h>
#include <Iphlpapi.h>
#else
#include <sys/socket.h>
#include <sys/ioctl.h>
#ifdef __APPLE__
#include <net/if.h>
//#include <iotypes.h>
#include <ifaddrs.h>
#include <net/if_dl.h>
#else
#include <linux/if.h>
#endif // Apple
#include <netdb.h>
#include <cstring>
#endif // MSCVER

#include "../../include/utilities/nodelocker.h"
#include "../../include/base/KiplException.h"
#include "../../include/strings/miscstring.h"
#include "../../include/strings/filenames.h"
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <set>
#include <list>

namespace kipl { namespace utilities { 

bool NodeLocker::Initialize(std::list<std::string> &liclist, std::string key,  std::string hwinfo)
{
    std::list<std::string>::iterator it;
	std::ifstream f;
    std::ostringstream msg;
	for (it=liclist.begin(); it!=liclist.end(); it++) {
		kipl::strings::filenames::CheckPathSlashes(*it,false);
        msg.str("");
        msg<<"Checking for "<<(*it);
        logger(kipl::logging::Logger::LogMessage,msg.str());
		f.open(it->c_str());
		if (f.good()) {
			f.close();
            msg.str("");
            msg<<"Found a license file at "<<*it<<std::endl;
            logger(kipl::logging::Logger::LogMessage,msg.str());
			return Initialize(*it,key,hwinfo);
		}
		f.close();
	}

    m_sMessage = "Failed to locate the license file";
	throw kipl::base::KiplException("Could not find the license file",__FILE__,__LINE__);

	return false;
}

bool NodeLocker::Initialize(std::string licfname, std::string key,  std::string hwinfo)
{

//#ifdef __APPLE__
#ifdef __NEVER__
    return true;
#else
    std::ostringstream msg;
    std::cout<<"Opening "<<licfname<<std::endl;
	std::ifstream licfile(licfname.c_str());

	if (licfile.fail())
		throw kipl::base::KiplException("Could not find the license file.",__FILE__,__LINE__);

	std::string item;
	std::string value;

	std::string license;
	std::string name;
	std::string expires;
	std::string lictype;
	char linestr[512];

	std::string line;

	licfile.getline(linestr,512);
	line=linestr;
	if (line.substr(1,7)!="license")
		throw kipl::base::KiplException("Wrong license file format, please update your license.",__FILE__,__LINE__);

	while (!licfile.eof()) {
		licfile.getline(linestr,512);
		line=linestr;

        size_t start=line.find('<');
		size_t end=line.find('>');
		item=line.substr(start+1,end-start-1);
		value=line.substr(end+1);
		value=value.substr(0,value.find('<'));

		if (item=="user")
			name=value;

		if (item=="type")
			lictype=value;

		if (item=="expires")
			expires=value;

		if (item=="code")
			license=value;
	}

    std::set<std::string> nodelist=GetNodeString();
    msg.str("");
    msg<<"Lic contents: user="<<name<<", type="<<lictype<<", expires="<<expires<<", code="<<license<<", size(maclist)="<<nodelist.size()<<std::endl;
    std::set<std::string>::iterator nit;
    msg<<"Nodes: ";
    for (nit=nodelist.begin(); nit!=nodelist.end(); nit++)
        msg<<*nit<<" ";

    logger(kipl::logging::Logger::LogMessage,msg.str());

    if (IsExpired(expires)) {
		m_bAccessGranted=false;
        m_sMessage = "Your license expired "+expires;
		return m_bAccessGranted;
	}

	try {
        std::set<std::string>::iterator it;
		for (it=nodelist.begin(); it!=nodelist.end(); it++) {
			std::string macadress  = (lictype=="locked" ? *it : lictype);

			std::string nodestring = ComputeCodeString(macadress, name,key, expires);
			msg.str("");
			msg<<"Code from file: "<<license<<", generated code:"<<nodestring ;
            logger(logging::Logger::LogVerbose,msg.str());
			
            if (m_bAccessGranted = (nodestring == license)) {
                m_sMessage = "Access granted";
				break;
            }
		}
        if (!m_bAccessGranted) {
            m_sMessage = "Access denied on this computer";
        }
	}
	catch (...) {
		m_bAccessGranted=false;
	}

	return m_bAccessGranted;
#endif
}

std::string NodeLocker::GetMAC(std::string hwinfo)
{
	// Getting the mac adress with ipconfig
	std::ifstream nodefile;
	std::ostringstream cmd;

	if (hwinfo.empty()) {
		int res=system(cmd.str().c_str());
		if (res!=0)
			throw kipl::base::KiplException("Could not generate node locking information",__FILE__,__LINE__);
	
		std::string fname=sPath+"ipconf.txt";
		nodefile.open(fname.c_str());
	}
	else {
		nodefile.open(hwinfo.c_str());
	}

	if (nodefile.fail())
		throw kipl::base::KiplException("Could not read node locking information",__FILE__,__LINE__);

	char cline[512];
	std::string line;
	nodefile.getline(cline,512);

	while (!nodefile.eof()) {
		line=cline;
		if (line.find("Physical Address")!=std::string::npos) {
			break;
		}
		nodefile.getline(cline,512);
	}

	if (!nodefile.eof()) {
		line=line.substr(line.find(":"));
		line=line.substr(line.find_first_not_of(": "));
		line=line.substr(0,line.find_first_not_of("0123456789ABCDEF-"));
	}
	else {
		line.clear();
	}

	nodefile.close();
	if (hwinfo.empty()) {
		cmd.str("");
		cmd<<"del \""<<sPath<<"ipconf.txt\"";
		int res=system(cmd.str().c_str());
	}

	return line;
}

#ifdef _MSC_VER
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x)) 
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

std::set<std::string> NodeLocker::GetMACaddress() // Print MAC address
{
    std::set<std::string> maclist;
	std::ostringstream macstr;

	PIP_ADAPTER_ADDRESSES AdapterAddresses = NULL;
    ULONG OutBufferLength = 65536;
    ULONG RetVal = 0;    

	RetVal = GetAdaptersAddresses(
			AF_INET, 
			0ul,
			NULL, 
			NULL, 
			&OutBufferLength);

	if (AdapterAddresses != NULL) {
		FREE(AdapterAddresses);
	} 

	AdapterAddresses = (PIP_ADAPTER_ADDRESSES) MALLOC(10*OutBufferLength);
	if (AdapterAddresses == NULL) {
		std::cerr<<"Buffer Allocation error."<<std::endl;
		exit(-1);
	}
	


	RetVal = GetAdaptersAddresses(
			AF_INET, 
			0,
			NULL, 
			AdapterAddresses, 
			&OutBufferLength);


    if (RetVal == NO_ERROR) {
      // If successful, output some information from the data we received
      PIP_ADAPTER_ADDRESSES AdapterList = AdapterAddresses;

	  while (AdapterList) {
		macstr.str("");
		for (size_t i=0; i<6; i++) {
			macstr<<kipl::strings::Byte2Hex(AdapterList->PhysicalAddress[i]);
			if (i!=5) macstr<<"-";
		}

		if (macstr.str()!="00-00-00-00-00-00")
			maclist.push_back(macstr.str());
		AdapterList=AdapterList->Next;
	  }

    }



	FREE(AdapterAddresses);


	return maclist;
}

#undef MALLOC
#undef FREE
#else

#ifdef __APPLE__
std::set<std::string> NodeLocker::GetMACaddress() // Print MAC address
{
    std::set<std::string> maclist;
    std::ostringstream macstr;

    ifaddrs* iflist;
    char mac_addr[256];

    if (getifaddrs(&iflist) == 0) {
        for (ifaddrs* cur = iflist; cur; cur = cur->ifa_next) {
            macstr.str("");
            if ((cur->ifa_addr->sa_family == AF_LINK) && cur->ifa_addr && !strncmp(cur->ifa_name,"en",2)) {
                sockaddr_dl* sdl = (sockaddr_dl*)cur->ifa_addr;
                memcpy(mac_addr, LLADDR(sdl), sdl->sdl_alen);
                for (size_t i=0; i<6; i++) {
                    macstr<<kipl::strings::Byte2Hex(mac_addr[i]);
                    if (i!=5) macstr<<"-";
                }

                if (macstr.str()!="00-00-00-00-00-00") {
                    maclist.insert(macstr.str());
//                    logger(kipl::logging::Logger::LogMessage,macstr.str());
                }

            }
        }

        freeifaddrs(iflist);
    }

	return maclist;
}
#else
std::set<std::string> NodeLocker::GetMACaddress() // Print MAC address
{
	std::ostringstream macstr;
    std::set<std::string> maclist;
	struct ifreq s;
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	strcpy(s.ifr_name, "eth0");
	if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {
		int i;
		for (i = 0; i < 6; ++i) {
			macstr<<kipl::strings::Byte2Hex(s.ifr_addr.sa_data[i]);
			if (i!=5) macstr<<"-";
		}
        maclist.insert(macstr.str());
	}

	return maclist;
}
#endif //OS X
#endif // Windows

std::set<std::string> NodeLocker::GetNodeString(std::string hwinfo)
{
    std::set<std::string> maclist;
    std::set<std::string> nodestrings;
	std::string node;
	std::string mac;

	try {
		maclist=GetMACaddress();
	}
	catch (kipl::base::KiplException &e) {
		throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
	}

    std::set<std::string>::iterator macit;

	for (macit=maclist.begin(); macit!=maclist.end(); macit++) {
		node.clear();
		int sum=1;

        std::string nodestr = *macit;
        for (int i=0; i<nodestr.size(); i++) {
            sum^=static_cast<int>(nodestr[i]);
			node.push_back(static_cast<char>(sum %10)+'0');
		}
	
        nodestrings.insert(node);
	}

	return nodestrings;
}

std::string NodeLocker::ComputeCodeString(std::string nodeinfo, std::string name, std::string key, std::string expires)
{
  //  std::cout<<"ComputeCodeString: node="<<nodeinfo<<", name="<<name<<", key="<<key<<", expires="<<expires<<std::endl;
	while (nodeinfo.size()<32) {
		nodeinfo+=nodeinfo;
	}
	nodeinfo=nodeinfo.substr(0,32);
	
	while (key.size()<32) {
		key+=key;
	}
	key=key.substr(0,32);

	while (name.size()<32) {
		name+=name+expires;
	}
	name=name.substr(0,32);
//    std::cout<<"ComputeCodeString: node="<<nodeinfo<<", name="<<name<<", key="<<key<<", expires="<<expires<<std::endl;

    int sum=0;
    std::string code="";

	for (int i=0; i<32; i++) {
		sum^=int(key[i])*int(nodeinfo[i])*int(name[i]);
		code.push_back('0'+ (sum % 10));	
	}

 //   std::cout<<"Code="<<code<<std::endl;
	return code;
}

void NodeLocker::GenerateLicenseFile(std::string node,std::string name, std::string key, std::string expires, std::string licfname)
{
	std::string code=ComputeCodeString(node, name, key,expires);
//	std::cout<<"Computed license file with\n"
//		<<"Name: "<<name<<"\n"
//		<<"key: "<<key<<"\n"
//		<<"node: "<<node<<"\n"
//		<<"expires:"<<expires<<"\n"
//		<<"-> Code string: "<<code<<std::endl;

	std::ofstream outfile(licfname.c_str());
	outfile<<"<license>\n";
	outfile<<"    <user>"<<name<<"</user>\n";
	outfile<<"    <expires>"<<expires<<"</expires>\n";
	outfile<<"    <type>"<<(node!="floating" ? "locked":"floating")<<"</type>\n";
	outfile<<"    <code>"<<code<<"</code>\n";
	outfile<<"</license>\n";

	outfile.close();
}

bool NodeLocker::IsExpired(std::string expire)
{
    std::ostringstream msg;
    msg<<"The application expires "<<expire<<std::endl;
    logger(kipl::logging::Logger::LogMessage,msg.str());

	if (expire=="never") {
		return false;
	}

	if (expire.size()!=8)
		return true;

	int year=atoi(expire.substr(0,4).c_str());
	int month=atoi(expire.substr(4,2).c_str());
	int day=atoi(expire.substr(6,2).c_str());

	time_t t = time(0);   // get time now
	struct tm * now = localtime( & t );

	bool expired = true ;
	if (year <= (now->tm_year + 1900))
		if (month<=(now->tm_mon + 1))
			expired =(day < (now->tm_mday));
		else
			expired = false;
	else
		expired = false;

	return expired;
}

}}
