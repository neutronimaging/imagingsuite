//+**********************************************************************
//
// File:	SPEClib.cpp
//
// Project:	 Fable data to NeXus translation
//
// Description:	implementation of a SPEClib functions. It is a library,
//              which makes possible reading data from SPEC format.
//              The library consists set of functions which parse
//              SPEC file(pure ASCI file). Every single function
//              retrieves different information from the file.
//
// Author(s):	Jaroslaw Butanowicz
//
// Original:	March 2006
//
//+**********************************************************************

#include "SPEClib.h"

/**
* Function reads file name from SPEC file. 
* Parameters: input - input file stream, 
*             output - char *result for storing file name
* Returns: true if file name was specified in the SPEC, false otherwise  
*/
bool get_file_name(ifstream &in, char *result){
    bool empty=0;
    char a,b;
  
    in.seekg(0, ios::beg);//rewind stream
    //
    //stop searching for tag when #S will be met
    //
    while(!(a =='#' && b =='S')){
        //
	//prevent from corrupted part of file
	//
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(result, BUFFER_SIZE-1);
            break;
        }
	//
	//get first sign in line
        //
	in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
	//
	//get the second one
        //
	in.get(b);
        //
	//check if file tag was met
	//
	if(a=='#'&& b=='F'){
            empty=1;//indicate data collection
            while(in.peek()==' ') in.ignore(3,' ');
            in.getline(result, BUFFER_SIZE-1);//read file name
            break;
        }
	//
	//go to next line if #F was not met
        //
	else
            in.getline(result, BUFFER_SIZE-1);
    }
    //
    //after data collection rewind file
    //
    in.seekg(0, ios::beg);
    return empty; 
}


/**
* Function reads date of given scan from SPEC file. 
* Parameters: input - input file stream, 
*             output - char *result for storing scanning date
* Returns: true if date of scan was specified in the SPEC, false otherwise  
*/
bool get_date(ifstream &in, char *result){
    bool empty=0;
    char a,b;
    //
    //finish searching when #L is met
    //
    while(!(a =='#' && b =='L')){
        //
	//prevention from corrupted bits in file
	//
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(result, BUFFER_SIZE-1);
            break;
        }
	//
	//get first sign in line    
        //
	in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
	//
	//get the second one
        //
	in.get(b);
	//
	//case for data tag
        //
	if(a=='#'&& b=='D'){
            empty=1;
            while(in.peek()==' ') in.ignore(3,' ');
	    //read date of the scan
            in.getline(result, BUFFER_SIZE-1);
            break;
        }
	//
	//go to next line in case wrong tag
        //
	else
            in.getline(result, BUFFER_SIZE-1);
    }
    //
    //rewind file when job is finished
    //
    in.seekg(0, ios::beg);
    return empty; 
}


/**
* Function reads user ID of an owner of the SPEC file. 
* Parameters: input - input file stream, 
*             output - char *result for storing user ID
* Returns: true if user ID was specified in the SPEC, false otherwise  
*/
bool get_userID(ifstream &in, char *result){
    bool empty=0;
    char a,b;
    //
    //finish searching when tag of new scan is met
    //
    while(!(a =='#' && b =='S')){
        //
	//prevention from bad bits in the file
	//
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(result, BUFFER_SIZE-1);
            break;
        }
	//
	//get first sign from the line      
        //
	in.get(a);
        while(isspace(a)!=0){//rewind if space
            in.get(a);
        }
	//
	//get the second sign from the file
        //
	in.get(b);	
	//
	//tag with user ID 
        //
	if(a=='#'&& b=='C'){
            empty=1;
            in.getline(result, BUFFER_SIZE-1, '=');//go to user ID after '='sign
            while(in.peek()==' ') in.ignore(3,' ');//ignore white spaces after '='
            in.getline(result, BUFFER_SIZE-1);//read user ID
            break;
        }
	//
	//go to next line in case of wrong label
        //
	else
            in.getline(result, BUFFER_SIZE-1);
    }
    //
    //rewind file when job is finished
    //
    in.seekg(0, ios::beg);
    return empty; 
}



