#-------------------------------------------------
#
# Date:
# Author: liucongjun
# Version: 1.0
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = DRE2008OS_plugin
TEMPLATE = lib
CONFIG += plugin

#不在安装目录中生成
#DESTDIR = $$[QT_INSTALL_PLUGINS]/generic

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(debug,debug|release):{
unix:TARGET=$$join(TARGET,,,_debug)
win32:TARGET=$$join(TARGET,,,_d)
}

# qcustomplot
include("modules/qcustomplot.pri")

#INCLUDEPATH += DRE2008OS/

include(../plugin_common/plugin_common.pri)

HEADERS += \
    form_plot.h \
    DRE2008OS_plugins.h \
    DRE2008_OS_Callback.h \
    My_FFT.h \
    AppUser_Message.h

SOURCES += \
    form_plot.cpp \
    DRE2008OS_plugins.cpp \
    DRE2008_OS_Callback.cpp \
    My_FFT.c \
    AppUser_Message.cpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    form_plot.ui

DISTFILES += \
    DRE2008OS_plugins.json

RESOURCES += \
    res_icons.qrc
