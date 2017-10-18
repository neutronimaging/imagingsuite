//+**********************************************************************
//
// File:	SPEClib.h
//
// Project:	 Fable data to NeXus translation
//
// Description:	definitions of a SPEClib functions. It is a library,
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

#ifndef SPECLIB_H_
#define SPECLIB_H_

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>

#define BUFFER_SIZE  256
//#define DELIMITER ':'
using namespace std;

bool get_file_name(ifstream &in, char *result);
bool get_date(ifstream &in, char *result);
bool get_userID(ifstream &in, char *result);
bool refill_time(ifstream &in, char *result);
bool refill_mode(ifstream &in, char *result);
bool get_label(ifstream &in, int number, char *label, int scan_number);
int count_columns(ifstream &in);
//long double get_value(ifstream &in, int number, int scanNumber);
bool get_values(ifstream &in, int number, double results[], int scan_number);
int get_motor_position(ifstream &in, char *label, int &motor_group, int scan);
int motor_counter(ifstream &in);
double get_motor_value(ifstream &in, int noMotor, int noValue, int scan);
//bool get_scan_comment(ifstream &in, char *result);
//double time_used(ifstream &in);
//int get_scan_number(ifstream &in);
int get_label_position(ifstream &in, char *label, int scan);
int count_data_sets(ifstream &in);
int m_label_counter(ifstream &in, int noMotor);
//int count_scans(ifstream &in);*/
int total_scans(ifstream &in);
bool scan_rewinder(ifstream &in, int scan_number);
bool is_motor(ifstream &in);
void get_motor_name(ifstream &in, char *motor_name, int motor_group, int motor_position);
//bool command_parser(const string &location, char &letter, char *label, int &scan);

#endif /*SPECLIB_H_*/
