QT       += core gui network webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += sdk_no_version_check

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += $$PWD/libs

SOURCES += \
    arealistform.cpp \
    danmuweight.cpp \
    libs/qrcodegen.cpp \
    main.cpp \
    mainwindow.cpp \
    recordfile.cpp

HEADERS += \
    arealistform.h \
    danmuweight.h \
    libs/qrcodegen.h \
    mainwindow.h \
    publiclib.h \
    recordfile.h

FORMS += \
    arealistform.ui \
    danmuweight.ui \
    mainwindow.ui

TRANSLATIONS += \
    client_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
