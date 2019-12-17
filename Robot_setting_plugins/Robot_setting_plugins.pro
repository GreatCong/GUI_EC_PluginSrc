#-------------------------------------------------
#
# Date:
# Author: liucongjun
# Version: 1.0
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += serialport

TARGET = Robot_setting_plugins
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
include(./xlsx/qtxlsx.pri)

#Excel_obj
SOURCES += $$PWD/Excel_Object/Excel_Object.cpp
HEADERS += $$PWD/Excel_Object/Excel_Object.h \
           $$PWD/Excel_Object/algorithm_common.h
INCLUDEPATH += $$PWD/Excel_Object

SOURCES += \
        Robot_setting_plugin.cpp \
    Form_Robot_Setting.cpp \
    Serial_Handle.cpp

HEADERS += \
        Robot_setting_plugin.h \
    Form_Robot_Setting.h \
    Serial_Handle.h

DISTFILES += Robot_setting_plugins.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    Form_Robot_Setting.ui

RESOURCES += \
    res_icons.qrc
