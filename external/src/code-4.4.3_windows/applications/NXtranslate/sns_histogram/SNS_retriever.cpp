#include "retriever.hpp"

#define SWAP_ENDIAN  //triger the swapping endian subroutine (if needed)

using std::ifstream;
using std::invalid_argument;
using std::runtime_error;
using std::string;
using std::cout;
using std::endl;
using std::vector;
   
/*********************************
/SnsHistogramRetriever constructor
/*********************************/
SnsHistogramRetriever::SnsHistogramRetriever(const string &str): source(str) 
{
  // open the file
  BinaryFile=fopen(source.c_str(),"rb");
  
  // check that open was successful
  if (BinaryFile==NULL)
    throw invalid_argument("Could not open file: "+source);
}

/*********************************
/SnsHistogramRetriever destructor
/*********************************/
SnsHistogramRetriever::~SnsHistogramRetriever()
{
  // close the file
  if(BinaryFile)
    fclose(BinaryFile);
}

/**
 * \brief This function creates the array according to the 
 * string location
 *
 * \param location (INPUT) is the string location coming from the 
 * translation file
 * \param tr (INPUT) is where to put the final array created
 */
void SnsHistogramRetriever::getData(const string &location, tree<Node> &tr)
{
  string new_location;
  string definition_version_with_groups = "";  //use to determine priorities

  vector<string> decla_def;           //declaration and definition parts
  vector<string> LocGlobArray; //local and global array (declaration part)
  vector<string> Ope;                //Operators of the defintion part
  int OperatorNumber; 
  string DefinitionPart;             //use to determine the operators
  vector<int> GrpPriority;           //Vector of priority of each group
  vector<int> OpePriority;           //Vector of priority for each operator
  vector<int> InverseDef; //True=Inverse definition, False=keep it like it is
  int Everything = 0;                //0= we don't want everything, 1=we do
  int GlobalArraySize = 1;           //Size of global array within our program
  vector<string> Tag, Def;
  vector<int> LocalArray, GlobalArray;

  vector<Grp_parameters> GrpPara;
  Grp_parameters record;

  // check that the argument is not an empty string
  if(location.size()<=0)
    throw invalid_argument("cannot parse empty string");
  
  //format the string location (remove white spaces)
  without_white_spaces(location, new_location);  
  
  DeclaDef_separator(new_location, decla_def);  //Separate Declaration part from Definition part -> decla_def
  
  //check if the defintion part has a valid format
  if (decla_def[1].size() > 0 && decla_def[1].size()<7)
    throw runtime_error("Definition part is not valid");
  
  //check if we want everything
  check_want_everything (decla_def, Everything, Tag, Ope);
  
  //Separate declaration arrays (local and global)
  declaration_separator(decla_def[0], LocGlobArray);
  
  if (Everything == 0)
    {
      //Work on definition part
      DefinitionPart = decla_def[1];
      
      //Parse defintion part, separate Tag from Def
      TagDef_separator(decla_def[1], Tag, Def, definition_version_with_groups);
      
      //check if we have at least one tag
      if (Tag.size()<1)
	throw runtime_error("Definition part is not valid");
      
      //Store operators
      OperatorNumber = Tag.size();
      store_operators(DefinitionPart,
                      OperatorNumber, 
                      Ope);
      
      //Give to each grp its priority
      assign_grps_priority(definition_version_with_groups, 
                           OperatorNumber, 
                           GrpPriority, 
                           InverseDef,
                           OpePriority);   
      
      //Store parameters of the definition part into GrpPara[0], GrpPara[1]...
      
      store_para_of_definition(Def,
                               OperatorNumber,
                               GrpPara,
                               record);
    }
  
  //parse Local and Global Array from Declaration part
  parse_declaration_array(LocGlobArray, 
                          LocalArray, 
                          GlobalArray);
  
  //allocate memory for the binary Array
  for (int i=0; i<GlobalArray.size(); i++)
    {
      GlobalArraySize *= GlobalArray[i];
    }
  
  binary_type * BinaryArray = new binary_type [GlobalArraySize];
  
  //transfer the data from the binary file into the GlobalArray
  fread(&BinaryArray[0],sizeof(BinaryArray[0]), GlobalArraySize, BinaryFile);
  
  //swap endian if necessary
#ifdef SWAP_ENDIAN

  swap_endian (GlobalArray, BinaryArray);
  
#endif  //SWAP_ENDIAN
  
  //Calculate arrays according to definition
  calculate_array(GrpPriority,
                  InverseDef, 
                  BinaryArray, 
                  Ope, 
                  OpePriority, 
                  tr,
                  Tag,
                  Def,
                  LocalArray,
                  GlobalArray,
                  GrpPara);
}

