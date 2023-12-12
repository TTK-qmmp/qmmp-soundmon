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

#CONFIG += BUILD_PLUGIN_INSIDE
contains(CONFIG, BUILD_PLUGIN_INSIDE){
    include($$PWD/../../plugins.pri)
    TARGET = $$PLUGINS_PREFIX/Input/soundmon

    unix{
        target.path = $$PLUGIN_DIR/Input
        INSTALLS += target
    }
}else{
    CONFIG += warn_off plugin lib thread link_pkgconfig c++11
    TEMPLATE = lib

    unix{
        PKGCONFIG += qmmp-1

        PLUGIN_DIR = $$system(pkg-config qmmp-1 --variable=plugindir)/Input
        INCLUDEPATH += $$system(pkg-config qmmp-1 --variable=prefix)/include

        plugin.path = $${PLUGIN_DIR}
        plugin.files = lib$${TARGET}.so
        INSTALLS += plugin
    }
}
