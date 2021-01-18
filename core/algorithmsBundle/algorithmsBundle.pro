TEMPLATE = subdirs

SUBDIRS = \
    ../algorithms/ImagingAlgorithms/qt/ImagingAlgorithms/ImagingAlgorithms.pro \
    ../algorithms/ImagingQAAlgorithms \
    ../algorithms/UnitTests/tImagingAlgorithms \
    ../algorithms/UnitTests/tImagingQAAlgorithms

HEADERS += \
    ../kipl/kipl/include/io/io_csv.h

SOURCES += \
    ../kipl/kipl/src/io/io_csv.cpp