/*********************************
/SnsHistogramRetriever::MIME_TYPE 
/*********************************/
const string SnsHistogramRetriever::MIME_TYPE("application/x-SNS-histogram");

string SnsHistogramRetriever::toString() const
{
  return "["+MIME_TYPE+"] "+source;
}

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
                              int HowManyDef, 
                              vector<Grp_parameters> & GrpPara, 
                              Grp_parameters & record)
{
  //find out first if it's a loop or a list of identifiers
  for (int i=0; i<HowManyDef ; ++i)
    {
      if (Def[i].find("loop") < Def[i].size()) 
	{
	  record.c = 'l';
        }
      else
	{
	  record.c = 'p';
        }
      GrpPara.push_back(record);
    }
  
  //isolate the variable
  for (int i=0; i<HowManyDef ; ++i)
    {
      if (GrpPara[i].c == 'l')      //loop
	{
	  InitLastIncre(Def[i],i, GrpPara);
	}
      else                          //(....)
	{
	  ParseGrp_Value(Def[i],i, GrpPara);
	}
    }
 return;
}

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
                    vector<Grp_parameters> & GrpPara)
{
  static const string sep=",";
  int pos1, pos2;
  string new_def;
  
  //Remove "loop(" and ")"
  def=def.substr(5,def.size()-6);
  
  //store the info into GrpPara[i].init, end and increment
  pos1 = def.find(sep);
  new_def = def.substr(pos1+1,def.size()-pos1);
  pos2 = new_def.find(sep);
 
  GrpPara[i].init =atoi((def.substr(0,pos1)).c_str());
  GrpPara[i].last =atoi((def.substr(pos1+1,pos2).c_str()));
  GrpPara[i].increment = atoi((new_def.substr(pos2+1, new_def.size()-1).c_str()));
  
  return;
}

/**
 * \brief This function parses the value of the list of identifiers
 *
 * \param def (INPUT) is the definition part to parse
 * \param GrpPara (INPUT) is the Grp_parameters structure of the list of 
 * identifiers
 */
