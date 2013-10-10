#-------------------------------------------------
#
# Project created by QtCreator 2013-08-14T00:53:06
#
#-------------------------------------------------

QT       += core gui widgets multimedia network

TARGET = call-qt
TEMPLATE = app

SOURCES += \
	src/audiodevices.cpp \
	src/audioinfo.cpp \
	src/call.cpp \
	src/chatclient.cpp \
    src/chattab.cpp \
    src/config.cpp \
    src/connection.cpp \
    src/contact.cpp \
    src/contactlist.cpp \
    src/contactmodel.cpp \
    src/contactscanner.cpp \
    src/dnscache.cpp \
    src/id.cpp \
    src/list-abstract.cpp \
    src/list-contacts.cpp \
    src/list-hosts.cpp \
    src/list-unknownhosts.cpp \
	src/main.cpp \
    src/maingui.cpp \
	src/model-abstract.cpp \
	src/model-contacts.cpp \
	src/model-unknownhosts.cpp \
    src/moviedelegate.cpp \
    src/networkutil.cpp \
    src/pingclient.cpp \
    src/serverrequest.cpp \
    src/server.cpp \
    src/systemutil.cpp \
    src/tab.cpp \
    src/tabs.cpp \
    src/terminal.cpp \
    src/thread.cpp \
    src/version.cpp

HEADERS  += \
	src/audiodevices.h \
	src/audioinfo.h \
    src/call.h \
    src/chatclient.h \
    src/chattab.h \
    src/config.h \
    src/connection.h \
    src/contact.h \
    src/contactlist.h \
    src/contactmodel.h \
    src/contactscanner.h \
    src/dnscache.h \
    src/id.h \
    src/list-abstract.h \
    src/list-contacts.h \
    src/list-hosts.h \
    src/list-unknownhosts.h \
	src/maingui.h \
	src/model-abstract.h \
	src/model-contacts.h \
	src/model-unknownhosts.h \
    src/moviedelegate.h \
    src/networkutil.h \
    src/pingclient.h \
    src/serverrequest.h \
    src/server.h \
    src/systemutil.h \
    src/tab.h \
    src/tabs.h \
    src/terminal.h \
    src/thread.h

FORMS    += \
    ui/maingui.ui \
    ui/terminal.ui \
    ui/chattab.ui \
    ui/audiodevices.ui

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

win32 {
	LIBS += -lws2_32
#	CONFIG += debug
	CONFIG += console
}

# The application version                                                          ## VERSION
VERSION = 0.925                                                                      ## VERSION
# Define the preprocessor macro to get the application version in our application. ## VERSION
DEFINES += APP_VERSION=\\\"$$VERSION\\\"                                           ## VERSION
DEFINES += APP_BUILD=\\\"925\\\"                                             ## VERSION
