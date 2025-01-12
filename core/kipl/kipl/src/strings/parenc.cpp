//<LICENCE>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
//#include <io.h>

#include <iostream>
#include <iomanip>
#include <vector>

#include "../../include/strings/parenc.h"


using namespace std;

namespace kipl { namespace strings { namespace parenc {

int getIntPar(char *data,char *par_key, int *val) // -ak- get the int value corresponding to par_key
{
	int i=0;
	int len=strlen(par_key);
	int found=-1;
	

	while (data[i]) {
		if (!strncmp(data+i,par_key,len)) {
			while (((data+len+i)[0]==' ') || (data+len+i)[0]=='=') i++;
			*val=atoi(data+len+i);
			found=0;
			break;
		} else i++;		
	}
	return found;
}

int getUIntPar(char *data,char *par_key, unsigned int *val) // -ak- get the int value corresponding to par_key
{
	int i=0;
	int len=strlen(par_key);
	int found=-1;
	

	while (data[i]) {
		if (!strncmp(data+i,par_key,len)) {
			while (((data+len+i)[0]==' ') || (data+len+i)[0]=='=') i++;
			*val=atoi(data+len+i);
			found=0;
			break;
		} else i++;		
	}
	return found;
}


int getIntVecPar(char *data,char *par_key, int *val, int n) // -ak- get the int value corresponding to par_key
{
	int i=0;
	int len=strlen(par_key);
	int found=-1;
	

	while ((data[i]) && (strncmp(data+i,par_key,len))) i++;
	
	if (!data[i]) return found;
	while (((data+len+i)[0]==' ') || (data+len+i)[0]=='=') i++;
	char ref[4096];
	
	strcpy(ref,data+len+i);
    size_t k;
	for (k=0; k<strlen(ref); k++) {
		if (ref[k]==0x0a) {
			ref[k]=0;
			break;
		}
	}
	
	char vstr[32];
	
	size_t l=0;
	for (i=0; i<n; i++) {
		sscanf(ref+l,"%s",vstr);
		if ((vstr[0]) && (l<k))
			val[i]=atoi(vstr);
		else
			break;
		l+=strlen(vstr)+1;
		found=0;
	}
	if (i!=n) val[0]=-1;

	return found;
}


int getCharSTLVecPar(char *data,char *par_key, vector<char> &val)
{
	int i=0,n=val.size();
	int len=strlen(par_key);
	int found=-1;
	

	while ((data[i]) && (strncmp(data+i,par_key,len))) i++;
	
	if (!data[i]) return found;
	while (((data+len+i)[0]==' ') || (data+len+i)[0]=='=') i++;
	char ref[4096];
	
	strcpy(ref,data+len+i);
    	unsigned int k;
	for (k=0; k<strlen(ref); k++) {
		if (ref[k]==0x0a) {
			ref[k]=0;
			break;
		}
	}
	
	char vstr[32];
	
	size_t l=0;
	if (n) {
		for (i=0; i<n; i++) {
			sscanf(ref+l,"%s",vstr);
			if ((vstr[0]) && (l<k))
				val[i]=vstr[0];
			else
				break;
			l+=strlen(vstr)+1;
			found=0;
		}
		if (i!=n) val[0]=-1;
	}
	else {
		i=0;
		while (l<k) {
			sscanf(ref+l,"%s",vstr);
			if (vstr[0])
				val.push_back(vstr[0]);
			else
				break;
				
			l+=strlen(vstr)+1;
			found=0;
		}
		
	}

	return found;
}


int getIntSTLVecPar(const char *data,const char *par_key, vector<int> &val)
{
	int i=0,n=val.size();
	int len=strlen(par_key);
	int datalen=strlen(data);
	int found=-1;
	

	while ((i!=datalen-len) && (data[i]) && (strncmp(data+i,par_key,len))) i++;
	
	if ((!data[i]) || (i>=datalen-len)) return found;
	while ((i+len<datalen) && ((data[len+i]==' ') || (data[len+i]=='='))) i++;
	if (i+len>=datalen) return found;
	
	char ref[4096];
	
	strcpy(ref,data+len+i);
    size_t k;
	for (k=0; k<strlen(ref); k++) {
		if (ref[k]==0x0a) {
			ref[k]=0;
			break;
		}
	}
	
	char vstr[32];
	
	size_t l=0;
	if (n) {
		for (i=0; i<n; i++) {
			sscanf(ref+l,"%s",vstr);
			if ((vstr[0]) && (l<k))
				val[i]=atoi(vstr);
			else
				break;
			l+=strlen(vstr)+1;
			found=0;
		}
		if (i!=n) val[0]=-1;
	}
	else {
		i=0;
		while (l<k) {
			sscanf(ref+l,"%s",vstr);
			if (vstr[0])
				val.push_back(atoi(vstr));
			else
				break;
				
			l+=strlen(vstr)+1;
			found=0;
		}
		
	}

	return found;
}

int getFloatSTLVecPar(const char *data,const char *par_key, vector<float> &val)
{
	int i=0,n=val.size();
	int len=strlen(par_key);
	int datalen=strlen(data);
	int found=-1;
	

	while ((i!=datalen-len) && (data[i]) && (strncmp(data+i,par_key,len))) i++;
	
	if ((i>=datalen-len) || (!data[i])) return found;
	
	while ((i+len<datalen) && ((data[len+i]==' ') || (data[len+i]=='='))) i++;
	
	if (i+len>=datalen) return found;
	char ref[4096];
	
	strcpy(ref,data+len+i);

    size_t k;
	for (k=0; k<strlen(ref); k++) {
		if (ref[k]==0x0a) {
			ref[k]=0;
			break;
		}
	}
	
	char vstr[32];
	
	size_t l=0;
	if (n) {
		for (i=0; i<n; i++) {
			sscanf(ref+l,"%s",vstr);
			if ((vstr[0]) && (l<k))
				val[i]=static_cast<float>(atof(vstr));
			else
				break;
			l+=strlen(vstr)+1;
			found=0;
		}
		if (i!=n) val[0]=-1;
	}
	else {
		i=0;
		while (l<k) {
			sscanf(ref+l,"%s",vstr);
			if (vstr[0])
				val.push_back(static_cast<float>(atof(vstr)));
			else
				break;
				
			l+=strlen(vstr)+1;
			found=0;
		}
		
	}

	return found;
}

int getIntPar(const std::string data,const std::string par_key, int &val) // -ak- get the float value corresponding to par_key
{
	std::string str;
	if (getStringPar(data,par_key,str)!=0)
		return -1;

	val=atoi(str.c_str());

	return 0;
}

int getUIntPar(const std::string data,const std::string par_key, unsigned int &val) // -ak- get the float value corresponding to par_key
{
	std::string str;
	if (getStringPar(data,par_key,str)!=0)
		return -1;

	val=static_cast<unsigned int>(atoi(str.c_str()));

	return 0;
}

int getFloatPar(const std::string data,const std::string par_key, float &val) // -ak- get the float value corresponding to par_key
{
	std::string str;
	if (getStringPar(data,par_key,str)!=0)
		return -1;

	val=static_cast<float>(atof(str.c_str()));

	return 0;
}

int getDoublePar(const std::string data,const std::string par_key,double &val) // -ak- get the float value corresponding to par_key
{
	std::string str;
	if (getStringPar(data,par_key,str)!=0)
		return -1;

	val=atof(str.c_str());

	return 0;
}
int getStringPar(const string data, const string par_key, string  & str) // -ak- get the string corresponding to par_key
{
	if (data.length()==0)
		return -1;

	// Find the key
	size_t line_start=data.find(par_key);
	if (line_start==std::string::npos)
		return -1;

	size_t line_end=data.find('\n',line_start);
	if (line_end==std::string::npos)
		str=data.substr(line_start);
	else
		str=data.substr(line_start,line_end-line_start);

	size_t eq_signpos=str.find('=');
	if (eq_signpos==std::string::npos)
		return -1;

	string key_str=str.substr(0,eq_signpos);
	while ((!key_str.empty()) && (*key_str.rbegin()==' '))
		key_str.erase(key_str.length()-1);

	if (key_str!=par_key)
		return -1;
	

	str=str.substr(eq_signpos+1);
	size_t comment_pos=str.find('#');
	str=str.substr(0,comment_pos);
	size_t non_space_pos_front=str.find_first_not_of(' ');
	
	while ((!str.empty()) && (*str.rbegin()==' '))
		str.erase(str.length()-1);

	str=str.substr(non_space_pos_front);

	return 0;
}
/*
int getChStringPar(char *data,char *par_key, char * str) // -ak- get the string corresponding to par_key
{
	int i=0,j=0;
	int len=strlen(par_key);
	int found=-1; 
	
	str[0]=0;

	while (data[i]) {
		if (!strncmp(data+i,par_key,len)) {
			while (((data+len+i)[0]==' ') || (data+len+i)[0]=='=') i++;
			while ((data+len+i)[j]!='\n') 
				str[j++]=(data+len+i)[j];
			str[j]=0;
			found=0;
			break;
		} else i++;
	}
	
	return found;
}
*/
}}}
