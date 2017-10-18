C------------------------------------------------------------------------------
C NeXus - Neutron & X-ray Common Data Format
C  
C Test program for NeXus FORTRAN 77 interface
C
C Copyright (C) 1997-2002, Freddie Akeroyd
C
C This library is free software; you can redistribute it and/or
C modify it under the terms of the GNU Lesser General Public
C License as published by the Free Software Foundation; either
C version 2 of the License, or (at your option) any later version.
C
C This library is distributed in the hope that it will be useful,
C but WITHOUT ANY WARRANTY; without even the implied warranty of
C MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
C Lesser General Public License for more details.
C
C You should have received a copy of the GNU Lesser General Public
C License along with this library; if not, write to the Free Software
C Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
C
C  For further information, see <http://www.nexusformat.org>
C
C $Id$
C------------------------------------------------------------------------------

      INCLUDE 'napif.inc'
      INTEGER NXRANK, NXDIMS(NX_MAXRANK), NXTYPE, NXLEN
      INTEGER ENTRY_STATUS, ATTR_STATUS, STAT
      INTEGER*4 I, J
      REAL*4 R
      INTEGER*1 I1_ARRAY(4)
      INTEGER*2 I2_ARRAY(4)
      INTEGER*4 I4_ARRAY(4) 
      REAL*4 R4_ARRAY(4,5)
      REAL*8 R8_ARRAY(4,5)
      INTEGER*4 ARRAY_DIMS(2), UNLIMITED_DIMS(1)
      INTEGER*4 CHUNK_SIZE(2)
      INTEGER*4 SLAB_START(2), SLAB_SIZE(2)
      CHARACTER*64 NAME, CLASS
      CHARACTER*128 CHAR_BUFFER
      INTEGER*1 CHAR_BUFFER_B(128)
      CHARACTER*64 GROUP_NAME, CLASS_NAME, PATH
      INTEGER FILEID(NXHANDLESIZE)
      INTEGER GLINK(NXLINKSIZE), DLINK(NXLINKSIZE), BLINK(NXLINKSIZE)
      INTEGER*4 COMP_ARRAY(20,100)
      INTEGER*4 DIMS(2), CDIMS(2), UDIMS(1)
      INTEGER*1 I1_BUFFER(4)
      INTEGER*2 I2_BUFFER(4)
      INTEGER*4 I4_BUFFER(4), U_BUFFER(7)
      REAL*4 R4_BUFFER(4)
      REAL*8 R8_BUFFER(16)
      DATA I1_ARRAY /1, 2, 3, 4/
      DATA I2_ARRAY /1000, 2000, 3000, 4000/
      DATA I4_ARRAY /1000000, 2000000, 3000000, 4000000/
      DATA R4_ARRAY /1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,
     +  15.,16.,17.,18.,19.,20./
      DATA R8_ARRAY/1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,
     +  15.,16.,17.,18.,19.,20./
      DATA ARRAY_DIMS /4, 5/
      DATA CHUNK_SIZE /4, 5/
      DATA U_BUFFER /0,1,2,3,4,5,6/
      EQUIVALENCE (CHAR_BUFFER, CHAR_BUFFER_B)

      IF (NXOPEN('napif4.nxs', NXACC_CREATE, FILEID) .NE. NX_OK) STOP
      IF (NXMAKEGROUP(FILEID, 'entry', 'NXentry') .NE. NX_OK) STOP
      IF (NXOPENGROUP(FILEID, 'entry', 'NXentry') .NE. NX_OK) STOP
         IF (NXMAKEDATA(FILEID, 'ch_data', NX_CHAR, 1, 10) .NE. NX_OK) 
     +        STOP
         IF (NXOPENDATA(FILEID, 'ch_data') .NE. NX_OK) STOP
            IF (NXPUTCHARDATA(FILEID, 'NeXus data') .NE. NX_OK) STOP
         IF (NXCLOSEDATA(FILEID) .NE. NX_OK) STOP
         IF (NXMAKEDATA(FILEID, 'i1_data', NX_INT8, 1, 4) .NE. NX_OK) 
     +        STOP
         IF (NXOPENDATA(FILEID, 'i1_data') .NE. NX_OK) STOP
            IF (NXPUTDATA(FILEID, I1_ARRAY) .NE. NX_OK) STOP
         IF (NXCLOSEDATA(FILEID) .NE. NX_OK) STOP
         IF (NXMAKEDATA(FILEID, 'i2_data', NX_INT16, 1, 4) .NE. NX_OK) 
     +        STOP
         IF (NXOPENDATA(FILEID, 'i2_data') .NE. NX_OK) STOP
            IF (NXPUTDATA(FILEID, I2_ARRAY) .NE. NX_OK) STOP
         IF (NXCLOSEDATA(FILEID) .NE. NX_OK) STOP
         IF (NXMAKEDATA(FILEID, 'i4_data', NX_INT32, 1, 4) .NE. NX_OK) 
     +        STOP
         IF (NXOPENDATA(FILEID, 'i4_data') .NE. NX_OK) STOP
            IF (NXPUTDATA(FILEID, I4_ARRAY) .NE. NX_OK) STOP
         IF (NXCLOSEDATA(FILEID) .NE. NX_OK) STOP
         IF (NXCOMPMAKEDATA(FILEID, 'r4_data', NX_FLOAT32, 2, 
     +        ARRAY_DIMS, NX_COMP_LZW, CHUNK_SIZE) .NE. NX_OK) STOP
         IF (NXOPENDATA(FILEID, 'r4_data') .NE. NX_OK) STOP
            IF (NXPUTDATA(FILEID, R4_ARRAY) .NE. NX_OK) STOP
         IF (NXCLOSEDATA(FILEID) .NE. NX_OK) STOP
         IF (NXMAKEDATA(FILEID, 'r8_data', NX_FLOAT64, 2, ARRAY_DIMS) 
     +        .NE. NX_OK) STOP
         IF (NXOPENDATA(FILEID, 'r8_data') .NE. NX_OK) STOP
            SLAB_START(1) = 1
            SLAB_START(2) = 5
            SLAB_SIZE(1) = 4
            SLAB_SIZE(2) = 1
            IF (NXPUTSLAB(FILEID, R8_ARRAY(1,5), SLAB_START, SLAB_SIZE) 
     +        .NE. NX_OK) STOP
            SLAB_START(1) = 1
            SLAB_START(2) = 1
            SLAB_SIZE(1) = 4
            SLAB_SIZE(2) = 4
            IF (NXPUTSLAB(FILEID, R8_ARRAY, SLAB_START, SLAB_SIZE) 
     +        .NE. NX_OK) STOP
            IF (NXPUTCHARATTR(FILEID, 'ch_attribute', 'NeXus',5,NX_CHAR) 
     +        .NE. NX_OK) STOP
            IF (NXPUTATTR(FILEID, 'i4_attribute', 42, 1, NX_INT32) 
     +        .NE. NX_OK) STOP
            IF (NXPUTATTR(FILEID, 'r4_attribute', 3.14159265, 1, 
     +        NX_FLOAT32) .NE. NX_OK) STOP
            IF (NXGETDATAID(FILEID, DLINK) .NE. NX_OK) STOP
         IF (NXCLOSEDATA(FILEID) .NE. NX_OK) STOP
         IF (NXMAKEGROUP(FILEID, 'data', 'NXdata') .NE. NX_OK) STOP
         IF (NXOPENGROUP(FILEID, 'data', 'NXdata') .NE. NX_OK) STOP
            IF (NXMAKELINK(FILEID, DLINK) .NE. NX_OK) STOP
            DIMS(1) = 20
            DIMS(2) = 100
            DO I = 1,100
               DO J = 1,20
                  COMP_ARRAY(J,I) = I-1
               END DO
            END DO
            CDIMS(1) = 20
            CDIMS(2) = 20
            IF (NXCOMPMAKEDATA(FILEID, 'comp_data', NX_INT32, 2, DIMS, 
     +        NX_COMP_LZW, CDIMS) .NE. NX_OK) STOP
            IF (NXOPENDATA(FILEID, 'comp_data') .NE. NX_OK) STOP
               IF (NXPUTDATA(FILEID, COMP_ARRAY) .NE. NX_OK) STOP
            IF (NXCLOSEDATA(FILEID) .NE. NX_OK) STOP
            IF (NXFLUSH(FILEID) .NE. NX_OK) STOP
            UDIMS(1) = NX_UNLIMITED
            IF (NXMAKEDATA(FILEID, 'flush_data', NX_INT32, 1, UDIMS) 
     +        .NE. NX_OK) STOP
               SLAB_SIZE(1) = 1
               DO I = 1,7
                  SLAB_START(1) = I
                  IF (NXOPENDATA(FILEID, 'flush_data') .NE. NX_OK) STOP
                     IF (NXPUTSLAB(FILEID, U_BUFFER(I), SLAB_START, 
     +                 SLAB_SIZE) .NE. NX_OK) STOP
                     STAT = NXCLOSEDATA(FILEID)
                     IF (NXFLUSH(FILEID) .NE. NX_OK) STOP
               END DO
         IF (NXCLOSEGROUP(FILEID) .NE. NX_OK) STOP
         IF (NXMAKEGROUP(FILEID, 'sample', 'NXsample') .NE. NX_OK) STOP
         IF (NXOPENGROUP(FILEID, 'sample', 'NXsample') .NE. NX_OK) STOP
            IF (NXMAKEDATA(FILEID, 'ch_data', NX_CHAR, 1, 12) .NE. 
     +        NX_OK) STOP
            IF (NXOPENDATA(FILEID, 'ch_data') .NE. NX_OK) STOP
            IF (NXPUTCHARDATA(FILEID, 'NeXus sample') .NE. NX_OK) STOP
            IF (NXCLOSEDATA(FILEID) .NE. NX_OK) STOP
            IF (NXGETGROUPID (FILEID, GLINK) .NE. NX_OK) STOP
         IF (NXCLOSEGROUP (FILEID) .NE. NX_OK) STOP
      IF (NXCLOSEGROUP (FILEID) .NE. NX_OK) STOP
      IF (NXMAKEGROUP (FILEID, "link", "NXentry") .NE. NX_OK) STOP
      IF (NXOPENGROUP (FILEID, "link", "NXentry") .NE. NX_OK) STOP
         IF (NXMAKELINK (fileid, GLINK) .NE. NX_OK) STOP
      IF (NXCLOSEGROUP(FILEID) .NE. NX_OK) STOP
      IF (NXCLOSE(FILEID) .NE. NX_OK) STOP
