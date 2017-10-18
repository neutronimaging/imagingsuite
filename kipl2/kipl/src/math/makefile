CPP_FILES := $(wildcard *.cpp ) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))
INCLUDES += -I"$(REPOS_PATH)/external/src/linalg"
all : $(OBJ_FILES)
			
$(OBJ_DEST)/GaussianNoise.o: GaussianNoise.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c -o $@ $^

$(OBJ_DEST)/LUTbase.o: LUTbase.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c -o $@ $^

$(OBJ_DEST)/median.o: median.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c -o $@ $^

$(OBJ_DEST)/LUTCollection.o: LUTCollection.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c -o $@ $^

$(OBJ_DEST)/mathfunctions.o: mathfunctions.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c -o $@ $^

$(OBJ_DEST)/nonlinfit.o: nonlinfit.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c -o $@ $^

$(OBJ_DEST)/numfunc.o: numfunc.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c -o $@ $^

$(OBJ_DEST)/sums.o: sums.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c -o $@ $^

clean : 	
	rm $(OBJ_FILES)