/**
* Function reads refilling mode of a scan from SPEC file. 
* Parameters: input - input file stream, 
*             output - char *result for storing refilling mode
* Returns: true if mode was specified for the scan, false otherwise  
*/
bool refill_mode(ifstream &in, char *result){
    bool exists=0;
    char a,b;
    int u_counter=0;
    //
    //finish when tag #L is met
    //
    while(!(a =='#' && b =='L')){
        //
	//prevention from corrupted bits
	//
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(result, BUFFER_SIZE-1);
            break;
        }
	//
	//get first sign in the line
        //
	in.get(a);
        while(isspace(a)!=0){
            in.get(a);//rewind white spaces
        }
	//
	//get the second sign from the file
        //
	in.get(b);
	//
	//case for user notes tag
        //
	if(a=='#'&& b=='U'){
            if(u_counter==2){//we are interested in #U2 tag
                exists=1;
                in.getline(result, BUFFER_SIZE-1, ':');//go to refill mode section
                while(in.peek()==' ') in.ignore(3,' ');//ignore white spaces after ':'
                in.getline(result, BUFFER_SIZE-1, ' ');//read mode of refill
                break;
            }
            else u_counter++;//if #U but not #U2
        }
	//
	//go to next line if tag does not match
        //
	in.getline(result, BUFFER_SIZE-1);
    }
    //
    //dont close the stream even if EOF is met
    //
    if(in.eof()){
        in.clear(in.rdstate() & ~ios::eofbit & ~ios::failbit);
    }
    //
    //check for bad bits
    //
    if(in.fail()){
        in.clear(in.rdstate() & ~ios::failbit);
    } 
    //
    //rewind file when job is finished
    //
    in.seekg(0, ios::beg);
    return exists; 
}



/**
* Function reads refilling time of a scan from SPEC file. 
* Parameters: input - input file stream, 
*             output - char *result for storing refilling time
* Returns: true if refill time was specified for the scan, false otherwise  
*/
bool refill_time(ifstream &in, char *result){
    bool exists=0;
    char a,b;
    int u_counter=0;
    //
    //finish searching when #L is met
    //
    while(!(a =='#' && b =='L')){
        //
	//bad bit prevention
	//
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(result, BUFFER_SIZE-1);
            break;
        }
	//
	//get first sign in line
        //
	in.get(a);
        while(isspace(a)!=0){
            in.get(a);//rewind white spaces
        }
	//
	//get second sign in line
        //
	in.get(b);
	//
	//#U tag met
        //
	if(a=='#'&& b=='U'){
            if(u_counter==2){//we look for #U2
                exists=1;
                in.getline(result, BUFFER_SIZE-1, ' ');
                while(in.peek()==' ') in.ignore(3,' ');
                in.getline(result, BUFFER_SIZE-1, ',');//get time of refill
                break;
            }
            else u_counter++;//if #U but not #U2
        }
        in.getline(result, BUFFER_SIZE-1);//go to next line if not #U
    }
    //
    //do not finish working with stream in case of EOF notified
    //
    if(in.eof()){
        in.clear(in.rdstate() & ~ios::eofbit & ~ios::failbit);
    }
    //
    //bad bit prevention
    //
    if(in.fail()){
        in.clear(in.rdstate() & ~ios::failbit);
    } 
    //
    //rewind stream when job is finished
    //
    in.seekg(0, ios::beg);
    return exists; 
}



/**
* Function returns position of the given label.
* It searches in #L group of labels if interested label exists.
* If yes, position number is returned.
* Parameters: input - input file stream, char *label, int number of scan 
*             
* Returns: int position of a label or zero if label does not exists  
*/
int get_label_position(ifstream &in, char *label, int scan){ 
    int position;
    //
    //go to the scan
    //
    scan_rewinder(in, scan);
    int MAX=count_columns(in);
    char temp[50];
    char str[BUFFER_SIZE];
    char a=' ';
    char b=' ';
    int counter=1;
    //
    //finish searching when #S tag is met 
    //
    while(!(a =='#' && b =='S')){
        //
	//fail bit prevention
        //
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(str, BUFFER_SIZE-1);
        }
	//
	//get first sign from the line
        //
	in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
	//
	//second sign from the line
        //
	in.get(b);
	//
	//check for #L tag
        //
	if(a=='#'&& b=='L'){
	    //
	    //go to first label
	    //
	    while(in.peek()==' ') in.ignore(3, ' ');
            //
	    //check labels apart of the last one
	    //
	    while(counter<MAX){ 
                in.getline(str, 49, ' ');          
                while(in.peek()!=' '){
                    strcat(str, " ");//labels can have one white spaces in the name
                    in.getline(temp, 49, ' ');
                    strcat(str, temp);           
                }
	        if(!strcmp(label, str))break;//chack if label matches
                counter++;//go to next label
                while(in.peek()==' '){//rewind white spaces
                    in.ignore(3,' ');
                } 
            }
	    //
	    //case for the last label
            //
	    if(counter==MAX)in.getline(str, 150);
            if(strcmp(label, str))counter=0;//check if last label matches
            break;
        }
        else
            in.getline(str, 255);//go to next line if tag doesnot match
    }
    //
    //rewind file when finished
    //
    in.seekg(0, ios::beg);
    return counter;
}