void ParseGrp_Value(string& def, int i, 
                    vector<Grp_parameters> & GrpPara)
{
  int b=0, a=0;

  while (b <= def.size())
    {
      if (def[b]==',')
	{
	  GrpPara[i].value.push_back(atoi((def.substr(a,b-a)).c_str()));
	  a=b+1;
	}
      if (b==def.size())
	{
	  GrpPara[i].value.push_back(atoi((def.substr(a,b-a)).c_str()));
	}
      ++b;
    }

  return;
}

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
                             vector<int> & GlobalArray)
{
  int a=0, b=0;
  
  //Parse Local array
  int i=0;
  
  //remove square braces
  LocGlobArray[i]=LocGlobArray[i].substr(1,LocGlobArray[i].size()-2);
  
  while (b <= LocGlobArray[i].size())
    {
      if (LocGlobArray[i][b]==',')
	{
	  LocalArray.push_back(atoi((LocGlobArray[i].substr(a,b-a)).c_str()));
          a=b+1;}
	
      if (b==LocGlobArray[i].size())
	{
	  LocalArray.push_back(atoi((LocGlobArray[i].substr(a,b-a)).c_str()));
        }
	
      ++b;
    }

  i=1,a=0,b=0;
  
  //Parse Global Array
  
  //remove square braces
  LocGlobArray[i]=LocGlobArray[i].substr(1,LocGlobArray[i].size()-2);
  
  while (b <= LocGlobArray[i].size())
    {
      if (LocGlobArray[i][b]==',')
	{
	  GlobalArray.push_back(atoi((LocGlobArray[i].substr(a,b-a)).c_str()));
          a=b+1;}
	
      if (b==LocGlobArray[i].size())
	{
	  GlobalArray.push_back(atoi((LocGlobArray[i].substr(a,b-a)).c_str()));
        }
      
      ++b;
    }

  return;
}

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
                      vector<Grp_parameters> & GrpPara)
{
  int HighestPriority;
  int GrpNumber = GrpPriority.size();
  int ArraySize = 1;
  int ArraySizeGlobal = 1;
 
  if (Tag[0]!="*")
    {
      HighestPriority = FindMaxPriority(GrpPriority);
    }
  
  //determine array size
  for (int i=0 ; i<LocalArray.size() ; ++i)
    {
      ArraySize *= LocalArray[i];
    }
  
  if (Tag[0] == "*")
    {
      GrpNumber=1;
      GrpPriority.push_back(1);
    }
  
  //determine array size
  for (int i=0 ; i<GlobalArray.size() ; ++i)
    {
      ArraySizeGlobal *= GlobalArray[i];
    }
  
  //Allocate memory for each grp
  binary_type **MyGrpArray;
  MyGrpArray = new binary_type*[GrpPriority.size()];

  for (int i=0 ; i<GrpPriority.size() ; ++i)
    {
      MyGrpArray[i]=new binary_type[ArraySizeGlobal];
      InitializeArray(MyGrpArray[i], GlobalArray);
    }

  //Allocate memory for the final array created
  void * NewArray;
  int rank=3;
  std::vector<int> dims(GlobalArray.size());
  for (int j=0 ; j< GlobalArray.size() ; ++j)
    {
      dims[j]=GlobalArray[j];
    }
  
  //  int dims[]={3,10,5};
  NXmalloc(&NewArray,rank,&(dims[0]),NX_INT32);
  
  //make an array for each group
  for (int i=0 ; i<GrpPriority.size() ; ++i)
    {
      
      if (Tag[i]=="pixelID") 
        {
          MakeArray_pixelID(MyGrpArray[i],
                            BinaryArray,
                            i,
                            InverseDef[i], 
                            Def, 
                            LocalArray, 
                            GlobalArray, 
                            GrpPara);
        }
      else if (Tag[i]=="pixelX")
        {
          MakeArray_pixelX(MyGrpArray[i],
                           BinaryArray,
                           i,
                           InverseDef[i], 
                           Def, 
                           LocalArray, 
                           GlobalArray, 
                           GrpPara);
        }
      else if (Tag[i]=="pixelY")
        {
          
          MakeArray_pixelY(MyGrpArray[i],
                           BinaryArray,
                           i,
                           InverseDef[i], 
                           Def, 
                           LocalArray, 
                           GlobalArray, 
                           GrpPara);
        }
      else if (Tag[i]=="Tbin")
        {
          MakeArray_Tbin(MyGrpArray[i],
                         BinaryArray,
                         i,
                         InverseDef[i], 
                         Def, 
                         LocalArray, 
                         GlobalArray, 
                         GrpPara);
        }
      else if (Tag[i]=="*")
        {
          MakeArray_Everything(MyGrpArray[i],
                               BinaryArray, 
                               LocalArray, 
                               GlobalArray);
        }
    }
  
  //free memory of binary array 
  delete[] BinaryArray;    
  
  //calculate the final Array according to all the parameters 
  //retrieved in the rest of the code 
  MakePriorities (GrpPriority, 
                  HighestPriority, 
                  OpePriority, 
                  MyGrpArray, 
                  GlobalArray, 
                  LocalArray, 
                  Ope, 
                  GrpPara);
 
  NewArray = MyGrpArray[0];
  
  delete[] MyGrpArray;  
  
  //write into nexus file
  Node node("New Array from string location",NewArray,rank,&(dims[0]),NX_INT32);
  
  tr.insert(tr.begin(),node);   
  
  NXfree(&NewArray);
  
  return;  
}

/**
 * \brief This function determines the highest priority of all the groups.
 * A group is a set of operation, can be a loop or a list of identifiers
 *
 * \param GrpPriority (INPUT) is the list of the priority
 *
 * \return the last maximum value of the list
 */
