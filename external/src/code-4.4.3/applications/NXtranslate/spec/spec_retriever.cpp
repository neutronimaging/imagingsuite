//+**********************************************************************
//
// File:	spec_retriever.cpp
//
// Project:	 Fable data to NeXus translation
//
// Description:	code for implementing a retriever of data from spec files.
//		It is added into NXtranslate program in the form of plugin. 
//              By parsing a XML configuration file of
//              NXtranslate program using this plugin, data from SPEC tags 
//              can be read and stored in NeXus file.
//
// Author(s):	Jaroslaw Butanowicz
//
// Original:	March 2006
//
//+**********************************************************************

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdio.h>
#include "spec_retriever.h"
#include "SPEClib.h"
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
* Function parses location command from .xml configuration file.
* It divides command on three parts. letter:label:scan_number.
* It do all the checkins for correctness of the command. Command need 
* to be built from these three parts(letter, label, int) separated by colons ':'.
* Parameters: input - string location(command)
*             output - char letter, char *label, int scan number
* Returns: true if letter indicates that label is needed(L, O, U), 
* false for the case of letters(D, F, C) were label can be ommited.
* In case of any other labels error will occur. 
*/
bool command_parser(const string &location, char &letter, char *label, int &scan){
    bool if_label=0;
    int size = location.size(); 
    int counter=0;
    int second_delimiter;
    //
    //location cannot be empty
    //
    if(size<=0)
        throw invalid_argument(" cannot parse empty string ");
    //
    //check second place in string for delimiter(L:label:scna_number)	
    //
    if(location[1]!=DELIMITER)
        throw invalid_argument(" after the letter suppose to be delimiter ");    
    //
    //count delimiters in location, starting from first one	
    //
    for(int i=2; i<size; i++){
        if(location[i]==DELIMITER){
            second_delimiter=i;
            counter++;
        }
    }
    //
    //after the first supposed to be only one delimiter in the string
    //
    if(counter!=1)
        throw invalid_argument(" insert just two delimiters in the string "); 
    //
    //letter always on the first position
    //
    letter = location[0];  
    
    // 
    //case for tags with labels
    //
    if(letter=='L' || letter=='O' || letter=='U' ){
        for(int i=2; i<second_delimiter; i++){
            label[i-2]=location[i];//subscribe label
        }
        label[second_delimiter-2]='\0';
        if_label=1;
    }
    
    //
    //case for tags without labels
    //
    else if(letter=='F' || letter=='D' || letter=='C' )if_label=0;
  
    //
    //letter does not match any of cases
    //
    else
        throw invalid_argument(" the letter indicating label group is not supported ");
    //
    //temp is equal to last paramter in location(scan number)
    //
    string temp = string(location, second_delimiter+1, size-second_delimiter);
    
    //
    //check if scan number is a number  
    //
    if(temp[0]!='*'){
        scan = string_util::str_to_int(temp);
        //
	//scan number must be geater than zero
        //
	if(scan<=0)
            throw invalid_argument(" scan number must be bigger than zero ");
    }
    //
    //case where scan_number == '*', retrieve data from every scans
    //
    else scan=0;
  
    return if_label;  //returns 1 for tags with labels, 0 otherwise
}




/**
 * The factory will call the constructor with a string. The string
 * specifies where to locate the data (e.g. a filename), but
 * interpreting the string is left up to the implementing code.
 */
