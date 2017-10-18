!------------------------------------------------------------------------------
! NeXus - Neutron & X-ray Common Data Format
!  
! Fortran 90 utility to list the contents of a NeXus file
!
! Copyright (C) 1999, Ray Osborn
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
! Contact : R. Osborn <ROsborn@anl.gov>
!           Materials Science Division
!           Argonne National Laboratory
!           Argonne, IL 60439-4845
!           USA
!
!  For further information, see <http://www.nexusformat.org>
!
!$Id$
!------------------------------------------------------------------------------

program NXdump

   use NXmodule
   character(len=50) :: file_name
   type(NXhandle) :: file_id
   integer :: indent

   write (unit=*, fmt="(a)", advance="no") " Give name of NeXus file : "
   read *, file_name
!Open input file
   if (NXopen (trim(file_name), NXACC_READ, file_id) /= NX_OK) stop
!Cycle through the groups
   indent = 1
   call getdir
   if (NXclose (file_id) /= NX_OK) stop

contains
   recursive subroutine getdir 

      integer :: status, type, rank, dimensions(NX_MAXRANK)      
      character(len=100) :: name, class

      indent = indent + 3
      do
         status = NXgetnextentry (file_id, name, class, type)
         if (status == NX_OK) then
            if (class(1:2) == "NX") then
               if (NXopengroup (file_id, name, class) /= NX_OK) stop
               print *, repeat(" ",indent)//"Group """//trim(name)//""" ("//trim(class)//") opened"
               call getdir
            else if (class(1:3) == "SDS") then
               print *, repeat(" ",indent)//"Next entry is """//trim(name)//""""
               if (NXopendata (file_id, name) /= NX_OK) stop
               if (NXgetinfo (file_id, rank, dimensions, type) /= NX_OK) stop
               print *, repeat(" ",indent)//"  Rank = ", rank, "Type = ", NXdatatype(type)
               print *, repeat(" ",indent)//"  Dimensions = ", dimensions(1:rank)
               if (NXclosedata (file_id) /= NX_OK) stop
            endif
         else if (status == NX_EOD) then
            if (NXclosegroup (file_id) /= NX_OK) stop
            indent = indent - 3
            print *, repeat(" ",indent)//"Group closed"
            exit
         else
            stop
         end if
      end do
   end subroutine getdir

end program NXdump
