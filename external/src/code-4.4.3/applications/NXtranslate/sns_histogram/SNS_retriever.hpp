#ifndef __SNS_HISTOGRAM_RETRIEVER_GUARD
#define __SNS_HISTOGRAM_RETRIEVER_GUARD

#include "../node.h"
#include "../node_util.h"
#include "../string_util.h"
#include "string_location_format.h"
#include "../tree.hh"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include "retriever.h"
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>

//Type of binary file
typedef int binary_type;

// this is not intended to be inherited from
class SnsHistogramRetriever: public Retriever{
 public:
  SnsHistogramRetriever(const std::string &);
  ~SnsHistogramRetriever();
  void getData(const std::string &, tree<Node> &);
  std::string toString() const;
  static const std::string MIME_TYPE;
 private:
  SnsHistogramRetriever(const SnsHistogramRetriever&); //copy constructor
  SnsHistogramRetriever& operator=(const SnsHistogramRetriever&); //operator "=" overloading
  std::string source;
  std::string location;
  FILE *BinaryFile;
};

struct Grp_parameters   //parameters of the different definitions
{
  int init, last, increment;  //with loop(init,end,increment)
  vector<int> value;          //(value[0],value[1],....)
  char c;                     //c=l for loop and c=p for (x,y,z,...)
};

/**
 * \brief This function isolate the different parameters of the definition
 * part of the location string
 *
 * \param def (INPUT) is the full definition part of the string location
 * \param OperatorNumber (INPUT) is the number of definition part the
 * string location contains
 * \para GrpPara (OUTPUT) is the list of operators (loop or list of 
 * identifiers
 * \param record (OUTPUT) is a structure Grp_parameters that contains
 * all the information of the different defintion part
 */
void store_para_of_definition(vector<string> Def,
                              int OperatorNumber, 
                              vector<Grp_parameters> & GrpPara, 
                              Grp_parameters & record);

/**
 * brief This function parse the definition and isolate the three parts 
 * of the loop definition; the initial value, the final value and the increment
 *
 * \param def (INPUT) the definition part to parse
 * \param i (INPUT) the index of the defintion part to parse
 * \param GrpPara (OUTPUT) the Grp_parameters structre of the defintion part
 */
void InitLastIncre (string & def, 
                    int i, 
                    vector<Grp_parameters> & GrpPara);   

/**
 * \brief This function parses the value of the list of identifiers
 *
 * \param def (INPUT) is the definition part to parse
 * \param GrpPara (INPUT) is the Grp_parameters structure of the list of 
 * identifiers
 */
void ParseGrp_Value (string & def, 
                     int i, 
                     vector<Grp_parameters> & GrpPara);  
/**
 * \brief This function parse the local and global array of the 
 * declaration part
 *
 * \param LocGlobArray (INPUT) is the full string of the declaration part
 * \param LocalArray (OUTPUT) is the list of parameters of the local part
 * \param GlobalArray (OUTPUT) is the list of parameters of the global part
 */
void parse_declaration_array(vector<string> & LocGlobArray, 
                           vector<int> & LocalArray,
                           vector<int> & GlobalArray);  

/**
 * \brief This function determines the highest priority of all the groups.
 * A group is a set of operation, can be a loop or a list of identifiers
 *
 * \param GrpPriority (INPUT) is the list of the priority
 *
 * \return the last maximum value of the list
 */
int FindMaxPriority (vector<int>& GrpPriority);  


/**
 * \brief This function calculates the final array according to the 
 * string location
 *
 * \param GrpPriority (INPUT) is a list of the groups priorities
 * \param InverseDef (INPUT) inverses or not the meaning of the definition
 * part having the same index
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param Ope (INPUT) is the list of the operators
 * \param OpePriority (INPUT) is a list of the operator priorities
 * \param tr (INPUT) is the final location of the array in the NeXus file
 * \param Tag (INPUT) is the list of the tag_names
 * \param Def (INPUT) is the list of the tag_definitions
 * \param LocalArray (INPUT) is the list of parameters of the local declaration
 * part
 * \param GlobalArray (INPUT) is the list of parameters of the global 
 * declaration part
 * \param GrpPara (INPUT) is a list of structures of all the parameters of the
 * defintion part
 */
void calculate_array (vector<int> & GrpPriority, 
                      vector<int> & InverseDef, 
                      binary_type * BinaryArray, 
                      vector<string> Ope, 
                      vector<int> & OpePriority,
                      tree<Node> & tr, 
                      vector<string> & Tag, 
                      vector<string> & Def, 
                      vector<int> & LocalArray, 
                      vector<int> & GlobalArray, 
                      vector<Grp_parameters> & GrpPara);  
