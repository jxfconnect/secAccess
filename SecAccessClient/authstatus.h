#ifndef AUTHSTATUS_H
#define AUTHSTATUS_H

#include "wmiccommand.h"
#include "windowinfo.h"

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QJsonObject>
#include <QJsonDocument>

class AuthStatus : public QObject
{
    Q_OBJECT
public:
    explicit AuthStatus(QObject *parent = nullptr);

    bool isAuthorized;
    QDateTime lastValidTime;
    QString authKey;
    QString serverName;
    int port;
    QString configPath;
    QString devCode;
    WMICCommand *wmic;

    void setAuthStatus(QJsonObject message);
    bool isClientAuthoried();
    bool loadConfiguration();
    void setAuthKey(QString authKey);
    void genDeviceCode();
    bool isCryptClientRuning();
    bool isCryptLogin();

signals:

};

#endif // AUTHSTATUS_H
