#-------------------------------------------------
#
# Date:
# Author: liucongjun
# Version: 1.0
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VirtualView_plugins
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

include(../plugin_common/plugin_common.pri)

include(VirtualView/VirtualView.pri)
#qt_propertybrowser
include(qt_propertybrowser/qt_propertybrowser.pri )

SOURCES += \
        VirtualView_plugins.cpp \
        Form_VirtualView.cpp \
    VirtualViewApp_Callback.cpp

HEADERS += \
        VirtualView_plugins.h \
        Form_VirtualView.h \
    VirtualViewApp_Callback.h

DISTFILES += VirtualView_plugins.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    Form_VirtualView.ui

RESOURCES += \
    res_icons.qrc
