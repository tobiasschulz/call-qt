#-------------------------------------------------
#
# Project created by QtCreator 2013-08-14T00:53:06
#
#-------------------------------------------------

QT       += core gui multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = voipcall-qt
TEMPLATE = app


SOURCES += src/main.cpp \
    src/maingui.cpp \
    src/server.cpp \
    src/serverconnection.cpp \
    src/serverthread.cpp \
    src/statusconnection.cpp \
    src/contact.cpp \
    src/connection.cpp \
    src/contactmodel.cpp \
    src/contactlist.cpp \
    src/contactscanner.cpp \
    src/config.cpp \
    src/util.cpp \
    src/log.cpp

HEADERS  += \
    src/maingui.h \
    src/server.h \
    src/serverconnection.h \
    src/serverthread.h \
    src/statusconnection.h \
    src/contact.h \
    src/connection.h \
    src/contactmodel.h \
    src/contactlist.h \
    src/contactscanner.h \
    src/config.h \
    src/util.h \
    src/log.h

FORMS    += \
    ui/maingui.ui
    
    
Release:DESTDIR = bin
Release:OBJECTS_DIR = build/release/obj
Release:MOC_DIR = build/release/.moc
Release:RCC_DIR = build/release/.rcc
Release:UI_DIR = build/release/.ui

Debug:DESTDIR = bin
Debug:OBJECTS_DIR = build/debug/.obj
Debug:MOC_DIR = build/debug/.moc
Debug:RCC_DIR = build/debug/.rcc
Debug:UI_DIR = build/debug/.ui

