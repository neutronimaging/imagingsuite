!------------------------------------------------------------------------------
! NeXus - Neutron & X-ray Common Data Format
!  
! Test program for the Fortran 90 API
!
! Copyright (C) 1999-2002, Ray Osborn
!
! This library is free software; you can redistribute it and/or
! modify it under the terms of the GNU Lesser General Public
! License as published by the Free Software Foundation; either
! version 2 of the License, or (at your option) any later version.
!
! This library is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
! Lesser General Public License for more details.
!
! You should have received a copy of the GNU Lesser General Public
! License along with this library; if not, write to the Free Software
! Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
!
!  For further information, see <http://www.nexusformat.org>
!
! $Id$
!------------------------------------------------------------------------------

program NXtest

   use NXmodule

   integer :: NXrank, NXdims(NX_maxrank), NXtype, NXlen, entry_status, attr_status
   integer(kind=NXi1), dimension(4) :: i1_array = (/1, 2, 3, 4/)
   integer(kind=NXi2), dimension(4) :: i2_array = (/1000, 2000, 3000, 4000/)
   integer(kind=NXi4), dimension(4) :: i4_array = (/1000000, 2000000, 3000000, 4000000/)
   real(kind=NXr4), dimension(4,5) :: r4_array
   real(kind=NXr8), dimension(4,5) :: r8_array
   integer(kind=NXi4), dimension(4) :: i4_buffer
   real(kind=NXr4), dimension(4) :: r4_buffer
   real(kind=NXr8), dimension(16) :: r8_buffer
   integer(kind=NXi4), dimension(2000) :: comp_array
   integer(kind=NXi4) :: i, j
   real(kind=NXr4) :: r
   character(len=20) :: char_buffer
   character(len=NX_maxnamelen) :: name, class
   type(NXhandle) :: fileid
   type(NXlink) :: glink, dlink, blink

   r4_array = reshape ((/(i*1.0_NXr4,i=1,20)/),(/4,5/))
   r8_array = reshape ((/(i*1.0_NXr8,i=1,20)/),(/4,5/))
