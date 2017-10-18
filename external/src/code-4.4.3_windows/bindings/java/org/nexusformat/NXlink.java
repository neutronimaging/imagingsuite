/**
  * NXlink is a replacement for the structure holding the data necessary
  * for doing a link in the NeXus-API. Consequently it is primitive.
  *
  * Mark Koennecke, October 2000
  * 
  * updated for Napi-2.0 with HDF-5
  * Mark Koennecke, August 2001
  *
  * updated for NAPI-3.0 with XML
  * Mark Koennecke, October 2004
  *
  * copyright: see accompanying COPYRIGHT file 
  */
package org.nexusformat;

public class NXlink {
    public int tag, ref, linkType;
    public String targetPath;
   
    public NXlink(){
       targetPath = new String("");
    }
}
