#
# 
# ***************************************************************************
# *                                                                         *
# *                     Fast Wavelet Transform Library                      *
# *                                                                         *
# * Release Version 0.0.1                                                   *
# * Copyright (c) 2007-2008 Yuriy Chesnokov                                 *
# * Copyright (c) 2008 Arash Partow                                         *
# * All Rights Reserved.                                                    *
# *                                                                         *
# * The Fast Wavelet Transform Library and all its components are supplied  *
# * under the terms of the General Public License. The contents of the      *
# * Fast Wavelet Transform Library and all its components may not be copied *
# * or disclosed except in accordance with the terms of that agreement.     *
# *                                                                         *
# * URL: http://www.gnu.org/licenses/gpl.txt                                *
# *                                                                         *
# ***************************************************************************
#


COMPILER         = -c++
OPTIMIZATION_OPT = -O3
OPTIONS          = -fopenmp -pedantic -ansi -Wall -lstdc++ $(OPTIMIZATION_OPT) -o
OPTIONS_LIBS     = -pedantic -ansi -Wall $(OPTIMIZATION_OPT) -c
INCLUDE = -I../../../libs/kipl/trunk/kipl/include
LIBS = -L../../../libs/kipl/trunk/kipl/Release -lkipl -ltiff -lm -lz -lgomp

CPP_SRC =

HPP_SRC =
HPP_SRC += base_fwt2d.hpp  
HPP_SRC += fwt2d_padded.hpp  
HPP_SRC += vec1d.hpp  
HPP_SRC += wavelet_utils.hpp

BUILDS = fwt_test


OBJECTS = $(CPP_SRC:.cpp=.o)


%.o: %.hpp %.cpp
	$(COMPILER) $(INCLUDE) $(OPTIONS_LIBS) $*.cpp


all: $(OBJECTS) $(BUILDS)

fwt_test: fwt_test.cpp $(HPP_SRC) $(OBJECTS)
	$(COMPILER) $(INCLUDE) $(OPTIONS) fwt_test fwt_test.cpp $(OBJECTS) $(LIBS)

clean:
	rm -f core *.o *.bak *stackdump *#