C *** read data
      IF (NXOPEN('napif4.nxs', NXACC_READ, FILEID) .NE. NX_OK) STOP
      IF (NXGETATTRINFO(FILEID, J) .NE. NX_OK) STOP
      IF (J .GT. 0) WRITE(*,'(1X,A,I2)') 
     +  'Number of global attributes: ', J
      DO I = 1,J
         ATTR_STATUS = NXGETNEXTATTR(FILEID,NAME,NXDIMS,NXTYPE)
         IF (ATTR_STATUS .EQ. NX_ERROR) THEN
            STOP
         ELSE IF (ATTR_STATUS .EQ. NX_OK) THEN
            NXLEN = LEN(CHAR_BUFFER)
            IF (NXGETCHARATTR(FILEID, NAME, CHAR_BUFFER, NXLEN, NXTYPE)
     +        .NE. NX_OK) STOP
	    IF ((NAME .NE. 'HDF_version') .AND. 
     +                       (NAME .NE. 'file_time')) THEN
               WRITE(*,'(4X,A)') NAME(1:LEN_TRIM(NAME))//' = '
     +           //CHAR_BUFFER(1:LEN_TRIM(CHAR_BUFFER))
	    END IF
         END IF
      END DO
      IF (NXOPENGROUP(FILEID, 'entry', 'NXentry') .NE. NX_OK) STOP
         IF (NXGETGROUPINFO(FILEID, I, GROUP_NAME, CLASS_NAME) .NE. 
     +     NX_OK) STOP
         WRITE(*,'(1X,A,I2,A)') 'Group: '
     +     //GROUP_NAME(1:LEN_TRIM(GROUP_NAME))//'('
     +     //CLASS_NAME(1:LEN_TRIM(CLASS_NAME))
     +     //') contains ',I,' items'
  100 ENTRY_STATUS=NXGETNEXTENTRY(FILEID,NAME,CLASS,NXTYPE)
      IF (ENTRY_STATUS .EQ. NX_ERROR) STOP
      IF (CLASS(1:LEN_TRIM(CLASS)) .NE. 'SDS') THEN
         IF (ENTRY_STATUS .NE. NX_EOD) THEN
            WRITE(*,'(4X,A)') 'Subgroup: '//NAME(1:LEN_TRIM(NAME))//'('
     +        //CLASS(1:LEN_TRIM(CLASS))//')'
            ENTRY_STATUS = NX_OK
         END IF
      ELSE IF (ENTRY_STATUS .EQ. NX_OK) THEN
         IF (NXOPENDATA(FILEID,NAME) .NE. NX_OK) STOP
         IF(NXGETPATH(FILEID,PATH) .NE. NX_OK) STOP
         WRITE(*,FMT='(1X,A,A)') 'Path = ', PATH(1:LEN_TRIM(PATH))
         IF (NXGETINFO(FILEID,NXRANK,NXDIMS,NXTYPE) .NE. NX_OK) STOP
         WRITE(*,FMT='(4X,A,I2,A)') NAME(1:LEN_TRIM(NAME))//'(', 
     +     NXTYPE,')'
         IF (NXTYPE .EQ. NX_CHAR) THEN
            IF (NXGETCHARDATA(FILEID,CHAR_BUFFER) .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A)') 
     +        'Values : '//CHAR_BUFFER(1:NXDIMS(1))
         ELSE IF (NXTYPE .EQ. NX_INT8) THEN
            IF (NXGETDATA(FILEID,I1_BUFFER) .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4I3)') 'Values : ', I1_BUFFER
         ELSE IF (NXTYPE .EQ. NX_INT16) THEN
            IF (NXGETDATA(FILEID,I2_BUFFER) .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4I6)') 'Values : ', I2_BUFFER
         ELSE IF (NXTYPE .EQ. NX_INT32) THEN
            IF (NXGETDATA(FILEID,I4_BUFFER) .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4I12)') 'Values : ', I4_BUFFER
         ELSE IF (NXTYPE .EQ. NX_FLOAT32) THEN
            SLAB_START(1) = 1
            SLAB_START(2) = 1
            SLAB_SIZE(1) = 4
            SLAB_SIZE(2) = 1
            IF (NXGETSLAB(FILEID, R4_BUFFER, SLAB_START, SLAB_SIZE) 
     +            .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4F7.2)') 'Values : ', R4_BUFFER
            SLAB_START(2) = 2
            IF (NXGETSLAB(FILEID, R4_BUFFER, SLAB_START, SLAB_SIZE) 
     +            .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4F7.2)') '       : ', R4_BUFFER
            SLAB_START(2) = 3
            IF (NXGETSLAB(FILEID, R4_BUFFER, SLAB_START, SLAB_SIZE) 
     +            .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4F7.2)') '       : ', R4_BUFFER
            SLAB_START(2) = 4
            IF (NXGETSLAB(FILEID, R4_BUFFER, SLAB_START, SLAB_SIZE) 
     +            .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4F7.2)') '       : ', R4_BUFFER
            SLAB_START(2) = 5
            IF (NXGETSLAB(FILEID, R4_BUFFER, SLAB_START, SLAB_SIZE) 
     +            .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4F7.2)') '       : ', R4_BUFFER
         ELSE IF (NXTYPE .EQ. NX_FLOAT64) THEN
            SLAB_START(1) = 1
            SLAB_START(2) = 1
            SLAB_SIZE(1) = 4
            SLAB_SIZE(2) = 1
            IF (NXGETSLAB(FILEID, R8_BUFFER, SLAB_START, SLAB_SIZE) 
     +            .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4F7.2)') 'Values : ', 
     +        (R8_BUFFER(I), I=1,4)
            SLAB_START(2) = 2
            IF (NXGETSLAB(FILEID, R8_BUFFER, SLAB_START, SLAB_SIZE) 
     +            .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4F7.2)') '       : ',  
     +        (R8_BUFFER(I), I=1,4)
            SLAB_START(2) = 3
            IF (NXGETSLAB(FILEID, R8_BUFFER, SLAB_START, SLAB_SIZE) 
     +            .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4F7.2)') '       : ',  
     +        (R8_BUFFER(I), I=1,4)
            SLAB_START(2) = 4
            IF (NXGETSLAB(FILEID, R8_BUFFER, SLAB_START, SLAB_SIZE) 
     +            .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4F7.2)') '       : ',  
     +        (R8_BUFFER(I), I=1,4)
            SLAB_START(2) = 5
            IF (NXGETSLAB(FILEID, R8_BUFFER, SLAB_START, SLAB_SIZE) 
     +            .NE. NX_OK) STOP
            WRITE(*,FMT='(4X,A,4F7.2)') '       : ', 
     +        (R8_BUFFER(I), I=1,4)
         END IF
  200    ATTR_STATUS = NXGETNEXTATTR (FILEID, NAME, NXDIMS, NXTYPE)
         IF (ATTR_STATUS .EQ. NX_ERROR) STOP
         IF (ATTR_STATUS .EQ. NX_OK) THEN
            IF (NXTYPE .EQ. NX_CHAR) THEN
               LENGTH=LEN(CHAR_BUFFER)
               IF (NXGETCHARATTR(FILEID,NAME,CHAR_BUFFER,LENGTH,NXTYPE) 
     +               .NE. NX_OK) STOP
               WRITE(*,FMT='(7X,A)') NAME(1:LEN_TRIM(NAME))//' : '
     +           //CHAR_BUFFER(1:LEN_TRIM(CHAR_BUFFER))
            ELSE IF (NXTYPE .EQ. NX_INT32) THEN
               LENGTH=1
               IF (NXGETATTR(FILEID,NAME,I,LENGTH,NXTYPE) 
     +               .NE. NX_OK) STOP
               WRITE(*,FMT='(7X,A,I5)') NAME(1:LEN_TRIM(NAME))//' : ',
     +           I
            ELSE IF (NXTYPE .EQ. NX_FLOAT32) THEN
               LENGTH=1
               IF (NXGETATTR(FILEID,NAME,R,LENGTH,NXTYPE) 
     +               .NE. NX_OK) STOP
               WRITE(*,FMT='(7X,A,F10.6)') NAME(1:LEN_TRIM(NAME))
     +           //' : ', R
            END IF
         END IF
         IF (ATTR_STATUS .NE. NX_EOD) GOTO 200
         IF (NXCLOSEDATA(FILEID) .NE. NX_OK) STOP
      END IF
      IF (ENTRY_STATUS .NE. NX_EOD) GOTO 100
      IF (NXCLOSEGROUP(FILEID) .NE. NX_OK) STOP
      IF (NXOPENGROUP (FILEID, "link", "NXentry") .NE. NX_OK) STOP
         IF (NXGETGROUPID (FILEID, GLINK) .NE. NX_OK) STOP
      IF (NXCLOSEGROUP (FILEID) .NE. NX_OK) STOP
      IF (NXOPENGROUP (FILEID, "link", "NXentry") .NE. NX_OK) STOP
         IF (NXGETGROUPID (FILEID, BLINK) .NE. NX_OK) STOP
         IF (NXSAMEID(FILEID, GLINK, BLINK)) THEN
            WRITE(*,*) 'Link Check OK'
         ELSE
            WRITE(*,*) 'Link Check Failed'
         ENDIF
      IF (NXCLOSEGROUP(FILEID) .NE. NX_OK) STOP
      IF (NXCLOSE(FILEID) .NE. NX_OK) STOP
      END
C----------------------------------------------------------------------
C     LEN_TRIM trims remaining blanks and tabs from the end of "string"
C      INTEGER FUNCTION LEN_TRIM (STRING)
C      INTEGER I
C      CHARACTER*(*) STRING
C      I = LEN(STRING)
C      DO WHILE (I .GE. 1 .AND. 
C     +(STRING(I:I).EQ.' '.OR. STRING(I:I).EQ.CHAR(0).OR.
C     + STRING(I:I).EQ.CHAR(9)))
C         I = I - 1
C      END DO
C      LEN_TRIM = MIN(I,LEN(STRING))
C      RETURN
C      END

