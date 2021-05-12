//<LICENSE>

#include "../include/ModuleConfig_global.h"
#include "../include/ConfigBase.h"
#include "../include/ModuleException.h"
#include <sstream>
#include <iomanip>

#include <utilities/TimeDate.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>

std::string ConfigBase::m_sHomePath = "";

ConfigBase::ConfigBase(std::string name, std::string path) :
	logger(name),
    m_sName(name),
    m_sApplicationPath(path)
{

}

ConfigBase::~ConfigBase(void)
{
}

ConfigBase::ConfigBase(const ConfigBase &config) :
	logger(config.m_sName),
	m_sName(config.m_sName),
    m_sApplicationPath(config.m_sApplicationPath),
    UserInformation(config.UserInformation),
    modules(config.modules)
{

}

const ConfigBase & ConfigBase::operator=(const ConfigBase &config)
{
    logger             = config.logger;
    m_sName            = config.m_sName;
    m_sApplicationPath = config.m_sApplicationPath;
    modules            = config.modules;
    UserInformation    = config.UserInformation;

	return *this;
}

void ConfigBase::LoadConfigFile(std::string configfile, std::string ProjectName)
{
    xmlTextReaderPtr reader;
    const xmlChar *name;
    int ret;
    std::string sName;
    std::ostringstream msg;

    modules.clear();
    reader = xmlReaderForFile(configfile.c_str(), nullptr, 0);
    if (reader != nullptr)
    {
    	ret = xmlTextReaderRead(reader);
        name = xmlTextReaderConstName(reader);

        if (name==nullptr)
        {
            throw ModuleException("Unexpected contents in parameter file",__FILE__,__LINE__);
        }

        sName=reinterpret_cast<const char *>(name);
        msg.str(""); msg<<"Found "<<sName<<" expect "<<ProjectName;
        logger(kipl::logging::Logger::LogMessage,msg.str());
        if (std::string(sName)!=ProjectName)
        {
            msg.str("");
            msg<<"Unexpected project contents in parameter file ("<<sName<<"!="<<ProjectName<<")";
            logger(kipl::logging::Logger::LogMessage,msg.str());
            throw ModuleException(msg.str(),__FILE__,__LINE__);
        }
        
        logger(kipl::logging::Logger::LogVerbose,"Got project");
    	
        ret = xmlTextReaderRead(reader);
        
        while (ret == 1)
        {
            if (xmlTextReaderNodeType(reader)==1)
            {
	            name = xmlTextReaderConstName(reader);
	            
                if (name==nullptr)
                {
	                throw ModuleException("Unexpected contents in parameter file",__FILE__,__LINE__);
	            }
	            sName=reinterpret_cast<const char *>(name);

				if (sName=="userinformation")
					ParseUserInformation(reader);

				if (sName=="processchain")
					ParseProcessChain(reader);

				ParseConfig(reader,sName);
        	}
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0)
        {
        	std::stringstream str;
        	str<<"Module config failed to parse "<<configfile;
        	throw ModuleException(str.str(),__FILE__,__LINE__);
        }
    }
    else
    {
    	std::stringstream str;
    	str<<"Module config could not open "<<configfile;
    	throw ModuleException(str.str(),__FILE__,__LINE__);
    }
    logger(kipl::logging::Logger::LogVerbose,"Parsing parameter file done");
}


