QT       += core gui network

TARGET = server
TEMPLATE = app

DESTDIR = ../bin
OBJECTS_DIR = ./o
MOC_DIR = ./moc

SOURCES += main.cpp\
        mainwindow.cpp \
    pages.cpp \
    ftp.cpp \
    log.cpp \
    users.cpp

HEADERS  += mainwindow.h \
    pages.h \
    ftp.h \
    log.h \
    common.h \
    users.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
