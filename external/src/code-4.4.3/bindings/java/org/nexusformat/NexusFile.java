/**
  * @mainpage This is an implementation of
  * a Java NeXus API using native methods.
  *
  * Some changes to the API have been necessary, due to the 
  * different calling standards between C and Java. 
  *
  * @author Mark Koennecke, 2000 -- 2011
  *
  * copyright: see accompanying COPYRIGHT file
  *
  * @see TestJapi.java
  * Test program for Java API.
  * Illustrates using the #org.nexusformat package
  */
package org.nexusformat;

import java.util.Hashtable;
import java.io.File;
import ncsa.hdf.hdflib.HDFArray;
import ncsa.hdf.hdflib.HDFException;
import ncsa.hdf.hdflib.HDFConstants;

public class NexusFile implements NeXusFileInterface {

    // constants 
    /**
      * possible access codes, @see #NexusFile.
      */
    public final static int NXACC_READ = 1;
    public final static int NXACC_RDWR = 2;
    public final static int NXACC_CREATE = 3;
    public final static int NXACC_CREATE4 = 4;
    public final static int NXACC_CREATE5 = 5;
    public final static int NXACC_CREATEXML = 6;
    public final static int NXACC_NOSTRIP = 128;
    
    /**
      * constant denoting an unlimited dimension.
      */
    public final static int NX_UNLIMITED = -1;

    /**
      * constants for number types. @see #makedata, @see #putattr 
      * and others.
      */
    public final static int NX_FLOAT32 = 5; 
    public final static int NX_FLOAT64 = 6; 
    public final static int NX_INT8 = 20; 
    public final static int NX_BINARY = 20;
    public final static int NX_UINT8 = 21; 
    public final static int NX_BOOLEAN = 21; 
    public final static int NX_INT16 = 22; 
    public final static int NX_UINT16 = 23; 
    public final static int NX_INT32 = 24; 
    public final static int NX_UINT32 = 25; 
    public final static int NX_INT64 = 26; 
    public final static int NX_UINT64 = 27; 
    public final static int NX_CHAR   = 4;

    /**
      * constants for compression schemes 
      */
    public final static int NX_COMP_NONE = 100;
    /* this one does zlib (deflate), no idea who chose the name */
    public final static int NX_COMP_LZW =  200;
    public final static int NX_COMP_RLE =  300; /* hdf4 only */
    public final static int NX_COMP_HUF =  400; /* hdf4 only */

    public final static int NX_COMP_LZW_LVL0 = (100*NX_COMP_LZW + 0);
    public final static int NX_COMP_LZW_LVL1 = (100*NX_COMP_LZW + 1);
    public final static int NX_COMP_LZW_LVL2 = (100*NX_COMP_LZW + 2);
    public final static int NX_COMP_LZW_LVL3 = (100*NX_COMP_LZW + 3);
    public final static int NX_COMP_LZW_LVL4 = (100*NX_COMP_LZW + 4);
    public final static int NX_COMP_LZW_LVL5 = (100*NX_COMP_LZW + 5);
    public final static int NX_COMP_LZW_LVL6 = (100*NX_COMP_LZW + 6);
    public final static int NX_COMP_LZW_LVL7 = (100*NX_COMP_LZW + 7);
    public final static int NX_COMP_LZW_LVL8 = (100*NX_COMP_LZW + 8);
    public final static int NX_COMP_LZW_LVL9 = (100*NX_COMP_LZW + 9);

    /**
      * Maximum name length, must be VGNAMELENMAX in hlimits.h
      */
    protected final static int MAXNAMELEN = 64;

    /*
       This code takes care of loading the static library required for
       this class to work properly. The algorithm first looks for a 
       property org.nexusformat.JNEXUSLIB and loads that file if available,
       else it tries to locate the library in the system shared library 
       path.
    */
     static {
        String filename = null;   
        filename = System.getProperty("org.nexusformat.JNEXUSLIB",null);
        if ((filename != null) && (filename.length() > 0)) {
            File hdfdll = new File(filename);
            if (hdfdll.exists() && hdfdll.canRead() && hdfdll.isFile()) {
                System.load(filename);
             } else {
                 throw (new UnsatisfiedLinkError("Invalid JNEXUS library"));
             }
         } else {
            System.loadLibrary("jnexus");
         }
      }

