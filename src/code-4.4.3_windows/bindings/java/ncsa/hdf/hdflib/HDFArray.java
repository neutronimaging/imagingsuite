/****************************************************************************
 * NCSA HDF                                                                 *
 * National Comptational Science Alliance                                   *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 *                                                                          *
 * For conditions of distribution and use, see the accompanying             *
 * hdf/COPYING file.                                                        *
 *                                                                          *
 ****************************************************************************/

package ncsa.hdf.hdflib;

/**
 *  This is a class for handling multidimensional arrays for
 *  HDF.
 *  <p>
 *  The purpose is to allow the storage and retrieval of
 *  arbitrary array types containing scientific data.
 *  <p>
 *  The methods support the conversion of an array to and
 *  from Java to a one-dimensional array of bytes suitable
 *  for I/O by the C library.
 *  <p>
 *  This class heavily uses the <a href="./ncsa.hdf.hdflib.HDFNativeData.html">HDFNativeData</a>
 *  class to convert between Java and C representations.
 */

public class HDFArray {

private Object _theArray = null;
private ArrayDescriptor _desc = null;
private byte [] _barray = null;

public HDFArray(Object anArray) throws HDFException {

	if (anArray == null) {
		HDFException ex =
		(HDFException)new HDFJavaException("HDFArray: array is null?: ");
ex.printStackTrace();
	}
	Class tc = anArray.getClass();
        if (tc.isArray() == false) {
                /* exception: not an array */
		HDFException ex =
		(HDFException)new HDFJavaException("HDFArray: not an array?: ");
ex.printStackTrace();
		throw(ex);
        }
	_theArray = anArray;
	_desc = new ArrayDescriptor( _theArray );

	/* extra error checking -- probably not needed */
	if (_desc == null ) {
		HDFException ex =
		(HDFException)new HDFJavaException("HDFArray: internal error: array description failed?: ");
		throw(ex);
	}
}

/**
 *  allocate a one-dimensional array of bytes sufficient to store
 *  the array.
 *  @exception HDFException .
 */

public byte[] emptyBytes()
throws HDFException
{
	byte[] b = null;
	if (_desc.dims == 1 && _desc.NT == 'B') {
		b = (byte [])_theArray;
	} else {
		b = new byte[_desc.totalSize];
	}
	if (b == null) {
                System.out.println("Error:  HDFArray can't allocate bytes for array");
 HDFException ex =
		(HDFException)new HDFJavaException("HDFArray: emptyBytes: allocation failed");
                        throw(ex);
	}
	return (b);
	//return (new byte[_desc.totalSize]);
}

/**
 *  Given a Java array of numbers, convert it to a one-dimensional
 *  array of bytes in correct native order.
 *
 *  @exception ncsa.hdf.hdflib.HDFException 
 *             thrown for errors: 
 *		object is not array:  HDFJavaException
 */
public byte[] byteify() throws HDFException{

	if (_barray != null) return _barray;
	if (_theArray == null) {
                /* exception: not an array */
		 HDFException ex =
		(HDFException)new HDFJavaException("HDFArray: byteify not an array?: ");
		throw(ex);
        }

	if (_desc.dims == 1) {
		/* special case */
		if (_desc.NT == 'B') {
			/* really special case! */
			_barray = (byte [])_theArray;
			return _barray;
		} else {
			try {
			_barray = new byte[_desc.totalSize];

			byte [] therow;
			if (_desc.NT == 'I') {
				therow = ncsa.hdf.hdflib.HDFNativeData.intToByte(0,_desc.dimlen[1],(int [])_theArray);
			} else if (_desc.NT == 'S') {
				therow = ncsa.hdf.hdflib.HDFNativeData.shortToByte(0,_desc.dimlen[1],(short [])_theArray);
			} else if (_desc.NT == 'F') {
				therow = ncsa.hdf.hdflib.HDFNativeData.floatToByte(0,_desc.dimlen[1],(float [])_theArray);
			} else if (_desc.NT == 'J') {
				therow = ncsa.hdf.hdflib.HDFNativeData.longToByte(0,_desc.dimlen[1],(long [])_theArray);
			} else if (_desc.NT == 'D') {
				therow = ncsa.hdf.hdflib.HDFNativeData.doubleToByte(0,_desc.dimlen[1],(double [])_theArray);
			} else if (_desc.NT == 'L') {
				if (_desc.className.equals("java.lang.Byte")) {
					therow = ByteObjToByte((Byte[])_theArray);
				} else if (_desc.className.equals("java.lang.Integer")) {
					therow = IntegerToByte((Integer[])_theArray);
				} else if (_desc.className.equals("java.lang.Short")) {
					therow = ShortToByte((Short[])_theArray);
				} else if (_desc.className.equals("java.lang.Float")) {
					therow = FloatObjToByte((Float[])_theArray);
				} else if (_desc.className.equals("java.lang.Double")) {
					therow = DoubleObjToByte((Double[])_theArray);
				} else if (_desc.className.equals("java.lang.Long")) {
					therow = LongObjToByte((Long[])_theArray);
				} else {
					 HDFJavaException ex =
						new HDFJavaException("HDFArray: unknown type of Object?");
					 throw(ex);
				}
			} else {
				HDFJavaException ex =
					new HDFJavaException("HDFArray: unknown type of Object?");
				throw(ex);
			}
			System.arraycopy(therow,0,_barray,0,(_desc.dimlen[1] * _desc.NTsize));
			return _barray;
			} catch (OutOfMemoryError err) {
				 HDFException ex =
				(HDFException)new HDFJavaException("HDFArray: byteify array too big?");
				ex.printStackTrace();
				throw(ex);
			}
		}
	}

	try {
		_barray = new byte[_desc.totalSize];
        } catch (OutOfMemoryError err) {
	 HDFException ex =
		(HDFException)new HDFJavaException("HDFArray: byteify array too big?");
		ex.printStackTrace();
                        throw(ex);
	}


	Object oo = _theArray;
	int n = 0;  /* the current byte */
	int index = 0;
	int i;
	while ( n < _desc.totalSize ) {
		oo = _desc.objs[0];
		index = n / _desc.bytetoindex[0];
                index %= _desc.dimlen[0];
		for (i = 0 ; i < (_desc.dims); i++) {
			index = n / _desc.bytetoindex[i];
			index %= _desc.dimlen[i];

			if (index == _desc.currentindex[i]) {
				/* then use cached copy */
				oo = _desc.objs[i];
			} else {
				/* check range of index */		
				if (index > (_desc.dimlen[i] - 1)) {
					System.out.println("out of bounds?");
					return null;
				}
				oo = java.lang.reflect.Array.get((Object) oo,index);
				_desc.currentindex[i] = index;
				_desc.objs[i] = oo;
			}
		}

		/* byte-ify */
		byte arow[];
		try {
		if (_desc.NT == 'J') {
			arow = ncsa.hdf.hdflib.HDFNativeData.longToByte(0,_desc.dimlen[_desc.dims],(long [])_desc.objs[_desc.dims - 1]);
			arow = ncsa.hdf.hdflib.HDFNativeData.longToByte(0,_desc.dimlen[_desc.dims],(long [])_desc.objs[_desc.dims - 1]);
		} else if (_desc.NT == 'I') {
			arow = ncsa.hdf.hdflib.HDFNativeData.intToByte(0,_desc.dimlen[_desc.dims],(int [])_desc.objs[_desc.dims - 1]);
		} else if (_desc.NT == 'S') {
			arow = ncsa.hdf.hdflib.HDFNativeData.shortToByte(0,_desc.dimlen[_desc.dims],(short [])_desc.objs[_desc.dims - 1]);
		} else if (_desc.NT == 'B') {
			arow = (byte [])_desc.objs[_desc.dims - 1];
		} else if (_desc.NT == 'F') {
			/* 32 bit float */
			arow = ncsa.hdf.hdflib.HDFNativeData.floatToByte(0,_desc.dimlen[_desc.dims],(float [])_desc.objs[_desc.dims - 1]);
		} else if (_desc.NT == 'D') {
			/* 64 bit float */
			arow = ncsa.hdf.hdflib.HDFNativeData.doubleToByte(0,_desc.dimlen[_desc.dims],(double [])_desc.objs[_desc.dims - 1]);
		} else if (_desc.NT == 'L') {
			if (_desc.className.equals("java.lang.Byte")) {
				arow = ByteObjToByte((Byte[])_desc.objs[_desc.dims - 1]);
			} else if (_desc.className.equals("java.lang.Integer")) {
				arow = IntegerToByte((Integer[])_desc.objs[_desc.dims - 1]);
			} else if (_desc.className.equals("java.lang.Short")) {
				arow = ShortToByte((Short[])_desc.objs[_desc.dims - 1]);
			} else if (_desc.className.equals("java.lang.Float")) {
				arow = FloatObjToByte((Float[])_desc.objs[_desc.dims - 1]);
			} else if (_desc.className.equals("java.lang.Double")) {
				arow = DoubleObjToByte((Double[])_desc.objs[_desc.dims - 1]);
			} else if (_desc.className.equals("java.lang.Long")) {
				arow = LongObjToByte((Long[])_desc.objs[_desc.dims - 1]);
			} else {
				HDFJavaException ex =
				new HDFJavaException("HDFArray: byteify Object type not implemented?");
				throw(ex);
			}
		} else {
			HDFJavaException ex =
			new HDFJavaException("HDFArray: byteify Object type not implemented?");
			throw(ex);
		}
		System.arraycopy(arow,0,_barray,n,(_desc.dimlen[_desc.dims] * _desc.NTsize));
		n += _desc.bytetoindex[_desc.dims - 1];
		} catch (OutOfMemoryError err) {
		 HDFException ex =
			(HDFException)new HDFJavaException("HDFArray: byteify array too big?");
			ex.printStackTrace();
			throw(ex);
		}
	}
/* assert:  the whole array is completed--currentindex should == len - 1 */

	/* error checks */

	if (n < _desc.totalSize) {
		throw new java.lang.InternalError(
		new String("HDFArray:::byteify: Panic didn't complete all input data: n=  "+n+" size = "+_desc.totalSize));
	}
	for (i = 0;i < _desc.dims; i++) {
		if (_desc.currentindex[i] != _desc.dimlen[i] - 1) {
			throw new java.lang.InternalError(
			new String("Panic didn't complete all data: currentindex["+i+"] = "+_desc.currentindex[i]+" (should be "+(_desc.dimlen[i] - 1)+" ?)"));
		}
	}
	return _barray;
}

/**
 *  Given a one-dimensional array of numbers, convert it to a java
 *  array of the shape and size passed to the constructor.
 *
 *  @exception ncsa.hdf.hdflib.HDFException 
 *             thrown for errors: 
 *		object not an array:  HDFJavaException 
 */
public Object arrayify(byte[] bytes) throws HDFException {

	if (_theArray == null) {
                /* exception: not an array */
		 HDFException ex = 
		(HDFException)new HDFJavaException("arrayify: not an array?: "); 
		throw(ex); 
	} 

	if (java.lang.reflect.Array.getLength((Object) bytes) != _desc.totalSize) { 
		/* exception: array not right size */ 
		 HDFException ex = 
		(HDFException)new HDFJavaException("arrayify: array is wrong size?: "); 
	} 
	_barray = bytes; /* hope that the bytes are correct.... */ 
	if (_desc.dims == 1) {
		/* special case */
		/* 2 data copies here! */
		try {
		if (_desc.NT == 'I') {
			int [] x = (int [])ncsa.hdf.hdflib.HDFNativeData.byteToInt(_barray);
			System.arraycopy(x,0,_theArray,0,_desc.dimlen[1]);
			return _theArray;
		} else if (_desc.NT == 'S') {
			short [] x = ncsa.hdf.hdflib.HDFNativeData.byteToShort(_barray);
			System.arraycopy(x,0,_theArray,0,_desc.dimlen[1]);
			return _theArray;
		} else if (_desc.NT == 'F') {
			float x[] = ncsa.hdf.hdflib.HDFNativeData.byteToFloat(_barray);
			System.arraycopy(x,0,_theArray,0,_desc.dimlen[1]);
			return _theArray;
		} else if (_desc.NT == 'J') {
			long x[] = ncsa.hdf.hdflib.HDFNativeData.byteToLong(_barray);
			System.arraycopy(x,0,_theArray,0,_desc.dimlen[1]);
			return _theArray;
		} else if (_desc.NT == 'D') {
			double x[] = ncsa.hdf.hdflib.HDFNativeData.byteToDouble(_barray);
			System.arraycopy(x,0,_theArray,0,_desc.dimlen[1]);
			return _theArray;
		} else if (_desc.NT == 'B') {
			System.arraycopy(_barray,0,_theArray,0,_desc.dimlen[1]);
			return _theArray;
		} else if (_desc.NT == 'L') {
			if (_desc.className.equals("java.lang.Byte")) {
				Byte I[] = ByteToByteObj(_barray);
				System.arraycopy(I,0,_theArray,0,_desc.dimlen[1]);
				return _theArray;
			} else if (_desc.className.equals("java.lang.Integer")) {
				Integer I[] = ByteToInteger(_barray);
				System.arraycopy(I,0,_theArray,0,_desc.dimlen[1]);
				return _theArray;
			} else if (_desc.className.equals("java.lang.Short")) {
				Short I[] = ByteToShort(_barray);
				System.arraycopy(I,0,_theArray,0,_desc.dimlen[1]);
				return _theArray;
			} else if (_desc.className.equals("java.lang.Float")) {
				Float I[] = ByteToFloatObj(_barray);
				System.arraycopy(I,0,_theArray,0,_desc.dimlen[1]);
				return _theArray;
			} else if (_desc.className.equals("java.lang.Double")) {
				Double I[] = ByteToDoubleObj(_barray);
				System.arraycopy(I,0,_theArray,0,_desc.dimlen[1]);
				return _theArray;
			} else if (_desc.className.equals("java.lang.Long")) {
				Long I[] = ByteToLongObj(_barray);
				System.arraycopy(I,0,_theArray,0,_desc.dimlen[1]);
				return _theArray;
			} else {
			HDFJavaException ex =
			new HDFJavaException("arrayify:  Object type not implemented yet...");
			throw(ex); 
			}
		} else {
			HDFJavaException ex =
			new HDFJavaException("arrayify:  Object type not implemented yet...");
			throw(ex); 
		}
		} catch (OutOfMemoryError err) {
		 HDFException ex =
			(HDFException)new HDFJavaException("HDFArray: arrayify array too big?");
			ex.printStackTrace();
			throw(ex);
		}
	}
	/* Assert dims >= 2 */

	Object oo = _theArray;
	int n = 0;  /* the current byte */
	int index = 0;
	int i;
	while ( n < _desc.totalSize ) {
		oo = _desc.objs[0];
		index = n / _desc.bytetoindex[0];
		index %= _desc.dimlen[0];
		for (i = 0 ; i < (_desc.dims); i++) {
			index = n / _desc.bytetoindex[i];
			index %= _desc.dimlen[i];

			if (index == _desc.currentindex[i]) {
				/* then use cached copy */
				oo = _desc.objs[i];
			} else {
				/* check range of index */		
				if (index > (_desc.dimlen[i] - 1)) {
					System.out.println("out of bounds?");
					return null;
				}
				oo = java.lang.reflect.Array.get((Object) oo,index);
				_desc.currentindex[i] = index;
				_desc.objs[i] = oo;
			}
		}

		/* array-ify */
		try {
		if (_desc.NT == 'J') {
			long [] arow = ncsa.hdf.hdflib.HDFNativeData.byteToLong(n,_desc.dimlen[_desc.dims],_barray);
			java.lang.reflect.Array.set(_desc.objs[_desc.dims - 2] ,
				(_desc.currentindex[_desc.dims - 1]), (Object)arow);
			n += _desc.bytetoindex[_desc.dims - 1];
			_desc.currentindex[_desc.dims - 1]++;
		} else if (_desc.NT == 'I') {
			int [] arow = ncsa.hdf.hdflib.HDFNativeData.byteToInt(n,_desc.dimlen[_desc.dims],_barray);
			java.lang.reflect.Array.set(_desc.objs[_desc.dims - 2] ,
				(_desc.currentindex[_desc.dims - 1]), (Object)arow);

			n += _desc.bytetoindex[_desc.dims - 1];
			_desc.currentindex[_desc.dims - 1]++;
		} else if (_desc.NT == 'S') {
			short [] arow = ncsa.hdf.hdflib.HDFNativeData.byteToShort(n,_desc.dimlen[_desc.dims],_barray);
			java.lang.reflect.Array.set(_desc.objs[_desc.dims - 2] ,
				(_desc.currentindex[_desc.dims - 1]), (Object)arow);

			n += _desc.bytetoindex[_desc.dims - 1];
			_desc.currentindex[_desc.dims - 1]++;
		} else if (_desc.NT == 'B') {
			System.arraycopy( _barray, n, _desc.objs[_desc.dims - 1], 0, _desc.dimlen[_desc.dims]);
			n += _desc.bytetoindex[_desc.dims - 1];
		} else if (_desc.NT == 'F') {
			float arow[] = ncsa.hdf.hdflib.HDFNativeData.byteToFloat(n,_desc.dimlen[_desc.dims],_barray);
			java.lang.reflect.Array.set(_desc.objs[_desc.dims - 2] ,
				(_desc.currentindex[_desc.dims - 1]), (Object)arow);

			n += _desc.bytetoindex[_desc.dims - 1];
			_desc.currentindex[_desc.dims - 1]++;
		} else if (_desc.NT == 'D') {
			double [] arow = ncsa.hdf.hdflib.HDFNativeData.byteToDouble(n,_desc.dimlen[_desc.dims],_barray);
			java.lang.reflect.Array.set(_desc.objs[_desc.dims - 2] ,
				(_desc.currentindex[_desc.dims - 1]), (Object)arow);

			n += _desc.bytetoindex[_desc.dims - 1];
			_desc.currentindex[_desc.dims - 1]++;
		} else if (_desc.NT == 'L') {
			if (_desc.className.equals("java.lang.Byte")) {
				Byte I[] = ByteToByteObj(n,_desc.dimlen[_desc.dims],_barray);
		java.lang.reflect.Array.set(_desc.objs[_desc.dims - 2] ,
			(_desc.currentindex[_desc.dims - 1]), 
			(Object)I);

			n += _desc.bytetoindex[_desc.dims - 1];
			_desc.currentindex[_desc.dims - 1]++;
			} else if (_desc.className.equals("java.lang.Integer")) {
				Integer I[] = ByteToInteger(n,_desc.dimlen[_desc.dims],_barray);
		java.lang.reflect.Array.set(_desc.objs[_desc.dims - 2] ,
			(_desc.currentindex[_desc.dims - 1]), 
			(Object)I);

			n += _desc.bytetoindex[_desc.dims - 1];
			_desc.currentindex[_desc.dims - 1]++;
			} else if (_desc.className.equals("java.lang.Short")) {
				Short I[] = ByteToShort(n,_desc.dimlen[_desc.dims],_barray);
			java.lang.reflect.Array.set(_desc.objs[_desc.dims - 2] ,
				(_desc.currentindex[_desc.dims - 1]), 
				(Object)I);

			n += _desc.bytetoindex[_desc.dims - 1];
			_desc.currentindex[_desc.dims - 1]++;
			} else if (_desc.className.equals("java.lang.Float")) {
				Float I[] = ByteToFloatObj(n,_desc.dimlen[_desc.dims],_barray);
			java.lang.reflect.Array.set(_desc.objs[_desc.dims - 2] ,
				(_desc.currentindex[_desc.dims - 1]), 
				(Object)I);

			n += _desc.bytetoindex[_desc.dims - 1];
			_desc.currentindex[_desc.dims - 1]++;
			} else if (_desc.className.equals("java.lang.Double")) {
				Double I[] = ByteToDoubleObj(n,_desc.dimlen[_desc.dims],_barray);
			java.lang.reflect.Array.set(_desc.objs[_desc.dims - 2] ,
				(_desc.currentindex[_desc.dims - 1]), 
				(Object)I);

			n += _desc.bytetoindex[_desc.dims - 1];
			_desc.currentindex[_desc.dims - 1]++;
			} else if (_desc.className.equals("java.lang.Long")) {
				Long I[] = ByteToLongObj(n,_desc.dimlen[_desc.dims],_barray);
			java.lang.reflect.Array.set(_desc.objs[_desc.dims - 2] ,
				(_desc.currentindex[_desc.dims - 1]), 
				(Object)I);

			n += _desc.bytetoindex[_desc.dims - 1];
			_desc.currentindex[_desc.dims - 1]++;
			} else {
			HDFJavaException ex =
			new HDFJavaException("HDFArray: unsupported Object type: "+_desc.NT);
			throw(ex);
			}
		} else {
			HDFJavaException ex =
			new HDFJavaException("HDFArray: unsupported Object type: "+_desc.NT);
			throw(ex);
		}
		} catch (OutOfMemoryError err) {
		 HDFException ex =
			(HDFException)new HDFJavaException("HDFArray: arrayify array too big?");
			ex.printStackTrace();
			throw(ex);
		}

	}

/* assert:  the whole array is completed--currentindex should == len - 1 */

	/* error checks */

	if (n < _desc.totalSize) {
		throw new java.lang.InternalError(
	       new String("HDFArray::arrayify Panic didn't complete all input data: n=  "+n+" size = "+_desc.totalSize));
	}
	for (i = 0;i <= _desc.dims-2; i++) {
		if (_desc.currentindex[i] != _desc.dimlen[i] - 1) {
		throw new java.lang.InternalError(
			new String("HDFArray::arrayify Panic didn't complete all data: currentindex["+i+"] = "+_desc.currentindex[i]+" (should be "+(_desc.dimlen[i] - 1)+"?"));
		}
	}
	if (_desc.NT != 'B') {
	if (_desc.currentindex[_desc.dims - 1] != _desc.dimlen[_desc.dims - 1]) {
		throw new java.lang.InternalError(
		new String("HDFArray::arrayify Panic didn't complete all data: currentindex["+i+"] = "+_desc.currentindex[i]+" (should be "+(_desc.dimlen[i])+"?"));
	}
	} else {
	if (_desc.currentindex[_desc.dims - 1] != (_desc.dimlen[_desc.dims - 1] - 1)) {
		throw new java.lang.InternalError(
		new String("HDFArray::arrayify Panic didn't complete all data: currentindex["+i+"] = "+_desc.currentindex[i]+" (should be "+(_desc.dimlen[i] - 1)+"?"));
	}
	}

	return _theArray;
}

private byte[] IntegerToByte( Integer in[] ) {
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	int[] out = new int[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = in[i].intValue();
	}
	return  HDFNativeData.intToByte(0,nelems,out);
}

private Integer[] ByteToInteger( byte[] bin ) {
	int in[] = (int [])HDFNativeData.byteToInt(bin);
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	Integer[] out = new Integer[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = new Integer(in[i]);
	}
	return  out;
}
private Integer[] ByteToInteger( int start, int len, byte[] bin ) {
	int in[] = (int [])HDFNativeData.byteToInt(start,len,bin);
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	Integer[] out = new Integer[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = new Integer(in[i]);
	}
	return  out;
}


private byte[] ShortToByte( Short in[] ) {
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	short[] out = new short[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = in[i].shortValue();
	}
	return  HDFNativeData.shortToByte(0,nelems,out);
}

private Short[] ByteToShort( byte[] bin ) {
	short in[] = (short [])HDFNativeData.byteToShort(bin);
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	Short[] out = new Short[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = new Short(in[i]);
	}
	return  out;
}

private Short[] ByteToShort( int start, int len, byte[] bin ) {
	short in[] = (short [])HDFNativeData.byteToShort(start,len,bin);
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	Short[] out = new Short[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = new Short(in[i]);
	}
	return  out;
}

private byte[] ByteObjToByte( Byte in[] ) {
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	byte[] out = new byte[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = in[i].byteValue();
	}
	return out;
}

private Byte[] ByteToByteObj( byte[] bin ) {
	int nelems = java.lang.reflect.Array.getLength((Object)bin);
	Byte[] out = new Byte[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = new Byte(bin[i]);
	}
	return  out;
}

private Byte[] ByteToByteObj( int start, int len, byte[] bin ) {
	Byte[] out = new Byte[len];

	for (int i = 0; i < len; i++) {
		out[i] = new Byte(bin[i]);
	}
	return  out;
}

private byte[] FloatObjToByte( Float in[] ) {
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	float[] out = new float[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = in[i].floatValue();
	}
	return  HDFNativeData.floatToByte(0,nelems,out);
}

private Float[] ByteToFloatObj( byte[] bin ) {
	float in[] = (float [])HDFNativeData.byteToFloat(bin);
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	Float[] out = new Float[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = new Float(in[i]);
	}
	return  out;
}

private Float[] ByteToFloatObj( int start, int len, byte[] bin ) {
	float in[] = (float [])HDFNativeData.byteToFloat(start,len,bin);
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	Float[] out = new Float[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = new Float(in[i]);
	}
	return  out;
}

private byte[] DoubleObjToByte( Double in[] ) {
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	double[] out = new double[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = in[i].doubleValue();
	}
	return  HDFNativeData.doubleToByte(0,nelems,out);
}

private Double[] ByteToDoubleObj( byte[] bin ) {
	double in[] = (double [])HDFNativeData.byteToDouble(bin);
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	Double[] out = new Double[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = new Double(in[i]);
	}
	return  out;
}

private Double[] ByteToDoubleObj( int start, int len, byte[] bin ) {
	double in[] = (double [])HDFNativeData.byteToDouble(start,len,bin);
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	Double[] out = new Double[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = new Double(in[i]);
	}
	return  out;
}

private byte[] LongObjToByte( Long in[] ) {
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	long[] out = new long[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = in[i].longValue();
	}
	return  HDFNativeData.longToByte(0,nelems,out);
}

private Long[] ByteToLongObj( byte[] bin ) {
	long in[] = (long [])HDFNativeData.byteToLong(bin);
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	Long[] out = new Long[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = new Long(in[i]);
	}
	return  out;
}

private Long[] ByteToLongObj( int start, int len, byte[] bin ) {
	long in[] = (long [])HDFNativeData.byteToLong(start,len,bin);
	int nelems = java.lang.reflect.Array.getLength((Object)in);
	Long[] out = new Long[nelems];

	for (int i = 0; i < nelems; i++) {
		out[i] = new Long(in[i]);
	}
	return  out;
}

}