/**
 * \brief This function calculates the array if the definition tag is PixelID
 *
 * \param MyGrpArray (INPUT) 
 * \param BinaryArray (INPUT) is the array that comes from the binary file
 * \param grp_number (INPUT) is the group index
 * \param InverseDef (INPUT) allows to check if we want or not the inverse
 * of the defintion 
 * \param def (INPUT) is the list of definition
 * \param LocalArray (INPUT) ???not used???
 * \param GlobalArray (INPUT) 
 * \param GrpPara (INPUT) is the list of parameters of the group
 * 
*/
void MakeArray_pixelID (binary_type* MyGrpArray, 
                        binary_type* BinaryArray,
                        int grp_number,
                        int InverseDef, 
                        vector<string> & Def, 
                        vector<int> & LocalArray, 
                        vector<int> & GlobalArray, 
                        vector<Grp_parameters> & GrpPara);  
/**
 * \brief This function calculates the array if the definition tag is PixelX
 *
 * \param MyGrpArray (INPUT) 
 * \param BinaryArray (INPUT) is the array that comes from the binary file
 * \param grp_number (INPUT) is the group index
 * \param InverseDef (INPUT) allows to check if we want or not the inverse
 * of the defintion 
 * \param def (INPUT) is the list of definition
 * \param LocalArray (INPUT) ???not used???
 * \param GlobalArray (INPUT) 
 * \param GrpPara (INPUT) is the list of parameters of the group
 * 
*/
void MakeArray_pixelX (binary_type* MyGrpArray, 
                       binary_type* BinaryArray,
                       int grp_number,
                       int InverseDef, 
                       vector<string> & Def, 
                       vector<int> & LocalArray, 
                       vector<int> & GlobalArray, 
                       vector<Grp_parameters> & GrpPara);   
/**
 * \brief This function calculates the array if the definition tag is PixelY
 *
 * \param MyGrpArray (INPUT) 
 * \param BinaryArray (INPUT) is the array that comes from the binary file
 * \param grp_number (INPUT) is the group index
 * \param InverseDef (INPUT) allows to check if we want or not the inverse
 * of the defintion 
 * \param def (INPUT) is the list of definition
 * \param LocalArray (INPUT) ???not used???
 * \param GlobalArray (INPUT) 
 * \param GrpPara (INPUT) is the list of parameters of the group
 * 
*/
void MakeArray_pixelY (binary_type* MyGrpArray, 
                       binary_type* BinaryArray,
                       int grp_number,
                       int InverseDef, 
                       vector<string> & Def, 
                       vector<int> & LocalArray, 
                       vector<int> & GlobalArray, 
                       vector<Grp_parameters> & GrpPara);   
/**
 * \brief This function calculates the array if the definition tag is Tbin
 *
 * \param MyGrpArray (INPUT) 
 * \param BinaryArray (INPUT) is the array that comes from the binary file
 * \param grp_number (INPUT) is the group index
 * \param InverseDef (INPUT) allows to check if we want or not the inverse
 * of the defintion 
 * \param def (INPUT) is the list of definition
 * \param LocalArray (INPUT) ???not used???
 * \param GlobalArray (INPUT) 
 * \param GrpPara (INPUT) is the list of parameters of the group
 * 
*/
void MakeArray_Tbin (binary_type* MyGrpArray, 
                     binary_type* BinaryArray,
                     int grp_number,
                     int InverseDef, 
                     vector<string> & Def, 
                     vector<int> & LocalArray, 
                     vector<int> & GlobalArray, 
                     vector<Grp_parameters> & GrpPara);     

/**
 * \brief This function makes a copy of the binary array
 *
 * \param MyGrpArray (OUTPUT) is the local copy of the binary array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param LocalArray (INPUT) ??? not used ???
 * \param GlobalArray (INPUT) is the list of parameters of the global
 * declaration part
 */
void MakeArray_Everything (binary_type* MyGrpArray, 
                           binary_type* BinaryArray, 
                           vector<int> & LocalArray, 
                           vector<int> & GlobalArray); 

/**
 * \brief This function does the actual calculation between the two
 * arrays that are pass in
 *
 * \param GrpArray1 (INPUT/OUTPUT) is the first array to operate on
 * \param GrpArray2 (INPUT) is the second array to operate on
 * \param Operator (INPUT) is the operator (or,and)
 * \param LocalArray (INPUT) ??? not used ???
 * \param GlobalArray (INPUT) is the list of parameters of the 
 * global declarataion part
 * \param GrpPara (INPUT) ??? not used ???
*/
void DoCalculation (binary_type* GrpArray1, 
                    binary_type* GrpArray2,
                    string Operator, 
                    vector<int> & LocalArray, 
                    vector<int> & GlobalArray, 
                    vector<Grp_parameters> & GrpPara);  

