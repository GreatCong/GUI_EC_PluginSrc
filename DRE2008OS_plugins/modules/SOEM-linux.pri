#unix
#{
    INCLUDEPATH += \
        $$PWD/../SOEM-src/osal/linux/ \
        $$PWD/../SOEM-src/osal/ \
        $$PWD/../SOEM-src/oshw/linux/ \
        $$PWD/../SOEM-src/soem/

    HEADERS += \
        SOEM-src/osal/linux/osal_defs.h \
        SOEM-src/osal/osal.h \
        SOEM-src/oshw/linux/nicdrv.h \
        SOEM-src/oshw/linux/oshw.h \
        SOEM-src/soem/ethercat.h \
        SOEM-src/soem/ethercatbase.h \
        SOEM-src/soem/ethercatcoe.h \
        SOEM-src/soem/ethercatconfig.h \
        SOEM-src/soem/ethercatconfiglist.h \
        SOEM-src/soem/ethercatdc.h \
        SOEM-src/soem/ethercatfoe.h \
        SOEM-src/soem/ethercatmain.h \
        SOEM-src/soem/ethercatprint.h \
        SOEM-src/soem/ethercatsoe.h \
        SOEM-src/soem/ethercattype.h

    SOURCES += \
        SOEM-src/osal/linux/osal.c \
        SOEM-src/oshw/linux/nicdrv.c \
        SOEM-src/oshw/linux/oshw.c \
        SOEM-src/soem/ethercatbase.c \
        SOEM-src/soem/ethercatcoe.c \
        SOEM-src/soem/ethercatconfig.c \
        SOEM-src/soem/ethercatdc.c \
        SOEM-src/soem/ethercatfoe.c \
        SOEM-src/soem/ethercatmain.c \
        SOEM-src/soem/ethercatprint.c \
        SOEM-src/soem/ethercatsoe.c \
#}
