BIN_DEST := ../bin
OBJ_DEST := obj
TARGET := $(BIN_DEST)/libReconFramework.so
INCLUDES := -I"../../../kipl/trunk/kipl/include" -I"../../../modules/trunk/ModuleConfig/include" -I/usr/include/libxml2
CXX_FLAGS := -O3 -Wall -c -fmessage-length=0 -fopenmp -fPIC -MMD -MP -msse2
LD_FLAGS :=-L"../../../kipl/trunk/kipl" -L"../../../modules/trunk/bin"
LIBS :=-lModuleConfig -lkipl -lcfitsio -lxml2 -lgomp -ltiff -lz

CPP_FILES := $(wildcard src/*.cpp ) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))

all : $(TARGET)
			
$(TARGET) : $(OBJ_FILES)
	mkdir -p $(BIN_DEST)
	$(CXX) $(LD_FLAGS) -shared -o $(TARGET) $^ $(LIBS)
	
$(OBJ_DEST)/BackProjectorBase.o: src/BackProjectorBase.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/PreprocModuleBase.o: src/PreprocModuleBase.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/ReconFramework.o: src/ReconFramework.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^	
	
$(OBJ_DEST)/stdafx.o: src/stdafx.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^	

$(OBJ_DEST)/dllmain.o: src/dllmain.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^	

$(OBJ_DEST)/InteractionBase.o: src/InteractionBase.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^	

$(OBJ_DEST)/ProjectionReader.o: src/ProjectionReader.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^	
				
$(OBJ_DEST)/ReconEngine.o: src/ReconEngine.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^	

$(OBJ_DEST)/ReconFactory.o: src/ReconFactory.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^	
	
$(OBJ_DEST)/ModuleItem.o: src/ModuleItem.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^	

$(OBJ_DEST)/ReconException.o: src/reconException.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^	

$(OBJ_DEST)/ReconHelpers.o: src/ReconHelpers.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^	
		
$(OBJ_DEST)/ReconConfig.o: src/ReconConfig.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^			
	
clean: 
	rm $(OBJ_DEST)/*.o $(OBJ_DEST)/*.d
	rm $(TARGET)	