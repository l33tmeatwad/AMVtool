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

LIBPATH += $$PWD/Resources
INCLUDEPATH += $$PWD/Resources

linux{
    SOURCES += Source/checkmedia.cpp
    LIBS += -lmediainfo
    CONFIG += c++11
    icon.path = /usr/share/pixmaps
    icon.files = Resources/AMVtool.png
    shortcut.path = /usr/share/applications
    shortcut.files = Resources/AMVtool.desktop
    target.path = /usr/local/bin
    INSTALLS += target icon shortcut
}

macx{
    SOURCES += Source/checkmedia.cpp
    LIBS += -lmediainfo.0
    QMAKE_INFO_PLIST = Resources/Info.plist
    QMAKE_APP_OR_DLL = Resources/
    ICON = Resources/icon.icns
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
}

win32{
    SOURCES += Source/Windows/checkmedia.cpp
    LIBS +=  -lmediainfo -lavisynthwrapper
    RC_ICONS = Resources/icon.ico
    VERSION = 1.0.0.9
    QMAKE_TARGET_PRODUCT = "AMVtool"
    QMAKE_TARGET_DESCRIPTION = "GUI for FFmpeg designed for AMV editors."
    QMAKE_TARGET_COPYRIGHT = "\\251 2018 by l33tmeatwad"
}
