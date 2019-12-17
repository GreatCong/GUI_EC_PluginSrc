# unix 用openGL编译总是会报错(要安装libqt5openGL)
#unix:DEFINES += QT_NO_OPENGL

# qcustomplot
INCLUDEPATH += $$PWD/../qcustomplot/

SOURCES += qcustomplot/qcustomplot.cpp


HEADERS += qcustomplot/qcustomplot.h
