CXX = g++
CPP_FILES := $(wildcard *.cpp ) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))
#LD_FLAGS := 
CXX_FLAGS := -O2

all : $(OBJ_FILES)		
		
$(OBJ_DEST)/wavelets.o: wavelets.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^


clean : 	
	rm $(OBJ_FILES)