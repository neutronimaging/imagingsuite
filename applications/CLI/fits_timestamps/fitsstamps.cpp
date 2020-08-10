#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
	string line;
	string ender;
	string mask = "DATE    = '";
	int nchar= mask.size();
	char c;
	for (int i=1; i<argc; i++) {
		line.clear();
		ender.clear();
		ifstream f(argv[i]);
		
		do {
		//for (int j=0; j<100; j++) {
			c=f.get();
			line.push_back(c);
			if (nchar<line.size()) {
				ender=line.substr(line.size()-nchar,nchar);
			}
		} while ((ender != mask) && !f.eof());
		
		string date;
		
		for (int j=0; j<19; j++)
			date.push_back(f.get());
			
		cout<<argv[i]<<"\t"<<date.substr(0,9)<<"\t"<<date.substr(11)<<endl;	
		
	}

}