/**
* Function counts columns of #L measurements. Used in get_label_position(...)
* function.
* Parameters: input - input file stream,  
*             
* Returns: int number of columns, zero in case of error  
*/
int count_columns(ifstream &in){
    long int columns_number=0;
    char string[BUFFER_SIZE];
    char a,b;
    //
    //end if #L is met
    //
    while(!(a =='#' && b =='L')){
        //
	//fail bit prevention
	//
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(string, BUFFER_SIZE-1);
        }
	//
	//get first sign in the line
        //
	in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
	//
	//get the second one
        //
	in.get(b);
        if(a=='#'&& b=='N'){
            in >> columns_number;//read number of columns
            break;
        }
        else
	    //
	    //go to next line if tag is wrong
            //
	    in.getline(string, 255);
    }
    return columns_number; 
}



/**
* Function reads data obtained during experiment for the given variable.
* Variable is inserted into the function by its position.
* To get position of the variable use get_label_position(...) function.
* Parameters: input - input file stream, int position, int scan number, 
*             output - double results[] - array with results obtained during experiment.
* Returns: true if data was read, false otherwise  
*/
bool get_values(ifstream &in, int number, double results[], int scan_number){
    double value;
    int counter=0; 
    bool flag=0;
    //
    //label position must be greater then zero
    //
    if(number == 0){
        cout<<"Uncorrect number of label\n";
        exit(1);
    }
    //
    //go to the scan
    //
    scan_rewinder(in, scan_number);
    //
    //finish working until EOF
    //
    while(!in.eof()){
        //
	//fail bit prevention
        //
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
        }
	//
	//always look for EOF
        //
	if(in.peek()==EOF)break;
	//
	//ignore rows with # sign or starting with white space
        //
	if(in.peek()=='#' || isspace(in.peek())!=0){
            in.ignore(500, '\n');
            if(flag==1)break;
            continue;      
        }
	//
	//if values section read values of the interesting label indicated by position 'number'
        //
	for(int i=0; i<number; i++){
            in >> value;
        }
	//
	//put vaules into the array
        //
	results[counter]=value;
        counter++;
        flag=1;
        in.ignore(500,'\n');
    }
    //
    //reset stream flags in case of EOF
    //
    if(in.eof()){
        in.clear(in.rdstate() & ~ios::eofbit & ~ios::failbit);
    }
    //
    //rewind file when finished
    //
    in.seekg(0, ios::beg); 
    return flag;
}



/**
* Function counts data sets during one scanning procedure.
* Main purpose of the function is to allocate enough memory 
* in the array for results.
* Parameters: input - input file stream,  
*             
* Returns: int number data sets, zero if no data was collected  
*/
int count_data_sets(ifstream &in){
    int data_sets=0;
    bool flag=0; 
    //
    //search till EOF
    //
    while(!in.eof()){
        //
	//fail bit prevention
	//
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
        }
	//
	//ignore lines starting with '#' or white space
        //
	if((in.peek()=='#' || isspace(in.peek())!=0)){
            in.ignore(500, '\n');
            if(flag==1)break;//if already read break
            continue;      
        }
	//
	//count rows with data
        //
	if(in.peek()!=EOF)data_sets++;
        in.ignore(500, '\n');
        flag=1;//set flag indicationg that it was read already
    }
    //
    //clear flag in case of EOF
    //
    if(in.eof()){
        in.clear(in.rdstate() & ~ios::eofbit & ~ios::failbit);
    }
    //
    //rewind file when finished
    //
    in.seekg(0, ios::beg);
    return data_sets;
}




