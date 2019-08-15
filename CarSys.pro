#-------------------------------------------------
#
# Project created by QtCreator 2019-03-30T23:05:18
#
#-------------------------------------------------

QT       += core gui sql webchannel websockets axcontainer
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CarSys
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    login.cpp \
    websockettransport.cpp \
    contojs.cpp \
    websocketclientwrapper.cpp \
    about.cpp \
    theme.cpp \
    carsmanager.cpp \
    driversmanager.cpp \
    adddriver.cpp \
    updatedriver.cpp \
    addcar.cpp \
    updatecar.cpp \
    mndialog.cpp

HEADERS += \
        mainwindow.h \
    login.h \
    websockettransport.h \
    contojs.h \
    websocketclientwrapper.h \
    about.h \
    theme.h \
    carsmanager.h \
    driversmanager.h \
    adddriver.h \
    updatedriver.h \
    addcar.h \
    updatecar.h \
    mndialog.h

FORMS += \
        mainwindow.ui \
    login.ui \
    about.ui \
    theme.ui \
    carsmanager.ui \
    driversmanager.ui \
    adddriver.ui \
    updatedriver.ui \
    addcar.ui \
    updatecar.ui \
    mndialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icores.qrc
