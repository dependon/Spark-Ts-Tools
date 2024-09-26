#-------------------------------------------------
#
# Project created by QtCreator 2018-04-04T10:31:33
#
#-------------------------------------------------

QT       += core gui xml
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Spark-Ts-Tools
TEMPLATE = app

include(./../../Path.pri)

INCLUDEPATH += ../../Libraries

win32 {
    LIBS += -L$${DESTDIR} -lLibXlsxRW
}

unix {
    LIBS += $$PWD/../../bin/libLibXlsxRW.a
}


SOURCES += main.cpp\
        MainWindow.cpp \
    XmlRW.cpp \
    ExcelRW.cpp \
    DataModel/TranslateModel.cpp \
    NetWorker.cpp \
    TranslateWorker.cpp \
    appobject.cpp

HEADERS  += MainWindow.h \
    XmlRW.h \
    ExcelRW.h \
    DataModel/TranslateModel.h \
    NetWorker.h \
    TranslateWorker.h \
    appobject.h

FORMS    += MainWindow.ui



