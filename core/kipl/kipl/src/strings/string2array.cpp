//<LICENCE>

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <iterator>

#include "../../include/strings/string2array.h"
#include "../../include/base/KiplException.h"

namespace kipl { namespace strings {
bool GetElement(std::string &str, std::string &element)
{	
    size_t pos=str.find_first_not_of(" ,:");
	if (pos!=str.npos)
		str=str.substr(pos);
	else {
		element.clear();
		return false;
	}
	
    pos=str.find_first_of(" ,:");
	if (pos==str.npos) { // Last element
		element=str;
		return false;
	}
	
	element=str.substr(0, pos);
	str=str.substr(pos+1,str.size());
	
	return true;
}
size_t String2Array(std::string str, double *v, size_t N)
{
	std::string data, element;
	data=str;
	bool status=true;
	size_t i=0;
	for (i=0; i<N; i++) 
	{
		status=GetElement(data,element);
			
		v[i]=atof(element.c_str());
		if (!status)
			break;
	}
	
	return i+1;
}

size_t String2Array(std::string str, float *v, size_t N)
{
	std::string data, element;
	data=str;
	bool status=true;
	size_t i=0;
	for (i=0; (i<N); i++) {
		status=GetElement(data,element);
			
		v[i]=static_cast<float>(atof(element.c_str()));
		if (!status)
			break;
	}
	
	return i+1;
}

size_t String2Array(std::string str, size_t *v, size_t N)
{
	std::string data, element;
	data=str;
	bool status=true;
	size_t i=0;
	for (i=0; (i<N); i++) {
		status=GetElement(data,element);
			
		v[i]=atoi(element.c_str());
		if (!status)
			break;
	}
	
	return i+1;
}


size_t String2Array(std::string str, ptrdiff_t *v, size_t N)
{
	std::string data, element;
	data=str;
	bool status=true;
	size_t i=0;
	for (i=0; (i<N); i++) {
		status=GetElement(data,element);

		v[i]=atoi(element.c_str());
		if (!status)
			break;
	}

	return i+1;
}

size_t String2Array(std::string str, int *v, size_t N)
{
	std::string data, element;
	data=str;
	bool status=true;
	size_t i=0;
	for (i=0; (i<N); i++) {
		status=GetElement(data,element);
			
		v[i]=atoi(element.c_str());
		if (!status)
			break;
	}
	
	return i+1;	
}

size_t String2Set(std::string str, std::set<size_t> &v)
{
	std::string data, element;
	data=str;
	bool status=true;
	v.clear();
	size_t i=0;
	while (status) {
		status=GetElement(data,element);
		if (!element.empty()) {
			v.insert(atoi(element.c_str()));
			i++;
		}
	}
	
	return i;
}

size_t String2List(std::string str, std::list<int> &v)
{
    std::string data, element;
    data=str;
    bool status=true;
    v.clear();
    size_t i=0;
    while (status) {
        status=GetElement(data,element);
        if (!element.empty()) {
            v.push_back(atoi(element.c_str()));
            i++;
        }
    }

    return i;
}

size_t String2Array(std::string str, std::vector<float> &v)
{
    throw kipl::base::KiplException("String2Array is not implemented for vector<float>",__FILE__,__LINE__);
    return 0;
}

size_t String2Array(std::string str, std::vector<size_t> &v)
{
    throw kipl::base::KiplException("String2Array is not implemented for vector<size_t>",__FILE__,__LINE__);
    return 0;
}

size_t String2Array(std::string str, std::vector<std::string> &v)
{
    v.clear();

    std::istringstream iss(str);
    std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                      std::istream_iterator<std::string>{}};

    v=tokens;

    return v.size();
}


std::string Array2String(double *v, size_t N)
{
    std::ostringstream s;

    for (size_t i=0; i<N; i++) {
        s<<v[i]<<(i<(N-1) ? " " : "");
    }
    return s.str();
}

std::string Array2String(float *v, size_t N)
{
    std::ostringstream s;

    for (size_t i=0; i<N; i++) {
        s<<v[i]<<(i<(N-1) ? " " : "");
    }
    return s.str();
}

std::string Array2String(size_t *v, size_t N)
{
    std::ostringstream s;

    for (size_t i=0; i<N; i++) {
        s<<v[i]<<(i<(N-1) ? " " : "");
    }
    return s.str();
}


std::string Array2String(ptrdiff_t *v, size_t N)
{
    std::ostringstream s;

    for (size_t i=0; i<N; i++) {
        s<<v[i]<<(i<(N-1) ? " " : "");
    }
    return s.str();
}


std::string Array2String(int *v, size_t N)
{
    std::ostringstream s;

    for (size_t i=0; i<N; i++) {
        s<<v[i]<<(i<(N-1) ? " " : "");
    }
    return s.str();
}

size_t String2List(std::string str, std::list<std::string> &slist)
{
    slist.clear();

    std::istringstream iss(str);
    std::list<std::string> tokens{std::istream_iterator<std::string>{iss},
                      std::istream_iterator<std::string>{}};

    slist=tokens;

    return slist.size();
}

std::string List2String(std::list<int> &v)
{
    std::ostringstream s;
    int i=0;
    for (auto it=v.begin(); it!=v.end(); ++it, ++i) {
        s<<(*it)<<(i<(v.size()-1) ? " " : "");
    }
    return s.str();
}

}}
