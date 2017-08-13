#-------------------------------------------------
#
# Project created by QtCreator 2013-04-23T07:11:54
#
#-------------------------------------------------

QT       -= gui
QT      += core

TARGET = kipl
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

message("Destdir $$DESTDIR")

unix {
    INCLUDEPATH += "../../../../external/src/linalg"
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    unix:macx {
#        QMAKE_MAC_SDK = macosx10.11
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
    }
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../../external/src/linalg ../../../../external/include ../../../../external/include/cfitsio
    QMAKE_LIBDIR += ../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2

    DEFINES += NOMINMAX
}

win32:CONFIG(release, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:win32:CONFIG(debug, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:symbian: LIBS += -lm -lz -ltiff -lfftw3 -lfftw3f -lcfitsio
else:unix: LIBS +=  -lm -lz -ltiff -lfftw3 -lfftw3f -lcfitsio




DEFINES += KIPL_LIBRARY

SOURCES += \
    ../src/math/sums.cpp \
    ../src/math/numfunc.cpp \
    ../src/math/nonlinfit.cpp \
    ../src/math/median.cpp \
    ../src/math/mathfunctions.cpp \
    ../src/math/LUTCollection.cpp \
    ../src/math/LUTbase.cpp \
    ../src/math/GaussianNoise.cpp \
    ../src/logging/logger.cpp \
    ../src/io/io_tiff.cpp \
    ../src/io/io_stack.cpp \
    ../src/io/core/matlabio.cpp \
    ../src/io/core/io_fits.cpp \
    ../src/generators/Sine2D.cpp \
    ../src/generators/SignalGenerator.cpp \
    ../src/generators/SequenceImage.cpp \
    ../src/generators/NoiseImage.cpp \
    ../src/filters/filterbase.cpp \
    ../src/fft/zeropadding.cpp \
    ../src/fft/fftbasef.cpp \
    ../src/fft/fftbase.cpp \
    ../src/base/KiplException.cpp \
    ../src/base/index2coord.cpp \
    ../src/base/imagesamplers.cpp \
    ../src/base/imageinfo.cpp \
    ../src/base/core/kiplenums.cpp \
    ../src/base/core/imagearithmetics.cpp \
    ../src/base/core/histogram.cpp \
    ../src/base/core/aligned_malloc.cpp \
    ../src/wavelets/wavelets.cpp \
    ../src/visualization/GNUPlot.cpp \
    ../src/utilities/SystemInformation.cpp \
    ../src/utilities/nodelocker.cpp \
    ../src/strings/string2array.cpp \
    ../src/strings/parenc.cpp \
    ../src/strings/miscstring.cpp \
    ../src/strings/filenames.cpp \
    ../src/segmentation/wsthres.cpp \
    ../src/segmentation/thresholds.cpp \
    ../src/scalespace/filterenums.cpp \
    ../src/profile/Timer.cpp \
    ../src/profile/MicroTimer.cpp \
    ../src/octree/octree.cpp \
    ../src/morphology/morphology.cpp \
    ../src/morphology/morphdist.cpp \
    ../src/morphology/label.cpp \
    ../src/utilities/TimeDate.cpp \
    ../src/pca/pca.cpp \
    ../src/math/covariance.cpp \
    ../src/morphology/skeleton.cpp \
    ../src/io/DirAnalyzer.cpp \
    ../src/math/linfit.cpp \
    ../src/generators/spotgenerator.cpp \
    ../src/io/analyzefileext.cpp \
    ../src/io/io_generic.cpp \
    ../src/filters/nonlocalmeans.cpp \
    ../src/math/PoissonNoise.cpp \
    ../src/filters/stddevfilter.cpp \
    ../src/interactors/interactionbase.cpp \
    ../src/segmentation/multivariateclassifyerbase.cpp \
    ../src/morphology/pixeliterator.cpp


HEADERS +=\
    ../include/kipl_global.h \
    ../include/algorithms/sortalg.h \
    ../include/base/tsubimage.h \
    ../include/base/trotate.h \
    ../include/base/tpermuteimage.h \
    ../include/base/timagetests.h \
    ../include/base/timage.h \
    ../include/base/thistogram.h \
    ../include/base/textractor.h \
    ../include/base/KiplException.h \
    ../include/base/kiplenums.h \
    ../include/base/index2coord.h \
    ../include/base/imagesamplers.h \
    ../include/base/imageoperators.h \
    ../include/base/imageinfo.h \
    ../include/base/imagecast.h \
    ../include/base/core/tsubimage.hpp \
    ../include/base/core/trotate.hpp \
    ../include/base/core/transpose.hpp \
    ../include/base/core/tpermuteimage.hpp \
    ../include/base/core/timage.hpp \
    ../include/base/core/textractor.hpp \
    ../include/base/core/sharedbuffer.h \
    ../include/base/core/quad.h \
    ../include/base/core/imagesamplers.hpp \
    ../include/base/core/imagecast.hpp \
    ../include/base/core/imagearithmetics.h \
    ../include/base/core/aligned_malloc.h \
    ../include/containers/PlotData.h \
    ../include/containers/ArrayBuffer.h \
    ../include/drawing/drawing.h \
    ../include/drawing/core/drawing.hpp \
    ../include/fft/zeropadding.h \
    ../include/fft/fftbase.h \
    ../include/fft/core/zeropadding.hpp \
    ../include/filters/convolutionkernels.h \
    ../include/filters/structureelements.h \
    ../include/filters/medianfilter.h \
    ../include/filters/kernelbase.h \
    ../include/filters/filterbase.h \
    ../include/filters/filter.h \
    ../include/filters/laplacianofgaussian.h \
    ../include/filters/core/laplacianofgaussian.hpp \
    ../include/filters/core/medianfilter.hpp \
    ../include/filters/core/filterbase.hpp \
    ../include/filters/core/filter.hpp \
    ../include/generators/Sine2D.h \
    ../include/generators/SignalGenerator.h \
    ../include/generators/SiemensStar.h \
    ../include/generators/SequenceImage.h \
    ../include/generators/NoiseImage.h \
    ../include/io/io_vtk.h \
    ../include/io/io_tiff.h \
    ../include/io/io_stack.h \
    ../include/io/io_matlab.h \
    ../include/io/io_fits.h \
    ../include/io/core/matlabio.h \
    ../include/io/core/io_fits.hpp \
    ../include/octree/octree.h \
    ../include/morphology/palagyi_skeleton.h \
    ../include/morphology/morphquantify.h \
    ../include/morphology/morphology.h \
    ../include/morphology/morphgeo.h \
    ../include/morphology/morphfilters.h \
    ../include/morphology/morphextrema.h \
    ../include/morphology/morphdist.h \
    ../include/morphology/label.h \
    ../include/morphology/DanielssonDistance.h \
    ../include/morphology/core/morphfilters.hpp \
    ../include/morphology/core/morphdist.hpp \
    ../include/morphology/core/DanielssonDistance.hpp \
    ../include/morphology/base3dskeleton.h \
    ../include/math/sums.h \
    ../include/math/statistics.h \
    ../include/math/numfunc.h \
    ../include/math/nonlinfit.h \
    ../include/math/median.h \
    ../include/math/mathfunctions.h \
    ../include/math/mathconstants.h \
    ../include/math/LUTCollection.h \
    ../include/math/LUTbase.h \
    ../include/math/LinearAlgebra.h \
    ../include/math/jama_inverses.h \
    ../include/math/image_statistics.h \
    ../include/math/GaussianNoise.h \
    ../include/math/core/statistics.hpp \
    ../include/math/core/median.hpp \
    ../include/math/core/mathfunctions.hpp \
    ../include/math/core/LinearAlgebra.hpp \
    ../include/math/core/image_statistics.hpp \
    ../include/math/core/compleximage.hpp \
    ../include/math/core/basicprojector.hpp \
    ../include/math/compleximage.h \
    ../include/math/basicprojector.h \
    ../include/regression/linefit.h \
    ../include/regression/core/linefit.hpp \
    ../include/segmentation/wsthres.h \
    ../include/segmentation/thresholds.h \
    ../include/segmentation/segmentationbase.h \
    ../include/segmentation/SeededRegionGrowing.h \
    ../include/segmentation/orientedupdater.h \
    ../include/segmentation/multiresseg.h \
    ../include/segmentation/mapupdater.h \
    ../include/segmentation/kernelfuzzykmeans.h \
    ../include/segmentation/fuzzykmeans.h \
    ../include/segmentation/core/multiresseg.hpp \
    ../include/segmentation/core/mapupdater.hpp \
    ../include/segmentation/core/kernelfuzzykmeans.hpp \
    ../include/segmentation/core/fuzzykmeans.hpp \
    ../include/segmentation/core/ClassGrowing.hpp \
    ../include/segmentation/ClassGrowing.h \
    ../include/segmentation/bigunupdater.h \
    ../include/scalespace/shock_filter.h \
    ../include/scalespace/separabelfilters.h \
    ../include/scalespace/NonLinDiffAOS.h \
    ../include/scalespace/lambdaest.h \
    ../include/scalespace/ISSfilterQ3Dp.h \
    ../include/scalespace/ISSfilterQ3D.h \
    ../include/scalespace/ISSfilterOrig3D.h \
    ../include/scalespace/ISSfilter2D.h \
    ../include/scalespace/ISSfilter.h \
    ../include/scalespace/filterenums.h \
    ../include/scalespace/diff_filterbase.h \
    ../include/scalespace/core/ISSfilterQ3Dp.hpp \
    ../include/scalespace/core/ISSfilterQ3D.hpp \
    ../include/scalespace/core/ISSfilter.hpp \
    ../include/scalespace/AnisoDiff.h \
    ../include/queuefilter/thresholdworker.h \
    ../include/queuefilter/shockworker.h \
    ../include/queuefilter/segmentationworker.h \
    ../include/queuefilter/segmentationbase.h \
    ../include/queuefilter/rankfilterworker.h \
    ../include/queuefilter/queuefiltermanager.h \
    ../include/queuefilter/queuefilter.h \
    ../include/queuefilter/mpitimer.h \
    ../include/queuefilter/mpiqueuefilter.h \
    ../include/queuefilter/modifierworker.h \
    ../include/queuefilter/maskworker.h \
    ../include/queuefilter/mapupdater.h \
    ../include/queuefilter/linearfilterworker.h \
    ../include/queuefilter/ioworker.h \
    ../include/queuefilter/factoryworkerxml.h \
    ../include/queuefilter/factoryworker.h \
    ../include/queuefilter/distanceworker.h \
    ../include/queuefilter/diffusionworker.h \
    ../include/queuefilter/basequeueworker.h \
    ../include/queuefilter/baseneighborhoodoperatorworker.h \
    ../include/queuefilter/absgradworker.h \
    ../include/profile/Timer.h \
    ../include/profile/MicroTimer.h \
    ../include/porespace/poresize.h \
    ../include/porespace/core/poresize.hpp \
    ../include/wavelets/wavelets.h \
    ../include/wavelets/core/wavelets.hpp \
    ../include/visualization/GNUPlot.h \
    ../include/utilities/SystemInformation.h \
    ../include/utilities/nodelocker.h \
    ../include/strings/string2array.h \
    ../include/strings/parenc.h \
    ../include/strings/miscstring.h \
    ../include/strings/filenames.h \
    ../include/stltools/zerocrossings.h \
    ../include/stltools/watershedstl.h \
    ../include/stltools/stlvecmath.h \
    ../include/stltools/geodesicstl.h \
    ../include/logging/logger.h \
    ../include/utilities/TimeDate.h \
    ../include/math/covariance.h \
    ../include/pca/pca.h \
    ../include/math/tnt_utils.h \
    ../include/math/core/covariance.hpp \
    ../include/filters/GaborFilter.h \
    ../include/morphology/skeleton.h \
    ../include/base/core/thistogram.hpp \
    ../include/io/DirAnalyzer.h \
    ../include/math/linfit.h \
    ../include/math/core/linfit.hpp \
    ../include/base/tprofile.h \
    ../include/base/core/tprofile.hpp \
    ../include/generators/spotgenerator.h \
    ../include/io/io_generic.h \
    ../include/io/core/io_generic.hpp \
    ../include/io/analyzefileext.h \
    ../include/generators/core/noiseimage.hpp \
    ../include/filters/nonlocalmeans.h \
    ../include/filters/core/nonlocalmeans.hpp \
    ../include/math/PoissonNoise.h \
    ../include/filters/stddevfilter.h \
    ../include/interactors/interactionbase.h \
    ../include/containers/ringbuffer.h \
    ../include/segmentation/multivariateclassifyerbase.h \
    ../include/morphology/pixeliterator.h \
    ../include/segmentation/gradientguidedthreshold.h \
    ../include/segmentation/core/gradientguidedthreshold.hpp \
    ../include/morphology/morphgeo2.h

unix:!mac {
exists(/usr/lib/*NeXus*) {

    message("-lNeXus exists")
    DEFINES += HAVE_NEXUS
    LIBS += -lNeXus -lNeXusCPP
    SOURCES += ../src/io/io_nexus.cpp
    HEADERS += ../include/io/io_nexus.h
}
else {
message("-lNeXus does not exists $$HEADERS")
}

}

unix:mac {
exists(/usr/local/lib/*NeXus*) {

    message("-lNeXus exists")
    DEFINES += HAVE_NEXUS
#    INCLUDEPATH += /usr/local/lib
#    LIBS += -L/usr/local/lib/  -lNeXus -lNeXusCPP

    LIBS += -L$$PWD/../../../../../../../usr/local/lib/ -lNeXusCPP.1.0.0 -lNeXus

    INCLUDEPATH += $$PWD/../../../../../../../usr/local/include
    DEPENDPATH += $$PWD/../../../../../../../usr/local/include

    SOURCES += ../src/io/io_nexus.cpp
    HEADERS += ../include/io/io_nexus.h
}
else {
message("-lNeXus does not exists $$HEADERS")
}

}

win32 {

exists($$PWD/../../../../external/lib64/nexus/*NeXus*) {

    message("-lNeXus exists")
    DEFINES += HAVE_NEXUS
    INCLUDEPATH += $$PWD/../../../../external/include/nexus $$PWD/../../../../external/include/hdf5
    QMAKE_LIBDIR += $$PWD/../../../../external/lib64/nexus $$PWD/../../../../external/lib64/hdf5

    LIBS +=  -lNeXus -lNeXusCPP

    SOURCES += ../src/io/io_nexus.cpp
    HEADERS += ../include/io/io_nexus.h
}

}






symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE96935EB
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = kipl.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

DISTFILES += \
    ../include/filters/nonlocalmeans.txt

message($$INCLUDEPATH)




