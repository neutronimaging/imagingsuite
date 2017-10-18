/*
 * code for napi_exports.c
 */

NXstatus CALL_MODE NXISETCACHE(long newVal)
{
    return NXsetcache(newVal);
}

/*
void CALL_MODE NXNXNXREPORTERROR(void *pData, char *string)
{
    NXNXNXReportError(pData, string);
}
*/
     
NXstatus CALL_MODE NXIOPEN(CONSTCHAR *filename, NXaccess am, NXhandle *gHandle)
{
    return NXopen(filename, am, gHandle);
}

NXstatus CALL_MODE NXICLOSE(NXhandle *fid)
{
    return NXclose(fid);
}

NXstatus CALL_MODE NXIFLUSH(NXhandle* pHandle)
{
    return NXflush(pHandle);
}

NXstatus CALL_MODE NXIMAKEGROUP(NXhandle handle, CONSTCHAR *name, CONSTCHAR* NXclass)
{
    return NXmakegroup(handle, name, NXclass);
}

NXstatus CALL_MODE NXIOPENGROUP(NXhandle handle, CONSTCHAR *name, CONSTCHAR* NXclass)
{
    return NXopengroup(handle, name, NXclass);
}

NXstatus CALL_MODE NXIOPENPATH(NXhandle handle, CONSTCHAR *path)
{
    return NXopenpath(handle, path);
}

NXstatus CALL_MODE NXIOPENGROUPPATH (NXhandle handle, CONSTCHAR *path)
{
    return NXopengrouppath(handle, path);
}

NXstatus CALL_MODE NXICLOSEGROUP(NXhandle handle)
{
    return NXclosegroup(handle);
}
  
NXstatus CALL_MODE NXIMAKEDATA (NXhandle handle, CONSTCHAR* label, int datatype, int rank, int dim[])
{
    return NXmakedata (handle, label, datatype, rank, dim);
}

NXstatus CALL_MODE NXICOMPMAKEDATA (NXhandle handle, CONSTCHAR* label, int datatype, int rank, int dim[], int comp_typ, int bufsize[])
{
    return NXcompmakedata (handle, label, datatype, rank, dim, comp_typ, bufsize);
}

NXstatus CALL_MODE NXICOMPRESS (NXhandle handle, int compr_type)
{
    return NXcompress (handle, compr_type);
}

NXstatus CALL_MODE NXIOPENDATA (NXhandle handle, CONSTCHAR* label)
{
    return NXopendata (handle, label);
}

NXstatus CALL_MODE NXICLOSEDATA(NXhandle handle)
{
    return NXclosedata(handle);
}

NXstatus CALL_MODE NXIPUTDATA(NXhandle handle, void* data)
{
    return NXputdata(handle, data);
}

NXstatus CALL_MODE NXIPUTATTR(NXhandle handle, CONSTCHAR* name, void* data, int iDataLen, int iType)
{
    return NXputattr(handle, name, data, iDataLen, iType);
}

NXstatus CALL_MODE NXIPUTSLAB(NXhandle handle, void* data, int start[], int size[])
{
    return NXputslab(handle, data, start, size);
}

NXstatus CALL_MODE NXIGETDATAID(NXhandle handle, NXlink* pLink)
{
    return NXgetdataID(handle, pLink);
}

NXstatus CALL_MODE NXIMAKELINK(NXhandle handle, NXlink* pLink)
{
    return NXmakelink(handle, pLink);
}

NXstatus CALL_MODE NXIOPENSOURCEGROUP(NXhandle handle)
{
    return NXopensourcegroup(handle);
}

NXstatus CALL_MODE NXIGETDATA(NXhandle handle, void* data)
{
    return NXgetdata(handle, data);
}

NXstatus CALL_MODE NXIGETINFO(NXhandle handle, int* rank, int dimension[], int* datatype)
{
    return NXgetinfo(handle, rank, dimension, datatype);
}

NXstatus CALL_MODE NXIGETNEXTENTRY(NXhandle handle, NXname name, NXname nxclass, int* datatype)
{
    return NXgetnextentry(handle, name, nxclass, datatype);
}

NXstatus CALL_MODE NXIGETSLAB(NXhandle handle, void* data, int start[], int size[])
{
    return NXgetslab(handle, data, start, size);
}

NXstatus CALL_MODE NXIGETNEXTATTR(NXhandle handle, NXname pName, int *iLength, int *iType)
{
    return NXgetnextattr(handle, pName, iLength, iType);
}