int FindMaxPriority (vector<int> & GrpPriority)
{
  int MaxValue = 0;
  
  for (int i=0 ; i<GrpPriority.size() ; ++i)
    {
      if (GrpPriority[i]>MaxValue)
        {
          MaxValue = GrpPriority[i];
        }
    }

  return MaxValue;
}

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
void MakeArray_pixelID (binary_type * MyGrpArray, 
                        binary_type * BinaryArray, 
                        int grp_number, 
                        int InverseDef, 
                        vector<string> & Def, 
                        vector<int> & LocalArray, 
                        vector<int> & GlobalArray, 
                        vector<Grp_parameters> & GrpPara)
{
  string loop="loop";
  
  if (Def[grp_number][0] == loop[0])  //case with loop
    {
      
      if (InverseDef==1)    //case inverse for loop
        {
          InversePixelIDLoop (MyGrpArray, 
                              BinaryArray, 
                              GlobalArray, 
                              GrpPara, 
                              grp_number);
        }
      else   //normal case for loop
        {
          PixelIDLoop (MyGrpArray, 
                       BinaryArray, 
                       GlobalArray, 
                       GrpPara, 
                       grp_number);
        }
    }
  else   //case with list of identifiers
    {
      if (InverseDef==1)   //case inverse for list
        {
          InversePixelIDList (MyGrpArray, 
                              BinaryArray, 
                              GlobalArray, 
                              GrpPara, 
                              grp_number);
        }
      else   //normal case for list
        {
          PixelIDList (MyGrpArray, 
                       BinaryArray, 
                       GlobalArray, 
                       GrpPara, 
                       grp_number);
        }
    }
  
  return;
}

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
                       vector<Grp_parameters> & GrpPara)
{
  string loop="loop";
  
  if (Def[grp_number][0] == loop[0]) 
    {
      if (InverseDef==1)   //case inverse with loop
        {
          InversePixelXLoop (MyGrpArray, 
                             BinaryArray, 
                             GlobalArray, 
                             GrpPara, 
                             grp_number);
        }
      else   //normal case with loop
        {
          PixelXLoop (MyGrpArray, 
                      BinaryArray, 
                      GlobalArray, 
                      GrpPara, 
                      grp_number);
        }
    }  
  else
    {
      if(InverseDef==1) //case inverse with list
        {
          InversePixelXList (MyGrpArray, 
                             BinaryArray, 
                             GlobalArray, 
                             GrpPara, 
                             grp_number);
        }
      else   //normal case with list
        {
          PixelXList (MyGrpArray, 
                      BinaryArray, 
                      GlobalArray, 
                      GrpPara, 
                      grp_number);
        }
    }
  return;
}

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
                       vector<Grp_parameters> & GrpPara)
{
  string loop="loop";
  
  if (Def[grp_number][0] == loop[0]) 
    {
      if (InverseDef==1)  //inverse case for loop
        {
          InversePixelYLoop (MyGrpArray, 
                             BinaryArray, 
                             GlobalArray, 
                             GrpPara, 
                             grp_number);
        }
      else  //normal case for loop
        {
          PixelYLoop (MyGrpArray, 
                      BinaryArray, 
                      GlobalArray, 
                      GrpPara, 
                      grp_number);
        }
    }
  else
    {
      if (InverseDef==1)   //case inverse with list
        {
          InversePixelYList (MyGrpArray, 
                             BinaryArray, 
                             GlobalArray, 
                             GrpPara, 
                             grp_number);
        }
      else    //normal case
        {
          PixelYList (MyGrpArray, 
                      BinaryArray, 
                      GlobalArray, 
                      GrpPara, 
                      grp_number);
        }
    }
  return;
}

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
                     vector<Grp_parameters> & GrpPara)
{
  string loop="loop";
  
  if (Def[grp_number][0] == loop[0]) 
    {
      if (InverseDef ==1)  //case inverse with loop
        {
          InverseTbinLoop (MyGrpArray, 
                           BinaryArray, 
                           GlobalArray, 
                           GrpPara, 
                           grp_number);
        }
      else   //normal case with loop
        {
          TbinLoop (MyGrpArray, 
                    BinaryArray, 
                    GlobalArray, 
                    GrpPara, 
                    grp_number);
        }
    }
  else
    {
      if (InverseDef==1)  //case inverse with list
        {
          InverseTbinList (MyGrpArray, 
                           BinaryArray, 
                           GlobalArray, 
                           GrpPara, 
                           grp_number);
        }
      else  //normal case with list
        {
          TbinList (MyGrpArray, 
                    BinaryArray, 
                    GlobalArray, 
                    GrpPara, 
                    grp_number);
        }
    }
  
  return;
}

/**
 * \brief This function makes a copy of the binary array
 *
 * \param MyGrpArray (OUTPUT) is the local copy of the binary array
 * \param BinaryArray (INPUT) is the array coming from the binary file
 * \param LocalArray (INPUT) ??? not used ???
 * \param GlobalArray (INPUT) is the list of parameters of the global
 * declaration part
 */
