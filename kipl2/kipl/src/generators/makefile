CPP_FILES := $(wildcard *.cpp ) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))

all : $(OBJ_FILES)
	
	
$(OBJ_DEST)/NoiseImage.o: NoiseImage.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/SignalGenerator.o: SignalGenerator.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/SequenceImage.o: SequenceImage.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/Sine2D.o: Sine2D.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $^

clean : 	
	rm $(OBJ_FILES)