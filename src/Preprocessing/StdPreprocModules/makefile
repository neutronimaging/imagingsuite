BIN_DEST := ../bin
OBJ_DEST := obj
TARGET := $(BIN_DEST)/libStdPreprocModules.so
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
	
$(OBJ_DEST)/AdaptiveFilter.o: src/AdaptiveFilter.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/BasicRingClean.o: src/BasicRingClean.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/DataScaler.o: src/DataScaler.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/GeneralFilter.o: src/GeneralFilter.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/ISSfilter.o: src/ISSfilter.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/MedianMixRingClean.o: src/MedianMixRingClean.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/NormPlugins.o: src/NormPlugins.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/PolynomialCorrection.o: src/PolynomialCorrection.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/PreprocEnums.o: src/Preprocenums.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/ProjectionFilter.o: src/ProjectionFilter.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/SpotClean.o: src/SpotClean.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/SpotClean2.o: src/SpotClean2.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/SpotRingClean.o: src/SpotRingClean.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/StdPreprocModules.o: src/StdPreprocModules.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/StripeFilter.o: src/StripeFilter.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^

$(OBJ_DEST)/TranslatedProjectionWeighting.o: src/TranslatedProjectionWeighting.cpp
	mkdir -p $(OBJ_DEST)
	$(CXX) $(INCLUDES) $(CXX_FLAGS) -c -o $@ $^
	
$(OBJ_DEST)/WaveletRingClean.o: src/WaveletRingClean.cpp
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