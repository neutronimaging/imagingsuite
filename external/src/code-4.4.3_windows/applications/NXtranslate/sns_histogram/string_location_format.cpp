#include "string_location_format.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

using std::string;
using std::vector;
using std::runtime_error;
using std::invalid_argument;

/**
 * \brief This function removes white spaces in the string location
 *
 * \param location (INPUT) is the string location as found in the 
 * translation file
 * \param new_location (INPUT) is the string location without white spaces 
 */
void without_white_spaces(const string & location,
                          string & new_location)
{
  typedef std::string::size_type string_size;
  string_size i=0;
  std::string ret="";
 
  while (i != location.size())
    {
      while (i != location.size() && isspace(location[i]))
	{
	  ++i;	
        }
      
      //find end of next word
      string_size j=i;
      while (j != location.size() && !isspace(location[j]))
        {
          ++j;
        }
      
      if (i != j)
        {
          ret+=location.substr(i,j-i);
          i=j;
        }
    } 
  new_location = ret;
  return;
}

/**
 * \brief This function separates the definition part from the declaration
 * part in the string location
 *
 * \param new_location (INPUT) the string location formated (whithout white
 * spaces for example)
 * \param decla_def (OUTPUT) is the declaration and definition part isolated
 */
void DeclaDef_separator(string & new_location,
                        vector<string> & decla_def)
{
  typedef std::string::size_type string_size;
  int Dposition = new_location.find("#");
  
  if (Dposition == -1)
    throw runtime_error("Declaration/Definition spacer invalid or not present");
  
  string_size taille = new_location.size();
  
  decla_def.push_back(new_location.substr(0,Dposition));
  
  if (taille == Dposition+1)
    {
        decla_def.push_back("");
    }
  else
    {
      decla_def.push_back(new_location.substr(Dposition+1, 
                                              taille - decla_def[0].size()-1));
    }
  return;
}

/*********************************
/Separate local from global array
/*********************************/
/**
 * \brief This function separates the two parts of the declaration part, the
 * local and global array declaration
 *
 * \param declaration_part (INPUT) is the declaration part of the string
 * location
 * \param LocGlobArray (OUTPUT) is the local and global parts of the 
 * declaration part
 */
void declaration_separator(string declaration_part, 
                           vector<string> & LocGlobArray)
{
  std::string str;
  typedef string::size_type string_size;
  string_size declaration_partSize = declaration_part.size();
  int SeparatorPosition = declaration_part.find("][");
  
  if (SeparatorPosition == -1)
    throw runtime_error("Format of declaration not valid");  

  LocGlobArray.push_back(declaration_part.substr(0,SeparatorPosition+1));
  LocGlobArray.push_back(declaration_part.substr(SeparatorPosition+1,
                         declaration_partSize-LocGlobArray[0].size()));
  
  return;
}

/**
 * \brief This function separates the tag parts from the definition part
 * of the string location. A tag is a set of tag_name and a operator.
 *
 * \param DefinitionPart (INPUT) is the definition part of the string location
 * \param Tag (OUTPUT) is the tag_name part of the tag
 * \param Def (OUTPUT) is the operator part of the tag
 * \DefinitionGrpVersion (OUTPUT) is a string where all the tags are replaces
 * by the word "grp" + their index. This will be used later to determine
 * the priorities of the tags
 */
void TagDef_separator(string & DefinitionPart, 
                      vector<string> & Tag, 
                      vector<string> & Def, 
                      string & definition_version_with_groups)
{
  std::vector<string> ret;
  typedef string::iterator iter;
  iter b = DefinitionPart.begin();
  iter e =  DefinitionPart.end();
  iter a;
  int CPosition;
  int OpenBraPosition;
  int CloseBraPosition = 1;
  string  separator = "|";
  string StringLocationGroup = DefinitionPart;
  static const string OpenBracket = "{";
  static const string CloseBracket = "}";
  int HowManyTimes = 0;
  int i = 0;
  int length = DefinitionPart.size();

  while (b!=e)
    { 
       if (find(b, DefinitionPart.end(), separator[0]) 
           !=  DefinitionPart.end())
      	{
	   ++HowManyTimes;
	   b = find(b, DefinitionPart.end(),separator[0]);
	   b+=1;
	}
        b+=1;
    }

  while (i < HowManyTimes)
    {
      CPosition =  DefinitionPart.find(separator[0]);
      OpenBraPosition =  DefinitionPart.find(OpenBracket);
      CloseBraPosition =  DefinitionPart.find(CloseBracket);

      CheckSpacerValidity(OpenBraPosition,CPosition,CloseBraPosition);

      Tag.push_back( DefinitionPart.substr(OpenBraPosition+1,
                                           CPosition-OpenBraPosition-1));
   
      CheckTagValidity(Tag[i]);
      Def.push_back( DefinitionPart.substr(CPosition+1,
                                           CloseBraPosition-CPosition-1));

       DefinitionPart = DefinitionPart.substr(CloseBraPosition+1, 
                                              length-CloseBraPosition-1);
       ++i;
    };

   ReplaceTagDef_by_Grp(StringLocationGroup,
                        HowManyTimes,
                        definition_version_with_groups);
   return;
}

/**
 * \brief This function replaces the set of tag_name and operator, also
 * called the tag_definition, by "grp"
 *
 * \param StringLocationGroup (INPUT) is the definition part of the string
 * location
 * \param HowManyTimes (INPUT) is the number of tag_name/tag_operator in the
 * definition part
 * \param DefinitionGrpVersion (OUTPUT) is the definition part of the string
 * location where the set of tag_names and tag_definition have been replaced 
 * by "grp"
 */