/**
* Function returns position of given motor for the specified scan,
* as well as number of motor group where that motor belongs to. 
* Function compares name of the motor we are interested in with all the motors
* from the given scan.
* Parameters: input - input file stream, char *label with name of the motor,
              int scan number
*             output - int motor group where given motor is
* Returns: int position of motor in the group, or zero if motor does not belogs to any,  
*/
int get_motor_position(ifstream &in, char *label, int &motor_group, int scan){
    char str[BUFFER_SIZE];
    char temp1[BUFFER_SIZE], temp2[50];
    char a,b;
    bool new_motors = 0;
    int total_motor_groups, motors_in_last_group;
    int counter=1;
    //
    //go to scan and check if new motors are specified  
    //
    if(scan>1){
        scan_rewinder(in, scan);
        new_motors = is_motor(in);
    }
    //
    //if there are new motors in the scan go there
    //
    if(new_motors)scan_rewinder(in, scan);
    //
    //count motor groups
    //
    if(!(total_motor_groups = motor_counter(in))){  
        cout<<"NO motors in the file\n";
        exit(1);
    }
    //
    //count motors in last group, in the rest of the groups there is always 8
    //
    scan_rewinder(in, scan);  
    if(!(motors_in_last_group = m_label_counter(in, total_motor_groups-1 ))) {
        cout<<"NO motors in last group\n";
        exit(1);
    }
    //
    //if new motors in the scan go there, else work at the beginnig of the file 
    //
    if(new_motors)scan_rewinder(in, scan);
  
    motor_group = 0;
    //
    //stop searching for motors if #L is met
    //
    while(!(a =='#' && b =='L')){
        //
	//fail bit prevention
	//
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(str, BUFFER_SIZE-1);
            break;
        }
	//
	//read first sign in line
        //
	in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
	//
	//get the second one
        //
	in.get(b);
        //
	//motor section
        //
	if(a=='#'&& b=='O'){
            //
	    //go through every motor groups when searching for the motor
            //
	    while(motor_group<total_motor_groups-1){
                //
		//ignore motor number (i.e. #O4)
		//
		in.ignore(7,' ');
		//
		//ignore white spaces until first motor
	        //
		while(in.peek()==' ') in.ignore(1, ' ');
		//
		//check until last motor
                //
		while(counter<8){
		    //
		    //get motor name
                    //
		    in.getline(temp1, 49, ' ');          
                    while(in.peek()!=' '){
                        strcat(temp1, " ");//single one spaces can be in the motor name
                        in.getline(temp2, 49, ' ');
                        strcat(temp1, temp2);            
                    } 
                    if(!strcmp(label, temp1)){//check if motor match
                        break;
                    }
                    counter++;//increment counter of labels in the group if not found
                    //
		    //get rid of white spaces separating motors
		    //
		    while(in.peek()==' '){
                        in.ignore(1,' ');
                    } 
                }
		//
		//case of last motor in group
                //
		if(counter!=8)break;
                //
		//get last moto and check if matches
		//
		else{
                    in.getline(temp1, 150);
                    if(!strcmp(label, temp1))break;
                    motor_group++;//if not go to next group
                    counter=1;
                }
            }
	    //
	    //case for last group where there is <=8 motors
            //
	    if(motor_group==total_motor_groups-1){
                //
		//ignore motor number in the tag
		//
		in.ignore(7,' ');
                //
		//ignore white spaces
		//
		while(in.peek()==' ') in.ignore(1, ' ');
                //
		//check motors until last in the last group
		//
		while(counter<motors_in_last_group){
                    in.getline(temp1, 49, ' ');          
                    while(in.peek()!=' '){
                        strcat(temp1, " ");
                        in.getline(temp2, 49, ' ');
                        strcat(temp1, temp2);            
                    } 
                    if(!strcmp(label, temp1)){//check if motor match
                        break;
                    }
                    counter++;
                    while(in.peek()==' '){//get rod of white spaces separating motors
                        in.ignore(1,' ');
                    } 
                }
		//
		//case for last motor in last group
                //
		if(counter==motors_in_last_group){
                    in.getline(temp1, 150);//get motor name
                    if(!strcmp(label, temp1))break;//check if matches
                    counter=0;//any of motors mathc the one invoked
                }          
            }
            break;
        }
        else
	    //
	    //go to next line in case of wrong tag
            //
	    in.getline(str, BUFFER_SIZE-1);
    }
    //
    //rewind file when finished
    //
    in.seekg(0, ios::beg);
    return counter;
}



