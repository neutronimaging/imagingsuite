pro write_test, argument

	new_dlm_file = 'NeXusIDL-API.dlm'
	DLM_REGISTER, new_dlm_file

	print, 'NeXus IDL Api Write test'
	access_method = 'nothing'

	if (N_PARAMS() GE 1) then begin

		if(strcmp(argument, 'hdf5') EQ 1) then begin
			print, 'Using HDF5'
			access_method = 'NXACC_CREATE5'
			filename = 'NXtest.h5'
			extfile = 'nxext.h5'
			link_external_path = 'nxfile://data/dmc01.h5#/entry1'
			endif

		if(strcmp(argument, 'hdf4') EQ 1) then begin
			print, 'Using HDF4'
			access_method = 'NXACC_CREATE'
			filename = 'NXtest.hdf'
			extfile = 'nxext.hdf'
			link_external_path = 'nxfile://data/dmc01.hdf#/entry1'
		endif

		if(strcmp(argument, 'xml') EQ 1) then begin
			access_method = 'NXACC_CREATEXML'
			print, 'Using XML'
			filename = 'NXtest.xml'
			extfile = 'nxext.xml'
			link_external_path = 'nxfile://data/dmc01.xml#/entry1'
		endif
	endif

	if (strcmp(access_method, 'nothing') EQ 1) then begin
		print, 'Using Default Access Method: HDF5'
		access_method = 'NXACC_CREATE5'
		filename = 'NXtest.h5'
		extfile = 'nxext.h5'
		link_external_path = 'nxfile://data/dmc01.h5#/entry1'
	endif

	; Windows (set correct path)
	;link_external_path = 'nxfile://C:\temp\dlm\dlm\data\dmc01.hdf#/entry1'

	; Linux


	; nxdatatypes:
	nx_int16 = 22
	nx_int32 = 24
	nx_float32 = 5
	nx_float64 = 6
	nx_uint16 = 22

	;access_method = 'NXACC_CREATE5'
	;access_method = 'NXACC_CREATE'

	; Only type LONG arrays are accepted in dimensions and chunck size.
	array_dims =[4L,4L]
	chunk_size =[4L,4L]

	rank = 2

	nx_uint8_array = BytArr(4, 4)
	v1 = [0B,1B,2B,4B]
	v2 = [4B,5B,6B,7B]
	v3 = [8B,9B,10B,11B]
	v4 = [12B,13B,14B,15B]
	nx_uint8_array =[[v1],[v2],[v3],[v4]]

	nx_int16_array = IntArr(4, 4)
	v1 = [0S,1S,2S,4S]
	v2 = [4S,5S,6S,7S]
	v3 = [-8S,-9S,-10S,-11S]
	v4 = [-12S,-13S,-14S,-15S]
	nx_int16_array =[[v1],[v2],[v3],[v4]]

	nx_int16_array = UIntArr(4, 4)
	v1 = [0U,1U,2U,4U]
	v2 = [4U,5U,6U,7U]
	v3 = [-8U,-9U,-10U,-11U]
	v4 = [-12U,-13U,-14U,-15U]
	nx_int16_array =[[v1],[v2],[v3],[v4]]

	nx_int32_array = LonArr(4, 4)
	v1 = [0L,1L,2L,3L]
	v2 = [4L,5L,6L,7L]
	v3 = [8L,9L,10L,11L]
	v4 = [12L,13L,14L,15L]
	nx_int32_array =[[v1],[v2],[v3],[v4]]

	nx_uint32_array = ULonArr(4, 4)
	v1 = [0UL,1UL,2UL,3UL]
	v2 = [4UL,5UL,6UL,7UL]
	v3 = [-8UL,-9UL,-10UL,-11UL]
	v4 = [-12UL,-13UL,-14UL,-15UL]
	nx_uint32_array =[[v1],[v2],[v3],[v4]]

	nx_float32_array = FltArr(4, 4)
	v1 = [0.0111112,0.02122222,0.233333333,0.34444444]
	v2 = [0.3443333,0.55555554,0.666666667,0.77777333]
	v3 = [0.6666689,0.99999977,10.10000023,11.2222209]
	v4 = [-12.20002,-13.444442,-14.2222223,-15.444444]
	nx_float32_array =[[v1],[v2],[v3],[v4]]

	nx_float64_array = ULonArr(4, 4)
	v1 = [0.0111112D,0.02122222D,0.233333333D,0.34444444D]
	v2 = [0.3443333D,0.55555554D,0.666666667D,0.77777333D]
	v3 = [0.6666689D,0.99999977D,10.10000023D,11.2222209D]
	v4 = [-12.20002D,-13.444442D,-14.2222223D,-15.444444D]
	nx_float64_array =[[v1],[v2],[v3],[v4]]

        S = nxopen(filename, access_method, fileid)
	print, 'nxopen status: ', S
	S = nxsetnumberformat(fileid,"NX_FLOAT32",'%9.3f')
	print, 'nxsetnumberformat status: ', S

        S = nxmakegroup(fileid, "entry", "NXentry")
	print, 'nxmakegroup status: ', S
        S = nxopengroup(fileid, "entry", "NXentry")
	print, 'nxopengroup status: ', S
