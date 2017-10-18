//+**********************************************************************
//
// File:	edf_reader.cpp
//
// Project:	 Fable data to NeXus translation
//
// Description:	implementation of a edf_reader class. It is a library,
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

#include "edf_reader.h"

using namespace std;

/**
* Destructor method. It realese memory allocated for the image. 
*/
edf_reader::~edf_reader(){
    //
    // release the image array
    //
    delete [] image;  
}

/**
* Initial constructor 
*/    
edf_reader::edf_reader(){
    key_double.clear();
    key_string.clear();
    header_numbers[0]=0;
    header_numbers[1]=0;
    header_numbers[2]=0;
}

/**
* Constructor responsible for reading data from the EDF file.
* It reads data from the header and stores key-value pairs in dictionry.
* Also EDF image is read.
*/
edf_reader::edf_reader(ifstream &in){
    
    char buffer[256], value_string[256];
    double value;
    char a = ' ';
    bool is_header = 0;
    int dim1, dim2;
    //
    //read first character from the file
    //
    in.get(a);
    //
    // read file until bace closing the header is met
    //
    while(in.peek()!='}'){
        //
	//prevention from corrupted bits in file
	//
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(buffer, 255);
            break;
        }
	//
	//check if the header begins
	//
	if((a=='{') || is_header==1){
	    //
	    //set flag if first time in the loop 
	    //
	    if(!is_header){
	        in.get(a);
	        is_header = 1;
	    }
	    //
	    //get key 
	    //
	    in.getline(buffer, 255,' ');
	    string key(buffer);
	    //
	    //check if key corresponds to float value
	    //
	    if(!strcmp(buffer,"Center_1")||!strcmp(buffer,"Center_2")||!strcmp(buffer,"DDummy")||!strcmp(buffer,"Dummy")||!strcmp(buffer,"Offset_2")||!strcmp(buffer,"Psize_1")||!strcmp(buffer,"Psize_2")||!strcmp(buffer,"SampleDistance")||!strcmp(buffer,"SaxsDataVersion")||!strcmp(buffer,"WaveLength")||!strcmp(buffer,"EDF_BinarySize")||!strcmp(buffer,"Image")||!strcmp(buffer, "Offset_1")) {
	        
	        //
		//get value corresponding to key
		//
		in.getline(buffer, 255, '=');
		while(in.peek()==' ')in.ignore(1,'\n');
                in >> value;
		//
		//store pairs key-double value in key_double dictionary
		//
		key_double.insert(pair<string, double>(key, value));        
	    }
	    
	    //
	    //get dimension separately and store in dictionary
	    //
	    
	    else if(!strcmp(buffer,"Dim_1")){
	        in.getline(buffer, 255, '=');
		while(in.peek()==' ')in.ignore(1,'\n');
                in >> value;
		dim1 = (int)value;
		(void)dim1; // TODO unused varable - quiet warnings
		key_double.insert(pair<string, double>(key, value));	            
	    }
	    
	    //
	    //get dimension separately and store in dictionary
	    //
	    
	    else if(!strcmp(buffer,"Dim_2")){
	        in.getline(buffer, 255, '=');
		while(in.peek()==' ')in.ignore(1,'\n');
                in >> value;
		dim2 = (int)value;
		(void)dim2; // TODO unused varable - quiet warnings
		key_double.insert(pair<string, double>(key, value));	            
	    }
	    
	    //
	    //get size of image, store in the dictionary as well as in image_size field
	    //it is used to allocate space for EDF image in the memory
	    //
	    
	    else if(!strcmp(buffer,"Size")){
	        in.getline(buffer, 255, '=');
		while(in.peek()==' ')in.ignore(1,'\n');
                in >> value;
		image_size = (int)value;
		key_double.insert(pair<string, double>(key, value));	            
	    }
	    
	    //
	    //in case of HeaderID store in separate fields, not in dictionary
	    //
	    
	    else if(!strcmp(buffer,"HeaderID")){
	        in.getline(buffer, 255, ':');
	        in >> header_numbers[0];
		in.getline(buffer, 255, ':');
		in >> header_numbers[1];
		in.getline(buffer, 255, ':');
		in >> header_numbers[2];		
		in.getline(buffer, 255, ';');
	    }
	    
	    //
	    //values corresponding to the rest of keys store in key_string diciotnary
	    //
	    
	    else{
	        if(!strcmp(buffer," ")||!strcmp(buffer,"\n")||strlen(buffer)==0){
                    in.get(buffer, 255);
                }
                else{
                    in.getline(buffer, 255, '=');
                    while(in.peek()==' ')in.ignore(1,'\n');
                    in.getline(value_string, 255,';');
		    //
		    //store pairs key-double value in key_string dictionary
		    //
		    key_string.insert(pair<string, string>(key, value_string));       
	        }
	    }
	    
	    //
	    //go to next line    
	    //
	    do{
	        in.get(a);
	    } while(a!='\n');
	}	    
    }
    //
    //after header was read, read the image
    //
    try{
        //
	//allocate memory for the image
        //
	image = new char [image_size]; 
    }catch (bad_alloc xa){
        cout<<"Allocation Failure\n";
	exit(1);
    }
    //
    //go at the beginning of the image
    //
    in.getline(buffer, 255, '\n');
    //
    //read the image into the buffer
    //
    in.read(image, image_size);
    //
    //reset flags in stream for EOF
    //
    if(in.eof()){
        in.clear(in.rdstate() & ~ios::eofbit & ~ios::failbit);
    }
    //
    //rewind at the beginnig of the file
    //
    in.seekg(0, ios::beg);
}