void ConfigBase::ParseUserInformation(xmlTextReaderPtr reader)
{
	const xmlChar *name, *value;
    int ret = xmlTextReaderRead(reader);
    std::string sName, sValue;
    int depth=xmlTextReaderDepth(reader);

    logger(kipl::logging::Logger::LogVerbose,"Enter parse UserInfo");
  
    while (ret == 1) {
    	if (xmlTextReaderNodeType(reader)==1) {
	        name = xmlTextReaderConstName(reader);
	        ret=xmlTextReaderRead(reader);
	        
	        value = xmlTextReaderConstValue(reader);
            if (name==nullptr) {
	            throw ModuleException("Unexpected contents in parameter file",__FILE__,__LINE__);
	        }
            if (value!=nullptr)
	        	sValue=reinterpret_cast<const char *>(value);
	        else
	        	sValue="Empty";
	        sName=reinterpret_cast<const char *>(name);

	        if (sName=="operator") {
				UserInformation.sOperator=reinterpret_cast<const char *>(value);
	        }

	        if (sName=="instrument") {
				UserInformation.sInstrument=reinterpret_cast<const char *>(value);
	        }

			if (sName=="projectnumber") {
				UserInformation.sProjectNumber=reinterpret_cast<const char *>(value);
	        }

			if (sName=="sample") {
				UserInformation.sSample=reinterpret_cast<const char *>(value);
	        }

			if (sName=="comment") {
                if (value!=nullptr)
					UserInformation.sComment=reinterpret_cast<const char *>(value);
	        }
            if (sName=="date") {
                if (value!=nullptr)
                    UserInformation.sDate=reinterpret_cast<const char *>(value);
            }

            if (sName=="version") {
                if (value!=nullptr)
                    UserInformation.sVersion=reinterpret_cast<const char *>(value);
            }
		}
        ret = xmlTextReaderRead(reader);
        if (xmlTextReaderDepth(reader)<depth)
        	ret=0;
	}
  
    logger(kipl::logging::Logger::LogVerbose,"Done parse UserInfo");
}
	

//----------------------
ConfigBase::cUserInformation::cUserInformation() :
	sOperator("Anders Kaestner"),
	sInstrument("ICON"),
	sProjectNumber("P11001"),
	sSample("Unknown item"),
    sComment("No comment"),
    sDate(kipl::utilities::TimeStamp()),
    sVersion("0")

{
}

ConfigBase::cUserInformation::cUserInformation(const cUserInformation &info) :
	sOperator(info.sOperator),
	sInstrument(info.sInstrument),
	sProjectNumber(info.sProjectNumber),
	sSample(info.sSample),
    sComment(info.sComment),
    sDate(info.sDate),
    sVersion(info.sVersion)
{
}

ConfigBase::cUserInformation & ConfigBase::cUserInformation::operator = (const cUserInformation &info)
{
	sOperator      = info.sOperator;
	sInstrument    = info.sInstrument;
	sProjectNumber = info.sProjectNumber;
	sSample        = info.sSample;
	sComment       = info.sComment;
    sDate          = info.sDate;
    sVersion       = info.sVersion;

	return * this;
}

std::string ConfigBase::cUserInformation::WriteXML(int indent)
{
	using namespace std;
	ostringstream str;

    str<<setw(indent)  <<" "<<"<userinformation>"<<std::endl;
		str<<setw(indent+4)  <<" "<<"<operator>"<<sOperator<<"</operator>\n";
		str<<setw(indent+4)  <<" "<<"<instrument>"<<sInstrument<<"</instrument>\n";
		str<<setw(indent+4)  <<" "<<"<projectnumber>"<<sProjectNumber<<"</projectnumber>\n";
		str<<setw(indent+4)  <<" "<<"<sample>"<<sSample<<"</sample>\n";
		str<<setw(indent+4)  <<" "<<"<comment>"<<sComment<<"</comment>\n";
        str<<setw(indent+4)  <<" "<<"<date>"<<sDate<<"</date>\n";
        str<<setw(indent+4)  <<" "<<"<version>"<<sVersion<<"</version>\n";
    str<<setw(indent)  <<" "<<"</userinformation>"<<std::endl;

	return str.str();
}