void ReplaceTagDef_by_Grp(string & StringLocationGroup, 
                          int HowManyTimes, 
                          string & definition_version_with_groups)
{
  static const string  separator = "|";
  static const string OpenBracket = "{";
  static const string CloseBracket = "}";
  string part1, part2;
  int OpenBraPosition, CloseBraPosition;
 
  for (int j=0 ; j<HowManyTimes ; ++j)
  
    {
      std::ostringstream Grp;
      OpenBraPosition =  StringLocationGroup.find(OpenBracket);
      CloseBraPosition =  StringLocationGroup.find(CloseBracket);
      
      StringLocationGroup.erase(OpenBraPosition, 
                                CloseBraPosition + 1 - OpenBraPosition);
     
      part1 = StringLocationGroup.substr(0,OpenBraPosition);
      part2 = StringLocationGroup.substr(OpenBraPosition, 
                                         StringLocationGroup.size());
      
      Grp << "grp" << j ;  
      StringLocationGroup = part1 + Grp.str() + part2;
    }

  definition_version_with_groups = StringLocationGroup;

  return;
}

/**
 * \brief This function check if there is a "|" present in each
 * group
 *
 * \param OpenBra (INPUT) is the position of the open bracket of the group
 * \param spacerPosition (INPUT) is the position of the "|"
 * \param closeBra (INPUT) is the position of the close bracket of the group
 */
void CheckSpacerValidity(int openBra, int spacerPosition, int closeBra)
{
  if (spacerPosition < openBra || spacerPosition > closeBra)
    {
      throw runtime_error("Missing \"|\" spacer or wrong definition declaration");
    }
  return;
}

/**
 * \brief This function checks if the tag_name is a valid tag
 *
 * \param Tag (INPUT) is the tag_name part of the tag
 */
void CheckTagValidity (string & Tag)
{
 if (Tag == "pixelID" ||
     Tag == "pixelX" || 
     Tag == "pixelY" || 
     Tag == "Tbin")
   {
     return;
   }
  else
    {
      throw runtime_error("One of the Tag is not a valid Tag");
    }
return;
}

/**
 * \brief This function stores the operators which are between the tags
 *
 * \param StrS (INPUT) is the definition part of the string location
 * \param HowMany (INPUT) is the number of groups, or tags
 * \param Ope (INPUT) is a list of the operators 
 */
void store_operators(string & StrS, 
                    int & HowMany, 
                    vector<string> & Ope)
{
  typedef string::iterator iter;
  std::vector<iter> VecIter;             //vector of iterators
  std::string::iterator a;
  string operatorOR = "OR";
  string operatorAND = "AND";

 //Store each position of "|" into VecIter
 VecIter = PositionSeparator(StrS,HowMany);
  
  for (int i=0 ; i<HowMany-1; ++i)
    {
      if (find(VecIter[i],VecIter[i+1],operatorOR[0])!=VecIter[i+1])
      { 
	Ope.push_back("OR");
      }
      else if (find(VecIter[i],VecIter[i+1],operatorAND[0])!=VecIter[i+1])
	{
	  Ope.push_back("AND");
	}
      else
        {
          throw runtime_error("Not a valid operator");
        }
    }	  
  return;
}

/**
 * \brief This function localize the position of each operator
 *
 * \param s (INPUT) is the definition part of the string location
 * \param TagName_Number (INPUT) is the number of tags, or groups.
 */
vector<string::iterator> PositionSeparator(string s, 
                                           int TagName_Number)
{ 
  std::vector<string::iterator> VecIter;
  int i = 0;
  typedef string::iterator iter;
  iter b = s.begin();  
  string separator = "|";

  while (i < TagName_Number)
    {
      VecIter.push_back(find(b, s.end(), separator[0]));
      b = find(b,s.end(),separator[0])+1;
      ++i;
    }
  return VecIter;
}

/*********************************
/Give priority for each group
/*********************************/
/**
 * \brief This function gives for each group a priority value
 *
 * \param s (INPUT) is the definition part of the string location
 * \param OperatorNumber (INPUT) is the number of operator
 * \param GrpPriority (OUTPUT) is a list of groups priorities
 * \param InverseDef (OUTPUT) equals to 1 or 0 for each groups; 1 means we want
 * to reverse the tag_operator meaning.
 * \param OpePriority (OUTPUT) is the list of the groups priorities
 */
void assign_grps_priority( string & s, 
                           int OperatorNumber, 
                           vector<int> & GrpPriority, 
                           vector<int> & InverseDef, 
                           vector<int> & OpePriority)
{
  int DefinitionString_size = s.size();
  int GrpNumberLive = 0;
  int Priority = 0;
    
  //Initialization of vector<int> and vector<bool>
  for (int i=0; i<OperatorNumber; i++)
    {
      GrpPriority.push_back(0);
      InverseDef.push_back(0);
      OpePriority.push_back(0);
    }
  
  //move along the definition part and look for g,A,O,!,(,and ).
  for (int j=0 ; j<DefinitionString_size; ++j)
    {
      switch (s[j])
	{
	case 'g':
          j=j+3;   //move to end of grp#
	  GrpPriority[GrpNumberLive]=Priority;
   	  ++GrpNumberLive;
     	  break;
	case '!':
	  InverseDef[GrpNumberLive]=1;
          break;
	case '(':
	  ++Priority;
          break;
	case ')':
	  --Priority;
          break;
	case 'A':
	  OpePriority[GrpNumberLive-1]=Priority;
	  break;
	case 'O':
	  OpePriority[GrpNumberLive-1]=Priority;
	  break;
	default:
          // do nothing
	  break;
	}
    }
 
  if (Priority != 0)
    {
      throw runtime_error("Format of parentheses not valid");
    }

  return;
}

