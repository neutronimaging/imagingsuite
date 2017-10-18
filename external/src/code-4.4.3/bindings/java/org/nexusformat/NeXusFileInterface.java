/**
  *
  * The NeXus-API for Java. NeXus is an attempt to define a common data 
  * format for org and x-ray diffraction. NeXus is built on top of the
  * Hierarchical Data Format from NCSA. There exist already API's to
  * NeXus files for F77, F90, C and C++. This is the interface definition
  * for a Java API to NeXus files.
  *
  * Some changes to the API have been necessary however, due to the 
  * different calling standards between C and Java. 
  *
  *
  * @author Mark Koennecke, 2000 -- 2011
  *
  * copyright: see accompanying COPYRIGHT file
  */
package org.nexusformat;

import java.util.Hashtable;

public interface NeXusFileInterface {

    // general functions
    /**
      * flush writes all previously unsaved data to disk. All directory
      * searches are invalidated. Any open SDS is closed.
      * @exception NexusException if an error occurs.
      */
    public void flush() throws NexusException;

    /**
      * finalize closes the file. It is supposed to be called by the
      * garbage collector when the object is collected. As this happens
      * at discretion of the garbage collector it is safer to call finalize
      * yourself, when a NeXus file needs to be closed. Multiple calls to
      * finalize do no harm.
      * @exception Throwable because it is required by the definition of
      * finalize. 
      */
    public void finalize() throws Throwable;

    /**
     * close the NeXus file. To make javalint and diamond happy
     * @throws NexusException
     */
    public void close() throws NexusException;

    // group functions
    /** 
      * makegroup creates a new group below the current group within
      * the NeXus file hierarchy.
      * @param name The name of the group to create.
      * @param nxclass The classname of the group.
      * @exception NexusException if an error occurs during this operation.
      */ 
    public void makegroup(String name, String nxclass) throws 
                            NexusException;
    /**
      * opengroup opens the group name with class nxclass. 
      * The group must exist, otherwise an exception is thrown. opengroup is
      * similar to a cd name in a filesystem.
      * @param name the name of the group to open.
      * @param nxclass the classname of the group to open. 
      * @exception NexusException when something goes wrong.
      */   
    public void opengroup(String name, String nxclass) throws 
                             NexusException;
    /**
      * openpath opens groups and datsets accroding to the path string
      * given. The path syntax follows unix conventions. Both absolute
      * and relative paths are possible. All objects of the path must
      * exist.
      * @param path The path string
      * @exception NexusException when something goes wrong.
      */   
    public void openpath(String path) throws NexusException;

    /**
      * opengrouppath opens groups and datsets accroding to the path string
      * given. The path syntax follows unix conventions. Both absolute
      * and relative paths are possible. All objects of the path must
      * exist. This function stops int the last group.
      * @param path The path string
      * @exception NexusException when something goes wrong.
      */   
    public void opengrouppath(String path) throws NexusException;

    /**
     * return the current path into the NeXus file in the 
     * form of a Unix path string.
     * @return A unix path string
     */
    public String getpath() throws NexusException;

    /**
      * closegroup closes access to the current group and steps down one
      * step in group hierarchy.
      * @exception NexusException when an error occurs during this
      * operation.
      */ 
    public void closegroup() throws NexusException;

    // data set handling
    /**
      * makedata creates a new dataset with the specified characteristics 
      * in the current group.
      * @param name The name of the dataset.
      * @param type The number type of the dataset. Usually a constant from
      * a selection of values.
      * @param rank The rank or number of dimensions of the dataset.
      * @param dim An array containing the length of each dimension. dim must
      * have at least rank entries. Dimension passed as -1 denote an
      * unlimited dimension.
      * @exception NexusException when the dataset could not be created.
      */ 
    public void makedata(String name, int type, int rank, int dim[]) 
	throws NexusException;

    /**
      * makedata creates a new dataset with the specified characteristics 
      * in the current group.
      * @param name The name of the dataset.
      * @param type The number type of the dataset. Usually a constant from
      * a selection of values.
      * @param rank The rank or number of dimensions of the dataset.
      * @param dim An array containing the length of each dimension. dim must
      * have at least rank entries. Dimension passed as -1 denote an
      * unlimited dimension.
      * @exception NexusException when the dataset could not be created.
      */ 
    public void makedata(String name, int type, int rank, long dim[]) 
	throws NexusException;

    /**
      * compmakedata creates a new dataset with the specified characteristics 
      * in the current group. This data set will be compressed.
      * @param name The name of the dataset.
      * @param type The number type of the dataset. Usually a constant from
      * a selection of values.
      * @param rank The rank or number of dimensions of the dataset.
      * @param dim An array containing the length of each dimension. dim must
      * have at least rank entries. Dimension passed as -1 denote an
      * unlimited dimension.
      * @param compression_type determines the compression type. 
      * @param iChunk With HDF-5, slabs can be written to compressed data 
      * sets. The size of these slabs is specified through the chunk array.
      * This must have the rank values for the size of the chunk to
      * be written in each dimension. 
      * @exception NexusException when the dataset could not be created.
      */ 
    public void compmakedata(String name, int type, int rank, int dim[],
             int compression_type, int iChunk[]) throws NexusException; 

