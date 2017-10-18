pro read_test, argument
	;new_dlm_file = '/home/scratch/lns/kauppila/temp/IDLNeXus-API/NeXusIDL-API.dlm'
	;new_dlm_file =  'C:\temp\dlm\dlm\testmodule.dlm'
	;DLM_REGISTER, new_dlm_file

        filename = 'nothing'

        if (N_PARAMS() GE 1) then begin

                if(strcmp(argument, 'hdf5') EQ 1) then begin
                        filename = 'NXtest.h5'
			print, 'Opening file: ', filename
                        endif

                if(strcmp(argument, 'hdf4') EQ 1) then begin
                        filename = 'NXtest.hdf'
			print, 'Opening file: ', filename
                endif

                if(strcmp(argument, 'xml') EQ 1) then begin
                        filename = 'NXtest.xml'
			print, 'Opening file: ', filename
                endif
        endif

        if (strcmp(filename, 'nothing') EQ 1) then begin
                filename = 'NXtest.h5'
		print, 'Default, Opening file: ', filename

       endif




	nx_char = 4
	nx_float32 = 5
	nx_float64 = 6
	nx_int8 = 20
	nx_uint8 = 21
	nx_int16 = 22
	nx_Uint16 = 23
	nx_int32 = 24
	nx_Uint32 = 25

	S = nxopen(filename,"NXACC_RDWR",fileid)
	print, 'nxopen status: ',S

	S = nxinquirefile(fileid, filename, 20)
	print, 'nxinquirefile status: ', S
        if (S EQ 1) then begin
		print, 'nxinquirefile found: ',  filename
	endif

	S = nxgetattrinfo(fileid, numberofattr)
        print, 'nxgetattrinfo status:',S

	if (numberofattr GT 0) then print, 'Number of global attributes: ', numberofattr

	repeat begin
		attr_status = NXgetnextattr (fileid, attrname, lenght,attr_type)
		print, ''
		print, 'NXgetnextattr status: ', attr_status
		print, ''
		if (attr_status EQ 1) then begin
			S = nxgetattr(fileid, attrname, value, lenght,attr_type)
			print, 'attribute name: ', attrname
			;print, 'attrubute lenght: ', lenght
			;print, 'attribute type: ', attr_type
			print, format='("attribute value: ",A)', value
		endif
	endrep until (attr_status NE 1)

	S = nxopengroup(fileid,"entry","NXentry")
	print, 'nxopengroup status:',S

	S = nxgetattrinfo(fileid, numberofattr)
        print, 'nxgetattrinfo status:',S
	print, 'Number of group attributes: ', numberofattr

	repeat begin
		attr_status = NXgetnextattr (fileid, attrname, lenght, attr_type)
		print, 'NXgetnextattr status: ', attr_status
		if (attr_status EQ 1) then begin
			S = nxgetattr(fileid, attrname, value, lenght,attr_type)
			print, 'nxgetattr status', S
			print, 'attribute name: ', attrname
			print, 'attrubute lenght: ', lenght
			print, 'attribute type: ', attr_type
			print, 'attribute value: ', value
			print, ''
		endif
	endrep until (attr_status NE 1)

         S = nxgetgroupinfo(fileid, item_number, group_name, group_class)
       print, 'nxgetgroupinfo status', S

		print, 'current group name: ', group_name
	        print, 'current group class: ', group_class
	        print, 'number of items in group: ', item_number
	        print, ''

	repeat begin
		entry_status = nxgetnextentry(fileid, name, class, data_type)
		print, 'nxgetnextentry status: ', entry_status
		if(entry_status NE -1) then begin
			print, 'nxgetnextentry name: ', name
			print, 'nxgetnextentry class: ', class
			print, 'nxgetnextentry data_type', data_type;			
			print, ''
		end

		if(strcmp(class, 'SDS')) then begin
			print, 'opening data: ', name
			S = nxopendata(fileid, name)
			print, 'nxopendata status: ',S
			print, ''

			S = nxgetinfo(fileid, rank, di, datatype)
			print, 'nxgetinfo Status:', S
			print, 'Rank:', rank
			for i = 0,rank-1 DO print, 'Dim [',i,']:', di[i]
			print, 'Data Type:', datatype
			print, ''

			if(datatype EQ nx_char) then begin
				S = nxgetdata(fileid, data)
				print, 'nxget data status: ', S
				print, 'data :'
				print,  data;				
				print, ''
			endif else begin;				
				slabstart = [0L,0L]
				slabsize = [1L,4L]

				S = nxgetslab(fileid, data, slabstart, slabsize)
				print, 'nxgetslab status: ', S
			        print, 'data :'
				print, data
				print, ''

				slabstart[0] = 1
				S = nxgetslab(fileid, data, slabstart, slabsize)
				print, 'nxgetslab status: ', S
			        print, 'data :'
				print, data
				print, ''

				slabstart[0] = 2
				S = nxgetslab(fileid, data, slabstart, slabsize)
				print, 'nxgetslab status: ', S
			        print, 'data :'
				print, data
				print, ''

				slabstart[0] = 3
				S = nxgetslab(fileid, data, slabstart, slabsize)
				print, 'nxgetslab status: ', S
			        print, 'data :'
				print, data
				print, ''
			endelse

			S = nxgetattrinfo(fileid, numberofattr)
		        print, 'nxgetattrinfo status:',S
			print, 'Number of group attributes: ', numberofattr
			if (numberofattr GT 0) then print, 'Number of global attributes: ', numberofattr

			repeat begin
				attr_status = NXgetnextattr (fileid, attrname, lenght, attr_type)
				print, 'nxgetnextattr status: ', attr_status
				if (attr_status EQ 1) then begin
					S = nxgetattr(fileid, attrname, value, lenght,attr_type)
					print, 'nxgetattr status', S
					print, 'attribute name: ', attrname
					print, 'attribute value: ', value
					print, ''
				endif
			endrep until (attr_status NE 1)

			S = nxclosedata(fileid)
			print, "nxclosedata status: ", S
		endif

	endrep until (entry_status NE 1)
	S = nxclosegroup(fileid)
	print, 'nxclosegroup status: ', S

	print, ''
	print, '#######################  Checking Links ###########################'
	print, ''

	S = nxopengroup(fileid,"entry","NXentry")
	print, 'nxopengroup status:',S
	print, ''

	S = nxopengroup(fileid,"sample","NXsample")
	print, 'nxopengroup status:',S
	print, ''

		S = nxgetgroupid(fileid, groupid1)
		print, 'nxgetgroupid status: ', S
		print, 'groupid handle: ', groupid1
		print, ''

	S = nxclosegroup(fileid)
	print, 'nxclosegroup status: ', S
	print, ''

	S = nxopengroup(fileid,"data","NXdata")
	print, 'nxopengroup status:',S
	print, ''

		S = nxopendata(fileid, 'r8_data')
		print, 'nxopendata status: ',S
		print, ''

			S = nxgetdataid(fileid, dataid1)
			print, 'nxgetdataid status: ', S
			;print, 'dataid handle: ', dataid

		S = nxclosedata(fileid)
		print, 'nxclosedata status: ', S

	S = nxclosegroup(fileid)
	print, 'nxclosegroup status: ', S
	print, ''

	S = nxopendata(fileid, 'r8_data')
	print, 'nxopendata status: ',S
	print, ''

		S = nxgetdataid(fileid, dataid2)
		print, 'nxgetdataid status: ', S
		;print, 'dataid handle: ', dataid

	S = nxclosedata(fileid)
	print, 'nxclosedata status: ', S

	print, 'debug'
	S = nxsameid(fileid, dataid1, dataid2)
        print, 'nxsamid status: ', S
	if(S EQ 1) then begin
	print, 'Data id: Link check succesfull'
	endif else begin
		print, 'Data id: Link check FAILED (sample)'
	endelse

	S = nxclosegroup(fileid)
	print, 'nxclosegroup status: ', S
	print, ''

	S = nxopengroup(fileid,'link','NXentry')
	print, 'nxopengroup status:',S
	print, ''

	S = nxopengroup(fileid,"sample","NXsample")
	print, 'nxopengroup status:',S
	print, ''

		S = nxgetgroupid(fileid, groupid2)
		print, 'nxgetgroupid status: ', S
		;print, 'groupid handle: ', groupid2
		print, ''

	S = nxsameid(fileid, groupid1, groupid2)
	if(S EQ 1) then begin
	print, 'Group Id: Link check succesfull'
	endif else begin
		print, 'Group Id: Link check FAILED (sample)';	
	endelse

	S = nxclosegroup(fileid)
	print, 'nxclosegroup status: ', S
	print, ''

	S = nxopengroup(fileid,"renLinkGroup","NXsample")
	print, 'nxopengroup status:',S
	print, ''

		S = nxgetgroupid(fileid, groupid_named)
		print, 'nxgetgroupid status: ', S
		;print, 'groupid handle: ', groupid2
		print, ''

		S = nxsameid(fileid, groupid1, groupid_named)
		if(S EQ 1) then begin
		print, 'Group Id Named: Link check succesfull'
		endif else begin
		print, 'Group Id Named: Link check FAILED (sample)';
		endelse


	print, ''
	S = nxclosegroup(fileid)
	print, 'nxclosegroup status: ', S
	print, ''

	S = nxopendata(fileid, 'renLinkData')
	print, 'nxopendata status: ',S
	print, ''

		S = nxgetdataid(fileid, dataid_named)
		print, 'nxgetdataid status: ', S
		;print, 'dataid handle: ', dataid


		S = nxsameid(fileid, dataid1, dataid_named)
		if(S EQ 1) then begin
		print, 'Data Id Named: Link check succesfull'
		endif else begin
		print, 'Data Id Named: Link check FAILED (sample)'
		endelse

	S = nxclosedata(fileid)
	print, 'nxclosedata status: ', S

	;nxopenpath tests

	S = nxopenpath(fileid, '/entry/data/comp_data')
	if(S EQ 1) then begin
	print, 'Openpath succesfull (/entry/data/comp_data)'
	endif else begin
		print, 'Openpath FAILED (/entry/data/comp_data)'
	endelse

	S = nxopenpath(fileid, '../r8_data')
	if(S EQ 1) then begin
	print, 'Openpath on a relative path  (../r8_data) succesfull'
	endif else begin
		print, 'Openpath on a relative path FAILED (../r8_data)'
        endelse

	S = nxclose(fileid)
	print, 'nxclose status: ',S

end
