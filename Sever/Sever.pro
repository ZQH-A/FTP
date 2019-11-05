TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        json/json_reader.cpp \
        json/json_value.cpp \
        json/json_writer.cpp \
        main.cpp \
        monitor.cpp \
        mythread.cpp \
        transport.cpp

HEADERS += \
    err_exit.h \
    json/allocator.h \
    json/assertions.h \
    json/autolink.h \
    json/config.h \
    json/features.h \
    json/forwards.h \
    json/json.h \
    json/json_tool.h \
    json/json_valueiterator.inl \
    json/reader.h \
    json/value.h \
    json/version.h \
    json/writer.h \
    monitor.h \
    mythread.h \
    transport.h

DISTFILES += \
    json/CMakeLists.txt
LIBS +=-lpthread
