#-------------------------------------------------
#
# Project created by QtCreator 2013-08-14T00:53:06
#
#-------------------------------------------------

QT       += core gui widgets multimedia network

TARGET = call-qt
TEMPLATE = app

SOURCES += \
	src/chatclient.cpp \
    src/chat.cpp \
    src/config.cpp \
    src/connection.cpp \
    src/contact.cpp \
    src/contactlist.cpp \
    src/contactmodel.cpp \
    src/contactscanner.cpp \
    src/dnscache.cpp \
    src/id.cpp \
	src/main.cpp \
    src/maingui.cpp \
    src/networkutil.cpp \
    src/pingclient.cpp \
    src/serverrequest.cpp \
    src/server.cpp \
    src/systemutil.cpp \
    src/tab.cpp \
    src/terminal.cpp \
    src/thread.cpp \
    src/version.cpp

HEADERS  += \
    src/chatclient.h \
    src/chat.h \
    src/config.h \
    src/connection.h \
    src/contact.h \
    src/contactlist.h \
    src/contactmodel.h \
    src/contactscanner.h \
    src/dnscache.h \
    src/id.h \
	src/maingui.h \
    src/networkutil.h \
    src/pingclient.h \
    src/serverrequest.h \
    src/server.h \
    src/systemutil.h \
    src/tab.h \
    src/terminal.h \
    src/thread.h

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
VERSION = 0.286                                                                      ## VERSION
# Define the preprocessor macro to get the application version in our application. ## VERSION
DEFINES += APP_VERSION=\\\"$$VERSION\\\"                                           ## VERSION
DEFINES += APP_BUILD=\\\"286\\\"                                             ## VERSION
