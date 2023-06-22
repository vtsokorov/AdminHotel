#-------------------------------------------------
#
# Project created by QtCreator 2017-04-08T12:25:20
#
#-------------------------------------------------

QT += core gui sql printsupport qml

CONFIG += c++14
CONFIG += zint

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CLIENT-HOTELS
TEMPLATE = app

INCLUDEPATH += $$PWD/limereport/$${QT_VERSION}/include
DEPENDPATH  += $$PWD/limereport/$${QT_VERSION}/include

build_pass:CONFIG(debug, debug|release) {
   contains(CONFIG, zint){
        LIBS += -L$$PWD/limereport/$${QT_VERSION}/debug -lQtZintd
   }
   DESTDIR += $$PWD/bin/debug
   LIBS += -L$$PWD/limereport/$${QT_VERSION}/debug -llimereportd
} else {
    contains(CONFIG,zint){
        LIBS += -L$$PWD/limereport/$${QT_VERSION}/release -lQtZint
  }
  DESTDIR += $$PWD/bin/release
  LIBS += -L$$PWD/limereport/$${QT_VERSION}/release -llimereport
}

CODECFORTR = UTF-8

SOURCES += main.cpp\
        mainwindow.cpp \
    logindialog.cpp \
    applicationmodel.cpp \
    hotelseditdialog.cpp \
    numberedstockeditdialog.cpp \
    registrationeditdialog.cpp \
    paymentlisteditdialog.cpp \
    referenceeditdialog.cpp \
    tablemodel.cpp \
    printerobject.cpp \
    generalreportmodel.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    logindialog.h \
    querys.h \
    global.h \
    reports.h \
    applicationmodel.h \
    hotelseditdialog.h \
    numberedstockeditdialog.h \
    registrationeditdialog.h \
    paymentlisteditdialog.h \
    referenceeditdialog.h \
    tablemodel.h \
    printerobject.h \
    generalreportmodel.h \
    aboutdialog.h

FORMS    += mainwindow.ui \
    logindialog.ui \
    hotelseditdialog.ui \
    numberedstockeditdialog.ui \
    registrationeditdialog.ui \
    paymentlisteditdialog.ui \
    referenceeditdialog.ui \
    aboutdialog.ui

RESOURCES += \
    img.qrc

RC_FILE = app.rc

OTHER_FILES += \
    app.rc
