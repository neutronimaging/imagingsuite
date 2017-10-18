//+**********************************************************************
//
// File:	edf_reader.h
//
// Project:	 Fable data to NeXus translation
//
// Description:	definition of a edf_reader class. It is a library,
//              which makes possible reading data from EDF format.
//		EDF file is read using the constructor of the class.
//              Every key-values pairs from the header are stored   
//              in the one of two dictionaries(private members).
//              Image is read into the memory allocated dynamically, 
//              and a pointer on that memory is a class's private 
//              member.
//
// Author(s):	Jaroslaw Butanowicz
//
// Original:	April 2006
//
//+**********************************************************************

#ifndef EDF_READER_H
#define EDF_READER_H 

#include <iostream> 
#include <fstream>
#include <map>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std; 


class edf_reader
{
    private:
        
        int header_numbers[3];
        map<string, double>key_double;
	map<string, string>key_string;
        int image_size;
	char *image;
    
    public:
        
        ~edf_reader();
        edf_reader();
        edf_reader(ifstream &in);
	int get_current_headerID();   
        int get_previous_headerID();
	int get_next_headerID();
	int size_map_double();
        int size_map_string();
	char* get_image();
	int get_image_size();
	string get_key_double(int key_number, double &value);
        string get_key_string(int key_number, string &value);
        bool double_by_key(string key, double &value);
        bool string_by_key(string key, string &value);
};



#endif

