//LICENSE

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <QDebug>
#include "../../include/io/io_csv.h"

namespace kipl {
namespace io {
    std::map<std::string, std::vector<float> > readCSV(const std::string &fname, char delim, bool haveHeader)
    {
       std::map<std::string, std::vector<float> > columns;

       // Reads a CSV file into a vector of <string, vector<int>> pairs where
       // each pair represents <column name, column values> 
       // Create an input filestream
       std::ifstream csvFile(fname.c_str());

       // Make sure the file is open
       if (!csvFile.is_open())
           throw std::runtime_error("Could not open file");

       // Helper vars
       std::string line, colname;
       float val;

       // Read the column names
       if (haveHeader && csvFile.good())
       {
           // Extract the first line in the file
           std::getline(csvFile, line);

           // Create a stringstream from line
           std::stringstream ss(line);

           // Extract each column name
           while(std::getline(ss, colname, ','))
           {
               // Initialize and add <colname, int vector> pairs to result
               columns.insert(make_pair(colname, std::vector<float> {} ));
           }
       }

       // Read data, line by line
       while(std::getline(csvFile, line))
       {
           // Create a stringstream of the current line
           std::stringstream ss(line);

           // Keep track of the current column index
           if (columns.empty())
           {
               char col = 'a';

               while(ss >> val)
               {
                   // Add the current integer to the 'colIdx' column's values vector
                   colname.clear();
                   colname.push_back(col);
                   columns.insert(make_pair(colname,std::vector<float>{val}));

                   // If the next token is a comma, ignore it and move on
                   if(ss.peek() == ',') ss.ignore();

                   // Increment the column index
                   ++col;
               }
           }
           else
           {
               auto it = columns.begin();

               // Extract each float
               while(ss >> val)
               {

                   // Add the current integer to the 'colIdx' column's values vector
                   it->second.push_back(val);

                   // If the next token is a comma, ignore it and move on
                   if(ss.peek() == ',') ss.ignore();

                   // Increment the column index
                   ++it;
               }
           }
       }

       // Close file
       csvFile.close();

       return columns;
    }
}
}
