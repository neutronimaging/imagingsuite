#include <iostream>
#include <vector>
#include <string>

using std::string;
using std::cout;
using std::endl;
using std::vector;


void CheckMakeArrayPixelList (int i, vector<int> & GlobalArray, binary_type * MyGrpArray, string str);
void checkMakePriorities(vector<int> & GrpPriority, int GrpPriority_size);
void checkGrpParaValue(int i, vector<Grp_parameters> & GrpPara);
void view_array(int & x, int & y, int & z, binary_type * MyGrpArray);
void checkDeclaration(vector<int> & LocalArray, vector<int> & GlobalArray);


void CheckMakeArrayPixelList (int i, vector<int> & GlobalArray, binary_type * MyGrpArray, string str)
{
  if (str == "PIXELX")
    {
      cout << endl << "****RETRIEVER_MAKE_ARRAY_PIXELX_LIST****************************************"<<endl;
      cout << endl << "Check if MyGrpArray["<<i<<"] after MakeArray_pixelX in CalculateArray is correct: " << endl;
    }
  else if (str == "PIXELY")
    {
      cout << endl << "****RETRIEVER_MAKE_ARRAY_PIXELY_LIST****************************************"<<endl;
      cout << endl << "Check if MyGrpArray["<<i<<"] after MakeArray_pixelY in CalculateArray is correct: " << endl;
    }
  else if (str == "PIXELYbefore")
    {
      cout << endl << "****RETRIEVER_MAKE_ARRAY_PIXELY_LIST****************************************"<<endl;
      cout << endl << "Check if MyGrpArray["<<i<<"] before MakeArray_pixelY in CalculateArray is correct: " << endl;
    }
  else if (str == "TBIN")
    {
      cout << endl << "****RETRIEVER_MAKE_ARRAY_TBIN_LIST****************************************"<<endl;
      cout << endl << "Check if MyGrpArray["<<i<<"] after MakeArray_Tbin in CalculateArray is correct: " << endl;
    }
  else if (str == "EVERYTHING")
    {
      cout << endl << "****RETRIEVER_EVERYTHING****************************************";
      cout << endl << "Check if MyGrpArray["<<i<<"] in CalculateArray is correct: " << endl<<endl;
    }
  else if (str == "FINAL")
    {
      cout << endl << "****RETRIEVER_FINAL_RESULT****************************************";
      cout << endl << "Check the final NewArray: " << endl<<endl;
    }
  else if (str == "PIXELID")
    {
      cout << endl << "****RETRIEVER_MAKE_ARRAY_PIXELID_LIST*******************************"<<endl;;
      cout << endl << "Check if MyGrpArray in Make_PixelID is correct: " << endl;
    }
  else if (str == "PIXELIDbefore")
    {
      cout << endl << "****RETRIEVER_MAKE_ARRAY_PIXELID_LIST*******************************"<<endl;;
      cout << endl << "Check if MyGrpArray["<<i<<"] in CalculateArray is correct: " << endl;
    }
  
  int ll = 0;
  int mm = 0;
          cout << "   ";

	  for (int j=0; j<GlobalArray[1]*GlobalArray[2]*GlobalArray[0]; ++j)
	    {
	      cout << MyGrpArray[j] << " ";            //listing of MyGrpArray    
      
	      if (ll == (GlobalArray[2]-1))
		{
		  cout << endl << "   ";
		  ll = 0;
		  ++mm;
		}
	      else
		{
		  ++ll;
		}
	      if (mm == (GlobalArray[1]))
		{
		  cout << endl;
		  cout << "   ";
		  mm = 0;
		}
	    } 

  return;
}

void checkMakePriorities(vector<int> & GrpPriority, int GrpPriority_size)
{
    cout << endl << "****RETRIEVER_MAKE_PRIORITIES***************************"<<endl;

      for (int j=0; j<GrpPriority_size;j++)
	{
	  if (GrpPriority[j]>-1)
	    {
	      cout << "   Grp["<<j<<"]/Pri#"<<GrpPriority[j];
	    }
	  else
	    {
	      cout << "   Grp["<<j<<"]/Pri#"<<GrpPriority[j]+1<<" ==>Done!"<<endl;
	    }
	}

      cout << endl;

  return;
}

void checkGrpParaValue(int i, vector<Grp_parameters> & GrpPara)
{
  cout << endl << "Def["<<i<<"], list of values from list of identifiers: " << endl;

   for (int j=0; j<GrpPara[i].value.size(); j++)
    {
      cout << "   GrpPara.value["<<i<<"]= " << GrpPara[i].value[j]<<endl;
    }

  return;
}

/*******************************************
/Print array for testing purpose
/*******************************************/
void view_array(int & x, int & y, int & z, binary_type * MyGrpArray)
{
   for (int a=0; a<y;++a)
	{
	  for (int b=0; b<x;++b)
	    {
	      for (int c=0; c<z;++c)
		{
		  cout << MyGrpArray[c+b*y*z+a*z];
		  
		  cout << " ";
		}
	      cout << "\t";
	    }
	  cout << endl;
	}
      cout << endl;
	 
      return;
}


void checkDeclaration(vector<int> & LocalArray, vector<int> & GlobalArray)
{
  cout << endl << "Parsing of values: " << endl;
  cout << "   Local array" << endl;

  for (int j=0; j<LocalArray.size();j++)
    {
      cout << "      LocalArray["<<j<<"]= "<<LocalArray[j];
    }

  cout << endl <<"   Global array" << endl;
  
  for (int k=0; k<GlobalArray.size();++k)
    {
      cout << "      GlobalArray["<<k<<"]= "<<GlobalArray[k];
    }
  return;
}