bool ConfigBase::ConfigChanged(ConfigBase & config, std::list<std::string> freelist)
{
    std::string thisconf=WriteXML();
    std::string otherconf=config.WriteXML();

    size_t tpos0 = 0;
    size_t tpos1 = 0;
    size_t tlen  = 0;

    size_t opos0 = 0;
    size_t opos1 = 0;
    size_t olen  = 0;
    while ((tpos0<thisconf.size()) && (tpos0!=std::string::npos) && (opos0!=std::string::npos)) {
        tpos0++; opos0++;
        tpos1=thisconf.find_first_of('\n',tpos0);  tlen = tpos1-tpos0;
        opos1=otherconf.find_first_of('\n',opos0); olen = opos1-opos0;

        if (thisconf.compare(tpos0,tlen,otherconf,opos0,olen)!=0) { // are the lines different?

            size_t thisclose = thisconf.find_first_of('>',tpos0)+1;
            size_t otherclose = otherconf.find_first_of('>',opos0)+1;
            std::string thispar  = thisconf.substr(thisconf.find_first_of('<',tpos0)+1,thisclose-thisconf.find_first_of('<',tpos0)-2);
            std::string otherpar = otherconf.substr(otherconf.find_first_of('<',opos0)+1,otherclose-otherconf.find_first_of('<',opos0)-2);

        //    std::clog<<"current parameter: "<<thispar<<", "<<otherpar<<std::endl;
            if (thispar.compare(otherpar)!=0) // paremeters are different, a clear true  {
            {
                std::ostringstream msg;
                msg<<"Parameters "<<thispar<<" and "<<otherpar<<" differs between the configurations";
                logger(kipl::logging::Logger::LogMessage,msg.str());

                return true;
            }

            if (find(freelist.begin(),freelist.end(),thispar)==freelist.end()) // parameter is in the free list
            {
                std::ostringstream msg;
                msg<<"Parameter "<<thispar<<" differs between the configurations";
                logger(kipl::logging::Logger::LogMessage,msg.str());

                return true;
            }
            else {
                std::ostringstream msg;
                msg<<"Parameter "<<thispar<<" differs between the configurations but is in the skip list";
                logger(kipl::logging::Logger::LogMessage,msg.str());
            }
        }

        tpos0=tpos1;
        opos0=opos1;
    }
    return false;
}

void ConfigBase::GetCommandLinePars(int argc, char * argv[])
{
    std::vector<std::string> args;

    for (int i=0; i<=argc; i++) {
            args.push_back(argv[i]);
    }

    GetCommandLinePars(args);
}

void ConfigBase::GetCommandLinePars(std::vector<std::string> &args)
{
    ConfigBase::ParseArgv(args); // The arguments from the base class
    ParseArgv(args);             // The arguments from the refined class
}

void ConfigBase::setAppPath(const std::string &path)
{
    m_sApplicationPath = path;
}

std::string ConfigBase::appPath()
{
    return m_sApplicationPath;
}

void ConfigBase::setHomePath(const std::string &path)
{
    m_sHomePath = path;
}

std::string ConfigBase::homePath()
{
    return m_sHomePath;
}

void ConfigBase::ParseArgv(std::vector<std::string> &args)
{
    std::ostringstream msg;
    logger.message("Base class argvparse");
    std::string group;
    std::string var;
    std::string value;

    std::vector<std::string>::iterator it;
    for (it=args.begin()+3 ; it!=args.end(); it++) {
        logger.message(*it);
        try {
            EvalArg(*it,group,var,value);
        }
        catch (ModuleException &e) {
            msg.str("");
            msg<<"Failed to parse argument "<<e.what();
            logger(kipl::logging::Logger::LogWarning,msg.str());
        }
        if (group=="userinformation") {
            if (var=="operator")      UserInformation.sOperator=value;
            if (var=="instrument")    UserInformation.sInstrument=value;
            if (var=="projectnumber") UserInformation.sProjectNumber=value;
            if (var=="sample")        UserInformation.sSample=value;
            if (var=="comment")       UserInformation.sComment=value;
        }
    }
}

void ConfigBase::EvalArg(std::string arg, std::string &group, std::string &var, std::string &value)
{
    size_t possep=arg.find(':');
    if (possep==std::string::npos)
        throw ModuleException("Could not find a separator",__FILE__,__LINE__);

    size_t poseq=arg.find('=',possep);
    if (poseq==std::string::npos)
        throw ModuleException("Could not find an assignment",__FILE__,__LINE__);

    group=arg.substr(0,possep);
    var=arg.substr(possep+1,poseq-possep-1);
    value=arg.substr(poseq+1);
}
