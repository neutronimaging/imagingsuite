#-------------------------------------------------
#
# Project created by QtCreator 2015-02-17T07:15:06
#
#-------------------------------------------------

QT       -= gui

TARGET = FDKBackProjectors
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../../lib/debug

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    unix:macx {
        QMAKE_CXXFLAGS += -fPIC -O2
        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        INCLUDEPATH += /usr/include/libxml2


## frome here I add some CUDA stuff
        CUDA_DIR = /usr/local/cuda
        INCLUDEPATH +=$$CUDA_DIR/include
        QMAKE_LIBDIR +=$$CUDA_DIR/lib64
        LIBS += lcudart -lcuda
        # GPU architecture
#        CUDA_ARCH     = sm_20                # Yeah! I've a new device. Adjust with your compute capability. not sure which one is mine
        # Here are some NVCC flags I've always used by default.
        NVCCFLAGS     = --compiler-options -fno-strict-aliasing -use_fast_math --ptxas-options=-v

        # Prepare the extra compiler configuration (taken from the nvidia forum - i'm not an expert in this part)
        CUDA_INC = $$join(INCLUDEPATH,' -I','-I',' ')

        cuda.commands = $$CUDA_DIR/bin/nvcc -m64 -O3 -arch=$$CUDA_ARCH -c $$NVCCFLAGS \
                        $$CUDA_INC $$LIBS  ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT} \
                        2>&1 | sed -r \"s/\\(([0-9]+)\\)/:\\1/g\" 1>&2
        # nvcc error printout format ever so slightly different from gcc
        # http://forums.nvidia.com/index.php?showtopic=171651

        cuda.dependency_type = TYPE_C # there was a typo here. Thanks workmate!
        cuda.depend_command = $$CUDA_DIR/bin/nvcc -O3 -M $$CUDA_INC $$NVCCFLAGS   ${QMAKE_FILE_NAME}

        cuda.input = CUDA_SOURCES
        cuda.output = ${OBJECTS_DIR}${QMAKE_FILE_BASE}_cuda.o
        # Tell Qt that we want add more stuff to the Makefile
        QMAKE_EXTRA_COMPILERS += cuda
    }

    LIBS += -ltiff -lxml2 -lfftw3 -lfftw3f

}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH  += ../../../../../../external/src/linalg ../../../../../../external/include ../../../../../../external/include/cfitsio
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio -llibfftw3-3 -llibfftw3f-3
    QMAKE_CXXFLAGS += /openmp /O2
}


DEFINES += FDKBACKPROJ_LIBRARY

SOURCES += ../../src/fdkbackproj.cpp  \
        ../../src/genericbp.cpp \
        ../../src/fdkreconbase.cpp \
        ../../src/fdkbp.cpp \
        ../../src/fdkbp_single.cpp \
        ../../src/fdk_cuda.cpp
#        ../../src/ramp_filter.cpp
#        ../../src/fdk.cxx \
#        ../../src/bowtie_correction.cxx \
#        ../../src/delayload.cxx \
#        ../../src/file_util.cxx \
#        ../../src/plm_math.cxx \
#        ../../src/plm_timer.cxx \
#        ../../src/proj_image.cxx \
#        ../../src/proj_matrix.cxx \
#        ../../src/proj_image_filter.cxx \
#        ../../src/proj_matrix.cxx \
#        ../../src/volume.cxx
#understand what to include in a smart way

CUDA_SOURCES += ../../src/fdk_cuda.cu \
                ../../src/fdk_cuda_test.cu

HEADERS += ../../src/fdkbackproj.h \
	../../src/fdkbackproj_global.h \
	../../src/genericbp.h \
        ../../src/fdkreconbase.h \
         ../../src/fdkbp.h \
        ../../src/fdkbp_single.h \
        ../../src/fdk_cuda.h \
        ../../src/cuda_util.h \
        ../../src/fdk_cuda_p.h
#        ../../src/ramp_filter.h
#        ../../src/fdk.h \
#        ../../src/bowtie_correction.h \
#        ../../src/delayload.h \
#        ../../src/file_util.h \
#        ../../src/plm_math.h \
#        ../../src/plm_timer.h \
#        ../../src/proj_image.h \
#        ../../src/proj_image_filter.h \
#        ../../src/proj_matrix.h \
#        ../../src/volume.h \
#        ../../src/threding.h

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../lib/ -lkipl -lModuleConfig -lReconFramework
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug/ -lkipl -lModuleConfig -lReconFramework -lReconAlgorithms

INCLUDEPATH += $$PWD/../../../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../../../core/kipl/kipl/src

INCLUDEPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../core/modules/ModuleConfig/src

INCLUDEPATH += $$PWD/../../../../Framework/ReconAlgorithms/ReconAlgorithms
DEPENDPATH += $$PWD/../../../../Framework/ReconAlgorithms/ReconAlgorithms

INCLUDEPATH += $$PWD/../../../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../../../Framework/ReconFramework/src

macx: {
INCLUDEPATH += $$PWD/../../../../../../external/mac/include
DEPENDPATH += $$PWD/../../../../../../external/mac/include
LIBS += -L$$PWD/../../../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
}


