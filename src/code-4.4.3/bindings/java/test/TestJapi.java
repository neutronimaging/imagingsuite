/**
 * TestJapi does some testing of the NeXus for Java API. 
 * It can also serve as an example for the usage of the NeXus API for Java. 
 * Mark Koennecke, October 2000 updated for NAPI-2 with HDF-5 support Mark Koennecke, August 2001
 */
import java.util.Hashtable;
import java.util.Enumeration;
import java.util.Map;
import java.util.Set;
import java.util.HashSet;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.lang.reflect.Array;

import org.nexusformat.*;

public class TestJapi {
	
	void fileTest(int fileType, String fileName) throws Exception {
		NexusFile nf = null;
		NXlink gid, did;
		String group = "entry1";
		String nxclass = "NXentry";
		Set<String> excludeattr = new HashSet<String>(Arrays.asList("HDF5_Version", "file_time", "NeXus_version", "HDF_version"));
		
		// create a NexusFile
		nf = new NexusFile(fileName, fileType);

		try {
			// error handling check
			try {
				nf.opengroup(group, nxclass);
				throw new RuntimeException("Exception handling broken");
			} catch (NexusException nex) {
				System.out.println("Exception handling mechanism works");
			}
			
			int iData1[][] = new int[3][10];
			int iData2[][] = new int[3][10];
			float fData1[][] = new float[3][10];
			int islab[] = new int[10];
			int iDim[] = new int[2], i, j;
			int iStart[] = new int[2];
			int signal[] = new int[1];
			int iEnd[] = new int[2];
			String attname, attvalue, vname, vclass;
			AttributeEntry atten;
			
			// create some data
			for (i = 0; i < 3; i++) {
				for (j = 0; j < 10; j++) {
					iData1[i][j] = i * 10 + j;
					fData1[i][j] = (float) (i * 10.1 + j * .2);
				}
			}
			for (i = 0; i < 10; i++) {
				islab[i] = 10000 + i;
			}
			
			// add attributes, the first one is also an example how to write
			attvalue = "@012345abcdef";
			nf.putattr("standardstringattribute", attvalue.getBytes(), NexusFile.NX_CHAR);
			signal[0] = -177;
			nf.putattr("singlenumber", signal, NexusFile.NX_INT32);
			nf.putattr("intarray", new int[] { 1, 2, 3, 4}, new int[] {2, 2}, NexusFile.NX_INT32);
			nf.putattr("floatarray", fData1, new int[] {3, 10}, NexusFile.NX_FLOAT32);

			// closedata
			// create and open a group
			nf.makegroup(group, nxclass);
			nf.opengroup(group, nxclass);

			// get a link ID for this group
			gid = nf.getgroupID();

			// create and open a dataset
			iDim[0] = 3;
			iDim[1] = 10;
			nf.makedata("iData1", NexusFile.NX_INT32, 2, iDim);
			nf.opendata("iData1");

			// get a link ID to this data set
			did = nf.getdataID();

			// write data to it
			nf.putdata(iData1);

			// add attributes, the first one is also an example how to write
			// strings (by converting to byte arrays)
			String units = "MegaFarts";
			nf.putattr("Units", units.getBytes(), NexusFile.NX_CHAR);
			iStart[0] = 1;
			signal[0] = 1;
			nf.putattr("signal", signal, NexusFile.NX_INT32);

			// closedata
			nf.closedata();

			// try unlimimited dim
			int unDim[] = new int[1];
			unDim[0] = -1;
			nf.makedata("Stuart", NexusFile.NX_FLOAT64, 1, unDim);

			// write a compressed data set
			nf.compmakedata("iData1_compressed", NexusFile.NX_INT32, 2, iDim, NexusFile.NX_COMP_LZW, iDim);
			nf.opendata("iData1_compressed");
			nf.putdata(iData1);
			nf.closedata();

			// write a float data set
			nf.makedata("fData1", NexusFile.NX_FLOAT32, 2, iDim);
			nf.opendata("fData1");
			nf.putdata(fData1);
			nf.closedata();

			// write a dataset in slabs */
			nf.makedata("slabbed", NexusFile.NX_INT32, 2, iDim);
			nf.opendata("slabbed");
			iStart[1] = 0;
			iEnd[1] = 10;
			iEnd[0] = 1;
			for (i = 0; i < 3; i++) {
				iStart[0] = i;
				nf.putslab(islab, iStart, iEnd);
			}
			nf.closedata();

			// closegroup
			nf.closegroup();

			// test linking code
			nf.makegroup("entry2", "NXentry");
			nf.opengroup("entry2", "NXentry");
			nf.makegroup("data", "NXdata");
			nf.opengroup("data", "NXdata");
			nf.makelink(did);
			// nf.debugstop();
			nf.closegroup();

			// close file explicitly (important!)
			nf.close();

			System.out.println(" *** Writing Tests passed with flying banners");

			// **************** reading tests *******************************
			iData2[2][5] = 66666;
			fData1[2][5] = 66666.66f;

			nf = new NexusFile(fileName, NexusFile.NXACC_READ);

			// test attribute enquiry routine at global attributes
			Hashtable h = nf.attrdir();
			Enumeration e = h.keys();
			byte bData[];
			while (e.hasMoreElements()) {
				attname = (String) e.nextElement();
				atten = (AttributeEntry) h.get(attname);
				if (!excludeattr.contains(attname)) {
					StringBuilder sb = new StringBuilder();
					for(i = 0; i < atten.dim.length; i++) { 
						sb.append(" "); sb.append(atten.dim[i]);
					}
					System.out.println("Found global attribute: " + attname + " type: " + atten.type 
						+ ", dimensions:" + sb.toString() + ", length: " + atten.length);
					try {
						Object attr = nf.getattr(attname);
						sb = new StringBuilder(Array.get(attr, 0).toString());
						for(i = 1 ; i < Array.getLength(attr) ; i++) {
							sb.append(", ");
							sb.append(Array.get(attr, i).toString());
						}
						System.out.println(String.format("%s = %s", attname, sb.toString()));
					} catch (NexusException ne) {
						System.out.println(String.format("ERROR reading attribute %s (%s)", attname, ne.getMessage()));
					}
				} else {
					System.out.println("Found global attribute: " + attname + " type: " + atten.type);
					System.out.println(attname + "= XXXX (volatile information withheld to aid automatic testing)");
				}
			}

			// test reading vGroup directory
			// nf.debugstop();
			nf.opengroup(group, nxclass);
			h = nf.groupdir();
			e = h.keys();
			System.out.println("Found in vGroup entry:");
			while (e.hasMoreElements()) {
				vname = (String) e.nextElement();
				vclass = (String) h.get(vname);
				System.out.println("     Item: " + vname + " class: " + vclass);
			}

			// test reading SDS info and attributes
			nf.opendata("iData1");
			nf.getinfo(iDim, iStart);
			System.out.println("Found iData1 with: rank = " + iStart[0] + " type = " + iStart[1] + " dims = " + iDim[0]
					+ ", " + iDim[1]);
			h = nf.attrdir();
			e = h.keys();
			while (e.hasMoreElements()) {
				attname = (String) e.nextElement();
				atten = (AttributeEntry) h.get(attname);
				System.out.println("Found SDS attribute: " + attname + " type: " + atten.type + ", length: "
						+ atten.length);
			}

			// success for inquiry routines
			nf.closedata();
			nf.closegroup();
			System.out.println(" **** Inquiry routines passed test");

			// test the data reading routines
			nf.opengroup(group, nxclass);
			nf.opendata("iData1");
			nf.getdata(iData2);
			for (i = 0; i < 3; i++) {
				for (j = 0; j < 10; j++) {
					if (iData1[i][j] != iData2[i][j])
						System.out.println(" Data Reading Error at : " + i + ", " + j);
				}
			}
			// test attribute reading. This is also an example for reading
			// Strings from a NeXus file.
			byte bString[] = new byte[60];
			iDim[0] = 60;
			iDim[1] = NexusFile.NX_CHAR;
			nf.getattr("Units", bString, iDim);
			System.out.println("Read attribute Units to: " + new String(bString, 0, iDim[0]));
			// check reading a slab
			iStart[0] = 0;
			iStart[1] = 0;
			iEnd[0] = 1;
			iEnd[1] = 10;
			nf.getslab(iStart, iEnd, islab);
			for (i = 0; i < 10; i++) {
				if (islab[i] != iData1[0][i])
					System.out.println(" Slab Reading Error at : " + i + " expected: " + iData1[0][i] + ", got: "
							+ islab[i]);
			}
			nf.closedata();

			// check compressed data
			nf.opendata("iData1_compressed");
			nf.getdata(iData2);
			for (i = 0; i < 3; i++) {
				for (j = 0; j < 10; j++) {
					if (iData1[i][j] != iData2[i][j])
						System.out.println(" Data Reading Error at : " + i + ", " + j);
				}
			}
			nf.closedata();

			// now, for completeness: check float data as well
			nf.opendata("fData1");
			nf.getdata(fData1);
			nf.closedata();
			for (i = 0; i < 3; i++) {
				for (j = 0; j < 10; j++) {
					if (Math.abs(fData1[i][j] - (float) (i * 10.1 + j * .2)) > .05) {
						System.out.println(" Float Reading Error at : " + i + ", " + j);
					}
				}
			}

			// reading success
			System.out.println(" *** Data Reading routines appear to work");

			// test openpath
			nf.openpath("/entry2/data/iData1");
			nf.openpath("/entry2/data/iData1");
			nf.openpath("../");
			System.out.println("*** openpath seems to work");

		} finally {
			nf.close();
		}
	}

	public static final Map<String, Integer> backendmap = new LinkedHashMap<String, Integer>(){{
		put("HDF5", NexusFile.NXACC_CREATE5);
		put("XML", NexusFile.NXACC_CREATEXML);
		put("HDF4", NexusFile.NXACC_CREATE4);
        }};
	
	static public void main(String args[]) {
		
		TestJapi tj = new TestJapi();
		
		System.out.println("=================");
		System.out.println("Testing Java API.");
		System.out.println("=================");

		if (args.length == 0) {
			args = backendmap.keySet().toArray(new String[] {});
		}

		for(String arg: args) {
			String lower = arg.toLowerCase();
			String upper = arg.toUpperCase();
			System.out.println("");
			if (backendmap.containsKey(upper)) { 
				System.out.println("Testing "+upper);
				System.out.println("------------");
				try {
					tj.fileTest(backendmap.get(upper), "japitest."+lower);
					System.out.println("Success.");
				} catch (Exception e) {
					System.err.println("Failed with exception: "+e.getMessage());
					e.printStackTrace();
				}
			} else {
				System.out.println("Failed to parse argument. "+arg+" is not a known NeXus backend");
			}
		}
		System.out.println("");
		System.out.println("Exiting.");
	}
}
