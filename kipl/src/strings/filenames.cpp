/***************************************************************************
 *   Copyright (C) 2006 by Anders Kaestner   *
 *   anders@kaestner.se   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <../include/kipl_global.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

#ifdef _MSC_VER
const char slash='\\';
const char wrong_slash='/';
#else 
const char slash='/';
const char wrong_slash='\\';
#endif
namespace kipl { namespace strings { namespace filenames {

void KIPLSHARED_EXPORT StripFileName(const std::string filestr,
		std::string &path, 
		std::string & name, 
		std::vector<std::string> &extensions)
{
        size_t pos,pos2;

        pos=filestr.rfind(slash);

        if (pos==filestr.npos) {
#ifndef _MSC_VER
                path=".";
				path=path+slash;
#endif
                pos=0;
                pos2=filestr.find_last_of('.',pos);
                name=filestr.substr(0,pos2-pos);
        }
        else {
                pos2=filestr.find_last_of('.',pos);
                path=filestr.substr(0,pos+1);
                name=filestr.substr(pos+1,pos2-pos-1);
        }

        pos=pos2;
        string tmp;
        extensions.clear();
        while (pos!=filestr.npos) {
                pos2=filestr.find('.',pos+1);
                tmp=filestr.substr(pos,pos2-pos);
                extensions.push_back(tmp);
                pos=pos2;
        }

}

int KIPLSHARED_EXPORT MakeFileName(const std::string filename,int num, std::string &name, std::string &ext, const char cWildCardChar, const char cFillChar,bool bReversedIndex)
{
	string::size_type first=filename.find_first_of(cWildCardChar);
	string::size_type last=filename.find_first_not_of(cWildCardChar,first);

	if (last==string::npos)
		last=filename.length();

    ostringstream fnstr, numstr;

	fnstr<<filename.substr(0,first);

    numstr.width(last-first);
    numstr.fill(cFillChar);
    numstr<<num;
    std::string idxstring;
    if (bReversedIndex) {
        std::string tmp=numstr.str();
        std::string::reverse_iterator it;
        idxstring.clear();
        for (it=tmp.rbegin(); it!=tmp.rend(); it++) {
            idxstring.push_back(*it);
        }
    }
    else {
        idxstring=numstr.str();
    }

    fnstr<<idxstring<<filename.substr(last);

	fnstr.width(1);

	name=fnstr.str();

	string::size_type pos=name.find_last_of('.');
	if (pos!=string::npos)
		ext=name.substr(pos);
	else
		ext.clear();

	return 0;
}

int GetStrNum(char const * const str)
{
	int i=0;
	while (!::isdigit(str[i]) && str[i]) {
		i++;
	}

	if (!str[i]) return -1;

	return atoi(str+i);
}

int KIPLSHARED_EXPORT ExtractWildCard(const char *src, char *prefix, char *suffix)
{
	const char *pW=strpbrk(src,"*");
	if (!pW) {
		strcpy(prefix,src);
		suffix[0]=0;

		return -1;
	}

	strncpy(prefix,src,pW-src);
	prefix[pW-src]=0;

	strcpy(suffix,pW+1);
	return 0;
}

int KIPLSHARED_EXPORT CheckPathSlashes(std::string &path, bool bAddSlashAtEnd=true)
{
	int cnt=0;
	if (!path.empty()) {
		size_t idx=path.find(wrong_slash,0);

		while (idx!=path.npos) {
			path[idx]=slash;
			idx=path.find(wrong_slash,idx);
			cnt++;
		} 
		
		if ((bAddSlashAtEnd==true) && (*path.rbegin()!=slash))
			path=path+slash;
	}
	return cnt;
}

std::string KIPLSHARED_EXPORT GetFileExtension(const std::string fname)
{
   std::string ext;

   string::size_type pos=fname.find_last_of('.');
   if (pos!=string::npos)
       ext=fname.substr(pos);
   else
       ext.clear();

   return ext;
}


}}}
