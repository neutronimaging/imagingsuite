pro opendir, fileid
	S = nxinitgroupdir (fileid)
	print, 'nxinitgroupdir', S
	print, ''



	S = nxgetgroupinfo(fileid, item_number, group_name, group_class)
	print, 'nxgetgroupinfo status', S
	print, 'current group name: ', group_name
	print, 'current group class: ', group_class
	print, 'number of items in group', item_number
	print, ''

	for j = 1, item_number, 1 do begin
		;wait, 1
		S = nxgetnextentry(fileid, name, class, data_type)
		print, 'nxgetnextentry status: ', S
		print, 'nxgetnextentry name: ', name
		print, 'nxgetnextentry class: ', class
		print, 'nxgetnextentry data_type', data_type
		print, ''
		if (strcmp(class, 'SDS')) then begin

			print, 'opening data: ', name
			S = nxopendata(fileid, name)
			print, 'nxopendata status:', S
			print, ''
			if(S EQ 1) then opendata, fileid
			S = nxclosedata(fileid)
			print, 'nxclosedata status', S
			print, ''

		endif else begin
			S = nxisexternalgroup(fileid ,name, class, nxurl, 100)
			print, 'nxisexternalgroup status: ', S
			if (S EQ 1) then begin
				print, 'Group is an externally link group'
				print, 'nxisexternalgroup url:', nxurl
				print, ''
				S = nxinquirefile(fileid,filename, 100);
				print, 'nxinquirefile status:', S
				print, 'file currently open: ', filename
			endif
			S = nxopengroup(fileid, name, class)
			print, 'nxopengroup status', S
			print, ''

			if(S EQ 1) then opendir, fileid

			S = nxclosegroup(fileid)
			print, 'nxclosegroup status', S
			print, ''

		endelse

	endfor

end

pro opendata, fileid

        S = nxgetinfo(fileid, rank, di, info_type)
        print, 'nxgetinfo status:', S
        print, 'rank:', rank
        for i = 0,rank-1 DO print, 'Dim [',i,']:', di[i]
        print, 'Data Type:', info_type
	print, ''
	S = nxgetdata(fileid, data)
        print, 'nxgetdata status: ', S
        print, 'data :'
        print,  data
	print, ''
        S = nxgetattrinfo(fileid, numberofattr)
        print, 'nxgetattrinfo status:',S
        print, 'number of attributes: ', numberofattr

	;S= nxinitattrdir (fileid)
	print, 'nxinitattrdir', S
	print, ''

	for j = 1, numberofattr, 1 do begin
		S = nxgetnextattr(fileid, attrname, lenght,attr_type)
	        print, 'attribute name: ', attrname
	        print, 'attrubute lenght: ', lenght
	        print, 'attribute type: ', attr_type

		S = nxgetattr(fileid, attrname, value, lenght,attr_type)
	        ;print, 'attribute name: ', attrname
	        ;print, 'attrubute lenght: ', lenght
	        ;print, 'attribute type: ', attr_type
	        print, 'attribute value: ', value
		print, ''

	endfor

end


pro recursiveread, filepath
        new_dlm_file = '/home/scratch/lns/kauppila/temp/IDLNeXus-API/NeXusIDL-API.dlm'
        DLM_REGISTER, new_dlm_file


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

	endif else begin	
		print, 'Reading default file NXtest.h5' 
		S = nxopen("NXtest.h5","NXACC_READ",fileid)
	endelse

 	print, 'nxopen status: ',S
	print, ''

	opendir, fileid

	; Read global attributes

        S = nxgetattrinfo(fileid, numberofattr)
        print, 'nxgetattrinfo status:',S
        print, 'number of attributes: ', numberofattr

        S= nxinitattrdir (fileid)
        print, 'nxinitattrdir', S
        print, ''

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
	print, 'nxclose status: ', S
end

