#--------------------------------------------------------------------------
proc printDataset {ds} {
    for {set i 0} {$i < [get-nxds-dim ds 0]} {incr i} {
	puts stdout " "
	for {set j 0} {$j < [get-nxds-dim ds 1]} {incr j} {
	    puts -nonewline stdout [format " %f" [get_nxds_value $ds $i $j]]
	}
    }
    puts stdout "     "
}
#------------------------------------------------------------------------
proc printAttributes {fd} {
    set run 1
    while {$run == 1} {
	set entry [nx_getnextattr $fd / ]
	if { [string length $entry] < 2 } {
	    set run 0
	    continue
	} 
	set list [split $entry /]
	set atData [nx_getattr fd [lindex $list 0] \
		[lindex $list 2] [lindex $list 1]]
	puts stdout [format "%s = %s " [lindex $list 0] \
		[get_nxds_text $atData]]
	drop_nxds $atData
    }
}
#----------------------------------------------------------------------
set fd [nx_open "nxinter.hdf" $NXACC_READ]
printAttributes $fd
nx_opengroup $fd "fish" "NXentry"
nx_opendata $fd "fish"
set tata [nx_getdata $fd]
printDataset $tata
drop_nxds $tata
nx_closedata $fd
nx_closegroup $fd
nx_close $fd

