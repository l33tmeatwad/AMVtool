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

INCLUDEPATH += $$PWD/Resources/Include

linux{
    SOURCES += Source/checkmedia.cpp
    LIBS += -lmediainfo
    CONFIG += c++11
    icon.path = /usr/share/pixmaps
    icon.files = Resources/Linux/AMVtool.png
    shortcut.path = /usr/share/applications
    shortcut.files = Resources/Linux/AMVtool.desktop
    target.path = /usr/local/bin
    INSTALLS += target icon shortcut
}

macx{
    SOURCES += Source/checkmedia.cpp
    LIBPATH += $$PWD/Resources/MacOS
    LIBS += -lmediainfo -lzen -lz
    QMAKE_INFO_PLIST = Resources/MacOS/Info.plist
    ICON = Resources/MacOS/icon.icns
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
}

win32{
    SOURCES += Source/Windows/checkmedia.cpp
    LIBPATH += $$PWD/Resources/Windows
    LIBS +=  -lmediainfo -lavisynthwrapper
    RC_ICONS = Resources/Windows/icon.ico
    VERSION = 1.2.0.1
    QMAKE_TARGET_PRODUCT = "AMVtool"
    QMAKE_TARGET_DESCRIPTION = "GUI for FFmpeg designed for AMV editors."
    QMAKE_TARGET_COPYRIGHT = "\\251 2023 by l33tmeatwad"
}