! *** create file
   if (NXopen("NXtest.nxs", NXACC_CREATE5, fileid) /= NX_OK) stop
   if (NXmakegroup(fileid, "entry", "NXentry") /= NX_OK) stop
   if (NXopengroup(fileid, "entry", "NXentry") /= NX_OK) stop
      if (NXmakedata(fileid, "ch_data", NX_CHAR, 1, (/10/)) /= NX_OK) stop
      if (NXopendata(fileid, "ch_data") /= NX_OK) stop
         if (NXputdata(fileid, "NeXus data") /= NX_OK) stop
      if (NXclosedata(fileid) /= NX_OK) stop
      if (NXmakedata(fileid, "i1_data", NX_INT8, 1, (/4/)) /= NX_OK) stop
      if (NXopendata(fileid, "i1_data") /= NX_OK) stop
         if (NXputdata(fileid, i1_array) /= NX_OK) stop
      if (NXclosedata(fileid) /= NX_OK) stop
      if (NXmakedata(fileid, "i2_data", NX_INT16, 1, (/4/)) /= NX_OK) stop
      if (NXopendata(fileid, "i2_data") /= NX_OK) stop
         if (NXputdata(fileid, i2_array) /= NX_OK) stop
      if (NXclosedata(fileid) /= NX_OK) stop
      if (NXmakedata(fileid, "i4_data", NX_INT32, 1, (/4/)) /= NX_OK) stop
      if (NXopendata(fileid, "i4_data") /= NX_OK) stop
         if (NXputdata(fileid, i4_array) /= NX_OK) stop
      if (NXclosedata(fileid) /= NX_OK) stop
      if (NXmakedata(fileid, "r4_data", NX_FLOAT32, 2, (/4,5/), NX_COMP_LZW, (/4,5/)) /= NX_OK) stop
      if (NXopendata(fileid, "r4_data") /= NX_OK) stop
         if (NXputdata(fileid, reshape(r4_array,(/size(r4_array)/))) /= NX_OK) stop
      if (NXclosedata(fileid) /= NX_OK) stop
      if (NXmakedata(fileid, "r8_data", NX_FLOAT64, 2, (/4,5/)) /= NX_OK) stop
      if (NXopendata(fileid, "r8_data") /= NX_OK) stop
         r8_buffer(1:4) = reshape(r8_array(1:4,5:5),(/4/))
         if (NXputslab(fileid, r8_buffer, (/1,5/), (/4,1/)) /= NX_OK) stop
         r8_buffer = reshape(r8_array(1:4,1:4),(/16/))
         if (NXputslab(fileid, r8_buffer, (/1,1/), (/4,4/)) /= NX_OK) stop
         if (NXputattr(fileid, "ch_attribute", "NeXus") /= NX_OK) stop
         if (NXputattr(fileid, "i4_attribute", 42) /= NX_OK) stop
         if (NXputattr(fileid, "r4_attribute", 3.141593_NXr4) /= NX_OK) stop
         if (NXgetdataID(fileid, dlink) /= NX_OK) stop
      if (NXclosedata(fileid) /= NX_OK) stop
      if (NXmakegroup(fileid, "data", "NXdata") /= NX_OK) stop
      if (NXopengroup(fileid, "data", "NXdata") /= NX_OK) stop
         if (NXmakelink(fileid, dlink) /= NX_OK) stop
         if (NXmakedata(fileid, "comp_data", NX_INT32, 2, (/20,100/)) /= NX_OK) stop
         if (NXopendata(fileid, "comp_data") /= NX_OK) stop
            comp_array = (/((j-1,i=1,20),j=1,100)/)
            if (NXputdata(fileid, comp_array) /= NX_OK) stop
         if (NXclosedata(fileid) /= NX_OK) stop
         if (NXflush(fileid) /= NX_OK) stop
         if (NXmakedata(fileid, "flush_data", NX_INT32, 1, (/NX_UNLIMITED/)) /= NX_OK) stop
            do i = 1,7
               if (NXopendata(fileid, "flush_data") /= NX_OK) stop
                  if (NXputslab(fileid, (/i/), (/i/), (/1/)) /= NX_OK) stop
                  if (NXflush(fileid) /= NX_OK) stop
            end do
      if (NXclosegroup(fileid) /= NX_OK) stop
      if (NXmakegroup(fileid, "sample", "NXsample") /= NX_OK) stop
      if (NXopengroup(fileid, "sample", "NXsample") /= NX_OK) stop
         print 300, "Writing character data"
         if (NXmakedata(fileid, "ch_data", NX_CHAR, 1, (/12/)) /= NX_OK) stop
         if (NXopendata(fileid, "ch_data") /= NX_OK) stop
            if (NXputdata(fileid, "NeXus sample") /= NX_OK) stop
         if (NXclosedata(fileid) /= NX_OK) stop
         if (NXgetgroupID(fileid, glink) /= NX_OK) stop
      if (NXclosegroup(fileid) /= NX_OK) stop
   if (NXclosegroup(fileid) /= NX_OK) stop
   if (NXmakegroup(fileid, "link", "NXentry") /= NX_OK) stop
   if (NXopengroup(fileid, "link", "NXentry") /= NX_OK) stop
      if (NXmakelink(fileid, glink) /= NX_OK) stop
   if (NXclosegroup(fileid) /= NX_OK) stop
   if (NXclose(fileid) /= NX_OK) stop