/**
* Method returns current header value.
*/
int edf_reader::get_current_headerID(){
    
    return header_numbers[0];

}

/**
* Method returns previous header value.
*/
int edf_reader::get_previous_headerID(){
    
    return header_numbers[2];

}

/**
* Method returns next header value.
*/
int edf_reader::get_next_headerID(){
    
    return header_numbers[1];

}

/**
* Method returns pointer to the EDF image.
*/
char* edf_reader::get_image(){
    
    return image;

}

/**
* Method returns size of the image.
*/
int edf_reader::get_image_size(){
    
    return image_size;

}

/**
* Method returns number of entries in the key-double_values dictionary.
*/
int edf_reader::size_map_double(){

    return key_double.size();

}

/**
* Method returns number of entries in the key-string_values dictionary.
*/
int edf_reader::size_map_string(){

    return key_string.size();

}

/**
* Method returns the name of the key and corresponding value
* from key-double_value dictionary. The key number parameter decides
* which key is read.
* Parameters: input - int key_number
*             output - double value
* Returns: string of the key
*/
string edf_reader::get_key_double(int key_number, double &value){
    
    map<string, double>::iterator p;
    //
    //set iterator at the beginning of the dictionary
    //
    p = key_double.begin();
    //
    //go to   invoked pair
    //
    for(int i=1; i<key_number; i++)p++;
    //
    //read value
    //
    value = p->second;
    //
    //return key
    //
    return p->first;

}

/**
* Method returns the name of the key and corresponding value
* from key-string_value dictionary. The key number parameter decides
* which key is read.
* Parameters: input - int key_number
*             output - string value
* Returns: string name of the key
*/
string edf_reader::get_key_string(int key_number, string &value){

    map<string, string>::iterator p;
    p = key_string.begin();
    for(int i=1; i<key_number; i++)p++;    
    value = p->second;
    
    return p->first;

}

/**
* Method returns value corresponding to the key invoked by name(not position),
* from key-double_value dictionary. 
* Parameters: input - string key
*             output - double value
* Returns: true if key value was found, false otherwise
*/
bool edf_reader::double_by_key(string key, double &value){

    bool exist=0;
    map<string, double>::iterator p;
    //
    //look for key
    //
    p = key_double.find(key);
    //
    //key_double.end() is set if thekey does not exist in the dictionary
    //
    if(p!=key_double.end()){
        //
	//read the value corresponding to the key
	//
	value = p->second;
	//
	//key exist in the dictionary
	//
	exist=1;
    }
    
    return exist;
}

/**
* Method returns value corresponding to the key invoked by name(not position),
* from key-string_value dictionary. 
* Parameters: input - string key
*             output - string value
* Returns: true if key value was found, false otherwise
*/
bool edf_reader::string_by_key(string key, string &value){
    
    bool exist=0;
    map<string, string>::iterator p;
    
    p = key_string.find(key);
    
    if(p!=key_string.end()){
        value = p->second;
	exist=1;
    }    
    return exist;
}

