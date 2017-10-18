CPP_FILES := $(wildcard *.cpp ) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))

all : $(OBJ_FILES)		

$(OBJ_DEST)/MicroTimer.o: MicroTimer.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/Timer.o: Timer.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

clean : 	
	rm $(OBJ_FILES)