void  MakeArray_Everything (binary_type* MyGrpArray, 
                            binary_type* BinaryArray, 
                            vector<int> & LocalArray, 
                            vector<int> & GlobalArray)
{
  for (int y=0 ; y<GlobalArray[0] ; ++y)
    {
      for (int x=0; x<GlobalArray[1] ; ++x)
        {
          for (int tbin=0;tbin<GlobalArray[2];++tbin)
            {
              MyGrpArray[(x*GlobalArray[2]+tbin)+\
                         (y*GlobalArray[2]*GlobalArray[1])]=
                BinaryArray[(x*GlobalArray[2]+tbin)+\
                            (y*GlobalArray[2]*GlobalArray[1])];
            }	     
        }
    }
  return ;
}

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
void DoCalculation (binary_type * GrpArray1, 
                    binary_type * GrpArray2, 
                    string Operator, 
                    vector<int> & LocalArray, 
                    vector<int> & GlobalArray, 
                    vector<Grp_parameters> & GrpPara)
{
  string OR="OR";
  
  if (Operator[0] == OR[0])
    {
      for (int y=0 ; y<GlobalArray[0] ; ++y)
        {
          for (int x=0 ; x<GlobalArray[1] ; ++x)
            { 
              for (int tbin=0;tbin<GlobalArray[2];tbin++)
                {	  
                  if ((GrpArray1[(x*GlobalArray[2]+tbin)+
                                 (y*GlobalArray[2]*GlobalArray[1])]==0)&&
                      (GrpArray2[(x*GlobalArray[2]+tbin)+
                                 (y*GlobalArray[2]*GlobalArray[1])]!=0))
                    {
                      GrpArray1[(x*GlobalArray[2]+tbin)+
                                (y*GlobalArray[2]*GlobalArray[1])]=
                        GrpArray2[(x*GlobalArray[2]+tbin)+
                                  (y*GlobalArray[2]*GlobalArray[1])];
                    }
                }
            }
        }
    }
  else
    {
      for (int y=0 ; y<GlobalArray[0] ; ++y)
        {
          for (int x=0 ; x<GlobalArray[1]; ++x)
            {
              for (int tbin=0 ; tbin<GlobalArray[2] ; ++tbin)
                {
                  if (GrpArray2[(x*GlobalArray[2]+tbin)+
                                (y*GlobalArray[2]*GlobalArray[1])]!=
                      GrpArray1[(x*GlobalArray[2]+tbin)+
                                (y*GlobalArray[2]*GlobalArray[1])])
                    {
                      GrpArray1[(x*GlobalArray[2]+tbin)+
                                (y*GlobalArray[2]*GlobalArray[1])]=0;
                    }	     
                }
            }
        }
    }
  return;
}

/**
 * \brief This function swap endians
 *
 * \param x (INPUT/OUTPUT) number to swap
 */
inline void endian_swap (binary_type & x)
{
  x = ((x>>24) & 0x000000FF) |
    ((x<<8) & 0x00FF0000) |
    ((x>>8) & 0x0000FF00) |
    ((x<<24) & 0xFF000000);
}

/**
 * \brief This function initialize the array to 0
 *
 * \param MyGrpArray (INPUT/OUTPUT) is the array to initialized
 * \param GlobalArray (INPUT) is the dimension of the array
 */