/**
 * \brief This function swap endians
 *
 * \param x (INPUT/OUTPUT) number to swap
 */
inline void endian_swap(binary_type & x);  

/**
 * \brief This function initialize the array to 0
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the array to initialized
 * \param GlobalArray (INPUT) is the dimension of the array
 */
void InitializeArray(binary_type * MyGrpArray, 
                     vector<int> & GlobalArray);    

/**
 * \brief This function calculates the array for the <b>!pixelID</b> case
 * for the <b>loop</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
*/
void InversePixelIDLoop (binary_type* MyGrpArray, 
                         binary_type * BinaryArray, 
                         vector<int> & GlobalArray, 
                         vector<Grp_parameters> & GrpPara, 
                         int grp_number);  

/**
 * \brief This function calculates the array for the <b>pixelID</b> case
 * for the <b>loop</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void PixelIDLoop (binary_type * MyGrpArray, 
                  binary_type * BinaryArray, 
                  vector<int> & GlobalArray, 
                  vector<Grp_parameters> & GrpPara, 
                  int grp_number);

/**
 * \brief This function calculates the array for the <b>!pixelID</b> case
 * for the <b>list of identifiers</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void InversePixelIDList (binary_type * MyGrpArray, 
                         binary_type * BinaryArray, 
                         vector<int> & GlobalArray, 
                         vector<Grp_parameters> & GrpPara, 
                         int grp_number);

/**
 * \brief This function calculates the array for the <b>pixelID</b> case
 * for the <b>list of identifiers</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void PixelIDList (binary_type * MyGrpArray, 
                  binary_type * BinaryArray, 
                  vector<int> & GlobalArray, 
                  vector<Grp_parameters> & GrpPara, 
                  int grp_number);

/**
 * \brief This function calculates the array for the <b>!pixelX</b> case
 * for the <b>loop</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void InversePixelXLoop (binary_type * MyGrpArray, 
                        binary_type * BinaryArray, 
                        vector<int> & GlobalArray, 
                        vector<Grp_parameters> & GrpPara, 
                        int grp_number);

/**
 * \brief This function calculates the array for the <b>!pixelX</b> case
 * for the <b>loop</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void PixelXLoop (binary_type * MyGrpArray, 
                 binary_type * BinaryArray, 
                 vector<int> & GlobalArray, 
                 vector<Grp_parameters> & GrpPara, 
                 int grp_number);

/**
 * \brief This function calculates the array for the <b>!pixelX</b> case
 * for the <b>list of identifiers</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void InversePixelXList (binary_type * MyGrpArray, 
                        binary_type * BinaryArray, 
                        vector<int> & GlobalArray, 
                        vector<Grp_parameters> & GrpPara, 
                        int grp_number);

/**
 * \brief This function calculates the array for the <b>pixelX</b> case
 * for the <b>list of identifiers</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void PixelXList (binary_type * MyGrpArray, 
                 binary_type * BinaryArray, 
                 vector<int> & GlobalArray, 
                 vector<Grp_parameters> & GrpPara, 
                 int grp_number);

/**
 * \brief This function calculates the array for the <b>!pixelY</b> case
 * for the <b>loop</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void InversePixelYLoop (binary_type * MyGrpArray, 
                        binary_type * BinaryArray, 
                        vector<int> & GlobalArray, 
                        vector<Grp_parameters> & GrpPara, 
                        int grp_number); 

/**
 * \brief This function calculates the array for the <b>pixelY</b> case
 * for the <b>loop</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void PixelYLoop  (binary_type * MyGrpArray, 
                  binary_type * BinaryArray, 
                  vector<int> & GlobalArray, 
                  vector<Grp_parameters> & GrpPara, 
                  int grp_number); 

/**
 * \brief This function calculates the array for the <b>!pixelY</b> case
 * for the <b>list of identifiers</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void InversePixelYList (binary_type * MyGrpArray, 
                        binary_type * BinaryArray, 
                        vector<int> & GlobalArray, 
                        vector<Grp_parameters> & GrpPara, 
                        int grp_number); 

/**
 * \brief This function calculates the array for the <b>pixelY</b> case
 * for the <b>list of identifiers</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void PixelYList (binary_type * MyGrpArray, 
                 binary_type * BinaryArray, 
                 vector<int> & GlobalArray, 
                 vector<Grp_parameters> & GrpPara, 
                 int grp_number); 

/**
 * \brief This function calculates the array for the <b>!Tbin</b> case
 * for the <b>loop</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void InverseTbinLoop (binary_type * MyGrpArray, 
                      binary_type * BinaryArray, 
                      vector<int> & GlobalArray, 
                      vector<Grp_parameters> & GrpPara, 
                      int grp_number); 

/**
 * \brief This function calculates the array for the <b>Tbin</b> case
 * for the <b>loop</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void TbinLoop (binary_type * MyGrpArray, 
               binary_type * BinaryArray, 
               vector<int> & GlobalArray, 
               vector<Grp_parameters> & GrpPara, 
               int grp_number);

/**
 * \brief This function calculates the array for the <b>!Tbin</b> case
 * for the <b>list of identifiers</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void InverseTbinList  (binary_type * MyGrpArray, 
                       binary_type * BinaryArray, 
                       vector<int> & GlobalArray, 
                       vector<Grp_parameters> & GrpPara, 
                       int grp_number); 

/**
 * \brief This function calculates the array for the <b>Tbin</b> case
 * for the <b>list of identifiers</b> case
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the resulting array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param GlobalArray (INPUT) is the the size of the array to return
 * \param GrpPara (INPUT) is the list of the parameters of the loop operator
 * \param grp_number (INPUT) is the index of the group 
 */
