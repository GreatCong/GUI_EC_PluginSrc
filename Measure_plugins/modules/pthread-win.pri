INCLUDEPATH += \
    $$PWD/../pthread-build-win/Pre-built.2/include/

HEADERS += \
    pthread-build-win/Pre-built.2/include/pthread.h \
    pthread-build-win/Pre-built.2/include/sched.h \
    pthread-build-win/Pre-built.2/include/semaphore.h

LIBS += $$PWD/../pthread-build-win/Pre-built.2/lib/x86/pthreadVC2.lib