    /**
      * compmakedata creates a new dataset with the specified characteristics 
      * in the current group. This data set will be compressed.
      * @param name The name of the dataset.
      * @param type The number type of the dataset. Usually a constant from
      * a selection of values.
      * @param rank The rank or number of dimensions of the dataset.
      * @param dim An array containing the length of each dimension. dim must
      * have at least rank entries. Dimension passed as -1 denote an
      * unlimited dimension.
      * @param compression_type determines the compression type. 
      * @param iChunk With HDF-5, slabs can be written to compressed data 
      * sets. The size of these slabs is specified through the chunk array.
      * This must have the rank values for the size of the chunk to
      * be written in each dimension. 
      * @exception NexusException when the dataset could not be created.
      */ 
    public void compmakedata(String name, int type, int rank, long dim[],
             int compression_type, long iChunk[]) throws NexusException; 

    /**
      * opendata opens an existing dataset for access. For instance for 
      * reading or writing.
      * @param name The name of the dataset to open.
      * @exception NexusException when the dataset does not exist or 
      * something else is wrong.
      */
    public void opendata(String name)throws NexusException;

    /**
      * closedata closes an opened dataset. Then no further access is 
      * possible without a call to opendata.
      * @exception NexusException when an error occurrs.
      */
    public void closedata() throws NexusException;

    /**
      * causes the currently open dataset to be compressed on file.
      * This must be called after makedata and before writing to the
      * dataset.
      * @param compression_type determines the type of compression 
      * to use.
      * @exception NexusException when no dataset is open or an error 
      * occurs.
      */ 
    public void compress(int compression_type) throws NexusException;

    // data set reading
    /**
      * getdata reads the data from an previously openend dataset into
      * array.
      * @param array An n-dimensional array of the appropriate number
      * type for the dataset. Make sure to have the right type and size
      * here.
      * @exception NexusException when either an error occurs or 
      * no dataset is open or array is not of the right type to hold
      * the data.
      */
    public void getdata(Object array) throws NexusException;

    /**
      * getslab reads a subset of a large dataset into array.
      * @param start An array of dimension rank which contains the start 
      * position in the dataset from where to start reading.
      * @param size An array of dimension rank which contains the size
      * in each dimension of the data subset to read.
      * @param array An array for holding the returned data values.
      * @exception NexusException when either an error occurs or 
      * no dataset is open or array is not of the right type to hold
      * the data.
      */
    public void getslab(int start[], int size[], Object array) throws
                          NexusException;

    /**
      * getslab reads a subset of a large dataset into array.
      * @param start An array of dimension rank which contains the start 
      * position in the dataset from where to start reading.
      * @param size An array of dimension rank which contains the size
      * in each dimension of the data subset to read.
      * @param array An array for holding the returned data values.
      * @exception NexusException when either an error occurs or 
      * no dataset is open or array is not of the right type to hold
      * the data.
      */
    public void getslab(long start[], long size[], Object array) throws
                          NexusException;

    /**
      * getattr retrieves the data associated with the attribute 
      * name. 
      * @param name The name of the attribute.
      * @param data an array with sufficient space for holding the attribute 
      * data.
      * @param args An integer array holding the number of data elements
      * in data as args[0], and the type as args[1]. Both values will be
      * updated while reading.
      * @exception NexusException when either an error occurs or 
      * the attribute could not be found.
      */
    public void getattr(String name, Object data, int args[]) throws
                          NexusException;

    /**
      * getattr retrieves the data associated with the attribute * name. 
      * @param name The name of the attribute.
      * @return The attribute data as an array.
      * @exception NexusException when either an error occurs or 
      * the attribute could not be found.
      */
    public Object getattr(String name) throws NexusException;

    // data set writing
    /**
      * putdata writes the data from array into a previously opened
      * dataset.
      * @param array The data to write.
      * @exception NexusException when an error occurs.
      */
    public void putdata(Object array) throws NexusException;

    /**
      * putslab writes a subset of a larger dataset to a previously opened
      * dataset.
      * @param array The data to write.
      * @param start An integer array of dimension rank which holds the
      * startcoordinates of the data subset in the larger dataset.
      * @param size An integer array of dimension rank whidh holds the
      * size in each dimension of the data subset to write.
      * @exception NexusException when an error occurs.
      */ 
    public void putslab(Object array, int start[], int size[]) throws
                          NexusException;

    /**
      * putslab writes a subset of a larger dataset to a previously opened
      * dataset.
      * @param array The data to write.
      * @param start An integer array of dimension rank which holds the
      * startcoordinates of the data subset in the larger dataset.
      * @param size An integer array of dimension rank whidh holds the
      * size in each dimension of the data subset to write.
      * @exception NexusException when an error occurs.
      */ 
    public void putslab(Object array, long start[], long size[]) throws
                          NexusException;

    /**
      * putattr adds a named attribute to a previously opened dataset or
      * group or a global attribute if nothing is open.
      * @param name The name of the attribute.
      * @param array The data of the attribute.
      * @param iType The number type of the attribute.
      * @exception NexusException if an error occurs.
      */  
    public void putattr(String name, Object array, int iType) throws
                          NexusException;