    /**
      * This is the handle to the NeXus file handle.
      */
    protected int handle;

    // Construction
    // native methods for this section
    protected native int  init(String filename, int access);
    protected native void close(int handle);
    protected native int  nxflush(int handle);
    
    /**
      * constructs a new NexusFile Object.
      * @param filename The name of the NeXus file to access.
      * @param access The access mode for the file. Can only be one
      * of the predefined NeXus access code NXACC.... These are:
      * <dl>
      * <dt>NXACC_CREATE
      * <DD>or creating a new file.
      * <DT>NXACC_RDWR
      * <DD>For opening an existing file for modification or appending 
      * data.
      * <DT>NXACC_READ
      * <DD>For opening a file for reading.
      * <DT>NXACC_NOSTRIP
      * <DD>To keep leading and trailing whitespace on strings
      * </dl>
      * @exception NexusException when the file could not be found or
      * an HDF error occurred.
      */
    public NexusFile(String filename, int access) throws NexusException {
         checkForNull(filename);

         handle = init(filename,access);
         if(handle < 0){
	    throw new NexusException("Failed to open " + filename);
	 }
    }

    /**
      * flushes all pending data to disk. Closes any open SDS's.
      */
    public void flush() throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
	 handle = nxflush(handle);
    }

    /**
     * close the NeXus file. To make javalint and diamond happy
     * @throws NexusException
     */
    public void close() throws NexusException {
        if(handle  >= 0) {
           close(handle);
           handle = -1;
        }
    }
    
    /**
      * removes all NeXus file data structures and closes the file. This 
      * function should automatically be called by the Java garbage 
      * collector whenever the NexusFile object falls into disuse. However
      * the time when this is done is left to the garbage collector. My
      * personal experience is that finalize might never be called. I
      * suggest, to call finalize yourself when you are done with the 
      * NeXus file. finalize makes sure that multiple invocations will not
      * do any harm.
      */   
    public void finalize() throws Throwable {
    	close();
    }


    // group functions
    //native methods for this section
    protected native void nxmakegroup(int handle, String name, String nxclass);
    protected native void nxopengroup(int handle, String name, String nxclass);
    protected native void nxopenpath(int handle, String path);
    protected native void nxopengrouppath(int handle, String path);
    protected native void nxclosegroup(int handle);
    protected native String nxgetpath(int handle);

    public void makegroup(String name, String nxclass) throws NexusException {
    	checkForNull(name, nxclass);
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
	nxmakegroup(handle, name, nxclass);
    }

    public void opengroup(String name, String nxclass) throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
        checkForNull(name, nxclass);
	nxopengroup(handle, name, nxclass);
    }

    public void openpath(String path) throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
        checkForNull(path);
	nxopenpath(handle,path);
    }

    public void opengrouppath(String path) throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
    	checkForNull(path);
	nxopengrouppath(handle,path);
    }

    public String getpath() throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
	return nxgetpath(handle);
    }

    public void closegroup() throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
	nxclosegroup(handle);
    }

    // data set handling
    // native methods for this section
    protected native void nxmakedata(int handle, String name, int type, int rank, int dim[]);
    protected native void nxmakedata64(int handle, String name, int type, int rank, long dim[]);
    protected native void nxmakecompdata(int handle, String name, int type, int rank, int dim[], int iCompress, int iChunk[]);
    protected native void nxmakecompdata64(int handle, String name, int type, int rank, long dim[], int iCompress, long iChunk[]);
    protected native void nxopendata(int handle, String name);
    protected native void nxclosedata(int handle);
    protected native void nxcompress(int handle, int compression_type);

    public void compmakedata(String name, int type, int rank, int dim[],
                   int compression_type, int iChunk[]) throws NexusException {
        if (handle < 0) throw new NexusException("NAPI-ERROR: File not open");
        checkType(type);    
        checkForNull(name, rank, iChunk);
        checkForNegInArray(true, dim, iChunk);
        checkCompression(compression_type);
	nxmakecompdata(handle, name, type, rank, dim, compression_type, iChunk);
    }

    public void compmakedata(String name, int type, int rank, long dim[],
                   int compression_type, long iChunk[]) throws NexusException {
        if (handle < 0) throw new NexusException("NAPI-ERROR: File not open");
        checkType(type);    
        checkForNull(name, rank, iChunk);
        checkForNegInArray(true, dim, iChunk);
        checkCompression(compression_type);
	nxmakecompdata64(handle, name, type, rank, dim, compression_type, iChunk);
    }

    public void makedata(String name, int type, int rank, int dim[]) throws
	                   NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
        checkType(type);
        checkForNull(name, dim);
        checkForNegInArray(true, dim);
        nxmakedata(handle, name, type, rank, dim);
    }

    public void makedata(String name, int type, int rank, long dim[]) throws
	                   NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
        checkType(type);
        checkForNull(name, dim);
        checkForNegInArray(true, dim);
        nxmakedata64(handle, name, type, rank, dim);
    }

    public void opendata(String name) throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
    	checkForNull(name);
	nxopendata(handle,name);
    }

    public void closedata() throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
	nxclosedata(handle);
    }

    public void compress(int compression_type) throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
        checkCompression(compression_type);
	nxcompress(handle,compression_type);
    }

    // data set reading
    // native methods in this section
    protected native void nxgetdata(int handle, byte bdata[]);
    protected native void nxgetslab(int handle, int Start[], int size[], byte bdata[]);
    protected native void nxgetslab64(int handle, long Start[], long size[], byte bdata[]);

    public void getdata(Object array) throws NexusException {
        byte bdata[];
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
    	checkForNull(array);
        try{
	    HDFArray ha = new HDFArray(array);
            bdata = ha.emptyBytes();
            nxgetdata(handle,bdata);
            array = ha.arrayify(bdata);
	 }catch(HDFException he) {
           throw new NexusException(he.getMessage());
	 }
    }

    public void getslab(int start[], int size[], Object array) throws NexusException {
        byte bdata[];
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
    	checkForNull(start, size, array);
    	checkForNegInArray(false, start, size);
        try{
	    HDFArray ha = new HDFArray(array);
            bdata = ha.emptyBytes();
            nxgetslab(handle,start,size,bdata);
            array = ha.arrayify(bdata);
	 }catch(HDFException he) {
           throw new NexusException(he.getMessage());
	 }
    }

    public void getslab(long start[], long size[], Object array) throws NexusException {
        byte bdata[];
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
    	checkForNull(start, size, array);
    	checkForNegInArray(false, start, size);
        try{
	    HDFArray ha = new HDFArray(array);
            bdata = ha.emptyBytes();
            nxgetslab64(handle,start,size,bdata);
            array = ha.arrayify(bdata);
	 }catch(HDFException he) {
           throw new NexusException(he.getMessage());
	 }
    }

    // data set writing
    // native methods for this section
    protected native void nxputdata(int handle, byte array[]); 
    protected native void nxputslab(int handle, byte array[], int start[], int size[]); 
    protected native void nxputslab64(int handle, byte array[], long start[], long size[]); 

    public void putdata(Object array) throws NexusException {
       byte data[];

       if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
   	   checkForNull(array);

       try {
           HDFArray ha =  new HDFArray(array);
           data = ha.byteify();
           ha = null;
       } catch (HDFException he) {
	   throw new NexusException(he.getMessage());
       }
       nxputdata(handle,data);
       data = null;
    }

    public void putslab(Object array, int start[], int size[]) throws NexusException {
       byte data[];

       if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
       checkForNull(array, start, size);
       checkForNegInArray(false, start, size);
       try {
           HDFArray ha =  new HDFArray(array);
           data = ha.byteify();
           ha = null;
       } catch(HDFException he) {
	   throw new NexusException(he.getMessage());
       }
       nxputslab(handle,data,start,size);
       data = null;
    }

    public void putslab(Object array, long start[], long size[]) throws NexusException {
       byte data[];

       if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
       checkForNull(array, start, size);
       checkForNegInArray(false, start, size);
       try {
           HDFArray ha =  new HDFArray(array);
           data = ha.byteify();
           ha = null;
       } catch(HDFException he) {
	   throw new NexusException(he.getMessage());
       }
       nxputslab64(handle,data,start,size);
       data = null;
    }

    // attribute methods
    protected native void nxgetattr(int handle, String name, byte bdata[], int args[]);
    protected native void nxputattr(int handle, String name, byte array[], int type); 
    protected native void nxputattra(int handle, String name, byte bdata[], int rank, int dim[], int iType);
    protected native int nxgetnextattra(int handle, String[] name, int dim[], int args[]);
    protected native void nxgetattra(int handle, String name, byte bdata[]);
    protected native void nxgetattrainfo(int handle, String name, int dim[], int args[]);
    protected native int nextattr(int handle, String names[], int args[]);
    protected native void initattrdir(int handle);
    protected native void initgroupdir(int handle);

    public Object getattr(String name) throws NexusException {

        byte bdata[];
        int args[] = new int[2];
        int dim[] = new int[32];
	int totalsize = 1;

	checkForNull(name);
        if (handle < 0) throw new NexusException("NAPI-ERROR: File not open");
	
    	nxgetattrainfo(handle, name, dim, args);

        int rank = args[0];
        int type = args[1];
        int[] thedims = new int[rank];
        for(int i = 0 ; i < rank ; i++) {
             thedims[i] = dim[i];
	     totalsize *= dim[i];
        }

	Object array = null;

	switch (type) {
    		case NX_CHAR:
		case NX_INT8:
			array = new byte[totalsize];
			break;
		case NX_INT16:
			array = new short[totalsize];
			break;
		case NX_INT32:
			array = new int[totalsize];
			break;
		case NX_INT64:
			array = new long[totalsize];
			break;
		case NX_FLOAT32:
			array = new float[totalsize];
			break;
		case NX_FLOAT64:
			array = new double[totalsize];
			break;
		case NX_BOOLEAN:
			throw new NexusException("type not currently supported in Java");
		case NX_UINT16:
		case NX_UINT32:
		case NX_UINT64:
			throw new NexusException("unsigned types not currently supported in Java");
		default:
			throw new NexusException("unknown type");
	}

        try {
	    HDFArray ha = new HDFArray(array);
            bdata = ha.emptyBytes();
            nxgetattra(handle, name, bdata);
            array = ha.arrayify(bdata);
	} catch(HDFException he) {
            throw new NexusException(he.getMessage());
	}

	if (type == NX_CHAR) {
		int noofstrings = totalsize / dim[rank-1];
		int n = 0;
		String strarr[] = new String[noofstrings];
		for(int i = 0 ; i < noofstrings ; i++) {
			strarr[i] = new String((byte[]) array, i * dim[rank-1], dim[rank-1]);	
		}
		array = strarr;
	}

        return array;
    }

    public void getattr(String name, Object array, int args[]) throws NexusException {
        byte bdata[];
        if (handle < 0) throw new NexusException("NAPI-ERROR: File not open");
        checkType(args[1]);
    	checkForNull(name, array, args);
        try {
	    HDFArray ha = new HDFArray(array);
            bdata = ha.emptyBytes();
            nxgetattr(handle, name, bdata, args);
            array = ha.arrayify(bdata);
	 } catch(HDFException he) {
           throw new NexusException(he.getMessage());
	 }
    }

    public void putattr(String name, Object array, int iType) throws NexusException {
       byte data[];

       if (handle < 0) throw new NexusException("NAPI-ERROR: File not open");
       checkType(iType);
       checkForNull(name, array);
       try {
           HDFArray ha =  new HDFArray(array);
           data = ha.byteify();
           ha = null;
       } catch(HDFException he) {
	   throw new NexusException(he.getMessage());
       }
       nxputattr(handle,name,data,iType);
       data = null;
    }

    public void putattr(String name, Object array, int size[], int iType) throws NexusException {
       byte data[];

       if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
       checkType(iType);
       checkForNull(name, array, size);
       try {
           HDFArray ha =  new HDFArray(array);
           data = ha.byteify();
           ha = null;
       } catch(HDFException he) {
	   throw new NexusException(he.getMessage());
       }
       nxputattra(handle, name, data, size.length, size, iType);
       data = null;
    }

    public Hashtable attrdir() throws NexusException {
        int args[] = new int[2];
        int dim[] = new int[32];
        AttributeEntry at;
        String names[] = new String[1];

        Hashtable h = new Hashtable();
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
	initattrdir(handle);
        
        while(nxgetnextattra(handle, names, dim, args) != -1) {
          int rank = args[0];
          int type = args[1];
	  String name = names[0];
	  int length = 1;
	  int[] thedims = new int[rank];
          for(int i = 0 ; i < rank ; i++) {
             thedims[i] = dim[i]; 
             length *= dim[i];
	  }
          at = new AttributeEntry();
	  at.dim = thedims; 
          at.length = length;
          at.type = type;
          h.put(name, at);
        } 
        return h;
    }

    // inquiry
    //native methods for this section
    protected native void nxgetinfo(int handle, int iDim[], int args[]);
    protected native void nxgetinfo64(int handle, long iDim[], int args[]);
    protected native void nxsetnumberformat(int handle, int type, 
					    String format);
    protected native int nextentry(int handle, String names[]);

    public void setnumberformat(int type, String format) throws NexusException {
       if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
       checkType(type);
       checkForNull(format);
       nxsetnumberformat(handle,type,format);
    }

    public void getinfo(int iDim[], int args[]) throws NexusException {
       if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
       nxgetinfo(handle,iDim,args);
    }

    public void getinfo(long iDim[], int args[]) throws NexusException {
       if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
       nxgetinfo64(handle,iDim,args);
    }

    public Hashtable groupdir() throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
        Hashtable h = new Hashtable();
        String names[] = new String[2];

        initgroupdir(handle);
        while(nextentry(handle,names) != -1) {
           h.put(names[0],names[1]);
        }
        return h;
    }

    // linking 
    // native methods for this section
    protected native void nxgetgroupid(int handle, NXlink link);
    protected native void nxgetdataid(int handle, NXlink link);
    protected native void nxmakelink(int handle, NXlink target); 
    protected native void nxmakenamedlink(int handle, String name, NXlink target); 
    protected native void nxopensourcepath(int handle); 

    public NXlink getgroupID() throws NexusException {
      if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
      NXlink l = new NXlink();
      nxgetgroupid(handle,l);
      return l;
    }

    public NXlink getdataID()throws NexusException {
      if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
      NXlink l = new NXlink();
      nxgetdataid(handle,l);
      return l;
    }

    public void makelink(NXlink target) throws NexusException {
      if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
      checkForNull(target);
      nxmakelink(handle, target);
    }

    public void makenamedlink(String name, NXlink target) throws NexusException {
      if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
      checkForNull(name, target);
      nxmakenamedlink(handle, name, target);
    }     

    public void opensourcepath() throws NexusException {
      if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
      nxopensourcepath(handle);
    }
	
    /**
     * checks if any of the arguments is null, 
     * throws appropriate runtime exception if so
     */
    private void checkForNull(Object... args) {
    	for (Object o : args)
    		if (o==null) throw new NullPointerException();
    }
    
    /**
     * checks if any of the ints in the arrays are negative, 
     * throws appropriate runtime exception if so
     */
    private void checkForNegInArray(boolean allowUnlimited, int[]... args) {
    	for (int[] array : args)
    		for (int value: array) {
    			if (value<0)
    				if (value == this.NX_UNLIMITED && allowUnlimited) {
    					// all ok this time
    				} else
    					throw new IllegalArgumentException("negative dimension received");
    		}
    }

    /**
     * checks if any of the longs in the arrays are negative, 
     * throws appropriate runtime exception if so
     */
    private void checkForNegInArray(boolean allowUnlimited, long[]... args) {
    	for (long[] array : args)
    		for (long value: array) {
    			if (value<0)
    				if (value == this.NX_UNLIMITED && allowUnlimited) {
    					// all ok this time
    				} else
    					throw new IllegalArgumentException("negative dimension received");
    		}
    }

    /**
      * checkType verifies if a parameter is a valid NeXus type code. 
      * If not an exception is thrown.
      * @param type The type value to check.
      * @exception NexusException if the the type is no known type value
      */
    private void checkType(int type) throws NexusException {
	switch(type) {
	case NexusFile.NX_FLOAT32:
	case NexusFile.NX_FLOAT64:
	case NexusFile.NX_INT8:
	case NexusFile.NX_UINT8:
	case NexusFile.NX_INT16:
	case NexusFile.NX_UINT16:
	case NexusFile.NX_INT32:
	case NexusFile.NX_UINT32:
	case NexusFile.NX_INT64:
	case NexusFile.NX_UINT64:
	case NexusFile.NX_CHAR:
	    break;
        default:
	    throw new NexusException("Illegal number type requested");
        }
    } 

    /**
      * checkCompression verifies a parameter is a valid NeXus compression code. 
      * If not an exception is thrown.
      * @param type The value to check.
      * @exception NexusException if the the type is no known compression value
      */
    private void checkCompression(int compression_type) throws NexusException {
        switch(compression_type) {
	case NexusFile.NX_COMP_NONE:
    	case NexusFile.NX_COMP_LZW:
    	case NexusFile.NX_COMP_RLE:
    	case NexusFile.NX_COMP_HUF:
	case NexusFile.NX_COMP_LZW_LVL0:
    	case NexusFile.NX_COMP_LZW_LVL1:
    	case NexusFile.NX_COMP_LZW_LVL2:
    	case NexusFile.NX_COMP_LZW_LVL3:
    	case NexusFile.NX_COMP_LZW_LVL4:
    	case NexusFile.NX_COMP_LZW_LVL5:
    	case NexusFile.NX_COMP_LZW_LVL6:
    	case NexusFile.NX_COMP_LZW_LVL7:
    	case NexusFile.NX_COMP_LZW_LVL8:
    	case NexusFile.NX_COMP_LZW_LVL9:
	    break;
	default:
	    throw new NexusException("Invalid compression code requested");
	}
    } 

    // external file interface
    // native methods for this section
    protected native void nxinquirefile(int handle, String names[]);
    protected native void nxlinkexternal(int handle, String name, String nxclass, String nxurl);
    protected native void nxlinkexternaldataset(int handle, String name, String nxurl);
    protected native int nxisexternalgroup(int handle, String name, String nxclass, String nxurl[]); 
    protected native int nxisexternaldataset(int handle, String name, String nxurl[]); 

    public String inquirefile() throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
	String names[] = new String[1];
	nxinquirefile(handle,names);
	return names[0];
    }

    public void linkexternal(String name, String nxclass, String nxurl) throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
    	checkForNull(name, nxclass, nxurl);
	nxlinkexternal(handle,name,nxclass,nxurl);
    }

    public void linkexternaldataset(String name, String nxurl) throws NexusException {
        if(handle < 0) throw new NexusException("NAPI-ERROR: File not open");
    	checkForNull(name, nxurl);
	nxlinkexternaldataset(handle,name,nxurl);
    }

    public String isexternalgroup(String name, String nxclass) throws NexusException {
        if (handle < 0) throw new NexusException("NAPI-ERROR: File not open");
    	checkForNull(name, nxclass);
	String nxurl[] = new String[1];

	int status = nxisexternalgroup(handle,name,nxclass,nxurl);
	if (status == 1) {
	    return nxurl[0];
	} else {
	    return null;
	}
    }

    public String isexternaldataset(String name) throws NexusException {
        if (handle < 0) throw new NexusException("NAPI-ERROR: File not open");
    	checkForNull(name);
	String nxurl[] = new String[1];

	int status = nxisexternaldataset(handle,name,nxurl);
	if (status == 1) {
	    return nxurl[0];
	} else {
	    return null;
	}
    }

    /**
      * debugstop is a debugging helper function which goes into an 
      * endless loop in the dynamic link library. Then a unix debugger
      * may attach to the running java process using the pid, interrupt,
      * set the loop variable to leave the loop, set a new breakpoint and
      * continue debugging. This works with ladebug on DU40D. This is an
      * developer support routine and should NEVER be called in normal
      * code. 
      */
    public native void debugstop();
}