! *** read data
   if (NXopen("NXtest.nxs", NXACC_READ, fileid) /= NX_OK) stop
   if (NXgetattrinfo(fileid, i) /= NX_OK) stop
   if (i > 0) print 200, "Number of global attributes: ", i
   attr_status = NX_OK
   do while (attr_status == NX_OK)
      attr_status = NXgetnextattr(fileid, name, NXlen, NXtype)
      if (attr_status == NX_ERROR) stop
      if (attr_status == NX_OK) then
	     if ((name /= "HDF_version") .and. (name /= "HDF5_Version") .and. &
	         (name /= "file_time")) then
            select case (NXtype)
               case (NX_CHAR)
                  NXlen = len(char_buffer)
                  if (NXgetattr(fileid, name, char_buffer, NXlen, NXtype) /= NX_OK) stop
                     print 300, "   "//trim(name)//" = "//trim(char_buffer)
            end select
         end if
      end if
   end do
   if (NXopengroup(fileid, "entry", "NXentry") /= NX_OK) stop
   if (NXgetgroupinfo(fileid, i, name, class) /= NX_OK) stop
      print '(A,i8,A)', "Group: "//trim(name)//"("//trim(class)//") contains ", i, " items"
   do
      entry_status = NXgetnextentry(fileid, name, class, NXtype)
      if (entry_status == NX_ERROR) then
         stop
      else if (entry_status == NX_EOD) then
         exit
      else if (trim(class) /= "SDS") then
         print 300, "   Subgroup: "//trim(name)//"("//trim(class)//")"
      else if (entry_status == NX_OK) then
         if (NXopendata(fileid, name) /= NX_OK) stop
            if (NXgetinfo(fileid, NXrank, NXdims, NXtype) /= NX_OK) stop
            print 300, "   "//trim(name)//" : ", trim(NXdatatype(NXtype))
            if (NXtype == NX_CHAR) then
               if (NXgetdata(fileid, char_buffer) /= NX_OK) stop
               print 300, "   Values : ", trim(char_buffer)
            else if (NXtype == NX_INT8 .or. NXtype == NX_INT16 .or. NXtype == NX_INT32) then
               if (NXgetdata(fileid, i4_buffer) /= NX_OK) stop
               print 200, "   Values : ", i4_buffer
            else if (NXtype == NX_FLOAT32 .or. NXtype == NX_FLOAT64) then
               if (NXgetslab(fileid, r4_buffer, (/1,1/), (/4,1/)) /= NX_OK) stop
               print 100, "   Values : ", r4_buffer
               if (NXgetslab(fileid, r4_buffer, (/1,2/), (/4,1/)) /= NX_OK) stop
               print 100, "          : ", r4_buffer
               if (NXgetslab(fileid, r4_buffer, (/1,3/), (/4,1/)) /= NX_OK) stop
               print 100, "          : ", r4_buffer
               if (NXgetslab(fileid, r4_buffer, (/1,4/), (/4,1/)) /= NX_OK) stop
               print 100, "          : ", r4_buffer
               if (NXgetslab(fileid, r4_buffer, (/1,5/), (/4,1/)) /= NX_OK) stop
               print 100, "          : ", r4_buffer
            end if
            do
               attr_status = NXgetnextattr(fileid, name, NXdims(1), NXtype) 
               if (attr_status == NX_ERROR) then
                  stop
               else if (attr_status == NX_EOD) then
                  exit
               else if (attr_status == NX_OK) then
                  if (NXtype == NX_CHAR) then
                     if (NXgetattr(fileid, name, char_buffer) /= NX_OK) stop
                     print 300, "   "//trim(name)//" : ", trim(char_buffer)
                  else if (NXtype == NX_INT32) then
                     if (NXgetattr(fileid, name, i) /= NX_OK) stop
                     print 200, "   "//trim(name)//" : ", i
                  else if (NXtype == NX_FLOAT32) then
                     if (NXgetattr(fileid, name, r) /= NX_OK) stop
                     print 100, "   "//trim(name)//" : ", r
                  end if
               end if
            end do
         if (NXclosedata(fileid) /= NX_OK) stop
      end if
   end do
   if (NXclosegroup(fileid) /= NX_OK) stop
   if (NXopengroup(fileid, "link", "NXentry") /= NX_OK) stop
      if (NXgetgroupID(fileid, glink) /= NX_OK) stop
   if (NXclosegroup(fileid) /= NX_OK) stop
   if (NXopengroup(fileid, "link", "NXentry") /= NX_OK) stop
      if (NXgetgroupID(fileid, blink) /= NX_OK) stop
      if (NXsameID(fileid, glink, blink)) then
         print 300, "Link Check OK"
      else
         print 300, "Link Check Failed"
      end if
   if (NXclosegroup(fileid) /= NX_OK) stop
   if (NXclose(fileid) /= NX_OK) stop

 100 format(A,4f12.7)
 200 format(A,4i8)
 300 format(4A)

end program NXtest
