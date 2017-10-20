BIN_DEST := ../bin
OBJ_DEST := obj
TARGET := $(BIN_DEST)/libStdBackProjectors.so
INCLUDES := -I"../ReconFramework/include" -I"../../../kipl/trunk/kipl/include" -I"../../../modules/trunk/ModuleConfig/include" -I"../../../../external/src/libwavelet" -I/usr/include/libxml2 
CXX_FLAGS := -DUSE_FFTW3 -O3 -Wall -c -fmessage-length=0 -fopenmp -fPIC -MMD -MP -msse2
LD_FLAGS :=-L"../../../kipl/trunk/kipl" -L"../../../modules/trunk/bin" -L"../bin"
LIBS := -lkipl -lfftw3 -lfftw3f -lcfitsio -lxml2 -lgomp -ltiff -lz -lModuleConfig -lReconFramework

CPP_FILES := $(wildcard src/*.cpp ) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))

all : $(TARGET)
			
$(TARGET) : $(OBJ_FILES)
	mkdir -p $(BIN_DEST)
	$(CXX) $(LD_FLAGS) -shared -o $(TARGET) $^ $(LIBS)
	
$(OBJ_DEST)/MultiProjBP.o: src/MultiProjBP.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/NNMultiProjBP.o: src/NNMultiProjBP.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/MultiProjBPparallel.o: src/MultiProjBPparallel.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/StdBackProjectors.o: src/StdBackProjectors.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/StdBackProjectorBase.o: src/StdBackProjectorBase.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/dllmain.o: src/dllmain.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/stdafx.o: src/stdafx.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^						
	
clean: 
	rm $(OBJ_DEST)/*.o $(OBJ_DEST)/*.d
	rm $(TARGET)	