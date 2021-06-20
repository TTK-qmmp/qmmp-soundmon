include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/bp

HEADERS += decoderbpfactory.h \
           decoder_bp.h \
           bphelper.h
    
SOURCES += decoderbpfactory.cpp \
           decoder_bp.cpp \
           bphelper.cpp \
           libbp/channel.cpp \
           libbp/instrument.cpp \
           libbp/module.cpp \
           libbp/player.cpp \
           libbp/soundplayer.cpp

INCLUDEPATH += $$PWD/libbp

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libbp.so
}
