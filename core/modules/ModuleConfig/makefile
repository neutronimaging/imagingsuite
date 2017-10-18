TARGET := $(BIN_DEST)/libModuleConfig.so
OBJ_DEST := obj
INCLUDES := -I"../../../kipl/trunk/kipl/include" -I"/usr/include/libxml2"
CXX_FLAGS := -O3 -Wall -c -fmessage-length=0 -fopenmp -fPIC -MMD -MP -msse2
LD_FLAGS := -L"../../../kipl/trunk/kipl"
LIBS := -lkipl -lxml2 -lgomp -lz

CPP_FILES := $(wildcard src/*.cpp ) 
OBJ_FILES := $(addprefix $(OBJ_DEST)/,$(notdir $(CPP_FILES:.cpp=.o)))

all : $(TARGET)
			
$(TARGET) : $(OBJ_FILES)
	$(CXX) $(LD_FLAGS) -shared -o $(TARGET) $^ $(LIBS)
	
$(OBJ_DEST)/ConfigBase.o: src/ConfigBase.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/ModuleConfig.o: src/ModuleConfig.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/ModuleException.o: src/ModuleException.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/ParameterHandling.o: src/ParameterHandling.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^			
	
$(OBJ_DEST)/ModuleItemBase.o: src/ModuleItemBase.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^	

$(OBJ_DEST)/ProcessModuleBase.o: src/ProcessModuleBase.cpp
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