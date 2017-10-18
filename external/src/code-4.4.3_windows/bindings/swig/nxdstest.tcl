#-------------------------------------------------------------------------
# NX_Dataset testing code for Tcl
#
# copyright: GPL
#
# Mark Koennecke, October 2002
#-------------------------------------------------------------------------

set ds [create_nxds 2 $NX_FLOAT32 3 3]

put_nxds_value $ds 1 0 0
put_nxds_value $ds 1 1 1
put_nxds_value $ds 1 2 2

puts stdout [format "rank = %d" [get_nxds_rank $ds]]
puts stdout [format "type = %d" [get_nxds_type $ds]]
puts stdout [format "dim1 = %d" [get_nxds_dim $ds 1]]

for {set i 0} {$i < 3} {incr i} {
    puts stdout " "
    for {set j 0} {$j < 3} {incr j} {
	puts -nonewline stdout [format " %f" [get_nxds_value $ds $i $j]]
    }
}
puts stdout "     "
puts stdout "Hmmmmmmmhhh......."

 drop_nxds $ds