NXstatus CALL_MODE NXIGETATTR(NXhandle handle, char* name, void* data, int* iDataLen, int* iType)
{
    return NXgetattr(handle, name, data, iDataLen, iType);
}

NXstatus CALL_MODE NXIGETATTRINFO(NXhandle handle, int* no_items)
{
    return NXgetattrinfo(handle, no_items);
}

NXstatus CALL_MODE NXIGETGROUPID(NXhandle handle, NXlink* pLink)
{
    return NXgetgroupID(handle, pLink);
}

NXstatus CALL_MODE NXIGETGROUPINFO(NXhandle handle, int* no_items, NXname name, NXname nxclass)
{
    return NXgetgroupinfo(handle, no_items, name, nxclass);
}

NXstatus CALL_MODE NXISAMEID(NXhandle handle, NXlink* pFirstID, NXlink* pSecondID)
{
    return NXsameID(handle, pFirstID, pSecondID);
}

NXstatus CALL_MODE NXIINITGROUPDIR(NXhandle handle)
{
    return  NXinitgroupdir(handle);
}
NXstatus CALL_MODE NXIINITATTRDIR(NXhandle handle)
{
    return  NXinitattrdir(handle);
}
NXstatus CALL_MODE NXISETNUMBERFORMAT(NXhandle handle, int type, char *format)
{
    return  NXsetnumberformat(handle,type, format);
}

NXstatus CALL_MODE NXIMALLOC(void** data, int rank, int dimensions[], int datatype)
{
    return NXmalloc(data, rank, dimensions, datatype);
}

NXstatus CALL_MODE NXIFREE(void** data)
{
    return NXfree(data);
}

#if 0
/*-----------------------------------------------------------------------
    NAPI internals 
------------------------------------------------------------------------*/
extern  void  NXMSetError(void *pData, void (*ErrFunc)(void *pD, char *text));
extern void (*NXIReportError)(void *pData,char *text);
extern void *NXpData;
extern char *NXIformatNeXusTime();
#endif

/* FORTRAN internals */

NXstatus CALL_MODE NXIFOPEN(char * filename, NXaccess* am, 
					NexusFunction* pHandle)
{
    return NXfopen(filename, am, pHandle);
}

NXstatus CALL_MODE NXIFCLOSE (NexusFunction* pHandle)
{
  return  NXfclose (pHandle);
}

NXstatus CALL_MODE NXIFPUTATTR(NXhandle fid, char *name, void *data, 
                                   int *pDatalen, int *pIType)
{
  return  NXfputattr(fid, name, data, pDatalen, pIType);
}

NXstatus CALL_MODE NXIFCOMPRESS(NXhandle fid, int *compr_type)
{
  return  NXfcompress(fid, compr_type);
}

NXstatus CALL_MODE NXIFCOMPMAKEDATA(NXhandle fid, char *name, 
                int *pDatatype,
		int *pRank, int dimensions[],
                int *compression_type, int chunk[])
{
  return  NXfcompmakedata(fid, name, pDatatype, pRank, dimensions,
                compression_type, chunk);
}

NXstatus CALL_MODE NXIFMAKEDATA(NXhandle fid, char *name, int *pDatatype,
		int *pRank, int dimensions[])
{
  return  NXfmakedata(fid, name, pDatatype, pRank, dimensions);
}

NXstatus CALL_MODE NXIFFLUSH(NexusFunction* pHandle)
{
  return NXfflush(pHandle);
}

NXstatus CALL_MODE NXIINQUIREFILE(NXhandle handle, char *filename, int filenameBufferLength)
{
    return NXinquirefile(handle, filename, filenameBufferLength);
}

NXstatus CALL_MODE NXIISEXTERNALGROUP(NXhandle fid, CONSTCHAR *name, CONSTCHAR *nxclass, char *url, int urlLen)
{
    return NXisexternalgroup(fid, name, nxclass, url, urlLen);
}

NXstatus CALL_MODE NXILINKEXTERNAL(NXhandle fid, CONSTCHAR *name, CONSTCHAR *nxclass, CONSTCHAR *url)
{
    return NXlinkexternal(fid, name, nxclass, url);
}

NXstatus CALL_MODE NXIMAKENAMEDLINK(NXhandle fid, CONSTCHAR *newname, NXlink* pLink)
{
    return NXmakenamedlink(fid, newname, pLink);
}

NXstatus CALL_MODE NXIGETRAWINFO(NXhandle handle, int* rank, int dimension[], int* datatype)
{
    return NXgetrawinfo(handle, rank, dimension, datatype);
}

