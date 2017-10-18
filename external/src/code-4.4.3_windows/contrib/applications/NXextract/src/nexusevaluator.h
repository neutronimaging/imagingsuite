//*****************************************************************************
// Synchrotron SOLEIL
//
// in this Class we evaluate a data by looking for its value in the Nexus File.
//
// Creation : 20/07/2005
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

#ifndef __NEXUSEVALUATOR_H__
#define __NEXUSEVALUATOR_H__

using namespace gdshare; 

// Forward declaration
class DataBuf;

// we use this map to stock the NexusDataSet that we have already look for.
typedef map<String, NexusDataSet *> DataSetMap;
//typedef pair<String, NexusDataSet *> NamedDataSetPair;
// we use this map to stock the names of groups that we need in our iterations (
// loop-over).

typedef map<String, String> ListGroup;
typedef pair<String,String> NxItem;  // (name, path) pair
typedef list<NxItem> ItemList;
typedef map<String, ItemList> ListCache;

//-----------------------------------------------------------------------------
// struct SSplittedRequest
// Store a request components
//-----------------------------------------------------------------------------
struct SSplittedRequest
{
  // Request type
  enum Type
  {
    DATASET_ARRAY_ELEMENT = 0,
    DATASET,
    DATASET_ATTRIBUTE,
    GLOBAL_ATTRIBUTE,
    SIZE_ATTRIBUTE,
    RANK_ATTRIBUTE,
    SCANDIM_ATTRIBUTE,
    NAME_ATTRIBUTE
  };

  Type   eType;         // Request type
  String strFullPath;   // Full request
  String strGroupPath;  // Path to NeXus group
  String strDataSet;    // DataSet name
  String strAttr;       // Attribute name
  String strLastPart;   // coordinate part
  String strDataSetPath;// path to dataset

  vector<String> vecArrayCoordinates; // 
};
typedef map<String, SSplittedRequest> MapRequests;
//typedef pair<String, MapRequests::iterator> ItRequestNamedPair;
//-----------------------------------------------------------------------------
// Class NexusEvaluator
//
//-----------------------------------------------------------------------------
class NexusEvaluator
{
private:
  NexusFile           m_nxf;
  DataSetMap          m_mapDataSetCache;
  MapRequests         m_mapRequestCache;
  ListCache           m_mapListCache;
  ListGroup           m_listGroupe;
  class ICounterVars *m_pCounterVars;

  // Get the data from NeXus data set.
  void GetData(const NexusDataSet &nxDataRes, int *piDim, DataBuf *pValue, bool bGetAllBuffer=false);
  // Get the data from NeXus data set.
  void GetDataEx(const NexusDataSet &nxDataRes, int *piDim, DataBuf *pValue, bool bGetAllBuffer=false);
  // Get 2D data slices
  void Get2DDataFromArray(const NexusDataSet &nxDataSet, int *piDim, DataBuf *pValue);
  // Get 1D data slices
  void Get1DDataFromArray(const NexusDataSet &nxDataSet, int *piDim, DataBuf *pValue);
  // Get 0D data slices
  void GetScalarDataFromArray(const NexusDataSet &nxDataSet, int *piDim, DataBuf *pValue);
  // Retreive data set
  bool GetDataSet(SSplittedRequest *pRequest, NexusDataSet **ppNxDataSet, bool bUseCache=true);
  // Retreive data set information
  bool GetDataSetInfo(SSplittedRequest *pRequest, NexusDataSet **ppNxDataSet);
  // Retreive request in cache
  SSplittedRequest *GetSplittedRequest(const String &strRequest);
  
  // Get "_size_" attribute
  bool GetSizeAttribute(const NexusDataSet &nds, const String &strAttrSpec, DataBuf *pValue);
  // Get coordinates
  void GetCoordinates(const SSplittedRequest &Request, int *piDim, int iRank);
  // get some usefull attributes (file_name,file_type..)
  bool ReadAttribute(const String &strAttr, DataBuf *pValue, NexusDataType *pNxType);
  // get the data to evaluate if bExist is true so we don't have to
  // open the file else we open it.
  bool OpenGroups(const String &strData);

public:
  NexusEvaluator(const String &strNeXusFile, ICounterVars *pCounterVars);
  virtual ~NexusEvaluator(); 
  // Get items list
  ItemList *GetItemsList(String *strPath, const String &strName); 
  // Evalute the a string and fill it in the end of the procedure by its value.
  // return false if the string can't be evaluated.
  virtual bool Evaluate(const String &strVar, DataBuf *pValue);

  // Look for a item (group or dataset)
  // return true if it exists, otherwise false
  bool CheckItem(const String &_strPath);
};

#endif
