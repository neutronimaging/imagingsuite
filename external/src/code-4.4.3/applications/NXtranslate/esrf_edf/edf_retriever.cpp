//+**********************************************************************
//
// File:	edf_retriever.cpp
//
// Project:	 Fable data to NeXus translation
//
// Description:	code for implementing a retriever of data from edf files.
//		It is added into NXtranslate program in the form of plugin. 
//              By parsing a XML configuration file of
//              NXtranslate program using this plugin, data from EDF image 
//              header as well as the image itself can be read and stored
//              in NeXus file.
//
// Author(s):	Jaroslaw Butanowicz
//
// Original:	April 2006
//
//+**********************************************************************

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdio.h>
#include "edf_retriever.h"
#include "edf_reader.h"
#include "../node.h"
#include "../node_util.h"
#include "../string_util.h"
#include "../tree.hh"
//
//delimiter used to separate statements in location parameter.
//used in command parser
//
#define DELIMITER ':'    

using std::ifstream;
using std::invalid_argument;
using std::runtime_error;
using std::string;
using std::cout;
using std::endl;
using std::vector;

/** 
* Fuction parses location command from .xml configuration file.
* Command can be of the form letter or letter:variable_name.
* It does all the checkins for correctness of the command. There are
* only three letters allowed "A", "I", "S". Only in case of letter "S"(single label),
* variable_name parameter can be in the location.
* Parameters: input - string location(command)
*             output - char* key, 
* Returns: 0 in case of A letter, 1 in case of S letter, 
* 2 in case of L letter.
* In case of any other labels error will occur. 
*/
int command_parser(const string &location, char* key){
   
    int if_label=0; //initialization of flag 
    int size = location.size(); //check size of command string
    //
    //location cannot be empty
    //
    if(size<=0)
        throw invalid_argument(" cannot parse empty string ");
    // 
    //label only of the form A(all paramters from header) S:key(value for the given key is saved)	
    // or I(only image saved)
    //
    if(location[0]!='A' && location[0]!='S' && location[0]!='I')
        throw invalid_argument(" bad location parameter only A , I or S:key are allowed ");    
    //
    //check syntax for All keys and Image location(only letter A or I allowed)	
    //
    if(location[0]=='A' || location[0]=='I'){
        if(size!=1)
            throw invalid_argument(" for all only A for image only I ");
    }
    //
    //check syntax for Single key location(syntax-> S:key)
    //
    if(location[0]=='S'){
        if(size<3 || location[1]!=DELIMITER)
            throw invalid_argument(" Bad syntax. Correct syntax: S:key ");
    } 
    //  
    //case for Single key
    //
    if(location[0]=='S'){
        for(int i=2; i<size; i++){
            key[i-2]=location[i];//subscribe key
	}
	key[size-2]='\0';
	if_label=1;
    }
    //
    //case for Image 
    //
    if(location[0]=='I'){
	if_label=2;
    }
    return if_label;  //returns 2 for Image, 1 for Single key, 0 All
}




/**
 * The factory will call the constructor with a string. The string
 * specifies where to locate the data (e.g. a filename), but
 * interpreting the string is left up to the implementing code.
 */
EdfRetriever::EdfRetriever(const string &str): source(str){
    
    //
    // open the file
    //
    infile.open(source.c_str());
    //
    // check that open was successful
    //
    if(!infile.is_open())
        throw invalid_argument("Could not open file: "+source);

}

/**
* Destructor is closing the file opened for reading.
*/
EdfRetriever::~EdfRetriever(){
    //
    // close the file
    //
    if(infile)
        infile.close();
}