void InitializeArray(binary_type * MyGrpArray, 
                     vector<int> & GlobalArray)
{
  for (int a=0 ; a<GlobalArray[1] ; ++a)
    {
      for (int b=0 ; b<GlobalArray[0] ; ++b)
	{
	  for (int c=0 ; c<GlobalArray[2] ; ++c)
	    {
	      MyGrpArray[c+b*GlobalArray[1]*
                         GlobalArray[2]+a*GlobalArray[2]]=0;
	    }
	}
    }  
  return;   
}

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
void InversePixelIDLoop (binary_type * MyGrpArray, 
                         binary_type * BinaryArray, 
                         vector<int> & GlobalArray, 
                         vector<Grp_parameters> & GrpPara, 
                         int grp_number)
{
  for (int i=0 ; i<GlobalArray[0]*GlobalArray[1] ; ++i)
    {
      for (int k=0 ; k<GlobalArray[2] ; ++k)
        {
          MyGrpArray[i*GlobalArray[2]+k]=BinaryArray[i*GlobalArray[2]+k];
        }
    }
  for (int i=GrpPara[grp_number].init;i<=GrpPara[grp_number].last;i=
         i+GrpPara[grp_number].increment)
    {
      for (int tbin=0 ; tbin < GlobalArray[2] ; ++tbin)
        {
          MyGrpArray[i*GlobalArray[2]+tbin]=0;
        }
    }
  return;
}

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
                  int grp_number)
{
  for (int i=GrpPara[grp_number].init;
       i<=GrpPara[grp_number].last;
       i=i+GrpPara[grp_number].increment)
    {
      for (int tbin=0 ; tbin < GlobalArray[2] ; ++tbin)
        {
          MyGrpArray[i*GlobalArray[2]+tbin]=
            BinaryArray[i*GlobalArray[2]+tbin];
        }
    }
  return;
}

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
                         int grp_number)
{
  for (int i=0 ; i<GlobalArray[0]*GlobalArray[1] ; ++i)
    {
      for (int k=0 ; k<GlobalArray[2] ; ++k)
        {
          MyGrpArray[i*GlobalArray[2]+k]=BinaryArray[i*GlobalArray[2]+k];
        }
    }
  for (int j=0 ; j<GrpPara[grp_number].value.size() ; ++j)
    {
      for (int tbin=0 ; tbin < GlobalArray[2] ; ++tbin)
        {
          MyGrpArray[GrpPara[grp_number].value[j]*GlobalArray[2]+tbin]=0;
        }
    }
  return;
}

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
                  int grp_number)
{
  for (int j=0 ; j<GrpPara[grp_number].value.size() ; ++j)
    {
      for (int tbin=0 ; tbin < GlobalArray[2] ; ++tbin)
        {
          MyGrpArray[GrpPara[grp_number].value[j]*GlobalArray[2]+tbin]=
            BinaryArray[GrpPara[grp_number].value[j]*GlobalArray[2]+tbin];
        }
    }
  return;
}

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
                        int grp_number)
{
  for (int i=0 ; i<GlobalArray[0]*GlobalArray[1] ; ++i)
    {
      for (int k=0 ; k<GlobalArray[2] ; ++k)
        {
          MyGrpArray[i*GlobalArray[2]+k]=BinaryArray[i*GlobalArray[2]+k];
        }
    }
  for (int y=0 ; y<GlobalArray[0] ; ++y)
    {
      for (int x = GrpPara[grp_number].init; 
           x <= GrpPara[grp_number].last; 
           x = x + GrpPara[grp_number].increment)
        {
          for (int tbin=0 ; tbin < GlobalArray[2] ; ++tbin)
            {
              MyGrpArray[(x*GlobalArray[2]+tbin)+
                         (y*GlobalArray[2]*GlobalArray[1])]=0;
            }
        } 
    }
  return;
}

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
                 int grp_number)
{
  for (int y=0 ; y<GlobalArray[0] ; ++y)
    {
      for (int x = GrpPara[grp_number].init; 
           x <= GrpPara[grp_number].last; 
           x = x + GrpPara[grp_number].increment)
        {
          for (int tbin=0 ; tbin<GlobalArray[2] ; ++tbin)
            {
              MyGrpArray[(x*GlobalArray[2]+tbin)+
                         (y*GlobalArray[2]*GlobalArray[1])]=
                BinaryArray[(x*GlobalArray[2]+tbin)+
                            (y*GlobalArray[2]*GlobalArray[1])];
            }
        } 
    }
  return;
}

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
                        int grp_number)
{
  for (int i=0 ; i<GlobalArray[0]*GlobalArray[1] ; ++i)
    {
      for (int k=0 ; k<GlobalArray[2]; ++k)
        {
          MyGrpArray[i*GlobalArray[2]+k]=BinaryArray[i*GlobalArray[2]+k];
        }
    }
  for (int y=0 ; y<GlobalArray[0] ; ++y)
    {
      for (int x=0 ; x<GrpPara[grp_number].value.size() ; ++x)
        {
          for (int tbin=0 ; tbin<GlobalArray[2] ; ++tbin)
            {
              MyGrpArray[(GrpPara[grp_number].value[x]*GlobalArray[2]+tbin)+
                         (y*GlobalArray[2]*GlobalArray[1])]=0;
            }
        }
    }
  return;
}

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
                 int grp_number)
{
  for (int y=0 ; y<GlobalArray[0] ; ++y)
    {
      for (int x=0 ; x<GrpPara[grp_number].value.size() ; ++x)
        {
          for (int tbin=0 ; tbin<GlobalArray[2] ; ++tbin)
            {
              MyGrpArray[(GrpPara[grp_number].value[x]*GlobalArray[2]+tbin)+
                         (y*GlobalArray[2]*GlobalArray[1])]=
                BinaryArray[(GrpPara[grp_number].value[x]*GlobalArray[2]+tbin)+
                            (y*GlobalArray[2]*GlobalArray[1])];
            }
        }
    }
  return;
}

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
                        int grp_number) 
{
  for (int i=0 ; i<GlobalArray[0]*GlobalArray[1] ; ++i)
    {
      for (int k=0 ; k<GlobalArray[2] ; ++k)
        {
          MyGrpArray[i*GlobalArray[2]+k]=BinaryArray[i*GlobalArray[2]+k];
        }
    }
  for (int y=GrpPara[grp_number].init; 
       y<=GrpPara[grp_number].last; 
       y=y + GrpPara[grp_number].increment)
    {
      for (int x=0; x<GlobalArray[1] ; ++x)
        {
          for (int tbin=0; tbin<GlobalArray[2] ; ++tbin)
            {
              MyGrpArray[(x*GlobalArray[2]+tbin)+
                         (y*GlobalArray[2]*GlobalArray[1])]=0;
            }
        } 
    }
  return;
}

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
                  int grp_number) 
{
  for (int y=GrpPara[grp_number].init; 
       y<=GrpPara[grp_number].last; 
       y=y + GrpPara[grp_number].increment)
    {
      for (int x=0 ; x<GlobalArray[1] ; ++x)
        {
          for (int tbin=0; tbin<GlobalArray[2] ; ++tbin)
            {
              MyGrpArray[(x*GlobalArray[2]+tbin)+
                         (y*GlobalArray[2]*GlobalArray[1])]=
                BinaryArray[(x*GlobalArray[2]+tbin)+
                            (y*GlobalArray[2]*GlobalArray[1])];
            }
        } 
    }
  return;
}

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
                        int grp_number) 
{
  for (int i=0 ; i<GlobalArray[0]*GlobalArray[1] ; ++i)
    {
      for (int k=0 ; k<GlobalArray[2] ; ++k)
        {
          MyGrpArray[i*GlobalArray[2]+k]=BinaryArray[i*GlobalArray[2]+k];
        }
    }
  for (int y=0 ; y<GrpPara[grp_number].value.size() ; ++y)
    {
      for (int x=0 ; x<GlobalArray[1] ; ++x)
        {
          for (int tbin=0 ; tbin<GlobalArray[2] ; ++tbin)
            {
              MyGrpArray[(x*GlobalArray[2]+tbin)+
                         (GrpPara[grp_number].value[y]*
                          GlobalArray[2]*GlobalArray[1])]=0;
            }
        }
    } 
  return;
}

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
                 int grp_number) 
{
  for (int y=0 ; y<GrpPara[grp_number].value.size() ; ++y)
    {
      for (int x=0; x<GlobalArray[1] ;++x)
        {
          for (int tbin=0 ; tbin<GlobalArray[2] ; ++tbin)
	    {
	      MyGrpArray[(x*GlobalArray[2]+tbin)+
                         (GrpPara[grp_number].value[y]*
                          GlobalArray[2]*GlobalArray[1])]= 
                BinaryArray[(x*GlobalArray[2]+tbin)+
                            (GrpPara[grp_number].value[y]*
                             GlobalArray[2]*GlobalArray[1])];
	    }
        }
    }
  return;
}

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
                      int grp_number) 
{
  for (int i=0 ; i<GlobalArray[0]*GlobalArray[1] ; ++i)
    {
      for (int k=0 ; k<GlobalArray[2] ; ++k)
        {
          MyGrpArray[i*GlobalArray[2]+k]=BinaryArray[i*GlobalArray[2]+k];
        }
    }
  for (int y=0 ; y<GlobalArray[0] ; ++y )
    {
      for (int x=0 ; x<GlobalArray[1] ; ++x)
        {
          for (int tbin=GrpPara[grp_number].init; 
               tbin <= GrpPara[grp_number].last; 
               tbin = tbin + GrpPara[grp_number].increment)
            {
              MyGrpArray[(x*GlobalArray[2]+tbin)+
                         (y*GlobalArray[2]*GlobalArray[1])]=0;
            }
        } 
    }
  return;
}

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
               int grp_number) 
{
  for (int y=0 ; y<GlobalArray[0] ; ++y )
    {
      for (int x=0; x<GlobalArray[1] ; ++x)
        {
          for (int tbin=GrpPara[grp_number].init; 
               tbin <= GrpPara[grp_number].last;
               tbin = tbin + GrpPara[grp_number].increment)
            {
              MyGrpArray[(x*GlobalArray[2]+tbin)+
                         (y*GlobalArray[2]*GlobalArray[1])]=
                BinaryArray[(x*GlobalArray[2]+tbin)+
                            (y*GlobalArray[2]*GlobalArray[1])];
            }
        } 
    }
  return;
}

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
                       int grp_number) 
{
  for (int i=0 ; i<GlobalArray[0]*GlobalArray[1] ; ++i)
    {
      for (int k=0 ; k<GlobalArray[2] ; ++k)
        {
          MyGrpArray[i*GlobalArray[2]+k]=BinaryArray[i*GlobalArray[2]+k];
        }
    }
  for (int y=0 ; y<GlobalArray[0] ; ++y)
    {
      for (int x=0 ; x<GlobalArray[1] ; ++x)
        {
          for (int tbin=0 ; tbin<GrpPara[grp_number].value.size() ; ++tbin)
            {
              MyGrpArray[(x*GlobalArray[2]+GrpPara[grp_number].value[tbin])+
                         y*GlobalArray[2]*GlobalArray[1]]=0;
            }
        }
    } 
  return;
}

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
               int grp_number) 
{
  for (int y=0 ; y<GlobalArray[0] ; ++y)
    {
      for (int x=0 ; x<GlobalArray[1] ; ++x)
        {
          for (int tbin=0 ; tbin<GrpPara[grp_number].value.size() ; tbin++)
            {
              MyGrpArray[(x*GlobalArray[2]+GrpPara[grp_number].value[tbin])+
                         y*GlobalArray[2]*GlobalArray[1]]= 
		BinaryArray[(x*GlobalArray[2]+GrpPara[grp_number].value[tbin])+
                            y*GlobalArray[2]*GlobalArray[1]];
            }
        }
    }
  return;
}

