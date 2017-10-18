CPP_FILES := $(wildcard *.cpp ) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))

all : $(OBJ_FILES)		
		
$(OBJ_DEST)/logger.o: logger.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^


clean : 	
	rm $(OBJ_FILES)