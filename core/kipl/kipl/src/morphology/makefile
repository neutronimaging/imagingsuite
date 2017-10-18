CPP_FILES := $(wildcard *.cpp ) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))

all : $(OBJ_FILES)		
	
$(OBJ_DEST)/label.o: label.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/morphdist.o: morphdist.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/morphology.o: morphology.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^


clean : 	
	rm $(OBJ_FILES)