/**
  * This class is used by HDFArray to discover the shape and type of an
  * arbitrary array.
  */

class ArrayDescriptor {

	static String theType = "";
	static Class theClass = null;
        static int [] dimlen = null;
        static int [] dimstart = null;
        static int [] currentindex = null;
        static int [] bytetoindex = null;
	static int totalSize = 0;
        static Object [] objs = null;
	static char NT = ' ';  /*  must be B,S,I,L,F,D, else error */
        static int NTsize = 0;
	static int dims = 0;
	static String className;

	public ArrayDescriptor ( Object anArray ) throws HDFException {

		Class tc = anArray.getClass();
		if (tc.isArray() == false) {
			/* exception: not an array */
			 HDFException ex =
		(HDFException)new HDFJavaException("ArrayDescriptor: not an array?: ");
			throw(ex);
		}

		theClass = tc;

		/* parse the type descriptor to discover the
			shape of the array */
		String ss = tc.toString();
		theType = ss;
		int n = 6;
		dims = 0;
		char c = ' ';
		while (n < ss.length()) {
			c = ss.charAt(n);
			n++;
			if (c == '[') {
				dims++;
			}
		}

		String css = ss.substring(ss.lastIndexOf('[')+1);
		Class compC = tc.getComponentType();
		String cs = compC.toString();
		NT = c;
		if (NT == 'B') {
			NTsize = 1;
		} else if (NT == 'S') {
			NTsize = 2;
		} else if ((NT == 'I') || (NT == 'F')) {
			NTsize = 4;
		} else if ((NT == 'J') || (NT == 'D')){
			NTsize = 8;
		} else if (css.startsWith("Ljava.lang.Byte")) {
			NT='L';
			className = "java.lang.Byte";
			NTsize = 1;
		} else if (css.startsWith("Ljava.lang.Short")) {
			NT='L';
			className = "java.lang.Short";
			NTsize = 2;
		} else if (css.startsWith("Ljava.lang.Integer")) {
			NT='L';
			className = "java.lang.Integer";
			NTsize = 4;
		} else if (css.startsWith("Ljava.lang.Float")) {
			NT='L';
			className = "java.lang.Float";
			NTsize = 4;
		} else if (css.startsWith("Ljava.lang.Double")) {
			NT='L';
			className = "java.lang.Double";
			NTsize = 8;
		} else if (css.startsWith("Ljava.lang.Long")) {
			NT='L';
			className = "java.lang.Long";
			NTsize = 8;
		} else if (css.startsWith("Ljava.lang.String")) {
throw new HDFJavaException(new String("ArrayDesciptor: Error:  String array not supported yet"));
		} else {
			/* exception:  not a numeric type */
throw new HDFJavaException(new String("Error:  array is not numeric? (type is "+css+")"));
		}

		/* fill in the table */
		dimlen = new int [dims+1];
		dimstart = new int [dims+1];
		currentindex = new int [dims+1];
		bytetoindex = new int [dims+1];
		objs = new Object [dims+1];

		Object o = anArray;
		objs[0] = o;
		dimlen[0]= 1;
		dimstart[0] = 0;
		currentindex[0] = 0;
		int i;
		for ( i = 1; i <= dims; i++) {
			dimlen[i]= java.lang.reflect.Array.getLength((Object) o);
			o = java.lang.reflect.Array.get((Object) o,0);
			objs [i] = o;
			dimstart[i] = 0;
			currentindex[i] = 0;
		}

		int j;
		int dd;
		bytetoindex[dims] = NTsize;
		for ( i = dims; i >= 0; i--) {
			dd = NTsize;
			for (j = i; j < dims; j++) {
				dd *= dimlen[j + 1];
			}
			bytetoindex[i] = dd;
		}

		totalSize = bytetoindex[0];
	}

	public void dumpInfo()
	{
		System.out.println("Type: "+theType);
		System.out.println("Class: "+theClass);
		System.out.println("NT: "+NT+" NTsize: "+NTsize);
		System.out.println("Array has "+dims+" dimensions ("+totalSize+" bytes)");
		int i;
		for (i = 0; i <= dims; i++) {
			Class tc = objs[i].getClass();
			String ss = tc.toString();
			System.out.println(i+":  start "+dimstart[i]+": len "+dimlen[i]+" current "+currentindex[i]+" bytetoindex "+bytetoindex[i]+" object "+objs[i]+" otype "+ss);
		}
	}
}
