#win32{
#    win32:DEFINES+=_CRT_SECURE_NO_WARNING

    INCLUDEPATH += $$PWD/../SOEM-src/oshw/win32/wpcap/Include/
    INCLUDEPATH += \
       $$PWD/../SOEM-src/osal/win32/ \
       $$PWD/../SOEM-src/osal/ \
       $$PWD/../SOEM-src/oshw/win32/ \
       $$PWD/../SOEM-src/soem/

    # wpcap libs
    LIBS += $$PWD/../SOEM-src/oshw/win32/wpcap/Lib/Packet.lib \
            $$PWD/../SOEM-src/oshw/win32/wpcap/Lib/wpcap.lib
    LIBS += Ws2_32.lib \
            Winmm.lib

    HEADERS += \
        SOEM-src/oshw/win32/wpcap/Include/pcap/bluetooth.h \
        SOEM-src/oshw/win32/wpcap/Include/pcap/bpf.h \
        SOEM-src/oshw/win32/wpcap/Include/pcap/namedb.h \
        SOEM-src/oshw/win32/wpcap/Include/pcap/pcap.h \
        SOEM-src/oshw/win32/wpcap/Include/pcap/sll.h \
        SOEM-src/oshw/win32/wpcap/Include/pcap/usb.h \
        SOEM-src/oshw/win32/wpcap/Include/pcap/vlan.h \
        SOEM-src/oshw/win32/wpcap/Include/bittypes.h \
        SOEM-src/oshw/win32/wpcap/Include/ip6_misc.h \
        SOEM-src/oshw/win32/wpcap/Include/Packet32.h \
        SOEM-src/oshw/win32/wpcap/Include/pcap-bpf.h \
        SOEM-src/oshw/win32/wpcap/Include/pcap-namedb.h \
        SOEM-src/oshw/win32/wpcap/Include/pcap-stdinc.h \
        SOEM-src/oshw/win32/wpcap/Include/pcap.h \
        SOEM-src/oshw/win32/wpcap/Include/remote-ext.h \
        SOEM-src/oshw/win32/wpcap/Include/Win32-Extensions.h
    HEADERS += \
        SOEM-src/osal/win32/inttypes.h \
        SOEM-src/osal/win32/osal_defs.h \
        SOEM-src/osal/win32/osal_win32.h \
        SOEM-src/osal/win32/stdint.h \
        SOEM-src/osal/osal.h \
        SOEM-src/oshw/win32/nicdrv.h \
        SOEM-src/oshw/win32/oshw.h \
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
        SOEM-src/osal/win32/osal.c \
        SOEM-src/oshw/win32/nicdrv.c \
        SOEM-src/oshw/win32/oshw.c \
        SOEM-src/soem/ethercatbase.c \
        SOEM-src/soem/ethercatcoe.c \
        SOEM-src/soem/ethercatconfig.c \
        SOEM-src/soem/ethercatdc.c \
        SOEM-src/soem/ethercatfoe.c \
        SOEM-src/soem/ethercatmain.c \
        SOEM-src/soem/ethercatprint.c \
        SOEM-src/soem/ethercatsoe.c \
#}
