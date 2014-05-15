ifndef REPOS_PATH
	export REPOS_PATH:=/home/kaestner/repos/src
endif
ifndef BIN_PATH
	export BIN_PATH:=$(REPOS_PATH)/bin
endif
export CXX:=g++-4.4
export CXX_FLAGS:=-DUSE_FFTW3 -O3 -msse2 -Wall -fmessage-length=0 -fopenmp -fPIC
export OBJ_DEST:=../../obj

AR=ar
ARFLAGS=-r
DIRS=base generators math scalespace utilities fft io morphology segmentation visualization filters logging profile strings wavelets

OBJ_FILES=obj/imagearithmetics.o obj/GNUPlot.o obj/fftbase.o obj/median.o \
	obj/GaussianNoise.o obj/fftbasef.o obj/miscstring.o obj/filenames.o \
	obj/morphdist.o obj/LUTCollection.o obj/filterbase.o obj/morphology.o \
	obj/LUTbase.o obj/filterenums.o obj/parenc.o obj/MicroTimer.o obj/io_fits.o \
	obj/string2array.o obj/NoiseImage.o obj/io_stack.o obj/sums.o obj/SequenceImage.o \
	obj/io_tiff.o obj/thresholds.o obj/SignalGenerator.o obj/label.o obj/wavelets.o \
	obj/Sine2D.o obj/mathfunctions.o obj/wsthres.o obj/Timer.o obj/matlabio.o \
	obj/zeropadding.o obj/logger.o obj/KiplException.o obj/kiplenums.o \
	obj/histogram.o obj/nodelocker.o obj/nonlinfit.o obj/numfunc.o \
	obj/imagesamplers.o
	
TARGET=$(BIN_PATH)/libkipl-4.4.so

all: $(TARGET)

$(TARGET) : $(OBJ_FILES)
	mkdir -p $(BIN_PATH)
	$(CXX) $(LD_FLAGS) -shared -o $(TARGET) $^ $(LIBS)

$(OBJ_FILES): 
	mkdir -p obj
	for i in $(DIRS); do make --directory src/$$i; done
       
clean:
	rm obj/*.o obj/*.a 
	#for i in $(DIRS); do make -directory $$i clean ; done
	
