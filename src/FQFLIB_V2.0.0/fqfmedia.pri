INCLUDEPATH += $$PWD

#QT += openglextensions opengl

INCLUDEPATH += ./../include

win32{
    #区分32位和64位windows程序
    opt = $$find(QMAKESPEC,"msvc2015_64")
    opt2 = $$find(QMAKESPEC,"msvc2017_64")
    isEmpty(opt){
        isEmpty(opt2){
            LIBS += -L./../lib/win32
        }
        !isEmpty(opt2){
            LIBS += -L./../lib/win64
        }
    }
    !isEmpty(opt){
        LIBS += -L./../lib/win64
    }
}
unix{
    LIBS += -L./../lib
}
mac{
    LIBS += -L./../lib
}


LIBS += -lavformat -lavcodec -lavutil -lswresample -lswscale

HEADERS += \
    $$PWD/fqfaudiothread.h \
    $$PWD/fqfdecode.h \
    $$PWD/fqfdecodethread.h \
    $$PWD/fqfdemux.h \
    $$PWD/fqfdemuxthread.h \
    $$PWD/fqfresample.h \
    $$PWD/fqfaudiodevice.h \
    $$PWD/fqfvideodevice.h \
    $$PWD/fqfvideothread.h \
    $$PWD/fqfscale.h \
    $$PWD/fqftools.h

SOURCES += \
    $$PWD/fqfaudiothread.cpp \
    $$PWD/fqfdecode.cpp \
    $$PWD/fqfdecodethread.cpp \
    $$PWD/fqfdemux.cpp \
    $$PWD/fqfdemuxthread.cpp \
    $$PWD/fqfresample.cpp \
    $$PWD/fqfvideothread.cpp \
    $$PWD/fqfscale.cpp \
    $$PWD/fqftools.cpp
