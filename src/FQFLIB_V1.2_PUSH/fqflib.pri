INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += concurrent

win32 {
INCLUDEPATH += D:/FFmpeg/ffmpeg-3.4.2/include
LIBS += -LD:/FFmpeg/ffmpeg-3.4.2/lib/win32 -lavdevice -lavformat -lavutil -lavcodec -lswscale
}

linux {
INCLUDEPATH += /opt/FFmpeg/include
LIBS += -L/opt/FFmpeg/lib -lavdevice -lavformat -lavutil -lavcodec -lswscale
}

HEADERS += \
    $$PWD/FQFCore/FQF.h \
    $$PWD/FQFCore/FQFMedia.h \
    $$PWD/FQFCore/FQFMediaPush.h \
    $$PWD/FQFCore/FQFNetworkMonitorThread.h \
    $$PWD/FQFPush/FQFPush.h

SOURCES += \
    $$PWD/FQFCore/FQF.cpp \
    $$PWD/FQFCore/FQFMedia.cpp \
    $$PWD/FQFCore/FQFMediaPush.cpp \
    $$PWD/FQFCore/FQFNetworkMonitorThread.cpp \
    $$PWD/FQFPush/FQFPush.cpp
