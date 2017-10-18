	pro build_testmodule, VERBOSE=verbose

  ; Locate the testmodule files from the IDL distribution
  ;
  subdirs = [ 'external', 'dlm' ]
  testmodule_dir = FILEPATH('', SUBDIRECTORY=subdirs)
  dlm_file = FILEPATH('testmodule.dlm_base', SUBDIRECTORY=subdirs)
  new_dlm_file = $
	FILEPATH('testmodule.dlm', ROOT_DIR=!make_dll.compile_directory)

  ; Build the testmodule DLM in the !MAKE_DLL.COMPILE_DIRECTORY directory.
  ;
  ; Normally, you wouldn't use VERBOSE, or SHOW_ALL_OUTPUT once your
  ; work is debugged, but as a learning exercise it can be useful to
  ; see all the underlying work that gets done. If the user specified
  ; VERBOSE, then use those keywords to show what MAKE_DLL is doing.
  ;
  ;MAKE_DLL, 'testmodule','IDL_Load', INPUT_DIR=testmodule_dir, $
  ;	VERBOSE=verbose, SHOW_ALL_OUTPUT=verbose

inputfiles = [ 'NeXusIDL-API','handle']
inputdir = '/home/scratch/lns/kauppila/temp/IDLNeXus-API'

cd,'.',current=dir

MAKE_DLL, inputfiles, 'IDL_Load', compile_directory=dir, INPUT_DIRECTORY=dir, output_dir=dir, EXTRA_CFLAGS='../../src/trunk/src/.libs/libNeXus.a -L$(HDFROOT)/lib -lhdf5 -lmfhdf -ldf -lmxml -lsz -lz -lm -ljpeg -lm '

  ; Copy the DLM file into the directory with the sharable library
  FILE_COPY, dlm_file, new_dlm_file, /OVERWRITE


  ; NOTE TO IDL USERS: This call to DLM_REGISTER is not the recommended
  ; way to make DLMs known to your IDL session. The best, and usually
  ; the only recommended, way to do this is to put the DLM files into
  ; one of the directories searched by IDL at startup (and contained in
  ; the !DLM_PATH system variable). Not only is is easier, but IDL will
  ; know the routines from your DLM before it compiles a single line of
  ; PRO code, avoiding one of the most common LINKIMAGE pitfalls.
  ; DLMs are largely intended to provide a safer easier interface to
  ; the capabilities of LINKIMAGE, so this is an important point.
  ;
  ; We do this here in order to simplify the demo. Otherwise, it would be
  ; necessary to alter the !DLM_PATH, and restart IDL, none of which is
  ; really central to our demo.
  ;
  ;DLM_REGISTER, new_dlm_file

  ; Display the DLM so the user can see that it is there
  ;
  ;print,''
  ;HELP,/DLM, 'testmodule'

  ; Run the 2 routines. Note that I have to use EXECUTE so that IDL
  ; will see these as system routines within this BUILD_TESTMODULE
  ; routine. This is because BUILD_TESTMODULE was compiled by IDL
  ; before it knew about the TESTMODULE DLM, and as such, plain calls
  ; get turned into IDL user routine calls. Since EXECUTE compiles at
  ; runtime, it will see them as system routines now that DLM_REGISTER
  ; has been called.
  ;
  ; This is the LINKIMAGE pitfall mentioned above, and a very good
  ; reason not to make a habit of using DLM_REGISTER.
  ;
  ;void = execute('testpro')
  ;void = execute('print, testfun()')

end
