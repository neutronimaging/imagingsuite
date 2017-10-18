/**
  * This is a little helper class which holds additional information about
  * a dataset or global attribute.
  *
  * @see NeXusFileInterface.
  *
  * copyright: see acompanying COPYRIGHT file.
  */
package org.nexusformat;

public class AttributeEntry {
    /**
      * length is the length of the attribute.
      * type is the number type of the attribute.
      */
  public int length, type, dim[];
}