void TbinList (binary_type * MyGrpArray, 
               binary_type * BinaryArray, 
               vector<int> & GlobalArray, 
               vector<Grp_parameters> & GrpPara, 
               int grp_number); 

/**
 * \brief This function check for the highest priority in all the groups
 *
 * \param GrpPriority_size (INPUT) is the number of groups
 * \param CurrentPriority (INPUT/OUTPUT) is the current highest priority
 * \param GrpPriority (INPUT) is the list of the groups priority
 */
void CheckHighestPriority (int & GrpPriority_size, 
                           int & CurrentPriority, 
                           vector<int> & GrpPriority);

/**
 * \brief This function shift to the left all the priority of the groups
 * behind index i
 *
 * \param GrpPriority (INPUT/OUTPUT) is the list of groups priority
 * \param GrpPriority_size (INPUT) is the size of the groups priority
 * \param i (INPUT) is the index of the group priority of interest
 */
void Shift_GrpPriorities_Left (vector<int> & GrpPriority, 
                               int & GrpPriority_size, 
                               int & i);

/**
 * \brief This function shift to the left all the operators behind index i
 *
 * \param OpePriority (INPUT/OUTPUT) is the list of operator priority
 * \param Ope (INPUT/OUTPUT) is the list of operators 
 * \param GrpPriority_size (INPUT) is the number of groups 
 * \param i (INPUT) is the index of the operator priority of interest
 */
void Shift_Ope_OpePriority_Left (vector<int> & OpePriority, 
                                 vector<string> & Ope, 
                                 int & GrpPriority_size, 
                                 int & i);

/**
 * \brief This function shift to the left all the array corresponding to 
 * each groups
 *
 * \param MyGrpArray (INPUT/OUTPUT) are the array of each group
 * \param GrpPriority_size (INPUT) is the number of groups
 * \param i (INPUT) is the index of the array of interest
 */
void Shift_MyGrpArray_Left (binary_type ** MyGrpArray, 
                            int & GrpPriority_size, 
                            int & i);

/**
 * \brief This function calculated the final array according to the priorities
 * of all the groups and of all the operators
 *
 * \param GrpPriority (INPUT) is the list of groups priority
 * \param HighestPriority (INPUT) is the highest priority
 * \param OpePriority (INPUT) is the list of the operators priority
 * \param MyGrpArray (INPUT) are the array of each groups
 * \param GlobalArray (OUTPUT) is the final array
 * \param LocalArray (INPUT) 
 * \param Ope (INPUT) is the list of operators
 * \param GrpPara (INPUT) is the list of the groups parameters
 */
void MakePriorities (vector<int> & GrpPriority, 
                     int & HighestPriority, 
                     vector<int> & OpePriority, 
                     binary_type ** MyGrpArray, 
                     vector<int> & GlobalArray, 
                     vector<int> & LocalArray, 
                     vector<string> & Ope, 
                     vector<Grp_parameters> & GrpPara);

/**
 * \brief This function checks if we want everything
 *
 * \param DeclaDef (INPUT) is the declaration part of the string location
 * \param Everything (INPUT,OUTPUT) is 1 if we want everything
 * \param Tag (INPUT) is the list of tags
 * \param Ope (INPUT) is the list of Operators
 */
void check_want_everything (vector<string> & DeclaDef, 
                            int & Everything, 
                            vector<string> & Tag, 
                            vector<string> & Ope); 

/**
 * \brief This function swap endians of all the numbers of the BinaryArray
 *
 * \param GlobalArray (INPUT) is the size of the binary array
 * \param BinaryArray (INPUT/OUTPUT) is the array coming from the binary file
 */
void swap_endian (vector<int> & GlobalArray, 
                  binary_type * BinaryArray);
#endif
