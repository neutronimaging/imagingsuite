CPP_FILES := $(wildcard *.cpp core/*.cpp) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))

all : $(OBJ_FILES)
	
$(OBJ_DEST)/index2coord.o: index2coord.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/imageinfo.o: imageinfo.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/KiplException.o: KiplException.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/imagesamplers.o: imagesamplers.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^


$(OBJ_DEST)/aligned_malloc.o: core/aligned_malloc.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/imagearithmetics.o: core/imagearithmetics.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/histogram.o: core/histogram.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/kiplenums.o: core/kiplenums.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

clean : 	
	rm $(OBJ_FILES)
