#-------------------------------------------------
#
# Project created by QtCreator 2016-06-07T13:53:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AMVtool
TEMPLATE = app

SOURCES += Source/main.cpp\
        Source/amvtool.cpp \
    Source/configure.cpp \
    Source/filesettings.cpp \
    Source/setupencode.cpp

linux: SOURCES += Source/checkmedia.cpp
macx: SOURCES += Source/checkmedia.cpp
win32: SOURCES += Source/Windows/checkmedia.cpp

HEADERS  += Source/amvtool.h \
    Source/configure.h \
    Source/filesettings.h \
    Source/checkmedia.h \
    Source/setupencode.h

linux: FORMS    += Source/Linux/amvtool.ui \
    Source/Linux/configure.ui

macx: FORMS    += Source/OSX/amvtool.ui \
    Source/OSX/configure.ui

win32: FORMS    += Source/Windows/amvtool.ui \
    Source/Windows/configure.ui

INCLUDEPATH += $$PWD/Resources
DEPENDPATH += $$PWD/Resources

linux: LIBS += -lmediainfo
linux: CONFIG += c++11
macx: LIBS += -L$$PWD/Resources -lmediainfo.0
win32:CONFIG(release, debug|release): LIBS +=  -lMediaInfo


macx: ICON = Resources/icon.icns
win32: RC_ICONS = Resources/icon.ico

