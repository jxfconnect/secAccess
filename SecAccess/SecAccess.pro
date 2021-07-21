QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    accessapproval.cpp \
    client.cpp \
    databasecomm.cpp \
    firewallrule.cpp \
    log.cpp \
    main.cpp \
    rootwin.cpp \
    secserver.cpp \
    socketserver.cpp

HEADERS += \
    accessapproval.h \
    client.h \
    databasecomm.h \
    firewallrule.h \
    log.h \
    rootwin.h \
    secserver.h \
    socketserver.h

FORMS += \
    accessapproval.ui \
    rootwin.ui \
    secserver.ui

TRANSLATIONS += \
    SecAccess_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

DISTFILES += \
    mysql.ini
