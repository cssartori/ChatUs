#-------------------------------------------------
#
# Project created by QtCreator 2016-08-08T10:29:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app
CONFIG += c++11

SOURCES += \
    ../protocol/src/protocol.cpp \
    ../protocol/src/message_t.cpp \
    src/client.cpp \
    src/clientmainwindow.cpp \
    src/infomessages.cpp \
    src/main.cpp \
    src/namedialog.cpp

HEADERS  += \
    ../protocol/include/protocol.h \
    ../protocol/include/message_t.h \
    ../protocol/include/constants.h \
    include/client.h \
    include/clientmainwindow.h \
    include/infomessages.h \
    include/namedialog.h \
    include/signaler.h

FORMS    += \
    clientmainwindow.ui \
    namedialog.ui