/**
* Function counts how many motor groups exist in the scan.
* Used in get_motor_position(...) function. 
* Parameters: input - input file stream, 
* 
* Returns: number of motor groups in the scan, zero if there is no motors.  
*/
int motor_counter(ifstream &in){
    char str[BUFFER_SIZE];
    char a,b;
    int counter=0; 
    bool flag=0;
    //
    //search until #L tag is found
    //
    while(!(a =='#' && b =='L')){
        //
	//fail bit prevention
	//
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(str, BUFFER_SIZE-1);
            break;
        }
	//
	//get first sign in the line
        //
	in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
	//
	//get the second one
        //
	in.get(b);
	//
	//check for motor tag
        //
	if(a=='#'&& b=='P'){
            counter++;
            flag=1;
        }
	//
	//if counted already break
        //
	else if(flag==1)break;
        //
	//go to next line in case of wrong tag
        //
	in.getline(str, BUFFER_SIZE-1);
    }
    //
    //rewind file when finished
    //
    in.seekg(0, ios::beg);
    return counter;
}



/**
* Function counts how many motors are in the given group.
* Used in get_motor_position(...) function for last 
* motor group, because in the rest there is always 8. 
* Parameters: input - input file stream, int motor group
* 
* Returns: number of motors in the group, zero if there is no motors.  
*/
int m_label_counter(ifstream &in, int noMotor){
    char str[BUFFER_SIZE];
    long double value;
    char a,b;
    int motor=0;
    int counter=0;
    //
    //search until #L is found
    //
    while(!(a =='#' && b =='L')){
        //
	//fail bit prevention
        //
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(str, BUFFER_SIZE-1);
            break;
        }
	//
	//get the first sign from the line
        //
	in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
	//
	//get the second one 
        //
	in.get(b);
	//
	//interesting case
        //
	if(a=='#'&& b=='P'){
            in.ignore(5,' ');
            if(motor==noMotor){//check if the interesting group is met
                while(in.peek()!='\n'){
                    if(in.peek()==' ')in.ignore(3,' ');
                    //
		    //count values of the motor in the motor group
		    //
		    else{
                        in >> value;
                        counter++;
                    }
                }
                break;
            }
	    //
	    //increment motor group
            //
	    else motor++;
        }
	//
	//next line, tag does not match
        //
	in.getline(str, BUFFER_SIZE-1);
    }
    //
    //rewind the file when finished
    //
    in.seekg(0, ios::beg);
    return counter;
}



/**
* Function checks if there are new motors in the scan.
* Used in get_motor_position(...) function. If there are any
* new motors in the scan, function get_motor_position(...) 
* search for motor at the beginning of the file. 
* Parameters: input - input file stream, 
* 
* Returns: true if new motor exist in the scan, false otherwise.  
*/
bool is_motor(ifstream &in){
    bool exist = 0;
    char str[BUFFER_SIZE];
    char a,b; 
    //
    //search until #P is met 
    //
    while(!(a =='#' && b =='P')){
        //
	//fail bit prevention
	//
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(str, BUFFER_SIZE-1);
            break;
        }
	//
	//get first sign
        //
	in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
	//
	//get the second one
        //
	in.get(b);
	//
	//motor tag
        //
	if(a=='#'&& b=='O'){
            exist=1;//motors met
            break;
        }
	//
	//go to next line 
        //
	else in.getline(str, BUFFER_SIZE);
    }
    //
    //rewind file
    //
    in.seekg(0, ios::beg);
    return exist;
}