/**
* This is the method for retrieving data from a file. The whole
* tree will be written to the new file immediately after being
* called. Interpreting the string is made by command_parser method.
*/
void EdfRetriever::getData(const string &location, tree<Node> &tr){

    char key[256];//variable for holding variable name
    int label_case = command_parser(location, key);//parse location, and check for case with label
    //
    //case for single label  
    //
    if(label_case==1){
        double double_value;    
        string string_value;
        int dims[1]={0};
        edf_reader Edf = edf_reader(infile);//parse edf file and built object with header values
	string key_string(key);//built string from the name of variable
	//
	//go thru the whole double value dictionary and look for key
	//if key exists read double value, and store in the NeXus file in the place from which it was invoked
	//
	if(Edf.double_by_key(key_string, double_value)){
            
	    double valueD[1]={double_value}; 	
	    dims[0] = 1;	
	    Node node(key, (void*)valueD, 1, dims, NX_FLOAT64);
            tr.insert(tr.begin(),node);
	}
	else {
	
	//
	//go thru the whole string dictionary and look for key
	//if key exists read string value, and store in the NeXus file in the place from which it was invoked
	//    
	    if(Edf.string_by_key(key_string, string_value)){
		
	        const int size=string_value.size();//check size of string
	        std::vector<char> valueS(size);
		for(int i=0; i<size; i++){
		    valueS[i]=string_value[i];//translate C++ string into C-style string, NXtranslate convetion
		}
		dims[0]=size;
		Node node(key, (void*)&(valueS[0]), 1, dims, NX_CHAR);
                tr.insert(tr.begin(),node);	        
	    }
	    //
	    //key does not exist in any of both dictionaries, store empty string
	    //
	    else{
	    
	        cout<<" Key "<<key_string<<" does not exist in the header. Empty string is saved.\n";
	        char empty[1];
		Node node(key, (void*)empty, 1, dims, NX_CHAR);
                tr.insert(tr.begin(),node);	    
	    
	    }    	
	}
    }
    //
    //case of single Image(I letter)
    //
    else if(label_case==2){
        //
	//read EDF file and create object with header values and image
	//
	edf_reader Edf = edf_reader(infile);
	//
	//check the size of image
	//
	int image_dim[1]={Edf.get_image_size()};
        //
	//store image in the place of the NeXus hierarchy from the command was invoked in .xml file
	//
	Node node_data("Image", (void*)Edf.get_image(), 1, image_dim, NX_INT8);
	tr.insert(tr.end(),node_data);        
    
    }
    //
    //case for storing every keys from header with an image
    //
    else{
        
        double valueD[1];
        double double_value;    
        string string_value;
	int dims1[1]={1};
        //
	//create object with header values and Image
	//
	edf_reader Edf = edf_reader(infile);
	string key;
	//
	//get size of both dictionaries
	//
	int double_map_size = Edf.size_map_double();
        int string_map_size = Edf.size_map_string();
	//
	//iterators used for creation of hierarchical subtree
	//
	tree<Node>::iterator root, log, data;
	//
	//create NXentry root group, in the place where command was invoked,  
	//
	Node node("empty", "NXentry"); 
	root = tr.insert(tr.end(),node);//insert root to the tree
	//
	//create NXlog group for storing header values
	//group is a sub group of NXentry root group
	//
	Node log_node("EDF_log", "NXlog"); 
	log = tr.append_child(root,log_node);
        //
	//create NXdata group for storing EDF image
	//group is a sub group of NXentry root group		
	//
	Node data_node("EDF_image", "NXdata"); 
	data = tr.append_child(root,data_node);	
	//
	//get every keys and values from dicitonary of double values
	//
	for(int i=1; i<double_map_size+1; i++){
	    key=Edf.get_key_double(i, double_value);
	    valueD[0]=double_value;
	    //
	    //store pairs key-double value in NXlog group
	    //
	    Node node_child(key, (void*)valueD, 1, dims1, NX_FLOAT64);
            tr.append_child(log,node_child);	    
	}
        //
	//get every keys and values from dicitonary of string values
	//
	for(int i=1; i<string_map_size+1; i++){
	    key=Edf.get_key_string(i, string_value);
	    int size=string_value.size();
	    dims1[0]=size;
	    std::vector<char> valueS(size);
	    //  
	    //convert C++ string into C-style string, NXtranslate convention
	    //
	    for(int i=0; i<size; i++){
	        valueS[i]=string_value[i];
            }
	    //
	    //store pairs key-string value in NXlog group
	    //
	    Node node_child(key, (void*)&(valueS[0]), 1, dims1, NX_CHAR);
            tr.append_child(log,node_child);	    
	}
	//
	//prepare image size 
	//
	int image_dim[1]={Edf.get_image_size()};
        //
	//store edf-image in NXdata group
	//
	Node node_data("Image", (void*)Edf.get_image(), 1, image_dim, NX_INT8);
	tr.append_child(data,node_data);
    }
}	
//
//indicate mime type
//
const string EdfRetriever::MIME_TYPE("edf");

string EdfRetriever::toString() const{
    return "["+MIME_TYPE+"] "+source;
}
 
