functiorecursivesearch, filepath
        ;new_dlm_file = '/home/scratch/lns/kauppila/temp/IDLNeXus-API/NeXusIDL-API.dlm'
        ;DLM_REGISTER, new_dlm_file

;	S = nxopen("data/nx.hdf","NXACC_READ",fileid)
;	S = nxopen("data/dmc01.h5","NXACC_READ",fileid)
;	S = nxopen("data/dmc01.hdf","NXACC_READ",fileid)
;	S = nxopen("data/dmc01.xml","NXACC_READ",fileid)
;	S = nxopen("data/dmc02.h5","NXACC_READ",fileid)
;	S = nxopen("data/dmc02.hdf","NXACC_READ",fileid)
;	S = nxopen("data/dmc02.xml","NXACC_READ",fileid)
;	S = nxopen("data/NXtest.nxs","NXACC_READ",fileid)
;	S = nxopen("data/NXtest.hdf","NXACC_READ",fileid)
;	S = nxopen("NXtest.nxs","NXACC_READ",fileid)
;	S = nxopen("C:\temp\dlm\dlm\NXtest.nxs","NXACC_READ",fileid)
;	S = nxopen("C:\temp\dlm\dlm\data\dmc01.h5","NXACC_READ",fileid)

	if(N_PARAMS() EQ 1) then begin
		S = nxopen(filepath,"NXACC_READ",fileid)

	paths = strarr(20)

	endif else begin	
		print, 'Reading default file NXtest.h5' 
		;S = nxopen("NXtest.h5","NXACC_READ",fileid)
		S = nxopen("data/dmc02.h5","NXACC_READ",fileid)

	endelse

 	print, 'nxopen status: ',S
	print, ''

	common cblock, testpaths
	common cblock2, numberofsignals

	numberofsignals = 0
	testpaths = strarr(20)

	opendir, fileid, paths

	; Read global attributes

        S = nxgetattrinfo(fileid, numberofattr)
;        print, 'nxgetattrinfo status:',S
        print, 'number of global attributes: ', numberofattr

        S= nxinitattrdir (fileid)
;        print, 'nxinitattrdir', S
;       print, ''

        for j = 1, numberofattr, 1 do begin
                S = nxgetnextattr(fileid, attrname, lenght,attr_type)
                print, 'global attribute name: ', attrname
                print, 'global attrubute lenght: ', lenght
                print, 'global attribute type: ', attr_type

                S = nxgetattr(fileid, attrname, value, lenght,attr_type)
                ;print, 'global attribute name: ', attrname
                ;print, 'global attrubute lenght: ', lenght
                ;print, 'global attribute type: ', attr_type
                print, 'global attribute value: ', value
                print, ''

        endfor

	S = nxclose(fileid)
;	print, 'nxclose status: ', S

	print, 'signal 1 found in paths: '

	print, testpaths[0]
	print, testpaths[1]

	RETURN, testpaths
end

pro opendir, fileid, paths

	common cblock
	common cblock2

	S = nxgetgroupinfo(fileid, item_number, group_name, group_class)
	;print, 'nxgetgroupinfo status', S
	print, 'current group name: ', group_name
	;print, 'current group class: ', group_class
	print, 'number of items in group', item_number
	print, ''

	for j = 1, item_number, 1 do begin

		S = nxgetnextentry(fileid, name, class, data_type)
		print, 'nxgetnextentry status: ', S
		if(S EQ 1) then begin
			print, 'nxgetnextentry name: ', name
			;print, 'nxgetnextentry class: ', class
			;print, 'nxgetnextentry data_type', data_type
				print, ''
			if (strcmp(class, 'SDS')) then begin

				print, 'opening data: ', name
				S = nxopendata(fileid, name)
				;print, 'nxopendata status:', S
				;print, ''
				if(S EQ 1) then begin 

					result = opendata(fileid)

					if(result EQ 1) then begin
						print, 'SIGNAL 1 FOUND'
						testpaths[numberofsignals] = '/' + group_name + '/'+ name
						print, 'SIGNAL 1 FOUND in : ', testpaths[1]
						numberofsignals = numberofsignals + 1
					endif

				endif
				

				S = nxclosedata(fileid)
				;print, 'nxclosedata status', S
				;print, ''

			endif else begin

				S = nxopengroup(fileid, name, class)
				;print, 'nxopengroup status', S
				;print, ''
				if(S EQ 1) then begin
					opendir, fileid, paths
				endif
	
				S = nxclosegroup(fileid)
				;print, 'nxclosegroup status', S
				;print, ''
	
			endelse
		endif
	endfor

end

function opendata, fileid

        S = nxgetdata(fileid, data)
        ;print, 'nxgetdata status: ', S
        ;print, 'data :'
        ;print,  data
	;print, ''
        S = nxgetattrinfo(fileid, numberofattr)
        ;print, 'nxgetattrinfo status:',S
        print, 'number of attributes: ', numberofattr

	;S= nxinitattrdir (fileid)
	;print, 'nxinitattrdir', S
	;print, ''

	for j = 1, numberofattr, 1 do begin
		S = nxgetnextattr(fileid, attrname, lenght,attr_type)
		if(S EQ 1) then begin
		        ;print, 'attribute name: ', attrname
		        ;print, 'attrubute lenght: ', lenght
		        ;print, 'attribute type: ', attr_type

			S = nxgetattr(fileid, attrname, value, lenght,attr_type)
		        ;print, 'attribute name: ', attrname
		        ;print, 'attrubute lenght: ', lenght
		        ;print, 'attribute type: ', attr_type
		        print, 'attribute value: ', value
			print, ''
			if((strcmp(attrname, 'signal')) AND (strcmp(string(value), '1'))) then begin
				RETURN, 1
			endif
		endif
	endfor
	RETURN, 0
end
