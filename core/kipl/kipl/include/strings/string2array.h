//<LICENCE>

#ifndef STRING2ARRAY_H_
#define STRING2ARRAY_H_

#include "../kipl_global.h"

#include <string>
#include <vector>
#include <cstdlib>
#include <set>
#include <list>
#include <sstream>
#include <cstddef>

namespace kipl { namespace strings {
size_t KIPLSHARED_EXPORT String2Array(std::string str, double *v, size_t N);
size_t KIPLSHARED_EXPORT String2Array(std::string str, float *v, size_t N);
size_t KIPLSHARED_EXPORT String2Array(std::string str, size_t *v, size_t N);
size_t KIPLSHARED_EXPORT String2Array(std::string str, ptrdiff_t *v, size_t N);
size_t KIPLSHARED_EXPORT String2Array(std::string str, int *v, size_t N);

std::string KIPLSHARED_EXPORT Array2String(double *v, size_t N);
std::string KIPLSHARED_EXPORT Array2String(float *v, size_t N);
std::string KIPLSHARED_EXPORT Array2String(size_t *v, size_t N);
std::string KIPLSHARED_EXPORT Array2String(ptrdiff_t *v, size_t N);
std::string KIPLSHARED_EXPORT Array2String(int *v, size_t N);

size_t KIPLSHARED_EXPORT String2Array(std::string str, std::vector<float> &v);
size_t KIPLSHARED_EXPORT String2Array(std::string str, std::vector<size_t> &v);
size_t KIPLSHARED_EXPORT String2Array(std::string str, std::vector<int> &v);
size_t KIPLSHARED_EXPORT String2Array(std::string str, std::vector<std::string> &v);

size_t KIPLSHARED_EXPORT String2Set(std::string str, std::set<size_t> &v);
size_t KIPLSHARED_EXPORT String2List(std::string str, std::list<int> &v);
size_t KIPLSHARED_EXPORT String2List(std::string str, std::list<std::string> &slist);

template <typename T>
std::string Set2String(std::set<T> &v)
{
	typename std::set<T>::iterator it;
	std::ostringstream str;
	if (!v.empty()) {
		for (it=v.begin(); it!=v.end(); it++)
			str<<*it<<" ";
	}

	return str.str();
}
}}
#endif /*STRING2ARRAY_H_*/
