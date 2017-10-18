#include <string>
#include <stdlib.h>
#include <vector>

using std::string;
using std::vector;

/**
 * \brief This function removes white spaces in the string location
 *
 * \param location (INPUT) is the string location as found in the 
 * translation file
 * \param new_location (INPUT) is the string location without white spaces 
 */
void without_white_spaces(const string & s, 
                          string & s_output);
                   

/**
 * \brief This function separates the definition part from the declaration
 * part in the string location
 *
 * \param new_location (INPUT) the string location formated (whithout white
 * spaces for example)
 * \param decla_def (OUTPUT) is the declaration and definition part isolated
 */
void DeclaDef_separator(string & s,
                        vector<string> & decla_def);

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
                           vector<string> & LocGlobArray); 

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
                      string & definition_version_with_groups); 


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
void ReplaceTagDef_by_Grp(string & s, 
                          int a, 
                          string & GrpVersion); 

/**
 * \brief This function check if there is a "|" present in each
 * group
 *
 * \param OpenBra (INPUT) is the position of the open bracket of the group
 * \param spacerPosition (INPUT) is the position of the "|"
 * \param closeBra (INPUT) is the position of the close bracket of the group
 */
void CheckSpacerValidity(int i, 
                         int j, 
                         int k);

/**
 * \brief This function checks if the tag_name is a valid tag
 *
 * \param Tag (INPUT) is the tag_name part of the tag
 */
void CheckTagValidity (string & Tag);

/**
 * \brief This function stores the operators which are between the tags
 *
 * \param StrS (INPUT) is the definition part of the string location
 * \param HowMany (INPUT) is the number of groups, or tags
 * \param Ope (INPUT) is a list of the operators 
 */
void store_operators(string & s, 
                    int & HowMany, 
                    vector<string> & Ope);

/**
 * \brief This function localize the position of each operator
 *
 * \param s (INPUT) is the definition part of the string location
 * \param TagName_Number (INPUT) is the number of tags, or groups.
 */
vector<string::iterator> PositionSeparator(string s, 
                                           int TagName_Number);

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
                           vector<int> & OpePriority);