/**
 * \brief This function check for the highest priority in all the groups
 *
 * \param GrpPriority_size (INPUT) is the number of groups
 * \param CurrentPriority (INPUT/OUTPUT) is the current highest priority
 * \param GrpPriority (INPUT) is the list of the groups priority
 */
void CheckHighestPriority (int & GrpPriority_size, 
                           int &  CurrentPriority, 
                           vector<int> & GrpPriority)
{
  int find_one = 0;
  for (int m=0 ; m<GrpPriority_size ; ++m)
    {
      if (GrpPriority[m] >= CurrentPriority)
        {
          find_one = 1;
          break;
        }
    }
  if (find_one == 0)
    {
      --CurrentPriority;
    }
  return;
}

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
                               int & i)
{
  for (int k=i+1 ; k<GrpPriority_size-1 ; ++k)
    {
      GrpPriority[k]=GrpPriority[k+1];
    }
  return;
}

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
                                 int & i)
{
  for (int j=i;j<GrpPriority_size-2;++j)
    {
      OpePriority[j]=OpePriority[j+1];
      Ope[j]=Ope[j+1];
    }
  return;
}

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
                            int & i)
{
  for (int k=i+1;k<GrpPriority_size-1;++k)
    {
      MyGrpArray[k]=MyGrpArray[k+1];
    }
  return;
}

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
                     vector<Grp_parameters> & GrpPara)
{
  int CurrentPriority = HighestPriority;    
  int GrpPriority_size = GrpPriority.size();   //number of grp
  
  while (GrpPriority_size > 1) //as long as we have more than just one array
    {
      for (int i=0 ; i<GrpPriority_size ; ++i)   
        {
          //find position of first group with the highest priority
          if (GrpPriority[i] == CurrentPriority)
            {
              //check if it's the last one
              if (i == GrpPriority_size-1)
                {
                  --GrpPriority[i];
                  break;   //exit the for loop
                }
              else
                {
                  //check if the next operator has the same priority
                  if ((OpePriority[i] == GrpPriority[i]) && 
                      (GrpPriority[i+1] == GrpPriority[i]))
                    {
                      //do calculation according to Ope[i]
                      DoCalculation(MyGrpArray[i],
                                    MyGrpArray[i+1],
                                    Ope[i], 
                                    LocalArray, 
                                    GlobalArray, 
                                    GrpPara);

                      if (i < GrpPriority_size-2)
                        {  
                          //shift to the left the rest of the GrpPriorities
                          Shift_GrpPriorities_Left(GrpPriority, 
                                                   GrpPriority_size, 
                                                   i);
                          
                          //shift to the left the rest of the 
                          //operators and operators priorities
                          Shift_Ope_OpePriority_Left(OpePriority,
                                                     Ope, 
                                                     GrpPriority_size, 
                                                     i);
                          
                          //shift to the left the rest of the arrays
                          Shift_MyGrpArray_Left(MyGrpArray, 
                                                GrpPriority_size,
                                                i);
                        }
                      
                      //we have one less array/grp
                      --GrpPriority_size; 
                      --i;
                    }
                  else
                    {
                      --GrpPriority[i];   
                    }
                }
            }
        }
      
      //check what is the highest priority
      CheckHighestPriority(GrpPriority_size, 
                           CurrentPriority, 
                           GrpPriority);
    }
  return;
}  

/**
 * \brief This function checks if we want everything
 *
 * \param decla_def (INPUT) is the declaration part of the string location
 * \param Everything (INPUT,OUTPUT) is 1 if we want everything
 * \param Tag (INPUT) is the list of tags
 * \param Ope (INPUT) is the list of Operators
 */
void check_want_everything (vector<string> & decla_def, 
                            int & Everything, 
                            vector<string> & Tag, 
                            vector<string> & Ope) 
{
  if (decla_def[1] == "")
    {
      Everything = 1;
      Tag.push_back("*");
      Ope.push_back("*");
    }
  return;
}

/**
 * \brief This function swap endians of all the numbers of the BinaryArray
 *
 * \param GlobalArray (INPUT) is the size of the binary array
 * \param BinaryArray (INPUT/OUTPUT) is the array coming from the binary file
 */
void swap_endian (vector<int> & GlobalArray, 
                 binary_type * BinaryArray)
{
  for (int j=0 ; j<GlobalArray[1]*GlobalArray[2]*GlobalArray[0] ; ++j)
    {
      endian_swap(BinaryArray[j]);
    } 
  return;
}
