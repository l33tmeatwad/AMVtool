#-------------------------------------------------
#
# Project created by QtCreator 2016-06-07T13:53:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AMVtool
TEMPLATE = app

SOURCES += source/main.cpp\
        source/amvtool.cpp \
    source/configure.cpp \
    source/filesettings.cpp \
    source/setupencode.cpp

macx: SOURCES += source/OSX/checkmedia.cpp
win32: SOURCES += source/WIN/checkmedia.cpp

HEADERS  += source/amvtool.h \
    source/configure.h \
    source/filesettings.h \
    source/checkmedia.h \
    source/setupencode.h

macx: FORMS    += source/OSX/amvtool.ui \
    source/OSX/configure.ui

win32: FORMS    += source/WIN/amvtool.ui \
    source/WIN/configure.ui

INCLUDEPATH += $$PWD/Resources
DEPENDPATH += $$PWD/Resources

linux: LIBS += -L$$PWD/Resources/ -lMediaInfo
macx: LIBS += -L$$PWD/Resources/ -lmediainfo.0
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Resources/ -lMediaInfo

macx: ICON = Resources/icon.icns