    /**
      * putattr adds a named attribute to a previously opened dataset or
      * group or a global attribute if nothing is open.
      * @param name The name of the attribute.
      * @param array The data of the attribute.
      * @param iType The number type of the attribute.
      * @exception NexusException if an error occurs.
      */  
    public void putattr(String name, Object array, int size[], int iType) throws
                          NexusException;

    // inquiry
    /**
      * getinfo retrieves information about a previously opened dataset.
      * @param iDim An array which will be filled with the size of
      * the dataset in each dimension.
      * @param args An integer array which will hold more information about
      * the dataset after return. The fields: args[0] is the rank, args[1] is
      * the number type.
      * @exception NexusException when  an error occurs.
      */ 
    public void getinfo(int iDim[], int args[]) throws NexusException;

    /**
      * getinfo retrieves information about a previously opened dataset.
      * @param iDim An array which will be filled with the size of
      * the dataset in each dimension.
      * @param args An integer array which will hold more information about
      * the dataset after return. The fields: args[0] is the rank, args[1] is
      * the number type.
      * @exception NexusException when  an error occurs.
      */ 
    public void getinfo(long iDim[], int args[]) throws NexusException;

    /**
     * setnumberformat sets the number format for printing number when
     * using the XML-NeXus format. For HDF4 and HDF5 this is ignored.
     * If a dataset is open, the format for the dataset is set, if none 
     * is open the default setting for the number type is changed.
     * The format must be a ANSII-C language format string.
     * @param type The NeXus type to set the format for. 
     * @param format The new format to use.
     */
    public void setnumberformat(int type, String format) throws NexusException;

    /**
      * groupdir will retrieve the content of the currently open vGroup.
      * groupdir is similar to an ls in unix. 
      * @return A Hashtable  which will hold the names of the items in 
      * the group as keys and the NeXus classname for vGroups or the 
      * string 'SDS' for datasets as values. 
      * @exception NexusException if an error occurs
      */
    public Hashtable groupdir() throws NexusException;

    /**
      * attrdir returns the attributes of the currently open dataset or
      * the file global attributes if no dataset is open.
      * @return A Hashtable which will hold the names of the attributes
      * as keys. For each key there is an AttributeEntry class as value.
      * @exception NexusException when an error occurs.
      */ 
    public Hashtable attrdir() throws NexusException;
    
    // linking 
    /**
      * getgroupID gets the data necessary for linking the current vGroup
      * somewhere else.
      * @return A NXlink object holding the link data.
      * @exception NexusException if an error occurs.
      */
    public NXlink getgroupID() throws NexusException;

    /**
      * getdataID gets the data necessary for linking the current dataset
      * somewhere else.
      * @return A NXlink object holding the link data.
      * @exception NexusException if an error occurs.
      */
    public NXlink getdataID() throws NexusException;

    /**
      * makelink links the object described by target into the current
      * vGroup.
      * @param target The Object to link into the current group.
      * @exception NexusException if an error occurs.
      */
    public void makelink(NXlink target) throws NexusException;     
    /**
      * makenamedlink links the object described by target into the current
      * vGroup. The object will have a new name in the group into which it is 
      * linked
      * @param target The Object to link into the current group.
      * @param name The name of this object in the current group
      * @exception NexusException if an error occurs.
      */
    public void makenamedlink(String name, NXlink target) throws NexusException;     

    /**
      * opensourcepath opens the group from which the current item was linked
      * Returns an error if the current item is not linked.
      * @exception NexusException if an error occurs.
      */
    public void opensourcepath() throws NexusException;     

    /**
     * inquirefile inquires which file we are currently in. This is
     * a support function for external linking
     * @return The current file
     * @throws NexusException when things are wrong
     */
    public String inquirefile() throws NexusException;

    /** 
     * linkexternal links group name, nxclass to the URL nxurl
     * @param name The name of the vgroup to link to
     * @param nxclass The class name of the linked vgroup
     * @param nxurl The URL to the linked external file
     * @throws NexusException if things are wrong
     */
    public void linkexternal(String name, String nxclass, String nxurl) throws NexusException;

    /** 
     * linkexternaldataset links dataset name to the URL nxurl
     * @param name The name of the dataset to link to
     * @param nxurl The URL to the linked external file
     * @throws NexusException if things are wrong
     */
    public void linkexternaldataset(String name, String nxurl) throws NexusException;

    /**
     * nxisexternalgroup test the group name, nxclass if it is linked externally
     * @param name of the group to test
     * @param  nxclass class of the group to test
     * @return null when the group is not linked, else a string giving the URL of the
     * linked resource
     * @throws NexusException if things are wrong
     */
    public String isexternalgroup(String name, String nxclass) throws NexusException;

    /**
     * nxisexternaldataset if the named dataset is is linked externally
     * @param name of the dataset to test
     * @return null when the it is not linked, else a string giving the URL of the
     * linked resource
     * @throws NexusException if things are wrong
     */
    public String isexternaldataset(String name) throws NexusException;
}