;        S = nxmakegroup(fileid, "anothergroup", "NXentry")
;	print, 'nxmakegroup status: ', S
	S = nxputattr(fileid, "hugo", "namenlos",strlen('namenlos'), 'NX_CHAR')
	print, 'nxputattr status: ', S
	S = nxputattr(fileid, "cucumber", "passion",strlen('passion'), 'NX_CHAR')
	print, 'nxputattr status: ', S

	; needs to be an array?
	nxlen = [10L]

	S = nxmakedata(fileid, 'ch_data','NX_CHAR', 1, nxlen)
        print, 'nxmakedata status: ', S
        S = nxopendata(fileid, "ch_data")
        print, "opendata status: ", S
	S = nxputdata(fileid, 'NeXus Data')
	print, 'nxputdata status:', S
	S = nxclosedata(fileid)
	print, "nxclosedata status: ", S

	S = nxmakedata(fileid, 'i1_data','NX_UINT8', 2, array_dims)
        print, 'nxmakedata status: ', S
	S = nxopendata(fileid, "i1_data")
        print, "opendata status: ", S
	S = nxputdata(fileid, nx_uint8_array)
	print, 'nxputdata status:', S
	S = nxclosedata(fileid)
	print, "nxclosedata status: ", S

	S = nxmakedata(fileid, 'i4_data','NX_INT32', 2, array_dims)
        print, 'nxmakedata status: ', S
	S = nxopendata(fileid, "i4_data")
        print, "opendata status: ", S
	S = nxputdata(fileid, nx_int32_array)
	print, 'nxputdata status:', S
	S = nxclosedata(fileid)
	print, "nxclosedata status: ", S

	S = nxcompmakedata(fileid, 'r4_data','NX_FLOAT32', 2, array_dims, 'NX_COMP_LZW', chunk_size)
        print, 'nxmakedata status: ', S
	S = nxopendata(fileid, "r4_data")
        print, "opendata status: ", S
	S = nxputdata(fileid, nx_float32_array)
	print, 'nxputdata status:', S
	S = nxclosedata(fileid)
	print, "nxclosedata status: ", S



	S = nxmakedata(fileid, 'r8_data','NX_FLOAT64', 2, array_dims)
        print, 'nxmakedata status: ', S
	S = nxopendata(fileid, "r8_data")
        print, "opendata status: ", S

		slab_start = [2L,0L]
		slab_size = [1L,4L]
		S = nxputslab(fileid, nx_float64_array, slab_start, slab_size)
		print, 'nxputslab status:', S

		S = nxputattr(fileid,'ch_attribute', 'NeXus', strlen ('NeXus'), 'NX_CHAR')
		print, 'nxputattr status : ',S

		i = 42L
		S = nxputattr(fileid,'i4_attribute', i, 1, 'NX_INT32')
		print, 'nxputattr status : ',S

		r = 3.14159265
		S = nxputattr(fileid,'r4_attribute', r, 1, 'NX_FLOAT32')
		print, 'nxputattr status : ',S
		S = nxgetdataid(fileid, dataid)
		print, 'nxgetdataid status: ', S
		;print, 'dataid handle: ', dataid


	S = nxclosedata(fileid)
	print, "nxclosedata status: ", S

	S = nxmakegroup(fileid, 'data', 'NXdata')
	print, 'nxmakegroup status: ', S

        S = nxopengroup(fileid, 'data', 'NXdata')
	print, 'nxopengroup status: ', S

	S = nxmakelink(fileid, dataid)
	print, 'nxmakelink status: ', S

	dims = [100L,20L]
	; indexed array of longs
	comp_array = lindgen(100,20)
        cdims = [20L,20L]
	rank = 2

	S = nxcompmakedata(fileid, 'comp_data', 'NX_INT32', rank, dims, 'NX_COMP_LZW', cdims)
	print, 'nxmakecompdata status:', S

	S = nxopendata(fileid, "comp_data")
        print, 'opendata status: ', S

		S = nxputdata(fileid, comp_array)
		print, 'nxputdata status:', S

	S = nxclosedata(fileid)
	print, 'nxclosedata status: ', S

	S = nxflush(fileid)
	print, 'nxflush status: ', S

	S = nxmakedata(fileid, 'flush_data','NX_INT32', 1, 'NX_UNLIMITED')
        print, 'nxmakedata status: ', S

	data = [0L]
	slab_size = [1L]
	for i = 1L, 7L, 1L do begin
		slab_start[0] = i

		; Maybe the putslab/putdata should accept single variables also,
		; easy to implement..

		data[0] = long(i)
		S = nxopendata(fileid, "flush_data")
	        print, 'opendata status: ', S

		S = nxputslab(fileid, data, slab_start, slab_size)
		print, 'nxputslab ', i,' status:', S
		S = nxflush(fileid)
		print, 'nxflush status: ', S
	endfor

	S = nxclosegroup(fileid)
	print, 'nxclosegroup status: ', S

	S = nxmakegroup(fileid, 'sample', 'NXsample')
	print, 'nxmakegroup status: ', S

        S = nxopengroup(fileid, 'sample', 'NXsample')
	print, 'nxopengroup status: ', S
	nxlen = [12L]
	S = nxmakedata(fileid, 'ch_data', 'NX_CHAR', 1, nxlen)
        print, 'nxmakedata status: ', S

	S = nxopendata(fileid, "ch_data")
	print, 'opendata status: ', S

		S = nxputdata(fileid, 'NeXus sample')
		print, 'nxputdata status:', S

	S = nxclosedata(fileid)
	print, 'nxclosedata status: ', S

	S = nxgetgroupid(fileid, groupid)
	print, 'nxgetgroupid status: ', S
	print, 'groupid handle: ', groupid

	S = nxclosegroup(fileid)
	print, 'nxclosegroup status: ', S

	S = nxclosegroup(fileid)
	print, 'nxclosegroup status: ', S

	S = nxmakegroup(fileid, 'link', 'NXentry')
	print, 'nxmakegroup status: ', S

	S = nxopengroup(fileid, 'link', 'NXentry')
	print, 'nxopengroup status: ', S

	S = nxmakelink(fileid, groupid)
	print, 'nxmakelink status: ', S

	print, 'groupid handle', groupid
	S = nxmakenamedlink(fileid, 'renLinkGroup', groupid)
	print, 'nxmakenamedlink status : ', S

	print, 'dataid handle', dataid

	S = nxmakenamedlink(fileid, 'renLinkData', dataid)
	print, 'nxmakenamedlink status : ', S

	S = nxclosegroup(fileid)
	print, 'nxclosegroup status: ', S

	S = nxclose(fileid)
	print, 'nxclose status: ', S


	access_method = 'NXACC_CREATE'

	print, ''
	print, ' ####################  Test external linking  ######################'
	print, ''


	; Create the test file
	S = nxopen(extfile, access_method, fileid)
	print, 'nxopen status: ', S

	S = nxmakegroup(fileid, 'entry1', 'NXentry')
	print, 'nxmakegroup status: ', S

	S = nxlinkexternal(fileid, 'entry1','NXentry',link_external_path)
 	print, 'nxlinkexternal: ', S

	S = nxmakegroup(fileid, 'entry2', 'NXentry')
	print, 'nxmakegroup status: ', S

	S = nxlinkexternal(fileid, 'entry2','NXentry',link_external_path)
	print, 'nxlinkexternal: ', S

	S = nxclose(fileid)
	print, 'nxclose status: ', S

	; Actual link testing begins

	access_method = 'NXACC_RDWR'
	S = nxopen(extfile, access_method, fileid)
	print, 'nxopen status: ', S
	S = nxopenpath(fileid, '/entry1/start_time')
	print, 'nxopenpath status', S
	print, ''

	S = nxgetdata(fileid, data)
		print, 'nxget data status: ', S
		print, 'First file time :'
		print,  data
		print, ''

	S = nxinquirefile(fileid, filename, 256)
	print, 'nxinquirefile status: ', S
	print, 'current file: ', filename

	S = nxopenpath(fileid, '/entry2/sample/sample_name')
	print, 'nxopenpath status', S
	S = nxgetdata(fileid, data)
		print, 'nxget data status: ', S
		print, 'Second file sample :'
		print,  data
		print, ''

	S = nxinquirefile(fileid, filename, 256)
	print, 'nxinquirefile status: ', S
	print, 'current file: ', filename

	S = nxopenpath(fileid, '/entry2/start_time')
	print, 'nxopenpath status', S
	S = nxgetdata(fileid, data)
		print, 'nxget data status: ', S
		print, 'Second file time :'
		print,  data
		print, ''

	S = nxopenpath(fileid, '/')
	S = nxisexternalgroup(fileid, 'entry1', 'NXentry', filename, 255)
	print, 'nxisexternalgroup status', S
	print, 'entry1 external URL =', filename

	S = nxclose(fileid)
	print, 'nxclose status: ', S
end
