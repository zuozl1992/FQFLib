INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

INCLUDEPATH += D:\FFmpeg\ffmpeg-3.4.2-win32\include
LIBS += -LD:\FFmpeg\ffmpeg-3.4.2-win32\lib -lavutil -lavformat -lavcodec -lswscale

QT += concurrent

HEADERS += \
    $$PWD/FQFCore/FQF.h \
    $$PWD/FQFCore/FQFMedia.h \
    $$PWD/FQFCore/FQFMediaPlayer.h \
	$$PWD/FQFCore/FQFNetworkMonitorThread.h \
    $$PWD/FQFWidgets/FQFMediaThumbnail.h \
    $$PWD/FQFWidgets/FQFMediaWidget.h \
    $$PWD/FQFWidgets/ThrmbnailWidget.h
    
SOURCES += \
    $$PWD/FQFCore/FQF.cpp \
    $$PWD/FQFCore/FQFMedia.cpp \
    $$PWD/FQFCore/FQFMediaPlayer.cpp \
	$$PWD/FQFCore/FQFNetworkMonitorThread.cpp \
    $$PWD/FQFWidgets/FQFMediaThumbnail.cpp \
    $$PWD/FQFWidgets/FQFMediaWidget.cpp \
    $$PWD/FQFWidgets/ThrmbnailWidget.cpp
    
    
