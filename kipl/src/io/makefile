CPP_FILES := $(wildcard *.cpp core/*.cpp) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))

all : $(OBJ_FILES)
	
$(OBJ_DEST)/io_stack.o: io_stack.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/io_tiff.o: io_tiff.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/io_fits.o: core/io_fits.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/matlabio.o: core/matlabio.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

clean : 	
	rm $(OBJ_FILES)