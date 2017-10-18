#-------------------------------------------------------------------------
# Test program for the nxinter interface. Also example usage.
#
# copyright: GPL
#
# Mark Koennecke, October 2002
#------------------------------------------------------------------------

#load ./nxinter.so

#------------- testing dataset interface
set ds [create_nxds 2 $NX_FLOAT32 3 3]

put_nxds_value $ds 1 0 0
put_nxds_value $ds 1 1 1
put_nxds_value $ds 1 2 2

puts stdout "Testing dataset interface "
puts stdout [format "rank = %d" [get_nxds_rank $ds]]
puts stdout [format "type = %d" [get_nxds_type $ds]]
puts stdout [format "dim1 = %d" [get_nxds_dim $ds 1]]

proc printDS {ds} {
for {set i 0} {$i < 3} {incr i} {
    puts stdout " "
    for {set j 0} {$j < 3} {incr j} {
	puts -nonewline stdout [format " %f" [get_nxds_value $ds $i $j]]
    }
}
puts stdout "     "
}

printDS $ds
puts stdout "Hmmmmmmmhhh....... seems OK"

#-------------- prepare a dimension dataset  
set dimds [create_nxds 1 $NX_INT32 2]
put_nxds_value $dimds 3 0
put_nxds_value $dimds 3 1

#--------- prepare slabbing slabber dimensions
set start [create_nxds 1 $NX_INT32 2]
put_nxds_value $start 0 0
put_nxds_value $start 0 1


#----------------------------------- write tests
puts stdout "Testing writing ..........."

set fd [nx_open "nxinter.hdf" $NXACC_CREATE5]
puts stdout [format "Opening file worked: %s" $fd]

#---------- write an attribute.....
set tds [create_text_nxds "Rosa Waschmaschinen sind hip"]
puts stdout [format "Writing SuperDuper = %s" [get_nxds_text $tds]]
puts stdout [format "Writing attribute results in: %d " \
	[nx_putattr $fd "SuperDuper" $tds]]
drop_nxds $tds

#----------- making groups....
set status [nx_makegroup $fd fish NXentry]
if {$status == 1} {
    puts stdout "Creating vGroup worked"
}

set status [nx_opengroup $fd fish NXentry]
if {$status == 1} {
    puts stdout "Opening vGroup worked"
}

set lnk [nx_getgroupID $fd]
puts stdout [format "groupID determined to: %s" $lnk]

#------------ writing tata
puts stdout "Test Writing data....."
puts stdout [nx_makedata $fd "fish" 2 $NX_FLOAT32 $dimds]
puts stdout [nx_opendata $fd "fish"]
puts stdout [nx_putdata $fd $ds]
set lnk [nx_getdataID $fd]
puts stdout $lnk
puts stdout [nx_closedata $fd]

#--------------- testing slabbed tata writing
puts stdout "Testing  writing in slabs"
put_nxds_value $dimds 6 0
puts stdout [format "Dimensions for slab test: %f, %f"  \
	[get_nxds_value $dimds 0] [get_nxds_value $dimds 1]]
 
puts stdout [nx_makedata $fd "fish2" 2 $NX_FLOAT32 $dimds]
puts stdout [nx_opendata $fd "fish2"]
puts stdout [nx_putslab  $fd $ds $start]
put_nxds_value $start 3 0
puts stdout [nx_putslab  $fd $ds $start]
puts stdout [nx_closedata $fd]
puts stdout "Finished Writing Slabs........."

puts stdout [format "Linking = %d" [nx_makelink $fd $lnk]]

set status [nx_closegroup $fd]
if {$status == 1} {
    puts stdout "Closing vGroup worked"
}


nx_close $fd
puts stdout "Closed file"
#---------------- finished writing tests


#---------------- trying to read
puts stdout "Testing Reading files"

set fd [nx_open "nxinter.hdf" $NXACC_READ]
puts stdout "Opening file for reading worked"

set run 1

#----------------- printing group content
puts stdout "Group directory listing"
while {$run == 1} {
    set entry [nx_getnextentry $fd / ]
    if { [string length $entry] < 2 } {
	set run 0
    } else {
	puts stdout $entry
    }
}
#--------- printing attributes
puts stdout "Attributes"
set run 1
while {$run == 1} {
    set entry [nx_getnextattr $fd / ]
    puts stdout $entry
    if { [string length $entry] < 2 } {
	set run 0
    } 
}

set rds [nx_getattr $fd "SuperDuper" $NX_CHAR 30]
puts stdout $rds
if {[string compare $rds NULL] != 0} {
    puts stdout [format "SuperDuper = %s" [get_nxds_text $rds]]
}
drop_nxds $rds

#---------------- reading tata
puts stdout [nx_opengroup $fd fish NXentry]
puts stdout [nx_opendata $fd "fish"]
set rds [nx_getdata $fd]
puts stdout $rds

puts stdout "Read data should be a 3x3 unity matrix"
printDS $rds

puts stdout [nx_closedata $fd]

#----------- reading slabbed data
puts stdout "Testing slabbed reading......"
puts stdout [nx_opendata $fd "fish2"]
set ids [nx_getinfo $fd]
puts stdout [format "fish2: type %f, rank %f, d1 %f, d2 %f" \
	[get_nxds_value $ids 0] [get_nxds_value $ids 1] \
	[get_nxds_value $ids 2] [get_nxds_value $ids 3]]
drop_nxds $ids

put_nxds_value $dimds 3 0
set r1 [nx_getslab $fd $start $dimds]
printDS $r1
put_nxds_value $start 0 0
set r2 [nx_getslab $fd $start $dimds]
printDS $r2
puts stdout [nx_closedata $fd]

puts stdout [nx_closegroup $fd]
puts stdout [nx_close $fd]

#--------------- dropping datasets: do not forget!!!
drop_nxds $ds
drop_nxds $rds
drop_nxds $dimds











