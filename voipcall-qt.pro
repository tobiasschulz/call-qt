#-------------------------------------------------
#
# Project created by QtCreator 2013-08-14T00:53:06
#
#-------------------------------------------------

QT       += core gui widgets multimedia network

TARGET = call-qt
TEMPLATE = app

SOURCES += src/main.cpp \
    src/maingui.cpp \
    src/server.cpp \
    src/serverconnection.cpp \
    src/serverthread.cpp \
    src/pingclient.cpp \
    src/contact.cpp \
    src/connection.cpp \
    src/contactmodel.cpp \
    src/contactlist.cpp \
    src/contactscanner.cpp \
    src/config.cpp \
    src/networkutil.cpp \
    src/systemutil.cpp \
    src/log.cpp \
    src/tab.cpp \
    src/terminal.cpp \
    src/chat.cpp \
    src/version.cpp

HEADERS  += \
    src/maingui.h \
    src/server.h \
    src/serverconnection.h \
    src/serverthread.h \
    src/pingclient.h \
    src/contact.h \
    src/connection.h \
    src/contactmodel.h \
    src/contactlist.h \
    src/contactscanner.h \
    src/config.h \
    src/networkutil.h \
    src/systemutil.h \
    src/log.h \
    src/tab.h \
    src/terminal.h \
    src/chat.h

FORMS    += \
    ui/maingui.ui \
    ui/terminal.ui \
    ui/chat.ui

RC_ICONS = img/icon.ico


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

# The application version                                                          ## VERSION
VERSION = 0.16                                                                      ## VERSION
# Define the preprocessor macro to get the application version in our application. ## VERSION
DEFINES += APP_VERSION=\\\"$$VERSION\\\"                                           ## VERSION