/**
* Function reads value of given motor in the given scan. Values
* of motor position and motor groups are calculated 
* by get_motor_position(...) function.  
* search for motor at the beginning of the file. 
* Parameters: input - input file stream, int motor group,   
*             int motor position, int scan number 
* Returns: double value of the motor in the scan.  
*/
double get_motor_value(ifstream &in, int noMotor, int noValue, int scan){
    char str[BUFFER_SIZE];
    double value;
    //
    //go to scan
    //
    scan_rewinder(in, scan);
    //
    //count motors in the group
    //
    int MAX = m_label_counter(in, noMotor);
    if(noValue>MAX || noValue<1){
        cout<<"Uncorrect number of value\n";
        exit(1);
    }
    //
    //go back to the scan
    //
    scan_rewinder(in, scan);
    char a,b;
    int motor=0;
    //
    //stop working when #L is met
    //
    while(!(a =='#' && b =='L')){
        //
	//fail bit prevention
        //
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(str, BUFFER_SIZE-1);
            break;
        }
        in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
        in.get(b);
	//
	//motor values section
        //
	if(a=='#'&& b=='P'){
            in.ignore(5,' ');
            if(motor==noMotor){
                for(int i=0; i<noValue; i++){
                    in >> value;//read value until intersted motor
                }
                break;
            }
            else motor++;
        }
        in.getline(str, BUFFER_SIZE-1);
    }
    //
    //rewind file when finished
    //
    in.seekg(0, ios::beg);
    return value;
}



/**
* Function set the carriage at the beginning of the scan,
* from which we want to read data. If the scan number parameter
* is bigger then number of last scan in the file, error will notified. 
* Parameters: input - input file stream, int scan number
*            
* Returns: true if scan was found, false otherwise  
*/
bool scan_rewinder(ifstream &in, int scan_number){
    //
    //check if the scan number exist in the file
    //
    if(scan_number>total_scans(in)){
        cout<<" Scan number "<<scan_number<<" does not exist in the file. ";
        in.seekg(0);
        exit(1);
    }
    bool correct=0;
    int counter=0;
    char string[BUFFER_SIZE];
    char a,b;
    //
    //rewind file
    //
    in.seekg(0);
    //
    //finish with EOF
    //
    while(!(in.eof())){
        //
	//fail bit prevention
        //
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(string, BUFFER_SIZE-1);
        }
        in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
        in.get(b);
	//
	//check for new scan seciotn
        //
	if(a=='#'&& b=='S'){
            counter++;//count scans
            if(counter==scan_number){
            correct=1;//break if the interesting scan is reached
            break;
            }
        }
        in.getline(string, BUFFER_SIZE-1);
    }
    //
    //reset falgs in stream in case of EOF
    //
    if(in.eof()){
        in.clear(in.rdstate() & ~ios::eofbit & ~ios::failbit);
    }
    return correct;   
}



/**
* Function counts all scans in the file.
* Carriage is rewinded at the beginning of the file
* after scans were counted. 
* Parameters: input - input file stream, 
*            
* Returns: int number of scans.  
*/
int total_scans(ifstream &in){
    int counter=0;
    char string[BUFFER_SIZE];
    char a,b;
    //
    //rewind file
    //
    in.seekg(0);
    //
    //finish with the end of file
    //
    while(!(in.eof())){
        //
	//fail bit prevention
        //
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(string, BUFFER_SIZE-1);
        }
        in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
        in.get(b);
	//
	//new scan section
        //
	if(a=='#'&& b=='S'){
            counter++;//count scans
        }
        in.getline(string, 255);
    }
    //
    //reset flags in stream for EOF
    //
    if(in.eof()){
        in.clear(in.rdstate() & ~ios::eofbit & ~ios::failbit);
    }
    //
    //rewind file when finished
    //
    in.seekg(0);
    return counter;   
}



