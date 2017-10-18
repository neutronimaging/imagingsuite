CPP_FILES := $(wildcard *.cpp ) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))

all : $(OBJ_FILES)		

$(OBJ_DEST)/fftbase.o: fftbase.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/fftbasef.o: fftbasef.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/zeropadding.o: zeropadding.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

clean : 	
	rm $(OBJ_FILES)