SpecRetriever::SpecRetriever(const string &str): source(str){
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

SpecRetriever::~SpecRetriever(){

    //
    // close the file
    //
    if(infile)
        infile.close();
}

/**
 * This is the method for retrieving data from a file. The whole
 * tree will be written to the new file immediately after being
 * called. Interpreting the string is left up to the implementing
 * code.
 */
void SpecRetriever::getData(const string &location, tree<Node> &tr){

    char letter;
    char label[BUFFER_SIZE];
    int scan_number;
    
    //
    //parse location, and check for case with label  
    //
    if(command_parser(location, letter, label, scan_number)){
        //
        //case for L tag
        //
	if(letter=='L'){
	    //
	    //iterators for setting the structure of the tree of NeXus file
	    //
	    tree<Node>::iterator root, first_gen, second_gen;
	    //
	    //case for every labels from the scan
	    //
	    if(!strcmp(label,"*")){
                
		string scan_group = "scan_";//template for name of the scan NXentry
                char lab[BUFFER_SIZE];//name of the label
                char number[5];//temporary array for storing scan number
                int MaxLabel; 
                //
		//case for every scans(L:*:*) 	    	     
                //
		if(scan_number==0){
                    auto_translation(tr);
                } 
		//
		//case for L:*:scan_number, store data of every labels for given scan  
                //
		else{
	            //
		    //create folder for the interesting scan
	            //
		    Node node("empty", "NXentry"); 
	            first_gen = tr.insert(tr.end(),node);
	            
		    //
		    //go to the scan and count number of measurements
	            //
		    scan_rewinder(infile, scan_number);
	        const int size_of_data = count_data_sets(infile);
	            if(!(size_of_data)){
	                cout<<" No data was collected in the scan "<<scan_number<<"\n";
	                exit(1);
	            }
	            //
		    //create array for storing data
	            //
		    std::vector<double> data(size_of_data);
                    const int dims[1]={size_of_data};
	            //
		    //count labels
	            //
		    scan_rewinder(infile,scan_number);
		    if(!(MaxLabel = count_columns(infile))){
	                cout<<" No labels in the scan "<<scan_number<<"\n";
	                exit(1);		    
		    }
		    //
		    //loop through every labels
	            //
		    for(int j=1; j<MaxLabel+1; j++){
		        //
			//get the name of the label j-th
	                //
			if(!get_label(infile, j, lab, scan_number)){
		            cout<<" Label number "<<j<<" in scan number "<<scan_number<<" does not exist. ";
		            strcpy(lab, "translation_error");
		        }
		        
			//
			//put '_' instead of every spaces in the name of the label    
			//
			for(int a=0; a<strlen(lab); a++){
			    if(lab[0]<57)lab[0]='A';
		            if(lab[a]==' ' || lab[a]=='(' || lab[a]==')')lab[a]='_';
		        }
		        //
			//get values corresponding to label j-th
	                //
			get_values(infile, j, &(data[0]), scan_number);
	            
			//
			//create NXdata group for storing label data item
		        //
			Node child_group(lab, "NXdata");
			//
			//append NXdata group to NXentry(scan) group
	                //
			second_gen = tr.append_child(first_gen, child_group);
		        //
			//insert data item into NXdata group
		        //
			Node node_child(lab, (void*)&(data[0]), 1, dims, NX_FLOAT64);
                        tr.append_child(second_gen,node_child);	      
	            }              
                }     
            }
	    //
	    //case for single label
            //
	    else{
	        
      	        int size_of_data;
	        int label_position;
		//	
		//single label from single scan
		//
		if(scan_number!=0){
                    //
		    //get position of label indicated by "label" parameter 
	            //
		    if(!(label_position = get_label_position(infile, label, scan_number))){
	                cout<<" Variable "<<label<<" is not in the scan number "<<scan_number<<"\n";
	                exit(1);
	            }
                    //
		    //go to scan
	            //
		    scan_rewinder(infile, scan_number);
	            //
		    //count data sets and prepare array for results
		    //
		    if(!(size_of_data = count_data_sets(infile))){
                        cout<<" No data was collected in the scan "<<scan_number<<"\n";
	                exit(1);
	            }
	            
	            std::vector<double> data(size_of_data);
                    const int dims[1]={size_of_data}; 
                    //
		    //store values read from scan
                    //
		    get_values(infile, label_position, &(data[0]), scan_number);
                    //
		    //insert data item with label values
	            //
		    Node node(location, (void*)&(data[0]), 1, dims, NX_FLOAT64);
                    tr.insert(tr.begin(),node);
	        }
		//
		//case for single label from every scans(L:label:*)
	        //
		else{
	            //
		    //prepare name for scan entry  
		    //
		    string scan_group = "scan_";
	            char number[5];
	            string item_name = string(label);
		    //
		    //insert "_" instead of every spaces in label name to name data item in nexus
		    //
		    for(int a=0; a<strlen(label); a++){
			if(label[0]<57)label[0]='A';
		        if(label[a]==' ' || label[a]=='(' || label[a]==')')label[a]='_';
		    }
		    //
		    //create entry for data corresponding to label from every scans 
	            //
		    Node node("empty", "NXentry");
                    first_gen = tr.insert(tr.end(),node);
	            //
		    //count scans
	            //
		    int scan_total=total_scans(infile);
                    //
		    //fetch data from every scans	
	            //
		    for(int i=1; i<scan_total+1; i++){
		        //
			//find postion of label
	                //
			if(!(label_position = get_label_position(infile, label, i))){
	                    cout<<" Variable "<<label<<" is not in the scan number "<<i<<"\n";
	                }
			//
			//prepare name of the scan
	                //
			sprintf(number,"%d",i);
	                scan_group.replace(5,5,number);
	                //
			//go to scan i and count data sets in order to prepare array for data
			//
			scan_rewinder(infile, i);
	                if(!(size_of_data = count_data_sets(infile))){
	                    cout<<" No data was collected in the scan "<<i<<"\n";
	                    exit(1);
			}
	                
	                std::vector<double> data(size_of_data);
                        const int dims[1]={size_of_data}; 
                        //
			//fetch data corresponding to label in i-th scan
	                //
			get_values(infile, label_position, &(data[0]), i);
                        //
			//insert data group with the name of the scan, 
	                //
			Node child_group(scan_group, "NXdata");
	                second_gen = tr.append_child(first_gen, child_group);
		        //
			//store plottable data item in the group NXdata, for given scan
                        //
			Node node_child(item_name, (void*)&(data[0]), 1, dims, NX_FLOAT64);
                        tr.append_child(second_gen,node_child);	
	
	            }              
	        }
            }
        }
        //
	//case for motors
        //
	else if(letter=='O'){
	    //
	    //iterators for setting the structure of the tree of NeXus file
	    //
	    tree<Node>::iterator root, first_gen, second_gen;
	    int motor_position;
	    int motor_group;
	    int motor_groups;
	    int motors_in_last_group;
	    double motor_value[1];//single values for motors
	    const int dims[1]={1};
	    //
	    //case for every motors from the scan
	    //
	    if(!strcmp(label,"*")){
                string scan_group = "moto_";
		char number[5];

		char lab[BUFFER_SIZE];//name of the label

                //		    
		//create folder for the interesting scan
	        //
		Node node("empty", "NXlog"); 
	        first_gen = tr.insert(tr.end(),node);
		    
	        scan_rewinder(infile, scan_number);
		//
		//count motor groups
                //
		if(!(motor_groups = motor_counter(infile))){  
                    cout<<"NO motors in the file\n";
                    exit(1);
                }
                //
                //count motors in last group, in the rest of the groups there is always 8
                //
		scan_rewinder(infile, scan_number);  
                if(!(motors_in_last_group = m_label_counter(infile, motor_groups-1 ))) {
                    cout<<"NO motors in last group\n";
                    exit(1);
                }
                //
		//loop through motors from every groups apart of the last one
	        //
		for(int j=0; j<motor_groups-1; j++){
		    for(int k=1; k<9; k++){
		        //   
		        //get the name of the k-th motor from the j-th motor group
		        //
			get_motor_name(infile, lab, j, k);
                        //
		        //put '_' instead of every spaces in the name of the label    
		        //
			for(int a=0; a<strlen(lab); a++){
			    if(lab[0]<57)lab[0]='A';
		            if(lab[a]==' ' || lab[a]=='(' || lab[a]==')')lab[a]='_';
	                }

		       //
		       //get position of the motor k-th from j-th
	               // 
			motor_value[0] = get_motor_value(infile, j, k, scan_number);
		        //
	                //insert data item with motor value into NXlog group
		        //
			Node node_child(lab, (void*)motor_value, 1, dims, NX_FLOAT64);
                        tr.append_child(first_gen,node_child);
		    }	      
	        }
		//
		//get motor positions from the last group 
		//
		for(int k=1; k<motors_in_last_group+1; k++){
		    //
		    //get the name of the k-th motor from the j-th motor group
		    //
		    get_motor_name(infile, lab, motor_groups-1, k);
                    //
		    //put '_' instead of every spaces in the name of the label    
		    //
		    for(int a=0; a<strlen(lab); a++){
			    if(lab[0]<57)lab[0]='A';
		            if(lab[a]==' ' || lab[a]=='(' || lab[a]==')')lab[a]='_';
	            }
		    
		    //    
		    //get position of the motor k-th from j-th
	            //
		    motor_value[0] = get_motor_value(infile, motor_groups-1, k, scan_number);
		    //    
	            //insert data item with motor value into NXlog group
		    //
		    Node node_child(lab, (void*)motor_value, 1, dims, NX_FLOAT64);
                    tr.append_child(first_gen,node_child);		
		}		
            }
	    //
	    //single motor
	    //
	    else{
	        //
		//single motor from single scan
	        //
		if(scan_number!=0){
	            //    
	            //look for motor position in the list of motors
	            //
		    if(!(motor_position = get_motor_position(infile, label, motor_group, scan_number))){
	                cout<<" Variable "<<label<<" is not in the scan number "<<scan_number<<"\n";
	                exit(1);
	            }        
                    //
	            //fetch motor value from interesting scan
	            //
		    motor_value[0] = get_motor_value(infile, motor_group, motor_position, scan_number);
                    //
	            //insert data item in the place from where it was called
    	            //
		    Node node(location, (void*)motor_value, 1, dims, NX_FLOAT64);
                    tr.insert(tr.begin(),node);
                }
	        //
		//single motor from every scans
	        //
		else{
	            //
		    //prepare name for scan entry  
		    //
		    string scan_group = "scan_";
	            char number[5];
	            string item_name=string(label);
		    //
		    //insert "_" instead of every spaces in label name to name data item in nexus
		    //
		    for(int a=0; a<strlen(label); a++){
			if(label[0]<57)item_name[0]='A';
		        if(label[a]==' ' || label[a]=='(' || label[a]==')')item_name[a]='_';
		    }
		    //
		    //create entry for data corresponding to motor from every scans 
	            //
		    Node node("empty", "NXentry");
	            first_gen = tr.insert(tr.end(),node);
		    //
		    //count scans
	            //
		    int scan_total=total_scans(infile);
                    //
		    //fetch data from every scans	
	            //
		    for(int i=1; i<scan_total+1; i++){
	                //
			//look for motor position in the list of motors
	                //
			if(!(motor_position = get_motor_position(infile, label, motor_group, i))){
	                    cout<<" Variable "<<label<<" is not in the scan number "<<i<<"\n";
	                    exit(1);
	                }        
                        //
	                //fetch motor value from interesting scan
	                //
			motor_value[0] = get_motor_value(infile, motor_group, motor_position, i);
		        //
		        //prepare name of the scan
	                //
			sprintf(number,"%d",i);
	                scan_group.replace(5,5,number);
		    	   
		        //
	                //insert log group with the name of the scan, 
	                //
			Node child_group(scan_group, "NXlog");
	                second_gen = tr.append_child(first_gen, child_group);
		        //
	                //store motor's value item in the group NXlog, for given scan
                        //
			Node node_child(item_name, (void*)motor_value, 1, dims, NX_FLOAT64);
                        tr.append_child(second_gen,node_child);
		    }
	        }
	    }
	}
	//
	//case for user notes tag
        //
	else if(letter=='U'){
	
            char refill[BUFFER_SIZE];
	    //
	    //case for refill mode
            //
	    if(!(strcmp(label, "mode"))){
                //
	        //go to scan
	        //
		scan_rewinder(infile, scan_number);
	        //
		//read refill mode
	        //
		if(!(refill_mode(infile, refill))){
	            cout<<" There is no refill mode indicated in scan "<<scan_number<<"\n";
	            cout<<" Empty string saved in the nexus file\n";
	            strcpy(refill," ");
	        } 
		//
		//prepare dimensions for storing refill mode       
	        //
		const int dims[1]={static_cast<int>(strlen(refill))};
	        //
		//store refill mode item in the place in hierarchy from where it was invoked
    	        //
		Node node(location, (void*)refill, 1, dims, NX_CHAR);
                tr.insert(tr.begin(),node);	  
            }
            //
	    //case for time of refill
            //
	    else if(!(strcmp(label, "time"))){
                //
		//go to scan
	        //
		scan_rewinder(infile, scan_number);
	        //
		//read time of refill, print error if it is not found
	        //
		if(!(refill_time(infile, refill))){
	            cout<<" There is no refill time indicated in scan "<<scan_number<<"\n";
	            cout<<" Empty string saved in the nexus file\n";
	            strcpy(refill," ");
	        } 
		//
		//prepare dimension parameter for storing time of refill information       
	        //
		const int dims[1]={static_cast<int>(strlen(refill))};
	        //
		//store refill time in the place from invoked
    	        //
		Node node(location, (void*)refill, 1, dims, NX_CHAR);
                tr.insert(tr.begin(),node);	  
            }
       
            else {
                //
		//letter does not match anything in SPEC file
	        //
		cout<<" Wrong label "<<label<<" for U mode.\n";
	        exit(1);
            }   
        }
    }  
    //
    //case for tags which do not need label argument
    //
    else{
    
        char result[BUFFER_SIZE];
        //
	//check letter
        //
	switch(letter){
            //
	    //file name tag
	    //
	    case 'F':
                //
		//read file name of SPEC file 
		//
		if(!get_file_name(infile, result)){
	            cout<<" File name not specified. Empty string written into the nexus file.\n";
	            strcpy(result," ");
	        }
                break;
	    //
	    //comment tag(user ID is specified there)
            //
	    case 'C':
                //
		//read user ID
		//
		if(!get_userID(infile, result)){
	            cout<<" User Id not specified. Empty string written into the nexus file.\n";
	            strcpy(result," ");
	        }	
                break;
	    //
	    //scan data tag
            //
	    case 'D':
	    	if(scan_number!=0){
		    //
		    //go to the scan
	            //
		    scan_rewinder(infile, scan_number);
	            //
		    //read date of the scan 
		    //
		    if(!get_date(infile, result)){
	                cout<<" Date of scan "<<scan_number<<" not specified. Empty string written into the nexus file.\n";
	                strcpy(result," ");
	            }
		}
		//
                //get dates of every scans	        
		//
		else{
		    //
		    //prepare iterators for the hierarchy
		    //
		    tree<Node>::iterator root;
		    //
	            //prepare name for scan date  
		    //
		    string scan_group = "date_of_scan_";
	            char number[5];
	            //
		    //create entry for data corresponding to dates of every scans 
	            //
		    Node node("empty", "NXlog");
	            root = tr.insert(tr.end(),node);
	            //
		    //count scans
	            //
		    int scan_total=total_scans(infile);
                    //
		    //fetch data from every scans	
	            //
		    for(int i=1; i<scan_total+1; i++){
		        //
			//go to the scan
	                //
			scan_rewinder(infile, i);
			//
			//get date of the file
		        //
			if(!get_date(infile, result)){
	                cout<<" Date of scan "<<scan_number<<" not specified. Empty string written into the nexus file.\n";
	                strcpy(result," ");
	                }
			//
			//prepare name of the scan
	                //
			sprintf(number,"%d",i);
	                scan_group.replace(13,5,number);
	                
	                
                        const int dims[1]={static_cast<int>(strlen(result))};
		        //
                        //store just read data in the place from invoked 
                        //
			Node node_child(scan_group, (void*)result, 1, dims, NX_CHAR);
                        tr.append_child(root,node_child);	
	
	            }              
	        }
                break;

            default:
	        break; //it will never go here
        }  
        if(scan_number!=0){
            const int dims[1]={static_cast<int>(strlen(result))};
            //
	    //store just read data in the place from invoked 
            //
	    Node node(location, (void*)result, 1, dims, NX_CHAR);
            tr.insert(tr.begin(),node);	 
        }
    } 

}
//
//indicate mime type
//
const string SpecRetriever::MIME_TYPE("spec");

string SpecRetriever::toString() const{
    return "["+MIME_TYPE+"] "+source;
}
 

/** 
* Method is used for automatic translation of the file.
* Details about the rules of translation are in the documantation of the retriever.
* The method is called from the SpecRetriever::getDate() method, when
* location paramter is "L:*:*". The whole Spec file is translated automatically. 
*/
void SpecRetriever::auto_translation(tree<Node> &t){

    string scan_group = "scan_";//template for name of the scan NXentry
    char lab[BUFFER_SIZE];//name of the label
    char number[5];//temporary array for storing scan number
    int MaxLabel; 
    int size_of_data;

    tree<Node>::iterator root, first_gen, second_gen;
    //
    //set number of scans from the file
    //
    int MAX =total_scans(infile);
    //
    //for text fields of date, userId, file name
    //
    char result[BUFFER_SIZE];
    //
    //create root folder, it is advised to call for L:*:* from root level 
    //
    Node node("empty", "NXroot"); 
    root = t.insert(t.end(),node);//insert root to the tree
 
    Node node_user("User_data", "NXuser"); 
    first_gen = t.append_child(root,node_user);                   
    //		    
    //read file name of SPEC file 
    //
    if(!get_file_name(infile, result)){
        cout<<" File name not specified. Empty string written into the nexus file.\n";
	strcpy(result," ");
    }
    //
    //dimensions of file name
    //
    const int dims_file[1]={static_cast<int>(strlen(result))};
    //	    
    //store file name in the user group 
    //
    Node node_file("file_name", (void*)result, 1, dims_file, NX_CHAR);
    t.append_child(first_gen,node_file);
    //	    
    //read user ID
    //
    if(!get_userID(infile, result)){
        cout<<" User Id not specified. Empty string written into the nexus file.\n";
	strcpy(result," ");
    }
    //
    //dimensions of userId
    //
    const int dims_id[1]={static_cast<int>(strlen(result))};
    //	    
    //store user Id in the user group
    //
    Node node_id("user_id", (void*)result, 1, dims_id, NX_CHAR);
    t.append_child(first_gen,node_id);		    
		    		    
    //	    
    //go thru every scans 
    //
    for(int i=1; i<MAX+1; i++){
    //
    //set the name of every scans(NXentry)
    //
	sprintf(number,"%d",i);
	scan_group.replace(5,5,number);
        //	                
	//create NXentry folders for every scans,    
	//
	Node node_parent(scan_group, "NXentry"); 
	//
	//append as a childs to the root in the tree hierarchy
	//
	first_gen = t.append_child(root,node_parent);	    
	//                
	//go to the scan i and set number of data sets obtained during experiment    
	//
	scan_rewinder(infile, i);
	if(!(size_of_data = count_data_sets(infile))){
	    cout<<" No data was collected in the scan "<<i<<"\n";
	    exit(1);
	}
	//                
	//create array for data sets
	//
	std::vector<double> data(size_of_data);
        const int dims[1]={size_of_data};
	//               
	//go back to scan and count every labels
	//
	scan_rewinder(infile,i);
	if(!(MaxLabel = count_columns(infile))){
	    cout<<" No labels in the scan "<<i<<"\n";
	    exit(1);		    
        }
	//	        
	//loop through every labels
        //
	for(int j=1; j<MaxLabel+1; j++){
	//                    
	//get name of the j-th label in scan i-th    
	//
	if(!get_label(infile, j, lab, i)){
	    cout<<" Label number "<<j<<" in scan number "<<i<<" does not exist. ";
	    strcpy(lab, "translation_error");
        }
	//
	//put underline sign for every spaces in the name of the label
        //
	for(int a=0; a<strlen(lab); a++){
            if(lab[0]<57)lab[0]='A';
            if(lab[a]==' ' || lab[a]=='(' || lab[a]==')')lab[a]='_';
        } 
	//	            
	//get the results coresponding to j-th label in i-th scan 
	//
	get_values(infile, j, &(data[0]), i);
	//            
	//create folder to keep data items
	//
	Node child_group(lab, "NXdata");
	//
	//append folder to the NXentry folder representing i-th scan
        //
	second_gen = t.append_child(first_gen, child_group);
	//	            
	//store data items in NXdata folder 
	//
	Node node_child(lab, (void*)&(data[0]), 1, dims, NX_FLOAT64);
        t.append_child(second_gen,node_child);	      
        }
	//		
	//add motors to the subtree
	//
	int motor_position;
	int motor_group;
	int motor_groups;
	int motors_in_last_group;
	double motor_value[1];//single values for motors
	const int dims_moto[1]={1}; 
	string moto_group = "moto_";

	char lab[BUFFER_SIZE];//name of the label
 
	//	    
	//create folder for the interesting scan
	//
	Node node_motors("Motors", "NXlog"); 
	second_gen = t.append_child(first_gen,node_motors);
		    
        scan_rewinder(infile, i);
	//
	//count motor groups
        //
	if(!(motor_groups = motor_counter(infile))){  
            cout<<"NO motors in the file\n";
            exit(1);
        }
        //
        //count motors in last group, in the rest of the groups there is always 8
        //
	scan_rewinder(infile, i);  
        if(!(motors_in_last_group = m_label_counter(infile, motor_groups-1 ))) {
            cout<<"NO motors in last group\n";
            exit(1);
        }
        //
	//loop through motors from every groups apart of the last one
	//
	for(int j=0; j<motor_groups-1; j++){
	    for(int k=1; k<9; k++){
	        //
	        //get the name of the k-th motor from the j-th motor group
		//
		get_motor_name(infile, lab, j, k);
                //
		//put '_' instead of every spaces in the name of the label    
		//
		for(int a=0; a<strlen(lab); a++){
		    if(lab[0]<57)lab[0]='A';
		    if(lab[a]==' ' || lab[a]=='(' || lab[a]==')')lab[a]='_';
	        }
	       //	        
	       //get value of the motor k-th from j-th
	       // 
		motor_value[0] = get_motor_value(infile, j, k, i);
                //       
	        //insert data item with motor value into NXlog group
		//
		Node node_motor(lab, (void*)motor_value, 1, dims_moto, NX_FLOAT64);
                t.append_child(second_gen,node_motor);
            }     
	}
	//
	//get motor positions from the last group 
	//
	for(int k=1; k<motors_in_last_group+1; k++){
            //
	    //get the name of the k-th motor from the j-th motor group
            //
	    get_motor_name(infile, lab, motor_groups-1, k);
	    //
	    //put '_' instead of every spaces in the name of the label    
	    //
	    for(int a=0; a<strlen(lab); a++){
                if(lab[0]<57)lab[0]='A';
		if(lab[a]==' ' || lab[a]=='(' || lab[a]==')')lab[a]='_';
            }
            //
            //get value of the motor k-th from j-th
	    //
	    motor_value[0] = get_motor_value(infile, motor_groups-1, k, i);
            //        
	    //insert data item with motor value into NXlog group
            //
	    Node node_child(lab, (void*)motor_value, 1, dims_moto, NX_FLOAT64);
            t.append_child(second_gen,node_child);		
        }
	//
	//insert dates of scanning into NXlog group
	//go to the scan
	//
	scan_rewinder(infile, i);
	//
	//read date of the scan 
	//
	if(!get_date(infile, result)){
	    cout<<" Date of scan "<<i<<" not specified. Empty string written into the nexus file.\n";
	    strcpy(result," ");
        }
	const int dims_date[1]={static_cast<int>(strlen(result))};
        //
        //go to the scan
	//
	scan_rewinder(infile, i);
	//                
	//store data in NXlog group
        //
	Node node_date("date_of_scan", (void*)result, 1, dims_date, NX_CHAR);
        t.append_child(second_gen,node_date);	    
    }
}