/**
* Function returns name of the label corresponding to its position.
* It searches in #L group until position is met. 
* Parameters: input - input file stream, int position, int number of scan 
*             output - char* label
* Returns: zero if there is no label for that postion, 1 otherwise  
*/
bool get_label(ifstream &in, int number, char *label, int scan_number){ 
    bool empty = 0;
    scan_rewinder(in, scan_number);
    char temp[50];
    char str[BUFFER_SIZE];
    char a,b;
    int counter=0;
    //
    //count labels
    //
    int MAX=count_columns(in);
    if(number==MAX) counter=1;
    if (number>MAX){
        cout<<" Label number "<<number<<" does not exist in the file. ";
	exit(1);
    }
    //
    //stop when #S is met
    //
    while(!(a =='#' && b =='S')){
        //
	//fail bit prevention
        //
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(str, BUFFER_SIZE-1);
        }
        in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
        in.get(b);
	//
	//variable section
        //
	if(a=='#'&& b=='L'){
            while(in.peek()==' ') in.ignore(3, ' ');
            //
	    //loop until the interested label
	    //
	    while(counter<number){ 
                in.getline(label, 49, ' ');          
                while(in.peek()!=' '){
                    strcat(label, " ");
                    in.getline(temp, 49, ' ');
                    strcat(label, temp);           
                }
                counter++;
                while(in.peek()==' '){
                    in.ignore(3,' ');
                } 
            }
	    //
	    //in case of last label 
            //
	    if(counter==(MAX))in.getline(label, 150);
                empty = 1;
                break;
        }
        else
	    //
	    //go to next line if not #L
  	    //
	    in.getline(str, BUFFER_SIZE-1);
    }
    //
    //rewind file when finished
    //
    in.seekg(0, ios::beg);
    return empty;
}

/**
* Function returns name of the motor corresponding to its position.
* It searches in #O group until position is met. 
* Parameters: input - input file stream, int position, int motor group 
*             output - char* motor_name
* Returns:   
*/
void get_motor_name(ifstream &in, char *motor_name, int motor_group, int motor_position){
    char str[BUFFER_SIZE];
    char temp2[50];
    char a,b;
    int total_motor_groups, motors_number;
    int counter=0;
    
    in.seekg(0, ios::beg);
    //
    //count motor groups
    //
    if(!(total_motor_groups = motor_counter(in))){  
        cout<<"NO motors in the file\n";
        exit(1);
    }
    if(motor_group>total_motor_groups-1){
        cout<<" Motor group "<<motor_group<<" is not in the file\n";
	exit(1);
    }
    //
    //count motors in last group, in the rest of the groups there is always 8 
    //
    if(!(motors_number = m_label_counter(in, motor_group ))) {
        cout<<"NO motors in last group\n";
        exit(1);
    }
    if(motor_position>motors_number || motor_position<1){
        cout<<" Uncorrect number of label\n";
        exit(1);
    }    
    //
    //set counter to 1 if last motor in the group
    //
    if(motor_position==motors_number) counter=1;
    //
    //stop searching for motors if #L is met
    //
    while(!(a =='#' && b =='L')){
        //
	//fail bit prevention
	//
	if(in.fail()){
            in.clear(in.rdstate() & ~ios::failbit);
            in.getline(str, BUFFER_SIZE-1);
            break;
        }
	//
	//read first sign in line
        //
	in.get(a);
        while(isspace(a)!=0){
            in.get(a);
        }
	//
	//get the second one
        //
	in.get(b);
        //
	//motor section
        //
	if(a=='#'&& b=='O'){    
	        //
		//go to invoked group
		//
		for(int i=0; i<motor_group; i++){
		    in.getline(str, BUFFER_SIZE-1);
		}
		//
		//ignore motor number (i.e. #O4)
		//
		in.ignore(7,' ');
		//
		//ignore white spaces until first motor
	        //
		while(in.peek()==' ') in.ignore(1, ' ');
		//
		//check until last motor
		//
		while(counter<motor_position){
		    //
		    //get motor name
                    //
		    in.getline(motor_name, 49, ' ');          
                    while(in.peek()!=' '){
                        strcat(motor_name, " ");//single one spaces can be in the motor name
                        in.getline(temp2, 49, ' ');
                        strcat(motor_name, temp2);            
                    } 
                        
                    counter++;//increment counter of labels in the group if not found
                    //     
	            //get rid of white spaces separating motors
		    //
		    while(in.peek()==' '){
                        in.ignore(1,' ');
                    } 
                }
	        //
		//case of last motor in group
	        //
		if(motor_position==motors_number)
                    //
		    //get last moto 
                    //
		    in.getline(motor_name, 150);
	       
	       break;    
        }
        else
	    //
	    //go to next line in case of wrong tag
            //
	    in.getline(str, BUFFER_SIZE-1);
    }
    //
    //rewind file when finished
    //
    in.seekg(0, ios::beg);
}





