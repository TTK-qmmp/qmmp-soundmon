include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/soundmon

HEADERS += decodersoundmonfactory.h \
           decoder_soundmon.h \
           soundmonhelper.h
    
SOURCES += decodersoundmonfactory.cpp \
           decoder_soundmon.cpp \
           soundmonhelper.cpp \
           libsoundmon/channel.cpp \
           libsoundmon/instrument.cpp \
           libsoundmon/module.cpp \
           libsoundmon/player.cpp \
           libsoundmon/soundplayer.cpp

INCLUDEPATH += $$PWD/libsoundmon

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libsoundmon.so
}
