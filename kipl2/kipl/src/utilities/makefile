CPP_FILES := $(wildcard *.cpp ) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))

all : $(OBJ_FILES)		

$(OBJ_DEST)/nodelocker.o: nodelocker.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/SystemInformation.o: SystemInformation.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^


clean : 	
	rm $(OBJ_FILES)