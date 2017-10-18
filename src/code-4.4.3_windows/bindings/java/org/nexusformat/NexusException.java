/**
  * NexusException is thrown whenever an error occurs in the NeXus Java API
  *
  * Mark Koennecke, October 2000
  *
  * copyright: see accompanying COPYRIGHT file.  
  */ 
package org.nexusformat;

public class NexusException extends Exception {

	
	static public final String OutOfMemoryMessage= 
                        "ERROR: NeXus-API: Out of memory";
	static public final String NexusExceptionMessage= 
                        "ERROR: NeXus-API Error"; 
	static public final String NeXusMessage=
                        "ERROR: Unknown NeXus-API Error"; 

	int HDFerror;
	String msg;

	public NexusException() {
		HDFerror = 0;
	}

	public NexusException(String s) {
		msg = s;
	}

	public NexusException(int err) {
		HDFerror = err;
	}

	public String getMessage() {
		return msg;
	}
}

