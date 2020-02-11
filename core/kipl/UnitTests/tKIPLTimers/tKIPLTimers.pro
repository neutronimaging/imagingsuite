QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

CONFIG += c++11

TEMPLATE = app

SOURCES +=  tst_timertests.cpp
