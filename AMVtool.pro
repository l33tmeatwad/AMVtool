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
    Source/setupencode.cpp \
    Source/queue.cpp \
    Source/progress.cpp \
    Source/dependencies.cpp

linux: SOURCES += Source/checkmedia.cpp
macx: SOURCES += Source/checkmedia.cpp
win32: SOURCES += Source/Windows/checkmedia.cpp

HEADERS  += Source/amvtool.h \
    Source/configure.h \
    Source/filesettings.h \
    Source/checkmedia.h \
    Source/setupencode.h \
    Source/queue.h \
    Source/progress.h \
    Source/dependencies.h

FORMS    += Source/amvtool.ui \
            Source/configure.ui

LIBPATH += $$PWD/Resources

linux: LIBS += -lmediainfo
linux: CONFIG += c++11
macx: LIBS += -lmediainfo.0
win32: LIBS +=  -lmediainfo -lavisynthwrapper

macx: ICON = Resources/icon.icns
win32: RC_ICONS = Resources/icon.ico

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
