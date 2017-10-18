//*****************************************************************************
// Synchrotron SOLEIL
//
//
// Creation : 01/12/2006
// Author   : Stephane Poirier
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; version 2 of the License.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT 
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//*****************************************************************************

#include "base.h"
#include "file.h"
#include "membuf.h"
#include "nxfile.h"
#include "variant.h"

#include <iostream>
#include <fstream>
#include <sstream>  
#include <vector>
#include <cstdlib>
#include <cstring>

#include "nexusevaluator.h"
#include "extractor.h"
#include "templateparsor.h"

using namespace std;

//-----------------------------------------------------------------------------
/// Class ExtractorApp
/// Main class for NeXus extractor application
//-----------------------------------------------------------------------------
class ExtractorApp: public IVariableEvaluator
{
private:
  StringDict  m_dictParam;        // Parameters dictionnary

public:

  /// Setting command line options
  void OnCommandLineOpts();

  /// Entry point
  int OnRun();

  /// Program exit
  int OnEnd();

  /// Get collection of input files
  void GetInputFiles(std::set<String> *psetSources);

  /// Add template parameters
  void AddParameters(const String &strParamList);

  /// Evaluate a variable and give its value if it has been stocked in the DictVar.
  virtual bool Evaluate(String *pstrVar);
};

//-----------------------------------------------------------------------------
// ExtractorApp::AddParameters
//-----------------------------------------------------------------------------
void ExtractorApp::AddParameters(const String &strParamList)
{
  String strList = strParamList;
  String strParamDef, strParamName, strParamSubtitution;
  while( !strList.empty() )
  {
    strList.ExtractToken(',', &strParamDef);
    if( strParamDef.find('=') != string::npos )
    {
      strParamDef.ExtractToken('=', &strParamName);
      strParamSubtitution = strParamDef;
    }
    else
    {
      strParamName = strParamDef;
      strParamSubtitution = "<defined>";
    }
    // Add paramater only if not always defined
    if( !m_dictParam.HasKey(strParamName) )
        m_dictParam.insert(StringDict::value_type(strParamName, strParamSubtitution));
  }
}

//-----------------------------------------------------------------------------
// ExtractorApp::Evalutate
//-----------------------------------------------------------------------------
bool ExtractorApp::Evaluate(String *pstrVar)
{ 
  // Look into the application dictionnary
  StringDict::iterator it = m_dictParam.find(*pstrVar);
  if( it != m_dictParam.end())
  {
    *pstrVar=(it->second);
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
// ExtractorApp::OnCommandLineOpts
//-----------------------------------------------------------------------------
void ExtractorApp::OnCommandLineOpts()
{
  CCommandLine::SetCmdNameVersion("nxextract", "1.12.0");
  CCommandLine::AddOpt('t', "template", "file", "Template file");
  CCommandLine::AddOpt('D', "Define", "symbols", "Symbols list");
  CCommandLine::AddOpt('s', "silent", NULL, "Silent mode");
#ifdef __LINUX__
  CCommandLine::AddOpt('m', "mode", "octal value", "Access mode of output files in octal ex:'755'");
  CCommandLine::AddOpt('w', "ownership", "user_id:group_id", "User and group in the form 'user:group'");
#endif
  CCommandLine::AddArg("input NeXus Files and/or directories");
}

//-----------------------------------------------------------------------------
// ExtractorApp::GetInputFiles
//-----------------------------------------------------------------------------
void ExtractorApp::GetInputFiles(std::set<String> *psetSources)
{
  FileName fnPathArg;

  for( int i = 0; i < CommandLine::ArgCount(); i++ )
  {
    String strPattern;
    bool bDoEnum = false;

    // Reads next argument
    fnPathArg.Set(CommandLine::Arg(i));

    if( fnPathArg.IsPathName() )
      // Consider full directory content
      bDoEnum = true;
    else if( fnPathArg.NameExt().find_first_of("*?") != string::npos )
    {
      // Consider only file whose name match the given pattern
      bDoEnum = true;
      strPattern = fnPathArg.NameExt();
    }

    if( bDoEnum )
    { // Directory enumeration
      FileEnum feDir(fnPathArg.Path(), FileEnum::ENUM_FILE);
      while( feDir.Find() )
      {
        if( strPattern.empty() || feDir.NameExt().Match(PSZ(strPattern)) )
          psetSources->insert(feDir.FullName());
      }
    }
    else
    {
      if( fnPathArg.FileExists() )
        psetSources->insert(fnPathArg.FullName());
      else
        cerr << "Input file '" << fnPathArg.FullName() << " not found." << endl;
    }
  }
}

//-----------------------------------------------------------------------------
// ExtractorApp::OnRun
//-----------------------------------------------------------------------------
int ExtractorApp::OnRun()
{
  String strTemplate = CommandLine::OptionValue("template");

  if( !gdshare::FileExists(PSZ(strTemplate)) )
  {
    cerr << "Template file '" << strTemplate << " not found." << endl;
    return 1;
  }

  // Build input files list from command line arguments
  std::set<String> setInputFiles;
  GetInputFiles(&setInputFiles);

  // Look for file properties options
  CString strMode, strOwnerShip;
  if( CommandLine::IsOption("mode") )
    strMode = CommandLine::OptionValue("mode");
  if( CommandLine::IsOption("ownership") )
    strOwnerShip = CommandLine::OptionValue("ownership");

  if( CCommandLine::IsOption("Define") )
    // Adds parameters found in command line
    AddParameters(CommandLine::OptionValue("Define"));

  // For execution time
  CurrentDate tmStart;

  // Script tokens vector
  VecToken vecToken;

  // Script parsor
  TemplateFileParsor Parsor(&vecToken);
  // Get default values for template parameters
  String strParams;
  Parsor.ReadHeader(strTemplate, &strParams);
  AddParameters(strParams);
  // Parse the script and build tokens
  Parsor.Parse(strTemplate, this);

  // For each input files run the script
  for( std::set<String>::const_iterator cit = setInputFiles.begin(); cit != setInputFiles.end(); cit++ )
  {
    try
    {
      // Script execution object
      Extractor Extractor(*cit, &vecToken, this, strMode, strOwnerShip);

      // Create specifics parameters
      FileName fn(*cit);
      Extractor.SetVar("_FILE_NAME_", fn.Name());

      if( CommandLine::IsOption("silent") )
        // Set silent mode
        Extractor.SetSilent();

      // Run the script
      Extractor.Execute();
    }
    catch( NexusException e )
    {
      e.PrintMessage();
    }
  }

  if( !CommandLine::IsOption("silent") )
    LogInfo("proc", "Elapsed time : %.3lf sec.", CurrentDate().DoubleUnix() - tmStart.DoubleUnix());
  return 0;
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  ExtractorApp App;
  App.OnCommandLineOpts();

  if( !CommandLine::Read(argc, argv) )
    return 1;

  int iRc = App.OnRun();
  if( iRc )
    return iRc;

  return 0;
}
