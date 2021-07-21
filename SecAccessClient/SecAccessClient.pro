QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    authstatus.cpp \
    main.cpp \
    rootwin.cpp \
    secclient.cpp \
    socketcomm.cpp

HEADERS += \
    authstatus.h \
    rootwin.h \
    secclient.h \
    socketcomm.h

FORMS += \
    rootwin.ui \
    secclient.ui

TRANSLATIONS += \
    SecAccessClient_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    img.qrc
