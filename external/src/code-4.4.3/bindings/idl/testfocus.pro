DLM_REGISTER, 'NeXusIDL-API.dlm'

s = nxopen('data/focus2007n001335.hdf',"NXACC_READ",fileid)
s = nxopenpath(fileid,'/entry1/merged/counts')
s = nxgetdata(fileid,data)
;s = nxclose(fileid)
loadct, 0
tvscl, data
