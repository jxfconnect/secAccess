#ifndef AUTHSTATUS_H
#define AUTHSTATUS_H

#include <QObject>
#include <QDateTime>

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

    void setAuthStatus(QByteArray bytes);
    bool isClientAuthoried();
    bool loadConfiguration();
    void setAuthKey(QString authKey);

signals:

};

#endif // AUTHSTATUS_H
